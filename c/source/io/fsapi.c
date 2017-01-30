#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "platform.h"
#include "fsapi.h"
#include "pbl.h"
#include "kstat.h"
#include "kprocess.h"
#include "kerror.h"
#include "kalloc.h"
#include "khashmap.h"
#include "kstring.h"

#include UTIME
#include MKDIR_INCLUDE
#include UNISTD

char pathSeperator;

char rootFileSystem[MAX_FILEPATH_LEN];

static char tmpPath[MAX_FILEPATH_LEN];
static char tmpLocalPath[MAX_FILEPATH_LEN];
static char tmpNativePath[MAX_FILEPATH_LEN];
static char pathTmp[MAX_FILEPATH_LEN];

#define nativePath1 reserved1
#define isLink1 reserved2
#define openNodes1 reserved3
#define cachedPosDuringDelete1 reserved4
#define handle1 reserved5
#define nextOpen1 reserved6
#define next1 reserved7
#define linkedNoded1 reserved8
#define hardLink1 reserved9
#define openfunc1 reserved10
#define mode1 reserved11

static int nodeId = 1;
static struct KHashmap nodeMap;
static PblMap* localSkipLinksMap;

void initNodes() {
    initHashmap(&nodeMap);
}

const char* pathMakeWindowsHappy(const char* path) {
    if (path[strlen(path)-1]==pathSeperator) {
        safe_strcpy(pathTmp, path, MAX_FILEPATH_LEN);
        pathTmp[strlen(path)-1]=0;
        return pathTmp;
    }
    return path;
}

BOOL doesPathExist(const char* path) {
    path = pathMakeWindowsHappy(path);
    if (access(path, 0)!=-1) {
        return TRUE;
    }
    return FALSE;
}

BOOL initFileSystem(const char* rootPath) {
    int len = strlen(rootPath);

    pathSeperator = '/';
    if (rootPath[len-1]=='/')
        pathSeperator = '/';
    else if (rootPath[len-1]=='\\')
        pathSeperator = '\\';
    else {
        if (strchr(rootPath, '\\'))
            pathSeperator = '\\';
    }
    
    if (!doesPathExist(rootPath))
        return FALSE;

    safe_strcpy(rootFileSystem, rootPath, MAX_FILEPATH_LEN);

    if (rootFileSystem[len-1]==pathSeperator)
        rootFileSystem[len-1] = 0;
    initNodes();
    localSkipLinksMap = pblMapNewHashMap();
    {
        struct FsNode* dir = getNodeFromLocalPath("", "/tmp/del", TRUE);
        if (dir) {
            struct FsNode* result[100];
            int count = listNodes(dir, result, 100);
            int i;
            for (i=0;i<count;i++) {
                remove(result[i]->nativePath1);
            }
        }
    }
    return TRUE;
}

// home/. dir should return the same node as /home/ dirs which should be the same as /home, so remove trailing . and /
BOOL normalizePath(char* path) {
    int len = strlen(path);
    int i;
    int lastDir = -1;
    int lastDir2 = -1;

    for (i=0;i<len;i++) {
        if (path[i]=='.') {
            if (i+1<len && path[i+1]=='.') {
                if (lastDir2>=0) {
                    memmove(path+lastDir2, path+i+2, len-i-1);					
                    return normalizePath(path);
                }
                return FALSE;
            } else if (i==len-1 && lastDir==i-1) {
                path[i]=0;
                len--;
            }
        } else if (path[i]=='/') {
            lastDir2 = lastDir;
            lastDir = i;
        }
    }
    if (path[len-1]=='/') {
        path[len-1]=0;
    }
    return TRUE;
}

static const char* invalidPaths[] = {
    "CON",
    "PRN",
    "AUX",
    "NUL",    
    "COM1",
    "COM2",
    "COM3",
    "COM4",
    "COM5",
    "COM6",
    "COM7",
    "COM8",
    "COM9",
    "LPT1",
    "LPT2",
    "LPT3",
    "LPT4",
    "LPT5",
    "LPT6",
    "LPT7",
    "LPT8",
    "LPT9",
    "con",
    "prn",
    "aux",
    "nul",    
    "com1",
    "com2",
    "com3",
    "com4",
    "com5",
    "com6",
    "com7",
    "com8",
    "com9",
    "lpt1",
    "lpt2",
    "lpt3",
    "lpt4",
    "lpt5",
    "lpt6",
    "lpt7",
    "lpt8",
    "lpt9",
};

void localPathToRemote(char* path) {
    int len = strlen(path);
    int i;

    for (i=0;i<len;i++) {
        if (path[i]=='/')
            path[i]=pathSeperator;
        else if (path[i]==':') {
            memmove(path+i+9, path+i+1, strlen(path+i+1)+1);
            memcpy(path+i, "(_colon_)", 9);
            i+=8;
            len+=8;
        }
    }
    for (i=0;i<sizeof(invalidPaths)/sizeof(invalidPaths[0]);i++) {
        const char* sub = strstr(path, invalidPaths[i]);
        if (sub) {
            int pos = sub-path;
            int len = strlen(invalidPaths[i]);

            if (path[pos-1]==pathSeperator && (path[pos+len]=='.' || path[pos+len]==pathSeperator || path[pos+len]==0)) {
                memmove(path+pos+len+4, path+pos+len, strlen(path+pos+len)+1);
                memcpy(path+pos+2, invalidPaths[i], len);
                path[pos]='(';
                path[pos+1]='_';
                path[pos+2+len]='_';
                path[pos+2+len+1]=')';
            }
        }
    }
}

void remotePathToLocal(char* path) {
    int len = strlen(path);
    int i;
    int rootlen = strlen(rootFileSystem);

    if (strncmp(path, rootFileSystem, rootlen)==0) {
        memmove(path, path+rootlen, strlen(path)-rootlen+1); 
    }
    for (i=0;i<len;i++) {
        if (path[i]==pathSeperator)
            path[i]='/';		
    }
    // :TODO: is reversing the colon code necessary
}

U32 symlinkInDirectory(struct KThread* thread, const char* currentDirectory, U32 path1, U32 path2) {
    char* s1 = getNativeString(MMU_PARAM_THREAD path1);
    char* s2 = getNativeString2(MMU_PARAM_THREAD path2);
    char tmp[MAX_FILEPATH_LEN];
    struct FsNode* node;
    struct FsOpenNode* openNode;

    node = getNodeFromLocalPath(currentDirectory, s2, TRUE);
    if (node && node->func->exists(node)) {
        return -K_EEXIST;
    }
    safe_strcpy(tmp, s2, MAX_FILEPATH_LEN);
    safe_strcat(tmp, ".link", MAX_FILEPATH_LEN);
    node = getNodeFromLocalPath(currentDirectory, tmp, FALSE);
    if (!node || node->func->exists(node)) {
        // :TODO: figure out real problem and remove this hack for libpango1.0-common_1.28.3-1+squeeze2_all.deb
        if (!strcmp(tmp, "/var/lib/defoma/locked.link")) {
            node->func->remove(node);
            node = getNodeFromLocalPath(currentDirectory, tmp, FALSE);
        } else {
            return -K_EEXIST;
        }
    }
    if (!node->func->canWrite(thread->process, node)) {
        return -K_EACCES;
    }
    openNode = node->func->open(thread->process, node, K_O_WRONLY|K_O_CREAT);
    if (!openNode) {
        return -K_EIO;
    }
    openNode->func->write(MMU_PARAM_THREAD openNode, path1, strlen(s1));
    openNode->func->close(openNode);
    pblMapClear(localSkipLinksMap);
    return 0;
}

BOOL kreadLink(const char* path, char* buffer, int bufferSize, BOOL makeAbsolute) {
    int h;
    char tmp[MAX_FILEPATH_LEN+1];
    int readCount;

    h = open(path, O_RDONLY);
    if (h<=0)
        return FALSE;
    readCount = read(h, tmp, MAX_FILEPATH_LEN+1);
    if (readCount<=0 || readCount>=MAX_FILEPATH_LEN) {
        close(h);
        return FALSE;
    }
    tmp[readCount]=0;
    if (makeAbsolute && tmp[0]!='/') {
        int len = strrchr(path, pathSeperator)-path;
        memcpy(buffer, path, len);
        buffer[len+1]=0;
        safe_strcat(buffer, tmp, bufferSize);
        remotePathToLocal(buffer);
    } else {
        safe_strcpy(buffer, tmp, bufferSize);
    }
    close(h);
    return TRUE;
}

BOOL followLinks(char* path, int pathSize, U32* isLink) {
    int len;
    int i;
    char tmp[MAX_FILEPATH_LEN];

    if (doesPathExist(path))
        return FALSE;
    safe_strcpy(tmp, path, MAX_FILEPATH_LEN);
    safe_strcat(tmp, ".link", MAX_FILEPATH_LEN);
    if (doesPathExist(tmp)) {
        if (kreadLink(tmp, tmp, MAX_FILEPATH_LEN, TRUE)) {
            safe_strcpy(path, tmp, pathSize);
            if (isLink)
                *isLink = 1;
            return TRUE;
        }
    }

    len = strlen(path);
    for (i=0;i<len;i++) {
        if (path[i]==pathSeperator) {
            safe_strcpy(tmp, path, MAX_FILEPATH_LEN); // don't include path seperator
            tmp[i]=0;
            if (!doesPathExist(tmp)) {
                safe_strcat(tmp, ".link", MAX_FILEPATH_LEN);
                if (doesPathExist(tmp)) {
                    if (kreadLink(tmp, tmp, MAX_FILEPATH_LEN, TRUE)) {
                        safe_strcat(tmp, path+i, MAX_FILEPATH_LEN); 
                        safe_strcpy(path, tmp, pathSize);						
                        return TRUE;
                    }
                }
            }
        }
    }
    return FALSE;
}

struct FsNode* getNodeInCache(const char* localPath) {
    return (struct FsNode*)getHashmapValue(&nodeMap, localPath);
}


struct LocalPath {
    char nativePath[MAX_FILEPATH_LEN];
    char localPath[MAX_FILEPATH_LEN];
    U32 isLink;
};

struct FsNode* getLocalAndNativePaths(const char* currentDirectory, const char* path, char* localPath, int localPathSize, char* nativePath, int nativePathSize, U32* isLink) {
    U32 tmp32=0;
    struct LocalPath localEntry;
    struct LocalPath* found;

    if (path[0]=='/')
        safe_strcpy(localPath, path, localPathSize);
    else {
        safe_strcpy(localPath, currentDirectory, localPathSize);
        safe_strcat(localPath, "/", localPathSize);
        safe_strcat(localPath, path, localPathSize);
    }	
    trimTrailingSpaces(localPath);

    found = pblMapGet(localSkipLinksMap, localPath, strlen(localPath)+1, NULL);
    if (found) {
        if (isLink)
            *isLink = found->isLink;
        safe_strcpy(localPath, found->localPath, localPathSize);
        safe_strcpy(nativePath, found->nativePath, nativePathSize);
    } else {
        char fullLocalPath[MAX_FILEPATH_LEN];
        safe_strcpy(fullLocalPath, localPath, MAX_FILEPATH_LEN);	    

        normalizePath(localPath);	
        safe_strcpy(nativePath, rootFileSystem, nativePathSize);	
        safe_strcat(nativePath, localPath, nativePathSize);
        while (TRUE) {
            localPathToRemote(nativePath+strlen(rootFileSystem)); // don't convert colon's in the root path			
            if (followLinks(nativePath, nativePathSize, &tmp32)) {
                normalizePath(nativePath);
                memmove(nativePath+strlen(rootFileSystem), nativePath, strlen(nativePath)+1);
                memcpy(nativePath, rootFileSystem, strlen(rootFileSystem));
            } else {
                break;
            }
        }
        if (isLink)
            *isLink = tmp32;
        safe_strcpy(localEntry.nativePath, nativePath, MAX_FILEPATH_LEN);
        safe_strcpy(localEntry.localPath, localPath, MAX_FILEPATH_LEN);
        localEntry.isLink = tmp32;
        pblMapAdd(localSkipLinksMap, fullLocalPath, strlen(fullLocalPath)+1, &localEntry, sizeof(struct LocalPath));
    }
    return getNodeInCache(localPath);
}

void removeOpenNodeFromNode(struct FsOpenNode* node) {
    struct FsNode* actualNode = node->linkedNoded1?node->linkedNoded1:node->node;
    struct FsOpenNode* prev = actualNode->openNodes1;
    while (prev) {
        // first one in list
        if (prev==node) {
            actualNode->openNodes1 = node->nextOpen1;
            break;
        } else if (prev->nextOpen1 == node) {
            prev->nextOpen1 = prev->nextOpen1->nextOpen1;
            break;
        }
        prev = prev->nextOpen1;
    }
    if (!prev) {
        kpanic("Could not find open node in list of open nodes.  This is a logic error");
    }
}

struct FsOpenNode* freeOpenNodes;
void freeOpenNode(struct FsOpenNode* node) {
    node->next1 = freeOpenNodes;
    freeOpenNodes = node;
}

struct FsOpenNode* allocOpenNode(struct KProcess* process, struct FsNode* node, U32 handle, U32 flags, struct FsOpenNodeFunc* func) {
    struct FsOpenNode* result;

    if (freeOpenNodes) {
        result = freeOpenNodes;
        freeOpenNodes = result->next1;
        memset(result, 0, sizeof(struct FsOpenNode));
    } else {
        result = (struct FsOpenNode*)kalloc(sizeof(struct FsOpenNode), KALLOC_OPENNODE);
    }	
    result->handle1 = handle;
    result->flags = flags;
    result->func = func;
    result->node = node;
    if (func->init(process, result)) {
        struct FsNode* actualNode = node;

        if (node->isLink1) {
            static char tmp[MAX_FILEPATH_LEN];
            safe_strcpy(tmp, node->nativePath1, MAX_FILEPATH_LEN);
            remotePathToLocal(tmp);
            result->linkedNoded1 = getNodeFromLocalPath("", tmp, TRUE);
            actualNode = result->linkedNoded1;
        }
        result->nextOpen1 = actualNode->openNodes1;
        actualNode->openNodes1 = result;
        return result;
    }
    freeOpenNode(result);
    return 0;
}

static S64 openfile_length(struct FsOpenNode* node) {
    S32 currentPos = lseek(node->handle1, 0, SEEK_CUR);
    S32 size = lseek(node->handle1, 00, SEEK_END);
    lseek(node->handle1, currentPos, SEEK_SET);
    return size;
}

static BOOL openfile_setLength(struct FsOpenNode* node, S64 len) {
    return ftruncate(node->handle1, (U32)len)==0;
}

static S64 openfile_getFilePointer(struct FsOpenNode* node) {
    return lseek(node->handle1, 0, SEEK_CUR);
}

static S64 openfile_seek(struct FsOpenNode* node, S64 pos) {
    return lseek(node->handle1, (U32)pos, SEEK_SET);
}

#ifdef USE_MMU
char tmp[4096];
#endif

static U32 openfile_read(MMU_ARG struct FsOpenNode* node, U32 address, U32 len) {
#ifndef USE_MMU
    return read(node->handle, (void*)address, len);
#else
    if (PAGE_SIZE-(address & (PAGE_SIZE-1)) >= len) {
        U8* ram = getPhysicalAddress(memory, address);
        U32 result;
        S64 pos = openfile_getFilePointer(node);

        if (ram) {
            result = read(node->handle1, ram, len);	
        } else {
            result = read(node->handle1, tmp, len);
            memcopyFromNative(MMU_PARAM address, tmp, result);
        }        
        // :TODO: why does this happen
        //
        // installing dpkg with dpkg
        // 1) dpkg will be mapped into memory (kfmmap on demand)
        // 2) dpkg will remove dpkg, this triggers the logic to close the handle move the file to /tmp then re-open it
        // 3) dpkg continues to run then hits a new part of the code that causes an on demand load
        // 4) on windows 7 x64 this resulted in a full read of one page, but the result returned by read was less that 4096, it was 0xac8 (2760)
        if (result<len) {
            if (openfile_getFilePointer(node)==pos+len) {
                result = len;
            }
        }
        return result;
    } else {		
        U32 result = 0;
        while (len) {
            U32 todo = PAGE_SIZE-(address & (PAGE_SIZE-1));
            S32 didRead;
            U8* ram = getPhysicalAddress(memory, address);

            if (todo>len)
                todo = len;
            if (ram) {
                didRead=read(node->handle1, ram, todo);		
            } else {
                didRead = read(node->handle1, tmp, todo);
                memcopyFromNative(MMU_PARAM address, tmp, didRead);
            }
            if (didRead<=0)
                break;
            len-=didRead;
            address+=didRead;
            result+=didRead;
        }
        return result;
    }
#endif
}

static U32 openfile_write(MMU_ARG struct FsOpenNode* node, U32 address, U32 len) {
#ifndef USE_MMU
    return write(node->handle, (void*)address, len);
#else	
    U32 wrote = 0;
    while (len) {
        U32 todo = PAGE_SIZE-(address & (PAGE_SIZE-1));
        U8* ram = getPhysicalAddress(memory, address);
        if (todo>len)
            todo = len;
        if (ram)
            wrote+=write(node->handle1, ram, todo);		
        else {
            memcopyToNative(MMU_PARAM address, tmp64k, todo);
            wrote+=write(node->handle1, tmp64k, todo);		
        }
        len-=todo;
        address+=todo;
    }
    return wrote;
#endif
}

static void openfile_close(struct FsOpenNode* node) {
    removeOpenNodeFromNode(node);
    close(node->handle1);
    freeOpenNode(node);
}

static U32 openfile_ioctl(struct KThread* thread, struct FsOpenNode* node, U32 request) {
    return -K_ENODEV;
}

static void openfile_setAsync(struct FsOpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("file_setAsync not implemented");
}

static BOOL openfile_isAsync(struct FsOpenNode* node, struct KProcess* process) {
    return 0;
}

static void openfile_waitForEvents(struct FsOpenNode* node, struct KThread* thread, U32 events) {
    kwarn("file_waitForEvents not implemented");
}

static BOOL openfile_isWriteReady(struct FsOpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_RDONLY;
}

static BOOL openfile_isReadReady(struct FsOpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

static U32 openfile_map(MMU_ARG struct FsOpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

static BOOL openfile_canMap(struct FsOpenNode* node) {
    return TRUE;
}

static BOOL openfile_init(struct KProcess* process, struct FsOpenNode* node) {
    return TRUE;
}

void openfile_free(struct FsOpenNode* node) {
    freeOpenNode(node);
}

struct FsNode* openfile_getDirectoryEntry(struct FsOpenNode* node, U32 index) {
    kpanic("openfile_getDirectoryEntry not implemented");
    return 0;
}

U32 openfile_getDirectoryEntryCount(struct FsOpenNode* node) {
    kpanic("openfile_getDirectoryEntryCount not implemented");
    return 0;
}

U32 openfile_readNative(struct FsOpenNode* node, U8* buffer, U32 len) {
    return read(node->handle1, buffer, len);
}

U32 openfile_writeNative(struct FsOpenNode* node, U8* buffer, U32 len) {
    return write(node->handle1, buffer, len);
}

struct FsOpenNodeFunc fileAccess = {openfile_init, openfile_length, openfile_setLength, openfile_getFilePointer, openfile_seek, openfile_read, openfile_write, openfile_close, openfile_map, openfile_canMap, openfile_ioctl, openfile_setAsync, openfile_isAsync, openfile_waitForEvents, openfile_isWriteReady, openfile_isReadReady, openfile_free, openfile_getDirectoryEntry, openfile_getDirectoryEntryCount, openfile_readNative, openfile_writeNative};

struct DirData {
    S32 pos;
    struct FsNode* nodes[MAX_DIR_LISTING];
    U32 count;
    struct DirData* next;
};

struct DirData* freeDirDatas;
void freeDirData(struct DirData* data) {
    data->next = freeDirDatas;
    freeDirDatas = data;
}

struct DirData* allocDirData() {
    struct DirData* result;

    if (freeDirDatas) {
        result = freeDirDatas;
        freeDirDatas = result->next;
        memset(result, 0, sizeof(struct DirData));
    } else {
        result = (struct DirData*)kalloc(sizeof(struct DirData), KALLOC_DIRDATA);
    }	
    return result;
}

struct DirData* getDirData(struct FsOpenNode* node) {
    struct DirData* data = (struct DirData*)node->data;
    if (!data) {
        data = allocDirData();
        data->count = listNodes(node->node, data->nodes, MAX_DIR_LISTING);
        node->data = data;
    }
    return data;
}

S64 dir_length(struct FsOpenNode* node) {	
    struct DirData* data = getDirData(node);
    return data->count;
}

BOOL dir_setLength(struct FsOpenNode* node, S64 len) {
    return FALSE;
}

S64 dir_getFilePointer(struct FsOpenNode* node) {
    struct DirData* data = getDirData(node);
    return data->pos;
}

S64 dir_seek(struct FsOpenNode* node, S64 pos) {
    struct DirData* data = getDirData(node);
    if (pos>=0 && pos<=data->count)
        data->pos = (S32)pos;
    else
        data->pos = data->count;
    return data->pos;
}

U32 dir_read(MMU_ARG struct FsOpenNode* node, U32 address, U32 len) {
    return 0;
}

U32 dir_write(MMU_ARG struct FsOpenNode* node, U32 address, U32 len) {
    return 0;
}

void dir_close(struct FsOpenNode* node) {
    struct DirData* data = (struct DirData*)node->data;
    if (data) {
        freeDirData(data);
    }
    removeOpenNodeFromNode(node);
    freeOpenNode(node);
}

U32 dir_ioctl(struct KThread* thread, struct FsOpenNode* node, U32 request) {
    return -K_ENODEV;
}

void dir_setAsync(struct FsOpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("dir_setAsync not implemented");
}

BOOL dir_isAsync(struct FsOpenNode* node, struct KProcess* process) {
    return 0;
}

void dir_waitForEvents(struct FsOpenNode* node, struct KThread* thread, U32 events) {
    kwarn("dir_waitForEvents not implemented");
}

BOOL dir_isWriteReady(struct FsOpenNode* node) {
    return FALSE;
}

BOOL dir_isReadReady(struct FsOpenNode* node) {
    return FALSE;
}

U32 dir_map(MMU_ARG struct FsOpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

BOOL dir_canMap(struct FsOpenNode* node) {
    return FALSE;
}

BOOL dir_init(struct KProcess* process, struct FsOpenNode* node) {
    node->data = 0;
    return TRUE;
}

void dir_free(struct FsOpenNode* node) {
    freeOpenNode(node);
}

struct FsNode* dir_getDirectoryEntry(struct FsOpenNode* node, U32 index) {
    struct DirData* data = getDirData(node);
    if (index<data->count)
        return data->nodes[index];
    return 0;
}

U32 dir_getDirectoryEntryCount(struct FsOpenNode* node) {
    struct DirData* data = getDirData(node);
    return data->count;
}

U32 dir_readNative(struct FsOpenNode* node, U8* buffer, U32 len) {
    kpanic("dir_readNative not implemented: %s", node->node->path);
    return 0;
}

U32 dir_writeNative(struct FsOpenNode* node, U8* buffer, U32 len) {
    kpanic("dir_writeNative not implemented: %s", node->node->path);
    return 0;
}

struct FsOpenNodeFunc dirAccess = {dir_init, dir_length, dir_setLength, dir_getFilePointer, dir_seek, dir_read, dir_write, dir_close, dir_map, dir_canMap, dir_ioctl, dir_setAsync, dir_isAsync, dir_waitForEvents, dir_isWriteReady, dir_isReadReady, dir_free, dir_getDirectoryEntry, dir_getDirectoryEntryCount, dir_readNative, dir_writeNative};

BOOL file_isDirectory(struct FsNode* node) {
    struct stat buf;
    const char* path = node->nativePath1;

    path = pathMakeWindowsHappy(path);
    if (stat(path, &buf)==0) {
        return S_ISDIR(buf.st_mode);
    }
    return FALSE;
}

BOOL file_remove(struct FsNode* node) {
    BOOL result;
    BOOL forcedClose = FALSE;

    if (node->isLink1) {
        safe_strcpy(tmpLocalPath, node->path, MAX_FILEPATH_LEN);
        safe_strcat(tmpLocalPath, ".link", MAX_FILEPATH_LEN);
        node = getNodeFromLocalPath("", tmpLocalPath, TRUE);
    }
    result = unlink(node->nativePath1)==0;
    if (!result && node->func->exists(node)) {
        struct FsOpenNode* openNode = node->openNodes1;
        int i;
        U32 isLink;

        while (openNode) {
            openNode->cachedPosDuringDelete1 = openNode->func->getFilePointer(openNode);
            close(openNode->handle1);
            openNode->handle1 = 0xFFFFFFFF;
            openNode = openNode->nextOpen1;
        }
        
        getLocalAndNativePaths("", "/tmp", tmpLocalPath, MAX_FILEPATH_LEN, tmpNativePath, MAX_FILEPATH_LEN, &isLink);
        MKDIR(tmpNativePath);
        getLocalAndNativePaths("", "/tmp/del", tmpLocalPath, MAX_FILEPATH_LEN, tmpNativePath, MAX_FILEPATH_LEN, &isLink);
        MKDIR(tmpNativePath);

        for (i=0;i<100000000;i++) {			
            sprintf(tmpPath, "/tmp/del/del%.8d.tmp", i);
            getLocalAndNativePaths("", tmpPath, tmpLocalPath, MAX_FILEPATH_LEN, tmpNativePath, MAX_FILEPATH_LEN, &isLink);
            if (!doesPathExist(tmpNativePath)) {
                break;
            }
        }
        if (rename(node->nativePath1, tmpNativePath)!=0) {
            kpanic("could not rename %s", node->nativePath1);
        }
        openNode = node->openNodes1;
        while (openNode) {
            int openFlags = 0;
            int flags = openNode->flags;
                        
            if ((flags & K_O_ACCMODE)==K_O_RDONLY) {
                openFlags|=O_RDONLY;
            } else if ((flags & K_O_ACCMODE)==K_O_WRONLY) {
                openFlags|=O_WRONLY;
            } else {
                openFlags|=O_RDWR;
            }
            if (flags & K_O_APPEND) {
                openFlags|=O_APPEND;
            }

            openNode->handle1 = open(tmpNativePath, openFlags, 0666);
            openNode->func->seek(openNode, openNode->cachedPosDuringDelete1);
            openNode = openNode->nextOpen1;
        }
        forcedClose = TRUE;
        result = TRUE;
    }
    return result;
}

U64 file_lastModified(struct FsNode* node) {
    struct stat buf;
    const char* path = node->nativePath1;

    path = pathMakeWindowsHappy(path);
    if (stat(path, &buf)==0) {
        return buf.st_mtime;
    }
    return 0;
}

U64 file_length(struct FsNode* node) {
    struct stat buf;
    const char* path = node->nativePath1;

    path = pathMakeWindowsHappy(path);
    if (stat(path, &buf)==0) {
        return buf.st_size;
    }
    return 0;
}

struct FsOpenNode* file_open(struct KProcess* process, struct FsNode* node, U32 flags) {
    U32 openFlags = O_BINARY;
    U32 f;

    if (node->func->isDirectory(node)) {
        return allocOpenNode(process, node, 0, flags, &dirAccess);
    }
    if ((flags & K_O_ACCMODE)==K_O_RDONLY) {
        openFlags|=O_RDONLY;
    } else if ((flags & K_O_ACCMODE)==K_O_WRONLY) {
        openFlags|=O_WRONLY;
    } else {
        openFlags|=O_RDWR;
    }
    if (flags & K_O_CREAT) {
        openFlags|=O_CREAT;
    }
    if (flags & K_O_EXCL) {
        openFlags|=O_EXCL;
    }
    if (flags & K_O_TRUNC) {
        openFlags|=O_TRUNC;
    }
    if (flags & K_O_APPEND) {
        openFlags|=O_APPEND;
    }
    f = open(node->nativePath1, openFlags, 0666);	
    if (!f || f==0xFFFFFFFF)
        return 0;
    return allocOpenNode(process, node, f, flags, &fileAccess);
}

BOOL file_setLastModifiedTime(struct FsNode* node, U32 time) {
    struct utimbuf buf = {time, time};
    return utime(node->nativePath1, &buf)==0;
}

U32 file_getType(struct FsNode* node, U32 checkForLink) {	
    if (file_isDirectory(node))
        return 4; // DT_DIR
    if (checkForLink && node->isLink1) 
        return 10; // DT_LNK
    return 8; // DT_REG
}

U32 file_getMode(struct KProcess* process, struct FsNode* node) {
    U32 result = K__S_IREAD | K__S_IEXEC | (file_getType(node, 0) << 12);
    if (process->userId == 0 || strstr(node->path, "/tmp")==node->path || strstr(node->path, "/var")==node->path || strstr(node->path, "/home")==node->path) {
        result|=K__S_IWRITE;
    }
    return result;
}

BOOL file_canRead(struct KProcess* process, struct FsNode* node) {
    return file_getMode(process, node) & K__S_IREAD;
}

BOOL file_canWrite(struct KProcess* process, struct FsNode* node) {
    return file_getMode(process, node) & K__S_IWRITE;
}

BOOL file_exists(struct FsNode* node) {
    return doesPathExist(node->nativePath1);
}

U32 file_rename(struct FsNode* oldNode, struct FsNode* newNode) {
    struct FsOpenNode* openNode = oldNode->openNodes1;
    U32 result;

    while (openNode) {
        openNode->cachedPosDuringDelete1 = openNode->func->getFilePointer(openNode);
        close(openNode->handle1);
        openNode->handle1 = 0xFFFFFFFF;
        openNode = openNode->nextOpen1;
    }

    result = rename(oldNode->nativePath1, newNode->nativePath1);

    openNode = oldNode->openNodes1;
    while (openNode) {
        int openFlags = 0;
        int flags = openNode->flags;
                        
        if ((flags & K_O_ACCMODE)==K_O_RDONLY) {
            openFlags|=O_RDONLY;
        } else if ((flags & K_O_ACCMODE)==K_O_WRONLY) {
            openFlags|=O_WRONLY;
        } else {
            openFlags|=O_RDWR;
        }
        if (flags & K_O_APPEND) {
            openFlags|=O_APPEND;
        }

        openNode->handle1 = open(newNode->nativePath1, openFlags, 0666);
        openNode->func->seek(openNode, openNode->cachedPosDuringDelete1);
        openNode = openNode->nextOpen1;
    }
    if (result!=0)
        result = -K_EIO;
    return result;
}

U32 file_removeDir(struct FsNode* node) {
    return rmdir(node->nativePath1);
}

U32 file_makeDir(struct FsNode* node) {
    return MKDIR(node->nativePath1);
}

BOOL isLink(struct FsNode* node) {
    return node->isLink1;
}

BOOL readLink(struct FsNode* node, char* buffer, int bufferSize, BOOL makeAbsolute) {
    return kreadLink(node->nativePath1, buffer, bufferSize, makeAbsolute);
}

U32 getHardLinkCount(struct FsNode* node) {
    if (node->hardLink1) 
        return 2; 
    return 1;
}

struct FsNode* getParentNode(struct FsNode* node) {
    static char tmp[MAX_FILEPATH_LEN];
    const char* pos = strrchr(node->path, '/');
    if (pos) {
        safe_strcpy(tmp, node->path, MAX_FILEPATH_LEN);
        tmp[pos-node->path]=0;
        return getNodeFromLocalPath("/", tmp, FALSE);
    }
    return 0;
}

struct FsNodeFunc fileNodeFunc = {file_isDirectory, file_exists, file_rename, file_remove, file_lastModified, file_length, file_open, file_setLastModifiedTime, file_canRead, file_canWrite, file_getType, file_getMode, file_removeDir, file_makeDir};

struct FsNode* allocNode(const char* localPath, const char* nativePath, struct FsNodeFunc* func, U32 rdev) {
    struct FsNode* result;

    U32 localLen = 0;
    U32 nativeLen = 0;
    if (localPath)
        localLen=strlen(localPath)+1;
    if (nativePath)
        nativeLen=strlen(nativePath)+1;
    result = (struct FsNode*)kalloc(sizeof(struct FsNode)+localLen+nativeLen, KALLOC_NODE);
    result->id = nodeId++;
    result->func = func;
    result->rdev = rdev;
    if (localPath) {
        result->path = (const char*)result+sizeof(struct FsNode);
        safe_strcpy((char*)result->path, localPath, localLen);
    } else {
        result->path = 0;
    }
    if (nativePath) {
        result->nativePath1 = (const char*)result+sizeof(struct FsNode)+localLen;
        safe_strcpy((char*)result->nativePath1, nativePath, nativeLen);
    } else {
        result->nativePath1 = 0;
    }
    result->name = strrchr(result->path, '/')+1;
    putHashmapValue(&nodeMap, result->path, result);	
    return result;
}

void removeNodeFromCache(struct FsNode* node) {
    removeHashmapKey(&nodeMap, node->path);
}

struct FsNode* getNodeFromLocalPath(const char* currentDirectory, const char* path, BOOL existing) {
    char localPath[MAX_FILEPATH_LEN];
    char nativePath[MAX_FILEPATH_LEN];
    U32 isLink = 0;
    struct FsNode* result = getLocalAndNativePaths(currentDirectory, path, localPath, MAX_FILEPATH_LEN, nativePath, MAX_FILEPATH_LEN, &isLink);
        
    if (result) {
        return result;		
    }
    
    if (existing && !doesPathExist(nativePath)) {
        return 0;
    }
    
    result = allocNode(localPath, nativePath, &fileNodeFunc, 1);	
    result->isLink1 = isLink;
    return result;
}

static int nextrdev = 100;

BOOL virtual_isDirectory(struct FsNode* node) {
    return FALSE;
}

BOOL virtual_remove(struct FsNode* node) {
    return 0;
}

U64 virtual_lastModified(struct FsNode* node) {
    return 0;
}

U64 virtual_length(struct FsNode* node) {
    return 0;
}

U32 virtual_getMode(struct KProcess* process, struct FsNode* node) {
    return node->mode1;
}

BOOL virtual_canRead(struct KProcess* process, struct FsNode* node) {
    return virtual_getMode(process, node) & K__S_IREAD;
}

BOOL virtual_canWrite(struct KProcess* process, struct FsNode* node) {
    return virtual_getMode(process, node) & K__S_IWRITE;
}

struct FsOpenNode* virtual_open(struct KProcess* process, struct FsNode* node, U32 flags) {
    if ((flags & K_O_ACCMODE)==K_O_RDONLY) {
        if (!virtual_canRead(process, node))
            return 0;
    } else if ((flags & K_O_ACCMODE)==K_O_WRONLY) {
        if (!virtual_canWrite(process, node))
            return 0;
    } else {
        if (!virtual_canWrite(process, node))
            return 0;
        if (!virtual_canRead(process, node))
            return 0;
    }
    if (flags & K_O_CREAT) {
        //return 0;
    }
    if (flags & K_O_EXCL) {
        kwarn("What about exclusive virtual files");
    }
    if (flags & K_O_TRUNC) {
        kwarn("What about truncating a virtual file");
    }
    if (flags & K_O_APPEND) {
        kwarn("What about appending a virtual file");
    }
    return allocOpenNode(process, node, -1, flags, node->openfunc1);
}

BOOL virtual_setLastModifiedTime(struct FsNode* node, U32 time) {
    return 0;
}

U32 virtual_getType(struct FsNode* node, U32 checkForLink) {
    U32 mode = node->mode1;

    if (virtual_isDirectory(node)) {
        return 4; // DT_DIR
    }
    if (mode & K__S_IFCHR) {
        return 2; // DT_CHR
    }
    if (mode & K_S_IFSOCK) {
        return 12; // DT_SOCK
    }
    return 8; // DT_REG
}

BOOL virtual_exists(struct FsNode* node) {
    return TRUE;
}

U32 virtual_rename(struct FsNode* oldNode, struct FsNode* newNode) {
    return -K_EIO;
}

U32 virtual_removeDir(struct FsNode* node) {
    kpanic("virtual_removeDir not implemented");
    return 0;
}

U32 virtual_makeDir(struct FsNode* node) {
    kpanic("virtual_makeDir not implemented");
    return 0;
}

struct FsNodeFunc virtualNodeType = {virtual_isDirectory, virtual_exists, virtual_rename, virtual_remove, virtual_lastModified, virtual_length, virtual_open, virtual_setLastModifiedTime, virtual_canRead, virtual_canWrite, virtual_getType, virtual_getMode, virtual_removeDir, virtual_makeDir};

void openvirtual_free(struct FsOpenNode* node) {
    freeOpenNode(node);
}

struct FsNode* addVirtualFile(const char* path, struct FsOpenNodeFunc* func, U32 mode) {
    struct FsNode* node = getNodeFromLocalPath(NULL, path, 0);
    node->func = &virtualNodeType;
    node->rdev = nextrdev++;
    node->openfunc1 = func;
    node->mode1 = mode;
    if (func->free == 0)
        func->free = openvirtual_free;
    return node;
}


U32 syscall_link(struct KThread* thread, U32 from, U32 to) {
    struct FsNode* fromNode = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(MMU_PARAM_THREAD from), TRUE);
    struct FsNode* toNode = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(MMU_PARAM_THREAD to), FALSE);
    struct FsOpenNode* fromOpenNode;
    struct FsOpenNode* toOpenNode;
    U8 buffer[PAGE_SIZE];

    if (!fromNode || !fromNode->func->exists(fromNode)) {
        return -K_ENOENT;
    }

    if (toNode->func->exists(toNode)) {
        return -K_EEXIST;
    }
    if (fromNode->func->isDirectory(fromNode)) {
        return -K_EPERM;
    }
    fromOpenNode = fromNode->func->open(thread->process, fromNode, K_O_RDONLY);
    if (!fromOpenNode)
        return -K_EIO;

    toOpenNode = toNode->func->open(thread->process, toNode, K_O_WRONLY|K_O_CREAT);
    if (!toOpenNode) {
        fromOpenNode->func->close(fromOpenNode);
        return -K_EIO;
    }

    while (1) {
        U32 r = fromOpenNode->func->readNative(fromOpenNode, buffer, PAGE_SIZE);	
        fromOpenNode->func->writeNative(toOpenNode, buffer, r);
        if (r<PAGE_SIZE)
            break;
    }
    toOpenNode->func->close(toOpenNode);
    fromOpenNode->func->close(fromOpenNode);
    if (toNode->hardLink1!=0 || fromNode->hardLink1!=0)
        kwarn("Hard link counting doesn't support more than 1 link");
    toNode->hardLink1 = fromNode;
    fromNode->hardLink1 = toNode;
    return 0;
}

U32 syscall_symlinkat(struct KThread* thread, U32 oldpath, FD dirfd, U32 newpath) {
    const char* currentDirectory;

    if (dirfd==-100) { // AT_FDCWD
        currentDirectory = thread->process->currentDirectory;
    } else {
        struct KFileDescriptor* fd = getFileDescriptor(thread->process, dirfd);
        if (!fd) {
            return -K_EBADF;
        } else if (fd->kobject->type!=KTYPE_FILE){
            return -K_ENOTDIR;
        } else {
            struct FsOpenNode* openNode = fd->kobject->openFile;
            currentDirectory = openNode->node->path;
            if (!openNode->node->func->isDirectory(openNode->node)) {
                return -K_ENOTDIR;
            }
        }
    }
    return symlinkInDirectory(thread, currentDirectory, oldpath, newpath);
}

U32 syscall_symlink(struct KThread* thread, U32 path1, U32 path2) {
    return symlinkInDirectory(thread, thread->process->currentDirectory, path1, path2);
}

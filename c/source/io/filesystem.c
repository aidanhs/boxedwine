#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "filesystem.h"
#include "khashmap.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "kstat.h"
#include "kerror.h"
#include "kalloc.h"
#include "log.h"
#include "kprocess.h"
#include "kfiledescriptor.h"
#include "ram.h"
#include "kstring.h"
#include "kscheduler.h"
#include "pbl.h"

#include UNISTD
#include UTIME
#include MKDIR_INCLUDE

static char root[MAX_FILEPATH_LEN];
static int nodeId = 1;
static struct KHashmap nodeMap;
static PblMap* localSkipLinksMap;

char pathSeperator = '/';

void initFileSystem(const char* rootPath) {
    int len = strlen(rootPath);
    if (rootPath[len-1]=='/')
        pathSeperator = '/';
    else if (rootPath[len-1]=='\\')
        pathSeperator = '\\';
    else {
        if (strchr(rootPath, '\\'))
            pathSeperator = '\\';
    }

    safe_strcpy(root, rootPath, MAX_FILEPATH_LEN);

    if (root[len-1]==pathSeperator)
        root[len-1] = 0;
    initHashmap(&nodeMap);
    localSkipLinksMap = pblMapNewHashMap();
    {
        struct Node* dir = getNodeFromLocalPath("", "/tmp/del", TRUE);
        if (dir) {
            struct Node* result[100];
            int count = listNodes(dir, result, 100);
            int i;
            for (i=0;i<count;i++) {
                remove(result[i]->path.nativePath);
            }
        }
    }
}

S64 openfile_length(struct OpenNode* node) {
    S32 currentPos = lseek(node->handle, 0, SEEK_CUR);
    S32 size = lseek(node->handle, 00, SEEK_END);
    lseek(node->handle, currentPos, SEEK_SET);
    return size;
}

BOOL file_setLength(struct OpenNode* node, S64 len) {
    return ftruncate(node->handle, (U32)len)==0;
}

S64 file_getFilePointer(struct OpenNode* node) {
    return lseek(node->handle, 0, SEEK_CUR);
}

S64 file_seek(struct OpenNode* node, S64 pos) {
    return lseek(node->handle, (U32)pos, SEEK_SET);
}

#ifdef USE_MMU
char tmp[4096];
#endif

U32 file_read(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
#ifndef USE_MMU
    return read(node->handle, (void*)address, len);
#else
    if (PAGE_SIZE-(address & (PAGE_SIZE-1)) >= len) {
        U8* ram = getPhysicalAddress(memory, address);
        U32 result;
        S64 pos = file_getFilePointer(node);

        if (!ram) {
            U32 i;

            read(node->handle, tmp, len);
            for (i=0;i<len;i++) {
                if (tmp[i]!=ram[i]) {
                    kpanic("ouch");
                }
            }
        }
        result = read(node->handle, ram, len);	
        // :TODO: why does this happen
        //
        // installing dpkg with dpkg
        // 1) dpkg will be mapped into memory (kfmmap on demand)
        // 2) dpkg will remove dpkg, this triggers the logic to close the handle move the file to /tmp then re-open it
        // 3) dpkg continues to run then hits a new part of the code that causes an on demand load
        // 4) on windows 7 x64 this resulted in a full read of one page, but the result returned by read was less that 4096, it was 0xac8 (2760)
        if (result<len) {
            if (file_getFilePointer(node)==pos+len) {
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
            didRead=read(node->handle, ram, todo);		
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

U32 file_write(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
#ifndef USE_MMU
    return write(node->handle, (void*)address, len);
#else
    if (PAGE_SIZE-(address & (PAGE_SIZE-1)) >= len) {
        U8* ram = getPhysicalAddress(memory, address);
        return write(node->handle, ram, len);		
    } else {		
        U32 wrote = 0;
        while (len) {
            U32 todo = PAGE_SIZE-(address & (PAGE_SIZE-1));
            U8* ram = getPhysicalAddress(memory, address);
            if (todo>len)
                todo = len;
            wrote+=write(node->handle, ram, todo);		
            len-=todo;
            address+=todo;
        }
        return wrote;
    }
#endif
}

void removeOpenNodeFromNode(struct OpenNode* node) {
    struct Node* actualNode = node->linkedNoded?node->linkedNoded:node->node;
    struct OpenNode* prev = actualNode->openNodes;
    while (prev) {
        // first one in list
        if (prev==node) {
            actualNode->openNodes = node->nextOpen;
            break;
        } else if (prev->nextOpen == node) {
            prev->nextOpen = prev->nextOpen->nextOpen;
            break;
        }
        prev = prev->nextOpen;
    }
    if (!prev) {
        kpanic("Could not find open node in list of open nodes.  This is a logic error");
    }
}

void file_close(struct OpenNode* node) {
    removeOpenNodeFromNode(node);
    close(node->handle);
    freeOpenNode(node);
}

U32 file_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    return -K_ENODEV;
}

void file_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("file_setAsync not implemented");
}

BOOL file_isAsync(struct OpenNode* node, struct KProcess* process) {
    return 0;
}

void file_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
    kwarn("file_waitForEvents not implemented");
}

BOOL file_isWriteReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_RDONLY;
}

BOOL file_isReadReady(struct OpenNode* node) {
    return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 file_map(MMU_ARG struct OpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

BOOL file_canMap(struct OpenNode* node) {
    return TRUE;
}

BOOL file_init(struct KProcess* process, struct OpenNode* node) {
    return TRUE;
}

struct NodeAccess fileAccess = {file_init, openfile_length, file_setLength, file_getFilePointer, file_seek, file_read, file_write, file_close, file_map, file_canMap, file_ioctl, file_setAsync, file_isAsync, file_waitForEvents, file_isWriteReady, file_isReadReady};

struct DirData {
    S32 pos;
    struct Node* nodes[MAX_DIR_LISTING];
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

struct DirData* getDirData(struct OpenNode* node) {
    struct DirData* data = (struct DirData*)node->data;
    if (!data) {
        data = allocDirData();
        data->count = listNodes(node->node, data->nodes, MAX_DIR_LISTING);
        node->data = data;
    }
    return data;
}

S64 dir_length(struct OpenNode* node) {	
    struct DirData* data = getDirData(node);
    return data->count;
}

BOOL dir_setLength(struct OpenNode* node, S64 len) {
    return FALSE;
}

S64 dir_getFilePointer(struct OpenNode* node) {
    struct DirData* data = getDirData(node);
    return data->pos;
}

S64 dir_seek(struct OpenNode* node, S64 pos) {
    struct DirData* data = getDirData(node);
    if (pos>=0 && pos<=data->count)
        data->pos = (S32)pos;
    else
        data->pos = data->count;
    return data->pos;
}

U32 dir_read(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
    return 0;
}

U32 dir_write(MMU_ARG struct OpenNode* node, U32 address, U32 len) {
    return 0;
}

void dir_close(struct OpenNode* node) {
    struct DirData* data = (struct DirData*)node->data;
    if (data) {
        freeDirData(data);
    }
    removeOpenNodeFromNode(node);
    freeOpenNode(node);
}

U32 dir_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
    return -K_ENODEV;
}

void dir_setAsync(struct OpenNode* node, struct KProcess* process, FD fd, BOOL isAsync) {
    if (isAsync)
        kwarn("dir_setAsync not implemented");
}

BOOL dir_isAsync(struct OpenNode* node, struct KProcess* process) {
    return 0;
}

void dir_waitForEvents(struct OpenNode* node, struct KThread* thread, U32 events) {
    kwarn("dir_waitForEvents not implemented");
}

BOOL dir_isWriteReady(struct OpenNode* node) {
    return FALSE;
}

BOOL dir_isReadReady(struct OpenNode* node) {
    return FALSE;
}

U32 dir_map(MMU_ARG struct OpenNode* node, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
    return 0;
}

BOOL dir_canMap(struct OpenNode* node) {
    return FALSE;
}

BOOL dir_init(struct KProcess* process, struct OpenNode* node) {
    node->data = 0;
    return TRUE;
}

U32 getDirCount(struct OpenNode* node) {
    struct DirData* data = getDirData(node);
    return data->count;
}

struct Node* getDirNode(struct OpenNode* node, U32 index) {
    struct DirData* data = getDirData(node);
    if (index<data->count)
        return data->nodes[index];
    return 0;
}

struct NodeAccess dirAccess = {dir_init, dir_length, dir_setLength, dir_getFilePointer, dir_seek, dir_read, dir_write, dir_close, dir_map, dir_canMap, dir_ioctl, dir_setAsync, dir_isAsync, dir_waitForEvents, dir_isWriteReady, dir_isReadReady};

char pathTmp[MAX_FILEPATH_LEN];

const char* pathMakeWindowsHappy(const char* path) {
    if (path[strlen(path)-1]==pathSeperator) {
        safe_strcpy(pathTmp, path, MAX_FILEPATH_LEN);
        pathTmp[strlen(path)-1]=0;
        return pathTmp;
    }
    return path;
}

BOOL file_isDirectory(struct Node* node) {
    struct stat buf;
    const char* path = node->path.nativePath;

    path = pathMakeWindowsHappy(path);
    if (stat(path, &buf)==0) {
        return S_ISDIR(buf.st_mode);
    }
    return FALSE;
}

static char tmpPath[MAX_FILEPATH_LEN];
static char tmpLocalPath[MAX_FILEPATH_LEN];
static char tmpNativePath[MAX_FILEPATH_LEN];

BOOL file_remove(struct Node* node) {
    BOOL result;
    BOOL forcedClose = FALSE;

    if (node->path.isLink) {
        safe_strcpy(tmpLocalPath, node->path.localPath, MAX_FILEPATH_LEN);
        safe_strcat(tmpLocalPath, ".link", MAX_FILEPATH_LEN);
        node = getNodeFromLocalPath("", tmpLocalPath, TRUE);
    }
    result = unlink(node->path.nativePath)==0;
    if (!result && node->nodeType->exists(node)) {
        struct OpenNode* openNode = node->openNodes;
        int i;
        U32 isLink;

        while (openNode) {
            openNode->cachedPosDuringDelete = openNode->access->getFilePointer(openNode);
            close(openNode->handle);
            openNode->handle = 0xFFFFFFFF;
            openNode = openNode->nextOpen;
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
        if (rename(node->path.nativePath, tmpNativePath)!=0) {
            kpanic("could not rename %s", node->path.nativePath);
        }
        openNode = node->openNodes;
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

            openNode->handle = open(tmpNativePath, openFlags, 0666);
            openNode->access->seek(openNode, openNode->cachedPosDuringDelete);
            openNode = openNode->nextOpen;
        }
        forcedClose = TRUE;
        result = TRUE;
    }
    return result;
}

U64 file_lastModified(struct Node* node) {
    struct stat buf;
    const char* path = node->path.nativePath;

    path = pathMakeWindowsHappy(path);
    if (stat(path, &buf)==0) {
        return buf.st_mtime;
    }
    return 0;
}

U64 file_length(struct Node* node) {
    struct stat buf;
    const char* path = node->path.nativePath;

    path = pathMakeWindowsHappy(path);
    if (stat(path, &buf)==0) {
        return buf.st_size;
    }
    return 0;
}

struct OpenNode* file_open(struct KProcess* process, struct Node* node, U32 flags) {
    U32 openFlags = O_BINARY;
    U32 f;

    if (node->nodeType->isDirectory(node)) {
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
    f = open(node->path.nativePath, openFlags, 0666);	
    if (!f || f==0xFFFFFFFF)
        return 0;
    return allocOpenNode(process, node, f, flags, &fileAccess);
}

BOOL file_setLastModifiedTime(struct Node* node, U32 time) {
    struct utimbuf buf = {time, time};
    return utime(node->path.nativePath, &buf)==0;
}

U32 file_getType(struct Node* node, U32 checkForLink) {	
    if (file_isDirectory(node))
        return 4; // DT_DIR
    if (checkForLink && node->path.isLink) 
        return 10; // DT_LNK
    return 8; // DT_REG
}

U32 file_getMode(struct KProcess* process, struct Node* node) {
    U32 result = K__S_IREAD | K__S_IEXEC | (file_getType(node, 0) << 12);
    if (process->userId == 0 || strstr(node->path.localPath, "/tmp")==node->path.localPath || strstr(node->path.localPath, "/var")==node->path.localPath || strstr(node->path.localPath, "/home")==node->path.localPath) {
        result|=K__S_IWRITE;
    }
    return result;
}

BOOL file_canRead(struct KProcess* process, struct Node* node) {
    return file_getMode(process, node) & K__S_IREAD;
}

BOOL file_canWrite(struct KProcess* process, struct Node* node) {
    return file_getMode(process, node) & K__S_IWRITE;
}

BOOL file_exists(struct Node* node) {
    return doesPathExist(node->path.nativePath);
}

U32 file_rename(struct Node* oldNode, struct Node* newNode) {
    struct OpenNode* openNode = oldNode->openNodes;
    U32 result;

    while (openNode) {
        openNode->cachedPosDuringDelete = openNode->access->getFilePointer(openNode);
        close(openNode->handle);
        openNode->handle = 0xFFFFFFFF;
        openNode = openNode->nextOpen;
    }

    result = rename(oldNode->path.nativePath, newNode->path.nativePath);

    openNode = oldNode->openNodes;
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

        openNode->handle = open(newNode->path.nativePath, openFlags, 0666);
        openNode->access->seek(openNode, openNode->cachedPosDuringDelete);
        openNode = openNode->nextOpen;
    }
    if (result!=0)
        result = -K_EIO;
    return result;
}

struct NodeType fileNodeType = {file_isDirectory, file_exists, file_rename, file_remove, file_lastModified, file_length, file_open, file_setLastModifiedTime, file_canRead, file_canWrite, file_getType, file_getMode};

struct Node* getNodeInCache(const char* localPath) {
    return (struct Node*)getHashmapValue(&nodeMap, localPath);
}

struct Node* getParentNode(struct Node* node) {
    static char tmp[MAX_FILEPATH_LEN];
    const char* pos = strrchr(node->path.localPath, '/');
    if (pos) {
        safe_strcpy(tmp, node->path.localPath, MAX_FILEPATH_LEN);
        tmp[pos-node->path.localPath]=0;
        return getNodeFromLocalPath("/", tmp, FALSE);
    }
    return 0;
}

void trimTrailingSpaces(char* path) {
    int i;
    int len = strlen(path);
    for (i=len-1;i>=0;i--) {
        if (path[i]==' ')
            path[i] = 0;
        else
            break;
    }
}

struct LocalPath {
    char nativePath[MAX_FILEPATH_LEN];
    char localPath[MAX_FILEPATH_LEN];
    U32 isLink;
};

struct Node* getLocalAndNativePaths(const char* currentDirectory, const char* path, char* localPath, int localPathSize, char* nativePath, int nativePathSize, U32* isLink) {
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
        safe_strcpy(nativePath, root, nativePathSize);	
        safe_strcat(nativePath, localPath, nativePathSize);
        while (TRUE) {
            localPathToRemote(nativePath+strlen(root)); // don't convert colon's in the root path			
            if (followLinks(nativePath, nativePathSize, &tmp32)) {
                normalizePath(nativePath);
                memmove(nativePath+strlen(root), nativePath, strlen(nativePath)+1);
                memcpy(nativePath, root, strlen(root));
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
    return (struct Node*)getHashmapValue(&nodeMap, localPath);
    /* why was this necessary?  it is not compatible with localSkipLinksMap since the link isn't rechecked.  If a
    file was removed shouldn't it just be removed from nodeMap
    result = (struct Node*)getHashmapValue(&nodeMap, localPath);
    if (result) {
        // might have changed from link to file or visa versa
        if (result->path.nativePath)
            safe_strcpy((char*)result->path.nativePath, nativePath, result->path.nativePathSize);
        result->path.isLink = tmp32;
        return result;
    }    
    return 0;
    */
}

struct Node* getNodeFromLocalPath(const char* currentDirectory, const char* path, BOOL existing) {
    char localPath[MAX_FILEPATH_LEN];
    char nativePath[MAX_FILEPATH_LEN];
    U32 isLink = 0;
    struct Node* result = getLocalAndNativePaths(currentDirectory, path, localPath, MAX_FILEPATH_LEN, nativePath, MAX_FILEPATH_LEN, &isLink);
        
    if (result) {
        return result;		
    }
    
    if (existing && !doesPathExist(nativePath)) {
        return 0;
    }
    
    result = allocNode(localPath, nativePath, &fileNodeType, 1);	
    result->path.isLink = isLink;
    return result;
}

struct OpenNode* freeOpenNodes;
void freeOpenNode(struct OpenNode* node) {
    node->next = freeOpenNodes;
    freeOpenNodes = node;
}

struct OpenNode* allocOpenNode(struct KProcess* process, struct Node* node, U32 handle, U32 flags, struct NodeAccess* nodeAccess) {
    struct OpenNode* result;

    if (freeOpenNodes) {
        result = freeOpenNodes;
        freeOpenNodes = result->next;
        memset(result, 0, sizeof(struct OpenNode));
    } else {
        result = (struct OpenNode*)kalloc(sizeof(struct OpenNode), KALLOC_OPENNODE);
    }	
    result->handle = handle;
    result->flags = flags;
    result->access = nodeAccess;
    result->node = node;
    if (nodeAccess->init(process, result)) {
        struct Node* actualNode = node;

        if (node->path.isLink) {
            static char tmp[MAX_FILEPATH_LEN];
            safe_strcpy(tmp, node->path.nativePath, MAX_FILEPATH_LEN);
            remotePathToLocal(tmp);
            result->linkedNoded = getNodeFromLocalPath("", tmp, TRUE);
            actualNode = result->linkedNoded;
        }
        result->nextOpen = actualNode->openNodes;
        actualNode->openNodes = result;
        return result;
    }
    freeOpenNode(result);
    return 0;
}

struct Node* allocNode(const char* localPath, const char* nativePath, struct NodeType* nodeType, U32 rdev) {
    struct Node* result;

    U32 localLen = 0;
    U32 nativeLen = 0;
    if (localPath)
        localLen=strlen(localPath)+40;
    if (nativePath)
        nativeLen=strlen(nativePath)+40;
    result = (struct Node*)kalloc(sizeof(struct Node)+localLen+nativeLen, KALLOC_NODE);
    result->id = nodeId++;
    result->nodeType = nodeType;
    result->rdev = rdev;
    result->path.localPathSize = localLen;
    result->path.nativePathSize = nativeLen;
    if (localPath) {
        result->path.localPath = (const char*)result+sizeof(struct Node);
        safe_strcpy((char*)result->path.localPath, localPath, result->path.localPathSize);
    } else {
        result->path.localPath = 0;
    }
    if (nativePath) {
        result->path.nativePath = (const char*)result+sizeof(struct Node)+localLen;
        safe_strcpy((char*)result->path.nativePath, nativePath, result->path.nativePathSize);
    } else {
        result->path.nativePath = 0;
    }
    result->name = strrchr(result->path.localPath, '/')+1;
    putHashmapValue(&nodeMap, result->path.localPath, result);	
    return result;
}

void removeNode(struct Node* node) {
    removeHashmapKey(&nodeMap, node->path.localPath);
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
    int rootlen = strlen(root);

    if (strncmp(path, root, rootlen)==0) {
        memmove(path, path+rootlen, strlen(path)-rootlen+1); 
    }
    for (i=0;i<len;i++) {
        if (path[i]==pathSeperator)
            path[i]='/';		
    }
    // :TODO: is reversing the colon code necessary
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

BOOL doesPathExist(const char* path) {
    path = pathMakeWindowsHappy(path);
    if (access(path, 0)!=-1) {
        return TRUE;
    }
    return FALSE;
}

U32 symlinkInDirectory(struct KThread* thread, const char* currentDirectory, U32 path1, U32 path2) {
    char* s1 = getNativeString(MMU_PARAM_THREAD path1);
    char* s2 = getNativeString2(MMU_PARAM_THREAD path2);
    char tmp[MAX_FILEPATH_LEN];
    struct Node* node;
    struct OpenNode* openNode;

    node = getNodeFromLocalPath(currentDirectory, s2, TRUE);
    if (node && node->nodeType->exists(node)) {
        return -K_EEXIST;
    }
    safe_strcpy(tmp, s2, MAX_FILEPATH_LEN);
    safe_strcat(tmp, ".link", MAX_FILEPATH_LEN);
    node = getNodeFromLocalPath(currentDirectory, tmp, FALSE);
    if (!node || node->nodeType->exists(node)) {
        // :TODO: figure out real problem and remove this hack for libpango1.0-common_1.28.3-1+squeeze2_all.deb
        if (!strcmp(tmp, "/var/lib/defoma/locked.link")) {
            node->nodeType->remove(node);
            node = getNodeFromLocalPath(currentDirectory, tmp, FALSE);
        } else {
            return -K_EEXIST;
        }
    }
    if (!node->nodeType->canWrite(thread->process, node)) {
        return -K_EACCES;
    }
    openNode = node->nodeType->open(thread->process, node, K_O_WRONLY|K_O_CREAT);
    if (!openNode) {
        return -K_EIO;
    }
    openNode->access->write(MMU_PARAM_THREAD openNode, path1, strlen(s1));
    openNode->access->close(openNode);
    pblMapClear(localSkipLinksMap);
    return 0;
}

U32 syscall_link(struct KThread* thread, U32 from, U32 to) {
    struct Node* fromNode = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(MMU_PARAM_THREAD from), TRUE);
    struct Node* toNode = getNodeFromLocalPath(thread->process->currentDirectory, getNativeString(MMU_PARAM_THREAD to), FALSE);
    struct OpenNode* fromOpenNode;
    struct OpenNode* toOpenNode;
    char buffer[PAGE_SIZE];

    if (!fromNode || !fromNode->nodeType->exists(fromNode)) {
        return -K_ENOENT;
    }

    if (toNode->nodeType->exists(toNode)) {
        return -K_EEXIST;
    }
    if (fromNode->nodeType->isDirectory(fromNode)) {
        return -K_EPERM;
    }
    fromOpenNode = fromNode->nodeType->open(thread->process, fromNode, K_O_RDONLY);
    if (!fromOpenNode)
        return -K_EIO;

    toOpenNode = toNode->nodeType->open(thread->process, toNode, K_O_WRONLY|K_O_CREAT);
    if (!toOpenNode) {
        fromOpenNode->access->close(fromOpenNode);
        return -K_EIO;
    }

    while (1) {
        U32 r = read(fromOpenNode->handle, buffer, PAGE_SIZE);	
        write(toOpenNode->handle, buffer, r);
        if (r<PAGE_SIZE)
            break;
    }
    toOpenNode->access->close(toOpenNode);
    fromOpenNode->access->close(fromOpenNode);
    if (toNode->nextHardLink!=0 || fromNode->nextHardLink!=0)
        kwarn("Hard link counting doesn't support more than 1 link");
    toNode->nextHardLink = fromNode;
    fromNode->nextHardLink = toNode;
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
            struct OpenNode* openNode = (struct OpenNode*)fd->kobject->data;
            currentDirectory = openNode->node->path.localPath;
            if (!openNode->node->nodeType->isDirectory(openNode->node)) {
                return -K_ENOTDIR;
            }
        }
    }
    return symlinkInDirectory(thread, currentDirectory, oldpath, newpath);
}

U32 syscall_symlink(struct KThread* thread, U32 path1, U32 path2) {
    return symlinkInDirectory(thread, thread->process->currentDirectory, path1, path2);
}
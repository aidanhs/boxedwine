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

#include UNISTD
#include UTIME

static char root[1024];
static int nodeId = 1;
static struct KHashmap nodeMap;

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

	strcpy(root, rootPath);

	if (root[len-1]==pathSeperator)
		root[len-1] = 0;
	initHashmap(&nodeMap);
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

U32 file_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	if (PAGE_SIZE-(address & (PAGE_SIZE-1)) >= len) {
		U8* ram = getPhysicalAddress(memory, address);
		return read(node->handle, ram, len);		
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
}

U32 file_write(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
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
}

void file_close(struct OpenNode* node) {
	close(node->handle);
	freeOpenNode(node);
}

U32 file_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
	return -K_ENODEV;
}

BOOL file_isWriteReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)==K_O_RDONLY;
}

BOOL file_isReadReady(struct OpenNode* node) {
	return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

U32 file_map(struct OpenNode* node, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
	return 0;
}

BOOL file_canMap(struct OpenNode* node) {
	return TRUE;
}

BOOL file_init(struct KProcess* process, struct OpenNode* node) {
	return TRUE;
}

struct NodeAccess fileAccess = {file_init, openfile_length, file_setLength, file_getFilePointer, file_seek, file_read, file_write, file_close, file_map, file_canMap, file_ioctl, file_isWriteReady, file_isReadReady};

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
		result = (struct DirData*)kalloc(sizeof(struct DirData));
	}	
	return result;
}

S64 dir_length(struct OpenNode* node) {	
	struct DirData* data = (struct DirData*)node->data;
	return data->count;
}

BOOL dir_setLength(struct OpenNode* node, S64 len) {
	return FALSE;
}

S64 dir_getFilePointer(struct OpenNode* node) {
	struct DirData* data = (struct DirData*)node->data;
	return data->pos;
}

S64 dir_seek(struct OpenNode* node, S64 pos) {
	struct DirData* data = (struct DirData*)node->data;
	if (pos>=0 && pos<=data->count)
		data->pos = (S32)pos;
	else
		data->pos = data->count;
	return data->pos;
}

U32 dir_read(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return 0;
}

U32 dir_write(struct Memory* memory, struct OpenNode* node, U32 address, U32 len) {
	return 0;
}

void dir_close(struct OpenNode* node) {
	struct DirData* data = (struct DirData*)node->data;
	if (data) {
		freeDirData(data);
	}
	freeOpenNode(node);
}

U32 dir_ioctl(struct KThread* thread, struct OpenNode* node, U32 request) {
	return -K_ENODEV;
}

BOOL dir_isWriteReady(struct OpenNode* node) {
	return FALSE;
}

BOOL dir_isReadReady(struct OpenNode* node) {
	return FALSE;
}

U32 dir_map(struct OpenNode* node, struct Memory* memory, U32 address, U32 len, S32 prot, S32 flags, U64 off) {
	return 0;
}

BOOL dir_canMap(struct OpenNode* node) {
	return FALSE;
}

BOOL dir_init(struct KProcess* process, struct OpenNode* node) {
	struct DirData* data = allocDirData();
	node->data = data;
	data->count = listNodes(node->node, data->nodes, MAX_DIR_LISTING);
	return TRUE;
}

U32 getDirCount(struct OpenNode* node) {
	struct DirData* data = (struct DirData*)node->data;
	return data->count;
}

struct Node* getDirNode(struct OpenNode* node, U32 index) {
	struct DirData* data = (struct DirData*)node->data;
	if (index<data->count)
		return data->nodes[index];
	return 0;
}

struct NodeAccess dirAccess = {dir_init, dir_length, dir_setLength, dir_getFilePointer, dir_seek, dir_read, dir_write, dir_close, dir_map, dir_canMap, dir_ioctl, dir_isWriteReady, dir_isReadReady};

char pathTmp[MAX_FILEPATH_LEN];

const char* pathMakeWindowsHappy(const char* path) {
	if (path[strlen(path)-1]==pathSeperator) {
		strcpy(pathTmp, path);
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

BOOL file_remove(struct Node* node) {
	return unlink(node->path.nativePath)==0;
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

U32 file_getType(struct Node* node) {
	if (file_isDirectory(node))
		return 4; // DT_DIR
	return 8; // DT_REG
}

U32 file_getMode(struct Node* node) {
	struct stat buf;
	U32 result = 0;
	const char* path = node->path.nativePath;

	path = pathMakeWindowsHappy(path);
	if (stat(path, &buf)==0) {
		if (S_ISDIR(buf.st_mode))
			result |= K__S_IFDIR;
		else
			result |= K__S_IFREG;
		if (buf.st_mode & S_IREAD)
			result |= K__S_IREAD;
		if (buf.st_mode & S_IWRITE)
			result |= K__S_IWRITE;
		if (buf.st_mode & S_IEXEC)
			result |= K__S_IEXEC;
	}
	return result;
}

BOOL file_canRead(struct Node* node) {
	return file_getMode(node) & K__S_IREAD;
}

BOOL file_canWrite(struct Node* node) {
	return file_getMode(node) & K__S_IREAD;
}

BOOL file_exists(struct Node* node) {
	return doesPathExist(node->path.nativePath);
}

U32 file_rename(struct Node* oldNode, struct Node* newNode) {
	if (!rename(oldNode->path.nativePath, newNode->path.nativePath))
		return 0;
	return K_EIO;
}

struct NodeType fileNodeType = {file_isDirectory, file_exists, file_rename, file_remove, file_lastModified, file_length, file_open, file_setLastModifiedTime, file_canRead, file_canWrite, file_getType, file_getMode};

struct Node* getNodeInCache(const char* localPath) {
	return (struct Node*)getHashmapValue(&nodeMap, localPath);
}

struct Node* getLocalAndNativePaths(const char* currentDirectory, const char* path, char* localPath, char* nativePath) {
	struct Node* result;

	if (path[0]=='/')
		strcpy(localPath, path);
	else {
		strcpy(localPath, currentDirectory);
		strcat(localPath, "/");
		strcat(localPath, path);
	}	

	normalizePath(localPath);
	result = (struct Node*)getHashmapValue(&nodeMap, localPath);
	if (result) {
		if (result->path.nativePath)
			strcpy(nativePath, result->path.nativePath);
		else {
			nativePath[0]=0;
		}
		return result;
	}
	strcpy(nativePath, root);	
	strcat(nativePath, localPath);
	while (TRUE) {
		localPathToRemote(nativePath+strlen(root)); // don't convert colon's in the root path			
		if (followLinks(nativePath)) {
			normalizePath(nativePath);
			memmove(nativePath+strlen(root), nativePath, strlen(nativePath)+1);
			memcpy(nativePath, root, strlen(root));
		} else {
			break;
		}
	}
	return 0;
}

struct Node* getNodeFromLocalPath(const char* currentDirectory, const char* path, BOOL existing) {
	char localPath[MAX_FILEPATH_LEN];
	char nativePath[MAX_FILEPATH_LEN];
	struct Node* result = getLocalAndNativePaths(currentDirectory, path, localPath, nativePath);
		
	if (result)
		return result;		
	
	if (existing && !doesPathExist(nativePath)) {
		return 0;
	}
	
	return allocNode(localPath, nativePath, &fileNodeType, 1);	
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
		result = (struct OpenNode*)kalloc(sizeof(struct OpenNode));
	}	
	result->handle = handle;
	result->flags = flags;
	result->access = nodeAccess;
	result->node = node;
	if (nodeAccess->init(process, result))
		return result;
	freeOpenNode(result);
	return 0;
}

struct Node* allocNode(const char* localPath, const char* nativePath, struct NodeType* nodeType, U32 rdev) {
	struct Node* result;
	U32 localLen = 0;
	U32 nativeLen = 0;
	if (localPath)
		localLen=strlen(localPath)+1;
	if (nativePath)
		nativeLen=strlen(nativePath)+1;
	result = (struct Node*)kalloc(sizeof(struct Node)+localLen+nativeLen);
	result->id = nodeId++;
	result->nodeType = nodeType;
	if (localPath) {
		result->path.localPath = (const char*)result+sizeof(struct Node);
		strcpy((char*)result->path.localPath, localPath);
	} else {
		result->path.localPath = 0;
	}
	if (nativePath) {
		result->path.nativePath = (const char*)result+sizeof(struct Node)+localLen;
		strcpy((char*)result->path.nativePath, nativePath);
	} else {
		result->path.nativePath = 0;
	}
	result->name = strrchr(result->path.localPath, '/')+1;
	putHashmapValue(&nodeMap, result->path.localPath, result);
	return result;
}

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
			}
		} else if (path[i]=='/') {
			lastDir2 = lastDir;
			lastDir = i;
		}
	}
	return TRUE;
}

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

BOOL readLink(const char* path, char* buffer) {
	struct stat buf;
	int h;
	char tmp[MAX_FILEPATH_LEN];

	path = pathMakeWindowsHappy(path);
	if (stat(path, &buf)!=0) {
		return FALSE;
	}
	if (buf.st_size>=MAX_FILEPATH_LEN) {
		kwarn("%s contains a link longer than the MAX_PATH: %d", path, MAX_FILEPATH_LEN);
		return FALSE;
	}
	h = open(path, O_RDONLY);
	if (!h)
		return FALSE;
	if (read(h, tmp, buf.st_size)!=buf.st_size) {
		close(h);
		return FALSE;
	}
	tmp[buf.st_size]=0;
	if (tmp[0]!='/') {
		int len = strrchr(path, pathSeperator)-path;
		memcpy(buffer, path, len);
		buffer[len+1]=0;
		strcat(buffer, tmp);
		remotePathToLocal(buffer);
	} else {
		strcpy(buffer, tmp);
	}
	close(h);
	return TRUE;
}

BOOL followLinks(char* path) {
	int len;
	int i;
	char tmp[MAX_FILEPATH_LEN];

	if (doesPathExist(path))
		return FALSE;
	strcpy(tmp, path);
	strcat(tmp, ".link");
	if (doesPathExist(tmp)) {
		if (readLink(tmp, tmp)) {
			strcpy(path, tmp);
			return TRUE;
		}
	}

	len = strlen(path);
	for (i=0;i<len;i++) {
		if (path[i]==pathSeperator) {
			strcpy(tmp, path); // don't include path seperator
			tmp[i]=0;
			if (!doesPathExist(tmp)) {
				strcat(tmp, ".link");
				if (doesPathExist(tmp)) {
					if (readLink(tmp, tmp)) {
						strcat(tmp, path+i); 
						strcpy(path, tmp);						
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL doesPathExist(const char* path) {
	struct stat buf;

	path = pathMakeWindowsHappy(path);
	if (stat(path, &buf)==0) {
		return TRUE;
	}
	return FALSE;
}

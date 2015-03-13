#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "filesystem.h"
#include "pathutils.h"
#include "khashmap.h"
#include "nodeaccess.h"
#include "nodetype.h"
#include "kstat.h"
#include "kerror.h"
#include "kalloc.h"

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

BOOL file_canMap(struct OpenNode* node) {
	return TRUE;
}

void file_init(struct OpenNode* node) {
}

struct NodeAccess fileAccess = {file_init, openfile_length, file_setLength, file_getFilePointer, file_seek, file_read, file_write, file_close, file_canMap, file_ioctl, file_isWriteReady, file_isReadReady};

BOOL file_isDirectory(struct Node* node) {
	struct stat buf;

	if (stat(node->path.nativePath, &buf)==0) {
		return S_ISDIR(buf.st_mode);
	}
	return FALSE;
}

struct Node* file_list(struct Node* node) {
    return 0;
}

BOOL file_remove(struct Node* node) {
	return unlink(node->path.nativePath)==0;
}

U64 file_lastModified(struct Node* node) {
	struct stat buf;

	if (stat(node->path.nativePath, &buf)==0) {
		return buf.st_mtime;
	}
	return 0;
}

U64 file_length(struct Node* node) {
	struct stat buf;

	if (stat(node->path.nativePath, &buf)==0) {
		return buf.st_size;
	}
	return 0;
}

struct OpenNode* file_open(struct Node* node, U32 flags) {
	U32 openFlags = O_BINARY;
	U32 f;

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
		openFlags|=K_O_EXCL;
	}
	if (flags & K_O_TRUNC) {
		openFlags|=O_TRUNC;
	}
	if (flags & K_O_APPEND) {
		openFlags|=O_APPEND;
	}
	f = open(node->path.nativePath, openFlags);	
	if (!f)
		return 0;
	return allocOpenNode(node, f, flags, node->nodeAccess);
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
	if (stat(node->path.nativePath, &buf)==0) {
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

struct NodeType fileNodeType = {file_isDirectory, file_list, file_remove, file_lastModified, file_length, file_open, file_setLastModifiedTime, file_canRead, file_canWrite, file_getType, file_getMode};

struct Node* getNodeFromLocalPath(const char* currentDirectory, const char* path) {
	char localPath[MAX_PATH];
	char nativePath[MAX_PATH];
	struct Node* result;
	
	if (path[0]=='/')
		strcpy(localPath, path);
	else {
		strcpy(localPath, currentDirectory);
		strcat(localPath, "/");
		strcat(localPath, path);
	}	

	result = (struct Node*)getHashmapValue(&nodeMap, localPath);
	if (result)
		return result;

	strcpy(nativePath, root);
	strcat(nativePath, localPath);
	localPathToRemote(nativePath);	
	normalizePath(nativePath);
	followLinks(nativePath);
	
	if (!doesPathExist(nativePath)) {
		return 0;
	}
	
	return allocNode(localPath, nativePath, &fileNodeType, &fileAccess, 1);	
}

struct OpenNode* freeOpenNodes;
void freeOpenNode(struct OpenNode* node) {
	node->next = freeOpenNodes;
	freeOpenNodes = node;
}

struct OpenNode* allocOpenNode(struct Node* node, U32 handle, U32 flags, struct NodeAccess* nodeAccess) {
	struct OpenNode* result;

	if (freeOpenNodes) {
		result = freeOpenNodes;
		freeOpenNodes = result->next;
	} else {
		result = (struct OpenNode*)kalloc(sizeof(struct OpenNode));
	}
	result->handle = handle;
	result->flags = flags;
	result->access = nodeAccess;
	result->node = node;
	nodeAccess->init(result);
	return result;
}

struct Node* allocNode(const char* localPath, const char* nativePath, struct NodeType* nodeType, struct NodeAccess* nodeAccess, U32 rdev) {
	struct Node* result;
	U32 localLen = 0;
	U32 nativeLen = 0;
	if (localPath)
		localLen=strlen(localPath)+1;
	if (nativePath)
		nativeLen=strlen(nativePath)+1;
	result = (struct Node*)kalloc(sizeof(struct Node)+localLen+nativeLen);
	result->id = nodeId++;
	result->data = 0;
	result->nodeType = nodeType;
	result->nodeAccess = nodeAccess;
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
	putHashmapValue(&nodeMap, result->path.localPath, result);
	return result;
}

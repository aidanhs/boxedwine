#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <io.h>
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
static KHashmap nodeMap;

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

S64 openfile_length(OpenNode* node) {
	S32 currentPos = lseek(node->handle, 0, SEEK_CUR);
    S32 size = lseek(node->handle, 00, SEEK_END);
    lseek(node->handle, currentPos, SEEK_SET);
	return size;
}

BOOL file_setLength(OpenNode* node, S64 len) {
	return ftruncate64(node->handle, len)==0;
}

S64 file_getFilePointer(OpenNode* node) {
	return lseek64(node->handle, 0, SEEK_CUR);
}

S64 file_seek(OpenNode* node, S64 pos) {
	return lseek64(node->handle, pos, SEEK_SET);
}

U32 file_read(Memory* memory, OpenNode* node, U32 address, U32 len) {
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

U32 file_write(Memory* memory, OpenNode* node, U32 address, U32 len) {
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

void file_close(OpenNode* node) {
	close(node->handle);
	freeOpenNode(node);
}

U32 file_ioctl(KThread* thread, OpenNode* node, U32 request) {
	return -K_ENODEV;
}

BOOL file_isWriteReady(OpenNode* node) {
	return (node->flags & K_O_ACCMODE)==K_O_RDONLY;
}

BOOL file_isReadReady(OpenNode* node) {
	return (node->flags & K_O_ACCMODE)!=K_O_WRONLY;
}

BOOL file_canMap(OpenNode* node) {
	return TRUE;
}

void file_init(OpenNode* node) {
}

NodeAccess fileAccess = {file_init, openfile_length, file_setLength, file_getFilePointer, file_seek, file_read, file_write, file_close, file_canMap, file_ioctl, file_isWriteReady, file_isReadReady};

BOOL file_isDirectory(Node* node) {
	struct stat buf;

	if (stat(node->path.nativePath, &buf)==0) {
		return S_ISDIR(buf.st_mode);
	}
	return FALSE;
}

Node* file_list(Node* node) {
}

BOOL file_remove(Node* node) {
	return unlink(node->path.nativePath)==0;
}

U64 file_lastModified(Node* node) {
	struct stat buf;

	if (stat(node->path.nativePath, &buf)==0) {
		return buf.st_mtime;
	}
	return 0;
}

U64 file_length(Node* node) {
	struct stat buf;

	if (stat(node->path.nativePath, &buf)==0) {
		return buf.st_size;
	}
	return 0;
}

OpenNode* file_open(Node* node, U32 flags) {
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

BOOL file_setLastModifiedTime(Node* node, U32 time) {
	struct utimbuf buf = {time, time};
	return utime(node->path.nativePath, &buf)==0;
}

U32 file_getType(Node* node) {
	if (file_isDirectory(node))
		return 4; // DT_DIR
	return 8; // DT_REG
}

U32 file_getMode(Node* node) {
	struct stat buf;
	U32 result = 0;
	if (stat(node->path.nativePath, &buf)==0) {
		if (S_ISDIR(buf.st_mode))
			result |= K__S_IFDIR;
		else
			result |= K__S_IFREG;
		if (buf.st_mode & _S_IREAD)
			result |= K__S_IREAD;
		if (buf.st_mode & _S_IWRITE)
			result |= K__S_IWRITE;
		if (buf.st_mode & _S_IEXEC)
			result |= K__S_IEXEC;
	}
	return result;
}

BOOL file_canRead(Node* node) {
	return file_getMode(node) & K__S_IREAD;
}

BOOL file_canWrite(Node* node) {
	return file_getMode(node) & K__S_IREAD;
}

NodeType fileNodeType = {file_isDirectory, file_list, file_remove, file_lastModified, file_length, file_open, file_setLastModifiedTime, file_canRead, file_canWrite, file_getType, file_getMode};

Node* getNodeFromLocalPath(const char* currentDirectory, const char* path) {
	char localPath[MAX_PATH];
	char nativePath[MAX_PATH];
	Node* result;
	
	if (path[0]=='/')
		strcpy(localPath, path);
	else {
		strcpy(localPath, currentDirectory);
		strcat(localPath, "/");
		strcat(localPath, path);
	}	

	result = (Node*)getHashmapValue(&nodeMap, localPath);
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

OpenNode* freeOpenNodes;
void freeOpenNode(OpenNode* node) {
	node->next = freeOpenNodes;
	freeOpenNodes = node;
}

OpenNode* allocOpenNode(Node* node, U32 handle, U32 flags, NodeAccess* nodeAccess) {
	OpenNode* result;

	if (freeOpenNodes) {
		result = freeOpenNodes;
		freeOpenNodes = result->next;
	} else {
		result = (OpenNode*)kalloc(sizeof(OpenNode));
	}
	result->handle = handle;
	result->flags = flags;
	result->access = nodeAccess;
	result->node = node;
	nodeAccess->init(result);
	return result;
}

Node* allocNode(const char* localPath, const char* nativePath, NodeType* nodeType, NodeAccess* nodeAccess, U32 rdev) {
	Node* result;
	U32 localLen = 0;
	U32 nativeLen = 0;
	if (localPath)
		localLen=strlen(localPath)+1;
	if (nativePath)
		nativeLen=strlen(nativePath)+1;
	result = (Node*)kalloc(sizeof(Node)+localLen+nativeLen);
	result->id = nodeId++;
	result->data = 0;
	result->nodeType = nodeType;
	result->nodeAccess = nodeAccess;
	if (localPath) {
		result->path.localPath = (const char*)(result+sizeof(Node));
		strcpy(result->path.localPath, localPath);
	} else {
		result->path.localPath = 0;
	}
	if (nativePath) {
		result->path.nativePath = (const char*)(result+sizeof(Node)+localLen);
		strcpy(result->path.nativePath, nativePath);
	} else {
		result->path.nativePath = 0;
	}
	putHashmapValue(&nodeMap, result->path.localPath, result);
	return result;
}

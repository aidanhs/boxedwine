#include "virtualfile.h"
#include "node.h"
#include "filesystem.h"
#include "nodetype.h"
#include "kstat.h"
#include "log.h"
#include "kerror.h"

static int nextrdev = 100;

BOOL virtual_isDirectory(struct Node* node) {
	return FALSE;
}

BOOL virtual_remove(struct Node* node) {
	return 0;
}

U64 virtual_lastModified(struct Node* node) {
	return 0;
}

U64 virtual_length(struct Node* node) {
	return 0;
}

U32 virtual_getMode(struct Node* node) {
	return node->data1;
}

BOOL virtual_canRead(struct Node* node) {
	return virtual_getMode(node) & K__S_IREAD;
}

BOOL virtual_canWrite(struct Node* node) {
	return virtual_getMode(node) & K__S_IWRITE;
}

struct OpenNode* virtual_open(struct Node* node, U32 flags) {
	if ((flags & K_O_ACCMODE)==K_O_RDONLY) {
		if (!virtual_canRead(node))
			return 0;
	} else if ((flags & K_O_ACCMODE)==K_O_WRONLY) {
		if (!virtual_canWrite(node))
			return 0;
	} else {
		if (!virtual_canWrite(node))
			return 0;
		if (!virtual_canRead(node))
			return 0;
	}
	if (flags & K_O_CREAT) {
		return 0;
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
	return allocOpenNode(node, 0, flags, (struct NodeAccess*)node->pData2);
}

BOOL virtual_setLastModifiedTime(struct Node* node, U32 time) {
	return 0;
}

U32 virtual_getType(struct Node* node) {
	U32 mode = node->data1;

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

BOOL virtual_exists(struct Node* node) {
	return getNodeInCache(node->path.localPath)!=0;
}

U32 virtual_rename(struct Node* oldNode, struct Node* newNode) {
	return -K_EIO;
}

struct NodeType virtualNodeType = {virtual_isDirectory, virtual_exists, virtual_rename, virtual_remove, virtual_lastModified, virtual_length, virtual_open, virtual_setLastModifiedTime, virtual_canRead, virtual_canWrite, virtual_getType, virtual_getMode};

void addVirtualFile(const char* localPath, struct NodeAccess* nodeAccess, U32 mode) {
	struct Node* node = allocNode(localPath, 0, &virtualNodeType, nextrdev++);
	node->data1 = mode;
	node->pData2 = nodeAccess;
}

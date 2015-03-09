#include "virtualfile.h"
#include "node.h"
#include "filesystem.h"
#include "nodetype.h"
#include "kstat.h"
#include "log.h"

static int nextrdev = 100;

BOOL virtual_isDirectory(Node* node) {
	return node->nodeAccess == 0;
}

Node* virtual_list(Node* node) {
}

BOOL virtual_remove(Node* node) {
	return 0;
}

U64 virtual_lastModified(Node* node) {
	return 0;
}

U64 virtual_length(Node* node) {
	return 0;
}

U32 virtual_getMode(Node* node) {
	return node->data;
}

BOOL virtual_canRead(Node* node) {
	return virtual_getMode(node) & K__S_IREAD;
}

BOOL virtual_canWrite(Node* node) {
	return virtual_getMode(node) & K__S_IREAD;
}

OpenNode* virtual_open(Node* node, U32 flags) {
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
	return allocOpenNode(node, 0, flags, node->nodeAccess);
}

BOOL virtual_setLastModifiedTime(Node* node, U32 time) {
	return 0;
}

U32 virtual_getType(Node* node) {
	U32 mode = node->data;

	if (virtual_isDirectory(node))
		return 4; // DT_DIR
	if (mode & K__S_IFCHR) {
		return 2; // DT_CHR
	}
	return 8; // DT_REG
}

NodeType virtualNodeType = {virtual_isDirectory, virtual_list, virtual_remove, virtual_lastModified, virtual_length, virtual_open, virtual_setLastModifiedTime, virtual_canRead, virtual_canWrite, virtual_getType, virtual_getMode};

void addVirtualFile(const char* localPath, NodeAccess* nodeAccess, U32 mode) {
	Node* node = allocNode(localPath, 0, &virtualNodeType, nodeAccess, nextrdev++);
	node->data = mode;
}
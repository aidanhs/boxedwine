#include "platform.h"
#include "nodeaccess.h"
#include "elf.h"
#include "filesystem.h"
#include "nodeaccess.h"
#include "node.h"
#include "nodetype.h"
#include "kmmap.h"
#include "kthread.h"
#include "kprocess.h"

#include <string.h>

#include UNISTD

#define SHF_WRITE 1
#define SHF_ALLOC 2

#define SHT_NOBITS 8

#define PT_LOAD 1
#define PT_INTERP 3 

static char interp[MAX_FILEPATH_LEN];
static char shell_interp[MAX_FILEPATH_LEN];

char* getInterpreter(struct OpenNode* openNode, BOOL* isElf) {
	U8 buffer[sizeof(struct Elf32_Ehdr)];
	struct Elf32_Ehdr* hdr = (struct Elf32_Ehdr*)buffer;
	U32 len = read(openNode->handle, buffer, sizeof(buffer));

	*isElf=TRUE;
	if (len!=sizeof(buffer)) {
		*isElf=FALSE;
	}
	if (*isElf) {		
		*isElf = isValidElf(hdr);
	}
	if (!*isElf) {
		if (buffer[0]=='#') {
			U32 i;
			U32 mode = 0;
			U32 pos = 0;

			for (i=1;i<len;i++) {
				if (mode==0) {
					if (buffer[i]=='!')
						mode = 1;
				} else if (mode==1 && (buffer[i]==' ' || buffer[i]=='\t')) {
					continue;
				} else if (buffer[i]=='\n' || buffer[i]=='\r') {
					break;
				} else {
					mode = 2;
					shell_interp[pos++] = buffer[i];
				}
			}
			shell_interp[pos++]=0;
			return shell_interp;
		}
	} else {
		U32 i;

		openNode->access->seek(openNode, hdr->e_phoff);	
		for (i=0;i<hdr->e_phoff;i++) {
			struct Elf32_Phdr phdr;		
			openNode->access->seek(openNode, hdr->e_phoff+i*hdr->e_phentsize);	
			read(openNode->handle, &phdr, sizeof(struct Elf32_Phdr));
			if (phdr.p_type==PT_INTERP) {
				openNode->access->seek(openNode, phdr.p_offset);	
				read(openNode->handle, interp, phdr.p_filesz);
				interp[phdr.p_filesz] = 0;
				return interp;
			}
		}
	}
	return 0;
}

BOOL inspectNode(struct KProcess* process, const char* currentDirectory, struct Node* node, const char** loader, const char** interpreter, const char** interpreterArgs, U32* interpreterArgsCount, struct OpenNode** result) {
	BOOL isElf = 0;
	struct OpenNode* openNode = 0;
	struct Node* interpreterNode = 0;
	struct Node* loaderNode = 0;

	if (node) {
		openNode = node->nodeType->open(process, node, K_O_RDONLY);
	}
	if (openNode) {
		char* arg;

		*interpreterArgsCount=0;
		*interpreter = getInterpreter(openNode, &isElf);
		if (isElf) {
			*loader = *interpreter;
			*interpreter = 0;
		} else if (*interpreter) {
			arg=(char*)*interpreter;
			while ((arg=strchr(arg, ' '))) {
				arg[*interpreterArgsCount]=0;
				arg++;
				interpreterArgs[*interpreterArgsCount]=arg;
				*interpreterArgsCount=*interpreterArgsCount+1;
			}
		}
		openNode->access->close(openNode);
		openNode = 0;
	}
	if (!*interpreter && !isElf) {
		return FALSE;
	}
	if (*interpreter) {
		interpreterNode = getNodeFromLocalPath(currentDirectory, *interpreter, TRUE);	
		if (!interpreterNode || !interpreterNode->nodeType->exists(interpreterNode)) {
			printf("Interpreter not found: %s\n", *interpreter);
			return FALSE;
		}
		openNode = interpreterNode->nodeType->open(process, interpreterNode, K_O_RDONLY);		
		*loader = getInterpreter(openNode, &isElf);
		openNode->access->close(openNode);
		openNode = 0;
	}
	if (*loader) {
		loaderNode = getNodeFromLocalPath(currentDirectory, *loader, TRUE);	
		if (!loaderNode || !loaderNode->nodeType->exists(loaderNode)) {
			return FALSE;
		}
	}		

	if (loaderNode) {
		*result = loaderNode->nodeType->open(process, loaderNode, K_O_RDONLY);
	} else if (interpreterNode) {
		*result = interpreterNode->nodeType->open(process, interpreterNode, K_O_RDONLY);		
	} else {
		*result = node->nodeType->open(process, node, K_O_RDONLY);
	}
	if (*result)
		return TRUE;
	return FALSE;
}

int getMemSizeOfElf(struct OpenNode* openNode) {
	U8 buffer[sizeof(struct Elf32_Ehdr)];
	struct Elf32_Ehdr* hdr = (struct Elf32_Ehdr*)buffer;
	U32 len;
	U64 pos = openNode->access->getFilePointer(openNode);
	U32 address = 0xFFFFFFFF;
	int i;
	int sections = 0;

	openNode->access->seek(openNode, 0);
	len = read(openNode->handle, buffer, sizeof(buffer));
	if (len != sizeof(buffer)) {
		return 0;
	}
	if (!isValidElf(hdr)) {
		openNode->access->seek(openNode, pos);
		return 0;
	}

	len = 0;
	openNode->access->seek(openNode, hdr->e_phoff);
	for (i = 0; i<hdr->e_phnum; i++) {
		struct Elf32_Phdr phdr;
		read(openNode->handle, &phdr, sizeof(struct Elf32_Phdr));
		if (phdr.p_type == PT_LOAD) {
			if (phdr.p_paddr<address)
				address = phdr.p_paddr;
			if (len<phdr.p_paddr + phdr.p_memsz)
				len = phdr.p_paddr + phdr.p_memsz;
			sections++;
		}
	}
	openNode->access->seek(openNode, pos);
	return len - address + 4096*sections; // 4096 for alignment
}

U32 getPELoadAddress(struct OpenNode* openNode, U32* baseOfData, U32* sizeofData) {
	U8 buffer[1024];	
	U64 pos = openNode->access->getFilePointer(openNode);
	U32 len;
	U32 address = 0xFFFFFFFF;
	U32 i;
	U32 reloc;
	U32 flags = K_MAP_PRIVATE | K_MAP_ANONYMOUS;
	int offset;

	openNode->access->seek(openNode, 0);
	len = read(openNode->handle, buffer, sizeof(buffer));
	openNode->access->seek(openNode, pos);
	if (len != sizeof(buffer)) {		
		return FALSE;
	}
	// DOS Magic MZ
	if (buffer[0] != 0x4D || buffer[1] != 0x5A) {
		return 0;
	}
	offset = buffer[0x3C] | ((int)buffer[0x3D] << 8);
	if (buffer[offset] != 0x50 || buffer[offset + 1] != 0x45 || buffer[offset + 2] != 0 || buffer[offset + 3] != 0) {
		return 0;
	}
	// BaseOfData
	*baseOfData = buffer[offset + 0x30] | ((U32)buffer[offset + 0x31] << 8) | ((U32)buffer[offset + 0x32] << 16) | ((U32)buffer[offset + 0x33] << 24);
	// SizeOfInitializedData
	*sizeofData = buffer[offset + 0x20] | ((U32)buffer[offset + 0x21] << 8) | ((U32)buffer[offset + 0x22] << 16) | ((U32)buffer[offset + 0x23] << 24);
	// ImageBase
	return buffer[offset + 0x34] | ((U32)buffer[offset + 0x35] << 8) | ((U32)buffer[offset + 0x36] << 16) | ((U32)buffer[offset + 0x37] << 24);
}

BOOL loadProgram(struct KProcess* process, struct KThread* thread, struct OpenNode* openNode, U32* eip) {
	U8 buffer[sizeof(struct Elf32_Ehdr)];
	struct Elf32_Ehdr* hdr = (struct Elf32_Ehdr*)buffer;
	U32 len = read(openNode->handle, buffer, sizeof(buffer));
	U32 address=0xFFFFFFFF;
	U32 i;
	U32 reloc;
	U32 flags = K_MAP_PRIVATE | K_MAP_ANONYMOUS;

	if (len!=sizeof(buffer)) {
		return FALSE;
	}
	if (!isValidElf(hdr))
		return FALSE;

	len=0;
	openNode->access->seek(openNode, hdr->e_phoff);	
	for (i=0;i<hdr->e_phnum;i++) {
		struct Elf32_Phdr phdr;		
		read(openNode->handle, &phdr, sizeof(struct Elf32_Phdr));
		if (phdr.p_type==PT_LOAD) {
			if (phdr.p_paddr<address)
				address=phdr.p_paddr;
			if (len<phdr.p_paddr+phdr.p_memsz)
				len=phdr.p_paddr+phdr.p_memsz;
		}
	}
#ifdef USE_MMU
	if (address>0x10000) {
		reloc = 0;
		len-=address;
	} else {
		reloc = ADDRESS_PROCESS_LOADER<<PAGE_SHIFT;
		address = reloc;
	}
	flags |= K_MAP_FIXED;
#endif
	address = syscall_mmap64(thread, address, len+BRK_EXTRA, K_PROT_READ | K_PROT_WRITE | K_PROT_EXEC, flags, -1, 0);
#ifndef USE_MMU
	reloc = address;
#endif
	process->loaderBaseAddress = address;
	process->brkEnd = address+len;
	process->phdr = 0;

	for (i=0;i<hdr->e_phnum;i++) {
		struct Elf32_Phdr phdr;		
		openNode->access->seek(openNode, hdr->e_phoff+hdr->e_phentsize*i);
		read(openNode->handle, &phdr, sizeof(struct Elf32_Phdr));
		if (phdr.p_type==PT_LOAD) {
			if (phdr.p_filesz>0) {
				if (phdr.p_offset<=hdr->e_phoff && hdr->e_phoff<phdr.p_offset+phdr.p_filesz) {
					process->phdr = reloc+phdr.p_paddr+hdr->e_phoff-phdr.p_offset;
				}
				openNode->access->seek(openNode, phdr.p_offset);
				openNode->access->read(MMU_PARAM_THREAD openNode, reloc+phdr.p_paddr, phdr.p_filesz);		
			}
		}
	}
	process->phentsize=hdr->e_phentsize;
	process->phnum=hdr->e_phnum;

	*eip = hdr->e_entry+reloc;
	process->entry = *eip; 
	return TRUE;
}

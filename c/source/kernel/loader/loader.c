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

const char* getInterpreter(struct OpenNode* openNode, BOOL* isElf) {
	U8 buffer[sizeof(struct Elf32_Ehdr)];
	struct Elf32_Ehdr* hdr = (struct Elf32_Ehdr*)buffer;
	U32 len = read(openNode->handle, buffer, sizeof(buffer));

	if (len!=sizeof(buffer)) {
		*isElf=FALSE;
	}
	if (*isElf) {		
		*isElf = isValidElf(hdr);
	}
	if (!*isElf) {
		if (!strncmp((char*)buffer, "#!/bin/sh", 9)) {
			return "/bin/sh";
		}
	}
	return 0;
}

#define SHF_WRITE 1
#define SHF_ALLOC 2

#define SHT_NOBITS 8

#define PT_LOAD 1

BOOL loadProgram(struct KProcess* process, struct KThread* thread, struct OpenNode* openNode, U32* eip) {
	U8 buffer[sizeof(struct Elf32_Ehdr)];
	struct Elf32_Ehdr* hdr = (struct Elf32_Ehdr*)buffer;
	U32 len = read(openNode->handle, buffer, sizeof(buffer));
	U32 address=0xFFFFFFFF;
	U32 i;

	if (len!=sizeof(buffer)) {
		return FALSE;
	}
	if (!isValidElf(hdr))
		return FALSE;

	len=0;
	openNode->access->seek(openNode, hdr->e_phoff);	
	for (i=0;i<hdr->e_phoff;i++) {
		struct Elf32_Phdr phdr;		
		read(openNode->handle, &phdr, sizeof(struct Elf32_Phdr));
		if (phdr.p_type==PT_LOAD) {
			if (phdr.p_paddr<address)
				address=phdr.p_paddr;
			if (len<phdr.p_paddr+phdr.p_memsz)
				len=phdr.p_paddr+phdr.p_memsz;
		}
	}

	address = mmap64(thread, 0, len, K_PROT_READ | K_PROT_WRITE | K_PROT_EXEC, K_MAP_PRIVATE|K_MAP_ANONYMOUS, -1, 0);
	process->brkEnd = address+len;
	for (i=0;i<hdr->e_phnum;i++) {
		struct Elf32_Phdr phdr;		
		openNode->access->seek(openNode, hdr->e_phoff+sizeof(struct Elf32_Phdr)*i);
		read(openNode->handle, &phdr, sizeof(struct Elf32_Phdr));
		if (phdr.p_type==PT_LOAD) {
			if (phdr.p_filesz>0) {
				openNode->access->seek(openNode, phdr.p_offset);
				openNode->access->read(thread->process->memory, openNode, address+phdr.p_paddr, phdr.p_filesz);		
			}
		}
	}
	
	*eip = hdr->e_entry+address;
	return TRUE;
}

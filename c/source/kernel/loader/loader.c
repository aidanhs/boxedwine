#include "platform.h"
#include "nodeaccess.h"
#include "elf.h"
#include "filesystem.h"
#include "nodeaccess.h"
#include "node.h"
#include "nodeType.h"
#include "kmmap.h"
#include "kthread.h"
#include "kprocess.h"

#include <string.h>

#include UNISTD

const char* getInterpreter(OpenNode* openNode, BOOL* isElf) {
	U8 buffer[sizeof(Elf32_Ehdr)];
	Elf32_Ehdr* hdr = (Elf32_Ehdr*)buffer;
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

BOOL loadProgram(KThread* thread, OpenNode* openNode, U32* eip) {
	U8 buffer[sizeof(Elf32_Ehdr)];
	Elf32_Ehdr* hdr = (Elf32_Ehdr*)buffer;
	U32 len = read(openNode->handle, buffer, sizeof(buffer));
	U32 address;

	if (len!=sizeof(buffer)) {
		return FALSE;
	}
	if (!isValidElf(hdr))
		return FALSE;
	len = (U32)openNode->access->length(openNode);
	address = mmap64(thread, 0, len, K_PROT_READ | K_PROT_WRITE | K_PROT_EXEC, K_MAP_PRIVATE|K_MAP_ANONYMOUS, -1, 0);
	openNode->access->seek(openNode, 0);
	openNode->access->read(thread->process->memory, openNode, address, len);	
	*eip = hdr->e_entry+address;
	return TRUE;
}
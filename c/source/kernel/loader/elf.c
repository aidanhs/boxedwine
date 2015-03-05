#include "elf.h"

BOOL isValidElf(Elf32_Ehdr* hdr) {
    if (hdr->e_ident[0] != 0x7F || hdr->e_ident[1] != 'E' || hdr->e_ident[2] != 'L' || hdr->e_ident[3] != 'F') {
        return FALSE;
    }
    if (hdr->e_ident[4] != 1) {
        return FALSE;
    }
    if (hdr->e_ident[5] != 1) {
        return FALSE;
    }
    return TRUE;
}


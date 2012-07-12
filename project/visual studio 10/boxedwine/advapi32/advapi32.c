#include "config.h"
#include "wine/port.h"

#include <stdlib.h>
#include "windef.h"
#include "winbase.h"
#include "winternl.h"

char* __wine_spec_file_name="ADVAPI32.DLL";
IMAGE_NT_HEADERS __wine_spec_nt_header;

void setImageNtHeaders(void* p) {
	memcpy(&__wine_spec_nt_header, p, sizeof(IMAGE_NT_HEADERS));
}

void _fini() {
}
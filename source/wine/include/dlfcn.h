#ifndef __DLFCN_H__
#define __DLFCN_H__

#define RTLD_DEFAULT ((void *) -2)
#define RTLD_LAZY	1	/* lazy function call binding */
#define RTLD_NOW	2	/* immediate function call binding */
#define RTLD_GLOBAL	0x100	/* allow symbols to be global */

DOSHAL char *dlerror(void);
DOSHAL void *dlopen(const char *file, int mode);
DOSHAL void *dlsym(void* handle, const char* name);
DOSHAL int dlclose(void *handle);

#endif
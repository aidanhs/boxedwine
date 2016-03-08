#ifndef __DLFCN_H__
#define __DLFCN_H__

#define RTLD_DEFAULT ((void *) -2)
#define RTLD_LAZY	1	/* lazy function call binding */
#define RTLD_NOW	2	/* immediate function call binding */
#define RTLD_GLOBAL	0x100	/* allow symbols to be global */

char *winecrt_dlerror(void);
#define dlerror winecrt_dlerror

void *winecrt_dlopen(const char *file, int mode);
#define dlopen winecrt_dlopen

void *winecrt_dlsym(void* handle, const char* name);
#define dlsym winecrt_dlsym

int winecrt_dlclose(void *handle);
#define dlclose winecrt_dlclose

#endif
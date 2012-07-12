#include <Windows.h>

char *hal_dlerror(void) 
{
	return 0;
}

typedef void(*setImageNtHeaders)(void* p);
void *hal_dlopen(const char *file, int mode)
{
	void* result = LoadLibraryA(file);
	if (result) {
		setImageNtHeaders set = (setImageNtHeaders)GetProcAddress((HMODULE)result, "setImageNtHeaders");
		if (set)
		{
			IMAGE_DOS_HEADER* doshdr=(IMAGE_DOS_HEADER*)result;
			set((char*)result+doshdr->e_lfanew);
		}
	} else {
		LPTSTR errorText = NULL;

		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorText, 0, NULL);

		if (errorText) {
			OutputDebugString(errorText);
			LocalFree(errorText);	
		}
	}
	return result;
}

void *hal_dlsym(void* handle, const char* name)
{
	return GetProcAddress((HMODULE)handle, name);
}

int hal_dlclose(void *handle)
{
	return 0;
}
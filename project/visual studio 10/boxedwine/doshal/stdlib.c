#include <windows.h>

void hal_exit(int status) 
{
	ExitProcess(status);
}

int hal_unsetenv(const char *name)
{
	return SetEnvironmentVariable(name, NULL);
}

char* hal_getenv(const char * name)
{
	return getenv(name);
}

int hal_putenv(char *string)
{
	return putenv(string);
}

void hal_abort(void)
{
	ExitProcess(-1);
}

int hal_system(const char *command)
{
	return 0;
}

void *hal_malloc(size_t size)
{
	return malloc(size);
}

void hal_free(void *ptr)
{
	free(ptr);
}

void *hal_realloc(void *ptr, size_t size)
{
	return realloc(ptr, size);
}
#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>
int server_main(int argc, char *argv[]);
void initProcess();
void initSystem(const char* root);

DWORD WINAPI serverThread(LPVOID lpParameter) {
    char* argv[] ={"/home/boxedwine/wineserver"};
    initProcess();
    return server_main(1, argv);
}
char* winecrt_getenv(const char * name);

IMAGE_NT_HEADERS winelib_headers;
IMAGE_NT_HEADERS winecrt_headers;
IMAGE_NT_HEADERS winapi32_headers;
IMAGE_NT_HEADERS user32_headers;

void initModule(HMODULE module, IMAGE_NT_HEADERS* nt, const char* name) {
    IMAGE_DOS_HEADER* pDOSHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* pNTHeaders = (IMAGE_NT_HEADERS*)((BYTE*)pDOSHeader + pDOSHeader->e_lfanew);
    *nt = *pNTHeaders;
    __wine_dll_register(nt, name);
}
int main(int argc, char *argv[]) {
    char error[1024];

	initSystem("c:\\root");
	initProcess();
    CreateThread(NULL, 0, serverThread, 0, 0, NULL);
    Sleep(100);
    // this hack is just so I can load the libaries such that I can line debug them in visual studio
    initModule(GetModuleHandleA("winecrt.dll"), &winecrt_headers, "winecrt.dll");
    initModule(GetModuleHandleA("winelib.dll"), &winelib_headers, "winelib.dll");    
    initModule(LoadLibraryExA("C:\\Source\\boxedwine_current\\c4\\project\\vs2015\\BoxedWine\\Debug\\advapi32.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH), &winapi32_headers, "advapi32.dll");
    initModule(LoadLibraryExA("C:\\Source\\boxedwine_current\\c4\\project\\vs2015\\BoxedWine\\Debug\\user32.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH), &user32_headers, "user32.dll");
    //
    wine_init(argc, argv, error, sizeof(error));
}
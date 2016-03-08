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
int main(int argc, char *argv[]) {
	WSADATA wsaData;

	if (WSAStartup(0x202, &wsaData) != 0) {
		printf("ERROR: Initialization failure.\n");
	} else {
        char error[1024];
		initSystem("c:\\root");
		initProcess();
        CreateThread(NULL, 0, serverThread, 0, 0, NULL);
        Sleep(100);
        wine_init(argc, argv, error, sizeof(error));
		WSACleanup();
	}
}
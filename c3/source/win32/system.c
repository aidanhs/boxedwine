#include <string.h>

extern char rootDirectory[1024];

void getFullNativePath(const char* currentDirectory, const char* localFile, char* nativeFile) {
    strcpy(nativeFile, rootDirectory);

    if (localFile[1]==':')
        strcat(nativeFile, localFile+3);
    else if (localFile[0]=='\\' || localFile[1]=='/')
        strcat(nativeFile, localFile+1);
    else {
        strcat(nativeFile, currentDirectory+3);
        strcat(nativeFile, localFile);
    }    
}
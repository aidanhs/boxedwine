#include <string.h>
#include "platform.h"

void splitIntoDirAndName(const char* path, char* dir, char* name) {
    char* pos = strrchr(path, '\\');
    if (pos==0) {
       pos = strrchr(path, '/');
    }
    if (pos==0) {
        dir[0] = 0;
        strcpy(name, path);
    } else {
        strcpy(name, pos+1);
        strcpy(dir, path);
        dir[pos-path+1] = 0;
    }
}
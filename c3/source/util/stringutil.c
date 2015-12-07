#include <ctype.h>

void convertStringToLower(const char* str, char* lower) {
    while (*str) {
        *lower = tolower(*str);
        str++;
        lower++;
    }
    *lower = 0;
}
#include "../include/winbase.h"

UINT WINAPI GetDriveTypeW_k(LPCWSTR root) {
    return DRIVE_FIXED;
}

UINT WINAPI GetDriveTypeA_k( LPCSTR root ) {
    return DRIVE_FIXED;
}
#include "windef.h"
#include "winbase.h"
#include "winuser.h"

int _fltused=1;
LRESULT WINPROC_wrapper( WNDPROC proc, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    return proc( hwnd, msg, wParam, lParam );
}
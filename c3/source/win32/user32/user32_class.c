/*
 * Window classes functions
 *
 * Copyright 1993, 1996, 2003 Alexandre Julliard
 * Copyright 1998 Juergen Schmied (jsch)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "../include/winbase.h"
#include "../include/unicode.h"
#include "win.h"
#include "user_private.h"
#include "../include/list.h"
#include "controls.h"
#include "kprocess.h"

#define MAX_ATOM_LEN 255 /* from dlls/kernel32/atom.c */

typedef struct tagCLASS
{
    UINT             style;         /* Class style */
    BOOL             local;         /* Local class? */
    WNDPROC          winproc;       /* Window procedure */
    INT              cbClsExtra;    /* Class extra bytes */
    INT              cbWndExtra;    /* Window extra bytes */
    LPWSTR           menuName;      /* Default menu name (Unicode followed by ASCII) */
    struct dce      *dce;           /* Opaque pointer to class DCE */
    HINSTANCE        hInstance;     /* Module that created the task */
    HICON            hIcon;         /* Default icon */
    HICON            hIconSm;       /* Default small icon */
    HICON            hIconSmIntern; /* Internal small icon, derived from hIcon */
    HCURSOR          hCursor;       /* Default cursor */
    HBRUSH           hbrBackground; /* Default background */
    ATOM             atomName;      /* Name of the class */
    WCHAR            name[MAX_ATOM_LEN + 1];
} CLASS;

/***********************************************************************
 *           get_class_ptr
 */
static CLASS *get_class_ptr( HWND hwnd, BOOL write_access )
{
    WND *ptr = WIN_GetPtr( hwnd );

    if (ptr)
    {
        return ptr->class;
    }
    SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
    return NULL;
}


/***********************************************************************
 *           get_int_atom_value
 */
ATOM get_int_atom_value( LPCWSTR name )
{
    UINT ret = 0;

    if (IS_INTRESOURCE(name)) return LOWORD(name);
    if (*name++ != '#') return 0;
    while (*name)
    {
        if (*name < '0' || *name > '9') return 0;
        ret = ret * 10 + *name++ - '0';
        if (ret > 0xffff) return 0;
    }
    return ret;
}

/***********************************************************************
 *           CLASS_GetMenuNameA
 *
 * Get the menu name as a ASCII string.
 */
static INLINE LPSTR CLASS_GetMenuNameA( CLASS *classPtr )
{
    if (IS_INTRESOURCE(classPtr->menuName)) return (LPSTR)classPtr->menuName;
    return (LPSTR)(classPtr->menuName + strlenW(classPtr->menuName) + 1);
}


/***********************************************************************
 *           CLASS_GetMenuNameW
 *
 * Get the menu name as a Unicode string.
 */
static INLINE LPWSTR CLASS_GetMenuNameW( CLASS *classPtr )
{
    return classPtr->menuName;
}


/***********************************************************************
 *           CLASS_SetMenuNameA
 *
 * Set the menu name in a class structure by copying the string.
 */
static void CLASS_SetMenuNameA( CLASS *classPtr, LPCSTR name )
{
    if (!IS_INTRESOURCE(classPtr->menuName)) HeapFree_k( GetProcessHeap_k(), 0, classPtr->menuName );
    if (!IS_INTRESOURCE(name))
    {
        DWORD lenA = strlen(name) + 1;
        DWORD lenW = MultiByteToWideChar_k( CP_ACP, 0, name, lenA, NULL, 0 );
        classPtr->menuName = HeapAlloc_k( GetProcessHeap_k(), 0, lenA + lenW*sizeof(WCHAR) );
        MultiByteToWideChar_k( CP_ACP, 0, name, lenA, classPtr->menuName, lenW );
        memcpy( classPtr->menuName + lenW, name, lenA );
    }
    else classPtr->menuName = (LPWSTR)name;
}


/***********************************************************************
 *           CLASS_SetMenuNameW
 *
 * Set the menu name in a class structure by copying the string.
 */
static void CLASS_SetMenuNameW( CLASS *classPtr, LPCWSTR name )
{
    if (!IS_INTRESOURCE(classPtr->menuName)) HeapFree_k( GetProcessHeap_k(), 0, classPtr->menuName );
    if (!IS_INTRESOURCE(name))
    {
        DWORD lenW = strlenW(name) + 1;
        DWORD lenA = WideCharToMultiByte_k( CP_ACP, 0, name, lenW, NULL, 0, NULL, NULL );
        classPtr->menuName = HeapAlloc_k( GetProcessHeap_k(), 0, lenA + lenW*sizeof(WCHAR) );
        memcpy( classPtr->menuName, name, lenW*sizeof(WCHAR) );
        WideCharToMultiByte_k( CP_ACP, 0, name, lenW,
                             (char *)(classPtr->menuName + lenW), lenA, NULL, NULL );
    }
    else classPtr->menuName = (LPWSTR)name;
}


/***********************************************************************
 *           CLASS_FreeClass
 *
 * Free a class structure.
 */
static void CLASS_FreeClass( CLASS *classPtr )
{
    struct KProcess* process = currentProcess();
    int index = pblListIndexOf(process->classes, classPtr);
    TRACE("%p\n", classPtr);

    if (classPtr->dce) free_dce( classPtr->dce, 0 );
    pblListRemoveAt(process->classes, index);    
    if (classPtr->hbrBackground > (HBRUSH)(COLOR_GRADIENTINACTIVECAPTION + 1))
        DeleteObject_k( classPtr->hbrBackground );
    HeapFree_k( GetProcessHeap_k(), 0, classPtr->menuName );
    HeapFree_k( GetProcessHeap_k(), 0, classPtr );
}


/***********************************************************************
 *           CLASS_FindClass
 *
 * Return a pointer to the class.
 * hinstance has been normalized by the caller.
 */
CLASS *CLASS_FindClass( LPCWSTR name, HINSTANCE hinstance )
{
    ATOM atom = get_int_atom_value( name );
    struct KProcess* process = currentProcess();
    PblIterator* it = pblListIterator(process->classes);

    while (it && pblIteratorHasNext(it)) {
        PblMapEntry* entry = pblIteratorNext(it);
        CLASS *class = pblMapEntryValue(entry);
        if (atom)
        {
            if (class->atomName != atom) continue;
        }
        else
        {
            if (!name || strcmpiW( class->name, name )) continue;
        }
        if (!hinstance || !class->local || class->hInstance == hinstance)
        {
            TRACE("%s %p -> %p\n", debugstr_w(name), hinstance, class);
            return class;
        }
    }
    pblIteratorFree(it);
    TRACE("%s %p -> not found\n", debugstr_w(name), hinstance);
    return NULL;
}


/***********************************************************************
 *           CLASS_RegisterClass
 *
 * The real RegisterClass() functionality.
 */
static CLASS *CLASS_RegisterClass( LPCWSTR name, HINSTANCE hInstance, BOOL local,
                                   DWORD style, INT classExtra, INT winExtra )
{
    CLASS *classPtr;
    struct KProcess* process = currentProcess();

    TRACE("name=%s hinst=%p style=0x%x clExtr=0x%x winExtr=0x%x\n",
          debugstr_w(name), hInstance, style, classExtra, winExtra );

    /* Fix the extra bytes value */

    if (classExtra > 40)  /* Extra bytes are limited to 40 in Win32 */
        WARN("Class extra bytes %d is > 40\n", classExtra);
    if (winExtra > 40)    /* Extra bytes are limited to 40 in Win32 */
        WARN("Win extra bytes %d is > 40\n", winExtra );

    classPtr = HeapAlloc_k( GetProcessHeap_k(), HEAP_ZERO_MEMORY, sizeof(CLASS) + classExtra );
    if (!classPtr) return NULL;

    classPtr->atomName = get_int_atom_value( name );
    if (!classPtr->atomName && name) strcpyW( classPtr->name, name );
    else GlobalGetAtomNameW_k( classPtr->atomName, classPtr->name, sizeof(classPtr->name)/sizeof(WCHAR) );

    classPtr->style       = style;
    classPtr->local       = local;
    classPtr->cbWndExtra  = winExtra;
    classPtr->cbClsExtra  = classExtra;
    classPtr->hInstance   = hInstance;

    /* Other non-null values must be set by caller */

    if (local) pblListAddFirst(process->classes,  classPtr);
    else pblListAddLast(process->classes,  classPtr);
    return classPtr;
}


/***********************************************************************
 *           register_builtin
 *
 * Register a builtin control class.
 * This allows having both ASCII and Unicode winprocs for the same class.
 */
static void register_builtin( const struct builtin_class_descr *descr )
{
    CLASS *classPtr;

    if (!(classPtr = CLASS_RegisterClass( descr->name, user32_module, FALSE,
                                          descr->style, 0, descr->extra ))) return;

    classPtr->hCursor       = LoadCursorA_k( 0, (LPSTR)descr->cursor );
    classPtr->hbrBackground = descr->brush;
    classPtr->winproc       = BUILTIN_WINPROC( descr->proc );
}


/***********************************************************************
 *           CLASS_RegisterBuiltinClasses
 */
void CLASS_RegisterBuiltinClasses(void)
{
    register_builtin( &DESKTOP_builtin_class );
    register_builtin( &BUTTON_builtin_class );
    register_builtin( &COMBO_builtin_class );
    register_builtin( &COMBOLBOX_builtin_class );
    register_builtin( &DIALOG_builtin_class );
    register_builtin( &EDIT_builtin_class );
    register_builtin( &ICONTITLE_builtin_class );
    register_builtin( &LISTBOX_builtin_class );
    register_builtin( &MDICLIENT_builtin_class );
    register_builtin( &MENU_builtin_class );
    register_builtin( &MESSAGE_builtin_class );
    register_builtin( &SCROLL_builtin_class );
    register_builtin( &STATIC_builtin_class );
}


/***********************************************************************
 *           get_class_winproc
 */
WNDPROC get_class_winproc( CLASS *class )
{
    return class->winproc;
}


/***********************************************************************
 *           get_class_dce
 */
struct dce *get_class_dce( CLASS *class )
{
    return class->dce;
}


/***********************************************************************
 *           set_class_dce
 */
struct dce *set_class_dce( CLASS *class, struct dce *dce )
{
    if (class->dce) return class->dce;  /* already set, don't change it */
    class->dce = dce;
    return dce;
}


/***********************************************************************
 *		RegisterClassA (USER32.@)
 *
 * Register a window class.
 *
 * RETURNS
 *	>0: Unique identifier
 *	0: Failure
 */
ATOM WINAPI RegisterClassA_k( const WNDCLASSA* wc ) /* [in] Address of structure with class data */
{
    WNDCLASSEXA wcex;

    wcex.cbSize        = sizeof(wcex);
    wcex.style         = wc->style;
    wcex.lpfnWndProc   = wc->lpfnWndProc;
    wcex.cbClsExtra    = wc->cbClsExtra;
    wcex.cbWndExtra    = wc->cbWndExtra;
    wcex.hInstance     = wc->hInstance;
    wcex.hIcon         = wc->hIcon;
    wcex.hCursor       = wc->hCursor;
    wcex.hbrBackground = wc->hbrBackground;
    wcex.lpszMenuName  = wc->lpszMenuName;
    wcex.lpszClassName = wc->lpszClassName;
    wcex.hIconSm       = 0;
    return RegisterClassExA_k( &wcex );
}


/***********************************************************************
 *		RegisterClassW (USER32.@)
 *
 * See RegisterClassA.
 */
ATOM WINAPI RegisterClassW_k( const WNDCLASSW* wc )
{
    WNDCLASSEXW wcex;

    wcex.cbSize        = sizeof(wcex);
    wcex.style         = wc->style;
    wcex.lpfnWndProc   = wc->lpfnWndProc;
    wcex.cbClsExtra    = wc->cbClsExtra;
    wcex.cbWndExtra    = wc->cbWndExtra;
    wcex.hInstance     = wc->hInstance;
    wcex.hIcon         = wc->hIcon;
    wcex.hCursor       = wc->hCursor;
    wcex.hbrBackground = wc->hbrBackground;
    wcex.lpszMenuName  = wc->lpszMenuName;
    wcex.lpszClassName = wc->lpszClassName;
    wcex.hIconSm       = 0;
    return RegisterClassExW_k( &wcex );
}


/***********************************************************************
 *		RegisterClassExA (USER32.@)
 */
ATOM WINAPI RegisterClassExA_k( const WNDCLASSEXA* wc )
{
    ATOM atom;
    CLASS *classPtr;
    HINSTANCE instance;

    if (wc->cbSize != sizeof(*wc) || wc->cbClsExtra < 0 || wc->cbWndExtra < 0 ||
        wc->hInstance == user32_module)  /* we can't register a class for user32 */
    {
         SetLastError_k( ERROR_INVALID_PARAMETER );
         return 0;
    }
    if (!(instance = wc->hInstance)) instance = GetModuleHandleW_k( NULL );

    if (!IS_INTRESOURCE(wc->lpszClassName))
    {
        WCHAR name[MAX_ATOM_LEN + 1];

        if (!MultiByteToWideChar_k( CP_ACP, 0, wc->lpszClassName, -1, name, MAX_ATOM_LEN + 1 )) return 0;
        classPtr = CLASS_RegisterClass( name, instance, !(wc->style & CS_GLOBALCLASS),
                                        wc->style, wc->cbClsExtra, wc->cbWndExtra );
    }
    else
    {
        classPtr = CLASS_RegisterClass( (LPCWSTR)wc->lpszClassName, instance,
                                        !(wc->style & CS_GLOBALCLASS), wc->style,
                                        wc->cbClsExtra, wc->cbWndExtra );
    }
    if (!classPtr) return 0;
    atom = classPtr->atomName;

    TRACE("name=%s atom=%04x wndproc=%p hinst=%p bg=%p style=%08x clsExt=%d winExt=%d class=%p\n",
          debugstr_a(wc->lpszClassName), atom, wc->lpfnWndProc, instance, wc->hbrBackground,
          wc->style, wc->cbClsExtra, wc->cbWndExtra, classPtr );

    classPtr->hIcon         = wc->hIcon;
    classPtr->hIconSm       = wc->hIconSm;
    classPtr->hIconSmIntern = wc->hIcon && !wc->hIconSm ?
                                            CopyImage_k( wc->hIcon, IMAGE_ICON,
                                                GetSystemMetrics_k( SM_CXSMICON ),
                                                GetSystemMetrics_k( SM_CYSMICON ), 0 ) : NULL;
    classPtr->hCursor       = wc->hCursor;
    classPtr->hbrBackground = wc->hbrBackground;
    classPtr->winproc       = WINPROC_AllocProc( wc->lpfnWndProc, FALSE );
    CLASS_SetMenuNameA( classPtr, wc->lpszMenuName );
    return atom;
}


/***********************************************************************
 *		RegisterClassExW (USER32.@)
 */
ATOM WINAPI RegisterClassExW_k( const WNDCLASSEXW* wc )
{
    ATOM atom;
    CLASS *classPtr;
    HINSTANCE instance;

    if (wc->cbSize != sizeof(*wc) || wc->cbClsExtra < 0 || wc->cbWndExtra < 0 ||
        wc->hInstance == user32_module)  /* we can't register a class for user32 */
    {
         SetLastError_k( ERROR_INVALID_PARAMETER );
         return 0;
    }
    if (!(instance = wc->hInstance)) instance = GetModuleHandleW_k( NULL );

    if (!(classPtr = CLASS_RegisterClass( wc->lpszClassName, instance, !(wc->style & CS_GLOBALCLASS),
                                          wc->style, wc->cbClsExtra, wc->cbWndExtra )))
        return 0;

    atom = classPtr->atomName;

    TRACE("name=%s atom=%04x wndproc=%p hinst=%p bg=%p style=%08x clsExt=%d winExt=%d class=%p\n",
          debugstr_w(wc->lpszClassName), atom, wc->lpfnWndProc, instance, wc->hbrBackground,
          wc->style, wc->cbClsExtra, wc->cbWndExtra, classPtr );

    classPtr->hIcon         = wc->hIcon;
    classPtr->hIconSm       = wc->hIconSm;
    classPtr->hIconSmIntern = wc->hIcon && !wc->hIconSm ?
                                            CopyImage_k( wc->hIcon, IMAGE_ICON,
                                                GetSystemMetrics_k( SM_CXSMICON ),
                                                GetSystemMetrics_k( SM_CYSMICON ), 0 ) : NULL;
    classPtr->hCursor       = wc->hCursor;
    classPtr->hbrBackground = wc->hbrBackground;
    classPtr->winproc       = WINPROC_AllocProc( wc->lpfnWndProc, TRUE );
    CLASS_SetMenuNameW( classPtr, wc->lpszMenuName );
    return atom;
}


/***********************************************************************
 *		UnregisterClassA (USER32.@)
 */
BOOL WINAPI UnregisterClassA_k( LPCSTR className, HINSTANCE hInstance )
{
    if (!IS_INTRESOURCE(className))
    {
        WCHAR name[MAX_ATOM_LEN + 1];

        if (!MultiByteToWideChar_k( CP_ACP, 0, className, -1, name, MAX_ATOM_LEN + 1 ))
            return FALSE;
        return UnregisterClassW_k( name, hInstance );
    }
    return UnregisterClassW_k( (LPCWSTR)className, hInstance );
}

/***********************************************************************
 *		UnregisterClassW (USER32.@)
 */
BOOL WINAPI UnregisterClassW_k( LPCWSTR className, HINSTANCE hInstance )
{
    CLASS *classPtr = CLASS_FindClass(className, hInstance);

    if (classPtr) CLASS_FreeClass( classPtr );
    return (classPtr != NULL);
}


/***********************************************************************
 *		GetClassWord (USER32.@)
 */
WORD WINAPI GetClassWord_k( HWND hwnd, INT offset )
{
    CLASS *class;
    WORD retvalue = 0;

    if (offset < 0) return (WORD)GetClassLongA_k( hwnd, offset );

    if (!(class = get_class_ptr( hwnd, FALSE ))) return 0;

    if (offset <= class->cbClsExtra - (INT)sizeof(WORD))
        memcpy( &retvalue, (char *)(class + 1) + offset, sizeof(retvalue) );
    else
        SetLastError_k( ERROR_INVALID_INDEX );
    return retvalue;
}


/***********************************************************************
 *             CLASS_GetClassLong
 *
 * Implementation of GetClassLong(Ptr)A/W
 */
static ULONG_PTR CLASS_GetClassLong( HWND hwnd, INT offset, UINT size,
                                     BOOL unicode )
{
    CLASS *class;
    ULONG_PTR retvalue = 0;

    if (!(class = get_class_ptr( hwnd, FALSE ))) return 0;

    if (offset >= 0)
    {
        if (offset <= class->cbClsExtra - (INT)size)
        {
            if (size == sizeof(DWORD))
            {
                DWORD retdword;
                memcpy( &retdword, (char *)(class + 1) + offset, sizeof(DWORD) );
                retvalue = retdword;
            }
            else
                memcpy( &retvalue, (char *)(class + 1) + offset, sizeof(ULONG_PTR) );
        }
        else
            SetLastError_k( ERROR_INVALID_INDEX );
        return retvalue;
    }

    switch(offset)
    {
    case GCLP_HBRBACKGROUND:
        retvalue = (ULONG_PTR)class->hbrBackground;
        break;
    case GCLP_HCURSOR:
        retvalue = (ULONG_PTR)class->hCursor;
        break;
    case GCLP_HICON:
        retvalue = (ULONG_PTR)class->hIcon;
        break;
    case GCLP_HICONSM:
        retvalue = (ULONG_PTR)(class->hIconSm ? class->hIconSm : class->hIconSmIntern);
        break;
    case GCL_STYLE:
        retvalue = class->style;
        break;
    case GCL_CBWNDEXTRA:
        retvalue = class->cbWndExtra;
        break;
    case GCL_CBCLSEXTRA:
        retvalue = class->cbClsExtra;
        break;
    case GCLP_HMODULE:
        retvalue = (ULONG_PTR)class->hInstance;
        break;
    case GCLP_WNDPROC:
        retvalue = (ULONG_PTR)WINPROC_GetProc( class->winproc, unicode );
        break;
    case GCLP_MENUNAME:
        retvalue = (ULONG_PTR)CLASS_GetMenuNameW( class );
        if (unicode)
            retvalue = (ULONG_PTR)CLASS_GetMenuNameW( class );
        else
            retvalue = (ULONG_PTR)CLASS_GetMenuNameA( class );
        break;
    case GCW_ATOM:
        retvalue = class->atomName;
        break;
    default:
        SetLastError_k( ERROR_INVALID_INDEX );
        break;
    }
    return retvalue;
}


/***********************************************************************
 *		GetClassLongW (USER32.@)
 */
DWORD WINAPI GetClassLongW_k( HWND hwnd, INT offset )
{
    return CLASS_GetClassLong( hwnd, offset, sizeof(DWORD), TRUE );
}



/***********************************************************************
 *		GetClassLongA (USER32.@)
 */
DWORD WINAPI GetClassLongA_k( HWND hwnd, INT offset )
{
    return CLASS_GetClassLong( hwnd, offset, sizeof(DWORD), FALSE );
}


/***********************************************************************
 *		SetClassWord (USER32.@)
 */
WORD WINAPI SetClassWord_k( HWND hwnd, INT offset, WORD newval )
{
    CLASS *class;
    WORD retval = 0;

    if (offset < 0) return (WORD)SetClassLongA_k( hwnd, offset, (DWORD)newval );

    if (!(class = get_class_ptr( hwnd, TRUE ))) return 0;

    if (offset > class->cbClsExtra - 2)
    {
        SetLastError_k( ERROR_INVALID_INDEX );
        return 0;
    }
    memcpy( &retval, class+sizeof(CLASS) + offset, 2);
    memcpy( class+sizeof(CLASS) + offset, &newval, 2);

    return retval;
}


/***********************************************************************
 *             CLASS_SetClassLong
 *
 * Implementation of SetClassLong(Ptr)A/W
 */
static ULONG_PTR CLASS_SetClassLong( HWND hwnd, INT offset, LONG_PTR newval,
                                     UINT size, BOOL unicode )
{
    CLASS *class;
    ULONG_PTR retval = 0;

    if (!(class = get_class_ptr( hwnd, TRUE ))) return 0;

    if (offset >= 0)
    {
        if (offset+(INT)size<class->cbClsExtra)
        {
            void *ptr = (char *)(class + 1) + offset;
            if ( size == sizeof(LONG) )
            {
                DWORD retdword;
                LONG newlong = newval;
                memcpy( &retdword, ptr, sizeof(DWORD) );
                memcpy( ptr, &newlong, sizeof(LONG) );
                retval = retdword;
            }
            else
            {
                memcpy( &retval, ptr, sizeof(ULONG_PTR) );
                memcpy( ptr, &newval, sizeof(LONG_PTR) );
            }
        }
    }
    else switch(offset)
    {
    case GCLP_MENUNAME:
        if ( unicode )
            CLASS_SetMenuNameW( class, (LPCWSTR)newval );
        else
            CLASS_SetMenuNameA( class, (LPCSTR)newval );
        retval = 0;  /* Old value is now meaningless anyway */
        break;
    case GCLP_WNDPROC:
        retval = (ULONG_PTR)WINPROC_GetProc( class->winproc, unicode );
        class->winproc = WINPROC_AllocProc( (WNDPROC)newval, unicode );
        break;
    case GCLP_HBRBACKGROUND:
        retval = (ULONG_PTR)class->hbrBackground;
        class->hbrBackground = (HBRUSH)newval;
        break;
    case GCLP_HCURSOR:
        retval = (ULONG_PTR)class->hCursor;
        class->hCursor = (HCURSOR)newval;
        break;
    case GCLP_HICON:
        retval = (ULONG_PTR)class->hIcon;
        if (retval && class->hIconSmIntern)
        {
            DestroyIcon_k(class->hIconSmIntern);
            class->hIconSmIntern = NULL;
        }
        if (newval && !class->hIconSm)
            class->hIconSmIntern = CopyImage_k( (HICON)newval, IMAGE_ICON,
                      GetSystemMetrics_k( SM_CXSMICON ), GetSystemMetrics_k( SM_CYSMICON ), 0 );
        class->hIcon = (HICON)newval;
        break;
    case GCLP_HICONSM:
        retval = (ULONG_PTR)class->hIconSm;
        if (retval && !newval)
            class->hIconSmIntern = class->hIcon ? CopyImage_k( class->hIcon, IMAGE_ICON,
                                                GetSystemMetrics_k( SM_CXSMICON ),
                                                GetSystemMetrics_k( SM_CYSMICON ), 0 ) : NULL;
        else if (!retval && newval && class->hIconSmIntern)
        {
            DestroyIcon_k(class->hIconSmIntern);
            class->hIconSmIntern = NULL;
        }
        class->hIconSm = (HICON)newval;
        break;
    case GCL_STYLE:
        retval = class->style;
        class->style = newval;
        break;
    case GCL_CBWNDEXTRA:
        retval = class->cbWndExtra;
        class->cbWndExtra = newval;
        break;
    case GCLP_HMODULE:
        retval = (ULONG_PTR)class->hInstance;
        class->hInstance = (HINSTANCE)newval;
        break;
    case GCW_ATOM:
        retval = class->atomName;
        class->atomName = (ATOM)newval;
        GlobalGetAtomNameW_k( (ATOM)newval, class->name, sizeof(class->name)/sizeof(WCHAR) );
        break;
    case GCL_CBCLSEXTRA:  /* cannot change this one */
        SetLastError_k( ERROR_INVALID_PARAMETER );
        break;
    default:
        SetLastError_k( ERROR_INVALID_INDEX );
        break;
    }
    return retval;
}


/***********************************************************************
 *		SetClassLongW (USER32.@)
 */
DWORD WINAPI SetClassLongW_k( HWND hwnd, INT offset, LONG newval )
{
    return CLASS_SetClassLong( hwnd, offset, newval, sizeof(LONG), TRUE );
}


/***********************************************************************
 *		SetClassLongA (USER32.@)
 */
DWORD WINAPI SetClassLongA_k( HWND hwnd, INT offset, LONG newval )
{
    return CLASS_SetClassLong( hwnd, offset, newval, sizeof(LONG), FALSE );
}


/***********************************************************************
 *		GetClassNameA (USER32.@)
 */
INT WINAPI GetClassNameA_k( HWND hwnd, LPSTR buffer, INT count )
{
    WCHAR tmpbuf[MAX_ATOM_LEN + 1];
    DWORD len;

    if (count <= 0) return 0;
    if (!GetClassNameW_k( hwnd, tmpbuf, sizeof(tmpbuf)/sizeof(WCHAR) )) return 0;
    RtlUnicodeToMultiByteN_k( buffer, count - 1, &len, tmpbuf, strlenW(tmpbuf) * sizeof(WCHAR) );
    buffer[len] = 0;
    return len;
}


/***********************************************************************
 *		GetClassNameW (USER32.@)
 */
INT WINAPI GetClassNameW_k( HWND hwnd, LPWSTR buffer, INT count )
{
    CLASS *class;
    INT ret;

    TRACE("%p %p %d\n", hwnd, buffer, count);

    if (count <= 0) return 0;

    if (!(class = get_class_ptr( hwnd, FALSE ))) return 0;

    lstrcpynW( buffer, class->name, count );
    ret = strlenW( buffer );
    return ret;
}


/***********************************************************************
 *		RealGetWindowClassA (USER32.@)
 */
UINT WINAPI RealGetWindowClassA_k( HWND hwnd, LPSTR buffer, UINT count )
{
    return GetClassNameA_k( hwnd, buffer, count );
}


/***********************************************************************
 *		RealGetWindowClassW (USER32.@)
 */
UINT WINAPI RealGetWindowClassW_k( HWND hwnd, LPWSTR buffer, UINT count )
{
    return GetClassNameW_k( hwnd, buffer, count );
}


/***********************************************************************
 *		GetClassInfoA (USER32.@)
 */
BOOL WINAPI GetClassInfoA_k( HINSTANCE hInstance, LPCSTR name, WNDCLASSA *wc )
{
    WNDCLASSEXA wcex;
    UINT ret = GetClassInfoExA_k( hInstance, name, &wcex );

    if (ret)
    {
        wc->style         = wcex.style;
        wc->lpfnWndProc   = wcex.lpfnWndProc;
        wc->cbClsExtra    = wcex.cbClsExtra;
        wc->cbWndExtra    = wcex.cbWndExtra;
        wc->hInstance     = wcex.hInstance;
        wc->hIcon         = wcex.hIcon;
        wc->hCursor       = wcex.hCursor;
        wc->hbrBackground = wcex.hbrBackground;
        wc->lpszMenuName  = wcex.lpszMenuName;
        wc->lpszClassName = wcex.lpszClassName;
    }
    return ret;
}


/***********************************************************************
 *		GetClassInfoW (USER32.@)
 */
BOOL WINAPI GetClassInfoW_k( HINSTANCE hInstance, LPCWSTR name, WNDCLASSW *wc )
{
    WNDCLASSEXW wcex;
    UINT ret = GetClassInfoExW_k( hInstance, name, &wcex );

    if (ret)
    {
        wc->style         = wcex.style;
        wc->lpfnWndProc   = wcex.lpfnWndProc;
        wc->cbClsExtra    = wcex.cbClsExtra;
        wc->cbWndExtra    = wcex.cbWndExtra;
        wc->hInstance     = wcex.hInstance;
        wc->hIcon         = wcex.hIcon;
        wc->hCursor       = wcex.hCursor;
        wc->hbrBackground = wcex.hbrBackground;
        wc->lpszMenuName  = wcex.lpszMenuName;
        wc->lpszClassName = wcex.lpszClassName;
    }
    return ret;
}


/***********************************************************************
 *		GetClassInfoExA (USER32.@)
 */
BOOL WINAPI GetClassInfoExA_k( HINSTANCE hInstance, LPCSTR name, WNDCLASSEXA *wc )
{
    ATOM atom;
    CLASS *classPtr;

    TRACE("%p %s %p\n", hInstance, debugstr_a(name), wc);

    if (!wc)
    {
        SetLastError_k( ERROR_NOACCESS );
        return FALSE;
    }

    if (!hInstance) hInstance = user32_module;

    if (!IS_INTRESOURCE(name))
    {
        WCHAR nameW[MAX_ATOM_LEN + 1];
        if (!MultiByteToWideChar_k( CP_ACP, 0, name, -1, nameW, sizeof(nameW)/sizeof(WCHAR) ))
            return FALSE;
        classPtr = CLASS_FindClass( nameW, hInstance );
    }
    else classPtr = CLASS_FindClass( (LPCWSTR)name, hInstance );

    if (!classPtr)
    {
        SetLastError_k( ERROR_CLASS_DOES_NOT_EXIST );
        return FALSE;
    }
    wc->style         = classPtr->style;
    wc->lpfnWndProc   = WINPROC_GetProc( classPtr->winproc, FALSE );
    wc->cbClsExtra    = classPtr->cbClsExtra;
    wc->cbWndExtra    = classPtr->cbWndExtra;
    wc->hInstance     = (hInstance == user32_module) ? 0 : hInstance;
    wc->hIcon         = classPtr->hIcon;
    wc->hIconSm       = classPtr->hIconSm ? classPtr->hIconSm : classPtr->hIconSmIntern;
    wc->hCursor       = classPtr->hCursor;
    wc->hbrBackground = classPtr->hbrBackground;
    wc->lpszMenuName  = CLASS_GetMenuNameA( classPtr );
    wc->lpszClassName = name;
    atom = classPtr->atomName;

    /* We must return the atom of the class here instead of just TRUE. */
    return atom;
}


/***********************************************************************
 *		GetClassInfoExW (USER32.@)
 */
BOOL WINAPI GetClassInfoExW_k( HINSTANCE hInstance, LPCWSTR name, WNDCLASSEXW *wc )
{
    ATOM atom;
    CLASS *classPtr;

    TRACE("%p %s %p\n", hInstance, debugstr_w(name), wc);

    if (!wc)
    {
        SetLastError_k( ERROR_NOACCESS );
        return FALSE;
    }

    if (!hInstance) hInstance = user32_module;

    if (!(classPtr = CLASS_FindClass( name, hInstance )))
    {
        SetLastError_k( ERROR_CLASS_DOES_NOT_EXIST );
        return FALSE;
    }
    wc->style         = classPtr->style;
    wc->lpfnWndProc   = WINPROC_GetProc( classPtr->winproc, TRUE );
    wc->cbClsExtra    = classPtr->cbClsExtra;
    wc->cbWndExtra    = classPtr->cbWndExtra;
    wc->hInstance     = (hInstance == user32_module) ? 0 : hInstance;
    wc->hIcon         = classPtr->hIcon;
    wc->hIconSm       = classPtr->hIconSm ? classPtr->hIconSm : classPtr->hIconSmIntern;
    wc->hCursor       = classPtr->hCursor;
    wc->hbrBackground = classPtr->hbrBackground;
    wc->lpszMenuName  = CLASS_GetMenuNameW( classPtr );
    wc->lpszClassName = name;
    atom = classPtr->atomName;

    /* We must return the atom of the class here instead of just TRUE. */
    return atom;
}


#if 0  /* toolhelp is in kernel, so this cannot work */

/***********************************************************************
 *		ClassFirst (TOOLHELP.69)
 */
BOOL16 WINAPI ClassFirst16( CLASSENTRY *pClassEntry )
{
    TRACE("%p\n",pClassEntry);
    pClassEntry->wNext = 1;
    return ClassNext16( pClassEntry );
}


/***********************************************************************
 *		ClassNext (TOOLHELP.70)
 */
BOOL16 WINAPI ClassNext16( CLASSENTRY *pClassEntry )
{
    int i;
    CLASS *class = firstClass;

    TRACE("%p\n",pClassEntry);

    if (!pClassEntry->wNext) return FALSE;
    for (i = 1; (i < pClassEntry->wNext) && class; i++) class = class->next;
    if (!class)
    {
        pClassEntry->wNext = 0;
        return FALSE;
    }
    pClassEntry->hInst = class->hInstance;
    pClassEntry->wNext++;
    GlobalGetAtomNameA( class->atomName, pClassEntry->szClassName,
                          sizeof(pClassEntry->szClassName) );
    return TRUE;
}
#endif

/* 64bit versions */

#ifdef GetClassLongPtrA
#undef GetClassLongPtrA
#endif

#ifdef GetClassLongPtrW
#undef GetClassLongPtrW
#endif

#ifdef SetClassLongPtrA
#undef SetClassLongPtrA
#endif

#ifdef SetClassLongPtrW
#undef SetClassLongPtrW
#endif

/***********************************************************************
 *		SetClassLongPtrW (USER32.@)
 */
ULONG_PTR WINAPI SetClassLongPtrW_k( HWND hwnd, INT offset, LONG_PTR newval )
{
    return CLASS_SetClassLong( hwnd, offset, newval, sizeof(LONG_PTR), TRUE );
}

/***********************************************************************
 *		SetClassLongPtrA (USER32.@)
 */
ULONG_PTR WINAPI SetClassLongPtrA_k( HWND hwnd, INT offset, LONG_PTR newval )
{
    return CLASS_SetClassLong( hwnd, offset, newval, sizeof(LONG_PTR), FALSE );
}

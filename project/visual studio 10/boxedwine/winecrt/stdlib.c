#include "config.h"
#include "wine/port.h"

#include <sys/types.h>

#include "windef.h"
#include "winbase.h"

int _fltused=1;

WINECRT int abs(int i)
{
	if (i<0)
		return -i;
	return i;
}

WINECRT long labs(long i)
{
	if (i<0)
		return -i;
	return i;
}

WINECRT int atexit(void (*func)(void))
{
	return 0;
}

WINECRT int atoi(const char *str)
{
	unsigned long long RunningTotal = 0;
    char bMinus = 0;

    while (*str == ' ' || (*str >= '\011' && *str <= '\015')) {
	str++;
    } /* while */

    if (*str == '+') {
	str++;
    } else if (*str == '-') {
	bMinus = 1;
	str++;
    } /* if */

    while (*str >= '0' && *str <= '9') {
	RunningTotal = RunningTotal * 10 + *str - '0';
	str++;
    } /* while */

    return bMinus ? -RunningTotal : RunningTotal;
}

static void mergesort( void *arr, void *barr, size_t elemsize, int(*compar)(const void *, const void *), size_t left, size_t right )
{
    if(right>left) {
        size_t i, j, k, m;
        m=left+(right-left)/2;
        mergesort( arr, barr, elemsize, compar, left, m);
        mergesort( arr, barr, elemsize, compar, m+1, right);

#define X(a,i) ((char*)a+elemsize*(i))
        for (k=left, i=left, j=m+1; i<=m && j<=right; k++) {
            if (compar(X(arr, i), X(arr,j)) <= 0) {
                memcpy(X(barr,k), X(arr, i), elemsize);
                i++;
            } else {
                memcpy(X(barr,k), X(arr, j), elemsize);
                j++;
            }
        }
        if (i<=m)
            memcpy(X(barr,k), X(arr,i), (m-i+1)*elemsize);
        else
            memcpy(X(barr,k), X(arr,j), (right-j+1)*elemsize);

        memcpy(X(arr, left), X(barr, left), (right-left+1)*elemsize);
    }
#undef X
}

WINECRT void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
    void *secondarr;
    if (nmemb < 2 || size == 0) return;
    secondarr = malloc(nmemb*size);
    mergesort( base, secondarr, size, compar, 0, nmemb-1 );
    free(secondarr);
}

WINECRT void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
    ssize_t min = 0;
    ssize_t max = nmemb - 1;

    while (min <= max)
    {
        ssize_t cursor = (min + max) / 2;
        int ret = compar(key,(const char *)base+(cursor*size));
        if (!ret)
            return (char*)base+(cursor*size);
        if (ret < 0)
            max = cursor - 1;
        else
            min = cursor + 1;
    }
    return NULL;
}

/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
/* 64-bit math operators for 32-bit systems */
WINECRT void __declspec(naked) _allmul()
{
	__asm {
		push        ebp
		mov         ebp,esp
		push        edi
		push        esi
		push        ebx
		sub         esp,0Ch
		mov         eax,dword ptr [ebp+10h]
		mov         edi,dword ptr [ebp+8]
		mov         ebx,eax
		mov         esi,eax
		sar         esi,1Fh
		mov         eax,dword ptr [ebp+8]
		mul         ebx
		imul        edi,esi
		mov         ecx,edx
		mov         dword ptr [ebp-18h],eax
		mov         edx,dword ptr [ebp+0Ch]
		add         ecx,edi
		imul        ebx,edx
		mov         eax,dword ptr [ebp-18h]
		lea         ebx,[ebx+ecx]
		mov         dword ptr [ebp-14h],ebx
		mov         edx,dword ptr [ebp-14h]
		add         esp,0Ch
		pop         ebx
		pop         esi
		pop         edi
		pop         ebp
		ret
	}
}

WINECRT void __declspec(naked) _alldiv()
{
	__asm {
		push        edi
		push        esi
		push        ebx
		xor         edi,edi
		mov         eax,dword ptr [esp+14h]
		or          eax,eax
		jge         L1
		inc         edi
		mov         edx,dword ptr [esp+10h]
		neg         eax
		neg         edx
		sbb         eax,0
		mov         dword ptr [esp+14h],eax
		mov         dword ptr [esp+10h],edx
L1:
		mov         eax,dword ptr [esp+1Ch]
		or          eax,eax
		jge         L2
		inc         edi
		mov         edx,dword ptr [esp+18h]
		neg         eax
		neg         edx
		sbb         eax,0
		mov         dword ptr [esp+1Ch],eax
		mov         dword ptr [esp+18h],edx
L2:
		or          eax,eax
		jne         L3
		mov         ecx,dword ptr [esp+18h]
		mov         eax,dword ptr [esp+14h]
		xor         edx,edx
		div         ecx
		mov         ebx,eax
		mov         eax,dword ptr [esp+10h]
		div         ecx
		mov         edx,ebx
		jmp         L4
L3:
		mov         ebx,eax
		mov         ecx,dword ptr [esp+18h]
		mov         edx,dword ptr [esp+14h]
		mov         eax,dword ptr [esp+10h]
L5:
		shr         ebx,1
		rcr         ecx,1
		shr         edx,1
		rcr         eax,1
		or          ebx,ebx
		jne         L5
		div         ecx
		mov         esi,eax
		mul         dword ptr [esp+1Ch]
		mov         ecx,eax
		mov         eax,dword ptr [esp+18h]
		mul         esi
		add         edx,ecx
		jb          L6
		cmp         edx,dword ptr [esp+14h]
		ja          L6
		jb          L7
		cmp         eax,dword ptr [esp+10h]
		jbe         L7
L6:
		dec         esi
L7:
		xor         edx,edx
		mov         eax,esi
L4:
		dec         edi
		jne         L8
		neg         edx
		neg         eax
		sbb         edx,0
L8:
		pop         ebx
		pop         esi
		pop         edi
		ret         10h
	}
}

WINECRT void __declspec(naked) _aulldiv()
{
	__asm {
		push        ebx
		push        esi
		mov         eax,dword ptr [esp+18h]
		or          eax,eax
		jne         L1
		mov         ecx,dword ptr [esp+14h]
		mov         eax,dword ptr [esp+10h]
		xor         edx,edx
		div         ecx
		mov         ebx,eax
		mov         eax,dword ptr [esp+0Ch]
		div         ecx
		mov         edx,ebx
		jmp         L2
L1:
		mov         ecx,eax
		mov         ebx,dword ptr [esp+14h]
		mov         edx,dword ptr [esp+10h]
		mov         eax,dword ptr [esp+0Ch]
L3:
		shr         ecx,1
		rcr         ebx,1
		shr         edx,1
		rcr         eax,1
		or          ecx,ecx
		jne         L3
		div         ebx
		mov         esi,eax
		mul         dword ptr [esp+18h]
		mov         ecx,eax
		mov         eax,dword ptr [esp+14h]
		mul         esi
		add         edx,ecx
		jb          L4
		cmp         edx,dword ptr [esp+10h]
		ja          L4
		jb          L5
		cmp         eax,dword ptr [esp+0Ch]
		jbe         L5
L4:
		dec         esi
L5:
		xor         edx,edx
		mov         eax,esi
L2:
		pop         esi
		pop         ebx
		ret         10h
	}
}

WINECRT void __declspec(naked) _allrem()
{
	__asm {
		push        ebx
		push        edi
		xor         edi,edi
		mov         eax,dword ptr [esp+10h]
		or          eax,eax
		jge         L1
		inc         edi
		mov         edx,dword ptr [esp+0Ch]
		neg         eax
		neg         edx
		sbb         eax,0
		mov         dword ptr [esp+10h],eax
		mov         dword ptr [esp+0Ch],edx
L1:
		mov         eax,dword ptr [esp+18h]
		or          eax,eax
		jge         L2
		mov         edx,dword ptr [esp+14h]
		neg         eax
		neg         edx
		sbb         eax,0
		mov         dword ptr [esp+18h],eax
		mov         dword ptr [esp+14h],edx
L2:
		or          eax,eax
		jne         L3
		mov         ecx,dword ptr [esp+14h]
		mov         eax,dword ptr [esp+10h]
		xor         edx,edx
		div         ecx
		mov         eax,dword ptr [esp+0Ch]
		div         ecx
		mov         eax,edx
		xor         edx,edx
		dec         edi
		jns         L4
		jmp         L8
L3:
		mov         ebx,eax
		mov         ecx,dword ptr [esp+14h]
		mov         edx,dword ptr [esp+10h]
		mov         eax,dword ptr [esp+0Ch]
L5:
		shr         ebx,1
		rcr         ecx,1
		shr         edx,1
		rcr         eax,1
		or          ebx,ebx
		jne         L5
		div         ecx
		mov         ecx,eax
		mul         dword ptr [esp+18h]
		xchg        eax,ecx
		mul         dword ptr [esp+14h]
		add         edx,ecx
		jb          L6
		cmp         edx,dword ptr [esp+10h]
		ja          L6
		jb          L7
		cmp         eax,dword ptr [esp+0Ch]
		jbe         L7
L6:
		sub         eax,dword ptr [esp+14h]
		sbb         edx,dword ptr [esp+18h]
L7:
		sub         eax,dword ptr [esp+0Ch]
		sbb         edx,dword ptr [esp+10h]
		dec         edi
		jns         L8
L4:
		neg         edx
		neg         eax
		sbb         edx,0
L8:
		pop         edi
		pop         ebx
		ret         10h
	}
}

WINECRT void __declspec(naked) _aullrem()
{
	__asm {
		push        ebx
		mov         eax,dword ptr [esp+14h]
		or          eax,eax
		jne         L1
		mov         ecx,dword ptr [esp+10h]
		mov         eax,dword ptr [esp+0Ch]
		xor         edx,edx
		div         ecx
		mov         eax,dword ptr [esp+8]
		div         ecx
		mov         eax,edx
		xor         edx,edx
		jmp         L2
L1:
		mov         ecx,eax
		mov         ebx,dword ptr [esp+10h]
		mov         edx,dword ptr [esp+0Ch]
		mov         eax,dword ptr [esp+8]
L3:
		shr         ecx,1
		rcr         ebx,1
		shr         edx,1
		rcr         eax,1
		or          ecx,ecx
		jne         L3
		div         ebx
		mov         ecx,eax
		mul         dword ptr [esp+14h]
		xchg        eax,ecx
		mul         dword ptr [esp+10h]
		add         edx,ecx
		jb          L4
		cmp         edx,dword ptr [esp+0Ch]
		ja          L4
		jb          L5
		cmp         eax,dword ptr [esp+8]
		jbe         L5
L4:
		sub         eax,dword ptr [esp+10h]
		sbb         edx,dword ptr [esp+14h]
L5:
		sub         eax,dword ptr [esp+8]
		sbb         edx,dword ptr [esp+0Ch]
		neg         edx
		neg         eax
		sbb         edx,0
L2:
		pop         ebx
		ret         10h
	}
}

WINECRT void __declspec(naked) _alldvrm()
{
	__asm {
		push        edi
		push        esi
		push        ebp
		xor         edi,edi
		xor         ebp,ebp
		mov         eax,dword ptr [esp+14h]
		or          eax,eax
		jge         L1
		inc         edi
		inc         ebp
		mov         edx,dword ptr [esp+10h]
		neg         eax
		neg         edx
		sbb         eax,0
		mov         dword ptr [esp+14h],eax
		mov         dword ptr [esp+10h],edx
L1:
		mov         eax,dword ptr [esp+1Ch]
		or          eax,eax
		jge         L2
		inc         edi
		mov         edx,dword ptr [esp+18h]
		neg         eax
		neg         edx
		sbb         eax,0
		mov         dword ptr [esp+1Ch],eax
		mov         dword ptr [esp+18h],edx
L2:
		or          eax,eax
		jne         L3
		mov         ecx,dword ptr [esp+18h]
		mov         eax,dword ptr [esp+14h]
		xor         edx,edx
		div         ecx
		mov         ebx,eax
		mov         eax,dword ptr [esp+10h]
		div         ecx
		mov         esi,eax
		mov         eax,ebx
		mul         dword ptr [esp+18h]
		mov         ecx,eax
		mov         eax,esi
		mul         dword ptr [esp+18h]
		add         edx,ecx
		jmp         L4
L3:
		mov         ebx,eax
		mov         ecx,dword ptr [esp+18h]
		mov         edx,dword ptr [esp+14h]
		mov         eax,dword ptr [esp+10h]
L5:
		shr         ebx,1
		rcr         ecx,1
		shr         edx,1
		rcr         eax,1
		or          ebx,ebx
		jne         L5
		div         ecx
		mov         esi,eax
		mul         dword ptr [esp+1Ch]
		mov         ecx,eax
		mov         eax,dword ptr [esp+18h]
		mul         esi
		add         edx,ecx
		jb          L6
		cmp         edx,dword ptr [esp+14h]
		ja          L6
		jb          L7
		cmp         eax,dword ptr [esp+10h]
		jbe         L7
L6:
		dec         esi
		sub         eax,dword ptr [esp+18h]
		sbb         edx,dword ptr [esp+1Ch]
L7:
		xor         ebx,ebx
L4:
		sub         eax,dword ptr [esp+10h]
		sbb         edx,dword ptr [esp+14h]
		dec         ebp
		jns         L9
		neg         edx
		neg         eax
		sbb         edx,0
L9:
		mov         ecx,edx
		mov         edx,ebx
		mov         ebx,ecx
		mov         ecx,eax
		mov         eax,esi
		dec         edi
		jne         L8
		neg         edx
		neg         eax
		sbb         edx,0
L8:
		pop         ebp
		pop         esi
		pop         edi
		ret         10h
	}
}

WINECRT void __declspec(naked) _aulldvrm()
{
	__asm {
		push        esi
		mov         eax,dword ptr [esp+14h]
		or          eax,eax
		jne         L1
		mov         ecx,dword ptr [esp+10h]
		mov         eax,dword ptr [esp+0Ch]
		xor         edx,edx
		div         ecx
		mov         ebx,eax
		mov         eax,dword ptr [esp+8]
		div         ecx
		mov         esi,eax
		mov         eax,ebx
		mul         dword ptr [esp+10h]
		mov         ecx,eax
		mov         eax,esi
		mul         dword ptr [esp+10h]
		add         edx,ecx
		jmp         L2
L1:
		mov         ecx,eax
		mov         ebx,dword ptr [esp+10h]
		mov         edx,dword ptr [esp+0Ch]
		mov         eax,dword ptr [esp+8]
L3:
		shr         ecx,1
		rcr         ebx,1
		shr         edx,1
		rcr         eax,1
		or          ecx,ecx
		jne         L3
		div         ebx
		mov         esi,eax
		mul         dword ptr [esp+14h]
		mov         ecx,eax
		mov         eax,dword ptr [esp+10h]
		mul         esi
		add         edx,ecx
		jb          L4
		cmp         edx,dword ptr [esp+0Ch]
		ja          L4
		jb          L5
		cmp         eax,dword ptr [esp+8]
		jbe         L5
L4:
		dec         esi
		sub         eax,dword ptr [esp+10h]
		sbb         edx,dword ptr [esp+14h]
L5:
		xor         ebx,ebx
L2:
		sub         eax,dword ptr [esp+8]
		sbb         edx,dword ptr [esp+0Ch]
		neg         edx
		neg         eax
		sbb         edx,0
		mov         ecx,edx
		mov         edx,ebx
		mov         ebx,ecx
		mov         ecx,eax
		mov         eax,esi
		pop         esi
		ret         10h
	}
}

WINECRT void __declspec(naked) _allshl()
{
	__asm {
		cmp         cl,40h
		jae         RETZERO
		cmp         cl,20h
		jae         MORE32
		shld        edx,eax,cl
		shl         eax,cl
		ret
MORE32:
		mov         edx,eax
		xor         eax,eax
		and         cl,1Fh
		shl         edx,cl
		ret
RETZERO:
		xor         eax,eax
		xor         edx,edx
		ret
	}
}

WINECRT void __declspec(naked) _allshr()
{
    /* *INDENT-OFF* */
    __asm {
        cmp         cl,40h
        jae         RETZERO
        cmp         cl,20h
        jae         MORE32
        shrd        eax,edx,cl
        sar         edx,cl
        ret
MORE32:
        mov         eax,edx
        xor         edx,edx
        and         cl,1Fh
        sar         eax,cl
        ret
RETZERO:
        xor         eax,eax
        xor         edx,edx
        ret
    }
    /* *INDENT-ON* */
}

WINECRT void __declspec(naked) _aullshr()
{
	__asm {
		cmp         cl,40h
		jae         RETZERO
		cmp         cl,20h
		jae         MORE32
		shrd        eax,edx,cl
		shr         edx,cl
		ret
MORE32:
		mov         eax,edx
		xor         edx,edx
		and         cl,1Fh
		shr         eax,cl
		ret
RETZERO:
		xor         eax,eax
		xor         edx,edx
		ret
	}
}

WINECRT void __declspec(naked) ftol()
{
	__asm {
		push        ebp
		mov         ebp,esp
		sub         esp,20h
		and         esp,0FFFFFFF0h
		fld         st(0)
		fst         dword ptr [esp+18h]
		fistp       qword ptr [esp+10h]
		fild        qword ptr [esp+10h]
		mov         edx,dword ptr [esp+18h]
		mov         eax,dword ptr [esp+10h]
		test        eax,eax
		je          integer_QnaN_or_zero
arg_is_not_integer_QnaN:
		fsubp       st(1),st
		test        edx,edx
		jns         positive
		fstp        dword ptr [esp]
		mov         ecx,dword ptr [esp]
		xor         ecx,80000000h
		add         ecx,7FFFFFFFh
		adc         eax,0
		mov         edx,dword ptr [esp+14h]
		adc         edx,0
		jmp         localexit
positive:
		fstp        dword ptr [esp]
		mov         ecx,dword ptr [esp]
		add         ecx,7FFFFFFFh
		sbb         eax,0
		mov         edx,dword ptr [esp+14h]
		sbb         edx,0
		jmp         localexit
integer_QnaN_or_zero:
		mov         edx,dword ptr [esp+14h]
		test        edx,7FFFFFFFh
		jne         arg_is_not_integer_QnaN
		fstp        dword ptr [esp+18h]
		fstp        dword ptr [esp+18h]
localexit:
		leave
		ret
	}
}

size_t SDL_ScanUnsignedLong(const char *text, int radix, unsigned long *valuep);
WINECRT unsigned long strtoul(const char *string, char **endp, int base)
{
    size_t len;
    unsigned long value;

    if ( !base ) {
        if ( (strlen(string) > 2) && (strncmp(string, "0x", 2) == 0) ) {
            base = 16;
        } else {
            base = 10;
        }
    }

    len = SDL_ScanUnsignedLong(string, base, &value);
    if ( endp ) {
        *endp = (char *)string + len;
    }
    return value;
}

size_t SDL_ScanLong(const char *text, int radix, long *valuep);
WINECRT long strtol(const char *string, char **endp, int base)
{
    size_t len;
    long value;

    if ( !base ) {
        if ( (strlen(string) > 2) && (strncmp(string, "0x", 2) == 0) ) {
            base = 16;
        } else {
            base = 10;
        }
    }

    len = SDL_ScanLong(string, base, &value);
    if ( endp ) {
        *endp = (char *)string + len;
    }
    return value;
}
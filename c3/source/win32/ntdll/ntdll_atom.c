/*
 * Atom table functions
 *
 * Copyright 1993, 1994, 1995 Alexandre Julliard
 * Copyright 2004,2005 Eric Pouech
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

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pbl.h"

#include "../include/winbase.h"
#include "../include/unicode.h"
#include "../include/ntstatus.h"

#define MAX_ATOM_LEN    255
#define IS_INTATOM(x)   (((ULONG_PTR)(x) >> 16) == 0)

//#define MIN_STR_ATOM  0xc000
//#define MAX_ATOMS     0x4000

static RTL_ATOM nextAtom;

struct atom_entry
{
    int                count;  /* reference count */
    short              pinned; /* whether the atom is pinned or not */
    RTL_ATOM             atom;   /* atom handle */
    unsigned short     str[255];
};

static void atom_table_dump( struct object *obj, int verbose );
static void atom_table_destroy( struct object *obj );

static RTL_ATOM_TABLE global_table;

/* retrieve an entry pointer from its atom */
static NTSTATUS get_atom_entry( RTL_ATOM_TABLE table, RTL_ATOM atom, struct atom_entry** entry )
{
    if (table)
        *entry = pblMapGet((PblMap*)table, &atom, sizeof(RTL_ATOM), 0);
    if (!*entry) return STATUS_INVALID_HANDLE;
    return STATUS_SUCCESS;
}

/* add an atom entry in the table and return its handle */
static RTL_ATOM add_atom_entry( RTL_ATOM_TABLE table, struct atom_entry *entry )
{
    nextAtom++;
    entry->atom = nextAtom;
    pblMapAdd((PblMap*)table, &nextAtom, sizeof(RTL_ATOM), entry, sizeof(struct atom_entry));
    return entry->atom;
}

static struct atom_entry *find_atom_entry( RTL_ATOM_TABLE table, const WCHAR* str)
{
    PblIterator* it = pblMapIteratorNew((PblMap*)table);
    struct atom_entry* result = NULL;

    while (it && pblIteratorHasNext(it)) {
        struct atom_entry *entry = pblMapEntryValue(pblIteratorNext(it));
        if (!strcmpiW(str, entry->str)) {
            result = entry;
        }
    }
    pblIteratorFree(it);
    return result;
}

/* add an atom to the table */
static NTSTATUS add_atom( RTL_ATOM_TABLE table, const WCHAR* str, RTL_ATOM* atom )
{
    struct atom_entry entry;    
    struct atom_entry* pEntry;    
    int len = strlenW(str);

    if (!len)
    {
        return STATUS_OBJECT_NAME_INVALID;
    }
    if (len > MAX_ATOM_LEN)
    {
        return STATUS_INVALID_PARAMETER;
    }
    if ((pEntry = find_atom_entry( table, str)))  /* exists already */
    {
        pEntry->count++;
        *atom = pEntry->atom;
        return STATUS_SUCCESS;
    }

    entry.count = 1;
    entry.pinned = 0;
    strcpyW(entry.str, str);

    *atom = add_atom_entry( table, &entry );
    return STATUS_SUCCESS;
}

/* delete an atom from the table */
static DWORD delete_atom( RTL_ATOM_TABLE table, RTL_ATOM atom, int if_pinned )
{
    struct atom_entry *entry;
    DWORD status = get_atom_entry( table, atom, &entry );
    if (!entry) return status;
    if (entry->pinned && !if_pinned)return STATUS_WAS_LOCKED;
    else if (!--entry->count)
    {
        pblMapRemove((PblMap*)table, &atom, sizeof(RTL_ATOM), 0);
    }
    return STATUS_SUCCESS;
}

/* find an atom in the table */
static DWORD find_atom( RTL_ATOM_TABLE table, const WCHAR *str, RTL_ATOM* atom )
{
    struct atom_entry *entry;
    int len = strlenW(str);

    if (!len)
    {
        return STATUS_OBJECT_NAME_INVALID;
    }
    if (len > MAX_ATOM_LEN)
    {
        return STATUS_INVALID_PARAMETER;
    }
    if (table && (entry = find_atom_entry( table, str)))
    {
        *atom = entry->atom;
        return STATUS_SUCCESS;
    }
    return STATUS_OBJECT_NAME_NOT_FOUND;
}

static RTL_ATOM_TABLE get_global_table( struct winstation *winstation, int create )
{
    // struct atom_table *table = winstation ? winstation->atom_table : global_table;
    if (!global_table)
    {        
        global_table = (RTL_ATOM_TABLE)pblMapNewHashMap();
    }
    return global_table;
}

/* add an atom in the global table; used for window properties */
DWORD add_global_atom( struct winstation *winstation, const WCHAR *str, RTL_ATOM* atom )
{
    RTL_ATOM_TABLE table = get_global_table( winstation, 1 );
    if (!table) return 0;
    return add_atom( table, str, atom );
}

/* find an atom in the global table; used for window properties */
RTL_ATOM find_global_atom( struct winstation *winstation, WCHAR *str )
{
    RTL_ATOM_TABLE table = get_global_table( winstation, 0 );
    struct atom_entry *entry;

    if ((entry = find_atom_entry( table, str)))
        return entry->atom;
    return 0;
}

/* decrement the ref count of a global atom; used for window properties */
void release_global_atom( struct winstation *winstation, RTL_ATOM atom )
{
    RTL_ATOM_TABLE table = get_global_table( winstation, 0 );
    if (table) delete_atom( table, atom, 1 );
}

/******************************************************************
 *		is_integral_atom
 * Returns STATUS_SUCCESS if integral atom and 'pAtom' is filled
 *         STATUS_INVALID_PARAMETER if 'atomstr' is too long
 *         STATUS_MORE_ENTRIES otherwise
 */
static NTSTATUS is_integral_atom( LPCWSTR atomstr, size_t len, RTL_ATOM* pAtom )
{
    RTL_ATOM atom;

    if (!IS_INTATOM( atomstr ))
    {
        const WCHAR* ptr = atomstr;
        if (!len) return STATUS_OBJECT_NAME_INVALID;

        if (*ptr++ == '#')
        {
            atom = 0;
            while (ptr < atomstr + len && *ptr >= '0' && *ptr <= '9')
            {
                atom = atom * 10 + *ptr++ - '0';
            }
            if (ptr > atomstr + 1 && ptr == atomstr + len) goto done;
        }
        if (len > MAX_ATOM_LEN) return STATUS_INVALID_PARAMETER;
        return STATUS_MORE_ENTRIES;
    }
    else atom = LOWORD( atomstr );
done:
    if (!atom || atom >= MAXINTATOM) return STATUS_INVALID_PARAMETER;
    *pAtom = atom;
    return STATUS_SUCCESS;
}

/******************************************************************
 *		RtlDeleteAtomFromAtomTable (NTDLL.@)
 */
NTSTATUS WINAPI RtlDeleteAtomFromAtomTable_k( RTL_ATOM_TABLE table, RTL_ATOM atom )
{
    TRACE( "%p %x\n", table, atom );
    if (!table) return STATUS_INVALID_PARAMETER;
    else return delete_atom(table, atom, 0);
}

/******************************************************************
 *		integral_atom_name (internal)
 *
 * Helper for fetching integral (local/global) atoms names.
 */
static ULONG integral_atom_name(WCHAR* buffer, ULONG len, RTL_ATOM atom)
{
    static const WCHAR fmt[] = {'#','%','u',0};
    WCHAR tmp[16];
    ULONG ret;

    ret = sprintfW( tmp, fmt, atom );
    if (!len) return ret * sizeof(WCHAR);
    if (len <= ret) ret = len - 1;
    memcpy( buffer, tmp, ret * sizeof(WCHAR) );
    buffer[ret] = 0;
    return ret * sizeof(WCHAR);
}

/******************************************************************
 *		RtlQueryAtomInAtomTable (NTDLL.@)
 */
NTSTATUS WINAPI RtlQueryAtomInAtomTable_k( RTL_ATOM_TABLE table, RTL_ATOM atom, ULONG* ref,
                                         ULONG* pin, WCHAR* name, ULONG* len )
{
    NTSTATUS    status = STATUS_SUCCESS;
    DWORD       wlen = 0;

    if (!table) status = STATUS_INVALID_PARAMETER;
    else if (atom < MAXINTATOM)
    {
        if (!atom) return STATUS_INVALID_PARAMETER;
        if (len) wlen = integral_atom_name( name, *len, atom);
        if (ref) *ref = 1;
        if (pin) *pin = 1;
    }
    else
    {
        struct atom_entry *entry;

        status = get_atom_entry( table, atom, &entry);
        if (entry)
        {
            if (ref) *ref = entry->count;
            if (pin) *pin = entry->pinned;
            if (name) strcpyW(name, entry->str);
            if (len) *len=strlenW(entry->str);
        }
    }
    if (status == STATUS_SUCCESS && len)
    {
        if (*len)
        {
            wlen = min( *len - sizeof(WCHAR), wlen );
            if (name) name[wlen / sizeof(WCHAR)] = 0;
        }
        else status = STATUS_BUFFER_TOO_SMALL;
        *len = wlen;
    }

    //TRACE( "%p %x -> %s (%x)\n", table, atom, len ? debugstr_wn(name, wlen / sizeof(WCHAR)) : "(null)", status );
    return status;
}

/******************************************************************
 *		RtlCreateAtomTable (NTDLL.@)
 */
NTSTATUS WINAPI RtlCreateAtomTable_k( ULONG size, RTL_ATOM_TABLE* table )
{
    NTSTATUS    status;

    if (*table)
    {
        if (size) status = STATUS_INVALID_PARAMETER;
        else status = STATUS_SUCCESS;
    }
    else
    {
        *table = (RTL_ATOM_TABLE)pblMapNewHashMap();
        status = STATUS_SUCCESS;
    }
    return status;
}

/******************************************************************
 *		RtlDestroyAtomTable (NTDLL.@)
 */
NTSTATUS WINAPI RtlDestroyAtomTable_k( RTL_ATOM_TABLE table )
{
    if (!table) return STATUS_INVALID_PARAMETER;
    pblMapFree((PblMap*)table);
    return STATUS_SUCCESS;
}

/******************************************************************
 *		RtlAddAtomToAtomTable (NTDLL.@)
 */
NTSTATUS WINAPI RtlAddAtomToAtomTable_k( RTL_ATOM_TABLE table, const WCHAR* name, RTL_ATOM* atom )
{
    NTSTATUS    status;

    if (!table) status = STATUS_INVALID_PARAMETER;
    else
    {
        size_t len = IS_INTATOM(name) ?  0 : strlenW(name);
        status = is_integral_atom( name, len, atom );
        if (status == STATUS_MORE_ENTRIES)
        {
            status = add_atom(table, name, atom);
        }
    }
    TRACE( "%p %s -> %x\n",
           table, debugstr_w(name), status == STATUS_SUCCESS ? *atom : 0 );

    return status;
}

/******************************************************************
 *		RtlLookupAtomInAtomTable (NTDLL.@)
 */
NTSTATUS WINAPI RtlLookupAtomInAtomTable_k( RTL_ATOM_TABLE table, const WCHAR* name, RTL_ATOM* atom )
{
    NTSTATUS    status;

    if (!table) status = STATUS_INVALID_PARAMETER;
    else
    {
        size_t len = IS_INTATOM(name) ? 0 : strlenW(name);
        status = is_integral_atom( name, len, atom );
        if (status == STATUS_MORE_ENTRIES)
        {
            status = find_atom(table, name, atom);
        }
    }
    TRACE( "%p %s -> %x\n",
           table, debugstr_w(name), status == STATUS_SUCCESS ? *atom : 0 );
    return status;
}

/******************************************************************
 *		RtlEmptyAtomTable (NTDLL.@)
 */
NTSTATUS WINAPI RtlEmptyAtomTable_k( RTL_ATOM_TABLE table, BOOLEAN delete_pinned )
{
    if (!table) return STATUS_INVALID_PARAMETER;
    else
    {
        if (delete_pinned) {
            pblMapClear((PblMap*)table);
        } else {
            PblIterator* it = pblMapIteratorNew((PblMap*)table);
            struct atom_entry* result = NULL;

            while (it && pblIteratorHasNext(it)) {
                struct atom_entry *entry = pblMapEntryValue(pblIteratorNext(it));
                if (!entry->pinned) {
                    pblMapRemove((PblMap*)table, &entry->atom, sizeof(RTL_ATOM), 0);
                }
            }
            pblIteratorFree(it);
        }
    }
    return STATUS_SUCCESS;
}

/******************************************************************
 *		RtlPinAtomInAtomTable (NTDLL.@)
 */
NTSTATUS WINAPI RtlPinAtomInAtomTable_k( RTL_ATOM_TABLE table, RTL_ATOM atom )
{
    struct atom_entry *entry;
    NTSTATUS status ;

    if (!table) return STATUS_INVALID_PARAMETER;
    if (atom < MAXINTATOM) return STATUS_SUCCESS;

    status = get_atom_entry( table, atom, &entry);
    if (entry)
    {
        entry->pinned = 1;
        return STATUS_SUCCESS;
    } 
    return status;    
}

/*************************************************
 *        Global handle table management
 *************************************************/

/******************************************************************
 *		NtAddAtom (NTDLL.@)
 */
NTSTATUS WINAPI NtAddAtom_k( const WCHAR* name, ULONG length, RTL_ATOM* atom )
{
    NTSTATUS    status;

    status = is_integral_atom( name, length / sizeof(WCHAR), atom );
    if (status == STATUS_MORE_ENTRIES)
    {
        status = add_atom(global_table, name, atom);
    }
    // TRACE( "%s -> %x\n", debugstr_wn(name, length/sizeof(WCHAR)), status == STATUS_SUCCESS ? *atom : 0 );
    return status;
}

/******************************************************************
 *		NtDeleteAtom (NTDLL.@)
 */
NTSTATUS WINAPI NtDeleteAtom_k(RTL_ATOM atom)
{
    return delete_atom(global_table, atom, 0);
}

/******************************************************************
 *		NtFindAtom (NTDLL.@)
 */
NTSTATUS WINAPI NtFindAtom_k( const WCHAR* name, ULONG length, RTL_ATOM* atom )
{
    NTSTATUS    status;

    status = is_integral_atom( name, length / sizeof(WCHAR), atom );
    if (status == STATUS_MORE_ENTRIES)
    {
        status = find_atom(global_table, name, atom);
    }
    // TRACE( "%s -> %x\n", debugstr_wn(name, length/sizeof(WCHAR)), status == STATUS_SUCCESS ? *atom : 0 );
    return status;
}

/******************************************************************
 *		NtQueryInformationAtom (NTDLL.@)
 */
NTSTATUS WINAPI NtQueryInformationAtom_k( RTL_ATOM atom, ATOM_INFORMATION_CLASS class,
                                        PVOID ptr, ULONG size, PULONG psize )
{
    NTSTATUS status;

    switch (class)
    {
    case AtomBasicInformation:
        {
            ULONG name_len;
            ATOM_BASIC_INFORMATION* abi = ptr;

            if (size < sizeof(ATOM_BASIC_INFORMATION))
                return STATUS_INVALID_PARAMETER;
            name_len = size - sizeof(ATOM_BASIC_INFORMATION);

            if (atom < MAXINTATOM)
            {
                if (atom)
                {
                    abi->NameLength = (USHORT)integral_atom_name( abi->Name, name_len, atom );
                    status = (name_len) ? STATUS_SUCCESS : STATUS_BUFFER_TOO_SMALL;
                    abi->ReferenceCount = 1;
                    abi->Pinned = 1;
                }
                else status = STATUS_INVALID_PARAMETER;
            }
            else
            {
                struct atom_entry* entry;
                
                get_atom_entry(global_table, atom, &entry);
                if (!entry)
                    return STATUS_OBJECT_NAME_NOT_FOUND;
                abi->ReferenceCount = entry->count;
                abi->Pinned = entry->pinned;
                name_len = strlenW(entry->str)*2;
                abi->NameLength = (USHORT)name_len;
                abi->Name[name_len / sizeof(WCHAR)] = '\0';
            }
            //TRACE( "%x -> %s (%u)\n",  atom, debugstr_wn(abi->Name, abi->NameLength / sizeof(WCHAR)), status );
            if (psize)
                *psize = sizeof(ATOM_BASIC_INFORMATION) + name_len;
        }
        break;
    default:
        FIXME( "Unsupported class %u\n", class );
        status = STATUS_INVALID_INFO_CLASS;
        break;
    }
    return status;
}

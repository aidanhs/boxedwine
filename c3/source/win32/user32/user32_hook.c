/*
 * Windows hook functions
 *
 * Copyright 2002 Alexandre Julliard
 * Copyright 2005 Dmitry Timoshkov
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
 *
 * NOTES:
 *   Status of the various hooks:
 *     WH_MSGFILTER                 OK
 *     WH_JOURNALRECORD             Partially implemented
 *     WH_JOURNALPLAYBACK           Partially implemented
 *     WH_KEYBOARD                  OK
 *     WH_GETMESSAGE	            OK (FIXME: A/W mapping?)
 *     WH_CALLWNDPROC	            OK (FIXME: A/W mapping?)
 *     WH_CBT
 *       HCBT_MOVESIZE              OK
 *       HCBT_MINMAX                OK
 *       HCBT_QS                    OK
 *       HCBT_CREATEWND             OK
 *       HCBT_DESTROYWND            OK
 *       HCBT_ACTIVATE              OK
 *       HCBT_CLICKSKIPPED          OK
 *       HCBT_KEYSKIPPED            OK
 *       HCBT_SYSCOMMAND            OK
 *       HCBT_SETFOCUS              OK
 *     WH_SYSMSGFILTER	            OK
 *     WH_MOUSE	                    OK
 *     WH_HARDWARE	            Not supported in Win32
 *     WH_DEBUG	                    Not implemented
 *     WH_SHELL
 *       HSHELL_WINDOWCREATED       OK
 *       HSHELL_WINDOWDESTROYED     OK
 *       HSHELL_ACTIVATESHELLWINDOW Not implemented
 *       HSHELL_WINDOWACTIVATED     Not implemented
 *       HSHELL_GETMINRECT          Not implemented
 *       HSHELL_REDRAW              Not implemented
 *       HSHELL_TASKMAN             Not implemented
 *       HSHELL_LANGUAGE            Not implemented
 *       HSHELL_SYSMENU             Not implemented
 *       HSHELL_ENDTASK             Not implemented
 *       HSHELL_ACCESSIBILITYSTATE  Not implemented
 *       HSHELL_APPCOMMAND          Not implemented
 *       HSHELL_WINDOWREPLACED      Not implemented
 *       HSHELL_WINDOWREPLACING     Not implemented
 *     WH_FOREGROUNDIDLE            Not implemented
 *     WH_CALLWNDPROCRET            OK (FIXME: A/W mapping?)
 *     WH_KEYBOARD_LL               Implemented but should use SendMessage instead
 *     WH_MOUSE_LL                  Implemented but should use SendMessage instead
 */

#include <stdarg.h>
#include <assert.h>
#include "../include/winbase.h"
#include "user_private.h"
#include "../include/unicode.h"
#include "kthread.h"
#include "kprocess.h"
#include "kalloc.h"

struct hook_info
{
    INT id;
    void *proc;
    void *handle;
    DWORD pid, tid;
    BOOL prev_unicode, next_unicode;
    WCHAR module[MAX_PATH];
};

struct hook_table;

struct hook
{
    struct list         chain;    /* hook chain entry */
    //user_handle_t       handle;   /* user handle for this hook */
    struct KProcess     *process;  /* process the hook is set to */
    struct KThread      *thread;   /* thread the hook is set to */
    struct KThread      *owner;    /* owner of the out of context hook */
    struct hook_table  *table;    /* hook table that contains this hook */
    int                 index;    /* hook table index */
    int                 event_min;
    int                 event_max;
    int                 flags;
    void*               proc;     /* hook function */
    int                 unicode;  /* is it a unicode hook? */
    WCHAR               module[260];   /* module name for global hooks */
    HHOOK               handle;
};

#define WH_WINEVENT (WH_MAXHOOK+1)

#define NB_HOOKS (WH_WINEVENT-WH_MINHOOK+1)
#define HOOK_ENTRY(p)  LIST_ENTRY( (p), struct hook, chain )

struct hook_table
{
    struct list   hooks[NB_HOOKS];  /* array of hook chains */
    int           counts[NB_HOOKS]; /* use counts for each hook chain */
};

static void hook_table_dump( struct object *obj, int verbose );
static void hook_table_destroy( struct object *obj );

/* create a new hook table */
static struct hook_table *alloc_hook_table(void)
{
    struct hook_table *table;
    int i;

    if ((table = kalloc(sizeof(struct hook_table))))
    {
        for (i = 0; i < NB_HOOKS; i++)
        {
            list_init( &table->hooks[i] );
            table->counts[i] = 0;
        }
    }
    return table;
}

static struct hook_table *get_global_hooks( struct KThread *thread )
{
    if (!thread->input->desktop) return NULL;
    return thread->input->desktop->global_hooks;
}

void set_queue_hooks( struct KThread *thread, struct hook_table *hooks );
struct hook_table *get_queue_hooks( struct KThread *thread );

/* create a new hook and add it to the specified table */
static struct hook *add_hook( struct desktop *desktop, struct KThread *thread, int index, int global )
{
    struct hook *hook;
    struct hook_table *table = global ? desktop->global_hooks : thread->input->hooks;

    if (!table)
    {
        if (!(table = alloc_hook_table())) return NULL;
        if (global) desktop->global_hooks = table;
        else set_queue_hooks( thread, table );
    }
    if (!(hook = kalloc( sizeof(*hook) ))) return NULL;

    hook->thread = thread;
    hook->table  = table;
    hook->index  = index;
    list_add_head( &table->hooks[index], &hook->chain );
    // :TODO: hook handle
    return hook;
}

/* free a hook, removing it from its chain */
static void free_hook( struct hook *hook )
{
    list_remove( &hook->chain );
    free( hook );
}

/* find a hook from its index and proc */
static struct hook *find_hook( struct KThread *thread, int index, void* proc )
{
    struct list *p;
    struct hook_table *table = get_queue_hooks( thread );

    if (table)
    {
        LIST_FOR_EACH( p, &table->hooks[index] )
        {
            struct hook *hook = HOOK_ENTRY( p );
            if (hook->proc == proc) return hook;
        }
    }
    return NULL;
}

static struct hook *find_hook_by_handle( struct KThread *thread, HHOOK handle)
{
    struct list *p;
    struct hook_table *table = get_queue_hooks( thread );

    if (table)
    {
        int i;

        for (i=0;i<NB_HOOKS;i++) {
            LIST_FOR_EACH( p, &table->hooks[i] )
            {
                struct hook *hook = HOOK_ENTRY( p );
                if (hook->handle == handle) return hook;
            }
        }
    }
    return NULL;
}

/* get the first hook in the chain */
static INLINE struct hook *get_first_hook( struct hook_table *table, int index )
{
    struct list *elem = list_head( &table->hooks[index] );
    return elem ? HOOK_ENTRY( elem ) : NULL;
}

/* check if a given hook should run in the owner thread instead of the current thread */
static INLINE int run_hook_in_owner_thread( struct hook *hook )
{
    if ((hook->index == WH_MOUSE_LL - WH_MINHOOK ||
         hook->index == WH_KEYBOARD_LL - WH_MINHOOK))
        return hook->owner != currentThread();
    return 0;
}

/* check if a given hook should run in the current thread */
static INLINE int run_hook_in_current_thread( struct hook *hook )
{
    struct KThread* current = currentThread();
    if (hook->process && hook->process != current->process) return 0;
    if ((hook->flags & WINEVENT_SKIPOWNPROCESS) && hook->process == current->process) return 0;
    if (hook->thread && hook->thread != current) return 0;
    if ((hook->flags & WINEVENT_SKIPOWNTHREAD) && hook->thread == current) return 0;
    /* don't run low-level hooks in processes suspended for debugging */
    return 1;
}

void post_win_event( struct KThread *thread, unsigned int event, HWND win, unsigned int object_id, unsigned int child_id, void* hook_proc, const WCHAR *module, HHOOK hook);
/* find the first non-deleted hook in the chain */
static INLINE struct hook *get_first_valid_hook( struct hook_table *table, int index,
                                                 int event, HWND win,
                                                 int object_id, int child_id )
{
    struct hook *hook = get_first_hook( table, index );

    while (hook)
    {
        if (hook->proc && run_hook_in_current_thread( hook ))
        {
            if (event >= hook->event_min && event <= hook->event_max)
            {
                if (hook->flags & WINEVENT_INCONTEXT) return hook;

                /* only winevent hooks may be out of context */
                assert(hook->index + WH_MINHOOK == WH_WINEVENT);
                post_win_event( hook->owner, event, win, object_id, child_id,
                                hook->proc, hook->module, 
                                hook->handle );
            }
        }
        hook = HOOK_ENTRY( list_next( &table->hooks[index], &hook->chain ) );
    }
    return hook;
}

/* find the next hook in the chain, skipping the deleted ones */
static struct hook *get_next_hook( struct KThread *thread, struct hook *hook, int event,
                                   HWND win, int object_id, int child_id )
{
    struct hook_table *global_hooks, *table = hook->table;
    int index = hook->index;

    while ((hook = HOOK_ENTRY( list_next( &table->hooks[index], &hook->chain ) )))
    {
        if (hook->proc && run_hook_in_current_thread( hook ))
        {
            if (event >= hook->event_min && event <= hook->event_max)
            {
                if (hook->flags & WINEVENT_INCONTEXT) return hook;

                /* only winevent hooks may be out of context */
                assert(hook->index + WH_MINHOOK == WH_WINEVENT);
                post_win_event( hook->owner, event, win, object_id, child_id,
                                hook->proc, hook->module,
                                hook->handle );
            }
        }
    }
    global_hooks = get_global_hooks( thread );
    if (global_hooks && table != global_hooks)  /* now search through the global table */
    {
        hook = get_first_valid_hook( global_hooks, index, event, win, object_id, child_id );
    }
    return hook;
}

static void hook_table_destroy( struct object *obj )
{
    int i;
    struct hook *hook;
    struct hook_table *table = (struct hook_table *)obj;

    for (i = 0; i < NB_HOOKS; i++)
    {
        while ((hook = get_first_hook( table, i )) != NULL) free_hook( hook );
    }
}

/* remove a hook, freeing it if the chain is not in use */
static void remove_hook( struct hook *hook )
{
    if (hook->table->counts[hook->index])
        hook->proc = 0; /* chain is in use, just mark it and return */
    else
        free_hook( hook );
}

/* release a hook chain, removing deleted hooks if the use count drops to 0 */
static BOOL release_hook_chain( struct hook_table *table, int index )
{
    if (!table->counts[index])  /* use count shouldn't already be 0 */
    {
        return FALSE;
    }
    if (!--table->counts[index])
    {
        struct hook *hook = get_first_hook( table, index );
        while (hook)
        {
            struct hook *next = HOOK_ENTRY( list_next( &table->hooks[hook->index], &hook->chain ) );
            if (!hook->proc) free_hook( hook );
            hook = next;
        }
    }
    return TRUE;
}

/* remove all global hooks owned by a given thread */
void remove_thread_hooks( struct KThread *thread )
{
    struct hook_table *global_hooks = get_global_hooks( thread );
    int index;

    if (!global_hooks) return;

    /* only low-level keyboard/mouse global hooks can be owned by a thread */
    for (index = WH_KEYBOARD_LL - WH_MINHOOK; index <= WH_MOUSE_LL - WH_MINHOOK; index++)
    {
        struct hook *hook = get_first_hook( global_hooks, index );
        while (hook)
        {
            struct hook *next = HOOK_ENTRY( list_next( &global_hooks->hooks[index], &hook->chain ) );
            if (hook->thread == thread) remove_hook( hook );
            hook = next;
        }
    }
}

/* get a bitmap of active hooks in a hook table */
static int is_hook_active( struct hook_table *table, int index )
{
    struct hook *hook = get_first_hook( table, index );

    while (hook)
    {
        if (hook->proc && run_hook_in_current_thread( hook )) return 1;
        hook = HOOK_ENTRY( list_next( &table->hooks[index], &hook->chain ) );
    }
    return 0;
}

/* get a bitmap of all active hooks for the current thread */
unsigned int get_active_hooks(void)
{
    struct KThread* current = currentThread();
    struct hook_table *table = current->input->hooks;
    struct hook_table *global_hooks = current->input->desktop->global_hooks;
    unsigned int ret = 1 << 31;  /* set high bit to indicate that the bitmap is valid */
    int id;

    for (id = WH_MINHOOK; id <= WH_WINEVENT; id++)
    {
        if ((table && is_hook_active( table, id - WH_MINHOOK )) ||
            (global_hooks && is_hook_active( global_hooks, id - WH_MINHOOK )))
            ret |= 1 << (id - WH_MINHOOK);
    }
    return ret;
}

/* return the thread that owns the first global hook */
struct KThread *get_first_global_hook( int id )
{
    struct hook *hook;
    struct hook_table *global_hooks = currentThread()->input->desktop->global_hooks;

    if (!global_hooks) return NULL;
    if (!(hook = get_first_valid_hook( global_hooks, id - WH_MINHOOK, EVENT_MIN, 0, 0, 0 ))) return NULL;
    return hook->owner;
}

/* set a window hook */
HANDLE set_hook(void* proc, int id, DWORD pid, DWORD tid, WCHAR* module, DWORD flags, DWORD event_min, DWORD event_max, BOOL unicode)
{
    struct KProcess *process = NULL;
    struct KThread *thread = NULL;
    struct desktop *desktop;
    struct hook *hook;
    int global;

    if (!proc || id < WH_MINHOOK || id > WH_WINEVENT)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!(desktop = currentThread()->input->desktop)) return 0;

    if (pid && !(process = getProcess( pid ))) return 0;

    if (tid)
    {
        if (!(thread = getThread( tid ))) return 0;
        if (process && process != thread->process)
        {
            SetLastError_k(ERROR_INVALID_PARAMETER);
            return 0;
        }
    }

    if (id == WH_KEYBOARD_LL || id == WH_MOUSE_LL)
    {
        /* low-level hardware hooks are special: always global, but without a module */
        if (thread)
        {
            SetLastError_k(ERROR_INVALID_PARAMETER);
            return 0;
        }
        module = NULL;
        global = 1;
    }
    else if (!tid)
    {
        /* out of context hooks do not need a module handle */
        if (!module && (flags & WINEVENT_INCONTEXT))
        {
            SetLastError_k(ERROR_INVALID_PARAMETER);
            return 0;
        }
        global = 1;
    }
    else
    {
        /* module is optional only if hook is in current process */
        if (!module)
        {
            module = NULL;
            if (thread->process != currentProcess())
            {
                SetLastError_k(ERROR_INVALID_PARAMETER);
                return 0;
            }
        }        
        global = 0;
    }

    if ((hook = add_hook( desktop, thread, id - WH_MINHOOK, global )))
    {
        hook->owner = currentThread();
        hook->process = process;
        hook->event_min   = event_min;
        hook->event_max   = event_max;
        hook->flags       = flags;
        hook->proc        = proc;
        hook->unicode     = unicode;
        if (module)
            strcpyW(hook->module, module);
        return hook->handle;
    }
    return 0;
}


/* remove a window hook */
BOOL remove_window_hook(HHOOK handle, int id, void* proc)
{
    struct hook *hook;

    if (handle)
    {
        if (!(hook = find_hook_by_handle(currentThread(), handle)))
        {
            SetLastError_k(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
    }
    else
    {
        if (!proc || id < WH_MINHOOK || id > WH_WINEVENT)
        {
            SetLastError_k(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        if (!(hook = find_hook( currentThread(), id - WH_MINHOOK, proc )))
        {
            SetLastError_k(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
    }
    remove_hook( hook );
    return TRUE;
}


/* start calling a hook chain */
struct hook * start_hook_chain(int id, DWORD event, HWND window, int object_id, int child_id)
{
    struct hook *hook;
    struct KThread* current = currentThread();
    struct hook_table *table = get_queue_hooks( current );
    struct hook_table *global_table = get_global_hooks( current );

    if (id < WH_MINHOOK || id > WH_WINEVENT)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    if (!table || !(hook = get_first_valid_hook( table, id - WH_MINHOOK, event, window, object_id, child_id )))
    {
        /* try global table */
        if (!global_table || !(hook = get_first_valid_hook( global_table, id - WH_MINHOOK, event,
                                                            window, object_id, child_id )))
            return NULL;  /* no hook set */
    }
    
    if (table) table->counts[hook->index]++;
    if (global_table) global_table->counts[hook->index]++;
    return hook;
}


/* finished calling a hook chain */
void finish_hook_chain(int id)
{
    struct KThread* current = currentThread();
    struct hook_table *table = get_queue_hooks( current );
    struct hook_table *global_hooks = get_global_hooks( current );
    int index = id - WH_MINHOOK;

    if (id < WH_MINHOOK || id > WH_WINEVENT)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return;
    }
    if (table) release_hook_chain( table, index );
    if (global_hooks) release_hook_chain( global_hooks, index );
}


/* get the hook information */
struct hook* get_hook_info(HHOOK handle, BOOL get_next, DWORD event, HWND window, DWORD object_id, DWORD child_id)
{
    struct hook *hook;
    struct KThread* current = currentThread();

    if (!(hook = find_hook_by_handle(current, handle))) return NULL;
    if (hook->thread && (hook->thread != current))
    {
        SetLastError_k(ERROR_INVALID_HANDLE);
        return NULL;
    }
    if (get_next && !(hook = get_next_hook( current, hook, event, window, object_id, child_id )))
        return NULL;
    return hook;
}

#define WH_WINEVENT (WH_MAXHOOK+1)

static const char * const hook_names[WH_WINEVENT - WH_MINHOOK + 1] =
{
    "WH_MSGFILTER",
    "WH_JOURNALRECORD",
    "WH_JOURNALPLAYBACK",
    "WH_KEYBOARD",
    "WH_GETMESSAGE",
    "WH_CALLWNDPROC",
    "WH_CBT",
    "WH_SYSMSGFILTER",
    "WH_MOUSE",
    "WH_HARDWARE",
    "WH_DEBUG",
    "WH_SHELL",
    "WH_FOREGROUNDIDLE",
    "WH_CALLWNDPROCRET",
    "WH_KEYBOARD_LL",
    "WH_MOUSE_LL",
    "WH_WINEVENT"
};


/***********************************************************************
 *		get_ll_hook_timeout
 *
 */
static UINT get_ll_hook_timeout(void)
{
    /* FIXME: should retrieve LowLevelHooksTimeout in HKEY_CURRENT_USER\Control Panel\Desktop */
    return 2000;
}


/***********************************************************************
 *		set_windows_hook
 *
 * Implementation of SetWindowsHookExA and SetWindowsHookExW.
 */
static HHOOK set_windows_hook( INT id, HOOKPROC proc, HINSTANCE inst, DWORD tid, BOOL unicode )
{
    HHOOK handle = 0;
    WCHAR module[MAX_PATH];
    DWORD len;

    if (!proc)
    {
        SetLastError_k( ERROR_INVALID_FILTER_PROC );
        return 0;
    }

    if (tid)  /* thread-local hook */
    {
        if (id == WH_JOURNALRECORD ||
            id == WH_JOURNALPLAYBACK ||
            id == WH_KEYBOARD_LL ||
            id == WH_MOUSE_LL ||
            id == WH_SYSMSGFILTER)
        {
            /* these can only be global */
            SetLastError_k( ERROR_INVALID_PARAMETER );
            return 0;
        }
    }
    else  /* system-global hook */
    {
        if (id == WH_KEYBOARD_LL || id == WH_MOUSE_LL) inst = 0;
        else if (!inst)
        {
            SetLastError_k( ERROR_HOOK_NEEDS_HMOD );
            return 0;
        }
    }

    if (inst && (!(len = GetModuleFileNameW_k( inst, module, MAX_PATH )) || len >= MAX_PATH))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return 0;
    }
    return set_hook(proc, id, 0, tid, module, WINEVENT_INCONTEXT, EVENT_MIN, EVENT_MAX, unicode);
}

#ifdef __i386__
/* Some apps pass a non-stdcall proc to SetWindowsHookExA,
 * so we need a small assembly wrapper to call the proc.
 */
extern LRESULT HOOKPROC_wrapper( HOOKPROC proc,
                                 INT code, WPARAM wParam, LPARAM lParam );
__ASM_GLOBAL_FUNC( HOOKPROC_wrapper,
                   "pushl %ebp\n\t"
                   __ASM_CFI(".cfi_adjust_cfa_offset 4\n\t")
                   __ASM_CFI(".cfi_rel_offset %ebp,0\n\t")
                   "movl %esp,%ebp\n\t"
                   __ASM_CFI(".cfi_def_cfa_register %ebp\n\t")
                   "pushl %edi\n\t"
                   __ASM_CFI(".cfi_rel_offset %edi,-4\n\t")
                   "pushl %esi\n\t"
                   __ASM_CFI(".cfi_rel_offset %esi,-8\n\t")
                   "pushl %ebx\n\t"
                   __ASM_CFI(".cfi_rel_offset %ebx,-12\n\t")
                   "pushl 20(%ebp)\n\t"
                   "pushl 16(%ebp)\n\t"
                   "pushl 12(%ebp)\n\t"
                   "movl 8(%ebp),%eax\n\t"
                   "call *%eax\n\t"
                   "leal -12(%ebp),%esp\n\t"
                   "popl %ebx\n\t"
                   __ASM_CFI(".cfi_same_value %ebx\n\t")
                   "popl %esi\n\t"
                   __ASM_CFI(".cfi_same_value %esi\n\t")
                   "popl %edi\n\t"
                   __ASM_CFI(".cfi_same_value %edi\n\t")
                   "leave\n\t"
                   __ASM_CFI(".cfi_def_cfa %esp,4\n\t")
                   __ASM_CFI(".cfi_same_value %ebp\n\t")
                   "ret" )
#else
static INLINE LRESULT HOOKPROC_wrapper( HOOKPROC proc,
                                 INT code, WPARAM wParam, LPARAM lParam )
{
    return proc( code, wParam, lParam );
}
#endif  /* __i386__ */


/***********************************************************************
 *		call_hook_AtoW
 */
static LRESULT call_hook_AtoW( HOOKPROC proc, INT id, INT code, WPARAM wparam, LPARAM lparam )
{
    LRESULT ret;
    UNICODE_STRING usBuffer;
    if (id != WH_CBT || code != HCBT_CREATEWND)
        ret = HOOKPROC_wrapper( proc, code, wparam, lparam );
    else
    {
        CBT_CREATEWNDA *cbtcwA = (CBT_CREATEWNDA *)lparam;
        CBT_CREATEWNDW cbtcwW;
        CREATESTRUCTW csW;
        LPWSTR nameW = NULL;
        LPWSTR classW = NULL;

        cbtcwW.lpcs = &csW;
        cbtcwW.hwndInsertAfter = cbtcwA->hwndInsertAfter;
        csW = *(CREATESTRUCTW *)cbtcwA->lpcs;

        if (!IS_INTRESOURCE(cbtcwA->lpcs->lpszName))
        {
            RtlCreateUnicodeStringFromAsciiz_k(&usBuffer,cbtcwA->lpcs->lpszName);
            csW.lpszName = nameW = usBuffer.Buffer;
        }
        if (!IS_INTRESOURCE(cbtcwA->lpcs->lpszClass))
        {
            RtlCreateUnicodeStringFromAsciiz_k(&usBuffer,cbtcwA->lpcs->lpszClass);
            csW.lpszClass = classW = usBuffer.Buffer;
        }
        ret = HOOKPROC_wrapper( proc, code, wparam, (LPARAM)&cbtcwW );
        cbtcwA->hwndInsertAfter = cbtcwW.hwndInsertAfter;
        HeapFree_k( GetProcessHeap_k(), 0, nameW );
        HeapFree_k( GetProcessHeap_k(), 0, classW );
    }
    return ret;
}


/***********************************************************************
 *		call_hook_WtoA
 */
static LRESULT call_hook_WtoA( HOOKPROC proc, INT id, INT code, WPARAM wparam, LPARAM lparam )
{
    LRESULT ret;

    if (id != WH_CBT || code != HCBT_CREATEWND)
        ret = HOOKPROC_wrapper( proc, code, wparam, lparam );
    else
    {
        CBT_CREATEWNDW *cbtcwW = (CBT_CREATEWNDW *)lparam;
        CBT_CREATEWNDA cbtcwA;
        CREATESTRUCTA csA;
        int len;
        LPSTR nameA = NULL;
        LPSTR classA = NULL;

        cbtcwA.lpcs = &csA;
        cbtcwA.hwndInsertAfter = cbtcwW->hwndInsertAfter;
        csA = *(CREATESTRUCTA *)cbtcwW->lpcs;

        if (!IS_INTRESOURCE(cbtcwW->lpcs->lpszName)) {
            len = WideCharToMultiByte_k( CP_ACP, 0, cbtcwW->lpcs->lpszName, -1, NULL, 0, NULL, NULL );
            nameA = HeapAlloc_k(GetProcessHeap_k(), 0, len*sizeof(CHAR) );
            WideCharToMultiByte_k( CP_ACP, 0, cbtcwW->lpcs->lpszName, -1, nameA, len, NULL, NULL );
            csA.lpszName = nameA;
        }

        if (!IS_INTRESOURCE(cbtcwW->lpcs->lpszClass)) {
            len = WideCharToMultiByte_k( CP_ACP, 0, cbtcwW->lpcs->lpszClass, -1, NULL, 0, NULL, NULL );
            classA = HeapAlloc_k(GetProcessHeap_k(), 0, len*sizeof(CHAR) );
            WideCharToMultiByte_k( CP_ACP, 0, cbtcwW->lpcs->lpszClass, -1, classA, len, NULL, NULL );
            csA.lpszClass = classA;
        }

        ret = HOOKPROC_wrapper( proc, code, wparam, (LPARAM)&cbtcwA );
        cbtcwW->hwndInsertAfter = cbtcwA.hwndInsertAfter;
        HeapFree_k( GetProcessHeap_k(), 0, nameA );
        HeapFree_k( GetProcessHeap_k(), 0, classA );
    }
    return ret;
}


/***********************************************************************
 *		call_hook_proc
 */
static LRESULT call_hook_proc( HOOKPROC proc, INT id, INT code, WPARAM wparam, LPARAM lparam,
                               BOOL prev_unicode, BOOL next_unicode )
{
    LRESULT ret;

    if (!prev_unicode == !next_unicode) ret = proc( code, wparam, lparam );
    else if (prev_unicode) ret = call_hook_WtoA( proc, id, code, wparam, lparam );
    else ret = call_hook_AtoW( proc, id, code, wparam, lparam );

    return ret;
}


/***********************************************************************
 *		get_hook_proc
 *
 * Retrieve the hook procedure real value for a module-relative proc
 */
void *get_hook_proc( void *proc, const WCHAR *module )
{
    HMODULE mod;

    if (!(mod = GetModuleHandleW_k(module)))
    {
        TRACE( "loading %s\n", debugstr_w(module) );
        /* FIXME: the library will never be freed */
        if (!(mod = LoadLibraryExW_k(module, NULL, LOAD_WITH_ALTERED_SEARCH_PATH))) return NULL;
    }
    return (char *)mod + (ULONG_PTR)proc;
}

/***********************************************************************
 *		call_hook
 *
 * Call hook either in current thread or send message to the destination
 * thread.
 */
struct user_thread_info* get_user_thread_info();
static LRESULT call_hook( struct hook_info *info, INT code, WPARAM wparam, LPARAM lparam )
{
    DWORD_PTR ret = 0;

    if (info->tid)
    {
        struct hook_extra_info h_extra;
        h_extra.handle = info->handle;
        h_extra.lparam = lparam;

        TRACE( "calling hook in thread %04x %s code %x wp %lx lp %lx\n",
               info->tid, hook_names[info->id-WH_MINHOOK], code, wparam, lparam );

        switch(info->id)
        {
        case WH_KEYBOARD_LL:
            MSG_SendInternalMessageTimeout( info->pid, info->tid, WM_WINE_KEYBOARD_LL_HOOK,
                                            wparam, (LPARAM)&h_extra, SMTO_ABORTIFHUNG,
                                            get_ll_hook_timeout(), &ret );
            break;
        case WH_MOUSE_LL:
            MSG_SendInternalMessageTimeout( info->pid, info->tid, WM_WINE_MOUSE_LL_HOOK,
                                            wparam, (LPARAM)&h_extra, SMTO_ABORTIFHUNG,
                                            get_ll_hook_timeout(), &ret );
            break;
        default:
            ERR("Unknown hook id %d\n", info->id);
            assert(0);
            break;
        }
    }
    else if (info->proc)
    {
        TRACE( "calling hook %p %s code %x wp %lx lp %lx module %s\n",
               info->proc, hook_names[info->id-WH_MINHOOK], code, wparam,
               lparam, debugstr_w(info->module) );

        if (!info->module[0] ||
            (info->proc = get_hook_proc( info->proc, info->module )) != NULL)
        {
            struct user_thread_info *thread_info = get_user_thread_info();
            HHOOK prev = thread_info->hook;
            BOOL prev_unicode = thread_info->hook_unicode;

            thread_info->hook = info->handle;
            thread_info->hook_unicode = info->next_unicode;
            ret = call_hook_proc( info->proc, info->id, code, wparam, lparam,
                                  info->prev_unicode, info->next_unicode );
            thread_info->hook = prev;
            thread_info->hook_unicode = prev_unicode;
        }
    }

    if (info->id == WH_KEYBOARD_LL || info->id == WH_MOUSE_LL)
        get_user_thread_info()->key_state_time = 0;  /* force refreshing the key state cache */

    return ret;
}


/***********************************************************************
 *           HOOK_IsHooked
 */
static BOOL HOOK_IsHooked( INT id )
{
    struct user_thread_info *thread_info = get_user_thread_info();

    if (!thread_info->active_hooks) return TRUE;
    return (thread_info->active_hooks & (1 << (id - WH_MINHOOK))) != 0;
}


/***********************************************************************
 *		HOOK_CallHooks
 */
LRESULT HOOK_CallHooks( INT id, INT code, WPARAM wparam, LPARAM lparam, BOOL unicode )
{
    struct user_thread_info *thread_info = get_user_thread_info();
    struct hook* hook;
    struct hook_info info;
    DWORD_PTR ret = 0;

    USER_CheckNotLock();

    if (!HOOK_IsHooked( id ))
    {
        TRACE( "skipping hook %s mask %x\n", hook_names[id-WH_MINHOOK], thread_info->active_hooks );
        return 0;
    }

    memset(&info, 0, sizeof(struct hook_info));
    info.prev_unicode = unicode;
    info.id = id;

    hook = start_hook_chain(id, EVENT_MIN, 0, 0, 0);
    if (hook) {
        strcpyW(info.module, hook->module);
        info.handle = hook->handle;
        info.proc = hook->proc;
        info.next_unicode = hook->unicode;
        if (run_hook_in_owner_thread( hook ))
        {
            info.pid          = hook->owner->process->id;
            info.tid          = hook->owner->id;
        }
        else
        {
            info.pid          = 0;
            info.tid          = 0;
        }
    }

    if (!info.tid && !info.proc) return 0;
    ret = call_hook( &info, code, wparam, lparam );
    finish_hook_chain(id);
    return ret;
}


/***********************************************************************
 *		SetWindowsHookA (USER32.@)
 */
HHOOK WINAPI SetWindowsHookA_k( INT id, HOOKPROC proc )
{
    return SetWindowsHookExA_k( id, proc, 0, GetCurrentThreadId_k() );
}


/***********************************************************************
 *		SetWindowsHookW (USER32.@)
 */
HHOOK WINAPI SetWindowsHookW_k( INT id, HOOKPROC proc )
{
    return SetWindowsHookExW_k( id, proc, 0, GetCurrentThreadId_k() );
}


/***********************************************************************
 *		SetWindowsHookExA (USER32.@)
 */
HHOOK WINAPI SetWindowsHookExA_k( INT id, HOOKPROC proc, HINSTANCE inst, DWORD tid )
{
    return set_windows_hook( id, proc, inst, tid, FALSE );
}

/***********************************************************************
 *		SetWindowsHookExW (USER32.@)
 */
HHOOK WINAPI SetWindowsHookExW_k( INT id, HOOKPROC proc, HINSTANCE inst, DWORD tid )
{
    return set_windows_hook( id, proc, inst, tid, TRUE );
}


/***********************************************************************
 *		UnhookWindowsHook (USER32.@)
 */
BOOL WINAPI UnhookWindowsHook_k( INT id, HOOKPROC proc )
{
    BOOL ret = remove_window_hook(0, id, proc);
    if (!ret && GetLastError_k() == ERROR_INVALID_HANDLE) SetLastError_k( ERROR_INVALID_HOOK_HANDLE );
    return ret;
}



/***********************************************************************
 *		UnhookWindowsHookEx (USER32.@)
 */
BOOL WINAPI UnhookWindowsHookEx_k( HHOOK hhook )
{
    BOOL ret = remove_window_hook(hhook, 0, NULL);
    if (!ret && GetLastError_k() == ERROR_INVALID_HANDLE) SetLastError_k( ERROR_INVALID_HOOK_HANDLE );
    return ret;
}


/***********************************************************************
 *		CallNextHookEx (USER32.@)
 */
LRESULT WINAPI CallNextHookEx_k( HHOOK hhook, INT code, WPARAM wparam, LPARAM lparam )
{
    struct user_thread_info *thread_info = get_user_thread_info();
    struct hook_info info;
    struct hook* hook;

    memset(&info, 0, sizeof(struct hook_info));
    hook = get_hook_info(hhook, 1, EVENT_MIN, 0, 0, 0);
    if (hook) {
        strcpyW(info.module, hook->module);
        info.handle = hook->handle;
        info.proc = hook->proc;
        info.id = hook->index + WH_MINHOOK;
        info.next_unicode = hook->unicode;
        if (run_hook_in_owner_thread( hook ))
        {
            info.pid          = hook->owner->process->id;
            info.tid          = hook->owner->id;
        }
        else
        {
            info.pid          = 0;
            info.tid          = 0;
        }
    }
    info.prev_unicode = thread_info->hook_unicode;
    return call_hook( &info, code, wparam, lparam );
}


LRESULT call_current_hook( HHOOK hhook, INT code, WPARAM wparam, LPARAM lparam )
{
    struct hook_info info;
    struct hook* hook;

    memset(&info, 0, sizeof(struct hook_info));
    hook = get_hook_info(hhook, 0, EVENT_MIN, 0, 0, 0);
    if (hook) {
        strcpyW(info.module, hook->module);
        info.handle = hook->handle;
        info.proc = hook->proc;
        info.next_unicode = hook->unicode;
        info.id = hook->index + WH_MINHOOK;
        if (run_hook_in_owner_thread( hook ))
        {
            info.pid          = hook->owner->process->id;
            info.tid          = hook->owner->id;
        }
        else
        {
            info.pid          = 0;
            info.tid          = 0;
        }
    }
    info.prev_unicode = TRUE;  /* assume Unicode for this function */
    return call_hook( &info, code, wparam, lparam );
}

/***********************************************************************
 *		CallMsgFilterA (USER32.@)
 */
BOOL WINAPI CallMsgFilterA_k( LPMSG msg, INT code )
{
    if (HOOK_CallHooks( WH_SYSMSGFILTER, code, 0, (LPARAM)msg, FALSE )) return TRUE;
    return HOOK_CallHooks( WH_MSGFILTER, code, 0, (LPARAM)msg, FALSE );
}


/***********************************************************************
 *		CallMsgFilterW (USER32.@)
 */
BOOL WINAPI CallMsgFilterW_k( LPMSG msg, INT code )
{
    if (HOOK_CallHooks( WH_SYSMSGFILTER, code, 0, (LPARAM)msg, TRUE )) return TRUE;
    return HOOK_CallHooks( WH_MSGFILTER, code, 0, (LPARAM)msg, TRUE );
}


/***********************************************************************
 *           SetWinEventHook                            [USER32.@]
 *
 * Set up an event hook for a set of events.
 *
 * PARAMS
 *  event_min [I] Lowest event handled by pfnProc
 *  event_max [I] Highest event handled by pfnProc
 *  inst      [I] DLL containing pfnProc
 *  proc      [I] Callback event hook function
 *  pid       [I] Process to get events from, or 0 for all processes
 *  tid       [I] Thread to get events from, or 0 for all threads
 *  flags     [I] Flags indicating the status of pfnProc
 *
 * RETURNS
 *  Success: A handle representing the hook.
 *  Failure: A NULL handle.
 */
HWINEVENTHOOK WINAPI SetWinEventHook_k(DWORD event_min, DWORD event_max,
                                     HMODULE inst, WINEVENTPROC proc,
                                     DWORD pid, DWORD tid, DWORD flags)
{
    WCHAR module[MAX_PATH];
    DWORD len;

    TRACE("%d,%d,%p,%p,%08x,%04x,%08x\n", event_min, event_max, inst,
          proc, pid, tid, flags);

    if (inst)
    {
        if (!(len = GetModuleFileNameW_k(inst, module, MAX_PATH)) || len >= MAX_PATH)
            inst = 0;
    }

    if ((flags & WINEVENT_INCONTEXT) && !inst)
    {
        SetLastError_k(ERROR_HOOK_NEEDS_HMOD);
        return 0;
    }

    if (event_min > event_max)
    {
        SetLastError_k(ERROR_INVALID_HOOK_FILTER);
        return 0;
    }

    /* FIXME: what if the tid or pid belongs to another process? */
    if (tid)  /* thread-local hook */
        inst = 0;

    return set_hook(proc, WH_WINEVENT, pid, tid, (inst?module:NULL), flags, event_min, event_max, 1);
}


/***********************************************************************
 *           UnhookWinEvent                             [USER32.@]
 *
 * Remove an event hook for a set of events.
 *
 * PARAMS
 *  hEventHook [I] Event hook to remove
 *
 * RETURNS
 *  Success: TRUE. The event hook has been removed.
 *  Failure: FALSE, if hEventHook is invalid.
 */
BOOL WINAPI UnhookWinEvent_k(HWINEVENTHOOK hEventHook)
{
    return remove_window_hook((HHOOK)hEventHook, WH_WINEVENT, 0);
}

static INLINE BOOL find_first_hook(DWORD id, DWORD event, HWND hwnd, LONG object_id,
                                   LONG child_id, struct hook_info *info)
{
    struct user_thread_info *thread_info = get_user_thread_info();
    struct hook* hook;

    if (!HOOK_IsHooked( id ))
    {
        TRACE( "skipping hook %s mask %x\n", hook_names[id-WH_MINHOOK], thread_info->active_hooks );
        return FALSE;
    }
    hook = start_hook_chain(id, event, hwnd, object_id, child_id);
    if (hook) {
        strcpyW(info->module, hook->module);
        info->handle = hook->handle;
        info->proc = hook->proc;
        if (run_hook_in_owner_thread( hook ))
        {
            info->tid          = hook->owner->id;
        }
        else
        {
            info->tid          = 0;
        }

    }
    return hook && (info->tid || info->proc);
}

static INLINE BOOL find_next_hook(DWORD event, HWND hwnd, LONG object_id,
                                  LONG child_id, struct hook_info *info)
{
    struct hook* hook = get_hook_info(info->handle, 1, event, hwnd, object_id, child_id);
    if (hook) {
        strcpyW(info->module, hook->module);
        info->handle = hook->handle;
        info->proc = hook->proc;
        if (run_hook_in_owner_thread( hook ))
        {
            info->tid          = hook->owner->id;
        }
        else
        {
            info->tid          = 0;
        }
    }
    return hook!=NULL;
}

static INLINE void find_hook_close(DWORD id)
{
    finish_hook_chain(id);
}

/***********************************************************************
 *           NotifyWinEvent                             [USER32.@]
 *
 * Inform the OS that an event has occurred.
 *
 * PARAMS
 *  event     [I] Id of the event
 *  hwnd      [I] Window holding the object that created the event
 *  object_id [I] Type of object that created the event
 *  child_id  [I] Child object of nId, or CHILDID_SELF.
 *
 * RETURNS
 *  Nothing.
 */
void WINAPI NotifyWinEvent_k(DWORD event, HWND hwnd, LONG object_id, LONG child_id)
{
    struct hook_info info;

    TRACE("%04x,%p,%d,%d\n", event, hwnd, object_id, child_id);

    if (!hwnd)
    {
        SetLastError_k(ERROR_INVALID_WINDOW_HANDLE);
        return;
    }

    USER_CheckNotLock();

#if 0
    if (event & 0x80000000)
    {
        /* FIXME: on 64-bit platforms we need to invent some other way for
         * passing parameters, nId and nChildId can't hold full [W|L]PARAM.
         * struct call_hook *hook = (LRESULT *)hWnd;
         * wparam = hook->wparam;
         * lparam = hook->lparam;
         */
        LRESULT *ret = (LRESULT *)hwnd;
        INT id, code, unicode;

        id = (dwEvent & 0x7fff0000) >> 16;
        code = event & 0x7fff;
        unicode = event & 0x8000;
        *ret = HOOK_CallHooks(id, code, object_id, child_id, unicode);
        return;
    }
#endif

    if (!find_first_hook(WH_WINEVENT, event, hwnd, object_id, child_id, &info)) return;

    do
    {
        WINEVENTPROC proc = info.proc;
        if (proc)
        {
            TRACE( "calling WH_WINEVENT hook %p event %x hwnd %p %x %x module %s\n",
                   proc, event, hwnd, object_id, child_id, debugstr_w(info.module) );

            if (!info.module[0] || (proc = get_hook_proc( proc, info.module )) != NULL)
            {

                proc( info.handle, event, hwnd, object_id, child_id,
                      GetCurrentThreadId_k(), GetCurrentTime_k());

            }
        }
        else
            break;
    }
    while (find_next_hook(event, hwnd, object_id, child_id, &info));

    find_hook_close(WH_WINEVENT);
}


/***********************************************************************
 *           IsWinEventHookInstalled                       [USER32.@]
 *
 * Determine if an event hook is installed for an event.
 *
 * PARAMS
 *  dwEvent  [I] Id of the event
 *
 * RETURNS
 *  TRUE,  If there are any hooks installed for the event.
 *  FALSE, Otherwise.
 *
 * BUGS
 *  Not implemented.
 */
BOOL WINAPI IsWinEventHookInstalled_k(DWORD dwEvent)
{
    /* FIXME: Needed by Office 2007 installer */
    WARN("(%d)-stub!\n", dwEvent);
    return TRUE;
}

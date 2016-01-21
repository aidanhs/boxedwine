/*
 * Window messaging support
 *
 * Copyright 2001 Alexandre Julliard
 * Copyright 2008 Maarten Lankhorst
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

#include "../include/winbase.h"
#include "../include/unicode.h"
#include "../include/ntstatus.h"

#include "user_private.h"
#include "win.h"
#include "controls.h"
#include "kthread.h"
#include "kprocess.h"
#include "../include/list.h"
#include "kalloc.h"

unsigned int get_active_hooks(void);

#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#define WM_NCMOUSELAST  (WM_NCMOUSEFIRST+(WM_MOUSELAST-WM_MOUSEFIRST))

#define MAX_PACK_COUNT 4

#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#define WM_NCMOUSELAST  (WM_NCMOUSEFIRST+(WM_MOUSELAST-WM_MOUSEFIRST))

#define MAX_PACK_COUNT 4

enum message_kind { SEND_MESSAGE, POST_MESSAGE };
#define NB_MSG_KINDS (POST_MESSAGE+1)

DWORD last_input_time;

struct message_result
{
    struct list            sender_entry;  /* entry in sender list */
    struct message        *msg;           /* message the result is for */
    struct message_result *recv_next;     /* next in receiver list */
    struct msg_queue      *sender;        /* sender queue */
    struct msg_queue      *receiver;      /* receiver queue */
    int                    replied;       /* has it been replied to? */
    unsigned int           error;         /* error code to pass back to sender */
    LPARAM                 result;        /* reply result */
    struct message        *hardware_msg;  /* hardware message if low-level hook result */
    struct desktop        *desktop;       /* desktop for hardware message */
    struct message        *callback_msg;  /* message to queue for callback */
    SDL_TimerID            timeout;       /* result timeout */
};

struct timer
{
    struct list     entry;     /* entry in timer list */
    DWORD           when;      /* next expiration */
    unsigned int    rate;      /* timer rate in ms */
    HWND            win;       /* window handle */
    unsigned int    msg;       /* message to post */
    LPARAM          id;        /* timer id */
    LPARAM          lparam;    /* lparam for message */
};

struct msg_queue
{
    HANDLE                 handle;             /* object header */
    struct fd             *fd;              /* optional file descriptor to poll */
    unsigned int           wake_bits;       /* wakeup bits */
    unsigned int           wake_mask;       /* wakeup mask */
    unsigned int           changed_bits;    /* changed wakeup bits */
    unsigned int           changed_mask;    /* changed wakeup mask */
    int                    paint_count;     /* pending paint messages count */
    int                    hotkey_count;    /* pending hotkey messages count */
    int                    quit_message;    /* is there a pending quit message? */
    int                    exit_code;       /* exit code of pending quit message */
    int                    cursor_count;    /* per-queue cursor show count */
    struct list            msg_list[NB_MSG_KINDS];  /* lists of messages */
    struct list            send_result;     /* stack of sent messages waiting for result */
    struct list            callback_result; /* list of callback messages waiting for result */
    struct message_result *recv_result;     /* stack of received messages waiting for result */
    struct list            pending_timers;  /* list of pending timers */
    struct list            expired_timers;  /* list of expired timers */
    LPARAM                 next_timer_id;   /* id for the next timer with a 0 window */
    SDL_TimerID            timeout;         /* timeout for next timer to expire */
    struct thread_input   *input;           /* thread input descriptor */
    struct hook_table     *hooks;           /* hook table */
    LPARAM                 last_get_msg;    /* time of last get message call */
};

/* the various structures that can be sent in messages, in platform-independent layout */
struct packed_CREATESTRUCTW
{
    ULONGLONG     lpCreateParams;
    ULONGLONG     hInstance;
    HMENU hMenu;
    DWORD         __pad1;
    HWND hwndParent;
    DWORD         __pad2;
    INT           cy;
    INT           cx;
    INT           y;
    INT           x;
    LONG          style;
    ULONGLONG     lpszName;
    ULONGLONG     lpszClass;
    DWORD         dwExStyle;
    DWORD         __pad3;
};

struct packed_DRAWITEMSTRUCT
{
    UINT          CtlType;
    UINT          CtlID;
    UINT          itemID;
    UINT          itemAction;
    UINT          itemState;
    HWND hwndItem;
    DWORD         __pad1;
    HDC hDC;
    DWORD         __pad2;
    RECT          rcItem;
    ULONGLONG     itemData;
};

struct packed_MEASUREITEMSTRUCT
{
    UINT          CtlType;
    UINT          CtlID;
    UINT          itemID;
    UINT          itemWidth;
    UINT          itemHeight;
    ULONGLONG     itemData;
};

struct packed_DELETEITEMSTRUCT
{
    UINT          CtlType;
    UINT          CtlID;
    UINT          itemID;
    HWND hwndItem;
    DWORD         __pad;
    ULONGLONG     itemData;
};

struct packed_COMPAREITEMSTRUCT
{
    UINT          CtlType;
    UINT          CtlID;
    HWND hwndItem;
    DWORD         __pad1;
    UINT          itemID1;
    ULONGLONG     itemData1;
    UINT          itemID2;
    ULONGLONG     itemData2;
    DWORD         dwLocaleId;
    DWORD         __pad2;
};

struct packed_WINDOWPOS
{
    HWND hwnd;
    DWORD         __pad1;
    HWND hwndInsertAfter;
    DWORD         __pad2;
    INT           x;
    INT           y;
    INT           cx;
    INT           cy;
    UINT          flags;
    DWORD         __pad3;
};

struct packed_COPYDATASTRUCT
{
    ULONGLONG dwData;
    DWORD     cbData;
    ULONGLONG lpData;
};

struct packed_HELPINFO
{
    UINT          cbSize;
    INT           iContextType;
    INT           iCtrlId;
    HANDLE hItemHandle;
    DWORD         __pad;
    ULONGLONG     dwContextId;
    POINT         MousePos;
};

struct packed_NCCALCSIZE_PARAMS
{
    RECT          rgrc[3];
    ULONGLONG     __pad1;
    HWND hwnd;
    DWORD         __pad2;
    HWND hwndInsertAfter;
    DWORD         __pad3;
    INT           x;
    INT           y;
    INT           cx;
    INT           cy;
    UINT          flags;
    DWORD         __pad4;
};

struct packed_MSG
{
    HWND hwnd;
    DWORD         __pad1;
    UINT          message;
    ULONGLONG     wParam;
    ULONGLONG     lParam;
    DWORD         time;
    POINT         pt;
    DWORD         __pad2;
};

struct packed_MDINEXTMENU
{
    HMENU hmenuIn;
    DWORD         __pad1;
    HMENU hmenuNext;
    DWORD         __pad2;
    HWND hwndNext;
    DWORD         __pad3;
};

struct packed_MDICREATESTRUCTW
{
    ULONGLONG szClass;
    ULONGLONG szTitle;
    ULONGLONG hOwner;
    INT       x;
    INT       y;
    INT       cx;
    INT       cy;
    DWORD     style;
    ULONGLONG lParam;
};

struct packed_hook_extra_info
{
    HANDLE handle;
    DWORD         __pad;
    ULONGLONG     lparam;
};

union packed_structs
{
    struct packed_CREATESTRUCTW cs;
    struct packed_DRAWITEMSTRUCT dis;
    struct packed_MEASUREITEMSTRUCT mis;
    struct packed_DELETEITEMSTRUCT dls;
    struct packed_COMPAREITEMSTRUCT cis;
    struct packed_WINDOWPOS wp;
    struct packed_COPYDATASTRUCT cds;
    struct packed_HELPINFO hi;
    struct packed_NCCALCSIZE_PARAMS ncp;
    struct packed_MSG msg;
    struct packed_MDINEXTMENU mnm;
    struct packed_MDICREATESTRUCTW mcs;
    struct packed_hook_extra_info hook;
};

/* description of the data fields that need to be packed along with a sent message */
struct packed_message
{
    union packed_structs ps;
    int                  count;
    const void          *data[MAX_PACK_COUNT];
    size_t               size[MAX_PACK_COUNT];
};

typedef union
{
    unsigned char            bytes[1];
    struct hardware_msg_data hardware;
    struct callback_msg_data callback;
    struct winevent_msg_data winevent;
    struct packed_message packed;
} message_data_t;

struct message
{
    struct list            entry;     /* entry in message list */
    enum message_type      type;      /* message type */
    HWND                   win;       /* window handle */
    unsigned int           msg;       /* message code */
    WPARAM                 wparam;    /* parameters */
    LPARAM                 lparam;    /* parameters */
    unsigned int           time;      /* message time */
    message_data_t         data;      /* message data for sent messages */
    unsigned int           unique_id; /* unique id for nested hw message waits */
    struct message_result *result;    /* result in sender queue */
};

/* info about the message currently being received by the current thread */
struct received_message_info
{
    enum message_type type;
    MSG               msg;
    UINT              flags;  /* InSendMessageEx return flags */
};

/* structure to group all parameters for sent messages of the various kinds */
struct send_message_info
{
    enum message_type type;
    DWORD             dest_tid;
    HWND              hwnd;
    UINT              msg;
    WPARAM            wparam;
    LPARAM            lparam;
    UINT              flags;      /* flags for SendMessageTimeout */
    UINT              timeout;    /* timeout for SendMessageTimeout */
    SENDASYNCPROC     callback;   /* callback function for SendMessageCallback */
    ULONG_PTR         data;       /* callback data */
    enum wm_char_mapping wm_char;
};

struct hotkey
{
    struct list         entry;        /* entry in desktop hotkey list */
    struct msg_queue   *queue;        /* queue owning this hotkey */
    HWND                win;          /* window handle */
    int                 id;           /* hotkey id */
    unsigned int        vkey;         /* virtual key code */
    unsigned int        flags;        /* key modifiers */
};

static void free_message( struct message *msg );
static void free_result( struct message_result *result );
static void restart_timer_queue( struct msg_queue *queue, struct timer *timer );
static void release_hardware_message( struct msg_queue *queue, unsigned int hw_id, int remove, HWND new_win );

static void set_caret_window( struct thread_input *input, HWND win )
{
    if (!win || win != input->caret)
    {
        input->caret_rect.left   = 0;
        input->caret_rect.top    = 0;
        input->caret_rect.right  = 0;
        input->caret_rect.bottom = 0;
    }
    input->caret             = win;
    input->caret_hide        = 1;
    input->caret_state       = 0;
}

void get_queue_status(BOOL clear, DWORD* wake_bits, DWORD* changed_bits)
{
    struct msg_queue *queue = currentThread()->queue;    

    if (queue)
    {
        *wake_bits    = queue->wake_bits;
        *changed_bits = queue->changed_bits;
        if (clear) queue->changed_bits = 0;
    }
    else {
        *wake_bits = 0;
        *changed_bits = 0;
    }
}

/* create a thread input object */
static struct thread_input *create_thread_input( struct KThread *thread )
{
    struct thread_input *input;

    if ((input = kalloc(sizeof(struct thread_input))))
    {
        input->focus        = 0;
        input->capture      = 0;
        input->active       = 0;
        input->menu_owner   = 0;
        input->move_size    = 0;
        input->cursor       = 0;
        input->cursor_count = 0;
        list_init( &input->msg_list );
        set_caret_window( input, 0 );
        memset( input->keystate, 0, sizeof(input->keystate) );
        thread->input = input;
        //if (!(input->desktop = get_thread_desktop( thread, 0 /* FIXME: access rights */ )))
        //{
        //    return NULL;
        //}
    }
    return input;
}


/* create a message queue object */
static struct msg_queue *create_msg_queue( struct KThread *thread, struct thread_input *input )
{
    struct thread_input *new_input = NULL;
    struct msg_queue *queue;
    int i;

    if (!input)
    {
        if (!(new_input = create_thread_input( thread ))) return NULL;
        input = new_input;
    }

    if ((queue = kalloc(sizeof(struct msg_queue))))
    {
        queue->fd              = NULL;
        queue->wake_bits       = 0;
        queue->wake_mask       = 0;
        queue->changed_bits    = 0;
        queue->changed_mask    = 0;
        queue->paint_count     = 0;
        queue->hotkey_count    = 0;
        queue->quit_message    = 0;
        queue->cursor_count    = 0;
        queue->recv_result     = NULL;
        queue->next_timer_id   = 0x7fff;
        queue->timeout         = NULL;
        queue->input           = input;
        queue->hooks           = NULL;
        queue->last_get_msg    = GetTickCount_k();
        list_init( &queue->send_result );
        list_init( &queue->callback_result );
        list_init( &queue->pending_timers );
        list_init( &queue->expired_timers );
        for (i = 0; i < NB_MSG_KINDS; i++) list_init( &queue->msg_list[i] );

        thread->queue = queue;
    }
    return queue;
}


static struct KThread* get_window_thread(HWND hwnd) {
    WND* wnd = WIN_GetPtr(hwnd);
    return getThread(wnd->tid);
}

/* change the input key state for a given key */
static void set_input_key_state( unsigned char *keystate, unsigned char key, int down )
{
    if (down)
    {
        if (!(keystate[key] & 0x80)) keystate[key] ^= 0x01;
        keystate[key] |= down;
    }
    else keystate[key] &= ~0x80;
}

/* update the input key state for a keyboard message */
static void update_input_key_state( struct desktop *desktop, unsigned char *keystate,
                                    const struct message *msg )
{
    unsigned char key;
    int down = 0;

    switch (msg->msg)
    {
    case WM_LBUTTONDOWN:
        down = (keystate == desktop->keystate) ? 0xc0 : 0x80;
        /* fall through */
    case WM_LBUTTONUP:
        set_input_key_state( keystate, VK_LBUTTON, down );
        break;
    case WM_MBUTTONDOWN:
        down = (keystate == desktop->keystate) ? 0xc0 : 0x80;
        /* fall through */
    case WM_MBUTTONUP:
        set_input_key_state( keystate, VK_MBUTTON, down );
        break;
    case WM_RBUTTONDOWN:
        down = (keystate == desktop->keystate) ? 0xc0 : 0x80;
        /* fall through */
    case WM_RBUTTONUP:
        set_input_key_state( keystate, VK_RBUTTON, down );
        break;
    case WM_XBUTTONDOWN:
        down = (keystate == desktop->keystate) ? 0xc0 : 0x80;
        /* fall through */
    case WM_XBUTTONUP:
        if (msg->wparam >> 16 == XBUTTON1) set_input_key_state( keystate, VK_XBUTTON1, down );
        else if (msg->wparam >> 16 == XBUTTON2) set_input_key_state( keystate, VK_XBUTTON2, down );
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        down = (keystate == desktop->keystate) ? 0xc0 : 0x80;
        /* fall through */
    case WM_KEYUP:
    case WM_SYSKEYUP:
        key = (unsigned char)msg->wparam;
        set_input_key_state( keystate, key, down );
        switch(key)
        {
        case VK_LCONTROL:
        case VK_RCONTROL:
            down = (keystate[VK_LCONTROL] | keystate[VK_RCONTROL]) & 0x80;
            set_input_key_state( keystate, VK_CONTROL, down );
            break;
        case VK_LMENU:
        case VK_RMENU:
            down = (keystate[VK_LMENU] | keystate[VK_RMENU]) & 0x80;
            set_input_key_state( keystate, VK_MENU, down );
            break;
        case VK_LSHIFT:
        case VK_RSHIFT:
            down = (keystate[VK_LSHIFT] | keystate[VK_RSHIFT]) & 0x80;
            set_input_key_state( keystate, VK_SHIFT, down );
            break;
        }
        break;
    }
}

/* check the queue status */
static INLINE int is_signaled( struct msg_queue *queue )
{
    return ((queue->wake_bits & queue->wake_mask) || (queue->changed_bits & queue->changed_mask));
}

/* set the hook table for a given thread, allocating the queue if needed */
void set_queue_hooks( struct KThread *thread, struct hook_table *hooks )
{
    struct msg_queue *queue = thread->queue;
    if (!queue && !(queue = create_msg_queue( thread, NULL ))) return;
    queue->hooks = hooks;
}

/* get the hook table for a given thread */
struct hook_table *get_queue_hooks( struct KThread *thread )
{
    if (!thread->queue) return NULL;
    return thread->queue->hooks;
}

/* set some queue bits */
static INLINE void set_queue_bits( struct msg_queue *queue, unsigned int bits )
{
    queue->wake_bits |= bits;
    queue->changed_bits |= bits;
    if (is_signaled( queue )) PulseEvent_k( &queue->handle);
}

/* clear some queue bits */
static INLINE void clear_queue_bits( struct msg_queue *queue, unsigned int bits )
{
    queue->wake_bits &= ~bits;
    queue->changed_bits &= ~bits;
}

/* increment (or decrement if 'incr' is negative) the queue paint count */
void inc_queue_paint_count( struct KThread* thread, int incr )
{
    struct msg_queue *queue = thread->queue;

    assert( queue );

    if ((queue->paint_count += incr) < 0) queue->paint_count = 0;

    if (queue->paint_count)
        set_queue_bits( queue, QS_PAINT );
    else
        clear_queue_bits( queue, QS_PAINT );
}

static int queue_hotkey_message( struct desktop *desktop, struct message *msg )
{
    struct hotkey *hotkey;
    unsigned int modifiers = 0;

    if (msg->msg != WM_KEYDOWN) return 0;

    if (desktop->keystate[VK_MENU] & 0x80) modifiers |= MOD_ALT;
    if (desktop->keystate[VK_CONTROL] & 0x80) modifiers |= MOD_CONTROL;
    if (desktop->keystate[VK_SHIFT] & 0x80) modifiers |= MOD_SHIFT;
    if ((desktop->keystate[VK_LWIN] & 0x80) || (desktop->keystate[VK_RWIN] & 0x80)) modifiers |= MOD_WIN;

    LIST_FOR_EACH_ENTRY( hotkey, &desktop->hotkeys, struct hotkey, entry )
    {
        if (hotkey->vkey != msg->wparam) continue;
        if ((hotkey->flags & (MOD_ALT|MOD_CONTROL|MOD_SHIFT|MOD_WIN)) == modifiers) goto found;
    }

    return 0;

found:
    msg->type      = MSG_POSTED;
    msg->win       = hotkey->win;
    msg->msg       = WM_HOTKEY;
    msg->wparam    = hotkey->id;
    msg->lparam    = ((hotkey->vkey & 0xffff) << 16) | modifiers;

    list_add_tail( &hotkey->queue->msg_list[POST_MESSAGE], &msg->entry );
    set_queue_bits( hotkey->queue, QS_POSTMESSAGE|QS_ALLPOSTMESSAGE|QS_HOTKEY );
    hotkey->queue->hotkey_count++;
    return 1;
}

/* check whether msg is a keyboard message */
static INLINE int is_keyboard_msg( struct message *msg )
{
    return (msg->msg >= WM_KEYFIRST && msg->msg <= WM_KEYLAST);
}

static INLINE int get_hardware_msg_bit( struct message *msg )
{
    if (msg->msg == WM_INPUT_DEVICE_CHANGE || msg->msg == WM_INPUT) return QS_RAWINPUT;
    if (msg->msg == WM_MOUSEMOVE || msg->msg == WM_NCMOUSEMOVE) return QS_MOUSEMOVE;
    if (is_keyboard_msg( msg )) return QS_KEY;
    return QS_MOUSEBUTTON;
}

/* find the window that should receive a given hardware message */
static HWND find_hardware_message_window( struct desktop *desktop, struct thread_input *input,
                                                   struct message *msg, unsigned int *msg_code )
{
    struct hardware_msg_data *data = &msg->data.hardware;
    HWND win = 0;

    *msg_code = msg->msg;
    if (msg->msg == WM_INPUT)
    {
        if (!(win = msg->win) && input) win = input->focus;
    }
    else if (is_keyboard_msg( msg ))
    {
        if (input && !(win = input->focus))
        {
            win = input->active;
            if (*msg_code < WM_SYSKEYDOWN) *msg_code += WM_SYSKEYDOWN - WM_KEYDOWN;
        }
    }
    else  /* mouse message */
    {
        if (!input || !(win = input->capture))
        {
            if (!(win = msg->win) || !IsWindowVisible_k( win ) || ((GetWindowLongA_k(win, GWL_EXSTYLE) & WS_EX_LAYERED|WS_EX_TRANSPARENT)==(WS_EX_LAYERED|WS_EX_TRANSPARENT))) {
                POINT p = {data->x, data->y};
                win = WindowFromPoint_k(p);
            }
        }
    }
    return win;
}

                                                   /* try to merge a message with the last in the list; return 1 if successful */
static int merge_message( struct thread_input *input, const struct message *msg )
{
    struct message *prev;
    struct list *ptr;

    if (msg->msg != WM_MOUSEMOVE) return 0;
    for (ptr = list_tail( &input->msg_list ); ptr; ptr = list_prev( &input->msg_list, ptr ))
    {
        prev = LIST_ENTRY( ptr, struct message, entry );
        if (prev->msg != WM_INPUT) break;
    }
    if (!ptr) return 0;
    if (prev->result) return 0;
    if (prev->win && msg->win && prev->win != msg->win) return 0;
    if (prev->msg != msg->msg) return 0;
    if (prev->type != msg->type) return 0;
    /* now we can merge it */
    prev->wparam  = msg->wparam;
    prev->lparam  = msg->lparam;
    prev->time    = msg->time;
    if (msg->type == MSG_HARDWARE)
    {
        prev->data = msg->data;
    }
    list_remove( ptr );
    list_add_tail( &input->msg_list, ptr );
    return 1;
}

/* queue a hardware message into a given thread input */
static void queue_hardware_message( struct desktop *desktop, struct message *msg, int always_queue )
{
    HWND win;
    struct KThread *thread;
    struct thread_input *input;
    unsigned int msg_code;
    struct hardware_msg_data *data = &msg->data.hardware;

    update_input_key_state( desktop, desktop->keystate, msg );
    last_input_time = GetTickCount_k();
    if (msg->msg != WM_MOUSEMOVE) always_queue = 1;

    if (is_keyboard_msg( msg ))
    {
        if (queue_hotkey_message( desktop, msg )) return;
        if (desktop->keystate[VK_MENU] & 0x80) msg->lparam |= KF_ALTDOWN << 16;
        if (msg->wparam == VK_SHIFT || msg->wparam == VK_LSHIFT || msg->wparam == VK_RSHIFT)
            msg->lparam &= ~(KF_EXTENDED << 16);
    }
    else if (msg->msg != WM_INPUT)
    {
        if (msg->msg == WM_MOUSEMOVE)
        {
            int x = min( max( data->x, desktop->cursor.clip.left ), desktop->cursor.clip.right-1 );
            int y = min( max( data->y, desktop->cursor.clip.top ), desktop->cursor.clip.bottom-1 );
            if (desktop->cursor.x != x || desktop->cursor.y != y) always_queue = 1;
            desktop->cursor.x = x;
            desktop->cursor.y = y;
            desktop->cursor.last_change = GetTickCount_k();
        }
        if (desktop->keystate[VK_LBUTTON] & 0x80)  msg->wparam |= MK_LBUTTON;
        if (desktop->keystate[VK_MBUTTON] & 0x80)  msg->wparam |= MK_MBUTTON;
        if (desktop->keystate[VK_RBUTTON] & 0x80)  msg->wparam |= MK_RBUTTON;
        if (desktop->keystate[VK_SHIFT] & 0x80)    msg->wparam |= MK_SHIFT;
        if (desktop->keystate[VK_CONTROL] & 0x80)  msg->wparam |= MK_CONTROL;
        if (desktop->keystate[VK_XBUTTON1] & 0x80) msg->wparam |= MK_XBUTTON1;
        if (desktop->keystate[VK_XBUTTON2] & 0x80) msg->wparam |= MK_XBUTTON2;
    }
    data->x = desktop->cursor.x;
    data->y = desktop->cursor.y;

    if (msg->win && (thread = get_window_thread( msg->win )))
    {
        input = thread->queue->input;
    }
    else input = desktop->foreground_input;

    win = find_hardware_message_window( desktop, input, msg, &msg_code );
    if (!win || !(thread = get_window_thread(win)))
    {
        if (input) update_input_key_state( input->desktop, input->keystate, msg );
        free_message( msg );
        return;
    }
    input = thread->queue->input;

    if (win != desktop->cursor.win) always_queue = 1;
    desktop->cursor.win = win;

    if (!always_queue || merge_message( input, msg )) free_message( msg );
    else
    {
        msg->unique_id = 0;  /* will be set once we return it to the app */
        list_add_tail( &input->msg_list, &msg->entry );
        set_queue_bits( thread->queue, get_hardware_msg_bit(msg) );
    }
}

/* remove the result from the sender list it is on */
static INLINE void remove_result_from_sender( struct message_result *result )
{
    assert( result->sender );

    list_remove( &result->sender_entry );
    result->sender = NULL;
    if (!result->receiver) free_result( result );
}

/* store the message result in the appropriate structure */
static void store_message_result( struct message_result *res, LPARAM result, unsigned int error )
{
    res->result  = result;
    res->error   = error;
    res->replied = 1;
    if (res->timeout)
    {
        SDL_RemoveTimer(res->timeout);
        res->timeout = NULL;
    }

    if (res->hardware_msg)
    {
        if (!error && result)  /* rejected by the hook */
            free_message( res->hardware_msg );
        else
            queue_hardware_message( res->desktop, res->hardware_msg, 0 );

        res->hardware_msg = NULL;
    }

    if (res->sender)
    {
        if (res->callback_msg)
        {
            /* queue the callback message in the sender queue */
            struct callback_msg_data *data = &res->callback_msg->data.callback;
            data->result = result;
            list_add_tail( &res->sender->msg_list[SEND_MESSAGE], &res->callback_msg->entry );
            set_queue_bits( res->sender, QS_SENDMESSAGE );
            res->callback_msg = NULL;
            remove_result_from_sender( res );
        }
        else
        {
            /* wake sender queue if waiting on this result */
            if (list_head(&res->sender->send_result) == &res->sender_entry)
                set_queue_bits( res->sender, QS_SMRESULT );
        }
    }
    else if (!res->receiver) free_result( res );
}


/* free a message when deleting a queue or window */
static void free_message( struct message *msg )
{
    struct message_result *result = msg->result;
    if (result)
    {
        result->msg = NULL;
        result->receiver = NULL;
        store_message_result( result, 0, ERROR_ACCESS_DENIED /*FIXME*/ );
    }
    kfree( msg );
}

/* free a result structure */
static void free_result( struct message_result *result )
{
    if (result->timeout) SDL_RemoveTimer( result->timeout );
    if (result->callback_msg) free_message( result->callback_msg );
    if (result->hardware_msg) free_message( result->hardware_msg );
    kfree( result );
}

/* remove (and free) a message from a message list */
static void remove_queue_message( struct msg_queue *queue, struct message *msg,
                                  enum message_kind kind )
{
    list_remove( &msg->entry );
    switch(kind)
    {
    case SEND_MESSAGE:
        if (list_empty( &queue->msg_list[kind] )) clear_queue_bits( queue, QS_SENDMESSAGE );
        break;
    case POST_MESSAGE:
        if (list_empty( &queue->msg_list[kind] ) && !queue->quit_message)
            clear_queue_bits( queue, QS_POSTMESSAGE|QS_ALLPOSTMESSAGE );
        if (msg->msg == WM_HOTKEY && --queue->hotkey_count == 0)
            clear_queue_bits( queue, QS_HOTKEY );
        break;
    }
    free_message( msg );
}

static int match_window( HWND win, HWND msg_win )
{
    if (!win) return 1;
    if ((int)win == -1 || (int)win == 1) return (int)!msg_win;
    if (msg_win == win) return 1;
    return IsChild_k(win, msg_win);
}

/* check if message is matched by the filter */
static INLINE int check_msg_filter( unsigned int msg, unsigned int first, unsigned int last )
{
    return (msg >= first && msg <= last);
}

/* check whether a message filter contains at least one potential hardware message */
static INLINE int filter_contains_hw_range( unsigned int first, unsigned int last )
{
    /* hardware message ranges are (in numerical order):
     *   WM_NCMOUSEFIRST .. WM_NCMOUSELAST
     *   WM_INPUT_DEVICE_CHANGE .. WM_KEYLAST
     *   WM_MOUSEFIRST .. WM_MOUSELAST
     */
    if (last < WM_NCMOUSEFIRST) return 0;
    if (first > WM_NCMOUSELAST && last < WM_INPUT_DEVICE_CHANGE) return 0;
    if (first > WM_KEYLAST && last < WM_MOUSEFIRST) return 0;
    if (first > WM_MOUSELAST) return 0;
    return 1;
}

struct KThread *get_first_global_hook( int id );
static struct message_result *alloc_message_result( struct msg_queue *send_queue, struct msg_queue *recv_queue, struct message *msg, DWORD timeout );
static int is_queue_hung( struct msg_queue *queue );

/* send the low-level hook message for a given hardware message */
static int send_hook_ll_message( struct desktop *desktop, struct message *hardware_msg,
                                 const INPUT *input, struct msg_queue *sender )
{
    struct KThread *hook_thread;
    struct msg_queue *queue;
    struct message *msg;
    DWORD timeout = 2000 * -10000;  /* FIXME: load from registry */
    int id = (input->type == INPUT_MOUSE) ? WH_MOUSE_LL : WH_KEYBOARD_LL;

    if (!(hook_thread = get_first_global_hook( id ))) return 0;
    if (!(queue = hook_thread->queue)) return 0;
    if (is_queue_hung( queue )) return 0;

    if (!(msg = kalloc( sizeof(*msg) ))) return 0;

    msg->type      = MSG_HOOK_LL;
    msg->win       = 0;
    msg->msg       = id;
    msg->wparam    = hardware_msg->msg;
    msg->time      = hardware_msg->time;
    msg->result    = NULL;

    if (input->type == INPUT_KEYBOARD)
    {
        unsigned short vkey = input->ki.wVk;
        if (input->ki.dwFlags & KEYEVENTF_UNICODE) vkey = VK_PACKET;
        msg->lparam = (input->ki.wScan << 16) | vkey;
    }
    else msg->lparam = input->mi.mouseData << 16;
    msg->data = hardware_msg->data;

    if (!(msg->result = alloc_message_result( sender, queue, msg, timeout )))
    {
        free_message( msg );
        return 0;
    }
    msg->result->hardware_msg = hardware_msg;
    msg->result->desktop = desktop;
    list_add_tail( &queue->msg_list[SEND_MESSAGE], &msg->entry );
    set_queue_bits( queue, QS_SENDMESSAGE );
    return 1;
}

/* change the thread input data of a given thread */
static int assign_thread_input( struct KThread *thread, struct thread_input *new_input )
{
    struct msg_queue *queue = thread->queue;

    if (!queue)
    {
        thread->queue = create_msg_queue( thread, new_input );
        return thread->queue != NULL;
    }
    if (queue->input)
    {
        queue->input->cursor_count -= queue->cursor_count;
    }
    queue->input = new_input;
    new_input->cursor_count += queue->cursor_count;
    return 1;
}

/* attach two thread input data structures */
int attach_thread_input( struct KThread *thread_from, struct KThread *thread_to )
{
    struct desktop *desktop;
    struct thread_input *input;
    int ret;

    if (!thread_to->queue && !(thread_to->queue = create_msg_queue( thread_to, NULL ))) return 0;
    if (!(desktop = thread_from->input->desktop)) return 0;
    input = thread_to->queue->input;
    if (input->desktop != desktop)
    {
        SetLastError_k(ERROR_ACCESS_DENIED);
        return 0;
    }

    ret = assign_thread_input( thread_from, input );
    if (ret) memset( input->keystate, 0, sizeof(input->keystate) );
    return ret;
}

/* detach two thread input data structures */
void detach_thread_input( struct KThread *thread_from )
{
    struct thread_input *input;

    if ((input = create_thread_input( thread_from )))
    {
        assign_thread_input( thread_from, input );
    }
}

/* attach (or detach) thread inputs */
int attach_thread_input_queue(BOOL attach, DWORD from, DWORD to)
{
    struct KThread *thread_from = getThread( from );
    struct KThread *thread_to = getThread( to );

    if (!thread_from || !thread_to)
    {
        return 0;
    }
    if (thread_from != thread_to)
    {
        if (attach) return attach_thread_input( thread_from, thread_to );
        else
        {
            if (thread_from->queue && thread_to->queue &&
                thread_from->queue->input == thread_to->queue->input) {
                detach_thread_input( thread_from );
                return 1;
            } else {
                SetLastError_k(ERROR_ACCESS_DENIED);
            }
        }
    }
    else {
        SetLastError_k(ERROR_ACCESS_DENIED);
    }
    return 0;
}

/* queue a hardware message for a mouse event */
static int queue_mouse_message( struct desktop *desktop, HWND win, const INPUT *input,
                                unsigned int hook_flags, struct msg_queue *sender )
{
    //const struct rawinput_device *device;
    struct message *msg;
    unsigned int i, time, flags;
    int wait = 0, x, y;
    struct KProcess* process = currentProcess();

    static const unsigned int messages[] =
    {
        WM_MOUSEMOVE,    /* 0x0001 = MOUSEEVENTF_MOVE */
        WM_LBUTTONDOWN,  /* 0x0002 = MOUSEEVENTF_LEFTDOWN */
        WM_LBUTTONUP,    /* 0x0004 = MOUSEEVENTF_LEFTUP */
        WM_RBUTTONDOWN,  /* 0x0008 = MOUSEEVENTF_RIGHTDOWN */
        WM_RBUTTONUP,    /* 0x0010 = MOUSEEVENTF_RIGHTUP */
        WM_MBUTTONDOWN,  /* 0x0020 = MOUSEEVENTF_MIDDLEDOWN */
        WM_MBUTTONUP,    /* 0x0040 = MOUSEEVENTF_MIDDLEUP */
        WM_XBUTTONDOWN,  /* 0x0080 = MOUSEEVENTF_XDOWN */
        WM_XBUTTONUP,    /* 0x0100 = MOUSEEVENTF_XUP */
        0,               /* 0x0200 = unused */
        0,               /* 0x0400 = unused */
        WM_MOUSEWHEEL,   /* 0x0800 = MOUSEEVENTF_WHEEL */
        WM_MOUSEHWHEEL   /* 0x1000 = MOUSEEVENTF_HWHEEL */
    };

    desktop->cursor.last_change = GetTickCount_k();
    flags = input->mi.dwFlags;
    time  = input->mi.time;
    if (!time) time = desktop->cursor.last_change;

    if (flags & MOUSEEVENTF_MOVE)
    {
        if (flags & MOUSEEVENTF_ABSOLUTE)
        {
            x = input->mi.dx;
            y = input->mi.dy;
            if (flags & ~(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE) &&
                x == desktop->cursor.x && y == desktop->cursor.y)
                flags &= ~MOUSEEVENTF_MOVE;
        }
        else
        {
            x = desktop->cursor.x + input->mi.dx;
            y = desktop->cursor.y + input->mi.dy;
        }
    }
    else
    {
        x = desktop->cursor.x;
        y = desktop->cursor.y;
    }

    if (process->rawinput_mouse)
    {
        if (!(msg = kalloc( sizeof(*msg) ))) return 0;

        msg->type      = MSG_HARDWARE;
        msg->win       = (HWND)process->rawinput_mouse;
        msg->msg       = WM_INPUT;
        msg->wparam    = RIM_INPUT;
        msg->lparam    = 0;
        msg->time      = time;
        msg->result    = NULL;

        msg->data.hardware.info                = input->mi.dwExtraInfo;
        msg->data.hardware.flags               = flags;
        msg->data.hardware.rawinput.type       = RIM_TYPEMOUSE;
        msg->data.hardware.rawinput.mouse.x    = x - desktop->cursor.x;
        msg->data.hardware.rawinput.mouse.y    = y - desktop->cursor.y;
        msg->data.hardware.rawinput.mouse.data = input->mi.mouseData;

        queue_hardware_message( desktop, msg, 0 );
    }

    for (i = 0; i < sizeof(messages)/sizeof(messages[0]); i++)
    {
        if (!messages[i]) continue;
        if (!(flags & (1 << i))) continue;
        flags &= ~(1 << i);

        if (!(msg = kalloc( sizeof(*msg) ))) return 0;

        msg->type      = MSG_HARDWARE;
        msg->win       = win;
        msg->msg       = messages[i];
        msg->wparam    = input->mi.mouseData << 16;
        msg->lparam    = 0;
        msg->time      = time;
        msg->result    = NULL;
        msg->data.hardware.x = x;
        msg->data.hardware.y = y;
        msg->data.hardware.info = input->mi.dwExtraInfo;
        if (hook_flags & SEND_HWMSG_INJECTED) msg->data.hardware.flags = LLMHF_INJECTED;

        /* specify a sender only when sending the last message */
        if (!(flags & ((1 << sizeof(messages)/sizeof(messages[0])) - 1)))
        {
            if (!(wait = send_hook_ll_message( desktop, msg, input, sender )))
                queue_hardware_message( desktop, msg, 0 );
        }
        else if (!send_hook_ll_message( desktop, msg, input, NULL ))
            queue_hardware_message( desktop, msg, 0 );
    }
    return wait;
}

/* queue a hardware message for a keyboard event */
static int queue_keyboard_message( struct desktop *desktop, HWND win, const INPUT *input,
                                   unsigned int hook_flags, struct msg_queue *sender )
{
    struct message *msg;
    unsigned char vkey = (unsigned char)input->ki.wVk;
    unsigned int message_code, time;
    int wait;
    struct KProcess* process = currentProcess();

    if (!(time = input->ki.time)) time = GetTickCount_k();

    if (!(input->ki.dwFlags & KEYEVENTF_UNICODE))
    {
        switch (vkey)
        {
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
            vkey = (input->ki.dwFlags & KEYEVENTF_EXTENDEDKEY) ? VK_RMENU : VK_LMENU;
            break;
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
            vkey = (input->ki.dwFlags & KEYEVENTF_EXTENDEDKEY) ? VK_RCONTROL : VK_LCONTROL;
            break;
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
            vkey = (input->ki.dwFlags & KEYEVENTF_EXTENDEDKEY) ? VK_RSHIFT : VK_LSHIFT;
            break;
        }
    }

    message_code = (input->ki.dwFlags & KEYEVENTF_KEYUP) ? WM_KEYUP : WM_KEYDOWN;
    switch (vkey)
    {
    case VK_LMENU:
    case VK_RMENU:
        if (input->ki.dwFlags & KEYEVENTF_KEYUP)
        {
            /* send WM_SYSKEYUP if Alt still pressed and no other key in between */
            /* we use 0x02 as a flag to track if some other SYSKEYUP was sent already */
            if ((desktop->keystate[VK_MENU] & 0x82) != 0x82) break;
            message_code = WM_SYSKEYUP;
            desktop->keystate[VK_MENU] &= ~0x02;
        }
        else
        {
            /* send WM_SYSKEYDOWN for Alt except with Ctrl */
            if (desktop->keystate[VK_CONTROL] & 0x80) break;
            message_code = WM_SYSKEYDOWN;
            desktop->keystate[VK_MENU] |= 0x02;
        }
        break;

    case VK_LCONTROL:
    case VK_RCONTROL:
        /* send WM_SYSKEYUP on release if Alt still pressed */
        if (!(input->ki.dwFlags & KEYEVENTF_KEYUP)) break;
        if (!(desktop->keystate[VK_MENU] & 0x80)) break;
        message_code = WM_SYSKEYUP;
        desktop->keystate[VK_MENU] &= ~0x02;
        break;

    default:
        /* send WM_SYSKEY for Alt-anykey and for F10 */
        if (desktop->keystate[VK_CONTROL] & 0x80) break;
        if (!(desktop->keystate[VK_MENU] & 0x80)) break;
        /* fall through */
    case VK_F10:
        message_code = (input->ki.dwFlags & KEYEVENTF_KEYUP) ? WM_SYSKEYUP : WM_SYSKEYDOWN;
        desktop->keystate[VK_MENU] &= ~0x02;
        break;
    }

    if (process->rawinput_kbd)
    {
        if (!(msg = kalloc( sizeof(*msg) ))) return 0;

        msg->type      = MSG_HARDWARE;
        msg->win       = (HWND)process->rawinput_kbd;
        msg->msg       = WM_INPUT;
        msg->wparam    = RIM_INPUT;
        msg->lparam    = 0;
        msg->time      = time;
        msg->result    = NULL;

        msg->data.hardware.info        = input->ki.dwExtraInfo;
        msg->data.hardware.flags       = input->ki.dwFlags;
        msg->data.hardware.rawinput.type        = RIM_TYPEKEYBOARD;
        msg->data.hardware.rawinput.kbd.message = message_code;
        msg->data.hardware.rawinput.kbd.vkey    = vkey;
        msg->data.hardware.rawinput.kbd.scan    = input->ki.wScan;

        queue_hardware_message( desktop, msg, 0 );
    }

    if (!(msg = kalloc( sizeof(*msg) ))) return 0;

    msg->type      = MSG_HARDWARE;
    msg->win       = win;
    msg->msg       = message_code;
    msg->lparam    = (input->ki.wScan << 16) | 1u; /* repeat count */
    msg->time      = time;
    msg->result    = NULL;
    msg->data.hardware.info = input->ki.dwExtraInfo;
    if (hook_flags & SEND_HWMSG_INJECTED) msg->data.hardware.flags = LLKHF_INJECTED;

    if (input->ki.dwFlags & KEYEVENTF_UNICODE)
    {
        msg->wparam = VK_PACKET;
    }
    else
    {
        unsigned int flags = 0;
        if (input->ki.dwFlags & KEYEVENTF_EXTENDEDKEY) flags |= KF_EXTENDED;
        /* FIXME: set KF_DLGMODE and KF_MENUMODE when needed */
        if (input->ki.dwFlags & KEYEVENTF_KEYUP) flags |= KF_REPEAT | KF_UP;
        else if (desktop->keystate[vkey] & 0x80) flags |= KF_REPEAT;

        msg->wparam = vkey;
        msg->lparam |= flags << 16;
        msg->data.hardware.flags |= (flags & (KF_EXTENDED | KF_ALTDOWN | KF_UP)) >> 8;
    }

    if (!(wait = send_hook_ll_message( desktop, msg, input, sender )))
        queue_hardware_message( desktop, msg, 1 );

    return wait;
}

/* queue a hardware message for a custom type of event */
static void queue_custom_hardware_message( struct desktop *desktop, HWND win,
                                           const INPUT *input )
{
    struct message *msg;

    if (!(msg = kalloc( sizeof(*msg) ))) return;

    msg->type      = MSG_HARDWARE;
    msg->win       = win;
    msg->msg       = input->hi.uMsg;
    msg->wparam    = 0;
    msg->lparam    = MAKELONG( input->hi.wParamL, input->hi.wParamH );
    msg->time      = GetTickCount_k();
    msg->result    = NULL;

    queue_hardware_message( desktop, msg, 1 );
}

/* check message filter for a hardware message */
static int check_hw_message_filter( HWND win, unsigned int msg_code,
                                    HWND filter_win, unsigned int first, unsigned int last )
{
    if (msg_code >= WM_KEYFIRST && msg_code <= WM_KEYLAST)
    {
        /* we can only test the window for a keyboard message since the
         * dest window for a mouse message depends on hittest */
        if (filter_win && win != filter_win && !IsChild_k( filter_win, win ))
            return 0;
        /* the message code is final for a keyboard message, we can simply check it */
        return check_msg_filter( msg_code, first, last );
    }
    else  /* mouse message */
    {
        /* we need to check all possible values that the message can have in the end */

        if (check_msg_filter( msg_code, first, last )) return 1;
        if (msg_code == WM_MOUSEWHEEL) return 0;  /* no other possible value for this one */

        /* all other messages can become non-client messages */
        if (check_msg_filter( msg_code + (WM_NCMOUSEFIRST - WM_MOUSEFIRST), first, last )) return 1;

        /* clicks can become double-clicks or non-client double-clicks */
        if (msg_code == WM_LBUTTONDOWN || msg_code == WM_MBUTTONDOWN ||
            msg_code == WM_RBUTTONDOWN || msg_code == WM_XBUTTONDOWN)
        {
            if (check_msg_filter( msg_code + (WM_LBUTTONDBLCLK - WM_LBUTTONDOWN), first, last )) return 1;
            if (check_msg_filter( msg_code + (WM_NCLBUTTONDBLCLK - WM_LBUTTONDOWN), first, last )) return 1;
        }
        return 0;
    }
}

/* retrieve a posted message */
static int get_posted_message_queue( struct msg_queue *queue, HWND win,
                               unsigned int first, unsigned int last, unsigned int flags,
                               struct message *reply )
{
    struct message *msg;

    /* check against the filters */
    LIST_FOR_EACH_ENTRY( msg, &queue->msg_list[POST_MESSAGE], struct message, entry )
    {
        if (!match_window( win, msg->win )) continue;
        if (!check_msg_filter( msg->msg, first, last )) continue;
        goto found; /* found one */
    }
    return 0;

    /* return it to the app */
found:
    *reply = *msg;

    if (flags & PM_REMOVE)
    {
        remove_queue_message( queue, msg, POST_MESSAGE );
    }

    return 1;
}

static int get_quit_message_queue( struct msg_queue *queue, unsigned int flags,
                             struct message *reply )
{
    if (queue->quit_message)
    {
        reply->type   = MSG_POSTED;
        reply->win    = 0;
        reply->msg    = WM_QUIT;
        reply->wparam = queue->exit_code;
        reply->lparam = 0;
        reply->time   = GetTickCount_k();

        if (flags & PM_REMOVE)
        {
            queue->quit_message = 0;
            if (list_empty( &queue->msg_list[POST_MESSAGE] ))
                clear_queue_bits( queue, QS_POSTMESSAGE|QS_ALLPOSTMESSAGE );
        }
        return 1;
    }
    else
        return 0;
}

/* get a (pseudo-)unique id to tag hardware messages */
static INLINE unsigned int get_unique_id(void)
{
    static unsigned int id;
    if (!++id) id = 1;  /* avoid an id of 0 */
    return id;
}

                             /* find a hardware message for the given queue */
static int get_hardware_message( struct KThread *thread, unsigned int hw_id, HWND filter_win,
                                 unsigned int first, unsigned int last, unsigned int flags,
                                 struct message *reply )
{
    struct thread_input *input = thread->queue->input;
    struct KThread *win_thread;
    struct list *ptr;
    HWND win;
    int clear_bits, got_one = 0;
    unsigned int msg_code;

    ptr = list_head( &input->msg_list );
    if (hw_id)
    {
        while (ptr)
        {
            struct message *msg = LIST_ENTRY( ptr, struct message, entry );
            if (msg->unique_id == hw_id) break;
            ptr = list_next( &input->msg_list, ptr );
        }
        if (!ptr) ptr = list_head( &input->msg_list );
        else ptr = list_next( &input->msg_list, ptr );  /* start from the next one */
    }

    if (ptr == list_head( &input->msg_list ))
        clear_bits = QS_INPUT;
    else
        clear_bits = 0;  /* don't clear bits if we don't go through the whole list */

    while (ptr)
    {
        struct message *msg = LIST_ENTRY( ptr, struct message, entry );
        struct hardware_msg_data *data = &msg->data.hardware;

        ptr = list_next( &input->msg_list, ptr );
        win = find_hardware_message_window( input->desktop, input, msg, &msg_code );
        if (!win || !(win_thread = get_window_thread( win )))
        {
            /* no window at all, remove it */
            update_input_key_state( input->desktop, input->keystate, msg );
            list_remove( &msg->entry );
            free_message( msg );
            continue;
        }
        if (win_thread != thread)
        {
            if (win_thread->queue->input == input)
            {
                /* wake the other thread */
                set_queue_bits( win_thread->queue, get_hardware_msg_bit(msg) );
                got_one = 1;
            }
            else
            {
                /* for another thread input, drop it */
                update_input_key_state( input->desktop, input->keystate, msg );
                list_remove( &msg->entry );
                free_message( msg );
            }
            continue;
        }

        /* if we already got a message for another thread, or if it doesn't
         * match the filter we skip it */
        if (got_one || !check_hw_message_filter( win, msg_code, filter_win, first, last ))
        {
            clear_bits &= ~get_hardware_msg_bit( msg );
            continue;
        }
        /* now we can return it */
        if (!msg->unique_id) msg->unique_id = get_unique_id();
        reply->type   = MSG_HARDWARE;
        reply->win    = win;
        reply->msg    = msg_code;
        reply->wparam = msg->wparam;
        reply->lparam = msg->lparam;
        reply->time   = msg->time;
        reply->data = msg->data;
        data->hw_id = msg->unique_id;

        if (msg->msg == WM_INPUT && (flags & PM_REMOVE))
            release_hardware_message( thread->queue, data->hw_id, 1, 0 );
        return 1;
    }
    /* nothing found, clear the hardware queue bits */
    clear_queue_bits( thread->queue, clear_bits );
    return 0;
}


/* receive a message, removing it from the sent queue */
static void receive_message_queue( struct msg_queue *queue, struct message *msg)
{
    struct message_result *result = msg->result;

    list_remove( &msg->entry );
    /* put the result on the receiver result stack */
    if (result)
    {
        result->msg = NULL;
        result->recv_next  = queue->recv_result;
        queue->recv_result = result;
    }
    free( msg );
    if (list_empty( &queue->msg_list[SEND_MESSAGE] )) clear_queue_bits( queue, QS_SENDMESSAGE );
}

static void reply_message_queue( struct msg_queue *queue, LPARAM result, unsigned int error, int remove, const void *data, DWORD len )
{
    struct message_result *res = queue->recv_result;

    if (remove)
    {
        queue->recv_result = res->recv_next;
        res->receiver = NULL;
        if (!res->sender && !res->hardware_msg)  /* no one waiting for it */
        {
            free_result( res );
            return;
        }
    }
    if (!res->replied)
    {
        store_message_result( res, result, error );
    }
}

/* check if the thread owning the queue is hung (not checking for messages) */
static int is_queue_hung( struct msg_queue *queue )
{
    // struct wait_queue_entry *entry;
    DWORD current_time = GetTickCount_k();

    if (current_time - queue->last_get_msg <= 5000000)
        return 0;  /* less than 5 seconds since last get message -> not hung */

    // :TODO:
    /*
    LIST_FOR_EACH_ENTRY( entry, &queue->obj.wait_queue, struct wait_queue_entry, entry )
    {
        if (get_wait_queue_thread(entry)->queue == queue)
            return 0;  // thread is waiting on queue -> not hung 
    }
    */
    return 1;
}

/* message timed out without getting a reply */
static Uint32 result_timeout(Uint32 interval, void *param)
{
    struct message_result *result = param;

    assert( !result->replied );

    result->timeout = NULL;

    if (result->msg)  /* not received yet */
    {
        struct message *msg = result->msg;

        result->msg = NULL;
        msg->result = NULL;
        remove_queue_message( result->receiver, msg, SEND_MESSAGE );
        result->receiver = NULL;
    }
    store_message_result( result, 0, STATUS_TIMEOUT );
    return 0;
}

/* allocate and fill a message result structure */
static struct message_result *alloc_message_result( struct msg_queue *send_queue,
                                                    struct msg_queue *recv_queue,
                                                    struct message *msg, DWORD timeout )
{
    struct message_result *result = kalloc( sizeof(*result) );
    if (result)
    {
        result->msg          = msg;
        result->sender       = send_queue;
        result->receiver     = recv_queue;
        result->replied      = 0;
        result->timeout      = NULL;
        result->hardware_msg = NULL;
        result->desktop      = NULL;
        result->callback_msg = NULL;

        if (msg->type == MSG_CALLBACK)
        {
            struct message *callback_msg = kalloc( sizeof(*callback_msg) );

            if (!callback_msg)
            {
                kfree( result );
                return NULL;
            }
            callback_msg->type      = MSG_CALLBACK_RESULT;
            callback_msg->win       = msg->win;
            callback_msg->msg       = msg->msg;
            callback_msg->wparam    = 0;
            callback_msg->lparam    = 0;
            callback_msg->time      = GetTickCount_k();
            callback_msg->result    = NULL;
            /* steal the data from the original message */
            callback_msg->data      = msg->data;

            result->callback_msg = callback_msg;
            list_add_head( &send_queue->callback_result, &result->sender_entry );
        }
        else if (send_queue) list_add_head( &send_queue->send_result, &result->sender_entry );

        if (timeout != INFINITE)
            SDL_AddTimer(timeout, result_timeout, result);
    }
    return result;
}

DWORD send_message_queue(DWORD threadId, DWORD flags, DWORD type, HWND hwnd, DWORD message, WPARAM wParam, LPARAM lParam, int timeout, message_data_t *data)
{
    struct message *msg;
    struct msg_queue *send_queue = currentThread()->queue;
    struct msg_queue *recv_queue = NULL;
    struct KThread *thread = getThread(threadId);

    if (!thread) return 0;

    if (!(recv_queue = thread->queue))
    {
        return ERROR_INVALID_PARAMETER;
    }
    if ((flags & SEND_MSG_ABORT_IF_HUNG) && is_queue_hung(recv_queue))
    {
        return ERROR_TIMEOUT;
    }

    if ((msg = kalloc( sizeof(*msg) )))
    {
        msg->type      = type;
        msg->win       = hwnd;
        msg->msg       = message;
        msg->wparam    = wParam;
        msg->lparam    = lParam;
        msg->time      = GetTickCount_k();
        msg->result    = NULL;
        if (data)
            msg->data  = *data;

        switch(msg->type)
        {
        case MSG_ASCII:
        case MSG_UNICODE:
        case MSG_CALLBACK:
            if (!(msg->result = alloc_message_result( send_queue, recv_queue, msg, timeout )))
            {
                free_message( msg );
                break;
            }
            /* fall through */
        case MSG_NOTIFY:
            list_add_tail( &recv_queue->msg_list[SEND_MESSAGE], &msg->entry );
            set_queue_bits( recv_queue, QS_SENDMESSAGE );
            break;
        case MSG_POSTED:
            list_add_tail( &recv_queue->msg_list[POST_MESSAGE], &msg->entry );
            set_queue_bits( recv_queue, QS_POSTMESSAGE|QS_ALLPOSTMESSAGE );
            if (msg->msg == WM_HOTKEY)
            {
                set_queue_bits( recv_queue, QS_HOTKEY );
                recv_queue->hotkey_count++;
            }
            break;
        case MSG_HARDWARE:  /* should use send_hardware_message instead */
        case MSG_CALLBACK_RESULT:  /* cannot send this one */
        case MSG_HOOK_LL:  /* generated internally */
        default:
            return ERROR_INVALID_PARAMETER;
            kfree( msg );
            break;
        }
    }
    return ERROR_OUTOFMEMORY;
}

static void release_hardware_message( struct msg_queue *queue, unsigned int hw_id,
                                      int remove, HWND new_win )
{
    struct thread_input *input = queue->input;
    struct message *msg;

    LIST_FOR_EACH_ENTRY( msg, &input->msg_list, struct message, entry )
    {
        if (msg->unique_id == hw_id) break;
    }
    if (&msg->entry == &input->msg_list) return;  /* not found */

    /* clear the queue bit for that message */
    if (remove || new_win)
    {
        struct message *other;
        int clr_bit;

        clr_bit = get_hardware_msg_bit( msg );
        LIST_FOR_EACH_ENTRY( other, &input->msg_list, struct message, entry )
        {
            if (other != msg && get_hardware_msg_bit( other ) == clr_bit)
            {
                clr_bit = 0;
                break;
            }
        }
        if (clr_bit) clear_queue_bits( queue, clr_bit );
    }

    if (new_win)  /* set the new window */
    {
        struct KThread *owner = get_window_thread( new_win );
        if (owner)
        {
            msg->win = new_win;
            if (owner->queue->input != input)
            {
                list_remove( &msg->entry );
                if (merge_message( owner->queue->input, msg ))
                {
                    free_message( msg );
                    return;
                }
                list_add_tail( &owner->queue->input->msg_list, &msg->entry );
            }
            set_queue_bits( owner->queue, get_hardware_msg_bit( msg ));
            remove = 0;
        }
    }
    if (remove)
    {
        update_input_key_state( input->desktop, input->keystate, msg );
        list_remove( &msg->entry );
        free_message( msg );
    }
}

/* post a win event */
void post_win_event( struct KThread *thread, unsigned int event, HWND win, unsigned int object_id, unsigned int child_id, void* hook_proc, const WCHAR *module, HHOOK hook)
{
    struct message *msg;

    if (thread->queue && (msg = kalloc( sizeof(*msg) )))
    {
        msg->type      = MSG_WINEVENT;
        msg->win       = win;
        msg->msg       = event;
        msg->wparam    = object_id;
        msg->lparam    = child_id;
        msg->time      = GetTickCount_k();
        msg->result    = NULL;


        msg->data.winevent.hook = hook;
        msg->data.winevent.hook_proc = hook_proc;
        strcpyW(msg->data.winevent.module, module);
        msg->data.winevent.tid = currentThread()->id;
        list_add_tail( &thread->queue->msg_list[SEND_MESSAGE], &msg->entry );
        set_queue_bits( thread->queue, QS_SENDMESSAGE );
    }
}

/* send a hardware message to a thread queue */
NTSTATUS send_hardware_message_queue(HWND win, const INPUT *input, UINT flags, BOOL* wait)
{
    struct KThread *thread = NULL;
    struct desktop *desktop;
    struct msg_queue *sender = currentThread()->queue;
    NTSTATUS result = STATUS_SUCCESS;

    if (!(desktop = currentThread()->input->desktop)) return result;

    if (win)
    {
        if (!(thread = get_window_thread( win ))) return result;
        if (desktop != thread->queue->input->desktop)
        {
            /* don't allow queuing events to a different desktop */
            return 0;
        }
    }
    *wait = 0;
    switch (input->type)
    {
    case INPUT_MOUSE:
        *wait = queue_mouse_message( desktop, win, input, flags, sender );
        break;
    case INPUT_KEYBOARD:
        *wait = queue_keyboard_message( desktop, win, input, flags, sender );
        break;
    case INPUT_HARDWARE:
        queue_custom_hardware_message( desktop, win, input );
        break;
    default:
        result =  STATUS_INVALID_PARAMETER;
    }

    return result;
}

/* find an expired timer matching the filtering parameters */
static struct timer *find_expired_timer( struct msg_queue *queue, HWND win,
                                         unsigned int get_first, unsigned int get_last,
                                         int remove )
{
    struct list *ptr;

    LIST_FOR_EACH( ptr, &queue->expired_timers )
    {
        struct timer *timer = LIST_ENTRY( ptr, struct timer, entry );
        if (win && timer->win != win) continue;
        if (check_msg_filter( timer->msg, get_first, get_last ))
        {
            if (remove) restart_timer_queue( queue, timer );
            return timer;
        }
    }
    return NULL;
}

HWND find_window_to_repaint( HWND parent, DWORD tid );

DWORD get_message_queue(struct message *msg, HWND hwnd, UINT first, UINT last, UINT flags, UINT changed_mask, UINT wake_mask, DWORD hw_id, message_data_t** data)
{
    struct timer *timer;
    struct list *ptr;
    struct KThread* thread = currentThread();
    struct KProcess* process = thread->process;
    struct msg_queue *queue = thread->queue;
    HWND get_win = hwnd;
    unsigned int filter = flags >> 16;

    if (!queue) return 0;
    queue->last_get_msg = GetTickCount_k();
    if (!filter) filter = QS_ALLINPUT;

    /* first check for sent messages */
    if ((ptr = list_head( &queue->msg_list[SEND_MESSAGE] )))
    {
        struct message *result = LIST_ENTRY( ptr, struct message, entry );
        *msg = *result;
        receive_message_queue( queue, result);        
        return 0;
    }

    /* clear changed bits so we can wait on them if we don't find a message */
    if (filter & QS_POSTMESSAGE)
    {
        queue->changed_bits &= ~(QS_POSTMESSAGE | QS_HOTKEY | QS_TIMER);
        if (first == 0 && last == ~0U) queue->changed_bits &= ~QS_ALLPOSTMESSAGE;
    }
    if (filter & QS_INPUT) queue->changed_bits &= ~QS_INPUT;
    if (filter & QS_PAINT) queue->changed_bits &= ~QS_PAINT;

    /* then check for posted messages */
    if ((filter & QS_POSTMESSAGE) &&
        get_posted_message_queue( queue, get_win, first, last, flags, msg ))
        return 0;

    if ((filter & QS_HOTKEY) && queue->hotkey_count &&
        first <= WM_HOTKEY && last >= WM_HOTKEY &&
        get_posted_message_queue( queue, get_win, WM_HOTKEY, WM_HOTKEY, flags, msg ))
        return 0;

    /* only check for quit messages if not posted messages pending.
     * note: the quit message isn't filtered */
    if (get_quit_message_queue( queue, flags, msg ))
        return 0;

    /* then check for any raw hardware message */
    if ((filter & QS_INPUT) &&
        filter_contains_hw_range( first, last ) &&
        get_hardware_message( currentThread(), hw_id, get_win, first, last, flags, msg ))
        return 0;

    /* now check for WM_PAINT */
    if ((filter & QS_PAINT) &&
        queue->paint_count &&
        check_msg_filter( WM_PAINT, first, last ) &&
        (msg->win = find_window_to_repaint( get_win, currentThread()->id )))
    {
        msg->type   = MSG_POSTED;
        msg->msg    = WM_PAINT;
        msg->wparam = 0;
        msg->lparam = 0;
        msg->time   = GetTickCount_k();
        return 0;
    }

    /* now check for timer */
    if ((filter & QS_TIMER) &&
        (timer = find_expired_timer( queue, get_win, first,
                                     last, (flags & PM_REMOVE) )))
    {
        msg->type   = MSG_POSTED;
        msg->win    = timer->win;
        msg->msg    = timer->msg;
        msg->wparam = timer->id;
        msg->lparam = timer->lparam;
        msg->time   = GetTickCount_k();
        if (!(flags & PM_NOYIELD) && process->idle_event)
            SetEvent_k( (HANDLE)process->idle_event );
        return 0;
    }

    if (get_win == (HWND)-1 && process->idle_event) SetEvent_k( (HANDLE)process->idle_event );
    queue->wake_mask = wake_mask;
    queue->changed_mask = changed_mask;
    return ERROR_EVENT_PENDING;  /* FIXME */
}

/* retrieve the reply for the last message sent */
void get_message_reply_queue(struct msg_queue *queue, DWORD cancel, LRESULT* res)
{
    struct message_result *result;
    struct list *entry;

    *res = 0;

    if (!(entry = list_head( &queue->send_result ))) return;  /* no reply ready */

    result = LIST_ENTRY( entry, struct message_result, sender_entry );
    if (result->replied || cancel)
    {
        if (result->replied)
        {
            *res = result->result;
            SetLastError_k(result->error);
        }
        remove_result_from_sender( result );

        entry = list_head( &queue->send_result );
        if (!entry) clear_queue_bits( queue, QS_SMRESULT );
        else
        {
            result = LIST_ENTRY( entry, struct message_result, sender_entry );
            if (!result->replied) clear_queue_bits( queue, QS_SMRESULT );
        }
    }
}

void set_queue_mask(struct msg_queue *queue, DWORD wake_mask, DWORD changed_mask, DWORD* wake_bits, DWORD* changed_bits, DWORD skip_wait)
{
    if (queue)
    {
        queue->wake_mask    = wake_mask;
        queue->changed_mask = changed_mask;
        if (wake_bits)
            *wake_bits = queue->wake_bits;
        if (changed_bits)
            *changed_bits = queue->changed_bits;
        if (is_signaled( queue ))
        {
            /* if skip wait is set, do what would have been done in the subsequent wait */
            if (skip_wait) queue->wake_mask = queue->changed_mask = 0;
            else wake_up( &queue->handle, 0 );
        }
    }
}
static void set_next_timer_queue( struct msg_queue *queue );

/* callback for the next timer expiration */
static Uint32 timer_callback(Uint32 interval, void *param)
{
    struct msg_queue *queue = param;
    struct list *ptr;

    queue->timeout = NULL;
    /* move on to the next timer */
    ptr = list_head( &queue->pending_timers );
    list_remove( ptr );
    list_add_tail( &queue->expired_timers, ptr );
    set_next_timer_queue( queue );
    return 0;
}

/* set the next timer to expire */
static void set_next_timer_queue( struct msg_queue *queue )
{
    struct list *ptr;

    if (queue->timeout)
    {
        SDL_RemoveTimer( queue->timeout );
        queue->timeout = NULL;
    }
    if ((ptr = list_head( &queue->pending_timers )))
    {
        struct timer *timer = LIST_ENTRY( ptr, struct timer, entry );
        queue->timeout = SDL_AddTimer( timer->when-GetTickCount_k(), timer_callback, queue );
    }
    /* set/clear QS_TIMER bit */
    if (list_empty( &queue->expired_timers ))
        clear_queue_bits( queue, QS_TIMER );
    else
        set_queue_bits( queue, QS_TIMER );
}

/* find a timer from its window and id */
static struct timer *find_timer_queue( struct msg_queue *queue, HWND win,
                                 unsigned int msg, LPARAM id )
{
    struct list *ptr;

    /* we need to search both lists */

    LIST_FOR_EACH( ptr, &queue->pending_timers )
    {
        struct timer *timer = LIST_ENTRY( ptr, struct timer, entry );
        if (timer->win == win && timer->msg == msg && timer->id == id) return timer;
    }
    LIST_FOR_EACH( ptr, &queue->expired_timers )
    {
        struct timer *timer = LIST_ENTRY( ptr, struct timer, entry );
        if (timer->win == win && timer->msg == msg && timer->id == id) return timer;
    }
    return NULL;
}

/* link a timer at its rightful place in the queue list */
static void link_timer_queue( struct msg_queue *queue, struct timer *timer )
{
    struct list *ptr;

    for (ptr = queue->pending_timers.next; ptr != &queue->pending_timers; ptr = ptr->next)
    {
        struct timer *t = LIST_ENTRY( ptr, struct timer, entry );
        if (t->when >= timer->when) break;
    }
    list_add_before( ptr, &timer->entry );
}

/* remove a timer from the queue timer list and free it */
static void free_timer_queue( struct msg_queue *queue, struct timer *timer )
{
    list_remove( &timer->entry );
    free( timer );
    set_next_timer_queue( queue );
}

/* restart an expired timer */
static void restart_timer_queue( struct msg_queue *queue, struct timer *timer )
{
    DWORD current_time = GetTickCount_k();
    list_remove( &timer->entry );
    while (timer->when <= current_time) timer->when += timer->rate * 10000;
    link_timer_queue( queue, timer );
    set_next_timer_queue( queue );
}

/* add a timer */
static struct timer *set_timer_queue( struct msg_queue *queue, unsigned int rate )
{
    struct timer *timer = kalloc( sizeof(*timer) );
    if (timer)
    {
        timer->rate = max( rate, 1 );
        timer->when = GetTickCount_k() + timer->rate * 10000;
        link_timer_queue( queue, timer );
        /* check if we replaced the next timer */
        if (list_head( &queue->pending_timers ) == &timer->entry) set_next_timer_queue( queue );
    }
    return timer;
}

/* set a window timer */
DWORD set_win_timer(HWND win, DWORD id, DWORD msg, DWORD rate, LPARAM lparam, DWORD* result)
{
    struct timer *timer;
    struct msg_queue *queue;
    struct KThread *thread = NULL;
    struct KProcess* process = currentProcess();

    if (win)
    {
        if (!(thread = get_window_thread( win )))
        {
            return ERROR_INVALID_PARAMETER;
        }
        if (thread->process != process)
        {
            return ERROR_ACCESS_DENIED;
        }
        queue = thread->queue;
        /* remove it if it existed already */
        if ((timer = find_timer_queue( queue, win, msg, id ))) free_timer_queue( queue, timer );
    }
    else
    {
        queue = currentThread()->queue;
        /* look for a timer with this id */
        if (id && (timer = find_timer_queue( queue, 0, msg, id )))
        {
            /* free and reuse id */
            free_timer_queue( queue, timer );
        }
        else
        {
            /* find a free id for it */
            do
            {
                id = queue->next_timer_id;
                if (--queue->next_timer_id <= 0x100) queue->next_timer_id = 0x7fff;
            }
            while (find_timer_queue( queue, 0, msg, id ));
        }
    }

    if ((timer = set_timer_queue( queue, rate )))
    {
        timer->win    = win;
        timer->msg    = msg;
        timer->id     = id;
        timer->lparam = lparam;
        *result       = id;
    }
    return 0;
}

/* kill a window timer */
DWORD kill_win_timer(HWND win, DWORD msg, DWORD id)
{
    struct timer *timer;
    struct KThread *thread;
    struct KProcess* process = currentProcess();

    if (win)
    {
        if (!(thread = get_window_thread( win )))
        {
            return ERROR_INVALID_HANDLE;
        }
        if (thread->process != process)
        {
            return ERROR_ACCESS_DENIED;
        }
    }
    else {
        thread = currentThread();
    }

    if (thread->queue && (timer = find_timer_queue( thread->queue, win, msg, id )))
        free_timer_queue( thread->queue, timer );
    else
        return ERROR_INVALID_PARAMETER;
    return 0;
}

/* Message class descriptor */
static const WCHAR messageW[] = {'M','e','s','s','a','g','e',0};

const struct builtin_class_descr MESSAGE_builtin_class =
{
    messageW,             /* name */
    0,                    /* style */
    WINPROC_MESSAGE,      /* proc */
    0,                    /* extra */
    IDC_ARROW,            /* cursor */
    0                     /* brush */
};



/* flag for messages that contain pointers */
/* 32 messages per entry, messages 0..31 map to bits 0..31 */

#define SET(msg) (1 << ((msg) & 31))

static const unsigned int message_pointer_flags[] =
{
    /* 0x00 - 0x1f */
    SET(WM_CREATE) | SET(WM_SETTEXT) | SET(WM_GETTEXT) |
    SET(WM_WININICHANGE) | SET(WM_DEVMODECHANGE),
    /* 0x20 - 0x3f */
    SET(WM_GETMINMAXINFO) | SET(WM_DRAWITEM) | SET(WM_MEASUREITEM) | SET(WM_DELETEITEM) |
    SET(WM_COMPAREITEM),
    /* 0x40 - 0x5f */
    SET(WM_WINDOWPOSCHANGING) | SET(WM_WINDOWPOSCHANGED) | SET(WM_COPYDATA) |
    SET(WM_NOTIFY) | SET(WM_HELP),
    /* 0x60 - 0x7f */
    SET(WM_STYLECHANGING) | SET(WM_STYLECHANGED),
    /* 0x80 - 0x9f */
    SET(WM_NCCREATE) | SET(WM_NCCALCSIZE) | SET(WM_GETDLGCODE),
    /* 0xa0 - 0xbf */
    SET(EM_GETSEL) | SET(EM_GETRECT) | SET(EM_SETRECT) | SET(EM_SETRECTNP),
    /* 0xc0 - 0xdf */
    SET(EM_REPLACESEL) | SET(EM_GETLINE) | SET(EM_SETTABSTOPS),
    /* 0xe0 - 0xff */
    SET(SBM_GETRANGE) | SET(SBM_SETSCROLLINFO) | SET(SBM_GETSCROLLINFO) | SET(SBM_GETSCROLLBARINFO),
    /* 0x100 - 0x11f */
    0,
    /* 0x120 - 0x13f */
    0,
    /* 0x140 - 0x15f */
    SET(CB_GETEDITSEL) | SET(CB_ADDSTRING) | SET(CB_DIR) | SET(CB_GETLBTEXT) |
    SET(CB_INSERTSTRING) | SET(CB_FINDSTRING) | SET(CB_SELECTSTRING) |
    SET(CB_GETDROPPEDCONTROLRECT) | SET(CB_FINDSTRINGEXACT),
    /* 0x160 - 0x17f */
    0,
    /* 0x180 - 0x19f */
    SET(LB_ADDSTRING) | SET(LB_INSERTSTRING) | SET(LB_GETTEXT) | SET(LB_SELECTSTRING) |
    SET(LB_DIR) | SET(LB_FINDSTRING) |
    SET(LB_GETSELITEMS) | SET(LB_SETTABSTOPS) | SET(LB_ADDFILE) | SET(LB_GETITEMRECT),
    /* 0x1a0 - 0x1bf */
    SET(LB_FINDSTRINGEXACT),
    /* 0x1c0 - 0x1df */
    0,
    /* 0x1e0 - 0x1ff */
    0,
    /* 0x200 - 0x21f */
    SET(WM_NEXTMENU) | SET(WM_SIZING) | SET(WM_MOVING) | SET(WM_DEVICECHANGE),
    /* 0x220 - 0x23f */
    SET(WM_MDICREATE) | SET(WM_MDIGETACTIVE) | SET(WM_DROPOBJECT) |
    SET(WM_QUERYDROPOBJECT) | SET(WM_DRAGLOOP) | SET(WM_DRAGSELECT) | SET(WM_DRAGMOVE),
    /* 0x240 - 0x25f */
    0,
    /* 0x260 - 0x27f */
    0,
    /* 0x280 - 0x29f */
    0,
    /* 0x2a0 - 0x2bf */
    0,
    /* 0x2c0 - 0x2df */
    0,
    /* 0x2e0 - 0x2ff */
    0,
    /* 0x300 - 0x31f */
    SET(WM_ASKCBFORMATNAME)
};

/* flags for messages that contain Unicode strings */
static const unsigned int message_unicode_flags[] =
{
    /* 0x00 - 0x1f */
    SET(WM_CREATE) | SET(WM_SETTEXT) | SET(WM_GETTEXT) | SET(WM_GETTEXTLENGTH) |
    SET(WM_WININICHANGE) | SET(WM_DEVMODECHANGE),
    /* 0x20 - 0x3f */
    SET(WM_CHARTOITEM),
    /* 0x40 - 0x5f */
    0,
    /* 0x60 - 0x7f */
    0,
    /* 0x80 - 0x9f */
    SET(WM_NCCREATE),
    /* 0xa0 - 0xbf */
    0,
    /* 0xc0 - 0xdf */
    SET(EM_REPLACESEL) | SET(EM_GETLINE) | SET(EM_SETPASSWORDCHAR),
    /* 0xe0 - 0xff */
    0,
    /* 0x100 - 0x11f */
    SET(WM_CHAR) | SET(WM_DEADCHAR) | SET(WM_SYSCHAR) | SET(WM_SYSDEADCHAR),
    /* 0x120 - 0x13f */
    SET(WM_MENUCHAR),
    /* 0x140 - 0x15f */
    SET(CB_ADDSTRING) | SET(CB_DIR) | SET(CB_GETLBTEXT) | SET(CB_GETLBTEXTLEN) |
    SET(CB_INSERTSTRING) | SET(CB_FINDSTRING) | SET(CB_SELECTSTRING) | SET(CB_FINDSTRINGEXACT),
    /* 0x160 - 0x17f */
    0,
    /* 0x180 - 0x19f */
    SET(LB_ADDSTRING) | SET(LB_INSERTSTRING) | SET(LB_GETTEXT) | SET(LB_GETTEXTLEN) |
    SET(LB_SELECTSTRING) | SET(LB_DIR) | SET(LB_FINDSTRING) | SET(LB_ADDFILE),
    /* 0x1a0 - 0x1bf */
    SET(LB_FINDSTRINGEXACT),
    /* 0x1c0 - 0x1df */
    0,
    /* 0x1e0 - 0x1ff */
    0,
    /* 0x200 - 0x21f */
    0,
    /* 0x220 - 0x23f */
    SET(WM_MDICREATE),
    /* 0x240 - 0x25f */
    0,
    /* 0x260 - 0x27f */
    0,
    /* 0x280 - 0x29f */
    SET(WM_IME_CHAR),
    /* 0x2a0 - 0x2bf */
    0,
    /* 0x2c0 - 0x2df */
    0,
    /* 0x2e0 - 0x2ff */
    0,
    /* 0x300 - 0x31f */
    SET(WM_PAINTCLIPBOARD) | SET(WM_SIZECLIPBOARD) | SET(WM_ASKCBFORMATNAME)
};

/* check whether a given message type includes pointers */
static INLINE int is_pointer_message( UINT message )
{
    if (message >= 8*sizeof(message_pointer_flags)) return FALSE;
    return (message_pointer_flags[message / 32] & SET(message)) != 0;
}

/* check whether a given message type contains Unicode (or ASCII) chars */
static INLINE int is_unicode_message( UINT message )
{
    if (message >= 8*sizeof(message_unicode_flags)) return FALSE;
    return (message_unicode_flags[message / 32] & SET(message)) != 0;
}

#undef SET

/* add a data field to a packed message */
static INLINE void push_data( struct packed_message *data, const void *ptr, size_t size )
{
    data->data[data->count] = ptr;
    data->size[data->count] = size;
    data->count++;
}

/* add a string to a packed message */
static INLINE void push_string( struct packed_message *data, LPCWSTR str )
{
    push_data( data, str, (strlenW(str) + 1) * sizeof(WCHAR) );
}

/* make sure that the buffer contains a valid null-terminated Unicode string */
static INLINE BOOL check_string( LPCWSTR str, size_t size )
{
    for (size /= sizeof(WCHAR); size; size--, str++)
        if (!*str) return TRUE;
    return FALSE;
}

/* pack a pointer into a 32/64 portable format */
static INLINE ULONGLONG pack_ptr( const void *ptr )
{
    return (ULONG_PTR)ptr;
}

/* unpack a potentially 64-bit pointer, returning 0 when truncated */
static INLINE void *unpack_ptr( ULONGLONG ptr64 )
{
    if ((ULONG_PTR)ptr64 != ptr64) return 0;
    return (void *)(ULONG_PTR)ptr64;
}

/* make sure that there is space for 'size' bytes in buffer, growing it if needed */
static INLINE void *get_buffer_space( void **buffer, size_t size )
{
    void *ret;

    if (*buffer)
    {
        if (!(ret = HeapReAlloc_k(GetProcessHeap_k(), 0, *buffer, size )))
            HeapFree_k(GetProcessHeap_k(), 0, *buffer );
    }
    else ret = HeapAlloc_k(GetProcessHeap_k(), 0, size );

    *buffer = ret;
    return ret;
}

/* check whether a combobox expects strings or ids in CB_ADDSTRING/CB_INSERTSTRING */
static INLINE BOOL combobox_has_strings( HWND hwnd )
{
    DWORD style = GetWindowLongA_k( hwnd, GWL_STYLE );
    return (!(style & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE)) || (style & CBS_HASSTRINGS));
}

/* check whether a listbox expects strings or ids in LB_ADDSTRING/LB_INSERTSTRING */
static INLINE BOOL listbox_has_strings( HWND hwnd )
{
    DWORD style = GetWindowLongA_k( hwnd, GWL_STYLE );
    return (!(style & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)) || (style & LBS_HASSTRINGS));
}

/* check whether message is in the range of keyboard messages */
static INLINE BOOL is_keyboard_message( UINT message )
{
    return (message >= WM_KEYFIRST && message <= WM_KEYLAST);
}

/* check whether message is in the range of mouse messages */
static INLINE BOOL is_mouse_message( UINT message )
{
    return ((message >= WM_NCMOUSEFIRST && message <= WM_NCMOUSELAST) ||
            (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST));
}

/* check whether message matches the specified hwnd filter */
static INLINE BOOL check_hwnd_filter( const MSG *msg, HWND hwnd_filter )
{
    if (!hwnd_filter || hwnd_filter == GetDesktopWindow_k()) return TRUE;
    return (msg->hwnd == hwnd_filter || IsChild_k( hwnd_filter, msg->hwnd ));
}

/* check for pending WM_CHAR message with DBCS trailing byte */
static INLINE BOOL get_pending_wmchar( MSG *msg, UINT first, UINT last, BOOL remove )
{
    struct wm_char_mapping_data *data = get_user_thread_info()->wmchar_data;

    if (!data || !data->get_msg.message) return FALSE;
    if ((first || last) && (first > WM_CHAR || last < WM_CHAR)) return FALSE;
    if (!msg) return FALSE;
    *msg = data->get_msg;
    if (remove) data->get_msg.message = 0;
    return TRUE;
}


/***********************************************************************
 *           MessageWndProc
 *
 * Window procedure for "Message" windows (HWND_MESSAGE parent).
 */
LRESULT WINAPI MessageWndProc_k( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if (message == WM_NCCREATE) return TRUE;
    return 0;  /* all other messages are ignored */
}


/***********************************************************************
 *		broadcast_message_callback
 *
 * Helper callback for broadcasting messages.
 */
static BOOL CALLBACK broadcast_message_callback( HWND hwnd, LPARAM lparam )
{
    struct send_message_info *info = (struct send_message_info *)lparam;
    if (!(GetWindowLongW_k( hwnd, GWL_STYLE ) & (WS_POPUP|WS_CAPTION))) return TRUE;
    switch(info->type)
    {
    case MSG_UNICODE:
        SendMessageTimeoutW_k( hwnd, info->msg, info->wparam, info->lparam,
                             info->flags, info->timeout, NULL );
        break;
    case MSG_ASCII:
        SendMessageTimeoutA_k( hwnd, info->msg, info->wparam, info->lparam,
                             info->flags, info->timeout, NULL );
        break;
    case MSG_NOTIFY:
        SendNotifyMessageW_k( hwnd, info->msg, info->wparam, info->lparam );
        break;
    case MSG_CALLBACK:
        SendMessageCallbackW_k( hwnd, info->msg, info->wparam, info->lparam,
                              info->callback, info->data );
        break;
    case MSG_POSTED:
        PostMessageW_k( hwnd, info->msg, info->wparam, info->lparam );
        break;
    default:
        ERR( "bad type %d\n", info->type );
        break;
    }
    return TRUE;
}

DWORD get_input_codepage( void )
{
    DWORD cp;
    int ret;
    HKL hkl = GetKeyboardLayout_k( 0 );

    ret = GetLocaleInfoW_k( LOWORD(hkl), LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_NUMBER,
                          (WCHAR *)&cp, sizeof(cp) / sizeof(WCHAR) );
    if (!ret) cp = CP_ACP;
    return cp;
}

/***********************************************************************
 *		map_wparam_AtoW
 *
 * Convert the wparam of an ASCII message to Unicode.
 */
BOOL map_wparam_AtoW( UINT message, WPARAM *wparam, enum wm_char_mapping mapping )
{
    char ch[2];
    WCHAR wch[2];
    DWORD cp = get_input_codepage();

    wch[0] = wch[1] = 0;
    switch(message)
    {
    case WM_CHAR:
        /* WM_CHAR is magic: a DBCS char can be sent/posted as two consecutive WM_CHAR
         * messages, in which case the first char is stored, and the conversion
         * to Unicode only takes place once the second char is sent/posted.
         */
        if (mapping != WMCHAR_MAP_NOMAPPING)
        {
            struct wm_char_mapping_data *data = get_user_thread_info()->wmchar_data;
            BYTE low = LOBYTE(*wparam);

            if (HIBYTE(*wparam))
            {
                ch[0] = low;
                ch[1] = HIBYTE(*wparam);
                MultiByteToWideChar_k( cp, 0, ch, 2, wch, 2 );
                TRACE( "map %02x,%02x -> %04x mapping %u\n", (BYTE)ch[0], (BYTE)ch[1], wch[0], mapping );
                if (data) data->lead_byte[mapping] = 0;
            }
            else if (data && data->lead_byte[mapping])
            {
                ch[0] = data->lead_byte[mapping];
                ch[1] = low;
                MultiByteToWideChar_k( cp, 0, ch, 2, wch, 2 );
                TRACE( "map stored %02x,%02x -> %04x mapping %u\n", (BYTE)ch[0], (BYTE)ch[1], wch[0], mapping );
                data->lead_byte[mapping] = 0;
            }
            else if (!IsDBCSLeadByte_k( low ))
            {
                ch[0] = low;
                MultiByteToWideChar_k( cp, 0, ch, 1, wch, 2 );
                TRACE( "map %02x -> %04x\n", (BYTE)ch[0], wch[0] );
                if (data) data->lead_byte[mapping] = 0;
            }
            else  /* store it and wait for trail byte */
            {
                if (!data)
                {
                    if (!(data = HeapAlloc_k(GetProcessHeap_k(), HEAP_ZERO_MEMORY, sizeof(*data) )))
                        return FALSE;
                    get_user_thread_info()->wmchar_data = data;
                }
                TRACE( "storing lead byte %02x mapping %u\n", low, mapping );
                data->lead_byte[mapping] = low;
                return FALSE;
            }
            *wparam = MAKEWPARAM(wch[0], wch[1]);
            break;
        }
        /* else fall through */
    case WM_CHARTOITEM:
    case EM_SETPASSWORDCHAR:
    case WM_DEADCHAR:
    case WM_SYSCHAR:
    case WM_SYSDEADCHAR:
    case WM_MENUCHAR:
        ch[0] = LOBYTE(*wparam);
        ch[1] = HIBYTE(*wparam);
        MultiByteToWideChar_k( cp, 0, ch, 2, wch, 2 );
        *wparam = MAKEWPARAM(wch[0], wch[1]);
        break;
    case WM_IME_CHAR:
        ch[0] = HIBYTE(*wparam);
        ch[1] = LOBYTE(*wparam);
        if (ch[0]) MultiByteToWideChar_k( cp, 0, ch, 2, wch, 2 );
        else MultiByteToWideChar_k( cp, 0, ch + 1, 1, wch, 1 );
        *wparam = MAKEWPARAM(wch[0], HIWORD(*wparam));
        break;
    }
    return TRUE;
}


/***********************************************************************
 *		map_wparam_WtoA
 *
 * Convert the wparam of a Unicode message to ASCII.
 */
static void map_wparam_WtoA( MSG *msg, BOOL remove )
{
    BYTE ch[4];
    WCHAR wch[2];
    DWORD len;
    DWORD cp = get_input_codepage();

    switch(msg->message)
    {
    case WM_CHAR:
        if (!HIWORD(msg->wParam))
        {
            wch[0] = LOWORD(msg->wParam);
            ch[0] = ch[1] = 0;
            len = WideCharToMultiByte_k( cp, 0, wch, 1, (LPSTR)ch, 2, NULL, NULL );
            if (len == 2)  /* DBCS char */
            {
                struct wm_char_mapping_data *data = get_user_thread_info()->wmchar_data;
                if (!data)
                {
                    if (!(data = HeapAlloc_k(GetProcessHeap_k(), HEAP_ZERO_MEMORY, sizeof(*data) ))) return;
                    get_user_thread_info()->wmchar_data = data;
                }
                if (remove)
                {
                    data->get_msg = *msg;
                    data->get_msg.wParam = ch[1];
                }
                msg->wParam = ch[0];
                return;
            }
        }
        /* else fall through */
    case WM_CHARTOITEM:
    case EM_SETPASSWORDCHAR:
    case WM_DEADCHAR:
    case WM_SYSCHAR:
    case WM_SYSDEADCHAR:
    case WM_MENUCHAR:
        wch[0] = LOWORD(msg->wParam);
        wch[1] = HIWORD(msg->wParam);
        ch[0] = ch[1] = 0;
        WideCharToMultiByte_k( cp, 0, wch, 2, (LPSTR)ch, 4, NULL, NULL );
        msg->wParam = MAKEWPARAM( ch[0] | (ch[1] << 8), 0 );
        break;
    case WM_IME_CHAR:
        wch[0] = LOWORD(msg->wParam);
        ch[0] = ch[1] = 0;
        len = WideCharToMultiByte_k( cp, 0, wch, 1, (LPSTR)ch, 2, NULL, NULL );
        if (len == 2)
            msg->wParam = MAKEWPARAM( (ch[0] << 8) | ch[1], HIWORD(msg->wParam) );
        else
            msg->wParam = MAKEWPARAM( ch[0], HIWORD(msg->wParam) );
        break;
    }
}


/***********************************************************************
 *           reply_message
 *
 * Send a reply to a sent message.
 */
static void reply_message( struct received_message_info *info, LRESULT result, BOOL remove )
{
    struct packed_message data;
    int replied = info->flags & ISMEX_REPLIED;
    struct KThread* thread = currentThread();

    if (info->flags & ISMEX_NOTIFY) return;  /* notify messages don't get replies */
    if (!remove && replied) return;  /* replied already */

    memset( &data, 0, sizeof(data) );
    info->flags |= ISMEX_REPLIED;

    if (thread->queue->recv_result) {
        reply_message_queue(thread->queue, result, 0, remove, 0, 0);
    }
}


/***********************************************************************
 *           handle_internal_message
 *
 * Handle an internal Wine message instead of calling the window proc.
 */
static LRESULT handle_internal_message( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    switch(msg)
    {
    case WM_WINE_DESTROYWINDOW:
        return WIN_DestroyWindow( hwnd );
    case WM_WINE_SETWINDOWPOS:
        if (is_desktop_window( hwnd )) return 0;
        return USER_SetWindowPos( (WINDOWPOS *)lparam );
    case WM_WINE_SHOWWINDOW:
        if (is_desktop_window( hwnd )) return 0;
        return ShowWindow_k( hwnd, wparam );
    case WM_WINE_SETPARENT:
        if (is_desktop_window( hwnd )) return 0;
        return (LRESULT)SetParent_k( hwnd, (HWND)wparam );
    case WM_WINE_SETWINDOWLONG:
        return WIN_SetWindowLong( hwnd, (short)LOWORD(wparam), HIWORD(wparam), lparam, TRUE );
    case WM_WINE_ENABLEWINDOW:
        if (is_desktop_window( hwnd )) return 0;
        return EnableWindow_k( hwnd, wparam );
    case WM_WINE_SETACTIVEWINDOW:
        if (is_desktop_window( hwnd )) return 0;
        return (LRESULT)SetActiveWindow_k( (HWND)wparam );
    case WM_WINE_KEYBOARD_LL_HOOK:
    case WM_WINE_MOUSE_LL_HOOK:
    {
        struct hook_extra_info *h_extra = (struct hook_extra_info *)lparam;

        return call_current_hook( h_extra->handle, HC_ACTION, wparam, h_extra->lparam );
    }
    case WM_WINE_CLIPCURSOR:
        if (wparam)
        {
            RECT rect;
            GetClipCursor_k( &rect );
            return USER_Driver->pClipCursor( &rect );
        }
        return USER_Driver->pClipCursor( NULL );
    default:
        if (msg >= WM_WINE_FIRST_DRIVER_MSG && msg <= WM_WINE_LAST_DRIVER_MSG)
            return USER_Driver->pWindowMessage( hwnd, msg, wparam, lparam );
        FIXME( "unknown internal message %x\n", msg );
        return 0;
    }
}

/* since the WM_DDE_ACK response to a WM_DDE_EXECUTE message should contain the handle
 * to the memory handle, we keep track (in the server side) of all pairs of handle
 * used (the client passes its value and the content of the memory handle), and
 * the server stored both values (the client, and the local one, created after the
 * content). When a ACK message is generated, the list of pair is searched for a
 * matching pair, so that the client memory handle can be returned.
 */
struct DDE_pair {
    HGLOBAL     client_hMem;
    HGLOBAL     server_hMem;
};

static      struct DDE_pair*    dde_pairs;
static      int                 dde_num_alloc;
static      int                 dde_num_used;

static CRITICAL_SECTION dde_crst;
static CRITICAL_SECTION_DEBUG critsect_debug =
{
    0, 0, &dde_crst,
    { &critsect_debug.ProcessLocksList, &critsect_debug.ProcessLocksList },
      0, 0, { (DWORD_PTR)(__FILE__ ": dde_crst") }
};
static CRITICAL_SECTION dde_crst = { &critsect_debug, -1, 0, 0, 0, 0 };

static BOOL dde_add_pair(HGLOBAL chm, HGLOBAL shm)
{
    int  i;
#define GROWBY  4

    EnterCriticalSection_k(&dde_crst);

    /* now remember the pair of hMem on both sides */
    if (dde_num_used == dde_num_alloc)
    {
        struct DDE_pair* tmp;
	if (dde_pairs)
	    tmp  = HeapReAlloc_k(GetProcessHeap_k(), 0, dde_pairs, (dde_num_alloc + GROWBY) * sizeof(struct DDE_pair));
	else
	    tmp  = HeapAlloc_k(GetProcessHeap_k(), 0, (dde_num_alloc + GROWBY) * sizeof(struct DDE_pair));

        if (!tmp)
        {
            LeaveCriticalSection_k(&dde_crst);
            return FALSE;
        }
        dde_pairs = tmp;
        /* zero out newly allocated part */
        memset(&dde_pairs[dde_num_alloc], 0, GROWBY * sizeof(struct DDE_pair));
        dde_num_alloc += GROWBY;
    }
#undef GROWBY
    for (i = 0; i < dde_num_alloc; i++)
    {
        if (dde_pairs[i].server_hMem == 0)
        {
            dde_pairs[i].client_hMem = chm;
            dde_pairs[i].server_hMem = shm;
            dde_num_used++;
            break;
        }
    }
    LeaveCriticalSection_k(&dde_crst);
    return TRUE;
}

static HGLOBAL dde_get_pair(HGLOBAL shm)
{
    int  i;
    HGLOBAL     ret = 0;

    EnterCriticalSection_k(&dde_crst);
    for (i = 0; i < dde_num_alloc; i++)
    {
        if (dde_pairs[i].server_hMem == shm)
        {
            /* free this pair */
            dde_pairs[i].server_hMem = 0;
            dde_num_used--;
            ret = dde_pairs[i].client_hMem;
            break;
        }
    }
    LeaveCriticalSection_k(&dde_crst);
    return ret;
}

/***********************************************************************
 *           call_window_proc
 *
 * Call a window procedure and the corresponding hooks.
 */
static LRESULT call_window_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                 BOOL unicode, BOOL same_thread, enum wm_char_mapping mapping )
{
    LRESULT result = 0;
    CWPSTRUCT cwp;
    CWPRETSTRUCT cwpret;

    if (msg & 0x80000000)
    {
        result = handle_internal_message( hwnd, msg, wparam, lparam );
        goto done;
    }

    /* first the WH_CALLWNDPROC hook */
    hwnd = WIN_GetFullHandle( hwnd );
    cwp.lParam  = lparam;
    cwp.wParam  = wparam;
    cwp.message = msg;
    cwp.hwnd    = hwnd;
    HOOK_CallHooks( WH_CALLWNDPROC, HC_ACTION, same_thread, (LPARAM)&cwp, unicode );

    /* now call the window procedure */
    if (!WINPROC_call_window( hwnd, msg, wparam, lparam, &result, unicode, mapping )) goto done;

    /* and finally the WH_CALLWNDPROCRET hook */
    cwpret.lResult = result;
    cwpret.lParam  = lparam;
    cwpret.wParam  = wparam;
    cwpret.message = msg;
    cwpret.hwnd    = hwnd;
    HOOK_CallHooks( WH_CALLWNDPROCRET, HC_ACTION, same_thread, (LPARAM)&cwpret, unicode );
 done:
    return result;
}


/***********************************************************************
 *           send_parent_notify
 *
 * Send a WM_PARENTNOTIFY to all ancestors of the given window, unless
 * the window has the WS_EX_NOPARENTNOTIFY style.
 */
static void send_parent_notify( HWND hwnd, WORD event, WORD idChild, POINT pt )
{
    /* pt has to be in the client coordinates of the parent window */
    MapWindowPoints_k( 0, hwnd, &pt, 1 );
    for (;;)
    {
        HWND parent;

        if (!(GetWindowLongW_k( hwnd, GWL_STYLE ) & WS_CHILD)) break;
        if (GetWindowLongW_k( hwnd, GWL_EXSTYLE ) & WS_EX_NOPARENTNOTIFY) break;
        if (!(parent = GetParent_k(hwnd))) break;
        if (parent == GetDesktopWindow_k()) break;
        MapWindowPoints_k( hwnd, parent, &pt, 1 );
        hwnd = parent;
        SendMessageW_k( hwnd, WM_PARENTNOTIFY,
                      MAKEWPARAM( event, idChild ), MAKELPARAM( pt.x, pt.y ) );
    }
}


/***********************************************************************
 *          accept_hardware_message
 *
 * Tell the server we have passed the message to the app
 * (even though we may end up dropping it later on)
 */
static void accept_hardware_message( UINT hw_id, BOOL remove, HWND new_hwnd )
{
    struct KThread* thread = currentThread();
    release_hardware_message(thread->queue, hw_id, remove, new_hwnd);    
}


static BOOL process_rawinput_message( MSG *msg, const struct hardware_msg_data *msg_data )
{
    struct user_thread_info *thread_info = get_user_thread_info();
    RAWINPUT *rawinput = thread_info->rawinput;

    if (!rawinput)
    {
        thread_info->rawinput = HeapAlloc_k(GetProcessHeap_k(), 0, sizeof(*rawinput) );
        if (!(rawinput = thread_info->rawinput)) return FALSE;
    }

    rawinput->header.dwType = msg_data->rawinput.type;
    if (msg_data->rawinput.type == RIM_TYPEMOUSE)
    {
        static const unsigned int button_flags[] =
        {
            0,                              /* MOUSEEVENTF_MOVE */
            RI_MOUSE_LEFT_BUTTON_DOWN,      /* MOUSEEVENTF_LEFTDOWN */
            RI_MOUSE_LEFT_BUTTON_UP,        /* MOUSEEVENTF_LEFTUP */
            RI_MOUSE_RIGHT_BUTTON_DOWN,     /* MOUSEEVENTF_RIGHTDOWN */
            RI_MOUSE_RIGHT_BUTTON_UP,       /* MOUSEEVENTF_RIGHTUP */
            RI_MOUSE_MIDDLE_BUTTON_DOWN,    /* MOUSEEVENTF_MIDDLEDOWN */
            RI_MOUSE_MIDDLE_BUTTON_UP,      /* MOUSEEVENTF_MIDDLEUP */
        };
        unsigned int i;

        rawinput->header.dwSize  = FIELD_OFFSET(RAWINPUT, data) + sizeof(RAWMOUSE);
        rawinput->header.hDevice = WINE_MOUSE_HANDLE;
        rawinput->header.wParam  = 0;

        rawinput->data.mouse.usFlags           = MOUSE_MOVE_RELATIVE;
        rawinput->data.mouse.usButtonFlags = 0;
        rawinput->data.mouse.usButtonData  = 0;
        for (i = 1; i < sizeof(button_flags) / sizeof(*button_flags); ++i)
        {
            if (msg_data->flags & (1 << i))
                rawinput->data.mouse.usButtonFlags |= button_flags[i];
        }
        if (msg_data->flags & MOUSEEVENTF_WHEEL)
        {
            rawinput->data.mouse.usButtonFlags |= RI_MOUSE_WHEEL;
            rawinput->data.mouse.usButtonData   = msg_data->rawinput.mouse.data;
        }
        if (msg_data->flags & MOUSEEVENTF_HWHEEL)
        {
            rawinput->data.mouse.usButtonFlags |= RI_MOUSE_HORIZONTAL_WHEEL;
            rawinput->data.mouse.usButtonData   = msg_data->rawinput.mouse.data;
        }
        if (msg_data->flags & MOUSEEVENTF_XDOWN)
        {
            if (msg_data->rawinput.mouse.data == XBUTTON1)
                rawinput->data.mouse.usButtonFlags |= RI_MOUSE_BUTTON_4_DOWN;
            else if (msg_data->rawinput.mouse.data == XBUTTON2)
                rawinput->data.mouse.usButtonFlags |= RI_MOUSE_BUTTON_5_DOWN;
        }
        if (msg_data->flags & MOUSEEVENTF_XUP)
        {
            if (msg_data->rawinput.mouse.data == XBUTTON1)
                rawinput->data.mouse.usButtonFlags |= RI_MOUSE_BUTTON_4_UP;
            else if (msg_data->rawinput.mouse.data == XBUTTON2)
                rawinput->data.mouse.usButtonFlags |= RI_MOUSE_BUTTON_5_UP;
        }

        rawinput->data.mouse.ulRawButtons       = 0;
        rawinput->data.mouse.lLastX             = msg_data->rawinput.mouse.x;
        rawinput->data.mouse.lLastY             = msg_data->rawinput.mouse.y;
        rawinput->data.mouse.ulExtraInformation = msg_data->info;
    }
    else if (msg_data->rawinput.type == RIM_TYPEKEYBOARD)
    {
        rawinput->header.dwSize  = FIELD_OFFSET(RAWINPUT, data) + sizeof(RAWKEYBOARD);
        rawinput->header.hDevice = WINE_KEYBOARD_HANDLE;
        rawinput->header.wParam  = 0;

        rawinput->data.keyboard.MakeCode = msg_data->rawinput.kbd.scan;
        rawinput->data.keyboard.Flags    = msg_data->flags & KEYEVENTF_KEYUP ? RI_KEY_BREAK : RI_KEY_MAKE;
        if (msg_data->flags & KEYEVENTF_EXTENDEDKEY) rawinput->data.keyboard.Flags |= RI_KEY_E0;
        rawinput->data.keyboard.Reserved = 0;

        switch (msg_data->rawinput.kbd.vkey)
        {
        case VK_LSHIFT:
        case VK_RSHIFT:
            rawinput->data.keyboard.VKey   = VK_SHIFT;
            rawinput->data.keyboard.Flags &= ~RI_KEY_E0;
            break;
        case VK_LCONTROL:
        case VK_RCONTROL:
            rawinput->data.keyboard.VKey = VK_CONTROL;
            break;
        case VK_LMENU:
        case VK_RMENU:
            rawinput->data.keyboard.VKey = VK_MENU;
            break;
        default:
            rawinput->data.keyboard.VKey = msg_data->rawinput.kbd.vkey;
            break;
        }

        rawinput->data.keyboard.Message          = msg_data->rawinput.kbd.message;
        rawinput->data.keyboard.ExtraInformation = msg_data->info;
    }
    else
    {
        FIXME("Unhandled rawinput type %#x.\n", msg_data->rawinput.type);
        return FALSE;
    }

    msg->lParam = (LPARAM)rawinput;
    return TRUE;
}

/***********************************************************************
 *          process_keyboard_message
 *
 * returns TRUE if the contents of 'msg' should be passed to the application
 */
static BOOL process_keyboard_message( MSG *msg, UINT hw_id, HWND hwnd_filter,
                                      UINT first, UINT last, BOOL remove )
{
    EVENTMSG event;

    if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN ||
        msg->message == WM_KEYUP || msg->message == WM_SYSKEYUP)
        switch (msg->wParam)
        {
            case VK_LSHIFT: case VK_RSHIFT:
                msg->wParam = VK_SHIFT;
                break;
            case VK_LCONTROL: case VK_RCONTROL:
                msg->wParam = VK_CONTROL;
                break;
            case VK_LMENU: case VK_RMENU:
                msg->wParam = VK_MENU;
                break;
        }

    /* FIXME: is this really the right place for this hook? */
    event.message = msg->message;
    event.hwnd    = msg->hwnd;
    event.time    = msg->time;
    event.paramL  = (msg->wParam & 0xFF) | (HIWORD(msg->lParam) << 8);
    event.paramH  = msg->lParam & 0x7FFF;
    if (HIWORD(msg->lParam) & 0x0100) event.paramH |= 0x8000; /* special_key - bit */
    HOOK_CallHooks( WH_JOURNALRECORD, HC_ACTION, 0, (LPARAM)&event, TRUE );

    /* check message filters */
    if (msg->message < first || msg->message > last) return FALSE;
    if (!check_hwnd_filter( msg, hwnd_filter )) return FALSE;

    if (remove)
    {
        if((msg->message == WM_KEYDOWN) &&
           (msg->hwnd != GetDesktopWindow_k()))
        {
            /* Handle F1 key by sending out WM_HELP message */
            if (msg->wParam == VK_F1)
            {
                PostMessageW_k( msg->hwnd, WM_KEYF1, 0, 0 );
            }
            else if(msg->wParam >= VK_BROWSER_BACK &&
                    msg->wParam <= VK_LAUNCH_APP2)
            {
                /* FIXME: Process keystate */
                SendMessageW_k(msg->hwnd, WM_APPCOMMAND, (WPARAM)msg->hwnd, MAKELPARAM(0, (FAPPCOMMAND_KEY | (msg->wParam - VK_BROWSER_BACK + 1))));
            }
        }
        else if (msg->message == WM_KEYUP)
        {
            /* Handle VK_APPS key by posting a WM_CONTEXTMENU message */
            if (msg->wParam == VK_APPS && !MENU_IsMenuActive())
                PostMessageW_k(msg->hwnd, WM_CONTEXTMENU, (WPARAM)msg->hwnd, -1);
        }
    }

    if (HOOK_CallHooks( WH_KEYBOARD, remove ? HC_ACTION : HC_NOREMOVE,
                        LOWORD(msg->wParam), msg->lParam, TRUE ))
    {
        /* skip this message */
        HOOK_CallHooks( WH_CBT, HCBT_KEYSKIPPED, LOWORD(msg->wParam), msg->lParam, TRUE );
        accept_hardware_message( hw_id, TRUE, 0 );
        return FALSE;
    }
    accept_hardware_message( hw_id, remove, 0 );

    //if ( remove && msg->message == WM_KEYDOWN )
        //if (ImmProcessKey(msg->hwnd, GetKeyboardLayout_k(0), msg->wParam, msg->lParam, 0) )
        //    msg->wParam = VK_PROCESSKEY;

    return TRUE;
}


/***********************************************************************
 *          process_mouse_message
 *
 * returns TRUE if the contents of 'msg' should be passed to the application
 */
static BOOL process_mouse_message( MSG *msg, UINT hw_id, ULONG_PTR extra_info, HWND hwnd_filter,
                                   UINT first, UINT last, BOOL remove )
{
    static MSG clk_msg;

    POINT pt;
    UINT message;
    INT hittest;
    EVENTMSG event;
    GUITHREADINFO info;
    MOUSEHOOKSTRUCT hook;
    BOOL eatMsg;

    /* find the window to dispatch this mouse message to */

    info.cbSize = sizeof(info);
    GetGUIThreadInfo_k( GetCurrentThreadId_k(), &info );
    if (info.hwndCapture)
    {
        hittest = HTCLIENT;
        msg->hwnd = info.hwndCapture;
    }
    else
    {
        msg->hwnd = WINPOS_WindowFromPoint( msg->hwnd, msg->pt, &hittest );
    }

    if (!msg->hwnd || !WIN_IsCurrentThread( msg->hwnd ))
    {
        accept_hardware_message( hw_id, TRUE, msg->hwnd );
        return FALSE;
    }

    /* FIXME: is this really the right place for this hook? */
    event.message = msg->message;
    event.time    = msg->time;
    event.hwnd    = msg->hwnd;
    event.paramL  = msg->pt.x;
    event.paramH  = msg->pt.y;
    HOOK_CallHooks( WH_JOURNALRECORD, HC_ACTION, 0, (LPARAM)&event, TRUE );

    if (!check_hwnd_filter( msg, hwnd_filter )) return FALSE;

    pt = msg->pt;
    message = msg->message;
    /* Note: windows has no concept of a non-client wheel message */
    if (message != WM_MOUSEWHEEL)
    {
        if (hittest != HTCLIENT)
        {
            message += WM_NCMOUSEMOVE - WM_MOUSEMOVE;
            msg->wParam = hittest;
        }
        else
        {
            /* coordinates don't get translated while tracking a menu */
            /* FIXME: should differentiate popups and top-level menus */
            if (!(info.flags & GUI_INMENUMODE))
                ScreenToClient_k( msg->hwnd, &pt );
        }
    }
    msg->lParam = MAKELONG( pt.x, pt.y );

    /* translate double clicks */

    if ((msg->message == WM_LBUTTONDOWN) ||
        (msg->message == WM_RBUTTONDOWN) ||
        (msg->message == WM_MBUTTONDOWN) ||
        (msg->message == WM_XBUTTONDOWN))
    {
        BOOL update = remove;

        /* translate double clicks -
	 * note that ...MOUSEMOVEs can slip in between
	 * ...BUTTONDOWN and ...BUTTONDBLCLK messages */

        if ((info.flags & (GUI_INMENUMODE|GUI_INMOVESIZE)) ||
            hittest != HTCLIENT ||
            (GetClassLongA_k( msg->hwnd, GCL_STYLE ) & CS_DBLCLKS))
        {
           if ((msg->message == clk_msg.message) &&
               (msg->hwnd == clk_msg.hwnd) &&
               (msg->wParam == clk_msg.wParam) &&
               (msg->time - clk_msg.time < GetDoubleClickTime_k()) &&
               (abs(msg->pt.x - clk_msg.pt.x) < GetSystemMetrics_k(SM_CXDOUBLECLK)/2) &&
               (abs(msg->pt.y - clk_msg.pt.y) < GetSystemMetrics_k(SM_CYDOUBLECLK)/2))
           {
               message += (WM_LBUTTONDBLCLK - WM_LBUTTONDOWN);
               if (update)
               {
                   clk_msg.message = 0;  /* clear the double click conditions */
                   update = FALSE;
               }
           }
        }
        if (message < first || message > last) return FALSE;
        /* update static double click conditions */
        if (update) clk_msg = *msg;
    }
    else
    {
        if (message < first || message > last) return FALSE;
    }

    /* message is accepted now (but may still get dropped) */

    hook.pt           = msg->pt;
    hook.hwnd         = msg->hwnd;
    hook.wHitTestCode = hittest;
    hook.dwExtraInfo  = extra_info;
    if (HOOK_CallHooks( WH_MOUSE, remove ? HC_ACTION : HC_NOREMOVE,
                        message, (LPARAM)&hook, TRUE ))
    {
        hook.pt           = msg->pt;
        hook.hwnd         = msg->hwnd;
        hook.wHitTestCode = hittest;
        hook.dwExtraInfo  = extra_info;
        HOOK_CallHooks( WH_CBT, HCBT_CLICKSKIPPED, message, (LPARAM)&hook, TRUE );
        accept_hardware_message( hw_id, TRUE, 0 );
        return FALSE;
    }

    if ((hittest == HTERROR) || (hittest == HTNOWHERE))
    {
        SendMessageW_k( msg->hwnd, WM_SETCURSOR, (WPARAM)msg->hwnd,
                      MAKELONG( hittest, msg->message ));
        accept_hardware_message( hw_id, TRUE, 0 );
        return FALSE;
    }

    accept_hardware_message( hw_id, remove, 0 );

    if (!remove || info.hwndCapture)
    {
        msg->message = message;
        return TRUE;
    }

    eatMsg = FALSE;

    if ((msg->message == WM_LBUTTONDOWN) ||
        (msg->message == WM_RBUTTONDOWN) ||
        (msg->message == WM_MBUTTONDOWN) ||
        (msg->message == WM_XBUTTONDOWN))
    {
        /* Send the WM_PARENTNOTIFY,
         * note that even for double/nonclient clicks
         * notification message is still WM_L/M/RBUTTONDOWN.
         */
        send_parent_notify( msg->hwnd, msg->message, 0, msg->pt );

        /* Activate the window if needed */

        if (msg->hwnd != info.hwndActive)
        {
            HWND hwndTop = msg->hwnd;
            while (hwndTop)
            {
                if ((GetWindowLongW_k( hwndTop, GWL_STYLE ) & (WS_POPUP|WS_CHILD)) != WS_CHILD) break;
                hwndTop = GetParent_k( hwndTop );
            }

            if (hwndTop && hwndTop != GetDesktopWindow_k())
            {
                LONG ret = SendMessageW_k( msg->hwnd, WM_MOUSEACTIVATE, (WPARAM)hwndTop,
                                         MAKELONG( hittest, msg->message ) );
                switch(ret)
                {
                case MA_NOACTIVATEANDEAT:
                    eatMsg = TRUE;
                    /* fall through */
                case MA_NOACTIVATE:
                    break;
                case MA_ACTIVATEANDEAT:
                    eatMsg = TRUE;
                    /* fall through */
                case MA_ACTIVATE:
                case 0:
                    if (!FOCUS_MouseActivate( hwndTop )) eatMsg = TRUE;
                    break;
                default:
                    WARN( "unknown WM_MOUSEACTIVATE code %d\n", ret );
                    break;
                }
            }
        }
    }

    /* send the WM_SETCURSOR message */

    /* Windows sends the normal mouse message as the message parameter
       in the WM_SETCURSOR message even if it's non-client mouse message */
    SendMessageW_k( msg->hwnd, WM_SETCURSOR, (WPARAM)msg->hwnd, MAKELONG( hittest, msg->message ));

    msg->message = message;
    return !eatMsg;
}


/***********************************************************************
 *           process_hardware_message
 *
 * Process a hardware message; return TRUE if message should be passed on to the app
 */
static BOOL process_hardware_message( MSG *msg, UINT hw_id, const struct hardware_msg_data *msg_data,
                                      HWND hwnd_filter, UINT first, UINT last, BOOL remove )
{
    if (msg->message == WM_INPUT)
        return process_rawinput_message( msg, msg_data );

    if (is_keyboard_message( msg->message ))
        return process_keyboard_message( msg, hw_id, hwnd_filter, first, last, remove );

    if (is_mouse_message( msg->message ))
        return process_mouse_message( msg, hw_id, msg_data->info, hwnd_filter, first, last, remove );

    ERR( "unknown message type %x\n", msg->message );
    return FALSE;
}


/***********************************************************************
 *           call_sendmsg_callback
 *
 * Call the callback function of SendMessageCallback.
 */
static INLINE void call_sendmsg_callback( SENDASYNCPROC callback, HWND hwnd, UINT msg,
                                          ULONG_PTR data, LRESULT result )
{
    if (!callback) return;

    callback( hwnd, msg, data, result );
}


/***********************************************************************
 *           peek_message
 *
 * Peek for a message matching the given parameters. Return FALSE if none available.
 * All pending sent messages are processed before returning.
 */
static BOOL peek_message( MSG *msg, HWND hwnd, UINT first, UINT last, UINT flags, UINT changed_mask )
{
    LRESULT result;
    struct user_thread_info *thread_info = get_user_thread_info();
    struct received_message_info info, *old_info;
    struct message message;

    if (!first && !last) last = ~0;
    if (hwnd == HWND_BROADCAST) hwnd = HWND_TOPMOST;

    for (;;)
    {
        message_data_t *msg_data = NULL;
        message_data_t msg_data_holder;
        DWORD res;
        DWORD hw_id;

        res = get_message_queue(&message, hwnd, first, last, flags, changed_mask, changed_mask & (QS_SENDMESSAGE | QS_SMRESULT), 0, &msg_data);
        info.type = message.type;
        info.msg.hwnd = message.win;
        info.msg.message = message.msg;
        info.msg.wParam  = message.wparam;
        info.msg.lParam  = message.lparam;
        info.msg.time    = message.time;
        info.msg.pt.x    = 0;
        info.msg.pt.y    = 0;
        hw_id            = 0;
        thread_info->active_hooks = get_active_hooks();       

        if (res)
        {
            return FALSE;
        }
        if (msg_data) {
            msg_data_holder = *msg_data;
            kfree(msg_data);
            msg_data = &msg_data_holder;
        }
        TRACE( "got type %d msg %x (%s) hwnd %p wp %lx lp %lx\n",
               info.type, info.msg.message,
               (info.type == MSG_WINEVENT) ? "MSG_WINEVENT" : SPY_GetMsgName(info.msg.message, info.msg.hwnd),
               info.msg.hwnd, info.msg.wParam, info.msg.lParam );

        switch(info.type)
        {
        case MSG_ASCII:
        case MSG_UNICODE:
            info.flags = ISMEX_SEND;
            break;
        case MSG_NOTIFY:
            info.flags = ISMEX_NOTIFY;
            break;
        case MSG_CALLBACK:
            info.flags = ISMEX_CALLBACK;
            break;
        case MSG_CALLBACK_RESULT:
            call_sendmsg_callback( msg_data->callback.callback,
                                       info.msg.hwnd, info.msg.message,
                                       msg_data->callback.data, msg_data->callback.result );
            continue;
        case MSG_WINEVENT:
            {
                WINEVENTPROC hook_proc;

                hook_proc = msg_data->winevent.hook_proc;
                if (msg_data->winevent.module[0])
                {
                    if (!(hook_proc = get_hook_proc( hook_proc, msg_data->winevent.module )))
                    {
                        ERR( "invalid winevent hook module name %s\n", debugstr_w(msg_data->winevent.module) );
                        continue;
                    }
                }

                hook_proc( msg_data->winevent.hook, info.msg.message,
                           info.msg.hwnd, info.msg.wParam, info.msg.lParam,
                           msg_data->winevent.tid, info.msg.time );

            }
            continue;
        case MSG_HOOK_LL:
            info.flags = ISMEX_SEND;
            result = 0;
            if (info.msg.message == WH_KEYBOARD_LL)
            {
                KBDLLHOOKSTRUCT hook;

                hook.vkCode      = LOWORD( info.msg.lParam );
                hook.scanCode    = HIWORD( info.msg.lParam );
                hook.flags       = msg_data->hardware.flags;
                hook.time        = info.msg.time;
                hook.dwExtraInfo = msg_data->hardware.info;
                TRACE( "calling keyboard LL hook vk %x scan %x flags %x time %u info %lx\n",
                       hook.vkCode, hook.scanCode, hook.flags, hook.time, hook.dwExtraInfo );
                result = HOOK_CallHooks( WH_KEYBOARD_LL, HC_ACTION, info.msg.wParam, (LPARAM)&hook, TRUE );
            }
            else if (info.msg.message == WH_MOUSE_LL)
            {
                MSLLHOOKSTRUCT hook;

                hook.pt.x        = msg_data->hardware.x;
                hook.pt.y        = msg_data->hardware.y;
                hook.mouseData   = info.msg.lParam;
                hook.flags       = msg_data->hardware.flags;
                hook.time        = info.msg.time;
                hook.dwExtraInfo = msg_data->hardware.info;
                TRACE( "calling mouse LL hook pos %d,%d data %x flags %x time %u info %lx\n",
                       hook.pt.x, hook.pt.y, hook.mouseData, hook.flags, hook.time, hook.dwExtraInfo );
                result = HOOK_CallHooks( WH_MOUSE_LL, HC_ACTION, info.msg.wParam, (LPARAM)&hook, TRUE );
            }
            reply_message( &info, result, TRUE );
            continue;
        case MSG_HARDWARE:
            info.msg.pt.x = msg_data->hardware.x;
            info.msg.pt.y = msg_data->hardware.y;
            hw_id         = msg_data->hardware.hw_id;
            if (!process_hardware_message( &info.msg, hw_id, &msg_data->hardware,
                                            hwnd, first, last, flags & PM_REMOVE ))
            {
                TRACE("dropping msg %x\n", info.msg.message );
                continue;  /* ignore it */
            }
            *msg = info.msg;
            thread_info->GetMessagePosVal = MAKELONG( info.msg.pt.x, info.msg.pt.y );
            thread_info->GetMessageTimeVal = info.msg.time;
            thread_info->GetMessageExtraInfoVal = msg_data->hardware.info;
            HOOK_CallHooks( WH_GETMESSAGE, HC_ACTION, flags & PM_REMOVE, (LPARAM)msg, TRUE );
            return TRUE;
        case MSG_POSTED:
            if (info.msg.message & 0x80000000)  /* internal message */
            {
                if (flags & PM_REMOVE)
                {
                    handle_internal_message( info.msg.hwnd, info.msg.message,
                                             info.msg.wParam, info.msg.lParam );
                    /* if this is a nested call return right away */
                    if (first == info.msg.message && last == info.msg.message)
                    {
                        return FALSE;
                    }
                }
                else
                    peek_message( msg, info.msg.hwnd, info.msg.message,
                                  info.msg.message, flags | PM_REMOVE, changed_mask );
                continue;
            }
	    if (info.msg.message >= WM_DDE_FIRST && info.msg.message <= WM_DDE_LAST)
	    {
		    //if (!unpack_dde_message( info.msg.hwnd, info.msg.message, &info.msg.wParam,
            //                                 &info.msg.lParam, &msg_data->packed ))
            //            continue;  /* ignore it */
	    }
            *msg = info.msg;
            msg->pt.x = (short)LOWORD( thread_info->GetMessagePosVal );
            msg->pt.y = (short)HIWORD( thread_info->GetMessagePosVal );
            thread_info->GetMessageTimeVal = info.msg.time;
            thread_info->GetMessageExtraInfoVal = 0;
            HOOK_CallHooks( WH_GETMESSAGE, HC_ACTION, flags & PM_REMOVE, (LPARAM)msg, TRUE );
            return TRUE;
        }

        /* if we get here, we have a sent message; call the window procedure */
        old_info = thread_info->receive_info;
        thread_info->receive_info = &info;
        result = call_window_proc( info.msg.hwnd, info.msg.message, info.msg.wParam,
                                   info.msg.lParam, (info.type != MSG_ASCII), FALSE,
                                   WMCHAR_MAP_RECVMESSAGE );
        reply_message( &info, result, TRUE );
        thread_info->receive_info = old_info;

        /* if some PM_QS* flags were specified, only handle sent messages from now on */
        if (HIWORD(flags) && !changed_mask) flags = PM_QS_SENDMESSAGE | LOWORD(flags);
    }
}


/***********************************************************************
 *           process_sent_messages
 *
 * Process all pending sent messages.
 */
static INLINE void process_sent_messages(void) 
{
    MSG msg;
    peek_message( &msg, 0, 0, 0, PM_REMOVE | PM_QS_SENDMESSAGE, 0 );
}


/***********************************************************************
 *           wait_message_reply
 *
 * Wait until a sent message gets replied to.
 */
static void wait_message_reply( UINT flags )
{
    HANDLE queue = currentThread()->queue->handle;

    for (;;)
    {
        unsigned int wake_bits = 0;
        DWORD wake_mask = QS_SMRESULT | ((flags & SMTO_BLOCK) ? 0 : QS_SENDMESSAGE);

        set_queue_mask(currentThread()->queue, wake_mask, wake_mask, &wake_bits, NULL, 1);

        if (wake_bits & QS_SMRESULT) return;  /* got a result */
        if (wake_bits & QS_SENDMESSAGE)
        {
            /* Process the sent message immediately */
            process_sent_messages();
            continue;
        }

        wow_handlers.wait_message( 1, &queue, INFINITE, QS_SENDMESSAGE, 0 );
    }
}

/***********************************************************************
 *		put_message_in_queue
 *
 * Put a sent message into the destination queue.
 * For inter-process message, reply_size is set to expected size of reply data.
 */
static BOOL put_message_in_queue( const struct send_message_info *info, size_t *reply_size )
{
    struct packed_message data;
    message_data_t msg_data;
    unsigned int res;
    int timeout = -1;

    /* Check for INFINITE timeout for compatibility with Win9x,
     * although Windows >= NT does not do so
     */
    if (info->type != MSG_NOTIFY &&
        info->type != MSG_CALLBACK &&
        info->type != MSG_POSTED &&
        info->timeout &&
        info->timeout != INFINITE)
    {
        /* timeout is signed despite the prototype */
        timeout = max( 0, (int)info->timeout ) * -10000;
    }

    memset( &data, 0, sizeof(data) );
    if (info->type == MSG_CALLBACK)
    {
        msg_data.callback.callback = info->callback;
        msg_data.callback.data     = info->data;
        msg_data.callback.result   = 0;
        data.data[0] = &msg_data;
        data.size[0] = sizeof(msg_data.callback);
        data.count = 1;
    }
    else if (info->type == MSG_POSTED && info->msg >= WM_DDE_FIRST && info->msg <= WM_DDE_LAST)
    {
        // :TODO:
        //return post_dde_message( &data, info );
    }
    res = send_message_queue(info->dest_tid, (info->flags & SMTO_ABORTIFHUNG)?SEND_MSG_ABORT_IF_HUNG:0, info->type, info->hwnd, info->msg, info->wparam, info->lparam, timeout, &msg_data);
    if (res) {
        SetLastError_k(res);
    }
    return !res;
}


/***********************************************************************
 *		retrieve_reply
 *
 * Retrieve a message reply from the server.
 */
static LRESULT retrieve_reply( const struct send_message_info *info,
                               size_t reply_size, LRESULT *result )
{
    get_message_reply_queue(currentThread()->queue, 1, result);
    return 0;
}


/***********************************************************************
 *		send_inter_thread_message
 */
static LRESULT send_inter_thread_message( const struct send_message_info *info, LRESULT *res_ptr )
{
    size_t reply_size = 0;

    TRACE( "hwnd %p msg %x (%s) wp %lx lp %lx\n",
           info->hwnd, info->msg, SPY_GetMsgName(info->msg, info->hwnd), info->wparam, info->lparam );

    USER_CheckNotLock();

    if (!put_message_in_queue( info, &reply_size )) return 0;

    /* there's no reply to wait for on notify/callback messages */
    if (info->type == MSG_NOTIFY || info->type == MSG_CALLBACK) return 1;

    wait_message_reply( info->flags );
    return retrieve_reply( info, reply_size, res_ptr );
}


/***********************************************************************
 *		send_inter_thread_callback
 */
static LRESULT send_inter_thread_callback( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
                                           LRESULT *result, void *arg )
{
    struct send_message_info *info = arg;
    info->hwnd   = hwnd;
    info->msg    = msg;
    info->wparam = wp;
    info->lparam = lp;
    return send_inter_thread_message( info, result );
}


/***********************************************************************
 *		send_message
 *
 * Backend implementation of the various SendMessage functions.
 */
static BOOL send_message( struct send_message_info *info, DWORD_PTR *res_ptr, BOOL unicode )
{
    DWORD dest_pid;
    BOOL ret;
    LRESULT result;

    if (is_broadcast(info->hwnd))
    {
        EnumWindows_k( broadcast_message_callback, (LPARAM)info );
        if (res_ptr) *res_ptr = 1;
        return TRUE;
    }

    if (!(info->dest_tid = GetWindowThreadProcessId_k( info->hwnd, &dest_pid ))) return FALSE;

    if (USER_IsExitingThread( info->dest_tid )) return FALSE;

    SPY_EnterMessage( SPY_SENDMESSAGE, info->hwnd, info->msg, info->wparam, info->lparam );

    if (info->dest_tid == GetCurrentThreadId_k())
    {
        result = call_window_proc( info->hwnd, info->msg, info->wparam, info->lparam,
                                   unicode, TRUE, info->wm_char );
        if (info->type == MSG_CALLBACK)
            call_sendmsg_callback( info->callback, info->hwnd, info->msg, info->data, result );
        ret = TRUE;
    }
    else
    {
        /* MSG_ASCII can be sent unconverted except for WM_CHAR; everything else needs to be Unicode */
        if (!unicode && is_unicode_message( info->msg ) &&
            (info->type != MSG_ASCII || info->msg == WM_CHAR))
            ret = WINPROC_CallProcAtoW( send_inter_thread_callback, info->hwnd, info->msg,
                                        info->wparam, info->lparam, &result, info, info->wm_char );
        else
            ret = send_inter_thread_message( info, &result );
    }

    SPY_ExitMessage( SPY_RESULT_OK, info->hwnd, info->msg, result, info->wparam, info->lparam );
    if (ret && res_ptr) *res_ptr = result;
    return ret;
}


/***********************************************************************
 *		send_hardware_message
 */
NTSTATUS send_hardware_message( HWND hwnd, const INPUT *input, UINT flags )
{
    struct user_thread_info *thread_info = get_user_thread_info();
    struct send_message_info info;
    int prev_x, prev_y;
    NTSTATUS ret;
    BOOL wait;
    struct desktop *desktop;

    if (!(desktop = currentThread()->input->desktop)) return 0;

    prev_x = desktop->cursor.x;
    prev_y = desktop->cursor.y;

    info.type     = MSG_HARDWARE;
    info.dest_tid = 0;
    info.hwnd     = hwnd;
    info.flags    = 0;
    info.timeout  = 0;

    ret = send_hardware_message_queue(hwnd, input, flags, &wait);

    if (!ret)
    {
        if (thread_info->key_state) thread_info->key_state_time = GetTickCount_k();
        if ((flags & SEND_HWMSG_INJECTED) && (prev_x != desktop->cursor.x || prev_y != desktop->cursor.y))
            USER_Driver->pSetCursorPos( desktop->cursor.x, desktop->cursor.y );
    }

    if (wait)
    {
        LRESULT ignored;
        wait_message_reply( 0 );
        retrieve_reply( &info, 0, &ignored );
    }
    return ret;
}


/***********************************************************************
 *		MSG_SendInternalMessageTimeout
 *
 * Same as SendMessageTimeoutW but sends the message to a specific thread
 * without requiring a window handle. Only works for internal Wine messages.
 */
LRESULT MSG_SendInternalMessageTimeout( DWORD dest_pid, DWORD dest_tid,
                                        UINT msg, WPARAM wparam, LPARAM lparam,
                                        UINT flags, UINT timeout, PDWORD_PTR res_ptr )
{
    struct send_message_info info;
    LRESULT ret, result;

    assert( msg & 0x80000000 );  /* must be an internal Wine message */

    info.type     = MSG_UNICODE;
    info.dest_tid = dest_tid;
    info.hwnd     = 0;
    info.msg      = msg;
    info.wparam   = wparam;
    info.lparam   = lparam;
    info.flags    = flags;
    info.timeout  = timeout;

    if (USER_IsExitingThread( dest_tid )) return 0;

    if (dest_tid == GetCurrentThreadId_k())
    {
        result = handle_internal_message( 0, msg, wparam, lparam );
        ret = 1;
    }
    else
    {
        ret = send_inter_thread_message( &info, &result );
    }
    if (ret && res_ptr) *res_ptr = result;
    return ret;
}


/***********************************************************************
 *		SendMessageTimeoutW  (USER32.@)
 */
LRESULT WINAPI SendMessageTimeoutW_k( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                    UINT flags, UINT timeout, PDWORD_PTR res_ptr )
{
    struct send_message_info info;

    info.type    = MSG_UNICODE;
    info.hwnd    = hwnd;
    info.msg     = msg;
    info.wparam  = wparam;
    info.lparam  = lparam;
    info.flags   = flags;
    info.timeout = timeout;

    return send_message( &info, res_ptr, TRUE );
}

/***********************************************************************
 *		SendMessageTimeoutA  (USER32.@)
 */
LRESULT WINAPI SendMessageTimeoutA_k( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                    UINT flags, UINT timeout, PDWORD_PTR res_ptr )
{
    struct send_message_info info;

    info.type    = MSG_ASCII;
    info.hwnd    = hwnd;
    info.msg     = msg;
    info.wparam  = wparam;
    info.lparam  = lparam;
    info.flags   = flags;
    info.timeout = timeout;
    info.wm_char  = WMCHAR_MAP_SENDMESSAGETIMEOUT;

    return send_message( &info, res_ptr, FALSE );
}


/***********************************************************************
 *		SendMessageW  (USER32.@)
 */
LRESULT WINAPI SendMessageW_k( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    DWORD_PTR res = 0;
    struct send_message_info info;

    info.type    = MSG_UNICODE;
    info.hwnd    = hwnd;
    info.msg     = msg;
    info.wparam  = wparam;
    info.lparam  = lparam;
    info.flags   = SMTO_NORMAL;
    info.timeout = 0;

    send_message( &info, &res, TRUE );
    return res;
}


/***********************************************************************
 *		SendMessageA  (USER32.@)
 */
LRESULT WINAPI SendMessageA_k( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    DWORD_PTR res = 0;
    struct send_message_info info;

    info.type    = MSG_ASCII;
    info.hwnd    = hwnd;
    info.msg     = msg;
    info.wparam  = wparam;
    info.lparam  = lparam;
    info.flags   = SMTO_NORMAL;
    info.timeout = 0;
    info.wm_char  = WMCHAR_MAP_SENDMESSAGE;

    send_message( &info, &res, FALSE );
    return res;
}


/***********************************************************************
 *		SendNotifyMessageA  (USER32.@)
 */
BOOL WINAPI SendNotifyMessageA_k( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    struct send_message_info info;

    if (is_pointer_message(msg))
    {
        SetLastError_k( ERROR_MESSAGE_SYNC_ONLY );
        return FALSE;
    }

    info.type    = MSG_NOTIFY;
    info.hwnd    = hwnd;
    info.msg     = msg;
    info.wparam  = wparam;
    info.lparam  = lparam;
    info.flags   = 0;
    info.wm_char = WMCHAR_MAP_SENDMESSAGETIMEOUT;

    return send_message( &info, NULL, FALSE );
}


/***********************************************************************
 *		SendNotifyMessageW  (USER32.@)
 */
BOOL WINAPI SendNotifyMessageW_k( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    struct send_message_info info;

    if (is_pointer_message(msg))
    {
        SetLastError_k( ERROR_MESSAGE_SYNC_ONLY );
        return FALSE;
    }

    info.type    = MSG_NOTIFY;
    info.hwnd    = hwnd;
    info.msg     = msg;
    info.wparam  = wparam;
    info.lparam  = lparam;
    info.flags   = 0;

    return send_message( &info, NULL, TRUE );
}


/***********************************************************************
 *		SendMessageCallbackA  (USER32.@)
 */
BOOL WINAPI SendMessageCallbackA_k( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                  SENDASYNCPROC callback, ULONG_PTR data )
{
    struct send_message_info info;

    if (is_pointer_message(msg))
    {
        SetLastError_k( ERROR_MESSAGE_SYNC_ONLY );
        return FALSE;
    }

    info.type     = MSG_CALLBACK;
    info.hwnd     = hwnd;
    info.msg      = msg;
    info.wparam   = wparam;
    info.lparam   = lparam;
    info.callback = callback;
    info.data     = data;
    info.flags    = 0;
    info.wm_char  = WMCHAR_MAP_SENDMESSAGETIMEOUT;

    return send_message( &info, NULL, FALSE );
}


/***********************************************************************
 *		SendMessageCallbackW  (USER32.@)
 */
BOOL WINAPI SendMessageCallbackW_k( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                  SENDASYNCPROC callback, ULONG_PTR data )
{
    struct send_message_info info;

    if (is_pointer_message(msg))
    {
        SetLastError_k( ERROR_MESSAGE_SYNC_ONLY );
        return FALSE;
    }

    info.type     = MSG_CALLBACK;
    info.hwnd     = hwnd;
    info.msg      = msg;
    info.wparam   = wparam;
    info.lparam   = lparam;
    info.callback = callback;
    info.data     = data;
    info.flags    = 0;

    return send_message( &info, NULL, TRUE );
}


/***********************************************************************
 *		ReplyMessage  (USER32.@)
 */
BOOL WINAPI ReplyMessage_k( LRESULT result )
{
    struct received_message_info *info = get_user_thread_info()->receive_info;

    if (!info) return FALSE;
    reply_message( info, result, FALSE );
    return TRUE;
}


/***********************************************************************
 *		InSendMessage  (USER32.@)
 */
BOOL WINAPI InSendMessage_k(void)
{
    return (InSendMessageEx_k(NULL) & (ISMEX_SEND|ISMEX_REPLIED)) == ISMEX_SEND;
}


/***********************************************************************
 *		InSendMessageEx  (USER32.@)
 */
DWORD WINAPI InSendMessageEx_k( LPVOID reserved )
{
    struct received_message_info *info = get_user_thread_info()->receive_info;

    if (info) return info->flags;
    return ISMEX_NOSEND;
}


/***********************************************************************
 *		PostMessageA  (USER32.@)
 */
BOOL WINAPI PostMessageA_k( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    if (!map_wparam_AtoW( msg, &wparam, WMCHAR_MAP_POSTMESSAGE )) return TRUE;
    return PostMessageW_k( hwnd, msg, wparam, lparam );
}


/***********************************************************************
 *		PostMessageW  (USER32.@)
 */
BOOL WINAPI PostMessageW_k( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    struct send_message_info info;

    if (is_pointer_message( msg ))
    {
        SetLastError_k( ERROR_MESSAGE_SYNC_ONLY );
        return FALSE;
    }

    TRACE( "hwnd %p msg %x (%s) wp %lx lp %lx\n",
           hwnd, msg, SPY_GetMsgName(msg, hwnd), wparam, lparam );

    info.type   = MSG_POSTED;
    info.hwnd   = hwnd;
    info.msg    = msg;
    info.wparam = wparam;
    info.lparam = lparam;
    info.flags  = 0;

    if (is_broadcast(hwnd))
    {
        EnumWindows_k( broadcast_message_callback, (LPARAM)&info );
        return TRUE;
    }

    if (!hwnd) return PostThreadMessageW_k( GetCurrentThreadId_k(), msg, wparam, lparam );

    if (!(info.dest_tid = GetWindowThreadProcessId_k( hwnd, NULL ))) return FALSE;

    if (USER_IsExitingThread( info.dest_tid )) return TRUE;

    return put_message_in_queue( &info, NULL );
}


/**********************************************************************
 *		PostThreadMessageA  (USER32.@)
 */
BOOL WINAPI PostThreadMessageA_k( DWORD thread, UINT msg, WPARAM wparam, LPARAM lparam )
{
    if (!map_wparam_AtoW( msg, &wparam, WMCHAR_MAP_POSTMESSAGE )) return TRUE;
    return PostThreadMessageW_k( thread, msg, wparam, lparam );
}


/**********************************************************************
 *		PostThreadMessageW  (USER32.@)
 */
BOOL WINAPI PostThreadMessageW_k( DWORD thread, UINT msg, WPARAM wparam, LPARAM lparam )
{
    struct send_message_info info;

    if (is_pointer_message( msg ))
    {
        SetLastError_k( ERROR_MESSAGE_SYNC_ONLY );
        return FALSE;
    }
    if (USER_IsExitingThread( thread )) return TRUE;

    info.type     = MSG_POSTED;
    info.dest_tid = thread;
    info.hwnd     = 0;
    info.msg      = msg;
    info.wparam   = wparam;
    info.lparam   = lparam;
    info.flags    = 0;
    return put_message_in_queue( &info, NULL );
}


/***********************************************************************
 *		PostQuitMessage  (USER32.@)
 *
 * Posts a quit message to the current thread's message queue.
 *
 * PARAMS
 *  exit_code [I] Exit code to return from message loop.
 *
 * RETURNS
 *  Nothing.
 *
 * NOTES
 *  This function is not the same as calling:
 *|PostThreadMessage(GetCurrentThreadId(), WM_QUIT, exit_code, 0);
 *  It instead sets a flag in the message queue that signals it to generate
 *  a WM_QUIT message when there are no other pending sent or posted messages
 *  in the queue.
 */
void WINAPI PostQuitMessage_k( INT exit_code )
{
    struct msg_queue* queue = currentThread()->queue;

    queue->quit_message = 1;
    queue->exit_code = exit_code;
    set_queue_bits( queue, QS_POSTMESSAGE|QS_ALLPOSTMESSAGE );
}

/* check for driver events if we detect that the app is not properly consuming messages */
static INLINE void check_for_driver_events( UINT msg )
{
    if (get_user_thread_info()->message_count > 200)
    {
        flush_window_surfaces( FALSE );
        USER_Driver->pMsgWaitForMultipleObjectsEx( 0, NULL, 0, QS_ALLINPUT, 0 );
    }
    else if (msg == WM_TIMER || msg == WM_SYSTIMER)
    {
        /* driver events should have priority over timers, so make sure we'll check for them soon */
        get_user_thread_info()->message_count += 100;
    }
    else get_user_thread_info()->message_count++;
}

/***********************************************************************
 *		PeekMessageW  (USER32.@)
 */
BOOL WINAPI PeekMessageW_k( MSG *msg_out, HWND hwnd, UINT first, UINT last, UINT flags )
{
    MSG msg;

    USER_CheckNotLock();
    check_for_driver_events( 0 );

    if (!peek_message( &msg, hwnd, first, last, flags, 0 ))
    {
        DWORD ret;

        flush_window_surfaces( TRUE );
        ret = wow_handlers.wait_message( 0, NULL, 0, QS_ALLINPUT, 0 );
        /* if we received driver events, check again for a pending message */
        if (ret == WAIT_TIMEOUT || !peek_message( &msg, hwnd, first, last, flags, 0 )) return FALSE;
    }

    check_for_driver_events( msg.message );

    /* copy back our internal safe copy of message data to msg_out.
     * msg_out is a variable from the *program*, so it can't be used
     * internally as it can get "corrupted" by our use of SendMessage()
     * (back to the program) inside the message handling itself. */
    if (!msg_out)
    {
        SetLastError_k( ERROR_NOACCESS );
        return FALSE;
    }
    *msg_out = msg;
    return TRUE;
}


/***********************************************************************
 *		PeekMessageA  (USER32.@)
 */
BOOL WINAPI PeekMessageA_k( MSG *msg, HWND hwnd, UINT first, UINT last, UINT flags )
{
    if (get_pending_wmchar( msg, first, last, (flags & PM_REMOVE) )) return TRUE;
    if (!PeekMessageW_k( msg, hwnd, first, last, flags )) return FALSE;
    map_wparam_WtoA( msg, (flags & PM_REMOVE) );
    return TRUE;
}


/***********************************************************************
 *		GetMessageW  (USER32.@)
 */
BOOL WINAPI GetMessageW_k( MSG *msg, HWND hwnd, UINT first, UINT last )
{
    HANDLE queueHandle = currentThread()->queue->handle;
    unsigned int mask = QS_POSTMESSAGE | QS_SENDMESSAGE;  /* Always selected */

    USER_CheckNotLock();
    check_for_driver_events( 0 );

    if (first || last)
    {
        if ((first <= WM_KEYLAST) && (last >= WM_KEYFIRST)) mask |= QS_KEY;
        if ( ((first <= WM_MOUSELAST) && (last >= WM_MOUSEFIRST)) ||
             ((first <= WM_NCMOUSELAST) && (last >= WM_NCMOUSEFIRST)) ) mask |= QS_MOUSE;
        if ((first <= WM_TIMER) && (last >= WM_TIMER)) mask |= QS_TIMER;
        if ((first <= WM_SYSTIMER) && (last >= WM_SYSTIMER)) mask |= QS_TIMER;
        if ((first <= WM_PAINT) && (last >= WM_PAINT)) mask |= QS_PAINT;
    }
    else mask = QS_ALLINPUT;

    while (!peek_message( msg, hwnd, first, last, PM_REMOVE | (mask << 16), mask ))
    {
        flush_window_surfaces( TRUE );
        wow_handlers.wait_message( 1, &queueHandle, INFINITE, mask, 0 );
    }
    check_for_driver_events( msg->message );

    return (msg->message != WM_QUIT);
}


/***********************************************************************
 *		GetMessageA  (USER32.@)
 */
BOOL WINAPI GetMessageA_k( MSG *msg, HWND hwnd, UINT first, UINT last )
{
    if (get_pending_wmchar( msg, first, last, TRUE )) return TRUE;
    GetMessageW_k( msg, hwnd, first, last );
    map_wparam_WtoA( msg, TRUE );
    return (msg->message != WM_QUIT);
}


/***********************************************************************
 *		IsDialogMessageA (USER32.@)
 *		IsDialogMessage  (USER32.@)
 */
BOOL WINAPI IsDialogMessageA_k( HWND hwndDlg, LPMSG pmsg )
{
    MSG msg = *pmsg;
    map_wparam_AtoW( msg.message, &msg.wParam, WMCHAR_MAP_NOMAPPING );
    return IsDialogMessageW_k( hwndDlg, &msg );
}


/***********************************************************************
 *		TranslateMessage (USER32.@)
 *
 * Implementation of TranslateMessage.
 *
 * TranslateMessage translates virtual-key messages into character-messages,
 * as follows :
 * WM_KEYDOWN/WM_KEYUP combinations produce a WM_CHAR or WM_DEADCHAR message.
 * ditto replacing WM_* with WM_SYS*
 * This produces WM_CHAR messages only for keys mapped to ASCII characters
 * by the keyboard driver.
 *
 * If the message is WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP, the
 * return value is nonzero, regardless of the translation.
 *
 */
BOOL WINAPI TranslateMessage_k( const MSG *msg )
{
    UINT message;
    WCHAR wp[8];
    BYTE state[256];
    INT len;

    if (msg->message < WM_KEYFIRST || msg->message > WM_KEYLAST) return FALSE;
    if (msg->message != WM_KEYDOWN && msg->message != WM_SYSKEYDOWN) return TRUE;

    TRACE_(key)("Translating key %s (%04lX), scancode %04x\n",
                SPY_GetVKeyName(msg->wParam), msg->wParam, HIWORD(msg->lParam));

    switch (msg->wParam)
    {
    case VK_PACKET:
        message = (msg->message == WM_KEYDOWN) ? WM_CHAR : WM_SYSCHAR;
        TRACE_(key)("PostMessageW(%p,%s,%04x,%08x)\n",
                    msg->hwnd, SPY_GetMsgName(message, msg->hwnd), HIWORD(msg->lParam), LOWORD(msg->lParam));
        PostMessageW_k( msg->hwnd, message, HIWORD(msg->lParam), LOWORD(msg->lParam));
        return TRUE;

    //case VK_PROCESSKEY:
    //    return ImmTranslateMessage(msg->hwnd, msg->message, msg->wParam, msg->lParam);
    }

    GetKeyboardState_k( state );
    len = ToUnicode_k(msg->wParam, HIWORD(msg->lParam), state, wp, sizeof(wp)/sizeof(WCHAR), 0);
    if (len == -1)
    {
        message = (msg->message == WM_KEYDOWN) ? WM_DEADCHAR : WM_SYSDEADCHAR;
        TRACE_(key)("-1 -> PostMessageW(%p,%s,%04x,%08lx)\n",
            msg->hwnd, SPY_GetMsgName(message, msg->hwnd), wp[0], msg->lParam);
        PostMessageW_k( msg->hwnd, message, wp[0], msg->lParam );
    }
    else if (len > 0)
    {
        INT i;

        message = (msg->message == WM_KEYDOWN) ? WM_CHAR : WM_SYSCHAR;
        //TRACE_(key)("%d -> PostMessageW(%p,%s,<x>,%08lx) for <x> in %s\n", len, msg->hwnd, SPY_GetMsgName(message, msg->hwnd), msg->lParam, debugstr_wn(wp, len));
        for (i = 0; i < len; i++)
            PostMessageW_k( msg->hwnd, message, wp[i], msg->lParam );
    }
    return TRUE;
}


/***********************************************************************
 *		DispatchMessageA (USER32.@)
 *
 * See DispatchMessageW.
 */
LRESULT WINAPI DispatchMessageA_k( const MSG* msg )
{
    LRESULT retval;

      /* Process timer messages */
    if ((msg->message == WM_TIMER) || (msg->message == WM_SYSTIMER))
    {
        if (msg->lParam)
        {
            retval = CallWindowProcA_k( (WNDPROC)msg->lParam, msg->hwnd, msg->message, msg->wParam, GetTickCount_k() );
            return retval;
        }
    }
    if (!msg->hwnd) return 0;

    SPY_EnterMessage( SPY_DISPATCHMESSAGE, msg->hwnd, msg->message,
                      msg->wParam, msg->lParam );

    if (!WINPROC_call_window( msg->hwnd, msg->message, msg->wParam, msg->lParam,
                              &retval, FALSE, WMCHAR_MAP_DISPATCHMESSAGE ))
    {
        if (!IsWindow_k( msg->hwnd )) SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        else SetLastError_k( ERROR_MESSAGE_SYNC_ONLY );
        retval = 0;
    }

    SPY_ExitMessage( SPY_RESULT_OK, msg->hwnd, msg->message, retval,
                     msg->wParam, msg->lParam );

    if (msg->message == WM_PAINT)
    {
        /* send a WM_NCPAINT and WM_ERASEBKGND if the non-client area is still invalid */
        HRGN hrgn = CreateRectRgn_k( 0, 0, 0, 0 );
        GetUpdateRgn_k( msg->hwnd, hrgn, TRUE );
        DeleteObject_k( hrgn );
    }
    return retval;
}


/***********************************************************************
 *		DispatchMessageW (USER32.@) Process a message
 *
 * Process the message specified in the structure *_msg_.
 *
 * If the lpMsg parameter points to a WM_TIMER message and the
 * parameter of the WM_TIMER message is not NULL, the lParam parameter
 * points to the function that is called instead of the window
 * procedure. The function stored in lParam (timer callback) is protected
 * from causing page-faults.
 *
 * The message must be valid.
 *
 * RETURNS
 *
 *   DispatchMessage() returns the result of the window procedure invoked.
 *
 * CONFORMANCE
 *
 *   ECMA-234, Win32
 *
 */
LRESULT WINAPI DispatchMessageW_k( const MSG* msg )
{
    LRESULT retval;

      /* Process timer messages */
    if ((msg->message == WM_TIMER) || (msg->message == WM_SYSTIMER))
    {
        if (msg->lParam)
        {
            retval = CallWindowProcW_k( (WNDPROC)msg->lParam, msg->hwnd, msg->message, msg->wParam, GetTickCount_k() );
            return retval;
        }
    }
    if (!msg->hwnd) return 0;

    SPY_EnterMessage( SPY_DISPATCHMESSAGE, msg->hwnd, msg->message,
                      msg->wParam, msg->lParam );

    if (!WINPROC_call_window( msg->hwnd, msg->message, msg->wParam, msg->lParam,
                              &retval, TRUE, WMCHAR_MAP_DISPATCHMESSAGE ))
    {
        if (!IsWindow_k( msg->hwnd )) SetLastError_k( ERROR_INVALID_WINDOW_HANDLE );
        else SetLastError_k( ERROR_MESSAGE_SYNC_ONLY );
        retval = 0;
    }

    SPY_ExitMessage( SPY_RESULT_OK, msg->hwnd, msg->message, retval,
                     msg->wParam, msg->lParam );

    if (msg->message == WM_PAINT)
    {
        /* send a WM_NCPAINT and WM_ERASEBKGND if the non-client area is still invalid */
        HRGN hrgn = CreateRectRgn_k( 0, 0, 0, 0 );
        GetUpdateRgn_k( msg->hwnd, hrgn, TRUE );
        DeleteObject_k( hrgn );
    }
    return retval;
}


/***********************************************************************
 *		GetMessagePos (USER.119)
 *		GetMessagePos (USER32.@)
 *
 * The GetMessagePos() function returns a long value representing a
 * cursor position, in screen coordinates, when the last message
 * retrieved by the GetMessage() function occurs. The x-coordinate is
 * in the low-order word of the return value, the y-coordinate is in
 * the high-order word. The application can use the MAKEPOINT()
 * macro to obtain a POINT structure from the return value.
 *
 * For the current cursor position, use GetCursorPos().
 *
 * RETURNS
 *
 * Cursor position of last message on success, zero on failure.
 *
 * CONFORMANCE
 *
 * ECMA-234, Win32
 *
 */
DWORD WINAPI GetMessagePos_k(void)
{
    return get_user_thread_info()->GetMessagePosVal;
}


/***********************************************************************
 *		GetMessageTime (USER.120)
 *		GetMessageTime (USER32.@)
 *
 * GetMessageTime() returns the message time for the last message
 * retrieved by the function. The time is measured in milliseconds with
 * the same offset as GetTickCount().
 *
 * Since the tick count wraps, this is only useful for moderately short
 * relative time comparisons.
 *
 * RETURNS
 *
 * Time of last message on success, zero on failure.
 */
LONG WINAPI GetMessageTime_k(void)
{
    return get_user_thread_info()->GetMessageTimeVal;
}


/***********************************************************************
 *		GetMessageExtraInfo (USER.288)
 *		GetMessageExtraInfo (USER32.@)
 */
LPARAM WINAPI GetMessageExtraInfo_k(void)
{
    return get_user_thread_info()->GetMessageExtraInfoVal;
}


/***********************************************************************
 *		SetMessageExtraInfo (USER32.@)
 */
LPARAM WINAPI SetMessageExtraInfo_k(LPARAM lParam)
{
    struct user_thread_info *thread_info = get_user_thread_info();
    LONG old_value = thread_info->GetMessageExtraInfoVal;
    thread_info->GetMessageExtraInfoVal = lParam;
    return old_value;
}


/***********************************************************************
 *		WaitMessage (USER.112) Suspend thread pending messages
 *		WaitMessage (USER32.@) Suspend thread pending messages
 *
 * WaitMessage() suspends a thread until events appear in the thread's
 * queue.
 */
BOOL WINAPI WaitMessage_k(void)
{
    return (MsgWaitForMultipleObjectsEx_k( 0, NULL, INFINITE, QS_ALLINPUT, 0 ) != WAIT_FAILED);
}


/***********************************************************************
 *		MsgWaitForMultipleObjectsEx   (USER32.@)
 */
DWORD WINAPI MsgWaitForMultipleObjectsEx_k( DWORD count, CONST HANDLE *pHandles,
                                          DWORD timeout, DWORD mask, DWORD flags )
{
    HANDLE handles[MAXIMUM_WAIT_OBJECTS];
    DWORD i;

    if (count > MAXIMUM_WAIT_OBJECTS-1)
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return WAIT_FAILED;
    }
    set_queue_mask(currentThread()->queue, (flags & MWMO_INPUTAVAILABLE) ? mask : 0, mask, NULL, NULL, 0);

    /* add the queue to the handle list */
    for (i = 0; i < count; i++) handles[i] = pHandles[i];
    handles[count] = (HANDLE)currentThread()->queue->handle;

    flush_window_surfaces( TRUE );
    return wow_handlers.wait_message( count+1, handles, timeout, mask, flags );
}


/***********************************************************************
 *		MsgWaitForMultipleObjects (USER32.@)
 */
DWORD WINAPI MsgWaitForMultipleObjects_k( DWORD count, CONST HANDLE *handles,
                                        BOOL wait_all, DWORD timeout, DWORD mask )
{
    return MsgWaitForMultipleObjectsEx_k( count, handles, timeout, mask,
                                        wait_all ? MWMO_WAITALL : 0 );
}


/***********************************************************************
 *		WaitForInputIdle (USER32.@)
 */
DWORD WINAPI WaitForInputIdle_k( HANDLE hProcess, DWORD dwTimeOut )
{
    DWORD start_time, elapsed, ret;
    HANDLE handles[2];

    handles[0] = hProcess;
    handles[1] = (HANDLE)currentProcess()->idle_event;

    if (!handles[1]) return WAIT_FAILED;  /* no event to wait on */

    start_time = GetTickCount_k();
    elapsed = 0;

    TRACE("waiting for %p\n", handles[1] );
    do
    {
        ret = MsgWaitForMultipleObjects_k ( 2, handles, FALSE, dwTimeOut - elapsed, QS_SENDMESSAGE );
        switch (ret)
        {
        case WAIT_OBJECT_0:
            return 0;
        case WAIT_OBJECT_0+2:
            process_sent_messages();
            break;
        case WAIT_TIMEOUT:
        case WAIT_FAILED:
            TRACE("timeout or error\n");
            return ret;
        default:
            TRACE("finished\n");
            return 0;
        }
        if (dwTimeOut != INFINITE)
        {
            elapsed = GetTickCount_k() - start_time;
            if (elapsed > dwTimeOut)
                break;
        }
    }
    while (1);

    return WAIT_TIMEOUT;
}


/***********************************************************************
 *		RegisterWindowMessageA (USER32.@)
 *		RegisterWindowMessage (USER.118)
 */
UINT WINAPI RegisterWindowMessageA_k( LPCSTR str )
{
    UINT ret = GlobalAddAtomA_k(str);
    TRACE("%s, ret=%x\n", str, ret);
    return ret;
}


/***********************************************************************
 *		RegisterWindowMessageW (USER32.@)
 */
UINT WINAPI RegisterWindowMessageW_k( LPCWSTR str )
{
    UINT ret = GlobalAddAtomW_k(str);
    TRACE("%s ret=%x\n", debugstr_w(str), ret);
    return ret;
}

typedef struct BroadcastParm
{
    DWORD flags;
    LPDWORD recipients;
    UINT msg;
    WPARAM wp;
    LPARAM lp;
    DWORD success;
    HWINSTA winsta;
} BroadcastParm;

static BOOL CALLBACK bcast_childwindow( HWND hw, LPARAM lp )
{
    BroadcastParm *parm = (BroadcastParm*)lp;
    DWORD_PTR retval = 0;
    LRESULT lresult;

    if (parm->flags & BSF_IGNORECURRENTTASK && WIN_IsCurrentProcess(hw))
    {
        TRACE("Not telling myself %p\n", hw);
        return TRUE;
    }

    /* I don't know 100% for sure if this is what Windows does, but it fits the tests */
    if (parm->flags & BSF_QUERY)
    {
        TRACE("Telling window %p using SendMessageTimeout\n", hw);

        /* Not tested for conflicting flags */
        if (parm->flags & BSF_FORCEIFHUNG || parm->flags & BSF_NOHANG)
            lresult = SendMessageTimeoutW_k( hw, parm->msg, parm->wp, parm->lp, SMTO_ABORTIFHUNG, 2000, &retval );
        else if (parm->flags & BSF_NOTIMEOUTIFNOTHUNG)
            lresult = SendMessageTimeoutW_k( hw, parm->msg, parm->wp, parm->lp, SMTO_NOTIMEOUTIFNOTHUNG, 2000, &retval );
        else
            lresult = SendMessageTimeoutW_k( hw, parm->msg, parm->wp, parm->lp, SMTO_NORMAL, 2000, &retval );

        if (!lresult && GetLastError_k() == ERROR_TIMEOUT)
        {
            WARN("Timed out!\n");
            if (!(parm->flags & BSF_FORCEIFHUNG))
                goto fail;
        }
        if (retval == BROADCAST_QUERY_DENY)
            goto fail;

        return TRUE;

fail:
        parm->success = 0;
        return FALSE;
    }
    else if (parm->flags & BSF_POSTMESSAGE)
    {
        TRACE("Telling window %p using PostMessage\n", hw);
        PostMessageW_k( hw, parm->msg, parm->wp, parm->lp );
    }
    else
    {
        TRACE("Telling window %p using SendNotifyMessage\n", hw);
        SendNotifyMessageW_k( hw, parm->msg, parm->wp, parm->lp );
    }

    return TRUE;
}

static BOOL CALLBACK bcast_desktop( LPWSTR desktop, LPARAM lp )
{
    BOOL ret;
    HDESK hdesktop;
    BroadcastParm *parm = (BroadcastParm*)lp;

    TRACE("desktop: %s\n", debugstr_w( desktop ));

    hdesktop = open_winstation_desktop( parm->winsta, desktop, 0, FALSE, DESKTOP_ENUMERATE|DESKTOP_WRITEOBJECTS|STANDARD_RIGHTS_WRITE );
    if (!hdesktop)
    {
        FIXME("Could not open desktop %s\n", debugstr_w(desktop));
        return TRUE;
    }

    ret = EnumDesktopWindows_k( hdesktop, bcast_childwindow, lp );
    CloseDesktop_k(hdesktop);
    TRACE("-->%d\n", ret);
    return parm->success;
}

static BOOL CALLBACK bcast_winsta( LPWSTR winsta, LPARAM lp )
{
    BOOL ret;
    HWINSTA hwinsta = OpenWindowStationW_k( winsta, FALSE, WINSTA_ENUMDESKTOPS );
    TRACE("hwinsta: %p/%s/%08x\n", hwinsta, debugstr_w( winsta ), GetLastError_k());
    if (!hwinsta)
        return TRUE;
    ((BroadcastParm *)lp)->winsta = hwinsta;
    ret = EnumDesktopsW_k( hwinsta, bcast_desktop, lp );
    CloseWindowStation_k( hwinsta );
    TRACE("-->%d\n", ret);
    return ret;
}

/***********************************************************************
 *		BroadcastSystemMessageA (USER32.@)
 *		BroadcastSystemMessage  (USER32.@)
 */
LONG WINAPI BroadcastSystemMessageA_k( DWORD flags, LPDWORD recipients, UINT msg, WPARAM wp, LPARAM lp )
{
    return BroadcastSystemMessageExA_k( flags, recipients, msg, wp, lp, NULL );
}


/***********************************************************************
 *		BroadcastSystemMessageW (USER32.@)
 */
LONG WINAPI BroadcastSystemMessageW_k( DWORD flags, LPDWORD recipients, UINT msg, WPARAM wp, LPARAM lp )
{
    return BroadcastSystemMessageExW_k( flags, recipients, msg, wp, lp, NULL );
}

/***********************************************************************
 *              BroadcastSystemMessageExA (USER32.@)
 */
LONG WINAPI BroadcastSystemMessageExA_k( DWORD flags, LPDWORD recipients, UINT msg, WPARAM wp, LPARAM lp, PBSMINFO pinfo )
{
    map_wparam_AtoW( msg, &wp, WMCHAR_MAP_NOMAPPING );
    return BroadcastSystemMessageExW_k( flags, recipients, msg, wp, lp, NULL );
}


/***********************************************************************
 *              BroadcastSystemMessageExW (USER32.@)
 */
LONG WINAPI BroadcastSystemMessageExW_k( DWORD flags, LPDWORD recipients, UINT msg, WPARAM wp, LPARAM lp, PBSMINFO pinfo )
{
    BroadcastParm parm;
    DWORD recips = BSM_ALLCOMPONENTS;
    BOOL ret = TRUE;
    static const DWORD all_flags = ( BSF_QUERY | BSF_IGNORECURRENTTASK | BSF_FLUSHDISK | BSF_NOHANG
                                   | BSF_POSTMESSAGE | BSF_FORCEIFHUNG | BSF_NOTIMEOUTIFNOTHUNG
                                   | BSF_ALLOWSFW | BSF_SENDNOTIFYMESSAGE | BSF_RETURNHDESK | BSF_LUID );

    TRACE("Flags: %08x, recipients: %p(0x%x), msg: %04x, wparam: %08lx, lparam: %08lx\n", flags, recipients,
         (recipients ? *recipients : recips), msg, wp, lp);

    if (flags & ~all_flags)
    {
        SetLastError_k(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!recipients)
        recipients = &recips;

    if ( pinfo && flags & BSF_QUERY )
        FIXME("Not returning PBSMINFO information yet\n");

    parm.flags = flags;
    parm.recipients = recipients;
    parm.msg = msg;
    parm.wp = wp;
    parm.lp = lp;
    parm.success = TRUE;

    if (*recipients & BSM_ALLDESKTOPS || *recipients == BSM_ALLCOMPONENTS)
        ret = EnumWindowStationsW_k(bcast_winsta, (LONG_PTR)&parm);
    else if (*recipients & BSM_APPLICATIONS)
    {
        EnumWindows_k(bcast_childwindow, (LONG_PTR)&parm);
        ret = parm.success;
    }
    else
        FIXME("Recipients %08x not supported!\n", *recipients);

    return ret;
}

/***********************************************************************
 *		SetMessageQueue (USER32.@)
 */
BOOL WINAPI SetMessageQueue_k( INT size )
{
    /* now obsolete the message queue will be expanded dynamically as necessary */
    return TRUE;
}


/***********************************************************************
 *		MessageBeep (USER32.@)
 */
BOOL WINAPI MessageBeep_k( UINT i )
{
    BOOL active = TRUE;
    SystemParametersInfoA_k( SPI_GETBEEP, 0, &active, FALSE );
    if (active) USER_Driver->pBeep();
    return TRUE;
}


/***********************************************************************
 *		SetTimer (USER32.@)
 */
UINT_PTR WINAPI SetTimer_k( HWND hwnd, UINT_PTR id, UINT timeout, TIMERPROC proc )
{
    UINT_PTR ret=0;
    WNDPROC winproc = 0;
    DWORD error;

    if (proc) winproc = WINPROC_AllocProc( (WNDPROC)proc, FALSE );

    /* MSDN states that the minimum timeout should be USER_TIMER_MINIMUM (10.0 ms), but testing
     * indicates that the true minimum is closer to 15.6 ms. */
    timeout = min( max( 15, timeout ), USER_TIMER_MAXIMUM );
    error = set_win_timer(hwnd, id, WM_TIMER, timeout, (LPARAM)winproc, &ret);
    if (error)
        SetLastError_k(error);

    TRACE("Added %p %lx %p timeout %d\n", hwnd, id, winproc, timeout );
    return ret!=0;
}


/***********************************************************************
 *		SetSystemTimer (USER32.@)
 */
UINT_PTR WINAPI SetSystemTimer_k( HWND hwnd, UINT_PTR id, UINT timeout, TIMERPROC proc )
{
    DWORD ret;
    WNDPROC winproc = 0;
    DWORD error;

    if (proc) winproc = WINPROC_AllocProc( (WNDPROC)proc, FALSE );

    /* MSDN states that the minimum timeout should be USER_TIMER_MINIMUM (10.0 ms), but testing
     * indicates that the true minimum is closer to 15.6 ms. */
    timeout = min( max( 15, timeout ), USER_TIMER_MAXIMUM );
    error = set_win_timer(hwnd, id, WM_SYSTIMER, timeout, (LPARAM)winproc, &ret);
    if (error)
        SetLastError_k(error);
    TRACE("Added %p %lx %p timeout %d\n", hwnd, id, winproc, timeout );
    return ret!=0;
}


/***********************************************************************
 *		KillTimer (USER32.@)
 */
BOOL WINAPI KillTimer_k( HWND hwnd, UINT_PTR id )
{
    DWORD error = kill_win_timer(hwnd, WM_TIMER, id);
    if (error)
        SetLastError_k(error);
    return error==0;
}


/***********************************************************************
 *		KillSystemTimer (USER32.@)
 */
BOOL WINAPI KillSystemTimer_k( HWND hwnd, UINT_PTR id )
{
    DWORD error = kill_win_timer(hwnd, WM_SYSTIMER, id);
    if (error)
        SetLastError_k(error);
    return error==0;
}


/**********************************************************************
 *		IsGUIThread  (USER32.@)
 */
BOOL WINAPI IsGUIThread_k( BOOL convert )
{
    FIXME( "%u: stub\n", convert );
    return TRUE;
}


/**********************************************************************
 *		GetGUIThreadInfo  (USER32.@)
 */
BOOL WINAPI GetGUIThreadInfo_k( DWORD id, GUITHREADINFO *info )
{
    struct KThread* thread = currentThread();
    struct thread_input* input = thread->input;

    if (info->cbSize != sizeof(*info))
    {
        SetLastError_k( ERROR_INVALID_PARAMETER );
        return FALSE;
    }

    info->flags          = 0;
    info->hwndActive     = input->active;
    info->hwndFocus      = input->focus;
    info->hwndCapture    = input->capture;
    info->hwndMenuOwner  = input->menu_owner;
    info->hwndMoveSize   = input->move_size;
    info->hwndCaret      = input->caret;
    info->rcCaret.left   = input->caret_rect.left;
    info->rcCaret.top    = input->caret_rect.top;
    info->rcCaret.right  = input->caret_rect.right;
    info->rcCaret.bottom = input->caret_rect.bottom;
    if (info->hwndMenuOwner) info->flags |= GUI_INMENUMODE;
    if (info->hwndMoveSize) info->flags |= GUI_INMOVESIZE;
    if (info->hwndCaret) info->flags |= GUI_CARETBLINKING;

    return TRUE;
}


/******************************************************************
 *		IsHungAppWindow (USER32.@)
 *
 */
BOOL WINAPI IsHungAppWindow_k( HWND hWnd )
{
    struct KThread* thread = get_window_thread( hWnd );
    if (thread) {
        return is_queue_hung(thread->queue);
    }
    return FALSE;
}

/******************************************************************
 *      ChangeWindowMessageFilter (USER32.@)
 */
BOOL WINAPI ChangeWindowMessageFilter_k( UINT message, DWORD flag )
{
    FIXME( "%x %08x\n", message, flag );
    return TRUE;
}

#ifndef __WINECRT_H__

extern DWORD msvcrt_tls_index;

#define ERR(x)

struct __thread_data {
    DWORD                           tid;
    HANDLE                          handle;
    int                             thread_errno;
};

typedef struct __thread_data thread_data_t;

thread_data_t *msvcrt_get_thread_data(void);

#endif
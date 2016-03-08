#ifndef __SYS_RESOURCE_H__
#define __SYS_RESOURCE_H__

typedef unsigned int rlim_t;

#define 	RLIMIT_AS   7

int winecrt_getrlimit(int resource, struct rlimit *rlp);
#define getrlimit winecrt_getrlimit

int winecrt_setrlimit(int resource, const struct rlimit *rlp);
#define setrlimit winecrt_setrlimit

struct rlimit {
    rlim_t rlim_cur;   /* Soft limit */
    rlim_t rlim_max;   /* Hard limit (ceiling 
                          for rlim_cur) */
};

#endif
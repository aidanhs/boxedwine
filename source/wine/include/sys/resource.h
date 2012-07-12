#ifndef __SYS_RESOURCE_H__
#define __SYS_RESOURCE_H__

typedef unsigned int rlim_t;

#define 	RLIMIT_AS   7

DOSHAL int getrlimit(int resource, struct rlimit *rlp);
DOSHAL int setrlimit(int resource, const struct rlimit *rlp);

struct rlimit {
    rlim_t rlim_cur;   /* Soft limit */
    rlim_t rlim_max;   /* Hard limit (ceiling 
                          for rlim_cur) */
};

#endif
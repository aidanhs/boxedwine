typedef unsigned int rlim_t;

#define 	RLIMIT_AS   7

struct rlimit {
    rlim_t rlim_cur;   /* Soft limit */
    rlim_t rlim_max;   /* Hard limit (ceiling 
                          for rlim_cur) */
};

int hal_getrlimit(int resource, struct rlimit *rlp)
{
	return 0;
}

int hal_setrlimit(int resource, const struct rlimit *rlp)
{
	return 0;
}
#ifndef __GETOPT_H__
#define __GETOPT_H__

struct option
{
  char *name;
  int has_arg;
  int *flag;
  int val;
};

#define	no_argument			0
#define required_argument	1
#define optional_argument	2

#define optarg getoptArg()
WINECRT char* getoptArg();
WINECRT int getopt_long (int argc, char *const *argv, const char *shortopts, const struct option *longopts, int *longind);

#endif
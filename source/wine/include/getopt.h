#ifndef __GETOPT_H__
#define __GETOPT_H__

struct option
{
  char *name;
  int has_arg;
  int *flag;
  int val;
};

#define optarg getoptArg()
WINECRT char* getoptArg();
WINECRT int getopt_long (int argc, char *const *argv, const char *shortopts, const struct option *longopts, int *longind);

#endif
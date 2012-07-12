#ifndef __SYS_UN_H__
#define __SYS_UN_H__

typedef unsigned short int sa_family_t;

struct sockaddr_un
  {
    sa_family_t sun_family;
    char sun_path[108];		/* Path name.  */
  };

#endif
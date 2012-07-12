#ifndef __SETJMP_H__
#define __SETJMP_H__

typedef struct { int _sjb[9]; } sigjmp_buf[1];

DOSHAL int    sigsetjmp(sigjmp_buf, int);
DOSHAL void   siglongjmp(sigjmp_buf, int);

#endif
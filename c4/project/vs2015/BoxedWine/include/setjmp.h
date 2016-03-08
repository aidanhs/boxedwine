#ifndef __SETJMP_H__
#define __SETJMP_H__

typedef struct { int _sjb[9]; } sigjmp_buf[1];

int winecrt_sigsetjmp(sigjmp_buf, int);
#define sigsetjmp winecrt_sigsetjmp

void winecrt_siglongjmp(sigjmp_buf, int);
#define siglongjmp winecrt_siglongjmp

#endif
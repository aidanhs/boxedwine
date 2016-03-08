#ifndef __PTHREAD_H__
#define __PTHREAD_H__

#define SIG_UNBLOCK 1

int pthread_sigmask(int how, const unsigned int *set, unsigned int *oldset);

#endif
#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>

int isSocket(int fd);
int socketPoll(struct pollfd* fds, int count, int timeout);

int hal_poll(struct pollfd *fds, unsigned int count, int timeout )
{
	int socket = 0;
	int i;

	for (i=0;i<count;i++) {
		if (isSocket(fds[i].fd)) {
			if (i==0) {
				socket = 1;
			} else if (!socket) {
				fprintf(stderr, "Poll does not support mixing file and socket file descriptors\n");
				ExitProcess(1);
			}
		}
	}
	if (socket) {
		return socketPoll(fds, count, timeout);
	}
	return 0;
}
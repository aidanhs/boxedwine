#include "kpoll.h"
#include "kprocess.h"
#include "kobjectaccess.h"
#include "ksystem.h"
#include "kerror.h"
#include "kthread.h"
#include "kscheduler.h"

S32 kpoll(struct KThread* thread, struct KPollData* data, U32 count, U32 timeout) {
    S32 result = 0;
	U32 i;

	for (i=0;i<count;i++) {
		struct KFileDescriptor* fd = getFileDescriptor(thread->process, data->fd);
        data->revents = 0;
		if (!fd->kobject->access->isOpen(fd->kobject)) {
            data->revents = K_POLLHUP;
        } else {
			if ((data->events & K_POLLIN) != 0 && fd->kobject->access->isReadReady(fd->kobject)) {
                data->revents |= K_POLLIN;
            }
			if ((data->events & K_POLLOUT) != 0 && fd->kobject->access->isWriteReady(fd->kobject)) {
                data->revents |= K_POLLOUT;
            }
        }
        if (data->revents!=0) {
            result++;
        }
		data++;
    }
	removeTimer(&thread->timer);
    if (result>0) {		
		thread->waitStartTime = 0;
        return result;
	}
	if (timeout==0) {
		thread->waitStartTime = 0;
		return 0;
	}
	if (thread->waitStartTime) {
		if (getMilliesSinceStart() - thread->waitStartTime > timeout) {
			return 0;
		}
		thread->waitType = WAIT_FD;
		thread->timer.process = thread->process;
		thread->timer.thread = thread;
		thread->timer.millies = thread->waitStartTime+timeout;
		addTimer(&thread->timer);
	} else {
		thread->waitStartTime = getMilliesSinceStart();
		thread->waitType = WAIT_FD;

		thread->timer.process = thread->process;
		thread->timer.thread = thread;
		if (timeout<0xF0000000)
			timeout+=thread->waitStartTime;
		thread->timer.millies = timeout;
		addTimer(&thread->timer);
	}
	return -K_WAIT;
}
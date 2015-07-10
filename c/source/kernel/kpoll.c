#include "kpoll.h"
#include "kprocess.h"
#include "kobjectaccess.h"
#include "ksystem.h"
#include "kerror.h"
#include "kthread.h"
#include "kscheduler.h"
#include "log.h"

S32 kpoll(struct KThread* thread, struct KPollData* data, U32 count, U32 timeout) {
    S32 result = 0;
	U32 i;
	U32 interrupted = !thread->inSignal && thread->interrupted;
	struct KPollData* firstData=data;

	if (interrupted)
		thread->interrupted = 0;
	for (i=0;i<count;i++) {
		struct KFileDescriptor* fd = getFileDescriptor(thread->process, data->fd);
        data->revents = 0;
		if (fd) {
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
		}
		data++;
    }
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
			thread->waitStartTime = 0;
			return 0;
		}	
		if (interrupted) {			
			return -K_EINTR;
		}
		data = firstData;
		for (i=0;i<count;i++) {
			struct KFileDescriptor* fd = getFileDescriptor(thread->process, data->fd);
			fd->kobject->access->waitForEvents(fd->kobject, thread, data->events);
			data++;
		}
		thread->timer.process = thread->process;
		thread->timer.thread = thread;
		if (timeout<0xF0000000)
			timeout+=thread->waitStartTime;
		thread->timer.millies = timeout;
		addTimer(&thread->timer);
	} else {		
		if (interrupted) {			
			return -K_EINTR;
		}
		thread->waitStartTime = getMilliesSinceStart();

		data = firstData;
		for (i=0;i<count;i++) {
			struct KFileDescriptor* fd = getFileDescriptor(thread->process, data->fd);
			fd->kobject->access->waitForEvents(fd->kobject, thread, data->events);
			data++;
		}

		thread->timer.process = thread->process;
		thread->timer.thread = thread;
		if (timeout<0xF0000000)
			timeout+=thread->waitStartTime;
		thread->timer.millies = timeout;
		addTimer(&thread->timer);
	}	
	return -K_WAIT;
}
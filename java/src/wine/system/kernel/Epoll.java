package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelEpoll;

public class Epoll {
    static public final int EPOLL_CTL_ADD = 1;
    static public final int EPOLL_CTL_DEL = 2;
    static public final int EPOLL_CTL_MOD = 3;

//    struct epoll_event {
//        __u32 events;
//        __u64 data;
//    }
    // int epoll_create(int size)
    static public int epoll_create(WineThread thread, int size) {
        int result = thread.process.getNextFileDescriptor();
        KernelEpoll object = new KernelEpoll();
        FileDescriptor fd = new FileDescriptor(thread.process, result, object);
        thread.process.fileDescriptors.put(fd.handle, fd);
        return result;
    }

    // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
    static public int epoll_ctl(WineThread thread, int epfd, int op, int fd, int event) {
        FileDescriptor epollFD = thread.process.getFileDescriptor(epfd);
        FileDescriptor targetFD = thread.process.getFileDescriptor(fd);
        if (targetFD == null || epollFD == null) {
            return -Errno.EBADF;
        }
        if (fd==epfd || !(epollFD.object instanceof KernelEpoll)) {
            return -Errno.EINVAL;
        }
        KernelEpoll epoll = (KernelEpoll)epollFD.object;
        KernelEpoll.Data data =  epoll.fds.get(new Integer(fd));
        if (op==EPOLL_CTL_ADD && data!=null) {
            return -Errno.EEXIST;
        } else if ((op == EPOLL_CTL_DEL || op == EPOLL_CTL_MOD) && data == null) {
            return -Errno.ENOENT;
        }
        Memory memory = thread.process.memory;
        switch (op) {
            case EPOLL_CTL_ADD:
                epoll.fds.put(new Integer(fd), new KernelEpoll.Data(fd, memory.readd(event), memory.readq(event+4)));
                break;
            case EPOLL_CTL_DEL:
                epoll.fds.remove(new Integer(fd));
                break;
            case EPOLL_CTL_MOD:
                data.events = memory.readd(event);
                data.data = memory.readq(event+4);
                break;
            default:
                return -Errno.EINVAL;
        }
        return 0;
    }

    // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
    static public int epoll_wait(WineThread thread, int epfd, int events, int maxevents, int timeout) {
        FileDescriptor epollFD = thread.process.getFileDescriptor(epfd);
        if (epollFD == null) {
            return -Errno.EBADF;
        }
        if (!(epollFD.object instanceof KernelEpoll)) {
            return -Errno.EINVAL;
        }
        KernelEpoll epoll = (KernelEpoll)epollFD.object;

        Poll.pollfd[] data = new Poll.pollfd[epoll.fds.size()];

        int i=0;
        for (KernelEpoll.Data d : epoll.fds.values()) {
            data[i++] = new Poll.pollfd(d.fd, d.events);
        }
        Poll.internalPoll(thread, data, timeout);
        int result = 0;
        for (i=0;i<data.length && i<maxevents;i++) {
            KernelEpoll.Data d = epoll.fds.get(new Integer(data[i].fd));
            thread.process.memory.writed(events+i*12, data[i].revents);
            if (data[i].revents!=0)
                result=i+1;
            thread.process.memory.writeq(events + i * 12 + 4, d.data);
        }
        return result;
    }
}

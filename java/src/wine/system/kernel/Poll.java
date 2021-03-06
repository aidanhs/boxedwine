package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.io.DevTouch;
import wine.system.kernel.Process;
import wine.system.WineThread;
import wine.system.io.FileDescriptor;

public class Poll {
//    struct pollfd
//    {
//        int fd;
//        short events;
//        short revents;
//    };

    static public class pollfd {
        public pollfd(int fd, int events) {
            this.fd = fd;
            this.events = events;
        }
        public pollfd(Memory memory, int address) {
            this.address = address;
            fd = memory.readd(address); address+=4;
            events = memory.readw(address); address+=2;
            revents = memory.readw(address);
        }

        public void writeRevents(Memory memory) {
            memory.writew(address+6, revents);
        }
        int address;
        int fd;
        int events;
        int revents;
    }

    static public final int POLLIN      = 0x001;
    static public final int POLLPRI     = 0x002;
    static public final int POLLOUT     = 0x004;

    static public final int POLLRDNORM  = 0x040;
    static public final int POLLRDBAND  = 0x080;
    static public final int POLLWRNORM  = 0x100;
    static public final int POLLWRBAND  = 0x200;

    static public final int POLLERR     = 0x0008;
    static public final int POLLHUP     = 0x0010;
    static public final int POLLNVAL    = 0x0020;
    
    // int poll (struct pollfd *pfd, nfds_t nfd, int timeout)
    static public int poll(WineThread thread, int pfd, int fds, int timeout) {
        Process process = thread.process;
        Memory memory = process.memory;

        pollfd[] data = new pollfd[fds];
        for (int i=0;i<data.length;i++) {
            data[i] = new pollfd(memory, pfd+i*8);
        }
        int result = internalPoll(thread, data, timeout);
        for (int i=0;i<data.length;i++) {
            data[i].writeRevents(memory);
        }
        return result;
    }

    static public int internalPoll(WineThread thread, pollfd[] data, int timeout) {
        int result = 0;
        long startTime = System.nanoTime();
        if (timeout<0)
            timeout = Integer.MAX_VALUE;
        while (true) {
            result = 0;
            synchronized (FileDescriptor.lock) {
                for (int i = 0; i < data.length; i++) {
                    pollfd p = data[i];
                    FileDescriptor fd = thread.process.getFileDescriptor(p.fd);
                    p.revents = 0;
                    if (!fd.object.isOpen()) {
                        p.revents = POLLHUP;
                    } else {
                        if ((p.events & POLLIN) != 0 && fd.object.isReadReady()) {
                            p.revents |= POLLIN;
                        }
                        if ((p.events & POLLOUT) != 0 && fd.object.isWriteReady()) {
                            p.revents |= POLLOUT;
                        }
                    }
                    if (p.revents!=0) {
                        result++;
                    }
                }
                if (result>0)
                    break;
                if (timeout==0) {
                    break;
                }
                try {
                    long millis = timeout - (System.nanoTime()-startTime)/1000000; // nano to milli
                    if (millis>0)
                        FileDescriptor.lock.wait(millis);
                    else {
                        break;
                    }
                } catch (InterruptedException e) {
                    thread.interrupted();
                }
            }
            if (result>0)
                break;
        }
        return result;
    }
}

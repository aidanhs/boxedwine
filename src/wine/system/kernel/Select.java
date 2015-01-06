package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.io.FileDescriptor;
import wine.util.Log;

import java.util.Vector;

public class Select {
    // #define FD_SETSIZE 64
    //
    // typedef struct
    // {
    // unsigned int fd_count;
    // int fd_array[FD_SETSIZE]; /* an array of SOCKETs */
    // } fd_set;
    // int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
    static public int select(WineThread thread, int ndfs, int readfds, int writefds, int errorfds, int timeout) {
        if (ndfs>0) {
            Vector<Poll.pollfd> data = new Vector<Poll.pollfd>();
            int i=0;

            while (i<ndfs) {
                int readbits = 0;
                int writebits = 0;
                int errorbits = 0;

                if (readfds!=0) {
                    readbits = thread.process.memory.readb(readfds+i/8);
                }
                if (writefds!=0) {
                    writebits = thread.process.memory.readb(writefds + i / 8);
                }
                if (errorfds!=0) {
                    errorbits = thread.process.memory.readb(errorfds + i / 8);
                }
                for (int b = 0; b < 8 && i < ndfs; b++, i++) {
                    int mask = 1 << b;
                    boolean r = (readbits & mask)!=0;
                    boolean w = (writebits & mask)!=0;
                    boolean e = (errorbits & mask)!=0;
                    if (r || w || e) {
                        int events = 0;
                        if (r)
                            events |= Poll.POLLIN;
                        if (w)
                            events |= Poll.POLLHUP;
                        if (e)
                            events |= Poll.POLLERR;
                        Poll.pollfd pfd = new Poll.pollfd(i, events);
                        data.add(pfd);
                    }
                }
            }

            if (timeout==0)
                timeout = Integer.MAX_VALUE;
            else {
                Memory memory = thread.process.memory;
                timeout = (int)((memory.readd(timeout) & 0xFFFFFFFFl)*1000+(memory.readd(timeout+4) & 0xFFFFFFFFl)/1000);
            }

            Poll.pollfd[] d = new Poll.pollfd[data.size()];
            data.copyInto(d);
            int result = Poll.internalPoll(thread, d, timeout);

            for (i=0;i<ndfs;i+=8) {
                if (readfds != 0) {
                    thread.process.memory.writeb(readfds + i / 8, 0);
                }
                if (writefds != 0) {
                    thread.process.memory.writeb(writefds + i / 8, 0);
                }
                if (errorfds != 0) {
                    thread.process.memory.writeb(errorfds + i / 8, 0);
                }
            }
            if (result == 0)
                return 0;
            int count = 0;
            for (Poll.pollfd p : d) {
                boolean found = false;
                FileDescriptor fd = thread.process.getFileDescriptor(p.fd);

                if (readfds!=0 && (p.revents & Poll.POLLIN)!=0 || !fd.object.isOpen()) {
                    int v = thread.process.memory.readb(readfds+p.fd/8);
                    v |= 1 << (p.fd % 8);
                    thread.process.memory.writeb(readfds+p.fd/8, v);
                    found = true;
                }
                if (writefds!=0 && (p.revents & Poll.POLLHUP)!=0) {
                    int v = thread.process.memory.readb(writefds+p.fd/8);
                    v |= 1 << (p.fd % 8);
                    thread.process.memory.writeb(writefds+p.fd/8, v);
                    found = true;
                }
                if (errorfds!=0 && (p.revents & Poll.POLLERR)!=0) {
                    int v = thread.process.memory.readb(errorfds+p.fd/8);
                    v |= 1 << (p.fd % 8);
                    thread.process.memory.writeb(errorfds+p.fd/8, v);
                    found = true;
                }
                if (found) {
                    count++;
                }
            }
            return count;
        } else {
            if (timeout == 0) {
                while (true) {
                    try {
                        Thread.sleep(Long.MAX_VALUE);
                    } catch (InterruptedException e) {
                        thread.interrupted();
                    }
                }
            }else {
                Memory memory = thread.process.memory;
                long t = (memory.readd(timeout) & 0xFFFFFFFFl) * 1000 + (memory.readd(timeout + 4) & 0xFFFFFFFFl) / 1000;
                long start = System.currentTimeMillis();
                while (true) {
                    try {
                        Thread.sleep(t);
                        break;
                    } catch (InterruptedException e) {
                        thread.interrupted();
                        long now = System.currentTimeMillis();
                        if (now - start < t)
                            t = t - (now - start);
                        else
                            break;
                    }
                }
            }
        }
        return 0;
    }
}

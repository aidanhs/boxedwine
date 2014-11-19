package wine.builtin.libc;

import wine.system.WineThread;
import wine.system.io.FileDescriptor;

public class Ioctl {
    // int ioctl(int d, unsigned long request, ...);
    static public int ioctl(int d, int request, Syscall.SyscallGetter getter) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(d);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        return fd.object.ioctl(request, getter);
    }
}

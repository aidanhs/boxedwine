package wine.system.kernel;

import wine.system.WineThread;
import wine.system.io.FileDescriptor;

public class Ioctl {
    static public int ioctl(WineThread thread, int d, int request, Syscall.SyscallGetter getter) {
        FileDescriptor fd = thread.process.getFileDescriptor(d);
        if (fd==null) {
            return -Errno.EBADF;
        }
        return fd.object.ioctl(request, getter);
    }
}

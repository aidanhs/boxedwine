package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.io.FileDescriptor;
import wine.util.Log;

public class Uio {
//    struct iovec
//    {
//        void __user *iov_base;  /* BSD uses caddr_t (1003.1g requires void *) */
//        __kernel_size_t iov_len; /* Must be size_t (1003.1g) */
//    };

    // ssize_t writev(int fildes, const struct iovec *iov, int iovcnt)
    static public int writev(int fildes, int iov, int iovcnt) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        if (!fd.canWrite()) {
            thread.setErrno(Errno.EINVAL);
            return -1;
        }
        Memory memory = thread.process.memory;
        int len = 0;
        for (int i=0;i<iovcnt;i++) {
            int result = fd.object.write(memory.readd(iov+i*8), memory.readd(iov+i*8+4));
            if (result<0) {
                if (i>0) {
                    Log.warn("writev partial fail: TODO file pointer should not change");
                }
                return result;
            }
            len+=result;
        }
        return len;
    }
}

package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.util.Log;

public class Sock {
    static public int pipe(WineThread thread, int fildes) {
        return Socket.socketpair(thread, Socket.AF_UNIX, Socket.SOCK_STREAM, 0, fildes);
    }

    static public int pipe2(WineThread thread, int fildes, int flags) {
        int result = pipe(thread, fildes);
        if (result==0) {
            Memory memory = thread.process.memory;
            if ((flags & Io.O_CLOEXEC)!=0) {
                Fs.fcntl(thread, memory.readd(fildes), Io.F_SETFD, Io.FD_CLOEXEC);
                Fs.fcntl(thread, memory.readd(fildes + 4), Io.F_SETFD, Io.FD_CLOEXEC);
            }
            if ((flags & Io.O_NONBLOCK)!=0) {
                Fs.fcntl(thread, memory.readd(fildes), Io.F_SETFL, Io.O_NONBLOCK);
                Fs.fcntl(thread, memory.readd(fildes + 4), Io.F_SETFL, Io.O_NONBLOCK);
            }
            if ((flags & ~(Io.O_CLOEXEC|Io.O_NONBLOCK))!=0) {
                Log.warn("Unknow flags sent to pipe2: 0x" + Integer.toHexString(flags));
            }
        }
        return result;
    }
}

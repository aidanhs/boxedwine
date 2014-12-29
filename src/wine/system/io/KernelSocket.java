package wine.system.io;

import wine.system.WineThread;
import wine.util.Log;

abstract public class KernelSocket extends KernelObject {
    public int type;
    public int protocol;


    public KernelSocket(int type, int protocol) {
        this.type = type;
        this.protocol = protocol;
    }

    public static class Msg {
        public FileDescriptorMsg[] objects;
        public byte[][] data;
    }

    abstract public int accept(SocketAddress address);
    abstract public int bind(SocketAddress address);
    abstract public int connect(SocketAddress address);
    abstract public int getpeername(WineThread thread, int address, int len);
    abstract public int getsockopt(int level, int name, int value, int len);
    abstract public int listen();
    abstract public int recv(int buffer, int len, int flags);
    abstract public int recvmsg(FileDescriptorMsg[][] objects, byte[][] msgs, int flags);
    abstract public int send(int buffer, int len, int flags);
    abstract public int sendmsg(FileDescriptorMsg[] objects, byte[][] msgs, int flags);
    abstract public int setsockopt(int level, int name, int value, int len);
    abstract public int shutdown(int how);

    public int write(int buffer, int len) {
        return send(buffer, len, 0);
    }

    public int read(int buffer, int len) {
        return recv(buffer, len, 0);
    }

    public int getLock(FileLock lock) {
        Log.panic("F_GETLK not implemented for socket");
        return -1;
    }

    public int setLockW(FileLock lock) {
        Log.panic("F_SETLKW not implemented for socket");
        return -1;
    }

    public int setLock(FileLock lock) {
        Log.panic("F_SETLK not implemented for socket");
        return -1;
    }
}

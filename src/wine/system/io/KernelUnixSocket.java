package wine.system.io;

import wine.system.WineSystem;
import wine.system.kernel.Errno;
import wine.system.kernel.Io;
import wine.system.kernel.Socket;
import wine.emulation.Memory;
import wine.system.WineThread;
import wine.util.Log;

import java.util.LinkedList;

public class KernelUnixSocket extends KernelSocket {
    private SocketAddress address;
    private SocketAddress destAddress;
    private boolean listening;
    private KernelUnixSocket connection;
    final private LinkedList waitingConnections = new LinkedList();
    private boolean blocking = true;
    final private LinkedList data = new LinkedList();
    private int dataPos=0;
    final private LinkedList msgs = new LinkedList();
    private int sendBuffer = 1024*1024;
    private int recvBuffer = 1024*1024;
    private boolean inClosed=true;
    private boolean outClosed=true;
    private FSNode node;
    private int pid;

    public KernelUnixSocket(int type, int protocol, int pid) {
        super(type, protocol);
        this.pid = pid;
    }

    protected void onDelete() {
        if (connection!=null) {
            connection.connection = null;
            connection.outClosed = true;
            connection.inClosed = true;
            synchronized (connection.data) {
                connection.data.notifyAll();
            }
            synchronized (connection.msgs) {
                connection.msgs.notify();
            }
            connection=null;
        }
        outClosed = true;
        inClosed = true;
        if (node!=null) {
            FSNode.remove(node);
        }
        synchronized(FileDescriptor.lock) {
            FileDescriptor.lock.notifyAll();
        }
        synchronized (data) {
            data.notifyAll();
        }
        synchronized (msgs) {
            msgs.notifyAll();
        }
    }

    public void setNonBlocking() {
        blocking = false;
    }

    public boolean isNonBlocking() {
        return blocking;
    }

    public boolean isOpen() {
        return listening || (connection!=null && !inClosed); // :TODO: is inClosed correct here
    }

    public boolean isReadReady() {
        if (listening) {
            return waitingConnections.size()>0;
        } else {
            // in data.size==0 & inClosed then returning 0 from a read won't block and should generate a POLLIN
            return data.size()!=0 || msgs.size()!=0 || inClosed;
        }
    }

    public boolean isWriteReady() {
        return connection!=null;
    }

    public int accept(SocketAddress address) {
        WineThread thread = WineThread.getCurrent();
        if (!this.listening) {
            return -Errno.EINVAL;
        }
        KernelUnixSocket connection = null;
        synchronized (this.waitingConnections) {
            if (this.waitingConnections.size()==0) {
                if (this.blocking) {
                    while (this.waitingConnections.size()==0) {
                        try {this.waitingConnections.wait();} catch (InterruptedException e) {}
                    }
                } else {
                    return -Errno.EWOULDBLOCK;
                }
            }
            connection = (KernelUnixSocket)this.waitingConnections.removeFirst();
        }
        KernelUnixSocket result = new KernelUnixSocket(this.type, this.protocol, thread.process.id);
        connection.connection = result;
        connection.inClosed = false;
        connection.outClosed = false;
        result.connection = connection;
        result.inClosed = false;
        result.outClosed = false;
        synchronized (connection) {
            connection.notify();
        }
        FileDescriptor fd = result.createNewFileDescriptor(thread.process);
        fd.accessFlags = Io.O_RDWR;
        return fd.handle;
    }

    public int bind(SocketAddress address) {
        WineThread thread = WineThread.getCurrent();
        String path = address.name;
        if (path==null || path.length()==0) {
            return -Errno.ENOENT;
        }
        // :TODO: hack for xorg
        if (path.equals("port0")) {
            listening = true;
            return 0;
        }
        if (!path.startsWith("/"))
            path = thread.process.currentDirectory+"/"+path;
        FSNode node = FSNode.getNode(path, false);
        if (node.exists()) {
            return -Errno.EADDRINUSE;
        }
        this.node = UnixSocketFSNode.add(path, this);
        this.address = address;
        return 0;
    }

    public int connect(SocketAddress address) {
        WineThread thread = WineThread.getCurrent();
        if (destAddress!=null || connection!=null) {
            return -Errno.EISCONN;
        }
        this.destAddress = address;
        if (type == Socket.SOCK_DGRAM) {
            return 0;
        } else if (type == Socket.SOCK_STREAM) {
            FSNode node = FSNode.getNode(address.name, true);
            KernelUnixSocket dest = null;

            if (node!=null && node instanceof UnixSocketFSNode) {
                dest = ((UnixSocketFSNode)node).socket;
            }
            if (dest==null || !dest.listening) {
                return -Errno.ECONNREFUSED;
            }
            synchronized (dest.waitingConnections) {
                dest.waitingConnections.addLast(this);
            }
            synchronized (FileDescriptor.lock) {
                FileDescriptor.lock.notifyAll();
            }
            synchronized (this) {
                while (connection==null) {
                    try {this.wait(10000000);} catch (InterruptedException e) {}
                }
            }
            if (connection == null) {
                return -Errno.ECONNREFUSED;
            }
            return 0;
        } else {
            Log.panic("connect not implemented for type "+type);
            return -1;
        }
    }

    public int getpeername(WineThread thread, int address, int len) {
        if (connection == null)
            return -Errno.ENOTCONN;
        destAddress.write(address, thread.process.memory.readd(len));
        thread.process.memory.writed(len, destAddress.name.length()+2);
        return 0;
    }

    public int getsockopt(int level, int name, int value, int len) {
        if (level == Socket.SOL_SOCKET) {
            if (name == Socket.SO_RCVBUF) {
                if (len!=4)
                    Log.panic("getsockopt SO_RCVBUF expecting len of 4");
                WineThread.getCurrent().process.memory.writed(value, this.recvBuffer);
            } else if (name == Socket.SO_SNDBUF) {
                if (len != 4)
                    Log.panic("getsockopt SO_SNDBUF expecting len of 4");
                WineThread.getCurrent().process.memory.writed(value, this.recvBuffer);
            } else if (name == Socket.SO_ERROR) {
                if (len != 4)
                    Log.panic("getsockopt SO_ERROR expecting len of 4");
                WineThread.getCurrent().process.memory.writed(value, 0);
            } else if (name == Socket.SO_PEERCRED) {
                if (WineThread.getCurrent().process.memory.readd(len) != 12)
                    Log.panic("getsockopt SO_PEERCRED expecting len of 12");
                WineThread.getCurrent().process.memory.writed(value, connection.pid);
                WineThread.getCurrent().process.memory.writed(value+4, WineSystem.uid);
                WineThread.getCurrent().process.memory.writed(value+8, WineSystem.gid);
            } else {
                Log.panic("getsockopt name "+name+" not implemented");
            }
        } else {
            Log.panic("getsockopt level "+level+" not implemented");
        }
        return 0;
    }

    public int listen() {
        if (this.address==null) {
            return -Errno.EDESTADDRREQ;
        }
        if (this.connection!=null) {
            return -Errno.EINVAL;
        }
        this.listening = true;
        return 0;
    }

    public int recv(int buffer, int len, int flags) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        int result = 0;
        synchronized(data) {
            while (true) {
                if (data.size()==0) {
                    if (inClosed)
                        return 0; // end of file
                    if (blocking) {
                        try {
                            data.wait();
                        } catch (InterruptedException e) {
                        }
                    } else {
                        if (result > 0)
                            return result;
                        return -Errno.EWOULDBLOCK;
                    }
                }
                while (len>0 && data.size()>0) {
                    byte[] d = (byte[]) data.getFirst();
                    int read = Math.min(len, d.length - dataPos);
                    memory.memcpy(buffer+result, d, dataPos, read);
                    if (read+dataPos==d.length) {
                        data.removeFirst();
                        dataPos=0;
                    } else {
                        dataPos+=read;
                    }
                    len-=read;
                    result+=read;
                }
                if (result!=0)
                    return result;
            }
        }
    }

    public int recvmsg(FileDescriptorMsg[][] objects, byte[][] msgs, int flags) {
        WineThread thread = WineThread.getCurrent();
        synchronized (this.msgs) {
            while (true) {
                if (this.msgs.size()==0) {
                    if (blocking) {
                        try {this.msgs.wait();} catch(InterruptedException e) {}
                    } else {
                        return -Errno.EWOULDBLOCK;
                    }
                } else {
                    Msg msg = (Msg)this.msgs.getFirst();
                    int result = 0;

                    objects[0] = msg.objects;
                    for (int i=0;i<msgs.length && i<msg.data.length;i++) {
                        int len = Math.min(msg.data[i].length, msgs[i].length);
                        if (len != msgs[i].length && type==Socket.SOCK_STREAM) {
                            Log.panic("recvmsg not fully implemented, can't handle partial msg's with SOCK_STREAM");
                        }
                        System.arraycopy(msg.data[i], 0, msgs[i], 0, len);
                        result+=len;
                    }
                    if ((flags & Socket.MSG_PEEK)==0)
                        this.msgs.removeFirst();
                    return result;
                }
            }
        }
    }

    public int send(int buffer, int len, int flags) {
        WineThread thread = WineThread.getCurrent();
        KernelUnixSocket dest;

        if (type == Socket.SOCK_STREAM) {
            if (connection==null) {
                return -Errno.ENOTCONN;
            }
            dest = connection;
        } else if (type == Socket.SOCK_DGRAM) {
            if (destAddress==null) {
                return -Errno.EDESTADDRREQ;
            }
            FSNode node = FSNode.getNode(address.name, true);
            dest = null;
            if (node!=null && node instanceof UnixSocketFSNode) {
                dest = ((UnixSocketFSNode)node).socket;
            }
            if (dest == null) {
                return -Errno.EHOSTUNREACH;
            }
        } else {
            Log.panic("send type "+type+" not implemented");
            return -1;
        }
        if (flags!=0) {
            Log.panic("send flags="+flags+" no implemented");
        }
        byte[] bytes = new byte[len];
        thread.process.memory.memcpy(bytes, 0, len, buffer);
        synchronized (dest.data) {
            dest.data.addLast(bytes);
            dest.data.notify();
        }
        synchronized (FileDescriptor.lock) {
            FileDescriptor.lock.notifyAll();
        }
        return len;
    }

    public int write(String msg) {
        WineThread thread = WineThread.getCurrent();
        KernelUnixSocket dest;

        if (type == Socket.SOCK_STREAM) {
            if (connection==null) {
                return -Errno.ENOTCONN;
            }
            dest = connection;
        } else if (type == Socket.SOCK_DGRAM) {
            if (destAddress==null) {
                return -Errno.EDESTADDRREQ;
            }
            FSNode node = FSNode.getNode(address.name, true);
            dest = null;
            if (node!=null && node instanceof UnixSocketFSNode) {
                dest = ((UnixSocketFSNode)node).socket;
            }
            if (dest == null) {
                return -Errno.EHOSTUNREACH;
            }
        } else {
            Log.panic("send type "+type+" not implemented");
            return -1;
        }
        byte[] b = msg.getBytes();
        synchronized (dest.data) {
            dest.data.addLast(b);
            dest.data.notify();
        }
        synchronized (FileDescriptor.lock) {
            FileDescriptor.lock.notifyAll();
        }
        return b.length;
    }

    public int sendmsg(FileDescriptorMsg[] objects, byte[][] msgs, int flags) {
        WineThread thread = WineThread.getCurrent();
        KernelUnixSocket dest;

        if (type == Socket.SOCK_STREAM) {
            if (connection==null) {
                return -Errno.ENOTCONN;
            }
            dest = connection;
        } else if (type == Socket.SOCK_DGRAM) {
            if (destAddress==null) {
                return -Errno.EDESTADDRREQ;
            }
            FSNode node = FSNode.getNode(address.name, true);
            dest = null;
            if (node!=null && node instanceof UnixSocketFSNode) {
                dest = ((UnixSocketFSNode)node).socket;
            }
            if (dest == null) {
                return -Errno.EHOSTUNREACH;
            }
        } else {
            Log.panic("sendmsg type "+type+" not implemented");
            return -1;
        }


        if (flags!=0) {
            Log.panic("sendmsg flags="+flags+" no implemented");
        }
        Msg msg = new Msg();
        msg.objects = objects;
        msg.data = msgs;
        synchronized (dest.msgs) {
            dest.msgs.addLast(msg);
            dest.msgs.notify();
        }
        synchronized (FileDescriptor.lock) {
            FileDescriptor.lock.notifyAll();
        }
        int sent = 0;
        for (int i=0;i<msgs.length;i++) {
            sent+=msgs[i].length;
        }
        return sent;
    }

    public int setsockopt(int level, int name, int value, int len) {
        if (level == Socket.SOL_SOCKET) {
            switch (name) {
                case Socket.SO_RCVBUF:
                    if (len!=4)
                        Log.panic("setsockopt SO_RCVBUF expecting len of 4");
                    this.recvBuffer = WineThread.getCurrent().process.memory.readd(value);
                case Socket.SO_SNDBUF:
                    if (len != 4)
                        Log.panic("setsockopt SO_SNDBUF expecting len of 4");
                    this.sendBuffer = WineThread.getCurrent().process.memory.readd(value);
                case Socket.SO_PASSCRED:
                    break;
                case Socket.SO_ATTACH_FILTER:
                    break;
                default:
                    Log.warn("setsockopt name "+name+" not implemented");
            }
        } else {
            Log.panic("setsockopt level "+level+" not implemented");
        }
        return 0;
    }

    public int shutdown(int how) {
        WineThread thread = WineThread.getCurrent();
        if (type == Socket.SOCK_DGRAM) {
            Log.panic("shutdown on SOCK_DGRAM not implemented");
        }
        if (connection==null) {
            return -Errno.ENOTCONN;
        }
        if (how == Socket.SHUT_RD) {
            inClosed=true;
            connection.outClosed=true;
        } else if (how == Socket.SHUT_WR) {
            outClosed=true;
            connection.inClosed=true;
        } else if (how == Socket.SHUT_RDWR) {
            outClosed=true;
            inClosed=true;
            connection.outClosed=true;
            connection.inClosed=true;
        }
        synchronized(FileDescriptor.lock) {
            FileDescriptor.lock.notifyAll();
        }
        return 0;
    }

    public void pair(KernelUnixSocket s) {
        this.connection = s;
        s.connection = this;
        this.inClosed = false;
        this.outClosed = false;
        s.inClosed = false;
        s.outClosed = false;
    }

    public String name() {
        if (node!=null)
            return "socket: "+node.name();
        return "socket";
    }

    public boolean stat(KernelStat stat) {
        stat.mtime = System.currentTimeMillis();
        if (node!=null)
            stat.st_ino = node.id;
        else
            stat.st_ino = 0;
        stat.st_dev = 1;
        stat.st_mode = KernelStat.S_IFSOCK|KernelStat._S_IWRITE|KernelStat._S_IREAD;
        stat.st_size = 0;
        stat.st_blocks = 0;
        stat.st_rdev = 1;
        return true;
    }
}

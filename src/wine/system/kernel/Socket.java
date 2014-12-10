package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.io.*;
import wine.util.Log;

import java.net.InetAddress;
import java.net.UnknownHostException;

public class Socket {
    static public final int	SOCK_STREAM	= 1;
    static public final int SOCK_DGRAM  = 2;

    static public final int	AF_UNIX     = 1;
    static public final int	AF_INET     = 2;
    static public final int AF_INET6    = 10;

    static public final int	PF_UNIX     = 1;
    static public final int	PF_INET     = 2;

    static public final int	MSG_PEEK    = 0x2;

    static public final int SHUT_RD     = 0;
    static public final int SHUT_WR     = 1;
    static public final int SHUT_RDWR   = 2;

    static public final int	SO_ERROR    = 4;
    static public final int SO_SNDBUF   = 7;
    static public final int SO_RCVBUF   = 8;
    static public final int SO_PASSCRED = 16;

    static public final int SOL_SOCKET = 1;
    static public final int SCM_RIGHTS = 1;

    static private class MsgHdr {
        public MsgHdr() {
        }
        public MsgHdr(Memory memory, int address) {
            read(memory, address);
        }
        public void read(Memory memory, int address) {
            msg_name = memory.readd(address);address+=4;
            msg_namelen = memory.readd(address);address+=4;
            msg_iov = memory.readd(address);address+=4;
            msg_iovlen = memory.readd(address);address+=4;
            msg_control = memory.readd(address);address+=4;
            msg_controllen = memory.readd(address);address+=4;
            msg_flags = memory.readd(address);
        }
        public int msg_name;
        public int msg_namelen;
        public int msg_iov;
        public int msg_iovlen;
        public int msg_control;
        public int msg_controllen;
        public int msg_flags;
    }
    static private class CMsgHdr {
        static public final int SIZE=12;
        public CMsgHdr() {
        }
        public CMsgHdr(Memory memory, int address) {
            read(memory, address);
        }
        public void read(Memory memory, int address) {
            cmsg_len = memory.readd(address);address+=4;
            cmsg_level = memory.readd(address);address+=4;
            cmsg_type = memory.readd(address);
        }
        public void write(Memory memory, int address) {
            memory.writed(address, cmsg_len);address+=4;
            memory.writed(address, cmsg_level);address+=4;
            memory.writed(address, cmsg_type);
        }
        public int cmsg_len;
        public int cmsg_level;
        public int cmsg_type;
    }

//    struct msghdr {
//        void    *       msg_name;       /* Socket name                  */
//        int             msg_namelen;    /* Length of name               */
//        struct iovec *  msg_iov;        /* Data blocks                  */
//        __kernel_size_t msg_iovlen;     /* Number of blocks             */
//        void    *       msg_control;    /* Per protocol magic (eg BSD file descriptor passing) */
//        __kernel_size_t msg_controllen; /* Length of cmsg list */
//        unsigned int    msg_flags;
//    };

//    struct cmsghdr {
//        __kernel_size_t cmsg_len;       /* data byte count, including hdr */
//        int             cmsg_level;     /* originating protocol */
//        int             cmsg_type;      /* protocol-specific type */
//    };

//
//    struct sockaddr {
//        unsigned char sa_len;			/* total length */
//        unsigned char sa_family;		/* address family */
//        char	sa_data[14];		/* actually longer; address value */
//    };
//
//    typedef int socklen_t;

    
    static public int accept(WineThread thread, int socket, int address, int len) {
        Process process = thread.process;
        FileDescriptor fd = process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (fd.getSocket()==null) {
            return -Errno.ENOTSOCK;
        }
        SocketAddress sa = new SocketAddress();
        int result = fd.getSocket().accept(sa);
        sa.write(address, len);
        return result;
    }

    static public int bind(WineThread thread, int socket, int address, int len) {
        Process process = thread.process;
        FileDescriptor fd = process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (fd.getSocket()==null) {
            return -Errno.ENOTSOCK;
        }
        return fd.getSocket().bind(SocketAddress.parse(address, len));
    }

    static public int connect(WineThread thread, int socket, int address, int len) {
        Process process = thread.process;
        FileDescriptor fd = process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (fd.getSocket()==null) {
            return -Errno.ENOTSOCK;
        }
        return fd.getSocket().connect(SocketAddress.parse(address, len));
    }

    static public void freeaddrinfo(int ai) {
        Log.warn("freeaddrinfo not implemented");
    }

    static public int getaddrinfo(int nodename, int servname, int hints, int res) {
        Log.panic("getaddrinfo not implemented");
        return -1;
    }

//    struct hostent
//    {
//        char *h_name;                 /* Official name of host.  */
//        char **h_aliases;             /* Alias list.  */
//        int h_addrtype;               /* Host address type.  */
//        int h_length;                 /* Length of address.  */
//        char **h_addr_list;           /* List of addresses from name server.  */
//    };

    static public final int HOST_NOT_FOUND = 1;

    static public int gethostbyname_r(WineThread thread, int pName, int ret, int buf, int buflen, int result, int errno) {
        Memory memory = thread.process.memory;
        String name = memory.readCString(pName);
        try {
            InetAddress[] addresses = java.net.InetAddress.getAllByName(name);
            if (addresses==null || addresses.length==0) {
                if (errno!=0)
                    memory.writed(errno, HOST_NOT_FOUND);
                if (result!=0)
                    memory.writed(result, 0);
                return 0;
            }
            int pos=(addresses.length+1)*8; // h_aliases and h_addr_list
            if (pos>=buflen) {
                return Errno.ERANGE;
            }
            memory.writed(buf+addresses.length*4, 0); // h_aliases is null terminated
            int addrPos = buf+addresses.length*4+4;
            memory.writed(addrPos+addresses.length*4, 0); // h_addr_list is null terminated
            memory.writed(ret+8, AF_INET);
            memory.writed(ret+12, 4);
            for (int i=0;i<addresses.length;i++) {
                String hostName = addresses[i].getHostName();
                byte[] hostAddr = addresses[i].getAddress();
                if (pos+hostName.length()+1>=buflen) {
                    return Errno.ERANGE;
                }
                memory.writeCString(buf+pos, hostName);
                if (i==0) {
                    memory.writed(ret, pos);
                }
                memory.writed(buf+i*4, pos);
                memory.memcpy(addrPos+i*4, hostAddr, 0, 4);
            }
            memory.writed(result, ret);
            return 0;
        } catch (UnknownHostException e) {
            if (errno!=0)
                memory.writed(errno, HOST_NOT_FOUND);
            if (result!=0)
                memory.writed(result, 0);
            return 0;
        }
    }

    static public int getnameinfo(int sa, int salen, int node, int nodelen, int service, int servicelen, int flags) {
        Log.panic("getnameinfo not implemented");
        return -1;
    }

    static public int getsockname(int socket, int address, int address_len) {
        Log.warn("getsockname not implemented");
        return -Errno.EOPNOTSUPP;
    }

    static public int getsockopt(WineThread thread, int socket, int level, int name, int value, int len) {
        Process process = thread.process;
        FileDescriptor fd = process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (fd.getSocket()==null) {
            return -Errno.ENOTSOCK;
        }
        return fd.getSocket().getsockopt(level, name, value, len);
    }

    static public int listen(WineThread thread, int socket, int backlog) {
        Process process = thread.process;
        FileDescriptor fd = process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (fd.getSocket()==null) {
            return -Errno.ENOTSOCK;
        }
        return fd.getSocket().listen();
    }

    static public int recv(WineThread thread, int socket, int buffer, int len, int flags) {
        Process process = thread.process;
        FileDescriptor fd = process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (fd.getSocket()==null) {
            return -Errno.ENOTSOCK;
        }
        return fd.getSocket().recv(buffer, len, flags);
    }

    static public int recvmsg(WineThread thread, int socket, int message, int flags) {
        Process process = thread.process;
        Memory memory = process.memory;

        FileDescriptor fd = process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        KernelSocket s = fd.getSocket();
        if (s==null) {
            return -Errno.ENOTSOCK;
        }
        MsgHdr msghdr = new MsgHdr(memory, message);
        int iov = msghdr.msg_iov;
        byte[][] msgs = new byte[msghdr.msg_iovlen][];
        for (int i=0;i<msghdr.msg_iovlen;i++) {
            int l = process.memory.readd(iov+8*i+4);
            msgs[i] = new byte[l];
        }
        FileDescriptorMsg[][] objects = new FileDescriptorMsg[1][];
        int result = s.recvmsg(objects, msgs, flags);
        if (msghdr.msg_control!=0) {
            if (objects[0]==null) {
                process.memory.writed(message + 20, 0);
            } else {
                for (int i = 0; i < objects[0].length; i++) {
                    CMsgHdr cmsg = new CMsgHdr();
                    cmsg.cmsg_level = SOL_SOCKET;
                    cmsg.cmsg_type = SCM_RIGHTS;
                    cmsg.cmsg_len = CMsgHdr.SIZE + 4;
                    cmsg.write(memory, msghdr.msg_control);
                    process.memory.writed(msghdr.msg_control + i * (CMsgHdr.SIZE + 4) + CMsgHdr.SIZE, objects[0][i].createNewFileDescriptor(process).handle);
                }
                // msg_controllen
                process.memory.writed(message + 20, objects[0].length * (CMsgHdr.SIZE + 4));
            }
        }
        for (int i=0;i<msghdr.msg_iovlen;i++) {
            int p = process.memory.readd(iov+8*i);
            int len = process.memory.readd(iov+8*i+4);
            process.memory.memcpy(p, msgs[i], 0, msgs[i].length);
        }
        return result;
    }

    static public int send(WineThread thread, int socket, int buffer, int len, int flags) {
        Process process = thread.process;
        FileDescriptor fd = process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (fd.getSocket()==null) {
            return -Errno.ENOTSOCK;
        }
        return fd.getSocket().send(buffer, len, flags);
    }

    static public int setsockopt(WineThread thread, int socket, int level, int name, int value, int len) {
        Process process = thread.process;
        FileDescriptor fd = process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (fd.getSocket()==null) {
            return -Errno.ENOTSOCK;
        }
        return fd.getSocket().setsockopt(level, name, value, len);
    }

    static public int sendmsg(WineThread thread, int socket, int message, int flags) {
        Process process = thread.process;
        Memory memory = process.memory;

        FileDescriptor fd = process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        KernelSocket s = fd.getSocket();
        if (s==null) {
            return -Errno.ENOTSOCK;
        }
        MsgHdr msghdr = new MsgHdr(memory, message);
        FileDescriptorMsg[] objects = null;
        if (msghdr.msg_control!=0) {
            CMsgHdr cmsg = new CMsgHdr(memory, msghdr.msg_control);
            if (cmsg.cmsg_level != SOL_SOCKET) {
                Log.panic("sendmsg control level "+cmsg.cmsg_level+" not implemented");
            } else if (cmsg.cmsg_type != SCM_RIGHTS) {
                Log.panic("sendmsg control type "+cmsg.cmsg_level+" not implemented");
            } else if ((cmsg.cmsg_len & 3) != 0) {
                Log.panic("sendmsg control len "+cmsg.cmsg_len+" not implemented");
            }
            int count = msghdr.msg_controllen/(CMsgHdr.SIZE+4);
            objects = new FileDescriptorMsg[count];
            for (int i=0;i<objects.length;i++) {
                FileDescriptor f = process.getFileDescriptor(process.memory.readd(msghdr.msg_control+(CMsgHdr.SIZE+4)*i+CMsgHdr.SIZE));
                if (f == null) {
                    Log.panic("tried to sendmsg a bad file descriptor");
                } else {
                    objects[i] = new FileDescriptorMsg(f);
                }
            }
        }
        int iov = msghdr.msg_iov;
        byte[][] msgs = new byte[msghdr.msg_iovlen][];
        for (int i=0;i<msghdr.msg_iovlen;i++) {
            int p = process.memory.readd(iov+8*i);
            int l = process.memory.readd(iov+8*i+4);
            byte[] msg = new byte[l];
            process.memory.memcpy(msg, 0, msg.length, p);
            msgs[i] = msg;
        }
        return s.sendmsg(objects, msgs, flags);
    }

    static public int socket(WineThread thread, int domain, int type, int protocol) {
        Process process = thread.process;
        if (domain==AF_UNIX) {
            return new KernelUnixSocket(type, protocol).createNewFileDescriptor(process).handle;
        } else if (domain==AF_INET) {
            //return new KernelInetSocket(type, protocol).createNewFileDescriptor(process).handle;
        }
        Log.warn("socket domain "+domain+" not implemented");
        return -1;
    }

    static public int socketpair(WineThread thread, int af, int type, int protocol, int socks) {
        if (af!=AF_UNIX) {
            Log.panic("socketpair with adress family "+af+" not implemented");
        }
        if (type!=SOCK_DGRAM && type!=SOCK_STREAM) {
            Log.panic("socketpair with type "+type+" not implemented");
        }
        Process process = thread.process;
        KernelUnixSocket s1 = new KernelUnixSocket(type, protocol);
        KernelUnixSocket s2 = new KernelUnixSocket(type, protocol);
        s1.pair(s2);
        FileDescriptor fd1 = s1.createNewFileDescriptor(process);
        FileDescriptor fd2 = s2.createNewFileDescriptor(process);
        fd1.accessFlags = Io.O_RDWR;
        fd2.accessFlags = Io.O_RDWR;
        process.memory.writed(socks, fd1.handle);
        process.memory.writed(socks+4, fd2.handle);
        return 0;
    }

    static public int shutdown(WineThread thread, int socket, int how) {
        FileDescriptor fd = thread.process.getFileDescriptor(socket);
        if (fd==null) {
            return -Errno.EBADF;
        }
        if (fd.getSocket()==null) {
            return -Errno.ENOTSOCK;
        }
        return fd.getSocket().shutdown(how);
    }
}

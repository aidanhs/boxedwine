package wine.system.io;

import wine.system.kernel.Errno;
import wine.system.kernel.*;
import wine.emulation.Memory;
import wine.system.kernel.Process;
import wine.system.WineThread;
import wine.util.Log;

public class KernelFile extends KernelObject {
    final public FSNode node;
    public FSNodeAccess io;
    private long seekPos = 0;
    public FSNode[] getdentsData;

    public KernelFile(FSNode fs) {
        this.node = fs;
    }

    public FileDescriptor createNewFileDescriptor(Process process) {
        FileDescriptor fd = new FileDescriptor(process, process.getNextFileDescriptor(), this);
        process.fileDescriptors.put(fd.handle, fd);
        return fd;
    }

    protected void onDelete() {
        if (io!=null) {
            io.close();
            io = null;
        }
    }

    public void setNonBlocking() {
    }

    public boolean isNonBlocking() {
        return false;
    }

    public void setAsync(Process process, boolean remove) {
        node.setAsync(process, remove);
    }

    public boolean isAsync(Process process) {
        return node.isAsync(process);
    }

    public int getLock(FileLock lock) {
        synchronized (node.locks) {
            for (int i = 0; i < node.locks.size(); i++) {
                FileLock f = (FileLock) node.locks.elementAt(i);
                if (f.doesOverlap(lock)) {
                    if (f.l_type == Io.F_WRLCK || lock.l_type == Io.F_WRLCK) {
                        lock.copy(f);
                        return 0;
                    }
                }
            }
            lock.l_type = Io.F_UNLCK;
            return 0;
        }
    }

    public int setLockW(WineThread thread, FileLock lock) {
        while (setLock(lock)==-1) {
            synchronized (node.locks) {
                try {node.locks.wait();} catch (InterruptedException e) {thread.interrupted();}
            }
        }
        return 0;
    }

    public int setLock(FileLock lock) {
        WineThread thread = WineThread.getCurrent();
        wine.system.kernel.Process process = thread.process;

        synchronized (node.locks) {
            if (lock.l_type==Io.F_UNLCK) {
                for (int i = 0; i < node.locks.size(); i++) {
                    FileLock f = (FileLock) node.locks.elementAt(i);
                    if (f.l_pid==process.id) {
                        if (f.doesOverlap(lock)) {
                            // are we cutting a hole in an existing lock
                            if (f.l_start<lock.l_start && f.l_start+f.l_len>lock.l_start+lock.l_len) {
                                long len = lock.l_start-f.l_start;
                                FileLock rightLock = new FileLock();
                                rightLock.copy(f);
                                rightLock.l_start=lock.l_start+lock.l_len;
                                f.l_len = len;
                                node.locks.add(rightLock);
                            }
                            // are we trimming the right of an existing lock
                            else if (f.l_start<lock.l_start) {
                                f.l_len = lock.l_start-f.l_start;
                            }
                            // are we trimming the left of an existing lock
                            else if (f.l_start+f.l_len>lock.l_start+lock.l_len) {
                                f.l_start=lock.l_start+lock.l_len;
                            }
                            // are we removing an existing lock
                            else if (f.l_start==lock.l_start && f.l_len==lock.l_len) {
                                node.locks.removeElementAt(i);
                                i--;
                            }
                            // this shouldn't happen
                            else {
                                Log.panic("Logic error when removing lock");
                            }
                        }
                    }
                }
            }
            FileLock leftLock = null;
            FileLock rightLock = null;
            for (int i = 0; i < node.locks.size(); i++) {
                FileLock f = (FileLock) node.locks.elementAt(i);
                if (f.doesOverlap(lock)) {
                    if ((f.l_type == Io.F_WRLCK || lock.l_type == Io.F_WRLCK) && f.l_pid!=process.id) {
                        return -1;
                    }
                    if (f.l_pid==process.id) {
                        if (f.l_start==lock.l_start && f.l_len==lock.l_len) {
                            f.copy(lock);
                            return 0;
                        } else if (f.l_start>=lock.l_start && f.l_start+f.l_len<=lock.l_start+lock.l_len) {
                            // this lock will be replaced entirely
                            node.locks.removeElementAt(i);
                            i--;
                        } else {
                            // both will be set this time if the new lock is in the middle of an existing one
                            if (f.l_start<=lock.l_start) {
                                if (leftLock!=null)
                                    Log.panic("Oops");
                                leftLock = f;
                            }
                            if (f.l_start+f.l_len>=lock.l_start+lock.l_len) {
                                if (rightLock!=null)
                                    Log.panic("Oops");
                                rightLock = f;
                            }
                        }
                    }
                }
            }
            if (leftLock==null && rightLock==null) {
                // the new lock does not overlap any other locks in this process
                node.locks.add(lock);
            } else if (leftLock!=null && rightLock!=null) {
                if (leftLock==rightLock) {
                    // the new lock is in the middle of an existing one
                } else {
                    // the new lock bridges (fills in the gap) between two other locks
                    if (leftLock.l_type==lock.l_type && rightLock.l_type==lock.l_type) {
                        leftLock.l_len = rightLock.l_start + rightLock.l_len - leftLock.l_start;
                        node.locks.remove(rightLock);
                    } else if (leftLock.l_type==lock.l_type) {
                        leftLock.l_len=lock.l_start+lock.l_len-leftLock.l_start;
                        long newStart = lock.l_start+lock.l_len;
                        rightLock.l_len-=newStart-rightLock.l_start;
                        rightLock.l_start=newStart;
                    } else if (rightLock.l_type==lock.l_type) {
                        leftLock.l_len=lock.l_start-leftLock.l_start;
                        rightLock.l_len+=rightLock.l_start-lock.l_start;
                        rightLock.l_start=lock.l_start;
                    } else {
                        leftLock.l_len=lock.l_start-leftLock.l_start;

                        long newStart = lock.l_start+lock.l_len;
                        rightLock.l_len-=newStart-rightLock.l_start;
                        rightLock.l_start=newStart;
                        node.locks.add(lock);
                    }
                }
            } else if (rightLock!=null) {
                // the new lock overlaps with an existing lock on the right
                if (rightLock.l_type==lock.l_type) {
                    rightLock.l_len+=rightLock.l_start-lock.l_start;
                    rightLock.l_start=lock.l_start;
                } else {
                    long newStart = lock.l_start+lock.l_len;
                    rightLock.l_len-=newStart-rightLock.l_start;
                    rightLock.l_start=newStart;
                    node.locks.add(lock);
                }
            } else if (leftLock!=null) {
                // the new lock overlaps with an existing lock on the left
                if (leftLock.l_type==lock.l_type) {
                    leftLock.l_len=lock.l_start+lock.l_len-leftLock.l_start;
                } else {
                    leftLock.l_len=lock.l_start-leftLock.l_start;
                    node.locks.add(lock);
                }
            } else {
                Log.panic("Oops");
            }
        }
        return 0;
    }

    public boolean isOpen() {
        return io!=null;
    }

    public boolean isReadReady() {
        return io.isReadReady();
    }

    public boolean isWriteReady() {
        return io.isWriteReady();
    }

    public int chmod(int mode) {
//        int m = mode >> 6
//        boolean owner_read = (m & 4)!=0;
//        boolean owner_write = (m & 2)!=0;
//        boolean owner_exe = (m & 1)!=0;
//
//        node.file.setExecutable(owner_exe);
//        node.file.setReadable(owner_read);
//        node.file.setWritable(owner_write);
        return 0;
    }

    synchronized public int write(int buffer, int len) {
        WineThread thread = WineThread.getCurrent();
        if (io==null) {
            return -Errno.EBADF;
        }
        byte[] b = new byte[len];
        thread.process.memory.memcpy(b, 0, len, buffer);
        if (!io.write(b)) {
            return -Errno.EIO;
        }
        return len;
    }

    synchronized public long getFilePointer() {
        if (io==null) {
            return seekPos;
        }
        return io.getFilePointer();
    }

    synchronized public int pread(int buffer, int len, long offset) {
        WineThread thread = WineThread.getCurrent();
        if (io==null) {
            return -Errno.EBADF;
        }
        byte[] b = new byte[len];
        long pos = io.getFilePointer();
        if (!io.seek(offset)) {
            return -Errno.EIO;
        }
        int read = io.read(b);
        thread.process.memory.memcpy(buffer, b, 0, read);
        if (pos>=0) {
            io.seek(pos);
        }
        return read;
    }
    synchronized public int pwrite(int buffer, int len, long offset) {
        WineThread thread = WineThread.getCurrent();
        if (io==null) {
            return -Errno.EBADF;
        }
        byte[] b = new byte[len];
        thread.process.memory.memcpy(b, 0, len, buffer);
        long pos = io.getFilePointer();
        if (!io.seek(offset)) {
            return -Errno.EIO;
        }
        if (!io.write(b)) {
            return -Errno.EIO;
        }
        if (pos>=0) {
            io.seek(pos);
        }
        return len;
    }

    synchronized public int write(String msg) {
        WineThread thread = WineThread.getCurrent();
        if (io==null) {
            return -Errno.EBADF;
        }
        byte[] b = msg.getBytes();
        if (!io.write(b)) {
            return -Errno.EIO;
        }
        return b.length+1;
    }

    synchronized public int read(int buffer, int len) {
        WineThread thread = WineThread.getCurrent();
        if (node.isDirectory()) {
            return -Errno.EISDIR;
        }
        if (io==null) {
            return -Errno.EBADF;
        }
        byte[] b = new byte[len];
        int result = io.read(b);
        if (result<0) {
            if (io.getFilePointer()==node.length()) {
                return 0;
            }
            return -Errno.EIO;
        }
        thread.process.memory.memcpy(buffer, b, 0, result);
        return result;
    }

    public boolean open(String mode) {
        if (io!=null) {
            Log.panic("Tried to open file more than once: "+node.localPath);
            return false;
        }
        io = node.open(mode);
        return io!=null;
    }

    synchronized public boolean setLength(long length) {
        return io.setLength(length);
    }

    synchronized public long seek(int whence, long offset) {
        if (io==null) {
            if (whence == Io.SEEK_SET) {
                seekPos = offset;
            } else if (whence == Io.SEEK_CUR) {
                seekPos += offset;
            } else {
                return -Errno.EINVAL;
            }
            return seekPos;
        }
        long pos = 0;
        if (whence == Io.SEEK_SET) {
            pos = offset;
        } else if (whence == Io.SEEK_CUR) {
            pos = offset + io.getFilePointer();
        } else if (whence == Io.SEEK_END) {
            pos = offset + node.length();
        } else {
            return -Errno.EINVAL;
        }
        io.seek(pos);
        return io.getFilePointer();
    }

    public String name() {
        return node.name();
    }

    public boolean stat(KernelStat stat) {
        if (!node.exists())
            return false;
        stat.mtime = node.lastModified();
        stat.st_ino = node.id;
        stat.st_dev = 1;
        stat.st_mode = node.getMode();
        stat.st_size = node.length();
        stat.st_blksize = 512;
        stat.st_blocks = (node.length() + 511) / 512;
        stat.st_rdev = node.rdev;
        return true;
    }

    public int ioctl(WineThread thread, int request, Syscall.SyscallGetter getter) {
        if (io!=null)
            return io.ioctl(thread, request, getter);
        return -Errno.ENODEV;
    }

    public int map(Process process, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        if (io==null) {
            Log.panic("Tried to map: "+name());
            return -1;
        }
        return io.map(process, fd, off, address, len, fixed, read, exec, write, shared);
    }
}

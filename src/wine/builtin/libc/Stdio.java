package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.system.io.FSNode;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;
import wine.util.Log;

// I don't think wine.exe or wineserver.exe really need buffered i/o, so FILE* will just be a File Descriptor id
public class Stdio {
    static public final int SEEK_SET              = 0;
    static public final int SEEK_CUR              = 1;
    static public final int SEEK_END              = 2;

    // int fclose(FILE *stream)
    static public int fclose(int stream) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;

        FileDescriptor fd = thread.process.getFileDescriptor(stream);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        fd.close();
        return 0;
    }

    // FILE *fdopen(int fildes, const char *mode)
    static public int fdopen64(int fildes, int mode) {
        return fdopen(fildes, mode);
    }

    static public int fdopen(int fildes, int mode) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;

        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        // The file descriptor is not dup'ed, and will be closed when the stream created by fdopen() is closed.
        // fd.incrementRef();
        return fildes;
    }

    // int feof(FILE *stream)
    static public int feof(int stream) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        FileDescriptor fd = thread.process.getFileDescriptor(stream);
        KernelFile file = null;
        if (fd!=null)
            file = fd.getFile();
        if (file==null) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        if (file.getFilePointer()==file.node.length()) {
            return 1;
        }
        return 0;
    }

    // int fflush ( FILE * stream )
    static public int fflush(int stream) {
        Log.warn("fflush not implemented");
        return 0;
    }

    // char *fgets(char * s, int n, FILE * stream)
    static public int fgets(int s, int n, int stream) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        FileDescriptor fd = thread.process.getFileDescriptor(stream);

        if (fd==null || !fd.canRead()) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        for (int i=0;i<n;i++) {
            if (fd.object.read(s + i, 1) != 1) {
                memory.writeb(s + i, 0);
                return 0;
            }
            char c = (char)(memory.readb(s+i) & 0xFF);
            if (c=='\n') {
                memory.writeb(s + i, 0);
                return s;
            }
        }
        memory.writeb(s+n-1,0);
        return s;
    }

    // FILE *fopen(const char * filename, const char * mode)
    static public int fopen64(int filename, int mode) {
        return fopen(filename, mode);
    }
    static public int fopen(int filename, int mode) {
        String m = WineThread.getCurrent().process.memory.readCString(mode);
        if (m.equalsIgnoreCase("r") || m.equalsIgnoreCase("rb")) {
            mode = Fcntl.O_RDONLY;
        } else if (m.equalsIgnoreCase("w") || m.equalsIgnoreCase("wb")) {
            mode = Fcntl.O_WRONLY | Fcntl.O_CREAT | Fcntl.O_TRUNC;
        // :TODO: append is suppose to make sure all writes go to the end even if there was a fseek
        //} else if (m.equalsIgnoreCase("a") || m.equalsIgnoreCase("ab")) {
        //    mode = Fcntl.O_WRONLY | Fcntl.O_CREAT | Fcntl.O_APPEND;
        } else if (m.equalsIgnoreCase("r+") || m.equalsIgnoreCase("r+b") || m.equalsIgnoreCase("rb+")) {
            mode = Fcntl.O_RDWR;
        } else if (m.equalsIgnoreCase("w+") || m.equalsIgnoreCase("w+b") || m.equalsIgnoreCase("wb+")) {
            mode = Fcntl.O_RDWR | Fcntl.O_CREAT | Fcntl.O_TRUNC;
        //} else if (m.equalsIgnoreCase("a+") || m.equalsIgnoreCase("a+b") || m.equalsIgnoreCase("ab+")) {
        //    mode = Fcntl.O_RDWR | Fcntl.O_CREAT | Fcntl.O_APPEND;
        } else {
            Log.panic("fopen mode "+mode+" not implemented");
        }
        int result = Fcntl.open(filename, mode);
        if (result==-1)
            result = 0;
        return result;
    }

    // int fputc(int c, FILE *stream)
    static public int fputc(int c, int stream) {
        WineThread thread = WineThread.getCurrent();

        FileDescriptor fd = thread.process.getFileDescriptor(stream);
        if (fd==null || !fd.canWrite()) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        fd.object.write(String.valueOf((char)c));
        return c;
    }

    // int fputs(const char * s, FILE * stream)
    static public int fputs(int s, int stream) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;

        FileDescriptor fd = thread.process.getFileDescriptor(stream);
        if (fd==null || !fd.canWrite()) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        int len = 0;
        int address = s;
        while (true) {
            if (memory.readb(address)==0)
                break;
            address++;
            len++;
        }
        fd.object.write(s, len);
        return 1;
    }

    // size_t fread(void * ptr, size_t size, size_t nitems, FILE * stream)
    static public int fread(int ptr, int size, int nitems, int stream) {
        WineThread thread = WineThread.getCurrent();

        FileDescriptor fd = thread.process.getFileDescriptor(stream);
        if (fd==null || !fd.canRead()) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        return fd.object.read(ptr, size * nitems);
    }

    // int fscanf(FILE *stream, const char *format, ... )
    static public int fscanf(int stream, int format) {
        Log.panic("fscanf not implemented");
        return 0;
    }

    // size_t fwrite(const void * ptr, size_t size, size_t nitems, FILE * stream) {
    static public int fwrite(int ptr, int size, int nitems, int stream) {
        WineThread thread = WineThread.getCurrent();

        FileDescriptor fd = thread.process.getFileDescriptor(stream);
        if (fd==null || !fd.canWrite()) {
            thread.setErrno(Errno.EBADF);
            return -1;
        }
        return fd.object.write(ptr, size*nitems);
    }

    // void perror(const char *s)
    static public void perror(int s) {
        WineThread thread = WineThread.getCurrent();
        thread.out(thread.process.memory.readCString(s));
    }

    // int putchar(int c)
    static public int putchar(int c) {
        WineProcess process = WineThread.getCurrent().process;
        process.getFileDescriptor(1).object.write(String.valueOf((char)c));
        return 1;
    }

    // int puts(const char *s)
    static public int puts(int s) {
        WineProcess process = WineThread.getCurrent().process;
        String msg = process.memory.readCString(s)+"\n";
        process.getFileDescriptor(1).object.write(msg);
        return 1;
    }

    // int rename(const char *old, const char *new)
    static public int rename(int old, int newName) {
        WineThread thread = WineThread.getCurrent();
        WineProcess process = thread.process;
        Memory memory = process.memory;
        FSNode from = FSNode.getNode(memory.readCString(old), true);
        FSNode to = FSNode.getNode(memory.readCString(newName), false);
        if (from==null || !from.exists() || to==null) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        if (to.exists()) {
            thread.setErrno(Errno.EEXIST);
            return -1;
        }
        if (!from.renameTo(to)) {
            thread.setErrno(Errno.EIO);
            return -1;
        }
        return 0;
    }

    // void setbuf(FILE * stream, char * buf)
    static public void setbuf(int stream, int buf) {
    }

    // int setvbuf(FILE * stream, char * buf, int type, size_t size)
    static public int setvbuf(int stream, int buf, int type, int size) {
        return 0;
    }
}

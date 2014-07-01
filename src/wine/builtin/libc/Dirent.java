package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.io.FSNode;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;

public class Dirent {
    static public final int DT_DIR = 4;
    static public final int DT_REG = 8;
//    struct dirent
//    {
//        __ino_t d_ino;              /* File serial number.  */
//        unsigned short int d_reclen; /* Length of the whole `struct dirent'.  */
//        unsigned char d_type;       /* File type, possibly unknown.  */
//        unsigned char d_namlen;     /* Length of the file name.  */
//
//    /* Only this member is in the POSIX standard.  */
//        char d_name[1];             /* File name (actually longer).  */
//    };


//    struct dirent64
//    {
//        __ino64_t d_ino;
//        unsigned short int d_reclen;
//        unsigned char d_type;
//        unsigned char d_namlen;
//
//        char d_name[1];
//    };

    // DIR *opendir(const char *name)
    static public int opendir(int name) {
        WineThread thread = WineThread.getCurrent();
        String dirName = thread.process.memory.readCString(name);
        FileDescriptor fd = thread.process.getFile(dirName, true);
        if (fd == null) {
            thread.setErrno(Errno.ENOENT);
            return 0;
        }
        KernelFile file = fd.getFile();
        if (file == null) {
            thread.setErrno(Errno.ENOENT);
            return 0;
        }
        if (!file.node.isDirectory()) {
            thread.setErrno(Errno.ENOTDIR);
            return 0;
        }
        return fd.handle;
    }

    // int closedir(DIR *dir)
    static public int closedir(int dir) {
        WineThread thread = WineThread.getCurrent();
        if (thread.process.closeFileDescriptor(dir)) {
            return 0;
        }
        thread.setErrno(Errno.EBADF);
        return -1;
    }

    static public class ReadDirData implements FileDescriptor.FileDescriptorData {
        public FSNode[] dirs;
        public int index;
        public int pDirent;

        public ReadDirData() {
            pDirent = WineThread.getCurrent().process.alloc(512);
        }
        public void onClose() {
            WineThread.getCurrent().process.free(pDirent);
        }
        public ReadDirData copy() {
            ReadDirData result = new ReadDirData();
            result.dirs = this.dirs.clone();
            result.index = this.index;
            result.pDirent = this.pDirent;
            return result;
        }
    }
    // struct dirent *readdir(DIR *dir)
    static public int readdir(int dir) {
        return readdir(dir, false);
    }

    // struct dirent64 *readdir64(DIR * dir)
    static public int readdir64(int dir) {
        return readdir(dir, true);
    }

    static private int readdir(int dir, boolean b64) {
        WineThread thread = WineThread.getCurrent();
        FileDescriptor fd = thread.process.getFileDescriptor(dir);
        KernelFile file = null;
        if (fd!=null) {
            file = fd.getFile();
        }
        if (file == null) {
            thread.setErrno(Errno.EBADF);
            return 0;
        }
        if (fd.readDirData==null) {
            FSNode[] files = file.node.list();
            if (files == null || files.length == 0) {
                return 0;
            }
            fd.readDirData = new ReadDirData();
            fd.readDirData.dirs = files;
        }
        if (fd.readDirData.index<fd.readDirData.dirs.length) {
            FSNode result = fd.readDirData.dirs[fd.readDirData.index++];
            Memory memory = thread.process.memory;
            int name;
            if (b64) {
                memory.writeq(fd.readDirData.pDirent, result.id);
                name = fd.readDirData.pDirent+12;
                memory.writew(name+8, result.name().length()+1+12);
            } else {
                memory.writed(fd.readDirData.pDirent, result.id);
                name = fd.readDirData.pDirent+8;
                memory.writew(name+4, result.name().length()+1+8);
            }
            int type = DT_REG;
            if (result.isDirectory()) {
                type = DT_DIR;
            }
            memory.writeb(name-2, type); // d_type
            memory.writeb(name-1, result.name().length()); // d_namlen
            memory.writeCString(name, result.name());
            return fd.readDirData.pDirent;
        }
        return 0;
    }
}

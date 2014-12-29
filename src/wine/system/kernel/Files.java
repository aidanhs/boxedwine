package wine.system.kernel;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.io.FSNode;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;

public class Files {
    static public int getdents(WineThread thread, int dir, int dirp, int count, boolean is64) {
        FileDescriptor fd = thread.process.getFileDescriptor(dir);
        KernelFile file = null;
        Memory memory = thread.process.memory;

        if (fd!=null) {
            file = fd.getFile();
        }
        if (file == null) {
            return -Errno.EBADF;
        }
        if (!file.node.isDirectory())
            return -Errno.ENOTDIR;
        if (file.getdentsData==null)
            file.getdentsData = file.node.list();
        int len = 0;
        for (int i=(int)file.getFilePointer();i<file.getdentsData.length;i++) {
            int recordLen;
            FSNode node = file.getdentsData[i];
            if (is64) {
                recordLen = 24+node.name().length();
                recordLen=(recordLen+3) / 4 * 4;
                if (recordLen+len>count) {
                    if (len==0)
                        return -Errno.EINVAL;
                    return len;
                }
                memory.writeq(dirp, node.id);
                memory.writeq(dirp+8, i);
                memory.writew(dirp+16, recordLen);
                memory.writeb(dirp+19, node.getType());
                memory.writeCString(dirp+19, node.name());
            } else {
                recordLen = 12+node.name().length();
                recordLen=(recordLen+3) / 4 * 4;
                if (recordLen+len>count) {
                    if (len==0)
                        return -Errno.EINVAL;
                    return len;
                }
                memory.writed(dirp, node.id);
                memory.writed(dirp+4, i);
                memory.writew(dirp+8, recordLen);
                memory.writeCString(dirp+10, node.name());
                memory.writeb(dirp+recordLen-1, node.getType());
            }
            dirp+=recordLen;
            len+=recordLen;
            file.seek(0, i+1);
        }
        return len;
    }
}

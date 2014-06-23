package wine.system.io;

import wine.emulation.Memory;
import wine.system.WineSystem;

public class KernelStat {
        static public final int S_IFMT         = 0xF000;          /* file type mask */
        static public final int S_IFSOCK       = 0xC000;
        static public final int _S_IFDIR       = 0x4000;          /* directory */
        static public final int _S_IFCHR       = 0x2000;          /* character special */
        static public final int _S_IFIFO       = 0x1000;          /* pipe */
        static public final int _S_IFREG       = 0x8000;          /* regular */
        static public final int _S_IREAD       = 0x0100;          /* read permission, owner */
        static public final int _S_IWRITE      = 0x0080;          /* write permission, owner */
        static public final int _S_IEXEC       = 0x0040;          /* execute/search permission, owner */

    public void writeStat32(Memory memory, int buf) {
        int t = (int)(mtime/1000); // ms to sec
        memory.writed(buf, (int)st_dev);buf+=4;//st_dev
        memory.writed(buf, (int)st_ino);buf+=4;//st_ino
        memory.writed(buf, st_mode);buf+=4;//st_mode
        memory.writed(buf, 1);buf+=4;//st_nlink
        memory.writed(buf, WineSystem.uid);buf+=4;//st_uid
        memory.writed(buf, WineSystem.gid);buf+=4;//st_gid
        memory.writed(buf, (int)st_rdev);buf+=4;//st_rdev
        memory.writed(buf, (int)st_size);buf+=4;//st_size
        memory.writed(buf, t);buf+=4;//st_atime
        memory.writed(buf, t);buf+=4;//st_mtime
        memory.writed(buf, t);buf+=4;//st_ctime
        memory.writed(buf, st_blksize);buf+=4;//st_blksize (not used on wine)
        memory.writed(buf, (int)st_blocks);//st_blocks
    }
    public void writeStat64(Memory memory, int buf) {
        int t = (int)(mtime/1000); // ms to sec
        int n = (int)(mtime % 1000) * 1000000;

        memory.writeq(buf, st_dev);buf+=8;//st_dev
        buf+=4; // padding
        memory.writed(buf, (int)st_ino);buf+=4;//__st_ino
        memory.writed(buf, st_mode);buf+=4;//st_mode
        memory.writed(buf, 1);buf+=4;//st_nlink
        memory.writed(buf, WineSystem.uid);buf+=4;//st_uid
        memory.writed(buf, WineSystem.gid);buf+=4;//st_gid
        memory.writeq(buf, st_rdev);buf+=8;//st_rdev
        buf+=4;
        memory.writeq(buf, st_size);buf+=8;//st_size
        memory.writed(buf, st_blksize);buf+=4;//st_blksize
        memory.writeq(buf, st_blocks);buf+=8; //st_blocks
        memory.writed(buf, t);buf+=4; // st_atime
        memory.writed(buf, n);buf+=4; // st_atime_nsec
        memory.writed(buf, t);buf+=4; // st_mtime
        memory.writed(buf, n);buf+=4; // st_mtime_nsec
        memory.writed(buf, t);buf+=4; // st_ctime
        memory.writed(buf, n);buf+=4; // st_ctime_nsec
        memory.writeq(buf, st_ino); // st_ino
    }
    public long mtime;
    public long st_ino;
    public long st_dev;
    public int st_mode;
    public long st_size;
    public long st_blocks;
    public long st_rdev;
    public int st_blksize = 512;
}

package wine.system.io;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.kernel.Io;
import wine.util.Log;

public class FileLock {
    public FileLock() {

    }

    public FileLock(int address) {
        read(address);
    }

    public FileLock(int address, boolean is64) {
        if (is64)
            read64(address);
        else
            read(address);
    }

    public void read(int address) {
        Memory memory = WineThread.getCurrent().process.memory;
        l_type=memory.readw(address);address+=2;
        l_whence=memory.readw(address);address+=2;
        l_start=memory.readd(address);address+=4;
        l_len=memory.readd(address);address+=4;
        if (l_len==0)
            l_len=Long.MAX_VALUE;
        l_pid=memory.readw(address);

        if (l_whence!= Io.SEEK_SET) {
            Log.panic("Lock with l_whence "+l_whence+" not implemented");
        }
    }

    public void write(int address) {
        Memory memory = WineThread.getCurrent().process.memory;
        memory.writew(address, l_type);address+=2;
        memory.writew(address, l_whence);address+=2;
        memory.writed(address, (int)l_start);address+=4;
        if (l_len==Long.MAX_VALUE) {
            memory.writed(address, 0);
            address += 4;
        } else {
            memory.writed(address, (int)l_len);
            address += 4;
        }
        memory.writed(address, l_pid);
    }

    public void read64(int address) {
        Memory memory = WineThread.getCurrent().process.memory;
        l_type=memory.readw(address);address+=2;
        l_whence=memory.readw(address);address+=2;
        l_start=memory.readq(address);address+=8;
        l_len=memory.readq(address);address+=8;
        if (l_len==0)
            l_len=Long.MAX_VALUE;
        l_pid=WineThread.getCurrent().process.id;

        if (l_whence!= Io.SEEK_SET) {
            Log.panic("Lock with l_whence "+l_whence+" not implemented");
        }
    }

    public void write64(int address) {
        Memory memory = WineThread.getCurrent().process.memory;
        memory.writew(address, l_type);address+=2;
        memory.writew(address, l_whence);address+=2;
        memory.writeq(address, l_start);address+=8;
        if (l_len==Long.MAX_VALUE) {
            memory.writed(address, 0);
            address += 8;
        } else {
            memory.writeq(address, l_len);
            address += 8;
        }
        memory.writed(address, l_pid);
    }

    public boolean doesOverlap(FileLock lock) {
        long end1 = l_start+l_len;
        long start1 = l_start;
        long end2 = lock.l_start+lock.l_len;
        long start2 = lock.l_start;
        return (end2>start1 && end2<=end1) || (start2>=start1 && start2<end2);
    }

    public void copy(FileLock lock) {
        this.l_type = lock.l_type;
        this.l_whence = lock.l_whence;
        this.l_start = lock.l_start;
        this.l_len = lock.l_len;
        this.l_pid = lock.l_pid;
    }
    public int l_type;
    public int l_whence;
    public long l_start;
    public long l_len;
    public int l_pid;
}

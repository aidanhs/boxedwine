package wine.builtin.libc;

import wine.emulation.PageHandler;
import wine.emulation.RAM;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;
import wine.util.Log;

import java.util.Arrays;

public class MMapHandler extends PageHandler {
    final public KernelFile file;
    final public FileDescriptor fd;
    final public int addressOffset;
    final public long fileOffset;
    public int physicalPage;
    final public boolean shared;
    public boolean dirty;

    protected void alloc() {
        physicalPage = RAM.allocPage();
        byte[] buffer = new byte[4096];
        int read = file.io.read(buffer);
        if (read<=0) {
            Log.panic("This should not happen, maybe the user messed with the file while running wine");
        }
        if (read<buffer.length) {
            Arrays.fill(buffer, read, buffer.length - 1, (byte) 0);
        }
        int address = physicalPage<<12;
        for (int i=0;i<buffer.length;i++) {
            RAM.writeb(address++, buffer[i]);
        }
    }

    public MMapHandler(FileDescriptor fd, KernelFile file, int addressOffset, long fileOffset, boolean shared) {
        this.fd = fd;
        this.file = file;
        this.fileOffset = fileOffset;
        this.addressOffset = addressOffset;
        this.shared = shared;
        file.incrementRefCount();
    }

    public void sync(int flags) {
        if (dirty) {
            dirty = false;
        }
    }

    public void close() {
        sync(Mmap.MS_SYNC);
        file.close();
        if (this.physicalPage != 0) {
            RAM.freePage(physicalPage);
        }
    }

    public int readd(int address) {
        if (physicalPage==0) {
            alloc();
        }
        return RAM.readd(address-addressOffset);
    }
    public int readw(int address) {
        if (physicalPage==0) {
            alloc();
        }
        return RAM.readw(address-addressOffset);
    }
    public int readb(int address) {
        if (physicalPage==0) {
            alloc();
        }
        return RAM.readb(address-addressOffset);
    }
    public void writed(int address, int value) {
        if (physicalPage==0) {
            alloc();
        }
        dirty=true;
        RAM.writed(address-addressOffset, value);
    }
    public void writew(int address, int value) {
        if (physicalPage==0) {
            alloc();
        }
        dirty=true;
        RAM.writew(address-addressOffset, value);
    }
    public void writeb(int address, int value) {
        if (physicalPage==0) {
            alloc();
        }
        dirty=true;
        RAM.writeb(address-addressOffset, value);
    }
}

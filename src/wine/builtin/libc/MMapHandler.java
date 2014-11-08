package wine.builtin.libc;

import wine.emulation.Memory;
import wine.emulation.PageHandler;
import wine.emulation.RAM;
import wine.system.WineProcess;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;
import wine.util.Log;

public class MMapHandler extends PageHandler {
    final public KernelFile file;
    final public FileDescriptor fd;
    final public int addressOffset;
    final public long fileOffset;
    public int addressTranslation;
    public int physicalPage;
    final public boolean shared;
    public boolean dirty;
    private int refCount = 1;

    protected void alloc() {
        physicalPage = RAM.allocPage();
        addressTranslation = (physicalPage<<12)-addressOffset;

        synchronized (file) {
            byte[] b = new byte[4096];
            long pos = file.io.getFilePointer();
            if (!file.io.seek(fileOffset)) {
                Log.panic("Oops");
            }
            int read = file.io.read(b);
            int a = physicalPage << 12;
            // to use read, use b.length, so that 0's will fill the end of the page
            for (int i = 0; i < b.length; i++) {
                RAM.writeb(a++, b[i]);
            }
            if (pos >= 0) {
                file.io.seek(pos);
            }
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
        refCount--;
        if (refCount==0) {
            sync(Mmap.MS_SYNC);
            file.close();
            if (this.physicalPage != 0) {
                RAM.freePage(physicalPage);
            }
        }
    }

    public int readd(Memory memory, int address) {
        if (physicalPage==0) {
            alloc();
        }
        if ((address & 0xFFF)<0xFFD) {
            return RAM.readd(address+addressTranslation);
        }
        return memory.unalignedReadd(address);
    }
    public int readw(Memory memory, int address) {
        if (physicalPage==0) {
            alloc();
        }
        if ((address & 0xFFF)<0xFFF) {
            return RAM.readw(address+addressTranslation);
        }
        return memory.unalignedReadw(address);
    }
    public int readb(int address) {
        if (physicalPage==0) {
            alloc();
        }
        return RAM.readb(address+addressTranslation);
    }
    public void writed(Memory memory, int address, int value) {
        if (physicalPage==0) {
            alloc();
        }
        dirty=true;

        if ((address & 0xFFF)<0xFFD) {
            RAM.writed(address+addressTranslation, value);
        } else {
            memory.unalignedWrited(address, value);
        }

    }
    public void writew(Memory memory, int address, int value) {
        if (physicalPage==0) {
            alloc();
        }
        dirty=true;

        if ((address & 0xFFF)<0xFFF) {
            RAM.writew(address+addressTranslation, value);
        } else {
            memory.unalignedWritew(address, value);
        }
    }
    public void writeb(int address, int value) {
        if (physicalPage==0) {
            alloc();
        }
        dirty=true;
        RAM.writeb(address+addressTranslation, value);
    }

    public PageHandler fork(WineProcess process) {
        MMapHandler handler = new MMapHandler(process.getFileDescriptor(fd.handle), file, addressOffset, fileOffset, shared);
        handler.dirty = this.dirty;
        if (shared) {
            Log.panic("forked mmap shared mapped files not implemented");
        }
        return handler;
    }
}

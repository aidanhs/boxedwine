package wine.builtin.libc;

import wine.emulation.Memory;
import wine.emulation.PageHandler;
import wine.emulation.RAM;
import wine.system.WineProcess;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;

public final class MMapHandlerNone extends MMapHandler {
    public MMapHandlerNone(FileDescriptor fd, KernelFile file, int addressOffset, long fileOffset, boolean shared) {
        super(fd, file, addressOffset, fileOffset, shared);
    }
    public void writed(Memory memory, int address, int value) {
        pf(address);
    }
    public void writew(Memory memory, int address, int value) {
        pf(address);
    }
    public void writeb(int address, int value) {
        pf(address);
    }
    public int readd(Memory memory, int address) {
        pf(address);
        return 0;
    }
    public int readw(Memory memory, int address) {
        pf(address);
        return 0;
    }
    public int readb(int address) {
        pf(address);
        return 0;
    }

    public PageHandler fork(WineProcess process) {
        MMapHandlerNone handler = new MMapHandlerNone(process.getFileDescriptor(fd.handle), file, addressOffset, fileOffset, shared);
        handler.dirty = this.dirty;
        if (physicalPage!=0) {
            if (shared) {
                RAM.incrementRef(physicalPage);
                handler.physicalPage = physicalPage;
                handler.addressTranslation = addressTranslation;
            } else {
                handler.physicalPage = RAM.allocPage();
                RAM.copy(physicalPage, handler.physicalPage);
                handler.addressTranslation = (handler.physicalPage<<12)-handler.addressOffset;
            }
        }
        return handler;
    }
}

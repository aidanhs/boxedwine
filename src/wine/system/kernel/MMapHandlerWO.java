package wine.system.kernel;

import wine.emulation.Memory;
import wine.emulation.PageHandler;
import wine.emulation.RAM;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;

public final class MMapHandlerWO extends MMapHandler {
    public MMapHandlerWO(FileDescriptor fd, KernelFile file, int addressOffset, long fileOffset, boolean shared) {
        super(fd, file, addressOffset, fileOffset, shared);
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

    public PageHandler fork(wine.system.kernel.Process process) {
        MMapHandlerWO handler = new MMapHandlerWO(process.getFileDescriptor(fd.handle), file, addressOffset, fileOffset, shared);
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

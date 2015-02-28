package wine.emulation;

import wine.system.kernel.*;
import wine.system.kernel.Process;
import wine.util.Log;

abstract public class PageHandler {
    protected int data;

    public static final int REF_COUNT_MASK  = 0x0000FFFF;
    public static final int FLAG_MASK       = 0xFFFF0000;
    public static final int READ            = 0x00010000;
    public static final int WRITE           = 0x00020000;
    public static final int EXEC            = 0x00040000;
    public static final int PERMISSIONS     = 0x00070000;
    public static final int MAPPED          = 0x00080000;
    public static final int SHARED          = 0x00100000;
    public static final int COPY_ON_WRITE   = 0x00200000;
    public static final int DIRTY           = 0x00400000;
    public static final int HAS_CODE        = 0x00800000;

    public boolean isMmap() {
        return (data & MAPPED)!=0;
    }

    private void setMmap(boolean mmap) {
        data = data & ~MAPPED;
        if (mmap)
            data|=MAPPED;
    }

    public boolean isCopyOnWrite() {
        return (data & COPY_ON_WRITE)!=0;
    }

    public void setCopyOnWrite(boolean copyOnWrite) {
        data = data & ~COPY_ON_WRITE;
        if (copyOnWrite)
            data|=COPY_ON_WRITE;
    }

    public boolean isShared() {
        return (data & SHARED)!=0;
    }

    public boolean hasCode() {
        return (data & HAS_CODE)!=0;
    }

    private void setShared(boolean shared) {
        data = data & ~SHARED;
        if (shared)
            data|=SHARED;
    }

    public boolean isReadWrite() {
        return (data & PERMISSIONS) == (READ|WRITE);
    }

    public boolean isReadOnly() {
        return (data & PERMISSIONS) == READ;
    }

    public boolean isWriteOnly() {
        return (data & PERMISSIONS) == WRITE;
    }

    public int getFlags() {
        return data & (~REF_COUNT_MASK);
    }

    public int getRefCount() {
        return data & REF_COUNT_MASK;
    }

    public void setRefCount(int refCount) {
        data = data & 0xFFFF0000;
        data |= (refCount & REF_COUNT_MASK);
        if (refCount>REF_COUNT_MASK) {
            Log.panic("RAM refcount greater than "+REF_COUNT_MASK);
        }
    }

    abstract public void close(Process process, int page);
    abstract public int readd(Memory memory, int address);
    abstract public int readw(Memory memory, int address);
    abstract public int readb(Memory memory, int address);
    abstract public void writed(Memory memory, int address, int value);
    abstract public void writew(Memory memory, int address, int value);
    abstract public void writeb(Memory memory, int address, int value);
    abstract public PageHandler fork(Process from, Process to, int index);
    public void pf(int address) {
        Log.panic("PF "+Integer.toHexString(address));
    }
}

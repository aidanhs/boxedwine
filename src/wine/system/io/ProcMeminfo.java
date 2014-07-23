package wine.system.io;

import wine.emulation.RAM;

public class ProcMeminfo implements FSNodeAccess {
    private int pos = 0;
    private byte[] buffer;

    public boolean open(String mode) {
        StringBuilder buffer = new StringBuilder();
        buffer.append("MemTotal: ");
        buffer.append(RAM.pageCount()<<2);
        buffer.append(" kB\n");
        buffer.append("MemFree: ");
        buffer.append(RAM.freePageCount<<2);
        buffer.append(" kB\n");
        this.buffer = buffer.toString().getBytes();
        return true;
    }

    public boolean setLength(long length) {
        return false;
    }

    public long getFilePointer() {
        return 0;
    }

    public boolean seek(long pos) {
        return false;
    }

    public int read(byte[] b) {
        int len = b.length;
        if (pos+len>buffer.length)
            len = buffer.length-pos;
        System.arraycopy(buffer, pos, b, 0, len);
        pos+=len;
        return len;
    }

    public boolean write(byte[] b) {
        return true;
    }

    public void close() {
    }
}

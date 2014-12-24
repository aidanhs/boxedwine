package wine.system.io;

import wine.emulation.Memory;
import wine.system.kernel.Syscall;

public class DevMice implements FSNodeAccess {
    public int lastX = 512;
    public int lastY = 384;

    private int firstPos = 0;
    private int lastPos = 0;
    private byte[] buffer = new byte[1024*3];

    public void event(int x, int y, int button) {
        int deltaX;
        int deltaY;

        if (lastX>=0)
            deltaX = x - lastX;
        else
            deltaX = 0;
        if (lastY>=0)
            deltaY = y - lastY;
        else
            deltaY = 0;
        int yOverflow = 0;
        int xOverflow = 0;
        int ySign = (deltaY<0)?1:0;
        int xSign = (deltaX<0)?1:0;
        int leftButton = 0;
        int rightButton = 0;
        int middleButton = 0;
        if (lastPos>=buffer.length)
            lastPos = 0;
        buffer[lastPos++] = (byte)((yOverflow << 7) | (xOverflow << 6) | (ySign << 5) | (xSign << 4) | (1 << 3) | (middleButton << 2) | (rightButton << 1) | (leftButton));
        buffer[lastPos++] = (byte)(Math.abs(deltaX));
        buffer[lastPos++] = (byte)(Math.abs(deltaY));
        synchronized (FileDescriptor.lock) {
            FileDescriptor.lock.notifyAll();
        }
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
        return 0;
    }

    public boolean write(byte[] b) {
        return false;
    }

    public void close() {

    }

    public boolean open(String mode) {
        return true;
    }

    public int ioctl(int request, Syscall.SyscallGetter getter) {
        return 0;
    }

    public int map(Memory memory, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        return 0;
    }

    public boolean isReadReady() {
        return firstPos!=lastPos;
    }

    private int length() {
        if (firstPos<lastPos) {
            return lastPos-firstPos;
        } else if (firstPos>lastPos) {
            return lastPos+(buffer.length-firstPos);
        }
        return 0;
    }

    public boolean isWriteReady() {
        return length()<buffer.length;
    }
}

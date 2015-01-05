package wine.system.io;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.kernel.Syscall;

public class DevPS2Mouse implements FSNodeAccess {
    public int lastX = 512;
    public int lastY = 384;
    private int firstPos = 0;
    private int lastPos = 0;
    private byte[] buffer = new byte[1024*3];
    private boolean streaming = false;
    private int sampling = 100;
    private int resolution = 100;

    synchronized public void event(int x, int y, int button) {
        if (!streaming)
            return;
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
        lastX = x;
        lastY = y;
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

    synchronized public int read(byte[] b) {
        int pos = 0;

        while (firstPos!=lastPos && pos<b.length) {
            b[pos++] = buffer[firstPos++];
            if (firstPos>=buffer.length)
                firstPos=0;
        }
        return pos;
    }

    private void write(int b) {
        if (lastPos >= buffer.length)
            lastPos = 0;
        buffer[lastPos++] = (byte)b;
    }

    private int command = 0;

    synchronized public boolean write(byte[] b) {
        if (command!=0) {
            switch (command) {
                case 0xF3: // sampling rate
                    sampling = b[0] & 0xFF;
                    write(0xFA);
                    break;
                case 0xE8: // resolution
                    resolution = b[0] & 0xFF;
                    write(0xFA);
                    break;
                default:
                    break;
            }
            command = 0;
            return true;
        }
        switch (b[0] & 0xFF) {
            case 0xFF: // reset
                write(0xFA);
                write(0xAA);
                sampling = 100;
                resolution = 100;
                streaming = false;
                break;
            case 0xF4: // Enable Packet Streaming
                this.streaming = true;
                write(0xFA);
                break;
            case 0xF3: // sampling rate
                command = 0xF3;
                write(0xFA);
                break;
            case 0xE8: // resolution
                command = 0xE8;
                write(0xFA);
                break;
            case 0xE6: // 1:1 scaling
                write(0xFA);
                break;
            default:
                break;
        }
        return true;
    }

    public void close() {
    }

    public boolean open(String mode) {
        return true;
    }

    public int ioctl(WineThread thread, int request, Syscall.SyscallGetter getter) {
        return -1;
    }

    public int map(Memory memory, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        return 0;
    }

    synchronized public boolean isReadReady() {
        return firstPos!=lastPos;
    }

    synchronized private int length() {
        if (firstPos<lastPos) {
            return lastPos-firstPos;
        } else if (firstPos>lastPos) {
            return lastPos+(buffer.length-firstPos);
        }
        return 0;
    }

    synchronized public boolean isWriteReady() {
        return length()<buffer.length;
    }
} 
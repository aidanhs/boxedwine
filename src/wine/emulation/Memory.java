package wine.emulation;

import wine.system.WineProcess;

import java.util.Vector;
import java.util.concurrent.locks.ReentrantReadWriteLock;

// one per process
public class Memory {
    final public PageHandler[] handlers = new PageHandler[0x100000]; // 1 million, this uses 4MB per process just to track paging
    static final public PageHandler invalidHandler = new InvalidHandler();
    final static private Object mutext = new Object();
    static private ReentrantReadWriteLock lock = new ReentrantReadWriteLock(true);

    public Memory() {
    }

    public int readd(int address) {
        return handlers[(address >>> 12)].readd(this, address);
    }
    public long readq(int address) {
        long v1=handlers[(address>>>12)].readd(this, address) & 0xFFFFFFFFl;
        long v2=handlers[(address>>>12)].readd(this, address+4) & 0xFFFFFFFFl;
        return v1 | (v2 << 32);
    }
    public int unalignedReadd(int address) {
        return readb(address) | (readb(address+1)<<8) | (readb(address+2)<<16) | (readb(address+3)<<24);
    }
    public int readw(int address) {
        return handlers[(address>>>12)].readw(this, address);
    }
    public int unalignedReadw(int address) {
        return readb(address) | (readb(address+1)<<8);
    }
    public int readb(int address) {
        return handlers[(address>>>12)].readb(address);
    }
    public void writed(int address, int value) {
        handlers[(address>>>12)].writed(this, address, value);
    }
    public void writeq(int address, long value) {
        handlers[(address>>>12)].writed(this, address, (int)value);
        handlers[(address>>>12)].writed(this, address+4, (int)(value>>>32));
    }
    public void unalignedWrited(int address, int value) {
        writeb(address, value);
        writeb(address+1, value>>8);
        writeb(address+2, value>>16);
        writeb(address+3, value>>24);
    }
    public void writew(int address, int value) {
        handlers[(address>>>12)].writew(this, address, value);
    }
    public void unalignedWritew(int address, int value) {
        writeb(address, value);
        writeb(address+1, value>>8);
    }
    public void writeb(int address, int value) {
        handlers[(address>>>12)].writeb(address, value);
    }
    public int readws(int address) {
        return (short)(readw(address));
    }
    public int readbs(int address) {
        return (byte) readb(address);
    }
    public void memcpy(int dest, int src, int length) {
        for (int i=0;i<length;i++) {
            writeb(dest+i, readb(src+i));
        }
    }
    public void memcpy(int address, byte[] src, int offset, int length) {
        for (int i=0;i<length;i++) {
            writeb(address+i, src[offset+i]);
        }
    }
    public void memcpy(byte[] dest, int offset, int length, int address) {
        for (int i=0;i<length;i++) {
            dest[offset+i] = (byte)readb(address+i);
        }
    }
    public void memset(int address, int c, int length) {
        for (int i=0;i<length;i++) {
            writeb(address+i, c);
        }
    }

    public void zero(int address, int length) {
        memset(address, 0, length);
    }

    public String readCString(int address) {
        return readCString(address, Integer.MAX_VALUE);
    }

    public String[] readCStringArray(int address) {
        return readCStringArray(address, Integer.MAX_VALUE);
    }
    public String[] readCStringArray(int address, int count) {
        Vector<String> results = new Vector<String>();
        for (int i=0;i<count;i++) {
            int p = readd(address);
            address+=4;
            if (p==0)
                break;
            results.add(readCString(p));
        }
        String[] s = new String[results.size()];
        results.copyInto(s);
        return s;
    }

    public String readCString(int address, int len) {
        StringBuilder result = new StringBuilder();
        for (int i=0;i<len;i++) {
            char c = (char)readb(address++); // :TODO: need to research converting according to 1252
            if (c == 0)
                break;
            result.append(c);
        }
        return result.toString();
    }
    public String readCStringW(int address) {
        StringBuilder result = new StringBuilder();
        while (true) {
            char c = (char)readw(address);
            address+=2;
            if (c == 0)
                break;
            result.append(c);
        }
        return result.toString();
    }
    public void writeCString(int address, String value, int len) {
        if (len<1)
            return;
        for (int i=0;i<value.length() && i<len-1;i++) {
            writeb(address++, value.charAt(i));
        }
        writeb(address, 0);
    }

    public void writeCString(int address, String value) {
        for (int i=0;i<value.length();i++) {
            writeb(address++, value.charAt(i));
        }
        writeb(address, 0);
    }

    public void fork(WineProcess process) {
        for (int i=0;i<handlers.length;i++) {
            process.memory.handlers[i] = handlers[i].fork(process);
        }
    }

    public void close() {
        for (int i=0;i<handlers.length;i++) {
            if (handlers[i]!=null)
                handlers[i].close();
            handlers[i]=null;
        }
    }

    public void init() {
        for (int i=0;i<handlers.length;i++) {
            if (handlers[i]!=null)
                handlers[i].close();
            handlers[i] = invalidHandler;
        }
    }

    public void lock() {
        lock.writeLock().lock();
    }

    public void unlock() {
        lock.writeLock().unlock();
    }
}

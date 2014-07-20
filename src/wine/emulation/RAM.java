package wine.emulation;

import wine.util.Log;

import java.util.Arrays;

public class RAM {
    static private int[] bytes;
    static private int[] freePages;
    static private int[] pageRefCount;
    static public int freePageCount;

    static public void init(int size) {
        bytes = new int[size>>2];
        freePageCount = size >>> 12;
        freePages = new int[freePageCount];
        for (int i=0;i<freePageCount;i++) {
            freePages[i]=i;
        }
        pageRefCount = new int[freePageCount];
    }

    static public int pageCount() {
        return pageRefCount.length;
    }

    synchronized static public int allocPage() {
        if (freePageCount==0) {
            Log.panic("Ran out of RAM pages: todo: need to implement unloading of memory mapped files");
        }
        int result = freePages[--freePageCount];
        if (pageRefCount[result]!=0) {
            Log.panic("RAM logic error");
        }
        pageRefCount[result]++;
        int start = result<<10;
        Arrays.fill(bytes, start, start+1024, 0);
        return result;
    }

    // must match what was returned in allocPages, you can't free random pages
    synchronized static public void freePage(int page) {
        pageRefCount[page]--;
        if (pageRefCount[page]==0) {
            freePages[freePageCount++]=page;
        } else if (pageRefCount[page]<0) {
            Log.panic("RAM logic error");
        }
    }

    synchronized static public void incrementRef(int page) {
        if (pageRefCount[page]==0) {
            Log.panic("RAM logic error");
        }
        pageRefCount[page]++;
    }
    static public int readd(int address) {
        int rem = (address & 0x3);
        if (rem == 0) {
            return bytes[address >>> 2];
        }
        int off = rem << 3;
        int[] local = bytes;
        int index = (address >>> 2);
        return local[index] >>> off | local[index + 1] << (32 - off);
    }
    static public int readw(int address) {
        int rem = address & 0x3;
        int[] local = bytes;
        int index = (address >>> 2);
        int val = local[index] >>> (rem << 3);
        if (rem == 3) {
            val |= local[index + 1] << 8;
        }
        return val & 0xFFFF;
    }
    static public int readb(int address) {
        return ((bytes[(address >>> 2)] >>> ((address & 0x3) << 3)) & 0xFF);
    }
    static public void writed(int address, int value) {
        int rem = (address & 0x3);
        if (rem == 0) {
            bytes[address >>> 2] = value;
        } else {
            int index = (address >>> 2);
            int[] local = bytes;
            int off = rem << 3;
            int mask = -1 << off;
            local[index] = (local[index] & ~mask) | (value << off);
            index++;
            local[index] = (local[index] & mask) | (value >>> (32 - off));
        }
    }
    static public void writew(int address, int value) {
        int rem = (address & 0x3);
        int[] local = bytes;
        int index = (address >>> 2);
        value &= 0xFFFF;
        if (rem == 3) {
            local[index] = (local[index] & 0xFFFFFF | value << 24);
            index++;
            local[index] = (local[index] & 0xFFFFFF00 | value >>> 8);
        } else {
            int off = rem << 3;
            int mask = ~(0xFFFF << off);
            local[index] = (local[index] & mask | value << off);
        }
    }
    static public void writeb(int address, int value) {
        int off = (address & 0x3) << 3;
        int[] local = bytes;
        int mask = ~(0xFF << off);
        int index = (address >>> 2);
        int val = local[index] & mask | (value & 0xFF) << off;
        local[index] = val;
    }
    static public void copy(int srcPage, int destPage) {
        System.arraycopy(bytes, srcPage*1024, bytes, destPage*1024, 1024);
    }
}

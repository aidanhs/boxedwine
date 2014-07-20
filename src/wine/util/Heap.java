package wine.util;

import wine.emulation.Memory;
import wine.system.WineThread;

import java.util.BitSet;

// This is a very simple ineffecient implementation, it could use some work
public class Heap {
    private BitSet blocks;
    private int start;
    private int end;
    private int blockSize;
    private static final int BLOCK_HEADER_SIZE = 8;
    private int blockCount;
    public Heap(int start, int end, int blockSize) {
        this.blockSize=blockSize;
        clear(start, end);
    }

    public void clear() {
        blocks = new BitSet(blockCount);
    }

    public void clear(int start, int end) {
        this.start = start;
        this.end = end;
        this.blockCount = (end-start)/blockSize;
        clear();
    }

    synchronized public int alloc(int size) {
        int blockStart = 0;
        int blockCount = (size+blockSize-1+BLOCK_HEADER_SIZE)/blockSize;
        int pos = blocks.nextClearBit(blockStart);
        if (pos+blockCount>this.blockCount)
            return 0;
        if (blockCount>1) {
            while (true) {
                int found = blocks.nextSetBit(pos);
                if (found<0) {
                    break;
                }
                if (found>=pos+blockCount) {
                    break;
                }
                pos = blocks.nextClearBit(found);
                if (pos+blockCount>this.blockCount)
                    return 0;
            }
        }
        blocks.set(pos, pos+blockCount);
        Memory memory = WineThread.getCurrent().process.memory;
        int result = pos*blockSize+start+BLOCK_HEADER_SIZE;
        memory.writed(result-4, size);
        memory.writed(result-8, blockCount);
        return result;
    }

    // returns start of grown address
    synchronized public int grow(int size) {
        int result = end;
        end+=size;
        this.blockCount = (end-start)/blockSize;
        return result;
    }

    synchronized public int getSize(int address) {
        return WineThread.getCurrent().process.memory.readd(address-4);
    }

    synchronized public void free(int address) {
        Memory memory = WineThread.getCurrent().process.memory;
        int blockCount = memory.readd(address-8);
        int blockStart = (address-start-BLOCK_HEADER_SIZE)/blockSize;
        for (int i=blockStart;i<blockStart+blockCount;i++) {
            if (!blocks.get(i)) {
                Log.panic("heap.free tried to free block that wasn't allocated");
            }
            blocks.clear(i);
        }
    }

    public Heap deepCopy() {
        Heap result = new Heap(start, end, blockSize);
        result.blocks = (BitSet)blocks.clone();
        return result;
    }
}


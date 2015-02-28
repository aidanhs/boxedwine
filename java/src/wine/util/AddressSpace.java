package wine.util;

import java.util.BitSet;

public class AddressSpace {
    private BitSet space = new BitSet(0x100000);

    synchronized public void clear() {
        space.clear();
    }

    synchronized public boolean allocPages(int pageStart, int pageCount) {
        for (int i=pageStart;i<pageStart+pageCount;i++) {
            if (space.get(i))
                return false;
        }
        space.set(pageStart, pageStart+pageCount);
        return true;
    }

    // when calling getNextPage/allocPages combination you might need to synchronize on the AddressSpace object
    synchronized public int getNextPage(int pageStart, int pageCount) {
        int pos = space.nextClearBit(pageStart);
        if (pos+pageCount>0x100000)
            return 0;
        if (pageCount>1) {
            while (true) {
                int found = space.nextSetBit(pos);
                if (found<0)
                    return pos;
                if (found>=pos+pageCount) {
                    return pos;
                }
                pos = space.nextClearBit(found);
                if (pos+pageCount>0x100000)
                    return 0;
            }
        }
        return pos;
    }

    synchronized public void freePages(int pageStart, int pageCount) {
        space.clear(pageStart, pageStart+pageCount);
    }

    synchronized public AddressSpace deepCopy() {
        AddressSpace result = new AddressSpace();
        result.space = (BitSet)space.clone();
        return result;
    }
}

package wine.util;

import wine.emulation.Memory;

import java.io.IOException;

public class MemoryStream implements LittleEndian {
    private int address;
    private int length;
    private Memory memory;

    public MemoryStream(Memory memory, int address, int length) {
        this.address = address;
        this.length = length;
        this.memory = memory;
    }

    public int available() {
        return length;
    }

    public MemoryStream(Memory memory, int address) {
        this.address = address;
        this.length = Integer.MAX_VALUE;
        this.memory = memory;
    }

    public int readInt() {
        int result = memory.readd(address);
        address+=4;
        length-=4;
        return result;
    }

    public void skip(int bytes) throws IOException {
        address+=bytes;
    }

    public void read(byte[] bytes) throws IOException {
        memory.memcpy(bytes, 0, bytes.length, address);
        address+=bytes.length;
        length-=bytes.length;
    }

    public int readUnsignedByte() throws IOException {
        int result = memory.readb(address);
        address+=1;
        length-=1;
        return result;
    }

    public int readUnsignedShort() {
        int result = memory.readw(address);
        address+=2;
        length-=2;
        return result;
    }

    public long readUnsignedInt() throws IOException {
        int result = memory.readd(address);
        address+=4;
        length-=4;
        return result;
    }
}

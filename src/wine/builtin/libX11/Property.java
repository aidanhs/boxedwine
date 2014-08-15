package wine.builtin.libX11;

import wine.emulation.Memory;
import wine.util.Log;

public class Property {
    public Property(int type, int format) {
        this.type = type;
        this.format = format;
    }
    public Property(int type, int format, int v1, int v2, int v3, int v4) {
        this.type = type;
        this.format = format;
        this.data = new byte[format/2];
        setIndex(0, v1);
        setIndex(1, v2);
        setIndex(2, v3);
        setIndex(3, v4);
    }
    public Property(int type, int format, int v1, int v2) {
        this.type = type;
        this.format = format;
        this.data = new byte[format/4];
        setIndex(0, v1);
        setIndex(1, v2);
    }
    public void setIndexesByAddress(Memory memory, int address, int count) {
        data = new byte[format/8*count];
        for (int i=0;i<count;i++) {
            switch (format) {
                case 32:
                    setIndex32(i, memory.readd(address));
                    address+=4;
                    break;
                case 16:
                    setIndex16(i, memory.readw(address));
                    address+=2;
                    break;
                case 8:
                    setIndex8(i, memory.readb(address));
                    address++;
                    break;
                default:
                    Log.panic("Window.Property unknown format: " + format);
            }
        }
    }
    public void setIndex(int index, int value) {
        switch (format) {
            case 32:
                setIndex32(index, value);
                break;
            case 16:
                setIndex16(index, value);
                break;
            case 8:
                setIndex8(index, value);
                break;
            default:
                Log.panic("Window.Property unknown format: "+format);
        }
    }

    public void setIndex32(int index, int value) {
        data[index*4] = (byte)value;
        data[index*4+1] = (byte)(value >> 8);
        data[index*4+2] = (byte)(value >> 16);
        data[index*4+3] = (byte)(value >>> 24);
    }

    public void setIndex16(int index, int value) {
        data[index*2] = (byte)value;
        data[index*2+1] = (byte)(value >> 8);
    }

    public void setIndex8(int index, int value) {
        data[index] = (byte)value;
    }

    public int getInt(int index) {
        return (data[index] & 0xFF) | ((data[index+1] & 0xFF) << 8) | ((data[index+2] & 0xFF) << 16) | ((data[index+3] & 0xFF) << 24);
    }

    public byte[] data;
    public int type;
    public int format; // 8, 16, or 32
}

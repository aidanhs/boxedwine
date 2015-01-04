package wine.system.io;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.kernel.Syscall;
import wine.util.Log;

import java.util.LinkedList;

public class DevInput implements FSNodeAccess {
    static public final int SIZE = 16;
    private int mask;
    private String name;
    protected int busType;
    protected int vendor;
    protected int product;
    protected int version;

    public DevInput(int mask, String name) {
        this.mask = mask;
        this.name = name;
    }
    static class Data {
        public Data(int type, int code, int value) {
            this.time = (int) System.currentTimeMillis();
            this.type = type;
            this.code = code;
            this.value = value;
        }

        public long time; // timeval
        public int type; // u16
        public int code; // u16
        public int value; // s32
    }

    private LinkedList<Data> queue = new LinkedList<Data>();

    public void event(int type, int code, int value) {
        synchronized (queue) {
            queue.add(new Data(type, code, value));
        }
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
        Data input;
        synchronized (queue)
        {
            int len = b.length;
            while (len >= DevInput.SIZE) {
                input = queue.removeFirst();
                Memory.writed(b, len, (int)(input.time / 1000)); // seconds
                Memory.writed(b, len+4, (int)(input.time % 1000)*1000); // microseconds
                Memory.writew(b, len+8, input.type);
                Memory.writew(b, len+10, input.code);
                Memory.writed(b, len+12, input.value);
                len += DevInput.SIZE;
            }
        }
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

    public int ioctl(WineThread thread, int request, Syscall.SyscallGetter getter) {
        switch (request & 0xFFFF) {
            case 0x4502: { // EVIOCGID
//                struct input_id {
//                    __u16 bustype;
//                    __u16 vendor;
//                    __u16 product;
//                    __u16 version;
//                };
                int len = (request & 0x1fff0000) >> 16;
                if (len!=8)
                    Log.panic("Bad length for EVIOCGID: "+len);
                int buffer = getter.next();
                thread.process.memory.writew(buffer, busType);
                thread.process.memory.writew(buffer+2, vendor);
                thread.process.memory.writew(buffer+4, product);
                thread.process.memory.writew(buffer+6, version);
                return 0;
            }
            case 0x4506: { // EVIOCGNAME
                int len = (request & 0x1fff0000) >> 16;
                int buffer = getter.next();
                thread.process.memory.writeCString(buffer, name, len);
                return Math.min(len, name.length());
            }
            case 0x4520: { // EVIOCGBIT
                int len = (request & 0x1fff0000) >> 16;
                int buffer = getter.next();
                if (len<4)
                    Log.panic("Bad length for EVIOCGBIT");
                thread.process.memory.writed(buffer, mask);
                return 4;
            }
            case 0x540B: // TCFLSH
                return 0;

        }
        return -1;
    }

    public int map(Memory memory, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        return 0;
    }

    public boolean isReadReady() {
        return queue.size()>0;
    }

    public boolean isWriteReady() {
        return true;
    }

    //    struct input_absinfo {
    //        __s32 value;
    //        __s32 minimum;
    //        __s32 maximum;
    //        __s32 fuzz;
    //        __s32 flat;
    //        __s32 resolution;
    //    };
    public void writeAbs(WineThread thread, int address, int value, int min, int max) {
        Memory memory = thread.process.memory;
        memory.writed(address, value);
        memory.writed(address+4, min);
        memory.writed(address+8, max);
        memory.writed(address+12, 0);
        memory.writed(address+16, 0);
        memory.writed(address+20, 96);
    }

    public void writeBit(WineThread thread, int address, int bit) {
        int b = bit/8;
        int p = bit % 8;
        int value = thread.process.memory.readb(address+b);
        value|=(1<<p);
        thread.process.memory.writeb(address+b, value);
    }

    static public final int EV_SYN                = 0x00;
    static public final int EV_KEY                = 0x01;
    static public final int EV_REL                = 0x02;
    static public final int EV_ABS                = 0x03;
    static public final int EV_MSC                = 0x04;
    static public final int EV_SW                 = 0x05;
    static public final int EV_LED                = 0x11;
    static public final int EV_SND                = 0x12;
    static public final int EV_REP                = 0x14;
    static public final int EV_FF                 = 0x15;
    static public final int EV_PWR                = 0x16;
    static public final int EV_FF_STATUS          = 0x17;
    static public final int EV_MAX                = 0x1f;

    static public final int ABS_X                 = 0x00;
    static public final int ABS_Y                 = 0x01;

    static public final int REL_X                 = 0x00;
    static public final int REL_Y                 = 0x01;

    static public final int BTN_MOUSE              = 0x110;
    static public final int BTN_LEFT               = 0x110;
    static public final int BTN_RIGHT              = 0x111;
    static public final int BTN_MIDDLE             = 0x112;
    static public final int BTN_SIDE               = 0x113;
    static public final int BTN_EXTRA              = 0x114;
    static public final int BTN_FORWARD            = 0x115;
    static public final int BTN_BACK               = 0x116;
    static public final int BTN_TASK               = 0x117;
}

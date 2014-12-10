package wine.system.io;

import wine.system.kernel.Syscall;
import wine.emulation.Memory;
import wine.system.WineThread;
import wine.util.Log;

public class DevTTY implements FSNodeAccess {
    static public int active = 0;

    static class termios {
        int c_iflag;               /* input mode flags */
        int c_oflag;               /* output mode flags */
        int c_cflag;               /* control mode flags */
        int c_lflag;               /* local mode flags */
        int c_line;                    /* line discipline */
        int[] c_cc=new int[19];                /* control characters */

        public void read(Memory memory, int address) {
            c_iflag = memory.readd(address);
            c_oflag = memory.readd(address+4);
            c_cflag = memory.readd(address+8);
            c_lflag = memory.readd(address+12);
            c_line = memory.readb(address+16);
            for (int i=0;i<c_cc.length;i++) {
                c_cc[i] = memory.readb(address+17+i);
            }
        }

        public void write(Memory memory, int address) {
            memory.writed(address, c_iflag);
            memory.writed(address+4, c_oflag);
            memory.writed(address+8, c_cflag);
            memory.writed(address+12, c_lflag);
            memory.writeb(address+16, c_line);
            for (int i=0;i<c_cc.length;i++) {
                memory.writeb(address+17+i, c_cc[i]);
            }
        }
    }

    static public final int VT_AUTO = 0;
    static public final int VT_PROCESS = 1;
    static public final int VT_ACKACQ = 2;

    public int mode = VT_AUTO;

    static public final int K_RAW = 0x00;
    static public final int K_XLATE = 0x01;
    static public final int K_MEDIUMRAW = 0x02;
    static public final int K_UNICODE = 0x03;
    static public final int KDSKBMODE = 0x04;

    public int kbMode = K_UNICODE;

    public int waitv;
    public int relsig;
    public int acqsig;
    public boolean graphics = false;
    public termios termios = new termios();

    public DevTTY(int index) {
    }

    public boolean setLength(long length) {
        return false;
    }

    public boolean open(String mode) {
        return true;
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
        StringBuilder builder = new StringBuilder();
        for (int i=0;i<b.length;i++) {
            builder.append((char) (b[i] & 0xFF));
        }
        WineThread.getCurrent().out(builder.toString());
        return true;
    }

    public void close() {
    }

    public int ioctl(int request, Syscall.SyscallGetter getter) {
        Memory memory = WineThread.getCurrent().process.memory;
        switch (request) {
            case 0x4B3A: // KDSETMODE
                graphics = getter.next()==1;
                break;
            case 0x4B44: // KDGKBMODE
                memory.writed(getter.next(), kbMode);
                break;
            case 0x4B45: // KDSKBMODE
                kbMode = getter.next();
                break;
            case 0x4B51: // KDSKBMUTE
                return -1;
            case 0x5401: // TCGETS
                termios.write(memory, getter.next());
                break;
            case 0x5402: // TCSETS
                termios.read(memory, getter.next());
                break;
            case 0x5600: // VT_OPENQRY
                memory.writed(getter.next(), 2);
                break;
            case 0x5601: { // VT_GETMODE
                int address = getter.next();
                memory.writeb(address, mode);
                memory.writeb(address+1, waitv); // waitv
                memory.writew(address+2, relsig); // relsig
                memory.writew(address+4, acqsig); // acqsig
                memory.writew(address+6, 0); // frsig
                break;
            }
            case 0x5602: { // VT_SETMODE
                int address = getter.next();
                mode = memory.readb(address); // VT_AUTO
                waitv = memory.readb(address+1); // waitv
                relsig = memory.readw(address+2); // relsig
                acqsig = memory.readw(address+4); // acqsig
                break;
            }
            case 0x5603: { // VT_GETSTATE
                int address = getter.next();
                memory.writew(address, 0); // v_active
                memory.writew(address, 0); // v_signal
                memory.writew(address, 1); // v_state
                break;
            }
            case 0x5606: // VT_ACTIVATE
                active = getter.next();
                break;
            case 0x5607: { // VT_WAITACTIVE
                int id = getter.next();
                if (id!=active)
                    Log.panic("VT_WAITACTIVE not fully implemented");
                break;
            }
            default:
                return -1;
        }
        return 0;
    }

    public int map(Memory memory, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        Log.panic("Mapping /dev/tty not supported");
        return -1;
    }
}

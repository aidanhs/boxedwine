package wine.system.io;

import wine.emulation.Memory;

public class Termios {
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

package wine.system.io;

import wine.builtin.libc.Syscall;
import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.util.Log;

public class DevFB implements FSNodeAccess {
    private int pos = 0;

    public DevFB(int cx, int cy, int bpp, int lineLength, int address, int len) {
        fb_fix_screeninfo.visual = 2; // FB_VISUAL_TRUECOLOR
        fb_fix_screeninfo.type = 0; // FB_TYPE_PACKED_PIXELS
        fb_fix_screeninfo.line_length = lineLength;
        fb_fix_screeninfo.mmio_start = 0;
        fb_fix_screeninfo.mmio_len = 0;
        fb_fix_screeninfo.smem_start = address;
        fb_fix_screeninfo.smem_len = len;

        fb_var_screeninfo.xres = cx;
        fb_var_screeninfo.yres = cy;
        fb_var_screeninfo.xres_virtual = cx;
        fb_var_screeninfo.yres_virtual = cy*2;
    }
    public fb_fix_screeninfo fb_fix_screeninfo = new fb_fix_screeninfo();
    static public class fb_fix_screeninfo {
        byte[] id = new byte[16];       /* identification string eg "TT Builtin" */
        int smem_start;                 /* Start of frame buffer mem */
        /* (physical address) */
        public int smem_len;            /* Length of frame buffer mem */
        public int type;                /* see FB_TYPE_*                */
        public int type_aux;            /* Interleave for interleaved Planes */
        public int visual;              /* see FB_VISUAL_*              */
        public int xpanstep;            /* zero if no hardware panning  */
        public int ypanstep;            /* zero if no hardware panning  */
        public int ywrapstep;           /* zero if no hardware ywrap    */
        public int line_length;         /* length of a line in bytes    */
        public int mmio_start;          /* Start of Memory Mapped I/O   */
        /* (physical address) */
        public int mmio_len;            /* Length of Memory Mapped I/O  */
        public int accel;               /* Indicate to driver which     */
                                        /*  specific chip/card we have  */
        public int  capabilities;       /* see FB_CAP_*                 */
        public int reserved;            /* Reserved for future compatibility */

        public void write(Memory memory, int address) {
            memory.memcpy(address, id, 0, id.length); address+=16;
            memory.writed(address, smem_start); address+=4;
            memory.writed(address, smem_len); address+=4;
            memory.writed(address, type); address+=4;
            memory.writed(address, type_aux); address+=4;
            memory.writed(address, visual); address+=4;
            memory.writew(address, xpanstep); address+=2;
            memory.writew(address, ypanstep); address+=2;
            memory.writew(address, ywrapstep); address+=2;
            memory.writed(address, line_length); address+=4;
            memory.writed(address, mmio_start); address+=4;
            memory.writed(address, mmio_len); address+=4;
            memory.writed(address, accel); address+=4;
            memory.writew(address, capabilities); address+=2;
        }
    }

    private fb_var_screeninfo fb_var_screeninfo = new fb_var_screeninfo();
    static private class fb_var_screeninfo {
        public int xres;                     /* visible resolution           */
        public int yres;
        public int xres_virtual;             /* virtual resolution           */
        public int yres_virtual;
        public int xoffset;                  /* offset from virtual to visible */
        public int yoffset;                  /* resolution                   */

        public int bits_per_pixel = 32;          /* guess what                   */
        public int grayscale = 0;                /* 0 = color, 1 = grayscale,    */
                                        /* >1 = FOURCC                  */
        public int red_offset=0;
        public int red_len=8;
        public int red_msb=0;
        public int green_offset=8;
        public int green_len=8;
        public int green_msb=0;
        public int blue_offset=16;
        public int blue_len=8;
        public int blue_msb=0;
        public int trans_offset=24;
        public int trans_len=8;
        public int trans_msb=0;

        public int nonstd = 0;               /* != 0 Non standard pixel format */

        public int activate;                 /* see FB_ACTIVATE_*            */

        public int height=300;                   /* height of picture in mm    */
        public int width=400;                    /* width of picture in mm     */

        public int accel_flags;              /* (OBSOLETE) see fb_info.flags */

        /* Timing: All values in pixclocks, except pixclock (of course) */
        public int pixclock;                 /* pixel clock in ps (pico seconds) */
        public int left_margin;              /* time from sync to picture    */
        public int right_margin;             /* time from picture to sync    */
        public int upper_margin;             /* time from sync to picture    */
        public int lower_margin;
        public int hsync_len;                /* length of horizontal sync    */
        public int vsync_len;                /* length of vertical sync      */
        public int sync;                     /* see FB_SYNC_*                */
        public int vmode;                    /* see FB_VMODE_*               */
        public int rotate;                   /* angle we rotate counter clockwise */
        public int colorspace;               /* colorspace for FOURCC-based modes */
        public int[] reserved = new int[4];  /* Reserved for future compatibility */

        public void write(Memory memory, int address) {
            memory.writed(address, xres); address+=4;
            memory.writed(address, yres); address+=4;
            memory.writed(address, xres_virtual); address+=4;
            memory.writed(address, yres_virtual); address+=4;
            memory.writed(address, xoffset); address+=4;
            memory.writed(address, yoffset); address+=4;

            memory.writed(address, bits_per_pixel); address+=4;
            memory.writed(address, grayscale); address+=4;

            memory.writed(address, red_offset); address+=4;
            memory.writed(address, red_len); address+=4;
            memory.writed(address, red_msb); address+=4;

            memory.writed(address, green_offset); address+=4;
            memory.writed(address, green_len); address+=4;
            memory.writed(address, green_msb); address+=4;

            memory.writed(address, blue_offset); address+=4;
            memory.writed(address, blue_len); address+=4;
            memory.writed(address, blue_msb); address+=4;

            memory.writed(address, trans_offset); address+=4;
            memory.writed(address, trans_len); address+=4;
            memory.writed(address, trans_msb); address+=4;

            memory.writed(address, nonstd); address+=4;
            memory.writed(address, activate); address+=4;
            memory.writed(address, height); address+=4;
            memory.writed(address, width); address+=4;
            memory.writed(address, accel_flags); address+=4;

            memory.writed(address, pixclock); address+=4;
            memory.writed(address, left_margin); address+=4;
            memory.writed(address, right_margin); address+=4;
            memory.writed(address, upper_margin); address+=4;
            memory.writed(address, lower_margin); address+=4;
            memory.writed(address, hsync_len); address+=4;
            memory.writed(address, vsync_len); address+=4;
            memory.writed(address, sync); address+=4;
            memory.writed(address, vmode); address+=4;
            memory.writed(address, rotate); address+=4;
            memory.writed(address, colorspace); address+=4;
            memory.zero(address, 16);
        }

        public void read(Memory memory, int address) {
            xres = memory.readd(address); address+=4;
            yres = memory.readd(address); address+=4;
            xres_virtual = memory.readd(address); address+=4;
            yres_virtual = memory.readd(address); address+=4;
            xoffset = memory.readd(address); address+=4;
            yoffset = memory.readd(address); address+=4;

            bits_per_pixel = memory.readd(address); address+=4;
            grayscale = memory.readd(address); address+=4;

            red_offset = memory.readd(address); address+=4;
            red_len = memory.readd(address); address+=4;
            red_msb = memory.readd(address); address+=4;

            green_offset = memory.readd(address); address+=4;
            green_len = memory.readd(address); address+=4;
            green_msb = memory.readd(address); address+=4;

            blue_offset = memory.readd(address); address+=4;
            blue_len = memory.readd(address); address+=4;
            blue_msb = memory.readd(address); address+=4;

            trans_offset = memory.readd(address); address+=4;
            trans_len = memory.readd(address); address+=4;
            trans_msb = memory.readd(address); address+=4;

            nonstd = memory.readd(address); address+=4;
            activate = memory.readd(address); address+=4;
            height = memory.readd(address); address+=4;
            width = memory.readd(address); address+=4;
            accel_flags = memory.readd(address); address+=4;

            pixclock = memory.readd(address); address+=4;
            left_margin = memory.readd(address); address+=4;
            right_margin = memory.readd(address); address+=4;
            upper_margin = memory.readd(address); address+=4;
            lower_margin = memory.readd(address); address+=4;
            hsync_len = memory.readd(address); address+=4;
            vsync_len = memory.readd(address); address+=4;
            sync = memory.readd(address); address+=4;
            vmode = memory.readd(address); address+=4;
            rotate = memory.readd(address); address+=4;
            colorspace = memory.readd(address); address+=4;
        }
    }

    public boolean setLength(long length) {
        return false;
    }

    static int i;
    public boolean open(String mode) {
        i++;
        if (i==4) {
            WineThread.getCurrent().cpu.log = true;
            WineThread.getCurrent().cpu.callIndex = 5;
        }
        return true;
    }
    public long getFilePointer() {
        return pos;
    }

    public boolean seek(long pos) {
        if (pos>=fb_fix_screeninfo.smem_len)
            return false;
        this.pos = (int)pos;
        return true;
    }

    public int read(byte[] b) {
        int len = b.length;
        if (len+pos>fb_fix_screeninfo.smem_len)
            len=fb_fix_screeninfo.smem_len-pos;
        if (len<=0)
            return 0;
        WineThread.getCurrent().process.memory.memcpy(b, 0, len, WineProcess.ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS+pos);
        pos+=len;
        return len;
    }

    public boolean write(byte[] b) {
        int len = b.length;
        if (len+pos>fb_fix_screeninfo.smem_len)
            len=fb_fix_screeninfo.smem_len-pos;
        if (len<=0)
            return false;
        WineThread.getCurrent().process.memory.memcpy(WineProcess.ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS+pos, b, 0, b.length);
        pos+=len;
        return true;
    }

    public void close() {
    }

    public int ioctl(int request, Syscall.SyscallGetter getter) {
        Memory memory = WineThread.getCurrent().process.memory;

        switch(request) {
            case 0x4600: // FBIOGET_VSCREENINFO
                fb_var_screeninfo.write(memory, getter.next());
                break;
            case 0x4601: // FBIOPUT_VSCREENINFO
                fb_var_screeninfo.read(memory, getter.next());
                break;
            case 0x4602: // FBIOGET_FSCREENINFO
                fb_fix_screeninfo.write(memory, getter.next());
                break;
            case 0x4605: // FBIOPUTCMAP
                break;
            case 0x4606: // FBIOPAN_DISPLAY
                fb_var_screeninfo.read(memory, getter.next());
                if (fb_var_screeninfo.yoffset!=0) {
                    int ii=0;
                }
                break;
            case 0x4611: // FBIOBLANK
                break;
            default:
                return -1;
        }
        return 0;
    }

    public int map(Memory memory, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        int pageStart = address>>>12;
        int pageCount = (int)((len+0xFFF)>>>12);
        if (!fixed)
            return WineProcess.ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS;
        Log.panic("Mapping /dev/fb at fixed address not supported");
        return -1;
    }
}

package wine.system.io;

import wine.emulation.Memory;
import wine.system.WineThread;
import wine.system.kernel.*;
import wine.system.kernel.Process;
import wine.util.Log;

import java.awt.event.MouseEvent;
import java.util.LinkedList;

public class DevInput implements FSNodeAccess {
    private int mask;
    private String name;
    protected int busType;
    protected int vendor;
    protected int product;
    protected int version;
    protected Process asyncProcess;

    public DevInput(int mask, String name) {
        this.mask = mask;
        this.name = name;
    }
    static class Data {
        static public final int SIZE = 16;

        public Data(int type, int code, int value) {
            this.time = System.currentTimeMillis();
            this.type = type;
            this.code = code;
            this.value = value;
        }

        public long time; // timeval 8 bytes on 32-bit systems (16 bytes on 64-bit systems)
        public int type; // u16
        public int code; // u16
        public int value; // s32
    }

    private LinkedList<Data> queue = new LinkedList<Data>();

    public void mouseDown(int button) {
        if (button == MouseEvent.BUTTON1)
            event(EV_KEY, BTN_LEFT, 1);
        else if (button == MouseEvent.BUTTON2)
            event(EV_KEY, BTN_MIDDLE, 1);
        else if (button == MouseEvent.BUTTON3)
            event(EV_KEY, BTN_RIGHT, 1);
        else
            return;
        event(EV_SYN, SYN_REPORT, 0);
        if (asyncProcess != null) {
            asyncProcess.signal(Signal.SIGIO);
        }
    }

    public void mouseUp(int button) {
        if (button == MouseEvent.BUTTON1)
            event(EV_KEY, BTN_LEFT, 0);
        else if (button == MouseEvent.BUTTON2)
            event(EV_KEY, BTN_MIDDLE, 0);
        else if (button == MouseEvent.BUTTON3)
            event(EV_KEY, BTN_RIGHT, 0);
        else
            return;
        event(EV_SYN, SYN_REPORT, 0);
        if (asyncProcess != null) {
            asyncProcess.signal(Signal.SIGIO);
        }
    }

    protected void event(int type, int code, int value) {
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
        int len = 0;
        int room = b.length;

        synchronized (queue)
        {
            while (room >= Data.SIZE && queue.size()>0) {
                input = queue.removeFirst();
                Memory.writed(b, len, (int) (input.time / 1000)); // seconds
                Memory.writed(b, len + 4, (int) (input.time % 1000) * 1000); // microseconds
                Memory.writew(b, len+8, input.type);
                Memory.writew(b, len+10, input.code);
                Memory.writed(b, len+12, input.value);
                len += Data.SIZE;
                room -= Data.SIZE;
            }
        }
        return len;
    }

    public boolean write(byte[] b) {
        return false;
    }

    public void close() {
    }

    public boolean open(String mode) {
        queue.clear();
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

    public void setAsync(Process process, boolean remove) {
        if (remove) {
            if (asyncProcess == process) {
                asyncProcess = null;
            }
        } else {
            if (asyncProcess != null) {
                Log.panic("More than one process opened /dev/input for async io.  This is currently not supported");
            }
            asyncProcess = process;
        }
    }

    public boolean isAsync(Process process) {
        return asyncProcess==process;
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

    static public final int SYN_REPORT            = 0;
    static public final int SYN_CONFIG            = 1;
    static public final int SYN_MT_REPORT         = 2;
    static public final int SYN_DROPPED           = 3;

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

    static public final int KEY_RESERVED           = 0;
    static public final int KEY_ESC                = 1;
    static public final int KEY_1                  = 2;
    static public final int KEY_2                  = 3;
    static public final int KEY_3                  = 4;
    static public final int KEY_4                  = 5;
    static public final int KEY_5                  = 6;
    static public final int KEY_6                  = 7;
    static public final int KEY_7                  = 8;
    static public final int KEY_8                  = 9;
    static public final int KEY_9                  = 10;
    static public final int KEY_0                  = 11;
    static public final int KEY_MINUS              = 12;
    static public final int KEY_EQUAL              = 13;
    static public final int KEY_BACKSPACE          = 14;
    static public final int KEY_TAB                = 15;
    static public final int KEY_Q                  = 16;
    static public final int KEY_W                  = 17;
    static public final int KEY_E                  = 18;
    static public final int KEY_R                  = 19;
    static public final int KEY_T                  = 20;
    static public final int KEY_Y                  = 21;
    static public final int KEY_U                  = 22;
    static public final int KEY_I                  = 23;
    static public final int KEY_O                  = 24;
    static public final int KEY_P                  = 25;
    static public final int KEY_LEFTBRACE          = 26;
    static public final int KEY_RIGHTBRACE         = 27;
    static public final int KEY_ENTER              = 28;
    static public final int KEY_LEFTCTRL           = 29;
    static public final int KEY_A                  = 30;
    static public final int KEY_S                  = 31;
    static public final int KEY_D                  = 32;
    static public final int KEY_F                  = 33;
    static public final int KEY_G                  = 34;
    static public final int KEY_H                  = 35;
    static public final int KEY_J                  = 36;
    static public final int KEY_K                  = 37;
    static public final int KEY_L                  = 38;
    static public final int KEY_SEMICOLON          = 39;
    static public final int KEY_APOSTROPHE         = 40;
    static public final int KEY_GRAVE              = 41;
    static public final int KEY_LEFTSHIFT          = 42;
    static public final int KEY_BACKSLASH          = 43;
    static public final int KEY_Z                  = 44;
    static public final int KEY_X                  = 45;
    static public final int KEY_C                  = 46;
    static public final int KEY_V                  = 47;
    static public final int KEY_B                  = 48;
    static public final int KEY_N                  = 49;
    static public final int KEY_M                  = 50;
    static public final int KEY_COMMA              = 51;
    static public final int KEY_DOT                = 52;
    static public final int KEY_SLASH              = 53;
    static public final int KEY_RIGHTSHIFT         = 54;
    static public final int KEY_KPASTERISK         = 55;
    static public final int KEY_LEFTALT            = 56;
    static public final int KEY_SPACE              = 57;
    static public final int KEY_CAPSLOCK           = 58;
    static public final int KEY_F1                 = 59;
    static public final int KEY_F2                 = 60;
    static public final int KEY_F3                 = 61;
    static public final int KEY_F4                 = 62;
    static public final int KEY_F5                 = 63;
    static public final int KEY_F6                 = 64;
    static public final int KEY_F7                 = 65;
    static public final int KEY_F8                 = 66;
    static public final int KEY_F9                 = 67;
    static public final int KEY_F10                = 68;
    static public final int KEY_NUMLOCK            = 69;
    static public final int KEY_SCROLLLOCK         = 70;
    static public final int KEY_KP7                = 71;
    static public final int KEY_KP8                = 72;
    static public final int KEY_KP9                = 73;
    static public final int KEY_KPMINUS            = 74;
    static public final int KEY_KP4                = 75;
    static public final int KEY_KP5                = 76;
    static public final int KEY_KP6                = 77;
    static public final int KEY_KPPLUS             = 78;
    static public final int KEY_KP1                = 79;
    static public final int KEY_KP2                = 80;
    static public final int KEY_KP3                = 81;
    static public final int KEY_KP0                = 82;
    static public final int KEY_KPDOT              = 83;

    static public final int KEY_ZENKAKUHANKAKU     = 85;
    static public final int KEY_102ND              = 86;
    static public final int KEY_F11                = 87;

    static public final int KEY_F12                = 88;
    static public final int KEY_RO                 = 89;
    static public final int KEY_KATAKANA           = 90;
    static public final int KEY_HIRAGANA           = 91;
    static public final int KEY_HENKAN             = 92;
    static public final int KEY_KATAKANAHIRAGANA   = 93;
    static public final int KEY_MUHENKAN           = 94;
    static public final int KEY_KPJPCOMMA          = 95;
    static public final int KEY_KPENTER            = 96;
    static public final int KEY_RIGHTCTRL          = 97;
    static public final int KEY_KPSLASH            = 98;
    static public final int KEY_SYSRQ              = 99;
    static public final int KEY_RIGHTALT           = 100;
    static public final int KEY_LINEFEED           = 101;
    static public final int KEY_HOME               = 102;
    static public final int KEY_UP                 = 103;
    static public final int KEY_PAGEUP             = 104;
    static public final int KEY_LEFT               = 105;
    static public final int KEY_RIGHT              = 106;
    static public final int KEY_END                = 107;
    static public final int KEY_DOWN               = 108;
    static public final int KEY_PAGEDOWN           = 109;
    static public final int KEY_INSERT             = 110;
    static public final int KEY_DELETE             = 111;
    static public final int KEY_MACRO              = 112;
    static public final int KEY_MUTE               = 113;
    static public final int KEY_VOLUMEDOWN         = 114;
    static public final int KEY_VOLUMEUP           = 115;
    static public final int KEY_POWER              = 116;     /* SC System Power Down */
    static public final int KEY_KPEQUAL            = 117;
    static public final int KEY_KPPLUSMINUS        = 118;
    static public final int KEY_PAUSE              = 119;
    static public final int KEY_SCALE              = 120;     /* AL Compiz Scale (Expose) */

    static public final int KEY_KPCOMMA            = 121;
    static public final int KEY_HANGEUL            = 122;
    static public final int KEY_HANGUEL            = KEY_HANGEUL;
    static public final int KEY_HANJA              = 123;
    static public final int KEY_YEN                = 124;
    static public final int KEY_LEFTMETA           = 125;
    static public final int KEY_RIGHTMETA          = 126;
    static public final int KEY_COMPOSE            = 127;

    static public final int KEY_STOP               = 128;     /* AC Stop */
    static public final int KEY_AGAIN              = 129;

    static public final int KEY_PROPS              = 130;     /* AC Properties */
    static public final int KEY_UNDO               = 131;     /* AC Undo */
    static public final int KEY_FRONT              = 132;
    static public final int KEY_COPY               = 133;     /* AC Copy */
    static public final int KEY_OPEN               = 134;     /* AC Open */
    static public final int KEY_PASTE              = 135;     /* AC Paste */
    static public final int KEY_FIND               = 136;     /* AC Search */
    static public final int KEY_CUT                = 137;     /* AC Cut */
    static public final int KEY_HELP               = 138;     /* AL Integrated Help Center */
    static public final int KEY_MENU               = 139;     /* Menu (show menu) */
    static public final int KEY_CALC               = 140;     /* AL Calculator */
    static public final int KEY_SETUP              = 141;
    static public final int KEY_SLEEP              = 142;     /* SC System Sleep */
    static public final int KEY_WAKEUP             = 143;     /* System Wake Up */
    static public final int KEY_FILE               = 144;     /* AL Local Machine Browser */
    static public final int KEY_SENDFILE           = 145;
    static public final int KEY_DELETEFILE         = 146;
    static public final int KEY_XFER               = 147;
    static public final int KEY_PROG1              = 148;
    static public final int KEY_PROG2              = 149;
    static public final int KEY_WWW                = 150;     /* AL Internet Browser */
    static public final int KEY_MSDOS              = 151;
    static public final int KEY_COFFEE             = 152;     /* AL Terminal Lock/Screensaver */
    static public final int KEY_SCREENLOCK         = KEY_COFFEE;
    static public final int KEY_DIRECTION          = 153;
    static public final int KEY_CYCLEWINDOWS       = 154;
    static public final int KEY_MAIL               = 155;
    static public final int KEY_BOOKMARKS          = 156;     /* AC Bookmarks */
    static public final int KEY_COMPUTER           = 157;
    static public final int KEY_BACK               = 158;     /* AC Back */
    static public final int KEY_FORWARD            = 159;     /* AC Forward */
    static public final int KEY_CLOSECD            = 160;
    static public final int KEY_EJECTCD            = 161;
    static public final int KEY_EJECTCLOSECD       = 162;
    static public final int KEY_NEXTSONG           = 163;
    static public final int KEY_PLAYPAUSE          = 164;
    static public final int KEY_PREVIOUSSONG       = 165;
    static public final int KEY_STOPCD             = 166;
    static public final int KEY_RECORD             = 167;
    static public final int KEY_REWIND             = 168;
    static public final int KEY_PHONE              = 169;     /* Media Select Telephone */
    static public final int KEY_ISO                = 170;
    static public final int KEY_CONFIG             = 171;     /* AL Consumer Control Configuration */
    static public final int KEY_HOMEPAGE           = 172;     /* AC Home */
    static public final int KEY_REFRESH            = 173;     /* AC Refresh */
    static public final int KEY_EXIT               = 174;     /* AC Exit */
    static public final int KEY_MOVE               = 175;
    static public final int KEY_EDIT               = 176;
    static public final int KEY_SCROLLUP           = 177;
    static public final int KEY_SCROLLDOWN         = 178;
    static public final int KEY_KPLEFTPAREN        = 179;
    static public final int KEY_KPRIGHTPAREN       = 180;
    static public final int KEY_NEW                = 181;     /* AC New */
    static public final int KEY_REDO               = 182;     /* AC Redo/Repeat */

    static public final int KEY_F13                = 183;
    static public final int KEY_F14                = 184;
    static public final int KEY_F15                = 185;
    static public final int KEY_F16                = 186;
    static public final int KEY_F17                = 187;
    static public final int KEY_F18                = 188;
    static public final int KEY_F19                = 189;
    static public final int KEY_F20                = 190;
    static public final int KEY_F21                = 191;
    static public final int KEY_F22                = 192;
    static public final int KEY_F23                = 193;
    static public final int KEY_F24                = 194;

    static public final int KEY_PLAYCD             = 200;
    static public final int KEY_PAUSECD            = 201;
    static public final int KEY_PROG3              = 202;
    static public final int KEY_PROG4              = 203;
    static public final int KEY_DASHBOARD          = 204;     /* AL Dashboard */
    static public final int KEY_SUSPEND            = 205;
    static public final int KEY_CLOSE              = 206;     /* AC Close */
    static public final int KEY_PLAY               = 207;
    static public final int KEY_FASTFORWARD        = 208;
    static public final int KEY_BASSBOOST          = 209;
    static public final int KEY_PRINT              = 210;     /* AC Print */
    static public final int KEY_HP                 = 211;
    static public final int KEY_CAMERA             = 212;
    static public final int KEY_SOUND              = 213;
    static public final int KEY_QUESTION           = 214;
    static public final int KEY_EMAIL              = 215;
    static public final int KEY_CHAT               = 216;
    static public final int KEY_SEARCH             = 217;
    static public final int KEY_CONNECT            = 218;
    static public final int KEY_FINANCE            = 219;     /* AL Checkbook/Finance */
    static public final int KEY_SPORT              = 220;
    static public final int KEY_SHOP               = 221;
    static public final int KEY_ALTERASE           = 222;
    static public final int KEY_CANCEL             = 223;     /* AC Cancel */
    static public final int KEY_BRIGHTNESSDOWN     = 224;
    static public final int KEY_BRIGHTNESSUP       = 225;
    static public final int KEY_MEDIA              = 226;

    static public final int KEY_SWITCHVIDEOMODE    = 227;     /* Cycle between available video
                                   outputs (Monitor/LCD/TV-out/etc) */
    static public final int KEY_KBDILLUMTOGGLE     = 228;
    static public final int KEY_KBDILLUMDOWN       = 229;
    static public final int KEY_KBDILLUMUP         = 230;

    static public final int KEY_SEND               = 231;     /* AC Send */
    static public final int KEY_REPLY              = 232;     /* AC Reply */
    static public final int KEY_FORWARDMAIL        = 233;     /* AC Forward Msg */
    static public final int KEY_SAVE               = 234;     /* AC Save */
    static public final int KEY_DOCUMENTS          = 235;

    static public final int KEY_BATTERY            = 236;

    static public final int KEY_BLUETOOTH          = 237;
    static public final int KEY_WLAN               = 238;
    static public final int KEY_UWB                = 239;

    static public final int KEY_UNKNOWN            = 240;

    static public final int KEY_VIDEO_NEXT         = 241;     /* drive next video source */
    static public final int KEY_VIDEO_PREV         = 242;     /* drive previous video source */
    static public final int KEY_BRIGHTNESS_CYCLE   = 243;     /* brightness up, after max is min */
    static public final int KEY_BRIGHTNESS_ZERO    = 244;     /* brightness off, use ambient */
    static public final int KEY_DISPLAY_OFF        = 245;     /* display device to off state */

    static public final int KEY_WIMAX              = 246;
    static public final int KEY_RFKILL             = 247;     /* Key that controls all radios */

    static public final int KEY_MICMUTE            = 248;     /* Mute / unmute the microphone */

}

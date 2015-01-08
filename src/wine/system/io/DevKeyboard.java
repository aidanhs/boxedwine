package wine.system.io;

import wine.system.WineThread;
import wine.system.kernel.Signal;
import wine.system.kernel.Syscall;
import wine.util.Log;

import java.awt.event.KeyEvent;

public class DevKeyboard extends DevInput {
    static private boolean log = false;

    public DevKeyboard() {
        super((1<<EV_SYN)|(1<<EV_KEY), "BoxedWine Keyboard");
        busType = 0x11;
        product = 1;
        vendor = 1;
        version = 0xab41;
    }

    private int translateKey(KeyEvent key) {
        switch (key.getKeyCode()) {
            case KeyEvent.VK_ESCAPE:
                return KEY_ESC;
            case KeyEvent.VK_1:
                return KEY_1;
            case KeyEvent.VK_2:
                return KEY_2;
            case KeyEvent.VK_3:
                return KEY_3;
            case KeyEvent.VK_4:
                return KEY_4;
            case KeyEvent.VK_5:
                return KEY_5;
            case KeyEvent.VK_6:
                return KEY_6;
            case KeyEvent.VK_7:
                return KEY_7;
            case KeyEvent.VK_8:
                return KEY_8;
            case KeyEvent.VK_9:
                return KEY_9;
            case KeyEvent.VK_0:
                return KEY_0;
            case KeyEvent.VK_MINUS:
                return KEY_MINUS;
            case KeyEvent.VK_EQUALS:
                return KEY_EQUAL;
            case KeyEvent.VK_BACK_SPACE:
                return KEY_BACKSPACE;
            case KeyEvent.VK_TAB:
                return KEY_TAB;
            case KeyEvent.VK_Q:
                return KEY_Q;
            case KeyEvent.VK_W:
                return KEY_W;
            case KeyEvent.VK_E:
                return KEY_E;
            case KeyEvent.VK_R:
                return KEY_R;
            case KeyEvent.VK_T:
                return KEY_T;
            case KeyEvent.VK_Y:
                return KEY_Y;
            case KeyEvent.VK_U:
                return KEY_U;
            case KeyEvent.VK_I:
                return KEY_I;
            case KeyEvent.VK_O:
                return KEY_O;
            case KeyEvent.VK_P:
                return KEY_P;
            case KeyEvent.VK_OPEN_BRACKET:
                return KEY_LEFTBRACE;
            case KeyEvent.VK_CLOSE_BRACKET:
                return KEY_RIGHTBRACE;
            case KeyEvent.VK_ENTER:
                return KEY_ENTER;
            case KeyEvent.VK_CONTROL:
                if (key.getKeyLocation() == KeyEvent.KEY_LOCATION_LEFT)
                    return KEY_LEFTCTRL;
                return KEY_RIGHTCTRL;
            case KeyEvent.VK_A:
                return KEY_A;
            case KeyEvent.VK_S:
                return KEY_S;
            case KeyEvent.VK_D:
                return KEY_D;
            case KeyEvent.VK_F:
                return KEY_F;
            case KeyEvent.VK_G:
                return KEY_G;
            case KeyEvent.VK_H:
                return KEY_H;
            case KeyEvent.VK_J:
                return KEY_J;
            case KeyEvent.VK_K:
                return KEY_K;
            case KeyEvent.VK_L:
                return KEY_L;
            case KeyEvent.VK_SEMICOLON:
                return KEY_SEMICOLON;
            case KeyEvent.VK_QUOTE:
                return KEY_APOSTROPHE;
            case KeyEvent.VK_BACK_QUOTE:
                return KEY_GRAVE;
            case KeyEvent.VK_SHIFT:
                if (key.getKeyLocation() == KeyEvent.KEY_LOCATION_LEFT)
                    return KEY_LEFTSHIFT;
                return KEY_RIGHTSHIFT;
            case KeyEvent.VK_BACK_SLASH:
                return KEY_BACKSLASH;
            case KeyEvent.VK_Z:
                return KEY_Z;
            case KeyEvent.VK_X:
                return KEY_X;
            case KeyEvent.VK_C:
                return KEY_C;
            case KeyEvent.VK_V:
                return KEY_V;
            case KeyEvent.VK_B:
                return KEY_B;
            case KeyEvent.VK_N:
                return KEY_N;
            case KeyEvent.VK_M:
                return KEY_M;
            case KeyEvent.VK_COMMA:
                return KEY_COMMA;
            case KeyEvent.VK_PERIOD:
                return KEY_DOT;
            case KeyEvent.VK_SLASH:
                return KEY_SLASH;
            case KeyEvent.VK_ALT:
                if (key.getKeyLocation()==KeyEvent.KEY_LOCATION_LEFT)
                    return KEY_LEFTALT;
                return KEY_RIGHTALT;
            case KeyEvent.VK_SPACE:
                return KEY_SPACE;
            case KeyEvent.VK_CAPS_LOCK:
                return KEY_CAPSLOCK;
            case KeyEvent.VK_F1:
                return KEY_F1;
            case KeyEvent.VK_F2:
                return KEY_F2;
            case KeyEvent.VK_F3:
                return KEY_F3;
            case KeyEvent.VK_F4:
                return KEY_F4;
            case KeyEvent.VK_F5:
                return KEY_F5;
            case KeyEvent.VK_F6:
                return KEY_F6;
            case KeyEvent.VK_F7:
                return KEY_F7;
            case KeyEvent.VK_F8:
                return KEY_F8;
            case KeyEvent.VK_F9:
                return KEY_F9;
            case KeyEvent.VK_F10:
                return KEY_F10;
            case KeyEvent.VK_NUM_LOCK:
                return KEY_NUMLOCK;
            case KeyEvent.VK_SCROLL_LOCK:
                return KEY_SCROLLLOCK;
            case KeyEvent.VK_F11:
                return KEY_F11;
            case KeyEvent.VK_F12:
                return KEY_F12;
            case KeyEvent.VK_HOME:
                return KEY_HOME;
            case KeyEvent.VK_UP:
                return KEY_UP;
            case KeyEvent.VK_PAGE_UP:
                return KEY_PAGEUP;
            case KeyEvent.VK_LEFT:
                return KEY_LEFT;
            case KeyEvent.VK_RIGHT:
                return KEY_RIGHT;
            case KeyEvent.VK_END:
                return KEY_END;
            case KeyEvent.VK_DOWN:
                return KEY_DOWN;
            case KeyEvent.VK_PAGE_DOWN:
                return KEY_PAGEDOWN;
            case KeyEvent.VK_INSERT:
                return KEY_INSERT;
            case KeyEvent.VK_DELETE:
                return KEY_DELETE;
            case KeyEvent.VK_PAUSE:
                return KEY_PAUSE;
            default:
                Log.warn("Unhandled key: "+key.toString());
                return 0;
        }
    }
    public void keyDown(KeyEvent key) {
        int code = translateKey(key);
        if (log)
            System.out.println("keyDown: "+key+toString()+" ("+code+")");
        if (code == 0)
            return;
        super.event(EV_KEY, code, 1);
        super.event(EV_SYN, SYN_REPORT, 0);
        if (asyncProcess != null) {
            asyncProcess.signal(Signal.SIGIO);
        }
    }

    public void keyUp(KeyEvent key) {
        int code = translateKey(key);
        if (log)
            System.out.println("keyUp: "+key+toString()+" ("+code+")");
        if (code == 0)
            return;
        super.event(EV_KEY, code, 0);
        super.event(EV_SYN, SYN_REPORT, 0);
        if (asyncProcess != null) {
            asyncProcess.signal(Signal.SIGIO);
        }
    }

    public int ioctl(WineThread thread, int request, Syscall.SyscallGetter getter) {
        switch (request & 0xFFFF) {
            case 0x4521: { // EVIOCGBIT, EV_KEY
                int len = (request & 0x1fff0000) >> 16;
                int buffer = getter.next();
                thread.process.memory.zero(buffer, len);
                writeBit(thread, buffer, KEY_ESC);
                writeBit(thread, buffer, KEY_1);
                writeBit(thread, buffer, KEY_2);
                writeBit(thread, buffer, KEY_3);
                writeBit(thread, buffer, KEY_4);
                writeBit(thread, buffer, KEY_5);
                writeBit(thread, buffer, KEY_6);
                writeBit(thread, buffer, KEY_7);
                writeBit(thread, buffer, KEY_8);
                writeBit(thread, buffer, KEY_9);
                writeBit(thread, buffer, KEY_0);
                writeBit(thread, buffer, KEY_MINUS);
                writeBit(thread, buffer, KEY_EQUAL);
                writeBit(thread, buffer, KEY_BACKSPACE);
                writeBit(thread, buffer, KEY_TAB);
                writeBit(thread, buffer, KEY_Q);
                writeBit(thread, buffer, KEY_W);
                writeBit(thread, buffer, KEY_E);
                writeBit(thread, buffer, KEY_R);
                writeBit(thread, buffer, KEY_T);
                writeBit(thread, buffer, KEY_Y);
                writeBit(thread, buffer, KEY_U);
                writeBit(thread, buffer, KEY_I);
                writeBit(thread, buffer, KEY_O);
                writeBit(thread, buffer, KEY_P);
                writeBit(thread, buffer, KEY_LEFTBRACE);
                writeBit(thread, buffer, KEY_RIGHTBRACE);
                writeBit(thread, buffer, KEY_ENTER);
                writeBit(thread, buffer, KEY_LEFTCTRL);
                writeBit(thread, buffer, KEY_A);
                writeBit(thread, buffer, KEY_S);
                writeBit(thread, buffer, KEY_D);
                writeBit(thread, buffer, KEY_F);
                writeBit(thread, buffer, KEY_G);
                writeBit(thread, buffer, KEY_H);
                writeBit(thread, buffer, KEY_J);
                writeBit(thread, buffer, KEY_K);
                writeBit(thread, buffer, KEY_L);
                writeBit(thread, buffer, KEY_SEMICOLON);
                writeBit(thread, buffer, KEY_APOSTROPHE);
                writeBit(thread, buffer, KEY_GRAVE);
                writeBit(thread, buffer, KEY_LEFTSHIFT);
                writeBit(thread, buffer, KEY_BACKSLASH);
                writeBit(thread, buffer, KEY_Z);
                writeBit(thread, buffer, KEY_X);
                writeBit(thread, buffer, KEY_C);
                writeBit(thread, buffer, KEY_V);
                writeBit(thread, buffer, KEY_B);
                writeBit(thread, buffer, KEY_N);
                writeBit(thread, buffer, KEY_M);
                writeBit(thread, buffer, KEY_COMMA);
                writeBit(thread, buffer, KEY_DOT);
                writeBit(thread, buffer, KEY_SLASH);
                writeBit(thread, buffer, KEY_RIGHTSHIFT);
                writeBit(thread, buffer, KEY_LEFTALT);
                writeBit(thread, buffer, KEY_SPACE);
                writeBit(thread, buffer, KEY_CAPSLOCK);
                writeBit(thread, buffer, KEY_F1);
                writeBit(thread, buffer, KEY_F2);
                writeBit(thread, buffer, KEY_F3);
                writeBit(thread, buffer, KEY_F4);
                writeBit(thread, buffer, KEY_F5);
                writeBit(thread, buffer, KEY_F6);
                writeBit(thread, buffer, KEY_F7);
                writeBit(thread, buffer, KEY_F8);
                writeBit(thread, buffer, KEY_F9);
                writeBit(thread, buffer, KEY_F10);
                writeBit(thread, buffer, KEY_NUMLOCK);
                writeBit(thread, buffer, KEY_SCROLLLOCK);

                writeBit(thread, buffer, KEY_F11);
                writeBit(thread, buffer, KEY_F12);
                writeBit(thread, buffer, KEY_RIGHTCTRL);
                writeBit(thread, buffer, KEY_RIGHTALT);
                writeBit(thread, buffer, KEY_HOME);
                writeBit(thread, buffer, KEY_UP);
                writeBit(thread, buffer, KEY_PAGEUP);
                writeBit(thread, buffer, KEY_LEFT);
                writeBit(thread, buffer, KEY_RIGHT);
                writeBit(thread, buffer, KEY_END);
                writeBit(thread, buffer, KEY_DOWN);
                writeBit(thread, buffer, KEY_PAGEDOWN);
                writeBit(thread, buffer, KEY_INSERT);
                writeBit(thread, buffer, KEY_DELETE);
                writeBit(thread, buffer, KEY_PAUSE);
                return (KEY_PAUSE+7)/8;
            }
            case 0x4522: { // EVIOCGBIT, EV_REL
                int len = (request & 0x1fff0000) >> 16;
                int buffer = getter.next();
                thread.process.memory.zero(buffer, len);
                thread.process.memory.writeb(buffer, 0);
                return 1;
            }
            case 0x4523: { // EVIOCGBIT, EV_ABS
                int len = (request & 0x1fff0000) >> 16;
                int buffer = getter.next();
                thread.process.memory.zero(buffer, len);
                thread.process.memory.writeb(buffer, 0);
                return 1;
            }
            case 0x4531: { // EVIOCGBIT, EV_LED
                int len = (request & 0x1fff0000) >> 16;
                int buffer = getter.next();
                thread.process.memory.zero(buffer, len);
                return 1;
            }
            default:
                return super.ioctl(thread, request, getter);
        }
    }
}

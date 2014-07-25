package wine.builtin.libX11;

import java.awt.event.KeyEvent;
import java.util.Hashtable;

public class KeySym {
    static public int keyMin = 255;
    static public int keyMax = 0;
    static public int keySymsPerKeyCode = 2;

    static public final int LEFT_SHIFT_KEYCODE = 0x80;
    static public final int RIGHT_SHIFT_KEYCODE = 0x81;
    static public final int CAPS_LOCK_KEYCODE = 0x82;
    static public final int LEFT_CONTROL_KEYCODE = 0x83;
    static public final int RIGHT_CONTROL_KEYCODE = 0x84;
    static public final int LEFT_ALT_KEYCODE = 0x85;
    static public final int RIGHT_ALT_KEYCODE = 0x86;
    static public final int NUM_LOCK_KEYCODE = 0x87;
    static public final int SCROLL_LOCK_KEYCODE = 88;

    public KeySym(int jk, int keyCode, int keySym1, char char1, int keySym2, char char2, int location) {
        this.keySym = new int[] {keySym1, keySym2};
        this.symbol = new char[] {char1, char2};
        this.keyCode = keyCode;
        this.javaKeyCodeLocation = location;
        this.javaKeyCode = jk;
    }

    public int translate(int mods) {
        if (mods == -1) {
            mods = 0; // :TODO: detect keyboard state
        }
        if ((mods & 1)!=0)
            return keySym[1];
        return keySym[0];
    }

    public char getChar(int mods) {
        if (mods == -1) {
            mods = 0; // :TODO: detect keyboard state
        }
        if ((mods & 1)!=0)
            return symbol[1];
        return symbol[0];
    }

    public int keyCode;
    public int[] keySym;
    public char[] symbol;
    public int javaKeyCodeLocation;
    public int javaKeyCode;

    static public Hashtable<Integer, KeySym> x11KeyCodeToKeySym = new Hashtable<Integer, KeySym>();
    static public Hashtable<Integer, KeySym> x11KeySymToKeySym = new Hashtable<Integer, KeySym>();
    static public Hashtable<Integer, String> x11KeySymToString = new Hashtable<Integer, String>();

    static private char noChar = (char)0;

    static private void add(int jk, int code, int xk1, char c1, int xk2, char c2) {
        add(jk, code, xk1, c1, xk2, c2, KeyEvent.KEY_LOCATION_STANDARD);
    }

    static private void add(int jk, int code, int xk1, char c1, int xk2, char c2, int location) {
        if (xk1<keyMin)
            keyMin=xk1;
        if (xk1>keyMax)
            keyMax=xk1;
        KeySym keySym = new KeySym(jk, code, xk1, c1, xk2, c2, location);
        x11KeyCodeToKeySym.put(code, keySym);
        x11KeySymToKeySym.put(xk1, keySym);
        x11KeySymToKeySym.put(xk2, keySym);
        if (c1!=0)
            x11KeySymToString.put(xk1, String.valueOf(c1));
        if (c2!=0)
            x11KeySymToString.put(xk2, String.valueOf(c2));
    }

    static  {
        add(KeyEvent.VK_1, 0x31, 0x31, '1', 0x21, '!');
        add(KeyEvent.VK_2, 0x32, 0x32, '2', 0x40, '@');
        add(KeyEvent.VK_3, 0x33, 0x33, '3', 0x23, '#');
        add(KeyEvent.VK_4, 0x34, 0x34, '4', 0x24, '$');
        add(KeyEvent.VK_5, 0x35, 0x35, '5', 0x25, '%');
        add(KeyEvent.VK_6, 0x36, 0x36, '6', 0x5e, '^');
        add(KeyEvent.VK_7, 0x37, 0x37, '7', 0x26, '&');
        add(KeyEvent.VK_8, 0x38, 0x38, '8', 0x2a, '*');
        add(KeyEvent.VK_9, 0x39, 0x39, '9', 0x28, '(');
        add(KeyEvent.VK_0, 0x30, 0x30, '0', 0x29, ')');

        add(KeyEvent.VK_A, 0x61, 0x61, 'a', 0x41, 'A');
        add(KeyEvent.VK_B, 0x62, 0x62, 'b', 0x42, 'B');
        add(KeyEvent.VK_C, 0x63, 0x63, 'c', 0x43, 'C');
        add(KeyEvent.VK_D, 0x64, 0x64, 'd', 0x44, 'D');
        add(KeyEvent.VK_E, 0x65, 0x65, 'e', 0x45, 'E');
        add(KeyEvent.VK_F, 0x66, 0x66, 'f', 0x46, 'F');
        add(KeyEvent.VK_G, 0x67, 0x67, 'g', 0x47, 'G');
        add(KeyEvent.VK_H, 0x68, 0x68, 'h', 0x48, 'H');
        add(KeyEvent.VK_I, 0x69, 0x69, 'i', 0x49, 'I');
        add(KeyEvent.VK_J, 0x6a, 0x6a, 'j', 0x4a, 'J');
        add(KeyEvent.VK_K, 0x6b, 0x6b, 'k', 0x4b, 'K');
        add(KeyEvent.VK_L, 0x6c, 0x6c, 'l', 0x4c, 'L');
        add(KeyEvent.VK_M, 0x6d, 0x6d, 'm', 0x4d, 'M');
        add(KeyEvent.VK_N, 0x6e, 0x6e, 'n', 0x4e, 'N');
        add(KeyEvent.VK_O, 0x6f, 0x6f, 'o', 0x4f, 'O');
        add(KeyEvent.VK_P, 0x70, 0x70, 'p', 0x50, 'P');
        add(KeyEvent.VK_Q, 0x71, 0x71, 'q', 0x51, 'Q');
        add(KeyEvent.VK_R, 0x72, 0x72, 'r', 0x52, 'R');
        add(KeyEvent.VK_S, 0x73, 0x73, 's', 0x53, 'S');
        add(KeyEvent.VK_T, 0x74, 0x74, 't', 0x54, 'T');
        add(KeyEvent.VK_U, 0x75, 0x75, 'u', 0x55, 'U');
        add(KeyEvent.VK_U, 0x76, 0x76, 'v', 0x56, 'V');
        add(KeyEvent.VK_W, 0x77, 0x77, 'w', 0x57, 'W');
        add(KeyEvent.VK_X, 0x78, 0x78, 'x', 0x58, 'X');
        add(KeyEvent.VK_Y, 0x79, 0x79, 'y', 0x59, 'Y');
        add(KeyEvent.VK_Z, 0x7a, 0x7a, 'z', 0x5a, 'Z');

        add(KeyEvent.VK_ESCAPE, 0x1b, 0x1b, noChar, 0x1b, noChar);
        add(KeyEvent.VK_BACK_QUOTE, 0x60, 0x60, '`', 0x7e, '~');
        add(KeyEvent.VK_MINUS, 0x2d, 0x2d, '-', 0x5f, '_');
        add(KeyEvent.VK_EQUALS, 0x3d, 0x3d, '=', 0x2b, '+');
        add(KeyEvent.VK_BACK_SPACE, 0x7f, 0x7f, noChar, 0x7f, noChar);
        add(KeyEvent.VK_TAB, 0x09, 0x09, noChar, 0x09, noChar);
        add(KeyEvent.VK_BRACELEFT, 0x5b, 0x5b, '[', 0x7b, '{');
        add(KeyEvent.VK_BRACERIGHT, 0x5d, 0x5d, ']', 0x7d, '}');
        add(KeyEvent.VK_BACK_SLASH, 0x5c, 0x5c, '\\', 0x7c, '|');
        add(KeyEvent.VK_SEMICOLON, 0x3b, 0x3b, ';', 0x3a, ':');
        add(KeyEvent.VK_QUOTE, 0x27, 0x27, '\'', 0x22, '"');
        add(KeyEvent.VK_ENTER, 0x0d, 0x0d, noChar, 0x0d, noChar);
        add(KeyEvent.VK_COMMA, 0x2c, 0x2c, ',', 0x3c, '<');
        add(KeyEvent.VK_PERIOD, 0x2e, 0x2e, ',', 0x3e, '>');

        // :TODO: not sure about this, can I just map key codes to keysym however I want?

        add(KeyEvent.VK_SHIFT, LEFT_SHIFT_KEYCODE, 0xFFE1, noChar, 0xFFE1, noChar, KeyEvent.KEY_LOCATION_LEFT);
        add(KeyEvent.VK_SHIFT, RIGHT_SHIFT_KEYCODE, 0xFFE2, noChar, 0xFFE2, noChar, KeyEvent.KEY_LOCATION_RIGHT);

        add(KeyEvent.VK_CAPS_LOCK, CAPS_LOCK_KEYCODE, 0xFFE5, noChar, 0xFFE5, noChar);

        add(KeyEvent.VK_CONTROL, LEFT_CONTROL_KEYCODE, 0xFFE3, noChar, 0xFFE3, noChar, KeyEvent.KEY_LOCATION_LEFT);
        add(KeyEvent.VK_CONTROL, RIGHT_CONTROL_KEYCODE, 0xFFE4, noChar, 0xFFE4, noChar, KeyEvent.KEY_LOCATION_RIGHT);

        add(KeyEvent.VK_ALT, LEFT_ALT_KEYCODE, 0xFFE9, noChar, 0xFFE9, noChar, KeyEvent.KEY_LOCATION_LEFT);
        add(KeyEvent.VK_ALT, RIGHT_ALT_KEYCODE, 0xFFEA, noChar, 0xFFEA, noChar, KeyEvent.KEY_LOCATION_RIGHT);

        add(KeyEvent.VK_NUM_LOCK, NUM_LOCK_KEYCODE, 0xFF7F, noChar, 0xFF7F, noChar);
        add(KeyEvent.VK_SCROLL_LOCK, SCROLL_LOCK_KEYCODE, 0xFF14, noChar, 0xFF14, noChar);



        // :TODO: number pad, f keys
    }
}

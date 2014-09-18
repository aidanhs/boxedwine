package wine.builtin.libX11;

import wine.emulation.Memory;

public class IM {
    static public class IMCallback {
        public IMCallback(Memory memory, int address) {
            client_data = memory.readd(address);address+=4;
            callback = memory.readd(address);
        }
        public void write(Memory memory, int address) {
            memory.writed(address, client_data);
            memory.writed(address+4, callback);
        }
        public int client_data;
        public int callback;
    }
    static public final int XIMPreeditArea		= 0x0001;
    static public final int XIMPreeditCallbacks	= 0x0002;
    static public final int XIMPreeditPosition	= 0x0004;
    static public final int XIMPreeditNothing	= 0x0008;
    static public final int XIMPreeditNone		= 0x0010;
    static public final int XIMStatusArea		= 0x0100;
    static public final int XIMStatusCallbacks	= 0x0200;
    static public final int XIMStatusNothing	= 0x0400;
    static public final int XIMStatusNone		= 0x0800;

    static public final int SIZE = 16;

    public int display;
    public int locale;
    public int onDestroyCallback;
    public int onDestroyData;

    public IM() {
    }

    public IM(Memory memory, int address) {
        read(memory, address);
    }

    public void read(Memory memory, int address) {
        display = memory.readd(address); address+=4;
        locale = memory.readd(address); address+=4;
        onDestroyCallback = memory.readd(address); address+=4;
        onDestroyData = memory.readd(address); address+=4;
    }
    public void write(Memory memory, int address) {
        memory.writed(address, display);address+=4;
        memory.writed(address, locale);address+=4;
        memory.writed(address, onDestroyCallback);address+=4;
        memory.writed(address, onDestroyData);
    }
}

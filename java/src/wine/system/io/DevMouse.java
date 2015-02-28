package wine.system.io;

import wine.system.WineThread;
import wine.system.kernel.Signal;
import wine.system.kernel.Syscall;
import wine.util.Log;

public class DevMouse extends DevInput {
    private int lastX = -1;
    private int lastY = -1;

    public DevMouse() {
        super((1<<EV_SYN)|(1<<EV_KEY)|(1<<EV_REL), "Logitech Unifying Device. Wireless PID:1028");
        busType = 3;
        product = 0xc52b;
        vendor = 0x046d;
        version = 0x0111;
    }

    public void event(int x, int y, int button) {
        if (lastX<0 || lastY<0) {
            lastX = x;
            lastY = y;
        }
        boolean send = false;
        if (x!=lastX) {
            super.event(EV_REL, REL_X, x-lastX);
            lastX = x;
            send = true;
        }
        if (y!=lastY) {
            super.event(EV_REL, REL_Y, y-lastY);
            lastY = y;
            send = true;
        }
        if (send) {
            super.event(EV_SYN, SYN_REPORT, 0);
            if (asyncProcess != null) {
                asyncProcess.signal(Signal.SIGIO);
            }
        }
    }

    public int ioctl(WineThread thread, int request, Syscall.SyscallGetter getter) {
        switch (request & 0xFFFF) {
            case 0x4521: { // EVIOCGBIT, EV_KEY
                int len = (request & 0x1fff0000) >> 16;
                int buffer = getter.next();
                int result = Math.max(BTN_LEFT, BTN_MIDDLE);
                result = Math.max(BTN_RIGHT, result);
                result = (result+7)/8;
                thread.process.memory.zero(buffer, len);
                writeBit(thread, buffer, BTN_LEFT);
                writeBit(thread, buffer, BTN_MIDDLE);
                writeBit(thread, buffer, BTN_RIGHT);
                return result;
            }
            case 0x4522: { // EVIOCGBIT, EV_REL
                int len = (request & 0x1fff0000) >> 16;
                int buffer = getter.next();
                thread.process.memory.zero(buffer, len);
                thread.process.memory.writeb(buffer, (1 << REL_X)|(1 << REL_Y));
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
            case 0x4540: { // EVIOCGABS (ABS_X)
                int len = (request & 0x1fff0000) >> 16;
                int address = getter.next();
                if (len<24)
                    Log.panic("Bad length for EVIOCGABS (ABS_X)");
                writeAbs(thread, address, 0, 0, 0);
                return 0;
            }
            case 0x4541: { // EVIOCGABS (ABS_Y)
                int len = (request & 0x1fff0000) >> 16;
                int address = getter.next();
                if (len<24)
                    Log.panic("Bad length for EVIOCGABS (ABS_X)");
                writeAbs(thread, address, 0, 0, 0);
                return 0;
            }
            default:
                return super.ioctl(thread, request, getter);
        }
    }
}

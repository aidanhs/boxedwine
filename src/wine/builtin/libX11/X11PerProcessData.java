package wine.builtin.libX11;

import wine.builtin.libX11.events.XEvent;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.system.io.FileDescriptor;

import java.util.LinkedList;

public class X11PerProcessData {
    private LinkedList<Integer> events = new LinkedList<Integer>();
    private LinkedList<Integer> freeEvents = new LinkedList<Integer>();
    static private final int EVENT_SIZE=64;

    synchronized public boolean hasEvents() {
        return events.size()>0;
    }

    synchronized public int checkIfEvent(WineThread thread, int display, int event_return, int predicate, int arg) {
        for (Integer event : events) {
            thread.cpu.call(predicate, display, event, arg);
            if (thread.cpu.eax.dword!=0) {
                thread.process.memory.memcpy(event_return, event, EVENT_SIZE );
                freeEvents.addLast(event);
                events.remove(event);
                return 1;
            }
        }
        return 0;
    }

    synchronized public void addEvent(WineProcess process, XEvent event) {
        int address;
        if (freeEvents.size()==0) {
            address = process.alloc(EVENT_SIZE);
        } else {
            address = freeEvents.removeLast();
        }
        event.write(process.memory, address);
        events.addLast(address);
        synchronized (FileDescriptor.lock) {
            FileDescriptor.lock.notifyAll();
        }
    }
}

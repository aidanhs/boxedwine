package wine.system;

import wine.emulation.CPU;
import wine.emulation.Memory;
import wine.emulation.PageHandler;
import wine.emulation.RAM;
import wine.util.Log;

import java.util.Enumeration;
import java.util.Hashtable;

public class WineThread {
    public final CPU cpu;
    public final Integer id;
    public int entryPoint;
    public int stackTop;
    public int stackBottom;
    public int stackSize;
    public int stackAddress;
    public final int stackSizeReserved;
    public final WineProcess process;
    static private int nextId = 1;
    static private Hashtable threadToWineThread = new Hashtable();
    private int errnoPtr;
    public int strerror;
    public Thread jThread;
    public int exitValue;
    public int alternateStack;
    public int alternateStackSize;
    public int sigMask;
    private StringBuilder stdoutBuffer = new StringBuilder();

    static public WineThread getCurrent() {
        return (WineThread)threadToWineThread.get(Thread.currentThread());
    }

    public void registerThread() {
        threadToWineThread.put(Thread.currentThread(), this);
    }

    public WineThread(WineProcess process, long startAddress, int stackAddress, int stackSizeCommit, int stackSizeReserved) {
        if (stackSizeReserved==0)
            stackSizeReserved = 1024*1024;
        if (stackSizeReserved<stackSizeCommit)
            stackSizeReserved=stackSizeCommit;
        if (stackSizeCommit==0)
            stackSizeCommit = 4096;
        this.errnoPtr = process.alloc(4);
        stackSizeReserved+=4096*4; // padding for how ThreadHandlerCheck works
        id = new Integer(nextId++);
        entryPoint = (int)startAddress;
        this.process = process;
        this.stackSizeReserved = stackSizeReserved;
        if (stackAddress!=0) {
            this.stackTop = stackAddress + stackSizeReserved;
            this.stackBottom = stackAddress;
            this.stackAddress = stackAddress;
        } else {
            this.stackAddress = (int) process.addressSpace.getNextAddress(WineProcess.ADDRESS_PROCESS_STACK_START, stackSizeReserved, true) + stackSizeReserved;
            process.addressSpace.alloc(this.stackAddress, stackSizeReserved);
            this.stackSize = 0;
            this.stackTop = this.stackAddress+stackSizeReserved;
            this.stackBottom = this.stackTop;

            int pages = (stackSizeCommit + 0xFFF) >> 12;
            growStack(pages);
        }
        this.cpu = new CPU(this, process.callReturnEip);
        cpu.esp.dword = stackTop;
        process.threads.put(new Integer(id), this);

    }

    public void out(String msg) {
        if (stdoutBuffer.length()==0) {
            stdoutBuffer.append(process.mainModule.name);
            stdoutBuffer.append(": ");
        }
        stdoutBuffer.append(msg);
        if (msg.indexOf('\n')>=0) {
            System.out.print(stdoutBuffer.toString());
            stdoutBuffer.setLength(0);
        }
    }

    public void close() {
        if (this.strerror!=0) {
            process.free(this.strerror);
            this.strerror = 0;
        }
        process.free(this.errnoPtr);
        this.errnoPtr = 0;

        int pageStart = this.stackAddress>>>12;
        int pageCount = this.stackSize >>> 12;
        for (int i=0;i<pageCount;i++) {
            PageHandler handler = process.memory.handlers[pageStart+i];
            if (handler instanceof ThreadHandler) {
                RAM.freePage(((ThreadHandler)handler).physicalPage);
            }
            process.memory.handlers[i+pageStart] = Memory.invalidHandler;
        }
        process.addressSpace.free(this.stackAddress);

        process.threads.remove(new Integer(id));
        Enumeration e = threadToWineThread.keys();
        while (e.hasMoreElements()) {
            Thread thread = (Thread)e.nextElement();
            WineThread w = (WineThread)threadToWineThread.get(thread);
            if (w == this) {
                threadToWineThread.remove(thread);
                return;
            }
        }
        Log.panic("Logic error");
    }

    final public static Callback wineThreadReturn = new Callback() {
        public String toString() {
            return "DllMainReturn";
        }

        public void call(CPU cpu) {
            WineThread.getCurrent().exitValue = cpu.eax.dword;
            throw new ExitThreadException();
        }
    };

    public void start(final int arg) {
        this.jThread = new Thread(new Runnable() {
            public void run() {
                try {
                    registerThread();
                    cpu.eip = entryPoint;
                    cpu.esp.dword = stackTop;
                    cpu.push32(arg);
                    cpu.push32(process.eipThreadReturn);
                    cpu.run();
                } catch (ExitThreadException e) {

                }
            }
        });
        this.jThread.start();
    }
    public void growStack(int pages) {
        int address = stackTop - stackSize - (pages << 12);
        if (stackSize>0) {
            ThreadHandlerCheck check = (ThreadHandlerCheck)process.memory.handlers[this.stackBottom>>>12];
            process.memory.handlers[this.stackBottom>>>12] = new ThreadHandler(check.offset, check.physicalPage);
        }
        int pageStart = address>>>12;
        int page = RAM.allocPage();
        process.memory.handlers[pageStart] = new ThreadHandlerCheck((pageStart-page)<<12, page);
        for (int i=1;i<pages;i++) {
            page = RAM.allocPage();
            process.memory.handlers[pageStart+i] = new ThreadHandler((pageStart-page)<<12, page);
        }

        stackSize+=(pages << 12);
        this.stackBottom = this.stackTop-this.stackSize;
        if (stackSize>stackSizeReserved) {
            Log.panic("Stack overflow");
        }
    }

    public int getErrnoPtr() {
        return errnoPtr;
    }

    public void setErrno(int errno) {
        process.memory.writed(this.errnoPtr, errno);
    }

    final private class ThreadHandlerCheck extends PageHandler {
        private int offset;
        private int physicalPage;

        public ThreadHandlerCheck(int offset, int physicalPage) {
            this.offset = offset;
            this.physicalPage = physicalPage;
        }
        private void grow() {
            growStack(4);
        }
        public void close() {
            RAM.freePage(physicalPage);
        }
        public int readd(int address) {
            grow();
            return RAM.readd(address-offset);
        }
        public int readw(int address) {
            grow();
            return RAM.readw(address-offset);
        }
        public int readb(int address) {
            grow();
            return RAM.readb(address-offset);
        }
        public void writed(int address, int value) {
            grow();
            RAM.writed(address-offset, value);
        }
        public void writew(int address, int value) {
            grow();
            RAM.writew(address-offset, value);
        }
        public void writeb(int address, int value) {
            grow();
            RAM.writeb(address-offset, value);
        }
    }

    final private class ThreadHandler extends PageHandler {
        private int offset;
        private int physicalPage;

        public ThreadHandler(int offset, int physicalPage) {
            this.offset = offset;
            this.physicalPage = physicalPage;
        }
        public void close() {
            RAM.freePage(physicalPage);
        }
        public int readd(int address) {
            return RAM.readd(address-offset);
        }
        public int readw(int address) {
            return RAM.readw(address-offset);
        }
        public int readb(int address) {
            return RAM.readb(address-offset);
        }
        public void writed(int address, int value) {
            RAM.writed(address-offset, value);
        }
        public void writew(int address, int value) {
            RAM.writew(address-offset, value);
        }
        public void writeb(int address, int value) {
            RAM.writeb(address-offset, value);
        }
    }

}

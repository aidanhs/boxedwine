package wine.system;

import wine.emulation.*;
import wine.util.Log;

import java.util.Hashtable;

public class WineThread {
    public CPU cpu;
    public int id;
    public int entryPoint;
    public int stackTop;
    public int stackBottom;
    public int stackSize;
    public int stackAddress;
    public final int stackSizeReserved;
    public final int stackSizeCommit;
    public final int initialStackAddress;
    public final WineProcess process;
    static private Hashtable threadToWineThread = new Hashtable();
    private int errnoPtr;
    public int strerror;
    public Thread jThread;
    public int exitValue;
    public int alternateStack;
    public int alternateStackSize;
    public int sigMask;
    public int strtok_last;
    private StringBuilder stdoutBuffer = new StringBuilder();

    static public WineThread getCurrent() {
        return (WineThread)threadToWineThread.get(Thread.currentThread());
    }

    public void registerThread() {
        threadToWineThread.put(Thread.currentThread(), this);
    }


    public WineThread fork(WineProcess process) {
        return new WineThread(process, this);
    }

    private WineThread(final WineProcess process, WineThread forkedThread) {
        this.process = process;
        this.errnoPtr = forkedThread.errnoPtr;
        this.strerror = forkedThread.strerror;
        this.id = WineSystem.nextid++;
        this.entryPoint = forkedThread.entryPoint;
        this.stackAddress = forkedThread.stackAddress;
        this.stackBottom = forkedThread.stackBottom;
        this.stackTop = forkedThread.stackTop;
        this.stackSize = forkedThread.stackSize;
        this.stackSizeReserved = forkedThread.stackSizeReserved;
        this.stackSizeCommit = forkedThread.stackSizeCommit;
        this.initialStackAddress = forkedThread.initialStackAddress;
        this.cpu = forkedThread.cpu.deepCopy(this, process.callReturnEip);
        this.jThread = new Thread(new Runnable() {
            public void run() {
                try {
                    registerThread();
                    cpu.run();
                } catch (ExitThreadException e) {

                } finally {
                    cleanup();
                }
            }
        });
        process.threads.put(id, this);
    }

    public WineThread(WineProcess process, long startAddress, int stackAddress, int stackSizeCommit, int stackSizeReserved, int id) {
        entryPoint = (int)startAddress;
        this.process = process;
        this.id = id;
        if (stackSizeReserved==0)
            stackSizeReserved = 1024*1024;
        if (stackSizeReserved<stackSizeCommit)
            stackSizeReserved=stackSizeCommit;
        if (stackSizeCommit==0)
            stackSizeCommit = 4096;
        this.stackSizeCommit = stackSizeCommit;
        if (stackAddress==0)
            stackSizeReserved+=4096*4; // padding for how ThreadHandlerCheck works
        this.stackSizeReserved = stackSizeReserved;

        this.initialStackAddress = stackAddress;
        if (stackAddress!=0) {
            this.stackTop = stackAddress + stackSizeReserved;
            this.stackBottom = stackAddress;
            this.stackAddress = stackAddress;
        }
    }

    public void init() {
        if (initialStackAddress==0) {
            synchronized (process.addressSpace) {
                int pageCount = stackSizeReserved>>>12;
                int page = process.addressSpace.getNextPage(WineProcess.ADDRESS_PROCESS_STACK_START, pageCount);
                this.stackAddress = page << 12;
                process.addressSpace.allocPages(page, pageCount);
            }
            this.stackSize = 0;
            this.stackTop = this.stackAddress+stackSizeReserved;
            this.stackBottom = this.stackTop;

            int pages = (stackSizeCommit + 0xFFF) >> 12;
            growStack(pages+1);
            process.memory.zero(this.stackTop, 0);
        } else {
            this.stackBottom = initialStackAddress;
            this.stackTop = this.stackBottom+this.stackSizeReserved;
            this.stackSize = this.stackSizeReserved;
        }
        if (this.cpu==null)
            this.cpu = new CPU(this);
        this.cpu.init(process.callReturnEip);
        cpu.esp.dword = stackTop-4096;
        process.threads.put(id, this);
        this.errnoPtr = process.alloc(4);
    }

    public void out(String msg) {
        if (Log.level>=Log.LEVEL_DEBUG && stdoutBuffer.length()==0) {
            stdoutBuffer.append(process.name);
            stdoutBuffer.append(":");
            stdoutBuffer.append(process.id);
            stdoutBuffer.append(" ");
        }
        stdoutBuffer.append(msg);
        if (msg.indexOf('\n')>=0) {
            System.out.print(stdoutBuffer.toString());
            stdoutBuffer.setLength(0);
        }
    }

    public void cleanup() {
        if (this.strerror!=0) {
            process.free(this.strerror);
            this.strerror = 0;
        }
        process.free(this.errnoPtr);
        this.errnoPtr = 0;

        if (stackAddress!=0) {
            int pageStart = this.stackAddress >>> 12;
            int pageCount = this.stackSize >>> 12;
            for (int i = 0; i < pageCount; i++) {
                PageHandler handler = process.memory.handlers[pageStart + i];
                handler.close();
                process.memory.handlers[i + pageStart] = Memory.invalidHandler;
            }
            process.addressSpace.freePages(pageStart, pageCount);
        }
        threadToWineThread.remove(Thread.currentThread());
        process.exitThread(this);
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

                } finally {
                    cleanup();
                }
            }
        });
        this.jThread.start();
    }
    public void growStack(int pages) {
        int address = stackTop - stackSize - (pages << 12);
        if (stackSize>0) {
            ThreadHandlerCheck check = (ThreadHandlerCheck)process.memory.handlers[this.stackBottom>>>12];
            process.memory.handlers[this.stackBottom>>>12] = new ThreadHandler(process.memory, check.getPhysicalPage());
        }
        int pageStart = address>>>12;
        int page = RAM.allocPage();
        process.memory.handlers[pageStart] = new ThreadHandlerCheck(process.memory, page);
        for (int i=1;i<pages;i++) {
            page = RAM.allocPage();
            process.memory.handlers[pageStart+i] = new ThreadHandler(process.memory, page);
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

    public int signal(int signal) {
        Log.panic("tgkill not implemented yet");
        SigAction action = process.sigActions[signal];
        if (action.sa_handler==SigAction.SIG_DFL) {

        } else if (action.sa_handler != SigAction.SIG_IGN) {

        }
        return 0;
    }

    final private class ThreadHandlerCheck extends ThreadHandler {
        public ThreadHandlerCheck(Memory memory, int physicalPage) {
            super(memory, physicalPage);
        }
        private void grow() {
            growStack(4);
        }
        public int readd(Memory memory, int address) {
            grow();
            return super.readd(memory, address);
        }
        public int readw(Memory memory, int address) {
            grow();
            return super.readw(memory, address);
        }
        public int readb(int address) {
            grow();
            return super.readb(address);
        }
        public void writed(Memory memory, int address, int value) {
            grow();
            super.writed(memory, address, value);
        }
        public void writew(Memory memory, int address, int value) {
            grow();
            super.writew(memory, address, value);
        }
        public void writeb(int address, int value) {
            grow();
            super.writeb(address, value);
        }
        public PageHandler fork(WineProcess process) {
            int page = RAM.allocPage();
            ThreadHandlerCheck handler = new ThreadHandlerCheck(process.memory, page);
            RAM.copy(getPhysicalPage(), page);
            return handler;
        }
    }

    private class ThreadHandler extends RAMHandler {
        public ThreadHandler(Memory memory, int physicalPage) {
            super(physicalPage, false, false);
        }
        public PageHandler fork(WineProcess process) {
            int page = RAM.allocPage();
            ThreadHandler handler = new ThreadHandler(process.memory, page);
            RAM.copy(getPhysicalPage(), page);
            return handler;
        }
    }
}

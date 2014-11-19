package wine.system;

import wine.builtin.libX11.X11PerProcessData;
import wine.builtin.libc.Errno;
import wine.builtin.libc.Fcntl;
import wine.builtin.libc.Signal;
import wine.builtin.libc.Syscall;
import wine.emulation.CPU;
import wine.emulation.Memory;
import wine.emulation.RAM;
import wine.emulation.RAMHandler;
import wine.gui.Screen;
import wine.loader.ElfModule;
import wine.loader.Loader;
import wine.loader.elf.ElfHeader;
import wine.system.io.FSNode;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;
import wine.system.io.KernelObject;
import wine.util.AddressSpace;
import wine.util.Heap;
import wine.util.LittleEndianStream;
import wine.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import java.util.concurrent.locks.Lock;

public class WineProcess {
    // the values don't really matter too much, it just makes it easier to debug if we know what might be in a particular address
    static public int ADDRESS_PROCESS_STACK_START =    0xE0000;
    static public int ADDRESS_PER_CPU =                0xE1000;
    static public int ADDRESS_PROCESS_CALLBACK_START = 0xE2000;
    static public int ADDRESS_PROCESS_SHARED_START =   0xE3000;
    static public int ADDRESS_PROCESS_DLL_START =      0xE4000;
    static public int ADDRESS_PER_PROCESS =            0xE5000;
    static public int ADDRESS_PROCESS_MMAP_START =     0xE6000;
    static public int ADDRESS_PROCESS_HEAP_START =     0xF0000; // this needs a continuous space, hopefully wine won't use more than 256MB
    static public int ADDRESS_PROCESS_FRAME_BUFFER =   0xF8000;
    static public int ADDRESS_PROCESS_FRAME_BUFFER_ADDRESS = 0xF8000000;
    static public final int STATE_INIT = 0;
    static public final int STATE_STARTED = 1;
    static public final int STATE_DONE = 2;

    // resources
    static public final int MAX_STACK_SIZE = 128*1024*1024;
    static public final int MAX_ADDRESS_SPACE = 0xE0000000;
    static public final int MAX_NUMBER_OF_FILES = 0xFFFF;

    // if new variables are addedd, make sure they are accounted for in fork and exec
    final public AddressSpace addressSpace;
    final public Memory memory;
    final public Hashtable<Integer, WineThread> threads = new Hashtable<Integer, WineThread>();
    final public SigAction[] sigActions = SigAction.create(64);

    public ElfModule mainModule;
    public Loader loader;
    public String currentDirectory;
    public Hashtable<Integer, FileDescriptor> fileDescriptors = new Hashtable<Integer, FileDescriptor>();
    public final int id;
    public int groupId=1;
    public int eipThreadReturn;
    public int end;
    public int maxStackSize = MAX_STACK_SIZE;
    public int maxAddressSpace = MAX_ADDRESS_SPACE;
    public int startData;
    final private Heap heap;
    public int callReturnEip;
    public int asctime;
    public int tm;
    public int exitCode;
    public boolean terminated = false;
    public int signaled;
    public int perCPUIndex = 0;
    public int state = STATE_INIT;
    public String name;
    public WineProcess parent;
    public int xerrorHandler;
    private Hashtable<String, Integer> stringMap = new Hashtable<String, Integer>();
    private int stringPage;
    private int stringPagePos;
    // if new variables are addedd, make sure they are accounted for in fork and exec
    public X11PerProcessData x11 = new X11PerProcessData();
    public String fullPath;

    static private class ExitFunction {
        public ExitFunction(int func, int arg) {
            this.func = func;
            this.arg = arg;
        }
        public int func;
        public int arg;
    }

    private WineProcess(int id, WineProcess process) {
        loader = process.loader.fork(this);
        this.mainModule = loader.main;
        this.addressSpace = process.addressSpace.deepCopy();
        this.heap = process.heap.deepCopy();
        this.currentDirectory = process.currentDirectory;
        for (FileDescriptor fd : process.fileDescriptors.values()) {
            if (fd.canFork()) {
                FileDescriptor f = fd.fork();
                this.fileDescriptors.put(f.handle, f);
            }
        }
        // must come after fd
        this.memory = new Memory();
        process.memory.fork(this);
        this.id = id;
        this.eipThreadReturn = process.eipThreadReturn;
        this.end = process.end;
        this.maxAddressSpace = process.maxAddressSpace;
        this.maxStackSize = process.maxStackSize;
        this.callReturnEip = process.callReturnEip;
        this.startData = process.startData;

        this.asctime = process.asctime;
        this.tm = process.tm;
        this.exitCode = process.exitCode;
        this.terminated = process.terminated;
        this.signaled = process.signaled;
        this.perCPUIndex = 0;
        this.state = process.state;
        this.name = process.name;
        this.fullPath = process.fullPath;
        this.stringMap = (Hashtable<String, Integer>)process.stringMap.clone();
        this.stringPage = process.stringPage;
        this.stringPagePos = process.stringPagePos;
        WineSystem.processes.put(id, this);
    }

    public WineProcess(String currentDirectory, Vector<String> args, int id, int tid) {
        memory  = new Memory();
        loader = new Loader(this);
        addressSpace = new AddressSpace();
        heap = new Heap(ADDRESS_PROCESS_HEAP_START<<12, ADDRESS_PROCESS_HEAP_START<<12, 64); // it will grow
        this.currentDirectory = currentDirectory;
        this.id = id;
        WineSystem.processes.put(id, this);
        WineThread thread = new WineThread(this, 0, 0, 0, 0, tid);
        thread.jThread = Thread.currentThread();
        thread.registerThread();
        init(args, thread);
    }

    public void init(Vector<String> args, WineThread thread) {
        memory.init();
        loader.init();
        addressSpace.clear();

        heap.clear(ADDRESS_PROCESS_HEAP_START<<12, ADDRESS_PROCESS_HEAP_START<<12);
        callReturnEip=CPU.registerCallReturnEip(loader);
        eipThreadReturn = loader.registerFunction(WineThread.wineThreadReturn);

        addressSpace.allocPages(ADDRESS_PROCESS_HEAP_START, 0x100000 - ADDRESS_PROCESS_HEAP_START - 1);

        if (fileDescriptors.get(0)==null)
            fileDescriptors.put(0, new FileDescriptor(0, KernelObject.stdin, Fcntl.O_RDONLY));
        if (fileDescriptors.get(1)==null)
            fileDescriptors.put(1, new FileDescriptor(0, KernelObject.stdout, Fcntl.O_WRONLY));
        if (fileDescriptors.get(2)==null)
            fileDescriptors.put(2, new FileDescriptor(0, KernelObject.stderr, Fcntl.O_WRONLY));

        Screen.initProcess(this);
        thread.init();
        mainModule = (ElfModule)loader.loadModule(thread, args.get(0));
        this.name = mainModule.name;
    }
    private void cleanup() {
        Vector<FileDescriptor> fds = new Vector<FileDescriptor>();
        for (FileDescriptor fd : this.fileDescriptors.values()) {
            if (fd.closeOnExec())
                fds.add(fd);
        }
        for (FileDescriptor fd : fds) {
            fd.close();
        }
        memory.close();
        mainModule = null;
        loader = null;
        fileDescriptors = null;
        stringMap = null;
    }

    public int alloc(int size) {
        synchronized (heap) {
            int result = heap.alloc(memory, size);
            if (result == 0) {
                int pages = ((size + 0xFFF) >>> 12) + 1;
                int address = heap.grow(pages << 12);
                int pageStart = address >>> 12;
                for (int i = 0; i < pages; i++) {
                    int page = RAM.allocPage();
                    memory.handlers[pageStart + i] = new RAMHandler(page, false, false);
                }
                result = heap.alloc(memory, size);
            }
            if (result == 0) {
                Log.panic("Out of memory");
            }
            return result;
        }
    }

    public int getSizeOfAllocation(int address) {
        return heap.getSize(memory, address);
    }

    public void allocPages(int pageStart, int pages, boolean map) {
        if (!addressSpace.allocPages(pageStart, pages)) {
            Log.panic("Failed to find address for " + pages+" pages @0x" + Integer.toHexString(pageStart<<12));
        }
        for (int i=0;i<pages;i++) {
            int page = RAM.allocPage();
            memory.handlers[pageStart+i] = new RAMHandler(page, map, false);
        }
    }

    public void freePages(int pageStart, int pages) {
        for (int i=0;i<pages;i++) {
            memory.handlers[pageStart+i].close();
            memory.handlers[pageStart+i] = Memory.invalidHandler;
        }
        addressSpace.freePages(pageStart, pages);
    }

    public int mapPage(int page) {
        synchronized (addressSpace) {
            int pageStart = addressSpace.getNextPage(ADDRESS_PROCESS_SHARED_START, 1);
            addressSpace.allocPages(pageStart, 1);
            memory.handlers[pageStart] = new RAMHandler(page, false, false);
            return pageStart<<12;
        }
    }

    public void free(int address) {
        if (address==0)
            return;
        synchronized (heap) {
            heap.free(memory, address);
        }
    }

    public int getNextFileDescriptor() {
        int id = 3;
        while (true) {
            if (fileDescriptors.get(id)==null)
                return id;
            id++;
        }
    }

    public FileDescriptor getFile(String localPath, boolean existing) {
        FSNode node = FSNode.getNode(localPath, existing);
        if (node == null) {
            return null;
        }
        return new KernelFile(node).createNewFileDescriptor(this);
    }

    public FileDescriptor getFileDescriptor(int handle) {
        return fileDescriptors.get(new Integer(handle));
    }

    public boolean closeFileDescriptor(int handle) {
        FileDescriptor file = getFileDescriptor(handle);
        if (file == null)
            return false;
        file.close();
        return true;
    }

    public void exitThread(WineThread thread) {
        synchronized (WineSystem.processes) {
            if (threads.size()==1) {
                if (Log.level>=Log.LEVEL_DEBUG)
                    thread.out("Process Terminated\n");
                this.terminated = true;
                if (parent!=null && !parent.terminated && parent.sigActions[Signal.SIGCHLD].sa_handler>1) {
                    parent.sigActions[Signal.SIGCHLD].call();
                }
                cleanup();
                // :TODO: need to track parent process
//                if (sigActions[Signal.SIGCHLD].sa_handler==SigAction.SIG_IGN) {
//                    WineSystem.processes.remove(id);
//                } else if (sigActions[Signal.SIGCHLD].sa_handler!=SigAction.SIG_DFL) {
//                    sigActions[Signal.SIGCHLD].call();
//                }
                WineSystem.processes.notifyAll();
            }
            threads.remove(thread.id);
        }
    }

    public void _exit(int status) {
        if (Log.level>=Log.LEVEL_DEBUG)
            WineThread.getCurrent().out("_exit "+status+"\n");
        this.exitCode = status;
        for (Integer threadId :threads.keySet()) {
            WineThread thread = threads.get(threadId);
            if (thread.jThread!=Thread.currentThread())
                thread.jThread.stop(new ExitThreadException());
        }
        throw new ExitThreadException();
    }

    public boolean isStopped() {
        return false;
    }

    public boolean isTerminated() {
        return terminated;
    }

    public void waitForPid() {
        synchronized (WineSystem.processes) {
            while (true) {
                if (isStopped() || isTerminated()) {
                    WineSystem.processes.remove(id);
                    return;
                }
                try {
                    WineSystem.processes.wait();
                } catch (Exception e) {
                }
            }
        }
    }

    public static WineProcess create(final String currentDirectory, final Vector<String> args, final Vector<String> env) {
        return create(currentDirectory, args, env, WineSystem.nextid++, WineSystem.nextid++, null);
    }

    private static WineProcess create(final String currentDirectory, final Vector<String> args, final Vector<String> env, final int pid, final int tid, final Object waitToStart) {
        final Object[] result = new Object[1];

        Thread thread = new Thread(new Runnable() {
            public void run() {
                final WineProcess process = new WineProcess(currentDirectory, args, pid, tid);
                if (process.mainModule==null) {
                    synchronized (result) {
                        result.notify();
                    }
                    return;
                }
                result[0] = process;
                args.set(0, process.mainModule.fullPath());
                try {
                    if (waitToStart!=null) {
                        synchronized (waitToStart) {
                            synchronized (result) {
                                result.notify();
                            }
                            waitToStart.wait();
                        }
                    } else {
                        synchronized (result) {
                            result.notify();
                        }
                    }
                    process.start(WineThread.getCurrent(), args, env, true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
        synchronized (result) {
            thread.start();
            try {result.wait();} catch (Exception e){}
        }
        if (result[0]!=null)
            return (WineProcess)result[0];
        return null;
    }

    private void start(WineThread thread, Vector<String> args, Vector<String> env, boolean run) {
        fullPath = FSNode.getNode(args.get(0), true).localPath;
        name = fullPath.substring(fullPath.lastIndexOf('/')+1);

        setupStack(thread, args, env, mainModule.getEntryPoint());
        synchronized (this) {
            state = STATE_STARTED;
        }
        if (run) {
            try {
                thread.cpu.run();
            } catch (ExitThreadException e) {
                thread.cleanup();
            }
        }
    }

    public void setupStack(WineThread thread, Vector<String> args, Vector<String> env, int eip) {
        int[] pArgs = new int[args.size()];
        int[] pEnv = new int[env.size()];

        for (int i=0;i<args.size();i++) {
            String arg = args.get(i);
            thread.cpu.esp.dword-=arg.length()+1;
            thread.process.memory.writeCString(thread.cpu.esp.dword, arg);
            pArgs[i]=thread.cpu.esp.dword;
        }
        for (int i=0;i<env.size();i++) {
            String e = env.get(i);
            thread.cpu.esp.dword-=e.length()+1;
            thread.process.memory.writeCString(thread.cpu.esp.dword, e);
            pEnv[i]=thread.cpu.esp.dword;
        }

        thread.cpu.push32(0);
        thread.cpu.push32(pArgs[0]);
        for (int i=pEnv.length-1;i>=0;i--)
            thread.cpu.push32(pEnv[i]);
        thread.cpu.push32(0);
        for (int i=pArgs.length-1;i>=0;i--)
            thread.cpu.push32(pArgs[i]);
        thread.cpu.push32(pArgs.length);
        thread.cpu.eip = eip;
        thread.cpu.eax.dword = 0;
        thread.cpu.ecx.dword = 0;
        thread.cpu.edx.dword = 0;
        thread.cpu.ebx.dword = 0;
        thread.cpu.ebp.dword = 0;
        thread.cpu.esi.dword = 0;
        thread.cpu.edi.dword = 0;
    }

    /*
 * cloning flags:
 */
    static public final int CSIGNAL        = 0x000000ff;      /* signal mask to be sent at exit */
    static public final int CLONE_VM       = 0x00000100;      /* set if VM shared between processes */
    static public final int CLONE_FS       = 0x00000200;      /* set if fs info shared between processes */
    static public final int CLONE_FILES    = 0x00000400;      /* set if open files shared between processes */
    static public final int CLONE_SIGHAND  = 0x00000800;      /* set if signal handlers and blocked signals shared */
    static public final int CLONE_PTRACE   = 0x00002000;      /* set if we want to let tracing continue on the child too */
    static public final int CLONE_VFORK    = 0x00004000;      /* set if the parent wants the child to wake it up on mm_release */
    static public final int CLONE_PARENT   = 0x00008000;      /* set if we want to have the same parent as the cloner */
    static public final int CLONE_THREAD   = 0x00010000;      /* Same thread group? */
    static public final int CLONE_NEWNS    = 0x00020000;      /* New namespace group? */
    static public final int CLONE_SYSVSEM  = 0x00040000;      /* share system V SEM_UNDO semantics */
    static public final int CLONE_SETTLS   = 0x00080000;      /* create a new TLS for the child */
    static public final int CLONE_PARENT_SETTID    = 0x00100000;      /* set the TID in the parent */
    static public final int CLONE_CHILD_CLEARTID   = 0x00200000;      /* clear the TID in the child */
    static public final int CLONE_DETACHED         = 0x00400000;      /* Unused, ignored */
    static public final int CLONE_UNTRACED         = 0x00800000;      /* set if the tracing process can't force CLONE_PTRACE on this clone */
    static public final int CLONE_CHILD_SETTID     = 0x01000000;      /* set the TID in the child */
/* 0x02000000 was previously the unused CLONE_STOPPED (Start in stopped state)
   and is now available for re-use. */
    static public final int CLONE_NEWUTS           = 0x04000000;      /* New utsname group? */
    static public final int CLONE_NEWIPC           = 0x08000000;      /* New ipcs */
    static public final int CLONE_NEWUSER          = 0x10000000;      /* New user namespace */
    static public final int CLONE_NEWPID           = 0x20000000;      /* New pid namespace */
    static public final int CLONE_NEWNET           = 0x40000000;      /* New network namespace */
    static public final int CLONE_IO               = 0x80000000;      /* Clone io context */



    public int linux_clone(int flags, int child_stack, int ptid, int tls, int ctid) {
        if ((flags & 0xFFFFFF00)==(CLONE_CHILD_SETTID|CLONE_CHILD_CLEARTID)) {
            WineProcess process = new WineProcess(WineSystem.nextid++, this);
            process.parent = this;
            WineThread thread = WineThread.getCurrent().fork(process);

            if ((flags & CLONE_CHILD_SETTID)!=0) {
                if (ctid!=0) {
                    process.memory.writed(ctid, thread.id);
                }
            }
            if ((flags & CLONE_CHILD_CLEARTID)!=0) {
                thread.ctid = ctid;
            }
            if (child_stack!=0)
                thread.cpu.esp.dword = child_stack;
            thread.cpu.eip = thread.cpu.peek32(0);
            thread.cpu.eax.dword = 0;
            thread.jThread.start();
            return process.id;

        } else if ((flags & 0xFFFFFF00) == (CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_SETTLS | CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID | CLONE_SYSVSEM)) {
            WineThread thread = WineThread.getCurrent().fork(this);
            Syscall.modify_ldt_s ldt = new Syscall.modify_ldt_s(memory, tls);
            if (ldt.base_addr!=0) {
                thread.cpu.ldt[ldt.entry_number] = ldt.base_addr;
                thread.cpu.gs.dword = ldt.base_addr;
            }
            thread.ctid = ctid;
            memory.writed(ptid, thread.id);
            thread.cpu.esp.dword = child_stack;
            thread.cpu.esp.dword+=8;
            thread.cpu.eip = thread.cpu.peek32(0);
            thread.cpu.callIndex = 10;
            thread.jThread.start();
            return id;
        } else {
            Log.panic("sys_clone does not implement flags: "+Integer.toHexString(flags));
            return 0;
        }
    }


    // should only return if there is an error
    public int exec(Vector<String> args, Vector<String> env) {
        WineThread thread = WineThread.getCurrent();
        if (args.get(0).endsWith("wine-preloader"))
            args.remove(0);
        String program = args.get(0);
        if (!program.startsWith("/")) {
            for (String p : WineSystem.path) {
                if (FSNode.getNode(p+"/"+program, true)!=null) {
                    args.set(0, p+"/"+program);
                    break;
                }
            }
        }
        args.insertElementAt("/lib/ld-linux.so.2", 0);

        FSNode node = FSNode.getNode(args.get(0), true);
        if (node!=null && !node.exists()) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        if (Log.level>=Log.LEVEL_DEBUG) {
            System.out.print("exec");
            for (int i = 0; i < args.size(); i++) {
                System.out.print(" " + args.get(i));
            }
            System.out.println();
        }
        InputStream fis = null;
        boolean valid = false;
        try {
            fis = node.getInputStream();
            LittleEndianStream is = new LittleEndianStream(fis);
            ElfHeader header = new ElfHeader();
            header.load(is);
            String err = header.isValid();
            if (err == null) {
                valid = true;
            }
        } catch (IOException e) {

        } finally {
            if (fis!=null) try {fis.close();} catch (IOException e){}
        }
        if (!valid) {
            thread.setErrno(Errno.ENOEXEC);
            return -1;
        }

        final Object waitToStart = new Object();

        //memory.close(); // free up memory before allocating new process

        WineProcess process = WineProcess.create(currentDirectory, args, env, id, thread.id, waitToStart);
        for (FileDescriptor fd : ((Hashtable<Integer, FileDescriptor>)fileDescriptors.clone()).values()) {
            if (!fd.closeOnExec()) {
                process.fileDescriptors.put(fd.handle, fd);
                fileDescriptors.remove(fd.handle);
            } else {
                fd.close();
            }
        }
        synchronized (waitToStart) {
            waitToStart.notify();
        }
        synchronized (WineSystem.processes) {
            WineSystem.processes.notify(); // wake up any waitForPid's so that they can wait on the new WineProcess
        }
        for (Integer threadId :threads.keySet()) {
            WineThread t = threads.get(threadId);
            if (t.jThread!=Thread.currentThread())
                t.jThread.stop(new ExitThreadException());
        }
        thread.id = -1;
        threads.clear();
        throw new ExitThreadException();
    }
}

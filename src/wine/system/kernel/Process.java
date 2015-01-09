package wine.system.kernel;

import wine.emulation.*;
import wine.system.ExitThreadException;
import wine.system.SigAction;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.gui.Screen;
import wine.loader.ElfModule;
import wine.loader.Loader;
import wine.loader.elf.ElfHeader;
import wine.system.io.*;
import wine.util.AddressSpace;
import wine.util.Heap;
import wine.util.LittleEndianStream;
import wine.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.util.Hashtable;
import java.util.Vector;

public class Process {
    // the values don't really matter too much, it just makes it easier to debug if we know what might be in a particular address
    static public int ADDRESS_PROCESS_MMAP_START =     0xD0000;
    static public int ADDRESS_PROCESS_STACK_START =    0xE0000;
    static public int ADDRESS_PER_CPU =                0xE1000;
    static public int ADDRESS_PROCESS_CALLBACK_START = 0xE2000;
    static public int ADDRESS_PROCESS_SHARED_START =   0xE3000;
    static public int ADDRESS_PROCESS_DLL_START =      0xE4000;
    static public int ADDRESS_PER_PROCESS =            0xE5000;
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
    final private Heap heap;
    public int callReturnEip;
    public int exitCode;
    public boolean terminated = false;
    public int signaled;
    public int state = STATE_INIT;
    public String name;
    public Process parent;
    // if new variables are added, make sure they are accounted for in fork and exec
    public String fullPath;
    public Vector<String> args;

    public long real_timer_next;
    public long real_timer_current;

    // should this be forked?
    volatile public int signal;
    final public Object signalMutex = new Object();

    final public Thread timerThread = new Thread(new Runnable() {
        public void run() {
            while (true) {
                while (real_timer_current != 0) {
                    try {
                        Thread.sleep(real_timer_current);
                    } catch (InterruptedException e) {

                    }
                    synchronized (timerThread) {
                        if (real_timer_current != 0) {
                            real_timer_current = real_timer_next;
                            signal(Signal.SIGALRM);
                        }
                    }
                }
                try {
                    Thread.sleep(100000);
                } catch (InterruptedException e) {

                }
            }
        }
    });

    public void signal(int signal) {
        WineThread thread = WineThread.getCurrent();
        if (thread!=null)
            thread.signal(signal);
        else {
            synchronized (this.signalMutex) {
                this.signal |= (1l << (signal - 1));
                for (WineThread t : threads.values()) {
                    t.jThread.interrupt();
                }
            }
        }
    }

    static private class ExitFunction {
        public ExitFunction(int func, int arg) {
            this.func = func;
            this.arg = arg;
        }
        public int func;
        public int arg;
    }

    private Process(int id, Process process) {
        loader = process.loader.fork(this);
        this.mainModule = loader.main;
        this.addressSpace = process.addressSpace.deepCopy();
        this.heap = process.heap.deepCopy();
        this.currentDirectory = process.currentDirectory;
        for (FileDescriptor fd : process.fileDescriptors.values()) {
            if (fd.canFork()) {
                FileDescriptor f = fd.fork(this);
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
        this.exitCode = process.exitCode;
        this.terminated = process.terminated;
        this.signaled = process.signaled;
        this.state = process.state;
        this.name = process.name;
        this.fullPath = process.fullPath;
        this.args = process.args;
        WineSystem.processes.put(id, this);
    }

    public Process(String currentDirectory, Vector<String> args, int id, int tid) {
        memory  = new Memory();
        loader = new Loader(this);
        addressSpace = new AddressSpace();
        this.args = args;
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

        if (fileDescriptors.get(0)==null) {
            FileDescriptor tty = getFile("/dev/tty0", true);
            tty.accessFlags = Io.O_RDONLY;
            tty.getFile().open("r");
        }
        if (fileDescriptors.get(1)==null) {
            FileDescriptor tty = getFile("/dev/tty0", true);
            tty.accessFlags = Io.O_WRONLY;
            tty.getFile().open("w");
        }
        if (fileDescriptors.get(2)==null) {
            FileDescriptor tty = getFile("/dev/tty0", true);
            tty.accessFlags = Io.O_WRONLY;
            tty.getFile().open("w");
        }

        Screen.initProcess(this);
        thread.init();
        mainModule = (ElfModule)loader.loadModule(thread, args.get(0));
        String exe = args.elementAt(1);
        if (exe.indexOf("/")>=0)
            this.name = exe.substring(exe.lastIndexOf("/")+1);
        else
            this.name = exe;

        VirtualFSNode.addVirtualFile("/proc/"+id+"/cmdline", new ProcCommandLine(id), KernelStat._S_IREAD);
    }
    private void cleanup() {
        Vector<FileDescriptor> fds = new Vector<FileDescriptor>();
        for (FileDescriptor fd : this.fileDescriptors.values()) {
            fds.add(fd);
        }
        for (FileDescriptor fd : fds) {
            fd.close();
        }
        memory.close();
        mainModule = null;
        loader = null;
        fileDescriptors = null;
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
                    memory.handlers[pageStart + i] = RAMHandler.create(page, RAMHandler.READ | RAMHandler.WRITE);
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
            addressSpace.allocPages(pageStart, pages);
            Log.panic("Failed to find address for " + pages+" pages @0x" + Integer.toHexString(pageStart<<12));
        }
        for (int i=0;i<pages;i++) {
            int page = RAM.allocPage();
            memory.handlers[pageStart+i] = RAMHandler.create(page, RAMHandler.READ|RAMHandler.WRITE|(map?RAMHandler.MAPPED:0));
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
        int id = 0;
        while (true) {
            if (fileDescriptors.get(id)==null)
                return id;
            id++;
        }
    }

    public int getNextFileDescriptor(int id) {
        if (id<3)
            id=3;
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
        FileDescriptor fd = fileDescriptors.get(new Integer(handle));
//        if (fd == null && handle == 2) {
//            if (fileDescriptors.get(2)==null) {
//                fd = getFile("/dev/tty0", true);
//                fd.accessFlags = Io.O_WRONLY;
//                fd.getFile().open("w");
//                fileDescriptors.put(2, fd);
//            }
//        }
        return fd;
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

    public static Process create(final String currentDirectory, final Vector<String> args, final Vector<String> env) {
        return create(currentDirectory, args, env, WineSystem.nextid++, WineSystem.nextid++, null);
    }

    private static Process create(final String currentDirectory, final Vector<String> args, final Vector<String> env, final int pid, final int tid, final Object waitToStart) {
        final Object[] result = new Object[1];

        Thread thread = new Thread(new Runnable() {
            public void run() {
                final Process process = new Process(currentDirectory, args, pid, tid);
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
            return (Process)result[0];
        return null;
    }

    private void start(WineThread thread, Vector<String> args, Vector<String> env, boolean run) {
        fullPath = args.get(1);
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
            thread.cpu.esp.dword-=(arg.length()+1+3) & ~3;
            thread.process.memory.writeCString(thread.cpu.esp.dword, arg);
            pArgs[i]=thread.cpu.esp.dword;
        }
        for (int i=0;i<env.size();i++) {
            String e = env.get(i);
            thread.cpu.esp.dword-=(e.length()+1+3) & ~3;
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
            Process process = new Process(WineSystem.nextid++, this);
            process.parent = this;
            WineThread thread = WineThread.getCurrent().fork(process);

            initStdio();
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

    private void initStdio() {
        if (fileDescriptors.get(0)==null) {
            FileDescriptor tty = getFile("/dev/tty0", true);
            tty.accessFlags = Io.O_RDONLY;
            tty.getFile().open("r");
            fileDescriptors.put(0, tty);
        }
        if (fileDescriptors.get(1)==null) {
            FileDescriptor tty = getFile("/dev/tty0", true);
            tty.accessFlags = Io.O_WRONLY;
            tty.getFile().open("w");
            fileDescriptors.put(1, tty);
        }
        if (fileDescriptors.get(2)==null) {
            FileDescriptor tty = getFile("/dev/tty0", true);
            tty.accessFlags = Io.O_WRONLY;
            tty.getFile().open("w");
            fileDescriptors.put(2, tty);
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
        FSNode node = FSNode.getNode(args.elementAt(0), true);
        for (String path : WineSystem.path) {
            if (node!=null)
                break;
            node = FSNode.getNode(path+"/"+args.elementAt(0), true);
        }
        if (node==null) {
            return -Errno.ENOENT;
        }
        InputStream fis = null;
        boolean isElf = false;
        try {
            fis = node.getInputStream();
            LittleEndianStream is = new LittleEndianStream(fis);
            ElfHeader header = new ElfHeader();
            header.load(is);
            String err = header.isValid();
            if (err == null) {
                isElf = true;
            }
        } catch (Exception e) {

        } finally {
            if (fis != null)
                try {fis.close();} catch (Exception e) {}
        }
        if (isElf)
            args.insertElementAt("/lib/ld-linux.so.2", 0);
        else {
            args.insertElementAt("/lib/ld-linux.so.2", 0);
            args.insertElementAt("/bin/sh", 1);
        }

        node = FSNode.getNode(args.get(0), true);
        if (node==null) {
            return -Errno.ENOENT;
        }
        if (Log.level>=Log.LEVEL_DEBUG) {
            System.out.print("exec");
            for (int i = 0; i < args.size(); i++) {
                System.out.print(" " + args.get(i));
            }
            System.out.println();
        }
        fis = null;
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
            return -Errno.ENOEXEC;
        }

        final Object waitToStart = new Object();

        //memory.close(); // free up memory before allocating new process

        Process process = Process.create(currentDirectory, args, env, id, thread.id, waitToStart);
        process.parent = this.parent;
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

    static public int waitpid(WineThread thread, int pid, int status, int options) {
        Process result = null;
        if (pid>0) {
            while (true) {
                Process process = WineSystem.processes.get(pid);
                if (process == null) {
                    return -Errno.ECHILD;
                }
                if (options!=1) {
                    synchronized (WineSystem.processes) {
                        while (true) {
                            // refetch in case the id maps to a new java object
                            process = WineSystem.processes.get(pid);
                            if (process.isStopped() || process.isTerminated()) {
                                WineSystem.processes.remove(process.id);
                                break;
                            }
                            try {
                                WineSystem.processes.wait();
                            } catch (InterruptedException e) {
                                thread.interrupted();
                            }
                        }
                    }
                }
                if (process.isStopped() || process.isTerminated()) {
                    result = process;
                    break;
                }
                if (options==1) {
                    return -Errno.ECHILD;
                }
            }
        } else {
            if (pid==0)
                pid = thread.process.groupId;
            while (true) {
                synchronized (WineSystem.processes) {
                    for (Process p : WineSystem.processes.values()) {
                        if (p.isStopped() || p.isTerminated()) {
                            if (pid == -1) {
                                if (p.parent!=null && p.parent.id != thread.process.id)
                                    continue;
                            } else {
                                if (p.groupId != -pid)
                                    continue;
                            }
                            WineSystem.processes.remove(p.id);
                            result = p;
                            break;
                        }
                    }
                    if (result != null)
                        break;
                    if (options == 1) {
                        return -Errno.ECHILD;
                    }
                    try {
                        WineSystem.processes.wait();
                    } catch (InterruptedException e) {
                        thread.interrupted();
                    }
                }
            }
        }
        if (status!=0) {
            int s = 0;
            if (result.isStopped()) {
                s |= 0x7f;
                s|=((result.signaled & 0xFF)<< 8);
            } else if (result.isTerminated()) {
                s|=((result.exitCode & 0xFF) << 8);
                s|=(result.signaled & 0x7F);
            }
            thread.process.memory.writed(status, s);
        }
        WineSystem.processes.remove(result);
        return result.id;
    }

    static public int execve(WineThread thread, int path, int argv, int envp) {
        Process process = thread.process;
        Vector<String> env = new Vector<String>();
        Vector<String> args = new Vector<String>();

        boolean hasLD = false;
        for (String s : process.memory.readCStringArray(envp)) {
            env.add(s);
            if (s.startsWith("LD_LIBRARY_PATH"))
                hasLD = true;
        }
        if (!hasLD)
            env.add("LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib");
        for (String a : process.memory.readCStringArray(argv))
            args.add(a);

        return process.exec(args, env);
    }

    static public int getpid(WineThread thread) {
        return thread.process.id;
    }

    static public int dup(WineThread thread, int fildes) {
        return dup(thread, fildes, 0);
    }

    static public int dup(WineThread thread, int fildes, int nextId) {
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null) {
            return -Errno.EBADF;
        }
        fd = fd.dup(thread.process.getNextFileDescriptor(nextId));
        thread.process.fileDescriptors.put(fd.handle, fd);
        return fd.handle;
    }

    static public int dup2(WineThread thread, int fildes, int fildes2) {
        FileDescriptor fd = thread.process.getFileDescriptor(fildes);
        if (fd==null || fildes2<0) {
            return -Errno.EBADF;
        }
        if (fildes == fildes2) {
            return fildes;
        }
        FileDescriptor fd2 = thread.process.getFileDescriptor(fildes2);
        if (fd2==null) {
            fd2 = fd.dup(fildes2);
            thread.process.fileDescriptors.put(fd2.handle, fd2);
        } else {
            fd2.dup2(fd);
        }
        return fildes2;
    }

    static public int getcwd(WineThread thread, int buf, int size) {
        byte[] cwd = thread.process.currentDirectory.getBytes();
        if (cwd.length>=size) {
            return -Errno.ERANGE;
        }
        thread.process.memory.memcpy(buf, cwd, 0, cwd.length);
        thread.process.memory.writeb(buf+cwd.length, 0);
        return buf;
    }
}

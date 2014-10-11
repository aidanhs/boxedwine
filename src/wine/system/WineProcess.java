package wine.system;

import wine.builtin.libX11.X11PerProcessData;
import wine.builtin.libc.Errno;
import wine.builtin.libc.Fcntl;
import wine.builtin.libc.Signal;
import wine.emulation.CPU;
import wine.emulation.Memory;
import wine.emulation.RAM;
import wine.emulation.RAMHandler;
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

    static public final int STATE_INIT = 0;
    static public final int STATE_STARTED = 1;
    static public final int STATE_DONE = 2;

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
    public int eipThreadReturn;
    public int passwd;
    public int penviron;
    public int ppenviron;
    public int optind;
    public int optarg;
    public int stdin;
    public int stdout;
    public int stderr;
    private int[] env;
    public final Hashtable<String, String> envByNameValue;
    public final Hashtable<String, Integer> envByNamePos;
    public int startData;
    final private Heap heap;
    public int callReturnEip;
    public int asctime;
    public int tm;
    public int exitCode;
    public boolean terminated = false;
    public int signaled;
    public int perCPUIndex = 0;
    public int p_ctype_b_loc;
    public int pp_ctype_b_loc;
    public int p_ctype_toupper_loc;
    public int pp_ctype_toupper_loc;
    public int p_ctype_tolower_loc;
    public int pp_ctype_tolower_loc;
    public int state = STATE_INIT;
    public String name;
    private Hashtable<Integer, Vector<ExitFunction>> exitFunctions = new Hashtable<Integer, Vector<ExitFunction>>();
    public WineProcess parent;
    public int xerrorHandler;
    private Hashtable<String, Integer> stringMap = new Hashtable<String, Integer>();
    private int stringPage;
    private int stringPagePos;
    // if new variables are addedd, make sure they are accounted for in fork and exec
    public X11PerProcessData x11 = new X11PerProcessData();
    public Vector<Lock> mutexes = new Vector<Lock>();

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
        this.passwd = process.passwd;
        this.penviron = process.penviron;
        this.ppenviron = process.ppenviron;
        this.optind = process.optind;
        this.optarg = process.optarg;
        this.stdin = process.stdin;
        this.stdout = process.stdout;
        this.stderr = process.stderr;
        this.callReturnEip = process.callReturnEip;
        this.env = process.env.clone();
        this.envByNameValue = (Hashtable<String, String>)process.envByNameValue.clone();
        this.envByNamePos = (Hashtable<String, Integer>)process.envByNamePos.clone();
        this.startData = process.startData;

        this.asctime = process.asctime;
        this.tm = process.tm;
        this.exitCode = process.exitCode;
        this.terminated = process.terminated;
        this.signaled = process.signaled;
        this.perCPUIndex = 0;
        this.p_ctype_b_loc = process.p_ctype_b_loc;
        this.pp_ctype_b_loc = process.pp_ctype_b_loc;
        this.p_ctype_toupper_loc = process.p_ctype_toupper_loc;
        this.pp_ctype_toupper_loc = process.pp_ctype_toupper_loc;
        this.p_ctype_tolower_loc = process.p_ctype_tolower_loc;
        this.pp_ctype_tolower_loc = process.pp_ctype_tolower_loc;
        this.state = process.state;
        this.name = process.name;
        for (Integer module : exitFunctions.keySet()) {
            Vector<ExitFunction> funcs = exitFunctions.get(module);
            for (ExitFunction func : funcs) {
                addExitFunction(module, func.func, func.arg);
            }
        }
        this.stringMap = (Hashtable<String, Integer>)process.stringMap.clone();
        this.stringPage = process.stringPage;
        this.stringPagePos = process.stringPagePos;
        WineSystem.processes.put(id, this);
    }

    public WineProcess(String currentDirectory, String name, int id, int tid, String envs[]) {
        memory  = new Memory();
        loader = new Loader(this);
        addressSpace = new AddressSpace();
        heap = new Heap(ADDRESS_PROCESS_HEAP_START<<12, ADDRESS_PROCESS_HEAP_START<<12, 64); // it will grow
        this.envByNameValue= new Hashtable<String, String>();
        envByNamePos = new Hashtable<String, Integer>();
        this.currentDirectory = currentDirectory;
        this.id = id;
        WineSystem.processes.put(id, this);
        WineThread thread = new WineThread(this, 0, 0, 0, 0, tid);
        thread.jThread = Thread.currentThread();
        thread.registerThread();
        init(name, envs, thread);
    }

    public void init(String name, String envs[], WineThread thread) {
        memory.init();
        loader.init();
        addressSpace.clear();
        heap.clear(ADDRESS_PROCESS_HEAP_START<<12, ADDRESS_PROCESS_HEAP_START<<12);
        envByNameValue.clear();
        callReturnEip=CPU.registerCallReturnEip(loader);
        envByNamePos.clear();
        eipThreadReturn = loader.registerFunction(WineThread.wineThreadReturn);
        String userName = "User";
        passwd = alloc(20+userName.length()+1+WineSystem.homeDirectory.length()+1);
        memory.writeCString(passwd+20, "User");
        memory.writed(passwd, passwd+20);
        memory.writed(passwd+4, WineSystem.uid);
        memory.writed(passwd+8, WineSystem.gid);
        memory.writeCString(passwd+20+userName.length()+1, WineSystem.homeDirectory);
        memory.writed(passwd+12, passwd+20+userName.length()+1);
        memory.writed(passwd+16, 0); // shell

        addressSpace.allocPages(ADDRESS_PROCESS_HEAP_START, 0x100000 - ADDRESS_PROCESS_HEAP_START - 1);
        int data = alloc(4096);
        memory.zero(data, 4096);
        optind = data;data+=4;
        memory.writed(optind, 1);
        optarg = data;data+=4;
        stdin = data;data+=4;
        memory.writeb(stdin, 0);
        stderr = data;data+=4;
        memory.writeb(stderr, 2);
        stdout = data;data+=4;
        memory.writeb(stdout, 1);
        ppenviron = data;data+=4;
        penviron = data;
        memory.writed(ppenviron, penviron);
        if (envs!=null) {
            for (int i = 0; i < envs.length; i++)
                setenv(envs[i]);
        }
        setenv("HOME="+WineSystem.homeDirectory);

        if (fileDescriptors.get(0)==null)
            fileDescriptors.put(0, new FileDescriptor(0, KernelObject.stdin, Fcntl.O_RDONLY));
        if (fileDescriptors.get(1)==null)
            fileDescriptors.put(1, new FileDescriptor(0, KernelObject.stdout, Fcntl.O_WRONLY));
        if (fileDescriptors.get(2)==null)
            fileDescriptors.put(2, new FileDescriptor(0, KernelObject.stderr, Fcntl.O_WRONLY));

        thread.init();
        mainModule = (ElfModule)loader.loadModule(thread, name);
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
        synchronized (this) {
            if (threads.size()==1) {
                if (Log.level>=Log.LEVEL_DEBUG)
                    thread.out("Process Terminated");
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
                this.notifyAll();
            }
            threads.remove(thread.id);
        }
    }

    public void _exit(int status) {
        if (Log.level>=Log.LEVEL_DEBUG)
            WineThread.getCurrent().out("_exit "+status);
        this.exitCode = status;
        for (Integer threadId :threads.keySet()) {
            WineThread thread = threads.get(threadId);
            if (thread.jThread!=Thread.currentThread())
                thread.jThread.stop(new ExitThreadException());
        }
        throw new ExitThreadException();
    }

    public void exit(int status) {
        WineThread thread = WineThread.getCurrent();
        for (Vector<ExitFunction> funcs : exitFunctions.values()) {
            for (ExitFunction func : funcs) {
                thread.cpu.call(func.func, func.arg);
            }
        }
        _exit(status);
    }

    public void runAtExit(int func) {
        addExitFunction(this.id, func, 0);
    }

    public int setenv(String env) {
        int pos = env.indexOf('=');
        String key;
        if (pos<=0)
            key = env;
        else
            key = env.substring(0, pos);
        String value = null;

        if (env.length()>pos+1) {
            value = env.substring(pos+1);
        }
        if (value==null)
            envByNameValue.remove(key);
        else
            envByNameValue.put(key, value);
        envByNamePos.clear();
        this.env = new int[envByNameValue.size()];
        Enumeration<String> keys = envByNameValue.keys();
        pos = 0;
        pos+=envByNameValue.size()*4+4;
        int index=0;
        int environ = memory.readd(ppenviron);
        while (keys.hasMoreElements()) {
            key = keys.nextElement();
            value = envByNameValue.get(key);
            String v = key +"="+value;
            memory.writeCString(environ+pos, v);
            memory.writed(environ + index * 4, environ + pos);
            this.env[index]=environ+pos;
            envByNamePos.put(key, environ+pos+key.length()+1);
            pos+=v.length()+1;
        }
        memory.writed(environ + index * 4, 0);
        return 0;
    }

    public int getenv(String name) {
        Integer result = envByNamePos.get(name);
        if (result!=null)
            return result.intValue();
        return 0;
    }

    public boolean isStopped() {
        return false;
    }

    public boolean isTerminated() {
        return terminated;
    }

    public void waitForPid() {
        synchronized (this) {
            if (isStopped() || isTerminated()) {
                WineSystem.processes.remove(id);
                return;
            }
            try {this.wait();} catch (Exception e){}
        }
    }

    public void addExitFunction(int module, int func, int arg) {
        Vector<ExitFunction> funcs = exitFunctions.get(module);
        if (funcs == null) {
            funcs = new Vector<ExitFunction>();
            exitFunctions.put(module, funcs);
        }
        funcs.add(new ExitFunction(func, arg));
    }

    public static WineProcess create(final String currentDirectory, final String[] args, final String[] env) {
        return create(currentDirectory, args, env, WineSystem.nextid++, WineSystem.nextid++, null);
    }

    private static WineProcess create(final String currentDirectory, final String[] args, final String[] env, final int pid, final int tid, final Object waitToStart) {
        final Object[] result = new Object[1];

        Thread thread = new Thread(new Runnable() {
            public void run() {
                final WineProcess process = new WineProcess(currentDirectory, args[0], pid, tid, env);
                if (process.mainModule==null) {
                    synchronized (result) {
                        result.notify();
                    }
                    return;
                }
                result[0] = process;
                args[0] = process.mainModule.fullPath();
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
                    process.start(WineThread.getCurrent(), args, true);
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

    private void start(WineThread thread, String[] args, boolean run) {
        int startLen = 0;

        for (int i=0;i<args.length;i++) {
            startLen+=args[i].length()+1;
        }
        this.startData = alloc(startLen+2);
        startLen=0;
        int[] argPtrs = new int[args.length];
        for (int i=0;i<args.length;i++) {
            memory.writeCString(this.startData +startLen, args[i]);
            argPtrs[i]=this.startData +startLen;
            startLen+=args[i].length()+1;
        }
        startLen++;
        memory.writeb(this.startData+startLen, 0);

        start(thread, argPtrs, env, run);
    }

    public void start(WineThread thread, int[] args, int[] env, boolean run) {
        thread.cpu.push32(0);
        for (int i=env.length-1;i>=0;i--)
            thread.cpu.push32(env[i]);
        thread.cpu.push32(0);
        for (int i=args.length-1;i>=0;i--)
            thread.cpu.push32(args[i]);
        thread.cpu.push32(args.length);
        thread.cpu.eip = mainModule.getEntryPoint();
        thread.cpu.eax.dword = 0;
        thread.cpu.ecx.dword = 0;
        thread.cpu.edx.dword = 0;
        thread.cpu.ebx.dword = 0;
        thread.cpu.ebp.dword = 0;
        thread.cpu.esi.dword = 0;
        thread.cpu.edi.dword = 0;
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

    // INHERITED
    // file descriptors
    // current thread
    //
    // NOT INHERITED
    // file locks
    // directory change notification
    // timers
    // other threads
    public WineProcess fork(int eip) {
        if (threads.size()>1) {
            // :TODO: what should we do if we have more than one thread, do we clean up the memory the other threads allocated?
            Log.warn("fork doesn't handle a process with more than one thread correctly, memory was leaked");
        }
        WineProcess process = new WineProcess(WineSystem.nextid++, this);
        process.parent = this;
        WineThread thread = WineThread.getCurrent().fork(process);

        thread.cpu.eip = eip;
        thread.cpu.eax.dword = 0;
        thread.jThread.start();
        return process;
    }

    // should only return if there is an error
    public int exec(String[] args, String[] env) {
        WineThread thread = WineThread.getCurrent();
        if (args[0].endsWith("wine-preloader")) {
            String[] t = new String[args.length-1];
            System.arraycopy(args, 1, t, 0, t.length);
            args = t;
        }
        FSNode node = FSNode.getNode(args[0], true);
        if (node!=null && !node.exists()) {
            thread.setErrno(Errno.ENOENT);
            return -1;
        }
        if (Log.level>=Log.LEVEL_DEBUG) {
            System.out.print("exec");
            for (int i = 0; i < args.length; i++) {
                System.out.print(" " + args[i]);
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
        if (env!=null) {
            for (int i = 0; i < env.length; i++) {
                setenv(env[i]);
            }
        }
        Vector<String> envTmp = new Vector<String>();
        for (String name : this.envByNameValue.keySet()) {
            envTmp.add(name+"="+this.envByNameValue.get(name));
        }
        env = new String[envTmp.size()];
        envTmp.copyInto(env);

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
        synchronized (this) {
            this.notify(); // wake up any waitForPid's so that they can wait on the new WineProcess
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

    public String[] paths() {
        String values = envByNameValue.get("PATH");
        if (values==null || values.length()==0)
            return new String[0];
        return values.split(";");
    }

    public int getString(String string) {
        Integer address = stringMap.get(string);
        if (address == null) {
            byte[] bytes = string.getBytes();
            int len = bytes.length+1;
            if (stringPage==0 || len>4096-stringPagePos) {
                int page = addressSpace.getNextPage(WineProcess.ADDRESS_PER_PROCESS, 1);
                allocPages(page, 1, false);
                stringPage = page << 12;
                stringPagePos = 0;
            }
            address = stringPage+stringPagePos;
            stringPagePos+=len;
            memory.writeCString(address, string);
        }
        return address;
    }
}

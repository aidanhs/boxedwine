package wine.system;

import wine.builtin.libc.Errno;
import wine.builtin.libc.Fcntl;
import wine.emulation.*;
import wine.loader.ElfModule;
import wine.loader.Loader;
import wine.loader.elf.ElfHeader;
import wine.system.io.FSNode;
import wine.system.io.FileDescriptor;
import wine.system.io.KernelFile;
import wine.system.io.KernelObject;
import wine.util.Heap;
import wine.util.LittleEndianStream;
import wine.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

public class WineProcess {
    // the values don't really matter too much, it just makes it easier to debug if we know what might be in a particular address
    static public long ADDRESS_PROCESS_STACK_START =    0xE0000000l;
    static public long ADDRESS_PER_CPU =                0xE1000000l;
    static public long ADDRESS_PROCESS_CALLBACK_START = 0xE2000000l;
    static public long ADDRESS_PROCESS_SHARED_START =   0xE3000000l;
    static public long ADDRESS_PROCESS_DLL_START =      0xE4000000l;
    static public long ADDRESS_PROCESS_MMAP_START =     0xE4000000l;
    static public long ADDRESS_PROCESS_HEAP_START =     0xF0000000l; // this needs a continuous space, hopefully wine won't use more than 256MB

    static public final int STATE_INIT = 0;
    static public final int STATE_STARTED = 1;
    static public final int STATE_DONE = 2;

    // if new variables are addedd, make sure they are accounted for in fork and exec
    final public Heap addressSpace;
    final public Memory memory;
    final public Hashtable<Integer, WineThread> threads = new Hashtable<Integer, WineThread>();
    final public SigAction[] sigActions = SigAction.create(64);

    public ElfModule mainModule;
    final public Loader loader;
    public String currentDirectory;
    public final Hashtable<Integer, FileDescriptor> fileDescriptors = new Hashtable<Integer, FileDescriptor>();
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
    // if new variables are addedd, make sure they are accounted for in fork and exec

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
        WineSystem.processes.put(id, this);
    }

    public WineProcess(String currentDirectory, String name, int id, String envs[]) {
        memory  = new Memory();
        loader = new Loader(this);
        addressSpace = new Heap(0x1000, 0xFFFFFFFFl);
        heap = new Heap(ADDRESS_PROCESS_HEAP_START, ADDRESS_PROCESS_HEAP_START); // it will grow
        this.envByNameValue= new Hashtable<String, String>();
        envByNamePos = new Hashtable<String, Integer>();
        this.currentDirectory = currentDirectory;
        this.id = id;
        WineSystem.processes.put(id, this);
        WineThread thread = new WineThread(this, 0, 0, 0, 0);
        thread.jThread = Thread.currentThread();
        thread.registerThread();
        init(name, envs, thread);
    }

    public void init(String name, String envs[], WineThread thread) {
        memory.init();
        loader.init();
        addressSpace.clear();
        heap.clear(ADDRESS_PROCESS_HEAP_START, ADDRESS_PROCESS_HEAP_START);
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

        addressSpace.alloc(ADDRESS_PROCESS_HEAP_START, 0x100000000l-ADDRESS_PROCESS_HEAP_START-4096);
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
        memory.close();
    }

    public int alloc(int size) {
        synchronized (heap) {
            int result = (int) heap.alloc(size, false);
            if (result == 0) {
                int pages = (size + 0xFFF) >>> 12;
                int address = (int) heap.grow(pages << 12);
                int pageStart = address >>> 12;
                for (int i = 0; i < pages; i++) {
                    int page = RAM.allocPage();
                    memory.handlers[pageStart + i] = new RAMHandler(memory, page, false, false);
                }
                result = (int) heap.alloc(size, false);
            }
            if (result == 0) {
                Log.panic("Out of memory");
            }
            return result;
        }
    }

    public int getSizeOfAllocation(int address) {
        return heap.getSize(address);
    }

    public void allocPages(int address, int pages, boolean map) {
        if (addressSpace.alloc(address, pages<<12)==0) {
            Log.panic("Failed to find address for " + pages+" pages @0x" + Integer.toHexString(address));
        }
        int pageStart = address>>>12;
        for (int i=0;i<pages;i++) {
            int page = RAM.allocPage();
            memory.handlers[pageStart+i] = new RAMHandler(memory, page, map, false);
        }
    }

    public int mapPage(int page) {
        long address = addressSpace.getNextAddress(ADDRESS_PROCESS_SHARED_START, 4096, true);
        memory.handlers[(int)(address>>>12)] = new RAMHandler(memory, page, false, false);
        return (int)address;
    }

    public void free(int address) {
        if (address==0)
            return;
        synchronized (heap) {
            int size = (int) heap.free(address);
            if (size <= 0) {
                Log.panic("Tried to free invalid address 0x" + Integer.toHexString(address));
            }
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
        return (FileDescriptor)fileDescriptors.get(new Integer(handle));
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
            threads.remove(thread.id);
            if (threads.size()==0) {
                this.terminated = true;
                cleanup();
                // :TODO: need to track parent process
//                if (sigActions[Signal.SIGCHLD].sa_handler==SigAction.SIG_IGN) {
//                    WineSystem.processes.remove(id);
//                } else if (sigActions[Signal.SIGCHLD].sa_handler!=SigAction.SIG_DFL) {
//                    sigActions[Signal.SIGCHLD].call();
//                }
                this.notifyAll();
            }
        }
    }

    public void _exit(int status) {
        this.exitCode = status;
        for (Integer threadId :threads.keySet()) {
            WineThread thread = threads.get(threadId);
            if (thread.jThread!=Thread.currentThread())
                thread.jThread.stop(new ExitThreadException());
        }
        Thread.currentThread().stop(new ExitThreadException());
    }

    public void exit(int status) {
        // :TODO: run at exit function
        _exit(status);
    }

    public void runAtExit(int func) {
        // :TODO:
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
        return create(currentDirectory, args, env, WineSystem.nextid++, null);
    }

    private static WineProcess create(final String currentDirectory, final String[] args, final String[] env, final int pid, final Object waitToStart) {
        final Object[] result = new Object[1];

        Thread thread = new Thread(new Runnable() {
            public void run() {
                final WineProcess process = new WineProcess(currentDirectory, args[0], pid, env);
                if (process.mainModule==null) {
                    synchronized (result) {
                        result.notify();
                    }
                    return;
                }
                result[0] = process;
                args[0] = process.mainModule.fullPath();
                try {
                    synchronized (result) {
                        result.notify();
                    }
                    if (waitToStart==null)
                        process.start(WineThread.getCurrent(), args, true);
                    else {
                        synchronized (waitToStart) {
                            process.start(WineThread.getCurrent(), args, true);
                        }
                    }
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
            Log.panic("fork doesn't handle a process with more than one thread");
        }
        WineProcess process = new WineProcess(WineSystem.nextid++, this);
        WineThread thread = WineThread.getCurrent().fork(process);
        thread.cpu.eip = eip;
        thread.cpu.eax.dword = 0;
        thread.jThread.start();
        return process;
    }

    // should only return if there is an error
    public int exec(String[] args, String[] env) {
        WineThread thread = WineThread.getCurrent();
        if (args[0].equals("/bin/wine-preloader")) {
            String[] t = new String[args.length-1];
            System.arraycopy(args, 1, t, 0, t.length);
            args = t;
            if (env==null) {
                env = new String[]{"WINELOADERNOEXEC=1", "WINEDLLPATH=/lib", "WINEDEBUG=+all"};
            } else {
                t = new String[env.length+3];
                System.arraycopy(env, 0, t, 0, env.length);
                t[env.length]="WINELOADERNOEXEC=1";
                t[env.length+1]="WINEDLLPATH=/lib";
                t[env.length+2]="WINEDEBUG=+all";
            }
        }
        FSNode node = FSNode.getNode(args[0], true);
        if (node!=null && !node.exists()) {
            thread.setErrno(Errno.ENOENT);
            return -1;
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

        for (Integer threadId :threads.keySet()) {
            WineThread t = threads.get(threadId);
            if (t.jThread!=Thread.currentThread())
                t.jThread.stop(new ExitThreadException());
        }

        for (int i=0;i<sigActions.length;i++) {
            if (sigActions[i].sa_handler>1) {
                sigActions[i].sa_handler=SigAction.SIG_DFL;
            }
        }
        for (FileDescriptor fd : ((Hashtable<Integer, FileDescriptor>)fileDescriptors.clone()).values()) {
            if (fd.closeOnExec()) {
                fd.close();
            }
        }
        init(args[0], env, thread);
        args[0]=mainModule.fullPath();
        start(WineThread.getCurrent(), args, false);

        Thread.currentThread().stop(new RestartThreadException());
        return -1; // can't reach here
    }
}
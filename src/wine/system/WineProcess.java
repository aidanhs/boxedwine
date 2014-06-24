package wine.system;

import wine.builtin.libc.Fcntl;
import wine.emulation.CPU;
import wine.emulation.Memory;
import wine.emulation.RAM;
import wine.emulation.RAMHandler;
import wine.loader.ElfModule;
import wine.loader.Loader;
import wine.system.io.*;
import wine.util.Heap;
import wine.util.Log;
import wine.util.Path;

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

    final public Heap addressSpace = new Heap(0x1000, 0xFFFFFFFFl);
    final public Memory memory = new Memory();
    final public Hashtable threads = new Hashtable();
    final public Hashtable memoryMaps = new Hashtable();
    final public SigAction[] sigActions = SigAction.create(64);

    public ElfModule mainModule;
    final public Loader loader;
    public String currentDirectory;
    public Hashtable fileDescriptors = new Hashtable();
    public final int id;
    public final int eipThreadReturn;
    public final int passwd;
    public final int penviron;
    public final int ppenviron;
    public final int optind;
    public final int optarg;
    public final int stdin;
    public final int stdout;
    public final int stderr;
    private int[] env;
    public final Hashtable<String, String> envByNameValue = new Hashtable<String, String>();
    public final Hashtable<String, Integer> envByNamePos = new Hashtable<String, Integer>();
    public int startData;
    final private Heap heap = new Heap(ADDRESS_PROCESS_HEAP_START, ADDRESS_PROCESS_HEAP_START); // it will grow
    public final int callReturnEip;
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

    static private class ExitFunctions {
        public ExitFunctions(int func, int arg) {
            this.func = func;
            this.arg = arg;
        }
        public int func;
        public int arg;
    }
    private Hashtable<Integer, Vector<ExitFunctions>> exitFunctions = new Hashtable<Integer, Vector<ExitFunctions>>();

    public WineProcess(String name, int id, String envs[]) {
        loader = new Loader(this);
        callReturnEip=CPU.registerCallReturnEip(loader);
        currentDirectory = ((Path) FileSystem.paths.elementAt(0)).localPath;
        this.id = id;
        WineSystem.processes.put(id, this);
        eipThreadReturn = loader.registerFunction(WineThread.wineThreadReturn);

        String homeDirectory = ((Path)FileSystem.paths.elementAt(0)).localPath;
        String userName = "User";
        passwd = alloc(20+userName.length()+1+homeDirectory.length()+1);
        memory.writeCString(passwd+20, "User");
        memory.writed(passwd, passwd+20);
        memory.writed(passwd+4, WineSystem.uid);
        memory.writed(passwd+8, WineSystem.gid);
        memory.writeCString(passwd+20+userName.length()+1, homeDirectory);
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
        for (int i=0;i<envs.length;i++)
            setenv(envs[i]);
        setenv("HOME="+homeDirectory);

        WineThread thread = new WineThread(this, 0, 0, 0, 0);
        thread.jThread = Thread.currentThread();
        thread.registerThread();

        fileDescriptors.put(new Integer(0), new FileDescriptor(0, KernelObject.stdin, Fcntl.O_RDONLY));
        fileDescriptors.put(new Integer(1), new FileDescriptor(0, KernelObject.stdout, Fcntl.O_WRONLY));
        fileDescriptors.put(new Integer(2), new FileDescriptor(0, KernelObject.stderr, Fcntl.O_WRONLY));

        mainModule = (ElfModule)loader.loadModule(thread, name);
        this.name = mainModule.name;
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

    public FileDescriptor getFile(String localPath) {
        FSNode node = FSNode.getNode(localPath);
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

    public void _exit(int status) {
        // :TODO: don't run exit functions
    }

    public void exit(int status) {
        // :TODO: run at exit function
        throw new ExitThreadException();
    }

    public void runAtExit(int func) {

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
            if (isStopped() || isTerminated())
                return;
            try {this.wait();} catch (Exception e){}
        }
    }

    public void addExitFunction(int module, int func, int arg) {
        Vector<ExitFunctions> funcs = exitFunctions.get(module);
        if (funcs == null) {
            funcs = new Vector<ExitFunctions>();
            exitFunctions.put(module, funcs);
        }
        funcs.add(new ExitFunctions(func, arg));
    }

    public static WineProcess create(final String[] args, final String[] env) {
        final int pid = WineSystem.nextid++;
        final Object[] result = new Object[1];

        Thread thread = new Thread(new Runnable() {
            public void run() {
                final WineProcess process = new WineProcess(args[0], pid, env);
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
                    process.start(WineThread.getCurrent(), args);
                } catch (ExitThreadException e) {
                } catch (Exception e) {
                    e.printStackTrace();
                }
                synchronized (result) {
                    result.notify();
                }
                synchronized (process) {
                    process.notify();
                }
                System.out.println(args[0]+" has exited");
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

    private void start(WineThread thread, String[] args) {
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

        start(thread, argPtrs, env);
    }

    public void start(WineThread thread, int[] args, int[] env) {
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
        thread.cpu.run();
        synchronized (this) {
            state = STATE_DONE;
        }
    }
}

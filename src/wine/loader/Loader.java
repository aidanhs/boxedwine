package wine.loader;

import wine.builtin.libc.Libc;
import wine.builtin.libdl.Libdl;
import wine.builtin.libm.Libm;
import wine.builtin.libpthread.LibPThread;
import wine.loader.elf.ElfSymbol;
import wine.system.*;
import wine.system.io.FSNode;

import java.util.Hashtable;
import java.util.Vector;

public class Loader {
    public Hashtable<String, Module> modulesByName = new Hashtable<String, Module>();
    public Hashtable<Integer, Module> modulesByHandle = new Hashtable<Integer, Module>();
    public ElfModule main;
    private int callbackPages;
    private Vector<Callback> callbacks = new Vector<Callback>();

    final private WineProcess process;

    public Loader(WineProcess process) {
        this.process = process;
    }

    public void init() {
        modulesByHandle.clear();
        modulesByName.clear();
        main = null;
        callbacks.clear();
        callbackPages = 0;
    }

    public Loader fork(WineProcess process) {
        Loader loader = new Loader(process);
        loader.callbacks = (Vector<Callback>)callbacks.clone();
        loader.callbackPages = callbackPages;
        for (String name : modulesByName.keySet()) {
            Module module = modulesByName.get(name);
            Module forked = module.fork(process);
            if (module == main) {
                loader.main = (ElfModule)forked;
            }
            loader.modulesByName.put(name, forked);
            loader.modulesByHandle.put(module.id, forked);
        }
        return loader;
    }
    public Callback getCallback(int index) {
        return callbacks.elementAt(index);
    }

    public int registerFunction(Callback callback) {
        int page = (callbacks.size()*4) >>> 12;
        if (page>=callbackPages) {
            process.allocPages((int)WineProcess.ADDRESS_PROCESS_CALLBACK_START + 0x1000 * page, 1, false);
            callbackPages++;
        }
        int address = (int)WineProcess.ADDRESS_PROCESS_CALLBACK_START+0x1000*page+(callbacks.size()*4 & 0xFFF);
        process.memory.writed(address, 0x38FE+(callbacks.size()<<16));
        callbacks.add(callback);
        return address;
    }

    private Module load_native_module(WineThread thread, String name) {
        try {
            int pos = name.lastIndexOf('/');
            String explicitPath = null;
            if (pos>=0) {
                explicitPath = name;
                name = name.substring(pos+1);
            }
            if (explicitPath==null) {
                for (String path : WineSystem.libDirs) {
                    FSNode node = FSNode.getNode(path+"/"+name, true);
                    if (node!=null && !node.isDirectory()) {
                        explicitPath = node.localPath;
                        break;
                    }
                }
            }
            if (explicitPath == null) {
                return null;
            }
            ElfModule module = new ElfModule(name, process, WineSystem.nextid++);
            if (main == null) {
                main = module;
                process.mainModule = module;
            }
            if (module.load(thread, explicitPath)) {
                if (main == module) {
                    thread.entryPoint = module.getEntryPoint();
                }
                modulesByName.put(name.toLowerCase(), module);
                modulesByHandle.put(module.id, module);
                module.init(thread);
                return module;
            }
        } catch (ExitThreadException e) {
            throw e;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    private Module load_builtin_module(String name) {
        BuiltinModule module = null;
        if (name.equalsIgnoreCase("libdl.so.2")) {
            module = new Libdl(name, process, WineSystem.nextid++);
        } else if (name.equalsIgnoreCase("libm.so.6")) {
            module = new Libm(name, process, WineSystem.nextid++);
        } else if (name.equalsIgnoreCase("libc.so.6")) {
            module = new Libc(name, process, WineSystem.nextid++);
        } else if (name.equalsIgnoreCase("libpthread.so.0")) {
            module = new LibPThread(name, process, WineSystem.nextid++);
        }
        if (module != null) {
            modulesByName.put(name.toLowerCase(), module);
            modulesByHandle.put(module.id, module);
        }
        return module;
    }

    private Module internalLoadModule(WineThread thread, String name) {
        Module result = modulesByName.get(name.toLowerCase());
        if (result == null)
            result = load_native_module(thread, name);
        if (result == null)
            result = load_builtin_module(name);
        return result;
    }

    // thread can be null if this is the main module
    public Module loadModule(WineThread thread, String name) {
        return internalLoadModule(thread, name);
    }

    public ElfSymbol getSymbol(String name) {
        ElfSymbol symbol = null;
        for (Module module: modulesByHandle.values()) {
            symbol = module.getSymbol(name);
            if (symbol!=null)
                return symbol;
        }
        if (main!=null)
            symbol = main.getSymbol(name);
        return symbol;
    }
}

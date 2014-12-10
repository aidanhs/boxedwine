package wine.loader;

import wine.loader.elf.ElfSymbol;
import wine.system.*;
import wine.system.kernel.Process;

import java.util.Hashtable;
import java.util.Vector;

public class Loader {
    public Hashtable<String, Module> modulesByName = new Hashtable<String, Module>();
    public Hashtable<Integer, Module> modulesByHandle = new Hashtable<Integer, Module>();
    public ElfModule main;
    private int callbackPages;
    private Vector<Callback> callbacks = new Vector<Callback>();

    final private Process process;

    public Loader(Process process) {
        this.process = process;
    }

    public void init() {
        modulesByHandle.clear();
        modulesByName.clear();
        main = null;
        callbacks.clear();
        callbackPages = 0;
    }

    public Loader fork(Process process) {
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
            process.allocPages(Process.ADDRESS_PROCESS_CALLBACK_START + page, 1, false);
            callbackPages++;
        }
        int address = ((Process.ADDRESS_PROCESS_CALLBACK_START+page)<<12)+(callbacks.size()*4 & 0xFFF);
        process.memory.writed(address, 0x38FE+(callbacks.size()<<16));
        callbacks.add(callback);
        return address;
    }

    public void unregisterFunction(int address) {
        callbacks.set((address-(wine.system.kernel.Process.ADDRESS_PROCESS_CALLBACK_START<<12))/4, null);
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
//                for (String path : WineSystem.libDirs) {
//                    FSNode node = FSNode.getNode(path+"/"+name, true);
//                    if (node!=null && !node.isDirectory()) {
//                        explicitPath = node.localPath;
//                        break;
//                    }
//                }
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
                    process.end = module.getAddress()+module.getImageSize();
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

    private Module internalLoadModule(WineThread thread, String name) {
        Module result = modulesByName.get(name.toLowerCase());
        if (result == null)
            result = load_native_module(thread, name);
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

    public boolean unload(Module module) {
        modulesByName.remove(module.name);
        modulesByHandle.remove(module.id);
        module.unload();
        return true;
    }

    public String getModule(int eip) {
        for (Module module: modulesByHandle.values()) {
            if (module instanceof ElfModule) {
                ElfModule elf = (ElfModule)module;
                if (eip>=elf.getAddress() && eip<elf.getAddress()+elf.getImageSize()) {
                    int offset = elf.getAddress() - elf.getOriginalAddress();
                    return elf.name+" 0x"+Integer.toHexString(eip-offset);
                }
            }
        }
        return "";
    }
}

package wine.builtin.libdl;

import wine.loader.ElfModule;
import wine.loader.Module;
import wine.loader.elf.ElfSymbol;
import wine.system.WineThread;
import wine.util.Log;

public class Dlfcn {
    static private final int RTLD_NOW = 2;

//    typedef struct {
//        const char *dli_fname;  /* Pathname of shared object that contains address */
//        void       *dli_fbase;  /* Address at which shared object is loaded */
//        const char *dli_sname;  /* Name of nearest symbol with address lower than addr */
//        void       *dli_saddr;  /* Exact address of symbol named in dli_sname */
//    } Dl_info;

    // int dladdr(const void * addr, Dl_info * dlip) {
    static public int dladdr(int address, int dlip) {
        WineThread thread = WineThread.getCurrent();
        Log.warn("dladdr only partially implemented");
        for (Module module: thread.process.loader.modulesByHandle.values()) {
            if (module instanceof ElfModule) {
                ElfModule elf = (ElfModule)module;
                if (address>=elf.getAddress() && address<=elf.getAddress()+elf.getImageSize()) {
                    if (elf.fullPath==0) {
                        elf.fullPath = thread.process.alloc(elf.fullPath().length()+1);
                        thread.process.memory.writeCString(elf.fullPath, elf.fullPath());
                    }
                    thread.process.memory.writed(dlip, elf.fullPath);
                    thread.process.memory.writed(dlip+4, elf.getAddress());
                    thread.process.memory.writed(dlip+8, 0);
                    thread.process.memory.writed(dlip+12, address);
                    return 1;
                }
            }
        }
        return 0;
    }

    // void *dlopen(const char *file, int mode)
    static public int dlopen(int file, int mode) {
        if (mode != RTLD_NOW) {
            Log.panic("dlopen mode "+mode+" not implemented");
        }
        WineThread thread = WineThread.getCurrent();
        String name = thread.process.memory.readCString(file);
        Module module = thread.process.loader.loadModule(thread, name);
        if (module==null) {
            return 0;
        }
        return module.id.intValue();
    }

    // char *dlerror(void)
    static public int dlerror() {
        return 0;
    }

    // void *dlsym(void * handle, const char * name)
    static public int dlsym(int handle, int pName) {
        WineThread thread = WineThread.getCurrent();
        Module module = thread.process.loader.modulesByHandle.get(new Integer(handle));
        if (module == null) {
            return 0;
        }
        String name = thread.process.memory.readCString(pName);
        ElfSymbol symbol = module.getSymbol(name);
        if (symbol==null) {
            Log.log("dlsym: could not find "+module.name+":"+name);
        }
        return (int)symbol.st_value;
    }

    // int dlclose(void *handle)
    static public int dlclose(int handle) {
        WineThread thread = WineThread.getCurrent();
        Module module = (Module)thread.process.loader.modulesByHandle.get(new Integer(handle));
        if (module == null) {
            return -1;
        }
        Log.log("dlclose not implement: "+module.name);
        return 0;
    }
}

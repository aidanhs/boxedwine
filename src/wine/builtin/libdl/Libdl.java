package wine.builtin.libdl;

import wine.loader.BuiltinModule;
import wine.system.WineProcess;

public class Libdl extends BuiltinModule {
    public Libdl(String name, WineProcess process, int id) {
        super(name, process, id);

        add_cdecl(Dlfcn.class, "dladdr");
        add_cdecl(Dlfcn.class, "dlopen");
        add_cdecl(Dlfcn.class, "dlerror");
        add_cdecl(Dlfcn.class, "dlsym");
        add_cdecl(Dlfcn.class, "dlclose");
    }
}

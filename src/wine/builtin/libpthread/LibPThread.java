package wine.builtin.libpthread;

import wine.loader.BuiltinModule;
import wine.system.WineProcess;

public class LibPThread extends BuiltinModule {
    public LibPThread(String name, WineProcess process, int id) {
        super(name, process, id);

        add_cdecl(PThread.class, "pthread_attr_destroy");
        add_cdecl(PThread.class, "pthread_attr_init");
        add_cdecl(PThread.class, "pthread_create");
        add_cdecl(PThread.class, "pthread_exit");
        add_cdecl(PThread.class, "pthread_join");
        add_cdecl(PThread.class, "pthread_self");
        add_cdecl(PThread.class, "pthread_attr_setscope");
        add_cdecl(PThread.class, "pthread_attr_setstack");
    }
}

package wine.builtin.libm;

import wine.loader.BuiltinModule;
import wine.system.WineProcess;

public class Libm extends BuiltinModule {
    public Libm(String name, WineProcess process, int id) {
        super(name, process, id);

        add_cdecl(Math.class, "atan");
        add_cdecl(Math.class, "atan2");
        add_cdecl(Math.class, "ceil");
        add_cdecl(Math.class, "cos");
        add_cdecl(Math.class, "fabs");
        add_cdecl(Math.class, "floor");
        add_cdecl(Math.class, "hypot");
        add_cdecl(Math.class, "log");
        add_cdecl(Math.class, "logf");
        add_cdecl(Math.class, "pow");
        add_cdecl(Math.class, "sin");
        add_cdecl(Math.class, "sincos");
        add_cdecl(Math.class, "tan");
        add_cdecl(Math.class, "sqrt");
        add_cdecl(Math.class, "sqrtf");
    }
}
package wine.builtin.libm;

import wine.loader.BuiltinModule;
import wine.system.WineProcess;

public class Libm extends BuiltinModule {
    public Libm(String name, WineProcess process, int id) {
        super(name, process, id);

        add_cdecl(Math.class, "acos");
        add_cdecl(Math.class, "acosf");
        add_cdecl(Math.class, "asin");
        add_cdecl(Math.class, "asinf");
        add_cdecl(Math.class, "atan");
        add_cdecl(Math.class, "atanf");
        add_cdecl(Math.class, "atan2");
        add_cdecl(Math.class, "cbrt");
        add_cdecl(Math.class, "ceil");
        add_cdecl(Math.class, "ceilf");
        add_cdecl(Math.class, "cos");
        add_cdecl(Math.class, "cosf");
        add_cdecl(Math.class, "cosh");
        add_cdecl(Math.class, "exp");
        add_cdecl(Math.class, "expf");
        add_cdecl(Math.class, "fabs");
        add_cdecl(Math.class, "floor");
        add_cdecl(Math.class, "floorf");
        add_cdecl(Math.class, "fmod");
        add_cdecl(Math.class, "__fpclassify");
        add_cdecl(Math.class, "frexp");
        add_cdecl(Math.class, "hypot");
        add_cdecl(Math.class, "hypotf");
        add_cdecl(Math.class, "__isinf");
        add_cdecl(Math.class, "__isinff");
        add_cdecl(Math.class, "__isnan");
        add_cdecl(Math.class, "__isnanf");
        add_cdecl(Math.class, "j0");
        add_cdecl(Math.class, "j1");
        add_cdecl(Math.class, "jn");
        add_cdecl(Math.class, "ldexp");
        add_cdecl(Math.class, "ldexpf");
        add_cdecl(Math.class, "log");
        add_cdecl(Math.class, "log10");
        add_cdecl(Math.class, "log10f");
        add_cdecl(Math.class, "logb");
        add_cdecl(Math.class, "logf");
        add_cdecl(Math.class, "modf");
        add_cdecl(Math.class, "nextafter");
        add_cdecl(Math.class, "pow");
        add_cdecl(Math.class, "powf");
        add_cdecl(Math.class, "sin");
        add_cdecl(Math.class, "sinf");
        add_cdecl(Math.class, "sinh");
        add_cdecl(Math.class, "sincos");
        add_cdecl(Math.class, "sqrt");
        add_cdecl(Math.class, "sqrtf");
        add_cdecl(Math.class, "tan");
        add_cdecl(Math.class, "tanf");
        add_cdecl(Math.class, "tanh");
        add_cdecl(Math.class, "y0");
        add_cdecl(Math.class, "y1");
        add_cdecl(Math.class, "yn");
    }
}
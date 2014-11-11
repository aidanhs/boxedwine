package wine.builtin.ld;

import wine.loader.BuiltinModule;
import wine.system.WineProcess;


/* struct rtld_global _rtld_global =
        {
    // Generally the default presumption without further information is an
    // executable stack but this is not true for all platforms.
        ._dl_stack_flags = DEFAULT_STACK_PERMS, PF_R|PF_W|PF_X = 7
        #ifdef _LIBC_REENTRANT
        ._dl_load_lock = _RTLD_LOCK_RECURSIVE_INITIALIZER,
        ._dl_load_write_lock = _RTLD_LOCK_RECURSIVE_INITIALIZER,
        #endif
        ._dl_nns = 1,
        ._dl_ns =
        {
        #ifdef _LIBC_REENTRANT
        [LM_ID_BASE] = { ._ns_unique_sym_table
        = { .lock = _RTLD_LOCK_RECURSIVE_INITIALIZER } }
        #endif
        }
    };
*/
public class Ld extends BuiltinModule {
    public Ld(String name, WineProcess process, int id) {
        super(name, process, id);

        addData("_rtld_global_ro", process.rtld_global_ro);
    }
}

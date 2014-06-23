package wine.builtin.libc;

import wine.system.WineSystem;
import wine.system.WineThread;

/**
 * Created by james on 5/23/14.
 */
public class Pwd {
//    typedef struct passwd {
//        char    *pw_name;
//        uid_t    pw_uid;
//        gid_t    pw_gid;
//        char    *pw_dir;
//        char    *pw_shell;
//    };

    // struct passwd *getpwuid(uid_t uid);
    static public int getpwuid(int id) {
        if (id == WineSystem.uid) {
            return WineThread.getCurrent().process.passwd;
        }
        return 0;
    }
}

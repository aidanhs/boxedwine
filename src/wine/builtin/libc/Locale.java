package wine.builtin.libc;

import wine.util.Log;

public class Locale {
    // struct lconv *localeconv(void)
    static public int localeconv() {
        Log.panic("localeconv not implemented");
        return 0;
    }

    // char *setlocale(int category, const char *locale)
    static public int setlocale(int category, int locale) {
        return 0;
    }
}

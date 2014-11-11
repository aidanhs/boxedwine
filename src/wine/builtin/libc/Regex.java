package wine.builtin.libc;

import wine.util.Log;

public class Regex {
    // int regcomp(regex_t * preg, const char * pattern, int cflags)
    static public int regcomp(int preg, int pattern, int cflags) {
        Log.panic("regcomp not implemented");
        return 0;
    }

    // size_t regerror(int errcode, const regex_t *preg, char *errbuf, size_t errbuf_size)
    static public int regerror(int errcode, int preg, int errbuf, int errbuf_size) {
        Log.panic("regerror not implemented");
        return 0;
    }

    // int regexec(const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags)
    static public int regexec(int preg, int string, int nmatch, int pmatch, int eflags) {
        Log.panic("regexec not implemented");
        return 0;
    }

    // void regfree(regex_t *preg);
    static public void regfree(int preg) {
        Log.panic("regfree not implemented");
    }
}

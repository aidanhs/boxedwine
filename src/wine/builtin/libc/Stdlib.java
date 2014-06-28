package wine.builtin.libc;

import wine.emulation.CPU;
import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.util.Log;

import java.util.Arrays;
import java.util.Comparator;

public class Stdlib {
    // void abort(void)
    static public void abort() {
        WineThread thread = WineThread.getCurrent();
        Log.panic("Abort "+thread.process.name);
    }

    // int abs(int i)
    static public int abs(int i) {
        return java.lang.Math.abs(i);
    }

    // int atexit(void (*func)(void))
    static public int atexit(int func) {
        WineThread thread = WineThread.getCurrent();
        thread.process.runAtExit(func);
        return 0;
    }

    // int atoi(const char *str)
    static public int atoi(int str) {
        return Integer.parseInt(WineThread.getCurrent().process.memory.readCString(str));
    }

    // void *calloc(size_t nelem, size_t elsize)
    static public int calloc(int nelem, int elsize) {
        WineThread thread = WineThread.getCurrent();
        int result = thread.process.alloc(nelem*elsize);
        thread.process.memory.zero(result, nelem*elsize);
        return result;
    }

    // void exit(int status)
    static public void exit(int status) {
        WineThread thread = WineThread.getCurrent();
        thread.process.exit(status);
    }

    // void free(void *ptr)
    static public void free(int ptr) {
        WineThread thread = WineThread.getCurrent();
        thread.process.free(ptr);
    }

    // char *getenv(const char *name)
    static public int getenv(int name) {
        WineThread thread = WineThread.getCurrent();
        return thread.process.getenv(thread.process.memory.readCString(name));
    }

    // long labs(long i)
    static public int labs(int i) {
        return abs(i);
    }

    // void *malloc(size_t size)
    static public int malloc(int size) {
        WineThread thread = WineThread.getCurrent();
        return thread.process.alloc(size);
    }

    // int putenv(char *string)
    static public int putenv(int str) {
        WineProcess process = WineThread.getCurrent().process;
        String pair = process.memory.readCString(str);
        return process.setenv(pair);
    }

    static private class QSortComparator implements Comparator<Integer> {
        private final int compar;
        private final CPU cpu;

        public QSortComparator(int compar, CPU cpu) {
            this.compar = compar;
            this.cpu = cpu;
        }
        public int compare(Integer a, Integer b) {
            return cpu.call(compar, a, b);
        }
    }

    // void qsort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *))
    static public void qsort(int base, int nel, int width, int compar) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        Integer[] items = new Integer[nel];
        for (int i=0;i<nel;i++) {
            items[i] = base+i*width;
        }
        Arrays.sort(items, new QSortComparator(compar, thread.cpu));
        int tmp = thread.process.alloc(nel*width);
        for (int i=0;i<items.length;i++) {
            memory.memcpy(tmp+i*width, items[i].intValue(), width);
        }
        memory.memcpy(base, tmp, nel*width);
        thread.process.free(tmp);
    }
    // void *realloc(void *ptr, size_t size)
    static public int realloc(int ptr, int size) {
        if (size==0) {
            if (ptr!=0) {
                free(ptr);
            } else {
                Log.warn("tried to realloc with ptr=0 and size=0");
            }
        }
        int result = malloc(size);
        if (ptr!=0) {
            WineThread thread = WineThread.getCurrent();
            WineProcess process = thread.process;
            int oldSize = process.getSizeOfAllocation(ptr);
            process.memory.memcpy(result, ptr, Math.min(size, oldSize));
            process.free(ptr);
        }
        return result;
    }

    // long strtol(const char * str, char ** endptr, int base)
    static public int strtol(int str, int endptr, int base) {
        long result = strtoll(str, endptr, base);
        if (result>Integer.MAX_VALUE)
            result = Integer.MAX_VALUE;
        if (result<Integer.MIN_VALUE)
            result = Integer.MIN_VALUE;
        return (int)result;
    }

    static public long strtoll(int str, int endptr, int base) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        String s = memory.readCString(str).toLowerCase();
        StringBuffer buf = new StringBuffer();
        int i;
        boolean neg = false;

        for (i=0;i<s.length();i++) {
            char c = s.charAt(i);
            if (Character.isSpaceChar(c)) {
                if (buf.length()==0)
                    continue;
                else
                    break;
            }
            if (buf.length()==0) {
                if (c=='-') {
                    neg = true;
                    continue;
                } else if (c=='+') {
                    continue;
                }
                if (base==0 || base==16) {
                    if (c == '0') {
                        if (i + 1 < s.length()) {
                            char c1 = s.charAt(i + 1);
                            if (c1 == 'x') {
                                i++;
                                base = 16;
                                continue;
                            }
                        }
                    }
                    if (base==0)
                        base=8;
                    continue;
                }
                base = 10;
            }
            if (base<=10) {
                if (c < '0' || c > '0' + base - 1) {
                    break;
                }
            } else {
                if (c < '0' || c > '9') {
                    if (c<'a' || c>'a'+base-11) {
                        break;
                    }
                }
            }
            buf.append(c);
        }
        if (endptr!=0) {
            memory.writed(endptr, str+i);
        }
        try {
            long result = Long.parseLong(buf.toString());
            if (neg)
                result=-result;
            return result;
        } catch (NumberFormatException e) {
            return 0;
        }
    }
    // unsigned long strtoul(const char * str, char ** endptr, int base)
    static public int strtoul(int str, int endptr, int base) {
        long result = strtoll(str, endptr, base);
        if (result>0xFFFFFFFFl)
            result = 0xFFFFFFFFl;
        if (result<0)
            result = 0;
        return (int)result;
    }

    // int system(const char *command)
    static public int system(int command) {
        Log.warn("system "+WineThread.getCurrent().process.memory.readCString(command)+" was not executed");
        return -1;
    }

    // int unsetenv(const char *name)
    static public int unsetenv(int name) {
        WineProcess process = WineThread.getCurrent().process;
        String pair = process.memory.readCString(name);
        return process.setenv(pair);
    }
}

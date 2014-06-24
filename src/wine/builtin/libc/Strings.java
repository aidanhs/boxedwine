package wine.builtin.libc;

import wine.emulation.Memory;
import wine.system.WineProcess;
import wine.system.WineThread;

import java.util.HashSet;

public class Strings {
    // int memcmp(const void *s1, const void *s2, size_t n);
    static public int memcmp(int s1, int s2, int n) {
        Memory memory = WineThread.getCurrent().process.memory;
        for (int i=0;i<n;i++) {
            int c1 = memory.readb(s1++);
            int c2 = memory.readb(s2++);

            if (c1<c2)
                return -1;
            else if (c1>c2)
                return 1;
        }
        return 0;
    }

    // void *memccpy(void * s1, const void * s2, int c, size_t n)
    static public int memccpy(int s1, int s2, int c, int n) {
        Memory memory = WineThread.getCurrent().process.memory;
        for (int i=0;i<n;i++) {
            int c2 = memory.readb(s2++);
            memory.writeb(s1++, c2);
            if (c2==c) {
                return s1;
            }
        }
        return 0;
    }

    // void *memchr(const void *s, int c, size_t n)
    static public int memchr(int s, int c, int n) {
        Memory memory = WineThread.getCurrent().process.memory;
        for (int i=0;i<n;i++) {
            int v = memory.readb(s);
            if (v==c) {
                return s;
            }
            s++;
        }
        return 0;
    }
    // void *memcpy(void *restrict s1, const void *restrict s2, size_t n)
    static public int memcpy(int s1, int s2, int n) {
        Memory memory = WineThread.getCurrent().process.memory;
        memory.memcpy(s1, s2, n);
        return s1;
    }

    // void *memmove(void *s1, const void *s2, size_t n)
    static public int memmove(int s1, int s2, int n) {
        Memory memory = WineThread.getCurrent().process.memory;
        memory.memcpy(s1, s2, n);
        return s1;
    }

    // void *memset(void *s, int c, size_t n)
    static public int memset(int s, int c, int n) {
        Memory memory = WineThread.getCurrent().process.memory;
        memory.memset(s, c, n);
        return s;
    }

    // char *stpcpy(char *restrict s1, const char *restrict s2)
    static public int stpcpy(int s1, int s2) {
        Memory memory = WineThread.getCurrent().process.memory;
        while (true) {
            int c1 = memory.readb(s2++);
            memory.writeb(s1, c1);
            if (c1==0)
                break;
            s1++;
        }
        return s1;
    }

    // int strcasecmp(const char *s1, const char *s2)
    static public int strcasecmp(int s1, int s2) {
        Memory memory = WineThread.getCurrent().process.memory;
        while (true) {
            int c1 = CType.tolower(memory.readb(s1++));
            int c2 = CType.tolower(memory.readb(s2++));

            if (c1<c2)
                return -1;
            else if (c1>c2)
                return 1;

            if (c1 == 0 && c1 == c2) {
                return 0;
            }
            if (c1 == 0)
                return -1;
            if (c2 == 0)
                return 1;
        }
    }

    // int strncasecmp(const char *s1, const char *s2, size_t n)
    static public int strncasecmp(int s1, int s2, int n) {
        Memory memory = WineThread.getCurrent().process.memory;
        for (int i=0;i<n;i++) {
            int c1 = CType.tolower(memory.readb(s1++));
            int c2 = CType.tolower(memory.readb(s2++));

            if (c1<c2)
                return -1;
            else if (c1>c2)
                return 1;

            if (c1 == 0 && c1 == c2) {
                return 0;
            }
            if (c1 == 0)
                return -1;
            if (c2 == 0)
                return 1;
        }
        return 0;
    }

    // char *strcat(char *restrict s1, const char *restrict s2)
    static public int strcat(int s1, int s2) {
        int address = s1+strlen(s1);
        Memory memory = WineThread.getCurrent().process.memory;
        while (true) {
            int c1 = memory.readb(s2++);
            memory.writeb(address++, c1);
            if (c1==0)
                break;
        }
        return s1;
    }

    // char *strncat(char * s1, const char * s2, size_t n)
    static public int strncat(int s1, int s2, int n) {
        int address = s1+strlen(s1);
        Memory memory = WineThread.getCurrent().process.memory;
        for (int i=0;i<n-1;i++) {
            int c1 = memory.readb(s2++);
            memory.writeb(address++, c1);
            if (c1==0)
                return s1;
        }
        memory.writeb(address, 0);
        return s1;
    }
    // char *strchr(const char *s, int c)
    static public int strchr(int s, int c) {
        Memory memory = WineThread.getCurrent().process.memory;
        while (true) {
            int c1 = memory.readb(s);
            if (c1==c)
                return s;
            if (c1==0)
                return 0;
            s++;
        }
    }

    // int strcmp(const char *s1, const char *s2)
    static public int strcmp(int s1, int s2) {
        Memory memory = WineThread.getCurrent().process.memory;
        while (true) {
            int c1 = memory.readb(s1++);
            int c2 = memory.readb(s2++);

            if (c1<c2)
                return -1;
            else if (c1>c2)
                return 1;

            if (c1 == 0 && c1 == c2) {
                return 0;
            }
            if (c1 == 0)
                return -1;
            if (c2 == 0)
                return 1;
        }
    }

    // char *strcpy(char *restrict s1, const char *restrict s2)
    static public int strcpy(int s1, int s2) {
        stpcpy(s1, s2);
        return s1;
    }

    // size_t strcspn(const char *s1, const char *s2)
    static public int strcspn(int s1, int s2) {
        Memory memory = WineThread.getCurrent().process.memory;
        int count = 0;
        while (strchr (s2, memory.readb(s1++)) == 0) {
            count++;
        }
        return count;
    }

    // char * __strdup(const char * string);
    static public int __strdup(int s) {
        WineProcess process = WineThread.getCurrent().process;
        int len = strlen(s);
        int result = process.alloc(len+1);
        strcpy(result, s);
        return result;
    }

    // size_t strlen(const char *s)
    static public int strlen(int s) {
        Memory memory = WineThread.getCurrent().process.memory;
        int len = 0;
        while (true) {
            if (memory.readb(s++) == 0)
                return len;
            len++;
        }
    }

    // int strncmp(const char *s1, const char *s2, size_t n)
    static public int strncmp(int s1, int s2, int n) {
        Memory memory = WineThread.getCurrent().process.memory;
        for (int i=0;i<n;i++) {
            int c1 = memory.readb(s1++);
            int c2 = memory.readb(s2++);

            if (c1<c2)
                return -1;
            else if (c1>c2)
                return 1;

            if (c1 == 0 && c1 == c2) {
                return 0;
            }
            if (c1 == 0)
                return -1;
            if (c2 == 0)
                return 1;
        }
        return 0;
    }

    // char *strncpy(char * s1, const char * s2, size_t n)
    static public int strncpy(int s1, int s2, int n) {
        Memory memory = WineThread.getCurrent().process.memory;
        int result = s1;
        int i=0;
        for (;i<n;i++) {
            int c = memory.readb(s2++);
            memory.writeb(s1++, c);
            if (c==0)
                break;
        }
        for (;i<n;i++) {
            memory.writeb(s1++, 0);
        }
        return result;
    }

    // char *strpbrk(const char *s1, const char *s2)
    static public int strpbrk(int s1, int s2) {
        Memory memory = WineThread.getCurrent().process.memory;
        if (s1==0 || memory.readb(s1)==0) {
            return 0;
        }

        while (true) {
            int sValue = memory.readb(s1);
            if (sValue==0)
                return 0;
            int c=s2;
            int cValue;
            while (true) {
                cValue = memory.readb(c++);
                if (cValue == 0)
                    break;
                if (sValue == cValue)
                    return s1;
            }
            s1++;
        }
    }

    // char *strrchr(const char *s, int c)
    static public int strrchr(int s, int c) {
        Memory memory = WineThread.getCurrent().process.memory;
        int result = 0;
        while (true) {
            int c1 = memory.readb(s);
            if (c1==c)
                result = s;
            if (c1==0)
                return result;
            s++;
        }
    }

    // size_t strspn(const char *s1, const char *s2)
    static public int strspn(int s1, int s2) {
        Memory memory = WineThread.getCurrent().process.memory;
        int count = 0;
        HashSet<Integer> set = new HashSet<Integer>();
        while (true) {
            int c = memory.readb(s2++);
            if (c==0)
                break;
            set.add(c);
        }
        while (true) {
            int c = memory.readb(s1++);
            if (c==0)
                break;
            if (set.contains(c))
                count++;
            else
                break;
        }
        return count;
    }

    // char *strstr(const char *s1, const char *s2)
    static public int strstr(int s1, int s2) {
        Memory memory = WineThread.getCurrent().process.memory;
        if (s2 == 0 || s1==0) {
            return s1;
        }
        String string1 = memory.readCString(s1);
        String string2 = memory.readCString(s2);
        int pos = string1.indexOf(string2);
        if (pos<0)
            return 0;
        return s1+pos;
    }
}

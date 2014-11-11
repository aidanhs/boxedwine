package wine.builtin.libc;

import wine.emulation.RAM;
import wine.system.WineProcess;
import wine.system.WineThread;

public class CType {
    static private int ramPage = -1;

    static private short X(int i) {
        return (short)(((i & 0xFF) << 8)|((i>> 8) & 0xFF));
    }
    static private final short ctypeTable[] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),
            X(0x200),X(0x320),X(0x220),X(0x220),X(0x220),X(0x220),X(0x200),X(0x200),
            X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),
            X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),X(0x200),
            X(0x160),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),
            X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),
            X(0x8d8),X(0x8d8),X(0x8d8),X(0x8d8),X(0x8d8),X(0x8d8),X(0x8d8),X(0x8d8),
            X(0x8d8),X(0x8d8),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),
            X(0x4c0),X(0x8d5),X(0x8d5),X(0x8d5),X(0x8d5),X(0x8d5),X(0x8d5),X(0x8c5),
            X(0x8c5),X(0x8c5),X(0x8c5),X(0x8c5),X(0x8c5),X(0x8c5),X(0x8c5),X(0x8c5),
            X(0x8c5),X(0x8c5),X(0x8c5),X(0x8c5),X(0x8c5),X(0x8c5),X(0x8c5),X(0x8c5),
            X(0x8c5),X(0x8c5),X(0x8c5),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),
            X(0x4c0),X(0x8d6),X(0x8d6),X(0x8d6),X(0x8d6),X(0x8d6),X(0x8d6),X(0x8c6),
            X(0x8c6),X(0x8c6),X(0x8c6),X(0x8c6),X(0x8c6),X(0x8c6),X(0x8c6),X(0x8c6),
            X(0x8c6),X(0x8c6),X(0x8c6),X(0x8c6),X(0x8c6),X(0x8c6),X(0x8c6),X(0x8c6),
            X(0x8c6),X(0x8c6),X(0x8c6),X(0x4c0),X(0x4c0),X(0x4c0),X(0x4c0),X(0x200),
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };

    // const unsigned short * * __ctype_b_loc (void)
    static public int __ctype_b_loc() {
        WineProcess process = WineThread.getCurrent().process;
        if (process.p_ctype_b_loc==0) {
            if (ramPage==-1) {
                ramPage = RAM.allocPage();
                int ramAddress = ramPage << 12;
                for (int i=0;i<ctypeTable.length;i++) {
                    RAM.writew(ramAddress, ctypeTable[i]);
                    ramAddress+=2;
                }
            }
            RAM.incrementRef(ramPage);

            process.p_ctype_b_loc = process.mapPage(ramPage);
            process.pp_ctype_b_loc = process.alloc(4);
            process.memory.writed(process.pp_ctype_b_loc, process.p_ctype_b_loc+256);
        }
        return process.pp_ctype_b_loc;
    }

    static final private int upperTable[] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
            16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
            32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
            48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
            64,
            'A','B','C','D','E','F','G','H','I','J','K','L','M',
            'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
            91,92,93,94,95,96,
            'A','B','C','D','E','F','G','H','I','J','K','L','M',
            'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
            123,124,125,126,127,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };

    // const int32_t **__ctype_toupper_loc(void)
    static public int __ctype_toupper_loc() {
        WineProcess process = WineThread.getCurrent().process;
        if (process.p_ctype_toupper_loc==0) {
            int start = ctypeTable.length*2;
            if (ramPage==-1) {
                ramPage = RAM.allocPage();
                int ramAddress = ramPage << 12;
                ramAddress+=start;
                for (int i=0;i<upperTable.length;i++) {
                    RAM.writed(ramAddress, upperTable[i]);
                    ramAddress+=4;
                }
            }
            RAM.incrementRef(ramPage);
            process.p_ctype_toupper_loc = process.mapPage(ramPage)+start;
            process.pp_ctype_toupper_loc = process.alloc(4);
            process.memory.writed(process.pp_ctype_toupper_loc, process.p_ctype_toupper_loc+512);
        }
        return process.pp_ctype_toupper_loc;
    }

    static private final int[] lowerTabletable = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
            16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
            32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
            48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
            64,
            'a','b','c','d','e','f','g','h','i','j','k','l','m',
            'n','o','p','q','r','s','t','u','v','w','x','y','z',
            91,92,93,94,95,96,
            'a','b','c','d','e','f','g','h','i','j','k','l','m',
            'n','o','p','q','r','s','t','u','v','w','x','y','z',
            123,124,125,126,127,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };

    // int32_t * * __ctype_tolower_loc(void) {
    static public int __ctype_tolower_loc() {
        WineProcess process = WineThread.getCurrent().process;
        if (process.p_ctype_tolower_loc==0) {
            int start = ctypeTable.length*2+upperTable.length*4;
            if (ramPage==-1) {
                ramPage = RAM.allocPage();
                int ramAddress = ramPage << 12;
                ramAddress+=start;
                for (int i=0;i<lowerTabletable.length;i++) {
                    RAM.writed(ramAddress, lowerTabletable[i]);
                    ramAddress+=4;
                }
            }
            RAM.incrementRef(ramPage);
            process.p_ctype_tolower_loc = process.mapPage(ramPage)+start;
            process.pp_ctype_tolower_loc = process.alloc(4);
            process.memory.writed(process.pp_ctype_tolower_loc, process.p_ctype_tolower_loc+512);
        }
        return process.pp_ctype_tolower_loc;
    }

    static public int isalnum(int c) {
        return (((c>='a') && (c<='z')) || ((c>='A') && (c<='Z')) || ((c>='0') && (c<='9')))?1:0;
    }

    static public int isalpha(int c) {
        return ((c >='a' && c <='z') || (c >='A' && c <='Z'))?1:0;
    }

    static public int iscntrl(int c) {
        return ((c==0x7F) || (c>=0 && c<=0x1F))?1:0;
    }

    static public int isdigit(int c) {
        return ((c>='0') && (c<='9'))?1:0;
    }

    static public int isgraph(int c) {
        return (c>0x20 && c<=0x7E)?1:0;
    }

    static public int islower(int c) {
        return ((c>='a') && (c<='z'))?1:0;
    }

    static public int isprint(int c) {
        return (c>=0x20 && c<=0x7E)?1:0;
    }

    static public int ispunct(int c) {
        return (isgraph(c)!=0 && isalnum(c)==0)?1:0;
    }

    static public int isspace(int c) {
        return ((c>=0x09 && c<=0x0D) || (c==0x20))?1:0;
    }

    static public int isupper(int c) {
        return ((c>='A') && (c<='Z'))?1:0;
    }

    static public int isxdigit(int c) {
        return(((c>='0') && (c<='9')) || ((c>='A') && (c<='F')) || ((c>='a') && (c<='f')))?1:0;
    }

    static public int tolower(int c) {
        return isupper(c)!=0 ? (c) - 'A' + 'a' : c;
    }

    static public int toupper(int c) {
        return islower(c)!=0 ? c - 'a' + 'A' : c;
    }
}

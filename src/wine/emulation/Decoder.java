package wine.emulation;

import wine.util.Log;

class Decoder {
    private interface Decode {
        public boolean call(CPU cpu, Op prev);
    }
    static Reg eb(CPU cpu, int rm) {
        switch (rm & 7) {
            case 0: return cpu.eax;
            case 1: return cpu.ecx;
            case 2: return cpu.edx;
            case 3: return cpu.ebx;
            case 4: return cpu.ah;
            case 5: return cpu.ch;
            case 6: return cpu.dh;
            case 7: return cpu.bh;
        }
        throw new RuntimeException("eb");
    }
    static Reg gb(CPU cpu, int rm) {
        return eb(cpu, (rm >> 3));
    }
    static Reg ew(CPU cpu, int rm) {
        switch (rm & 7) {
            case 0: return cpu.eax;
            case 1: return cpu.ecx;
            case 2: return cpu.edx;
            case 3: return cpu.ebx;
            case 4: return cpu.esp;
            case 5: return cpu.ebp;
            case 6: return cpu.esi;
            case 7: return cpu.edi;
        }
        throw new RuntimeException("ew");
    }
    static Reg gw(CPU cpu, int rm) {
        return ew(cpu, (rm >> 3));
    }
    static Reg ed(CPU cpu, int rm) {
        return ew(cpu, rm);
    }
    static Reg gd(CPU cpu, int rm) {
        return ew(cpu, (rm >> 3));
    }
    private static EaaBase getEaa32(CPU cpu, int rm) {
        if (rm<0x40) {
            switch (rm & 7) {
                case 0x00: return new Eaa.EA_32_00_n(cpu);
                case 0x01: return new Eaa.EA_32_01_n(cpu);
                case 0x02: return new Eaa.EA_32_02_n(cpu);
                case 0x03: return new Eaa.EA_32_03_n(cpu);
                case 0x04: return new Eaa.EA_32_04_n(cpu);
                case 0x05: return new Eaa.EA_32_05_n(cpu);
                case 0x06: return new Eaa.EA_32_06_n(cpu);
                case 0x07: return new Eaa.EA_32_07_n(cpu);
            }
        } else if (rm<0x80) {
            switch (rm & 7) {
                case 0x00: return new Eaa.EA_32_40_n(cpu);
                case 0x01: return new Eaa.EA_32_41_n(cpu);
                case 0x02: return new Eaa.EA_32_42_n(cpu);
                case 0x03: return new Eaa.EA_32_43_n(cpu);
                case 0x04: return new Eaa.EA_32_44_n(cpu);
                case 0x05: return new Eaa.EA_32_45_n(cpu);
                case 0x06: return new Eaa.EA_32_46_n(cpu);
                case 0x07: return new Eaa.EA_32_47_n(cpu);
            }
        } else {
            switch (rm & 7) {
                case 0x00: return new Eaa.EA_32_80_n(cpu);
                case 0x01: return new Eaa.EA_32_81_n(cpu);
                case 0x02: return new Eaa.EA_32_82_n(cpu);
                case 0x03: return new Eaa.EA_32_83_n(cpu);
                case 0x04: return new Eaa.EA_32_84_n(cpu);
                case 0x05: return new Eaa.EA_32_85_n(cpu);
                case 0x06: return new Eaa.EA_32_86_n(cpu);
                case 0x07: return new Eaa.EA_32_87_n(cpu);
            }
        }
        return null;
    }

    private static EaaBase getEaa16(CPU cpu, int rm) {
        if (rm<0x40) {
            switch (rm & 7) {
                case 0x00: return new Eaa.EA_16_00_n(cpu);
                case 0x01: return new Eaa.EA_16_01_n(cpu);
                case 0x02: return new Eaa.EA_16_02_n(cpu);
                case 0x03: return new Eaa.EA_16_03_n(cpu);
                case 0x04: return new Eaa.EA_16_04_n(cpu);
                case 0x05: return new Eaa.EA_16_05_n(cpu);
                case 0x06: return new Eaa.EA_16_06_n(cpu);
                case 0x07: return new Eaa.EA_16_07_n(cpu);
            }
        } else if (rm<0x80) {
            switch (rm & 7) {
                case 0x00: return new Eaa.EA_16_40_n(cpu);
                case 0x01: return new Eaa.EA_16_41_n(cpu);
                case 0x02: return new Eaa.EA_16_42_n(cpu);
                case 0x03: return new Eaa.EA_16_43_n(cpu);
                case 0x04: return new Eaa.EA_16_44_n(cpu);
                case 0x05: return new Eaa.EA_16_45_n(cpu);
                case 0x06: return new Eaa.EA_16_46_n(cpu);
                case 0x07: return new Eaa.EA_16_47_n(cpu);
            }
        } else {
            switch (rm & 7) {
                case 0x00: return new Eaa.EA_16_80_n(cpu);
                case 0x01: return new Eaa.EA_16_81_n(cpu);
                case 0x02: return new Eaa.EA_16_82_n(cpu);
                case 0x03: return new Eaa.EA_16_83_n(cpu);
                case 0x04: return new Eaa.EA_16_84_n(cpu);
                case 0x05: return new Eaa.EA_16_85_n(cpu);
                case 0x06: return new Eaa.EA_16_86_n(cpu);
                case 0x07: return new Eaa.EA_16_87_n(cpu);
            }
        }
        return null;
    }

    private static EaaBase getEaa(CPU cpu, int rm) {
        if (cpu.ea16())
            return getEaa16(cpu, rm);
        return getEaa32(cpu, rm);
    }

    static private void EbGb(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg8_Reg8(instruction, eb(cpu, rm), gb(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem8_Reg8(instruction, getEaa(cpu, rm), gb(cpu, rm));
        }
    }

    static private void EwGw(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_Reg16(instruction, ew(cpu, rm), gw(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem16_Reg16(instruction, getEaa(cpu, rm), gw(cpu, rm));
        }
    }

    static private void EwGb(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_Reg8(instruction, ew(cpu, rm), gb(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem16_Reg8(instruction, getEaa(cpu, rm), gb(cpu, rm));
        }
    }

    static private void EdGd(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_Reg32(instruction, ed(cpu, rm), gd(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem32_Reg32(instruction, getEaa(cpu, rm), gd(cpu, rm));
        }
    }

    static private void EdGb(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_Reg8(instruction, ed(cpu, rm), gb(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem32_Reg8(instruction, getEaa(cpu, rm), gb(cpu, rm));
        }
    }

    static private void GbEb(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg8_Reg8(instruction, gb(cpu, rm), eb(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Reg8_Mem8(instruction, gb(cpu, rm), getEaa(cpu, rm));
        }
    }

    static private void GwEw(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_Reg16(instruction, gw(cpu, rm), ew(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Reg16_Mem16(instruction, gw(cpu, rm), getEaa(cpu, rm));
        }
    }

    static private void GwEwS8(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_Reg16_Value(instruction, gw(cpu, rm), ew(cpu, rm), cpu.fetchbs());
        } else {
            EaaBase eaa = getEaa(cpu, rm);
            prev.next = new Ops.Instruction_Reg16_Mem16_Value(instruction, gw(cpu, rm), eaa, cpu.fetchbs());
        }
    }

    static private void GwEwS16(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_Reg16_Value(instruction, gw(cpu, rm), ew(cpu, rm), cpu.fetchws());
        } else {
            EaaBase eaa = getEaa(cpu, rm);
            prev.next = new Ops.Instruction_Reg16_Mem16_Value(instruction, gw(cpu, rm), eaa, cpu.fetchws());
        }
    }

    static private void GdEd(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_Reg32(instruction, gd(cpu, rm), ed(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Reg32_Mem32(instruction, gd(cpu, rm), getEaa(cpu, rm));
        }
    }

    static private void GdEdS8(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_Reg32_Value(instruction, gd(cpu, rm), ed(cpu, rm), cpu.fetchbs());
        } else {
            EaaBase eaa = getEaa(cpu, rm);
            prev.next = new Ops.Instruction_Reg32_Mem32_Value(instruction, gd(cpu, rm), eaa, cpu.fetchbs());
        }
    }

    static private void GdEdId(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_Reg32_Value(instruction, gd(cpu, rm), ed(cpu, rm), cpu.fetchd());
        } else {
            EaaBase eaa = getEaa(cpu, rm);
            prev.next = new Ops.Instruction_Reg32_Mem32_Value(instruction, gd(cpu, rm), eaa, cpu.fetchd());
        }
    }

    static private void Eb(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg8(instruction, eb(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem8(instruction, getEaa(cpu, rm));
        }
    }

    static private void EbCustom(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg8_Custom(instruction, eb(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem8_Custom(instruction, getEaa(cpu, rm));
        }
    }

    static private int EbIb(CPU cpu, Op prev, Instruction instruction, int rm) {
        int value;
        if (rm >= 0xc0 ) {
            value = cpu.fetchb();
            prev.next = new Ops.Instruction_Reg8_value(instruction, eb(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchb()
            value = cpu.fetchb();
            prev.next = new Ops.Instruction_Mem8_value(instruction, eaa, value);
        }
        return value;
    }

    static private int EbIbMask(CPU cpu, Op prev, Instruction instruction, int rm, int mask) {
        int value;
        if (rm >= 0xc0 ) {
            value = cpu.fetchb() & mask;
            prev.next = new Ops.Instruction_Reg8_value(instruction, eb(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchb()
            value = cpu.fetchb() & mask;
            prev.next = new Ops.Instruction_Mem8_value(instruction, eaa, value);
        }
        return value;
    }

    static private int EbIb(CPU cpu, Op prev, Instruction instruction, int rm, int value) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg8_value(instruction, eb(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm);
            prev.next = new Ops.Instruction_Mem8_value(instruction, eaa, value);
        }
        return value;
    }

    static private int EbIbMaskOriginal(CPU cpu, Op prev, Instruction instruction, int rm, int mask) {
        int value;
        if (rm >= 0xc0 ) {
            value = cpu.fetchb();
            prev.next = new Ops.Instruction_Reg8_value(instruction, eb(cpu, rm), value & mask);
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchb()
            value = cpu.fetchb();
            prev.next = new Ops.Instruction_Mem8_value(instruction, eaa, value & mask);
        }
        return value;
    }

    static private int EbIbMod(CPU cpu, Op prev, Instruction instruction, int rm, int mod) {
        int value;
        if (rm >= 0xc0 ) {
            value = cpu.fetchb() % mod;
            prev.next = new Ops.Instruction_Reg8_value(instruction, eb(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchb()
            value = cpu.fetchb() % mod;
            prev.next = new Ops.Instruction_Mem8_value(instruction, eaa, value);
        }
        return value;
    }

    static private void Ew(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16(instruction, ew(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem16(instruction, getEaa(cpu, rm));
        }
    }

    static private void EwCustom(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_Custom(instruction, ew(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem16_Custom(instruction, getEaa(cpu, rm));
        }
    }

    static private int EwIw(CPU cpu, Op prev, Instruction instruction, int rm) {
        int value;
        if (rm >= 0xc0 ) {
            value = cpu.fetchw();
            prev.next = new Ops.Instruction_Reg16_value(instruction, ew(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchw()
            value = cpu.fetchw();
            prev.next = new Ops.Instruction_Mem16_value(instruction, eaa, value);
        }
        return value;
    }

    static private int EwIbMask(CPU cpu, Op prev, Instruction instruction, int rm, int mask) {
        int value;
        if (rm >= 0xc0 ) {
            value = cpu.fetchb() & mask;
            prev.next = new Ops.Instruction_Reg16_value(instruction, ew(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchw()
            value = cpu.fetchb() & mask;
            prev.next = new Ops.Instruction_Mem16_value(instruction, eaa, value);
        }
        return value;
    }

    static private int EwIbMaskOriginal(CPU cpu, Op prev, Instruction instruction, int rm, int mask) {
        int value;
        if (rm >= 0xc0 ) {
            value = cpu.fetchb();
            prev.next = new Ops.Instruction_Reg16_value(instruction, ew(cpu, rm), value & mask);
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchw()
            value = cpu.fetchb();
            prev.next = new Ops.Instruction_Mem16_value(instruction, eaa, value & mask);
        }
        return value;
    }

    static private int EwIw(CPU cpu, Op prev, Instruction instruction, int rm, int value) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_value(instruction, ew(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm);
            prev.next = new Ops.Instruction_Mem16_value(instruction, eaa, value);
        }
        return value;
    }

    static private int EwIbMod(CPU cpu, Op prev, Instruction instruction, int rm, int mod) {
        int value;
        if (rm >= 0xc0 ) {
            value = cpu.fetchb() % mod;
            prev.next = new Ops.Instruction_Reg16_value(instruction, ew(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchw()
            value = cpu.fetchb() % mod;
            prev.next = new Ops.Instruction_Mem16_value(instruction, eaa, value);
        }
        return value;
    }

    static private void EwIx(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_value(instruction, ew(cpu, rm), (cpu.fetchbs() & 0xFFFF));
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchbs()
            prev.next = new Ops.Instruction_Mem16_value(instruction, eaa, (cpu.fetchbs() & 0xFFFF));
        }
    }

    static private void Ed(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32(instruction, ed(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem32(instruction, getEaa(cpu, rm));
        }
    }

    static private void EdCustom(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_Custom(instruction, ed(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem32_Custom(instruction, getEaa(cpu, rm));
        }
    }

    static private int EdId(CPU cpu, Op prev, Instruction instruction, int rm) {
        int value;
        if (rm >= 0xc0 ) {
            value = cpu.fetchd();
            prev.next = new Ops.Instruction_Reg32_value(instruction, ed(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchd()
            value = cpu.fetchd();
            prev.next = new Ops.Instruction_Mem32_value(instruction, eaa, value);
        }
        return value;
    }

    static private int EdIbMask(CPU cpu, Op prev, Instruction instruction, int rm, int mask) {
        int value;
        if (rm >= 0xc0 ) {
            value = cpu.fetchb() & mask;
            prev.next = new Ops.Instruction_Reg32_value(instruction, ed(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchd()
            value = cpu.fetchb() & mask;
            prev.next = new Ops.Instruction_Mem32_value(instruction, eaa, value);
        }
        return value;
    }

    static private int EdId(CPU cpu, Op prev, Instruction instruction, int rm, int value) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_value(instruction, ed(cpu, rm), value);
        } else {
            EaaBase eaa = getEaa(cpu, rm);
            prev.next = new Ops.Instruction_Mem32_value(instruction, eaa, value);
        }
        return value;
    }

    static private void EdIx(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_value(instruction, ed(cpu, rm), cpu.fetchbs());
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchbs()
            prev.next = new Ops.Instruction_Mem32_value(instruction, eaa, cpu.fetchbs());
        }
    }

    static private void EbGb_noresult(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg8_Reg8_noresult(instruction, eb(cpu, rm), gb(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem8_Reg8_noresult(instruction, getEaa(cpu, rm), gb(cpu, rm));
        }
    }

    static private void EwGw_noresult(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_Reg16_noresult(instruction, ew(cpu, rm), gw(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem16_Reg16_noresult(instruction, getEaa(cpu, rm), gw(cpu, rm));
        }
    }

    static private void EdGd_noresult(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_Reg32_noresult(instruction, ed(cpu, rm), gd(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Mem32_Reg32_noresult(instruction, getEaa(cpu, rm), gd(cpu, rm));
        }
    }

    static private void GbEb_noresult(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg8_Reg8_noresult(instruction, gb(cpu, rm), eb(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Reg8_Mem8_noresult(instruction, gb(cpu, rm), getEaa(cpu, rm));
        }
    }

    static private void GwEw_noresult(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_Reg16_noresult(instruction, gw(cpu, rm), ew(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Reg16_Mem16_noresult(instruction, gw(cpu, rm), getEaa(cpu, rm));
        }
    }

    static private void GdEd_noresult(CPU cpu, Op prev, Instruction instruction) {
        int rm = cpu.fetchb();
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_Reg32_noresult(instruction, gd(cpu, rm), ed(cpu, rm));
        } else {
            prev.next = new Ops.Instruction_Reg32_Mem32_noresult(instruction, gd(cpu, rm), getEaa(cpu, rm));
        }
    }

    static private void EbIb_noresult(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg8_noresult(instruction, eb(cpu, rm), cpu.fetchb());
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchb()
            prev.next = new Ops.Instruction_Mem8_noresult(instruction, eaa, cpu.fetchb());
        }
    }

    static private void EwIw_noresult(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_noresult(instruction, ew(cpu, rm), cpu.fetchw());
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchw()
            prev.next = new Ops.Instruction_Mem16_noresult(instruction, eaa, cpu.fetchw());
        }
    }

    static private void EwIx_noresult(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg16_noresult(instruction, ew(cpu, rm), (cpu.fetchbs() & 0xFFFF));
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchbs()
            prev.next = new Ops.Instruction_Mem16_noresult(instruction, eaa, (cpu.fetchbs() & 0xFFFF));
        }
    }

    static private void EdId_noresult(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_noresult(instruction, ed(cpu, rm), cpu.fetchd());
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchd()
            prev.next = new Ops.Instruction_Mem32_noresult(instruction, eaa, cpu.fetchd());
        }
    }

    static private void EdIx_noresult(CPU cpu, Op prev, Instruction instruction, int rm) {
        if (rm >= 0xc0 ) {
            prev.next = new Ops.Instruction_Reg32_noresult(instruction, ed(cpu, rm), cpu.fetchbs());
        } else {
            EaaBase eaa = getEaa(cpu, rm); // must come before cpu.fetchbs()
            prev.next = new Ops.Instruction_Mem32_noresult(instruction, eaa, cpu.fetchbs());
        }
    }

    static private void addInstructions(int offset, final Instruction instruction8, final Instruction instruction16, final Instruction instruction32) {
        /* Eb,Gb */
        decoder[offset] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EbGb(cpu, prev, instruction8, cpu.fetchb());
                return true;
            }
        };
        decoder[0x200+offset]=decoder[offset];

        /* Ew,Gw */
        decoder[offset+1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EwGw(cpu, prev, instruction16, cpu.fetchb());
                return true;
            }
        };

        /* Ed,Gd */
        decoder[offset+0x201] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EdGd(cpu, prev, instruction32);
                return true;
            }
        };

        /* Gb,Eb */
        decoder[offset+2] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GbEb(cpu, prev, instruction8);
                return true;
            }
        };
        decoder[0x202+offset]=decoder[offset+2];

        /* Gw,Ew */
        decoder[offset+3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GwEw(cpu, prev, instruction16);
                return true;
            }
        };

        /* Gd,Ed */
        decoder[offset+0x203] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GdEd(cpu, prev, instruction32);
                return true;
            }
        };

        /* AL,Ib */
        decoder[offset+4] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Instruction_Reg8_value(instruction8, cpu.eax, cpu.fetchb());
                return true;
            }
        };
        decoder[0x204+offset]=decoder[offset+4];

        /* AX,Iw */
        decoder[offset+5] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Instruction_Reg16_value(instruction16, cpu.eax, cpu.fetchw());
                return true;
            }
        };

        /* EAX,Id */
        decoder[offset+0x205] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Instruction_Reg32_value(instruction32, cpu.eax, cpu.fetchd());
                return true;
            }
        };
    }

    static private void addInstructions_noresult(int offset, final Instruction instruction8, final Instruction instruction16, final Instruction instruction32) {
        /* Eb,Gb */
        decoder[offset] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EbGb_noresult(cpu, prev, instruction8);
                return true;
            }
        };
        decoder[0x200+offset]=decoder[offset];

        /* Ew,Gw */
        decoder[offset+1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EwGw_noresult(cpu, prev, instruction16);
                return true;
            }
        };

        /* Ed,Gd */
        decoder[offset+0x201] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EdGd_noresult(cpu, prev, instruction32);
                return true;
            }
        };

        /* Gb,Eb */
        decoder[offset+2] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GbEb_noresult(cpu, prev, instruction8);
                return true;
            }
        };
        decoder[0x202+offset]=decoder[offset+2];

        /* Gw,Ew */
        decoder[offset+3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GwEw_noresult(cpu, prev, instruction16);
                return true;
            }
        };

        /* Gd,Ed */
        decoder[offset+0x203] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GdEd_noresult(cpu, prev, instruction32);
                return true;
            }
        };

        /* AL,Ib */
        decoder[offset+4] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Instruction_Reg8_noresult(instruction8, cpu.eax, cpu.fetchb());
                return true;
            }
        };
        decoder[0x204+offset]=decoder[offset+4];

        /* AX,Iw */
        decoder[offset+5] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Instruction_Reg16_noresult(instruction16, cpu.eax, cpu.fetchw());
                return true;
            }
        };

        /* EAX,Id */
        decoder[offset+0x205] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Instruction_Reg32_noresult(instruction32, cpu.eax, cpu.fetchd());
                return true;
            }
        };
    }

    static private void addInstructionForAllRegs16(int offset, final Instruction instruction) {
        for (int i=0;i<8;i++) {
            final int rm = i;
            decoder[offset+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Instruction_Reg16(instruction, ew(cpu, rm));
                    return true;
                }
            };
        }
    }

    static private void addInstructionForAllRegs32(int offset, final Instruction instruction) {
        for (int i=0;i<8;i++) {
            final int rm = i;
            decoder[offset+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Instruction_Reg32(instruction, ed(cpu, rm));
                    return true;
                }
            };
        }
    }

    static private Decode[] decoder = new Decode[1024];
    static private class NullDecoder implements Decode {
        private int op;
        public NullDecoder(int op) {
            this.op = op;
        }
        public boolean call(CPU cpu, Op prev) {
            Log.panic("Instruction "+Integer.toHexString(op)+" not implemented");
            return false;
        }
    }
    static {
        for (int i=0;i<decoder.length;i++) {
            decoder[i] = new NullDecoder(i);
        }
        addInstructions(0x00, Instructions.addb, Instructions.addw, Instructions.addd);
        addInstructions(0x08, Instructions.orb, Instructions.orw, Instructions.ord);

        /* 2 byte opcodes*/
        decoder[0x0f] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                return decoder[cpu.fetchb()+0x100].call(cpu, prev);
            }
        };
        decoder[0x20f] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                return decoder[cpu.fetchb()+0x300].call(cpu, prev);
            }
        };

        addInstructions(0x10, Instructions.adcb, Instructions.adcw, Instructions.adcd);
        addInstructions(0x18, Instructions.sbbb, Instructions.sbbw, Instructions.sbbd);
        addInstructions(0x20, Instructions.andb, Instructions.andw, Instructions.andd);
        addInstructions(0x28, Instructions.subb, Instructions.subw, Instructions.subd);
        addInstructions(0x30, Instructions.xorb, Instructions.xorw, Instructions.xord);
        addInstructions_noresult(0x38, Instructions.subb, Instructions.subw, Instructions.subd);

        /* SEG ES: */
        decoder[0x26] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                cpu.base_ds = cpu.es;
                cpu.base_ss = cpu.es;
                boolean result = decoder[cpu.fetchb()+cpu.op_index].call(cpu, prev);
                cpu.base_ds = cpu.ds;
                cpu.base_ss = cpu.ss;
                return result;
            }
        };
        decoder[0x226] = decoder[0x26];

        /* SEG CS: */
        decoder[0x2e] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                cpu.base_ds = cpu.cs;
                cpu.base_ss = cpu.cs;
                boolean result = decoder[cpu.fetchb()+cpu.op_index].call(cpu, prev);
                cpu.base_ds = cpu.ds;
                cpu.base_ss = cpu.ss;
                return result;
            }
        };
        decoder[0x22e] = decoder[0x2e];

        /* SEG SS: */
        decoder[0x36] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                cpu.base_ds = cpu.ss;
                cpu.base_ss = cpu.ss;
                boolean result = decoder[cpu.fetchb()+cpu.op_index].call(cpu, prev);
                cpu.base_ds = cpu.ds;
                cpu.base_ss = cpu.ss;
                return result;
            }
        };
        decoder[0x236] = decoder[0x36];

        /* SEG DS: */
        decoder[0x3e] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                cpu.base_ds = cpu.ds;
                cpu.base_ss = cpu.ds;
                boolean result = decoder[cpu.fetchb()+cpu.op_index].call(cpu, prev);
                cpu.base_ds = cpu.ds;
                cpu.base_ss = cpu.ss;
                return result;
            }
        };
        decoder[0x23e] = decoder[0x3e];

        /* SEG FS: */
        decoder[0x64] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                cpu.base_ds = cpu.fs;
                cpu.base_ss = cpu.fs;
                boolean result = decoder[cpu.fetchb()+cpu.op_index].call(cpu, prev);
                cpu.base_ds = cpu.ds;
                cpu.base_ss = cpu.ss;
                return result;
            }
        };
        decoder[0x264] = decoder[0x64];

        /* SEG GS: */
        decoder[0x65] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                cpu.base_ds = cpu.gs;
                cpu.base_ss = cpu.gs;
                boolean result = decoder[cpu.fetchb()+cpu.op_index].call(cpu, prev);
                cpu.base_ds = cpu.ds;
                cpu.base_ss = cpu.ss;
                return result;
            }
        };
        decoder[0x265] = decoder[0x65];

        addInstructionForAllRegs16(0x40, Instructions.incw);
        addInstructionForAllRegs32(0x240, Instructions.incd);
        addInstructionForAllRegs16(0x48, Instructions.decw);
        addInstructionForAllRegs32(0x248, Instructions.decd);
        for (int i=0;i<8;i++) {
            final int rm = i;
            decoder[0x50+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.PushReg16(ew(cpu, rm));
                    return true;
                }
            };
            decoder[0x58+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.PopReg16(ew(cpu, rm));
                    return true;
                }
            };
            decoder[0x250+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.PushReg32(ed(cpu, rm));
                    return true;
                }
            };
            decoder[0x258+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.PopReg32(ed(cpu, rm));
                    return true;
                }
            };
        }

        /* PUSHA */
        decoder[0x60] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PushA16();
                return true;
            }
        };

        /* PUSHAD */
        decoder[0x260] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PushA32();
                return true;
            }
        };

        /* POPA */
        decoder[0x61] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PopA16();
                return true;
            }
        };

        /* POPAD */
        decoder[0x261] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PopA32();
                return true;
            }
        };

        /* Operand Size Prefix */
        decoder[0x66] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                return decoder[cpu.fetchb()+0x200].call(cpu, prev);
            }
        };
        decoder[0x266] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                return decoder[cpu.fetchb()].call(cpu, prev);
            }
        };

        /* Address Size Prefix */
        decoder[0x67] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int p = cpu.prefixes;
                cpu.prefixes=(cpu.prefixes & ~CPU.PREFIX_ADDR) |(cpu.big?0:CPU.PREFIX_ADDR);
                boolean result = decoder[cpu.op_index + cpu.fetchb()].call(cpu, prev);
                cpu.prefixes = p;
                return result;
            }
        };
        decoder[0x267] = decoder[0x67];

        /* PUSH Iw */
        decoder[0x68] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PushValue16(cpu.fetchw());
                return true;
            }
        };

        /* PUSH Id */
        decoder[0x268] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PushValue32(cpu.fetchd());
                return true;
            }
        };

        /* IMUL Gw,Ew,Iw */
        decoder[0x69] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GwEwS16(cpu, prev, Instructions.imulw2);
                return true;
            }
        };

        /* IMUL Gd,Ed,Id */
        decoder[0x269] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GdEdId(cpu, prev, Instructions.imuld2);
                return true;
            }
        };

        /* PUSH Ib */
        decoder[0x6a] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PushValue16(cpu.fetchbs() & 0xFFFF);
                return true;
            }
        };

        /* PUSH Ib */
        decoder[0x26a] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PushValue32(cpu.fetchbs());
                return true;
            }
        };

        /* IMUL Gw,Ew,Ib */
        decoder[0x6b] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GwEwS8(cpu, prev, Instructions.imulw2);
                return true;
            }
        };

        /* IMUL Gd,Ed,Ib */
        decoder[0x26b] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GdEdS8(cpu, prev, Instructions.imuld2);
                return true;
            }
        };

        for (int i=0;i<16;i++) {
            final int index = i;
            decoder[0x70+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Jump(Conditions.get(index), cpu.fetchbs());
                    return false;
                }
            };
        }

        for (int i=0;i<16;i++) {
            final int index = i;
            decoder[0x270+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Jump(Conditions.get(index), cpu.fetchbs());
                    return false;
                }
            };
        }

        /* Grpl Eb,Ib */
        decoder[0x80] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:EbIb(cpu, prev, Instructions.addb, rm);break;
                    case 0x01:EbIb(cpu, prev, Instructions.orb, rm);break;
                    case 0x02:EbIb(cpu, prev, Instructions.adcb, rm);break;
                    case 0x03:EbIb(cpu, prev, Instructions.sbbb, rm);break;
                    case 0x04:EbIb(cpu, prev, Instructions.andb, rm);break;
                    case 0x05:EbIb(cpu, prev, Instructions.subb, rm);break;
                    case 0x06:EbIb(cpu, prev, Instructions.xorb, rm);break;
                    case 0x07:EbIb_noresult(cpu, prev, Instructions.subb, rm);break;
                }
                return true;
            }
        };
        decoder[0x280] = decoder[0x80];

        /* Grpl Eb,Ib Mirror instruction*/
        decoder[0x82] = decoder[0x80];
        decoder[0x282] = decoder[0x80];

        /* Grpl Ew,Iw */
        decoder[0x81] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:EwIw(cpu, prev, Instructions.addw, rm);break;
                    case 0x01:EwIw(cpu, prev, Instructions.orw, rm);break;
                    case 0x02:EwIw(cpu, prev, Instructions.adcw, rm);break;
                    case 0x03:EwIw(cpu, prev, Instructions.sbbw, rm);break;
                    case 0x04:EwIw(cpu, prev, Instructions.andw, rm);break;
                    case 0x05:EwIw(cpu, prev, Instructions.subw, rm);break;
                    case 0x06:EwIw(cpu, prev, Instructions.xorw, rm);break;
                    case 0x07:EwIw_noresult(cpu, prev, Instructions.subw, rm);break;
                }
                return true;
            }
        };

        /* Grpl Ed,Id */
        decoder[0x281] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:EdId(cpu, prev, Instructions.addd, rm);break;
                    case 0x01:EdId(cpu, prev, Instructions.ord, rm);break;
                    case 0x02:EdId(cpu, prev, Instructions.adcd, rm);break;
                    case 0x03:EdId(cpu, prev, Instructions.sbbd, rm);break;
                    case 0x04:EdId(cpu, prev, Instructions.andd, rm);break;
                    case 0x05:EdId(cpu, prev, Instructions.subd, rm);break;
                    case 0x06:EdId(cpu, prev, Instructions.xord, rm);break;
                    case 0x07:EdId_noresult(cpu, prev, Instructions.subd, rm);break;
                }
                return true;
            }
        };

        /* Grpl Ew,Ix */
        decoder[0x83] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:EwIx(cpu, prev, Instructions.addw, rm);break;
                    case 0x01:EwIx(cpu, prev, Instructions.orw, rm);break;
                    case 0x02:EwIx(cpu, prev, Instructions.adcw, rm);break;
                    case 0x03:EwIx(cpu, prev, Instructions.sbbw, rm);break;
                    case 0x04:EwIx(cpu, prev, Instructions.andw, rm);break;
                    case 0x05:EwIx(cpu, prev, Instructions.subw, rm);break;
                    case 0x06:EwIx(cpu, prev, Instructions.xorw, rm);break;
                    case 0x07:EwIx_noresult(cpu, prev, Instructions.subw, rm);break;
                }
                return true;
            }
        };

        /* Grpl Ed,Ix */
        decoder[0x283] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:EdIx(cpu, prev, Instructions.addd, rm);break;
                    case 0x01:EdIx(cpu, prev, Instructions.ord, rm);break;
                    case 0x02:EdIx(cpu, prev, Instructions.adcd, rm);break;
                    case 0x03:EdIx(cpu, prev, Instructions.sbbd, rm);break;
                    case 0x04:EdIx(cpu, prev, Instructions.andd, rm);break;
                    case 0x05:EdIx(cpu, prev, Instructions.subd, rm);break;
                    case 0x06:EdIx(cpu, prev, Instructions.xord, rm);break;
                    case 0x07:EdIx_noresult(cpu, prev, Instructions.subd, rm);break;
                }
                return true;
            }
        };

        /* TEST Eb,Gb */
        decoder[0x84] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EbGb_noresult(cpu, prev, Instructions.andb);
                return true;
            }
        };
        decoder[0x284] = decoder[0x84];

        /* TEST Ew,Gw */
        decoder[0x85] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EwGw_noresult(cpu, prev, Instructions.andw);
                return true;
            }
        };

        /* TEST Ed,Gd */
        decoder[0x285] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EdGd_noresult(cpu, prev, Instructions.andd);
                return true;
            }
        };

        /* XCHG Eb,Gb */
        decoder[0x86] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Exchange_Reg8_Reg8(eb(cpu,rm), gb(cpu,rm));
                } else {
                    prev.next = new Ops.Exchange_Mem8_Reg8(getEaa(cpu, rm), gb(cpu,rm));
                }
                return true;
            }
        };
        decoder[0x286] = decoder[0x86];

        /* XCHG Ew,Gw */
        decoder[0x87] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Exchange_Reg16_Reg16(ew(cpu,rm), gw(cpu,rm));
                } else {
                    prev.next = new Ops.Exchange_Mem16_Reg16(getEaa(cpu, rm), gw(cpu,rm));
                }
                return true;
            }
        };

        /* XCHG Ed,Gd */
        decoder[0x287] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Exchange_Reg32_Reg32(ed(cpu,rm), gd(cpu,rm));
                } else {
                    prev.next = new Ops.Exchange_Mem32_Reg32(getEaa(cpu, rm), gd(cpu,rm));
                }
                return true;
            }
        };

        /* MOV Eb,Gb */
        decoder[0x88] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg8_Reg8(eb(cpu, rm), gb(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Mem8_Reg8(getEaa(cpu, rm), gb(cpu, rm));
                }
                return true;
            }
        };
        decoder[0x288] = decoder[0x88];

        /* MOV Ew,Gw */
        decoder[0x89] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg16_Reg16(ew(cpu, rm), gw(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Mem16_Reg16(getEaa(cpu, rm), gw(cpu, rm));
                }
                return true;
            }
        };

        /* MOV Ed,Gd */
        decoder[0x289] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg32_Reg32(ed(cpu, rm), gd(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Mem32_Reg32(getEaa(cpu, rm), gd(cpu, rm));
                }
                return true;
            }
        };

        /* MOV Gb,Eb */
        decoder[0x8a] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg8_Reg8(gb(cpu, rm), eb(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg8_Mem8(gb(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };
        decoder[0x28a] = decoder[0x8a];

        /* MOV Gw,Ew */
        decoder[0x8b] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg16_Reg16(gw(cpu, rm), ew(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg16_Mem16(gw(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* Mov Ew,Sw */
        decoder[0x8c] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                Reg reg;
                switch ((rm>>3) & 7) {
                    case 0: reg = cpu.esValue; break;
                    case 1: reg = cpu.csValue; break;
                    case 2: reg = cpu.ssValue; break;
                    case 3: reg = cpu.dsValue; break;
                    case 4: reg = cpu.fsValue; break;
                    case 5: reg = cpu.gsValue; break;
                    default:
                        reg=null;
                        Log.panic("Invalid Mov Ew, Sw");
                }
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg16_Reg16(reg, ew(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg16_Mem16(reg, getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* Mov Ew,Sw */
        decoder[0x28c] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int sel = (rm>>3) & 7;
                if (sel>5) {
                    Log.panic("Invalid Mov Ew, Sw");
                }
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg32_Reg32(cpu.segValue[sel], ew(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg16_Mem16(cpu.segValue[sel], getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* MOV Gd,Ed */
        decoder[0x28b] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg32_Reg32(gd(cpu, rm), ed(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg32_Mem32(gd(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* LEA Gw */
        decoder[0x8d] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                prev.next = new Ops.LeaGw(getEaa(cpu, rm), gw(cpu, rm));
                return true;
            }
        };

        /* LEA Gd */
        decoder[0x28d] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                prev.next = new Ops.LeaGd(getEaa(cpu, rm), gd(cpu, rm));
                return true;
            }
        };

        /* MOV Sw,Ew */
        decoder[0x08e] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int sel = (rm>>3) & 7;
                if (sel>5) {
                    Log.panic("Invalid Mov Sw, Ew");
                }
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg16_Seg(cpu.segPhys[sel], cpu.segValue[sel], ew(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Mem16_Seg(cpu.segPhys[sel], cpu.segValue[sel], getEaa(cpu, rm));
                }
                return true;
            }
        };
        decoder[0x28e] = decoder[0x08e];

        /* POP Ew */
        decoder[0x8f] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.PopReg16(ew(cpu, rm));
                } else {
                    prev.next = new Ops.PopMem16(getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* POP Ed */
        decoder[0x28f] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.PopReg32(ed(cpu, rm));
                } else {
                    prev.next = new Ops.PopMem32(getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* NOP */
        decoder[0x90] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Nop();
                return true;
            }
        };
        decoder[0x290] = decoder[0x90];

        for (int i=1;i<8;i++) {
            final int index = i;
            decoder[0x90+i] = new Decode() {
                final public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Exchange_Reg16_Reg16(cpu.eax, ew(cpu, index));
                    return true;
                }
            };
            decoder[0x290+i] = new Decode() {
                final public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Exchange_Reg32_Reg32(cpu.eax, ed(cpu, index));
                    return true;
                }
            };
        }


        /* CBW */
        decoder[0x98] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Cbw();
                return true;
            }
        };

        /* CWDE */
        decoder[0x298] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Cwde();
                return true;
            }
        };

        /* CWD */
        decoder[0x99] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Cwd();
                return true;
            }
        };

        /* CDQ */
        decoder[0x299] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Cdq();
                return true;
            }
        };

        /* WAIT */
        decoder[0x09b] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                return true;
            }
        };
        decoder[0x29b] = decoder[0x09b];

        /* PUSHF */
        decoder[0x09c] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PushF16();
                return true;
            }
        };
        decoder[0x29c] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PushF32();
                return true;
            }
        };

        /* POPF */
        decoder[0x09d] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PopF16();
                return true;
            }
        };
        decoder[0x29d] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.PopF32();
                return true;
            }
        };

        /* SAHF */
        decoder[0x09e] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Sahf();
                return true;
            }
        };
        decoder[0x29e] = decoder[0x09e];

        /* LAHF */
        decoder[0x09f] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Lahf();
                return true;
            }
        };
        decoder[0x29f] = decoder[0x09f];

        /* MOV AL,Ob */
        decoder[0x0a0] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Mov_Reg8_0b(cpu.eax, cpu.base_ds, (cpu.ea16()?cpu.fetchw():cpu.fetchd()));
                return true;
            }
        };
        decoder[0x2a0] = decoder[0x0a0];

        /* MOV AX,Ow */
        decoder[0x0a1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Mov_Reg16_0w(cpu.eax, cpu.base_ds, (cpu.ea16()?cpu.fetchw():cpu.fetchd()));
                return true;
            }
        };

        /* MOV EAX,Od */
        decoder[0x2a1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Mov_Reg32_0d(cpu.eax, cpu.base_ds, (cpu.ea16()?cpu.fetchw():cpu.fetchd()));
                return true;
            }
        };

        /* MOV Ob,AL */
        decoder[0x0a2] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Mov_0b_Reg8(cpu.base_ds, (cpu.ea16()?cpu.fetchw():cpu.fetchd()), cpu.eax);
                return true;
            }
        };
        decoder[0x2a2] = decoder[0x0a2];

        /* MOV Ow,AX */
        decoder[0x0a3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Mov_0w_Reg16(cpu.base_ds, (cpu.ea16()?cpu.fetchw():cpu.fetchd()), cpu.eax);
                return true;
            }
        };

        /* MOV Od,EAX */
        decoder[0x2a3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Mov_0d_Reg32(cpu.base_ds, (cpu.ea16()?cpu.fetchw():cpu.fetchd()), cpu.eax);
                return true;
            }
        };

        /* MOVSB */
        decoder[0x0a4] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Movsb32_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Movsb32(cpu.DF()?-1:1, cpu.base_ds);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Movsb16_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Movsb16(cpu.DF()?-1:1, cpu.base_ds);
                    }
                }
                return true;
            }
        };
        decoder[0x2a4] = decoder[0x0a4];
        
        /* MOVSW */
        decoder[0x0a5] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Movsw32_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Movsw32(cpu.DF()?-1:1, cpu.base_ds);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Movsw16_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Movsw16(cpu.DF()?-1:1, cpu.base_ds);
                    }
                }
                return true;
            }
        };
        
        /* MOVSD */
        decoder[0x2a5] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Movsd32_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Movsd32(cpu.DF()?-1:1, cpu.base_ds);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Movsd16_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Movsd16(cpu.DF()?-1:1, cpu.base_ds);
                    }
                }
                return true;
            }            
        };
        
        /* CMPSB */
        decoder[0x0a6] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Cmpsb32_r(cpu.DF()?-1:1, cpu.base_ds, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Cmpsb32(cpu.DF()?-1:1, cpu.base_ds);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Cmpsb16_r(cpu.DF()?-1:1, cpu.base_ds, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Cmpsb16(cpu.DF()?-1:1, cpu.base_ds);
                    }
                }
                return true;
            }
        };
        decoder[0x2a6] = decoder[0x0a6];
        
        /* CMPSW */
        decoder[0x0a7] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Cmpsw32_r(cpu.DF()?-1:1, cpu.base_ds, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Cmpsw32(cpu.DF()?-1:1, cpu.base_ds);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Cmpsw16_r(cpu.DF()?-1:1, cpu.base_ds, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Cmpsw16(cpu.DF()?-1:1, cpu.base_ds);
                    }
                }
                return true;
            }
        };
        
        /* CMPSD */
        decoder[0x2a7] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Cmpsd32_r(cpu.DF()?-1:1, cpu.base_ds, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Cmpsd32(cpu.DF()?-1:1, cpu.base_ds);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Cmpsd16_r(cpu.DF()?-1:1, cpu.base_ds, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Cmpsd16(cpu.DF()?-1:1, cpu.base_ds);
                    }
                }
                return true;
            }
        };   
        
        /* TEST AL,Ib */
        decoder[0x0a8] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EbIb_noresult(cpu, prev, Instructions.andb, 0xC0);
                return true;
            }
        };
        decoder[0x2a8] = decoder[0x0a8];
        
        /* TEST AX,Iw */
        decoder[0x0a9] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EwIw_noresult(cpu, prev, Instructions.andw, 0xC0);
                return true;
            }
        };
        
        /* TEST EAX,Id */
        decoder[0x2a9] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                EdId_noresult(cpu, prev, Instructions.andd, 0xC0);
                return true;
            }
        };
        
        /* STOSB */
        decoder[0x0aa] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Stosb32_r(cpu.DF()?-1:1);
                    } else {
                        prev.next = new Ops.Stosb32(cpu.DF()?-1:1);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Stosb16_r(cpu.DF()?-1:1);
                    } else {
                        prev.next = new Ops.Stosb16(cpu.DF()?-1:1);
                    }
                }
                return true;
            }
        };
        decoder[0x2aa] = decoder[0x0aa];
        
        /* STOSW */
        decoder[0x0ab] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Stosw32_r(cpu.DF()?-1:1);
                    } else {
                        prev.next = new Ops.Stosw32(cpu.DF()?-1:1);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Stosw16_r(cpu.DF()?-1:1);
                    } else {
                        prev.next = new Ops.Stosw16(cpu.DF()?-1:1);
                    }
                }
                return true;
            }
        };
        
        /* STOSD */
        decoder[0x2ab] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Stosd32_r(cpu.DF()?-1:1);
                    } else {
                        prev.next = new Ops.Stosd32(cpu.DF()?-1:1);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Stosd16_r(cpu.DF()?-1:1);
                    } else {
                        prev.next = new Ops.Stosd16(cpu.DF()?-1:1);
                    }
                }
                return true;
            }
        };
        
        /* LODSB */
        decoder[0x0ac] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Lodsb32_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Lodsb32(cpu.DF()?-1:1, cpu.base_ds);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Lodsb16_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Lodsb16(cpu.DF()?-1:1, cpu.base_ds);
                    }
                }
                return true;
            }
        };
        decoder[0x2ac] = decoder[0x0ac];
        
        /* LODSW */
        decoder[0x0ad] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Lodsw32_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Lodsw32(cpu.DF()?-1:1, cpu.base_ds);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Lodsw16_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Lodsw16(cpu.DF()?-1:1, cpu.base_ds);
                    }
                }
                return true;
            }
        };
        
        /* LODSD */
        decoder[0x2ad] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Lodsd32_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Lodsd32(cpu.DF()?-1:1, cpu.base_ds);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Lodsd16_r(cpu.DF()?-1:1, cpu.base_ds);
                    } else {
                        prev.next = new Ops.Lodsd16(cpu.DF()?-1:1, cpu.base_ds);
                    }
                }
                return true;
            }
        };
        
        /* SCASB */
        decoder[0x0ae] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Scasb32_r(cpu.DF()?-1:1, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Scasb32(cpu.DF()?-1:1);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Scasb16_r(cpu.DF()?-1:1, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Scasb16(cpu.DF()?-1:1);
                    }
                }
                return true;
            }
        };
        decoder[0x2ae] = decoder[0x0ae];
        
        /* SCASW */
        decoder[0x0af] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Scasw32_r(cpu.DF()?-1:1, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Scasw32(cpu.DF()?-1:1);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Scasw16_r(cpu.DF()?-1:1, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Scasw16(cpu.DF()?-1:1);
                    }
                }
                return true;
            }
        };
        
        /* SCASD */
        decoder[0x2af] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Scasd32_r(cpu.DF()?-1:1, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Scasd32(cpu.DF()?-1:1);
                    }
                } else {
                    if ((cpu.prefixes & CPU.PREFIX_REP)!=0) {
                        prev.next = new Ops.Scasd16_r(cpu.DF()?-1:1, cpu.repeat_zero);
                    } else {
                        prev.next = new Ops.Scasd16(cpu.DF()?-1:1);
                    }
                }
                return true;
            }
        };

        for (int i=0;i<8;i++) {
            final int rm=i+0xC0;
            decoder[0x0b0+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Mov_Reg8(eb(cpu, rm), cpu.fetchb());
                    return true;
                }
            };
            decoder[0x2b0+i] = decoder[0x0b0+i];

            decoder[0x0b8+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Mov_Reg16(ew(cpu, rm), cpu.fetchw());
                    return true;
                }
            };

            decoder[0x2b8+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Mov_Reg32(ed(cpu, rm), cpu.fetchd());
                    return true;
                }
            };
        }

        /* GRP2 Eb,Ib */
        decoder[0x0c0] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                int value;
                switch (which) {
                    case 0x00:
                        value = EbIbMaskOriginal(cpu, prev, Instructions.rolb, rm, 0x7);
                        if ((value & 0x7)==0) {
                            prev.next = null; // don't set flags
                            if ((value & 0x18)!=0) {
                                EbIb(cpu, prev, Instructions.rolb_check, rm, value);
                            }
                        }
                        break;
                    case 0x01:
                        value = EbIbMaskOriginal(cpu, prev, Instructions.rorb, rm, 0x7);
                        if ((value & 0x7)==0) {
                            prev.next = null; // don't set flags
                            if ((value & 0x18)!=0) {
                                EbIb(cpu, prev, Instructions.rorb_check, rm, value);
                            }
                        }
                        break;
                    case 0x02:
                        value = EbIbMod(cpu, prev, Instructions.rclb, rm, 9);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x03:
                        value = EbIbMod(cpu, prev, Instructions.rcrb, rm, 9);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x04:
                        value = EbIbMask(cpu, prev, Instructions.shlb, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x05:
                        value = EbIbMask(cpu, prev, Instructions.shrb, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x06: //salb is the same as shlb
                        value = EbIbMask(cpu, prev, Instructions.shlb, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x07:
                        value = EbIbMask(cpu, prev, Instructions.sarb, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                }
                return true;
            }
        };
        decoder[0x2c0] = decoder[0x0c0];

        /* GRP2 Ew,Ib */
        decoder[0x0c1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                int value;
                switch (which) {
                    case 0x00:
                        value = EwIbMaskOriginal(cpu, prev, Instructions.rolw, rm, 0xf);
                        if ((value & 0xf)==0) {
                            prev.next = null; // don't set flags
                            if ((value & 0x10)!=0) {
                                EwIw(cpu, prev, Instructions.rolw_check, rm, value);
                            }
                        }
                        break;
                    case 0x01:
                        value = EwIbMaskOriginal(cpu, prev, Instructions.rorw, rm, 0xf);
                        if ((value & 0xf)==0) {
                            prev.next = null; // don't set flags
                            if ((value & 0x10)!=0) {
                                EwIw(cpu, prev, Instructions.rorw_check, rm, value);
                            }
                        }
                        break;
                    case 0x02:
                        value = EwIbMod(cpu, prev, Instructions.rclw, rm, 17);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x03:
                        value = EwIbMod(cpu, prev, Instructions.rcrw, rm, 17);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x04:
                        value = EwIbMask(cpu, prev, Instructions.shlw, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x05:
                        value = EwIbMask(cpu, prev, Instructions.shrw, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x06: //salw is the same as shlw
                        value = EwIbMask(cpu, prev, Instructions.shlw, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x07:
                        value = EwIbMask(cpu, prev, Instructions.sarw, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                }
                return true;
            }
        };

        /* GRP2 Ed,Ib */
        decoder[0x2c1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                int value;
                switch (which) {
                    case 0x00:
                        value = EdIbMask(cpu, prev, Instructions.rold, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x01:
                        value = EdIbMask(cpu, prev, Instructions.rord, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x02:
                        value = EdIbMask(cpu, prev, Instructions.rcld, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x03:
                        value = EdIbMask(cpu, prev, Instructions.rcrd, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x04:
                        value = EdIbMask(cpu, prev, Instructions.shld, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x05:
                        value = EdIbMask(cpu, prev, Instructions.shrd, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x06: //salw is the same as shlw
                        value = EdIbMask(cpu, prev, Instructions.shld, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                    case 0x07:
                        value = EdIbMask(cpu, prev, Instructions.sard, rm, 0x1f);
                        if (value==0) {
                            prev.next = null; // don't set flags
                        }
                        break;
                }
                return true;
            }
        };

        /* RETN Iw */
        decoder[0xc2] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.RetIw16(cpu.fetchw());
                return false;
            }
        };
        decoder[0x2c2] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.RetIw32(cpu.fetchw());
                return false;
            }
        };

        /* RETN */
        decoder[0xc3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Ret16();
                return false;
            }
        };
        decoder[0x2c3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Ret32();
                return false;
            }
        };

        /* MOV Eb,Ib */
        decoder[0xc6] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg8(eb(cpu, rm), cpu.fetchb());
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    prev.next = new Ops.Mov_Mem8(eaa, cpu.fetchb());
                }
                return true;
            }
        };
        decoder[0x2c6] = decoder[0xc6];

        /* MOV EW,Iw */
        decoder[0xc7] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg16(ew(cpu, rm), cpu.fetchw());
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    prev.next = new Ops.Mov_Mem16(eaa, cpu.fetchw());
                }
                return true;
            }
        };

        /* MOV Ed,Id */
        decoder[0x2c7] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg32(ed(cpu, rm), cpu.fetchd());
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    prev.next = new Ops.Mov_Mem32(eaa, cpu.fetchd());
                }
                return true;
            }
        };

        /* LEAVE */
        decoder[0x0c9] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Leave16();
                return true;
            }
        };

        /* LEAVE */
        decoder[0x2c9] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Leave32();
                return true;
            }
        };

        /* INT Ib */
        decoder[0x0cd] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int n = cpu.fetchb();
                if (n==0x80) {
                    prev.next = new Ops.Syscall();
                } else {
                    Log.panic("Unhandled interrupt "+Integer.toHexString(n));
                }
                return true;
            }
        };
        decoder[0x2cd] = decoder[0x0cd];

        /* GRP2 Eb,1 */
        decoder[0xd0] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:
                        EbIb(cpu, prev, Instructions.rolb, rm, 1);
                        break;
                    case 0x01:
                        EbIb(cpu, prev, Instructions.rorb, rm, 1);
                        break;
                    case 0x02:
                        EbIb(cpu, prev, Instructions.rclb, rm, 1);
                        break;
                    case 0x03:
                        EbIb(cpu, prev, Instructions.rcrb, rm, 1);
                        break;
                    case 0x04:
                        EbIb(cpu, prev, Instructions.shlb, rm, 1);
                        break;
                    case 0x05:
                        EbIb(cpu, prev, Instructions.shrb, rm, 1);
                        break;
                    case 0x06: //salb is the same as shlb
                        EbIb(cpu, prev, Instructions.shlb, rm, 1);
                        break;
                    case 0x07:
                        EbIb(cpu, prev, Instructions.sarb, rm, 1);
                        break;
                }
                return true;
            }
        };
        decoder[0x2d0] = decoder[0xd0];

        /* GRP2 Ew,1 */
        decoder[0xd1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:
                        EwIw(cpu, prev, Instructions.rolw, rm, 1);
                        break;
                    case 0x01:
                        EwIw(cpu, prev, Instructions.rorw, rm, 1);
                        break;
                    case 0x02:
                        EwIw(cpu, prev, Instructions.rclw, rm, 1);
                        break;
                    case 0x03:
                        EwIw(cpu, prev, Instructions.rcrw, rm, 1);
                        break;
                    case 0x04:
                        EwIw(cpu, prev, Instructions.shlw, rm, 1);
                        break;
                    case 0x05:
                        EwIw(cpu, prev, Instructions.shrw, rm, 1);
                        break;
                    case 0x06: //salw is the same as shlw
                        EwIw(cpu, prev, Instructions.shlw, rm, 1);
                        break;
                    case 0x07:
                        EwIw(cpu, prev, Instructions.sarw, rm, 1);
                        break;
                }
                return true;
            }
        };

        /* GRP2 Ed,1 */
        decoder[0x2d1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:
                        EdId(cpu, prev, Instructions.rold, rm, 1);
                        break;
                    case 0x01:
                        EdId(cpu, prev, Instructions.rord, rm, 1);
                        break;
                    case 0x02:
                        EdId(cpu, prev, Instructions.rcld, rm, 1);
                        break;
                    case 0x03:
                        EdId(cpu, prev, Instructions.rcrd, rm, 1);
                        break;
                    case 0x04:
                        EdId(cpu, prev, Instructions.shld, rm, 1);
                        break;
                    case 0x05:
                        EdId(cpu, prev, Instructions.shrd, rm, 1);
                        break;
                    case 0x06: //sald is the same as shld
                        EdId(cpu, prev, Instructions.shld, rm, 1);
                        break;
                    case 0x07:
                        EdId(cpu, prev, Instructions.sard, rm, 1);
                        break;
                }
                return true;
            }
        };

        /* GRP2 Eb,CL */
        decoder[0xd2] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                rm = (rm & 7) | (1 << 3) | (rm & 0xC0);
                switch (which) {
                    case 0x00:
                        EbGb(cpu, prev, Instructions.rolb_check, rm);
                        break;
                    case 0x01:
                        EbGb(cpu, prev, Instructions.rorb_check, rm);
                        break;
                    case 0x02:
                        EbGb(cpu, prev, Instructions.rclb_check, rm);
                        break;
                    case 0x03:
                        EbGb(cpu, prev, Instructions.rcrb_check, rm);
                        break;
                    case 0x04:
                        EbGb(cpu, prev, Instructions.shlb_check, rm);
                        break;
                    case 0x05:
                        EbGb(cpu, prev, Instructions.shrb_check, rm);
                        break;
                    case 0x06: //salb is the same as shlb
                        EbGb(cpu, prev, Instructions.shlb_check, rm);
                        break;
                    case 0x07:
                        EbGb(cpu, prev, Instructions.sarb_check, rm);
                        break;
                }
                return true;
            }
        };
        decoder[0x2d2] = decoder[0xd2];

        /* GRP2 Ew,CL */
        decoder[0xd3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                rm = (rm & 7) | (1 << 3) | (rm & 0xC0);
                switch (which) {
                    case 0x00:
                        EwGb(cpu, prev, Instructions.rolw_check, rm);
                        break;
                    case 0x01:
                        EwGb(cpu, prev, Instructions.rorw_check, rm);
                        break;
                    case 0x02:
                        EwGb(cpu, prev, Instructions.rclw_check, rm);
                        break;
                    case 0x03:
                        EwGb(cpu, prev, Instructions.rcrw_check, rm);
                        break;
                    case 0x04:
                        EwGb(cpu, prev, Instructions.shlw_check, rm);
                        break;
                    case 0x05:
                        EwGb(cpu, prev, Instructions.shrw_check, rm);
                        break;
                    case 0x06: //salw is the same as shlw
                        EwGb(cpu, prev, Instructions.shlw_check, rm);
                        break;
                    case 0x07:
                        EwGb(cpu, prev, Instructions.sarw_check, rm);
                        break;
                }
                return true;
            }
        };

        /* GRP2 Ed,CL */
        decoder[0x2d3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                rm = (rm & 7) | (1 << 3) | (rm & 0xC0);
                switch (which) {
                    case 0x00:
                        EdGb(cpu, prev, Instructions.rold_check, rm);
                        break;
                    case 0x01:
                        EdGb(cpu, prev, Instructions.rord_check, rm);
                        break;
                    case 0x02:
                        EdGb(cpu, prev, Instructions.rcld_check, rm);
                        break;
                    case 0x03:
                        EdGb(cpu, prev, Instructions.rcrd_check, rm);
                        break;
                    case 0x04:
                        EdGb(cpu, prev, Instructions.shld_check, rm);
                        break;
                    case 0x05:
                        EdGb(cpu, prev, Instructions.shrd_check, rm);
                        break;
                    case 0x06: //salw is the same as shlw
                        EdGb(cpu, prev, Instructions.shld_check, rm);
                        break;
                    case 0x07:
                        EdGb(cpu, prev, Instructions.sard_check, rm);
                        break;
                }
                return true;
            }
        };

        /* SALC */
        decoder[0x0d6] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Salc();
                return true;
            }
        };
        decoder[0x2d6] = decoder[0x0d6];

        /* XLAT */
        decoder[0x0d7] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    prev.next = new Ops.Xlat32(cpu.base_ds);
                } else {
                    prev.next = new Ops.Xlat16(cpu.base_ds);
                }
                return true;
            }
        };
        decoder[0x2d7] = decoder[0x0d7];
        
        /* FPU ESC 0 */
        decoder[0xd8] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm = cpu.fetchb();
                if (rm >= 0xc0) {
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FADD_ST0_STj(rm); break;
                        case 1: prev.next = new FPUOps.FMUL_ST0_STj(rm); break;
                        case 2: prev.next = new FPUOps.FCOM_STi(rm, false); break;
                        case 3: prev.next = new FPUOps.FCOM_STi(rm, true); break;
                        case 4: prev.next = new FPUOps.FSUB_ST0_STj(rm); break;
                        case 5: prev.next = new FPUOps.FSUBR_ST0_STj(rm); break;
                        case 6: prev.next = new FPUOps.FDIV_ST0_STj(rm); break;
                        case 7: prev.next = new FPUOps.FDIVR_ST0_STj(rm); break;
                    }
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FADD_SINGLE_REAL(eaa); break;
                        case 1: prev.next = new FPUOps.FMUL_SINGLE_REAL(eaa); break;
                        case 2: prev.next = new FPUOps.FCOM_SINGLE_REAL(eaa, false); break;
                        case 3: prev.next = new FPUOps.FCOM_SINGLE_REAL(eaa, true); break;
                        case 4: prev.next = new FPUOps.FSUB_SINGLE_REAL(eaa); break;
                        case 5: prev.next = new FPUOps.FSUBR_SINGLE_REAL(eaa); break;
                        case 6: prev.next = new FPUOps.FDIV_SINGLE_REAL(eaa); break;
                        case 7: prev.next = new FPUOps.FDIVR_SINGLE_REAL(eaa); break;
                    }
                }
                return true;
            }
        };
        decoder[0x2d8] = decoder[0xd8];
        
        /* FPU ESC 1 */
        decoder[0xd9] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm = cpu.fetchb();
                if (rm >= 0xc0) {
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FLD_STi(rm); break;
                        case 1: prev.next = new FPUOps.FXCH_STi(rm); break;
                        case 2: prev.next = new FPUOps.FNOP(); break;
                        case 3: prev.next = new FPUOps.FST_STi(rm, true); break;
                        case 4:
                        {
                            switch (rm & 7) {
                                case 0: prev.next = new FPUOps.FCHS(); break;
                                case 1: prev.next = new FPUOps.FABS(); break;
                                case 4: prev.next = new FPUOps.FTST(); break;
                                case 5: prev.next = new FPUOps.FXAM(); break;
                                default:
                                    prev.next = null;
                                    Log.warn("ESC 1:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                                    break;
                            }
                            break;
                        }
                        case 5:
                        {
                            switch (rm & 7) {
                                case 0: prev.next = new FPUOps.FLD1(); break;
                                case 1: prev.next = new FPUOps.FLDL2T(); break;
                                case 2: prev.next = new FPUOps.FLDL2E(); break;
                                case 3: prev.next = new FPUOps.FLDPI(); break;
                                case 4: prev.next = new FPUOps.FLDLG2(); break;
                                case 5: prev.next = new FPUOps.FLDLN2(); break;
                                case 6: prev.next = new FPUOps.FLDZ(); break;
                                case 7:
                                    prev.next = null;
                                    Log.warn("ESC 1:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                                    break;
                            }
                            break;
                        }
                        case 6:
                        {
                            switch (rm & 7) {
                                case 0: prev.next = new FPUOps.F2XM1(); break;
                                case 1: prev.next = new FPUOps.FYL2X(); break;
                                case 2: prev.next = new FPUOps.FPTAN(); break;
                                case 3: prev.next = new FPUOps.FPATAN(); break;
                                case 4: prev.next = new FPUOps.FXTRACT(); break;
                                case 5: prev.next = new FPUOps.FPREM(true); break;
                                case 6: prev.next = new FPUOps.FDECSTP(); break;
                                case 7: prev.next = new FPUOps.FINCSTP(); break;
                            }
                            break;
                        }
                        case 7:
                        {
                            switch (rm & 7) {
                                case 0: prev.next = new FPUOps.FPREM(false); break;
                                case 1: prev.next = new FPUOps.FYL2XP1(); break;
                                case 2: prev.next = new FPUOps.FSQRT(); break;
                                case 3: prev.next = new FPUOps.FSINCOS(); break;
                                case 4: prev.next = new FPUOps.FRNDINT(); break;
                                case 5: prev.next = new FPUOps.FSCALE(); break;
                                case 6: prev.next = new FPUOps.FSIN(); break;
                                case 7: prev.next = new FPUOps.FCOS(); break;
                            }
                            break;
                        }
                    }
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FLD_SINGLE_REAL(eaa); break;
                        case 1:
                            prev.next = null;
                            Log.warn("ESC 1 EA :Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                            break;
                        case 2: prev.next = new FPUOps.FST_SINGLE_REAL(eaa, false); break;
                        case 3: prev.next = new FPUOps.FST_SINGLE_REAL(eaa, true); break;
                        case 4: prev.next = new FPUOps.FLDENV(eaa); break;
                        case 5: prev.next = new FPUOps.FLDCW(eaa); break;
                        case 6: prev.next = new FPUOps.FNSTENV(eaa); break;
                        case 7: prev.next = new FPUOps.FNSTCW(eaa); break;
                    }
                }
                return true;
            }
        };
        decoder[0x2d9] = decoder[0xd9];

        /* FPU ESC 2 */
        decoder[0xda] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm = cpu.fetchb();
                if (rm >= 0xc0) {
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FCMOV_ST0_STj_CF(rm); break;
                        case 1: prev.next = new FPUOps.FCMOV_ST0_STj_ZF(rm); break;
                        case 2: prev.next = new FPUOps.FCMOV_ST0_STj_CF_OR_ZF(rm); break;
                        case 3: prev.next = new FPUOps.FCMOV_ST0_STj_PF(rm); break;
                        case 5:
                        {
                            if ((rm & 7)==1) {
                                prev.next = new FPUOps.FUCOMPP();
                            } else {
                                prev.next = null;
                                Log.warn("ESC 2:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                            }
                            break;
                        }
                        default:
                            prev.next = null;
                            Log.warn("ESC 2:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                            break;
                    }
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FIADD_DWORD_INTEGER(eaa); break;
                        case 1: prev.next = new FPUOps.FIMUL_DWORD_INTEGER(eaa); break;
                        case 2: prev.next = new FPUOps.FICOM_DWORD_INTEGER(eaa, false); break;
                        case 3: prev.next = new FPUOps.FICOM_DWORD_INTEGER(eaa, true); break;
                        case 4: prev.next = new FPUOps.FISUB_DWORD_INTEGER(eaa); break;
                        case 5: prev.next = new FPUOps.FISUBR_DWORD_INTEGER(eaa); break;
                        case 6: prev.next = new FPUOps.FIDIV_DWORD_INTEGER(eaa); break;
                        case 7: prev.next = new FPUOps.FIDIVR_DWORD_INTEGER(eaa); break;
                    }
                }
                return true;
            }
        };
        decoder[0x2da] = decoder[0xda];

        /* FPU ESC 3 */
        decoder[0xdb] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm = cpu.fetchb();
                if (rm >= 0xc0) {
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FCMOV_ST0_STj_NCF(rm); break;
                        case 1: prev.next = new FPUOps.FCMOV_ST0_STj_NZF(rm); break;
                        case 2: prev.next = new FPUOps.FCMOV_ST0_STj_NCF_AND_NZF(rm); break;
                        case 3: prev.next = new FPUOps.FCMOV_ST0_STj_NPF(rm); break;
                        case 4:
                        {
                            switch (rm & 7) {
                                case 2:prev.next = new FPUOps.FNCLEX(); break;
                                case 3:prev.next = new FPUOps.FNINIT(); break;
                                default:
                                    prev.next = null;
                                    Log.warn("ESC 3:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                                    break;

                            }
                            break;
                        }
                        case 5: prev.next = new FPUOps.FUCOMI_ST0_STj(rm, false); break;
                        case 6: prev.next = new FPUOps.FCOMI_ST0_STj(rm, false); break;
                        default:
                            prev.next = null;
                            Log.warn("ESC 3:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                            break;
                    }
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FILD_DWORD_INTEGER(eaa); break;
                        case 1: prev.next = new FPUOps.FISTTP32(eaa); break;
                        case 2: prev.next = new FPUOps.FIST_DWORD_INTEGER(eaa, false); break;
                        case 3: prev.next = new FPUOps.FIST_DWORD_INTEGER(eaa, true); break;
                        case 5: prev.next = new FPUOps.FLD_EXTENDED_REAL(eaa); break;
                        case 7: prev.next = new FPUOps.FSTP_EXTENDED_REAL(eaa); break;
                        default:
                            Log.warn("ESC 3 EA:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                    }
                }
                return true;
            }
        };
        decoder[0x2db] = decoder[0xdb];

        /* FPU ESC 4 */
        decoder[0xdc] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm = cpu.fetchb();
                if (rm >= 0xc0) {
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FADD_STi_ST0(rm, false); break;
                        case 1: prev.next = new FPUOps.FMUL_STi_ST0(rm, false); break;
                        case 2: prev.next = new FPUOps.FCOM_STi(rm, false); break;
                        case 3: prev.next = new FPUOps.FCOM_STi(rm, true); break;
                        case 4: prev.next = new FPUOps.FSUBR_STi_ST0(rm, false); break;
                        case 5: prev.next = new FPUOps.FSUB_STi_ST0(rm, false); break;
                        case 6: prev.next = new FPUOps.FDIVR_STi_ST0(rm, false); break;
                        case 7: prev.next = new FPUOps.FDIV_STi_ST0(rm, false); break;
                    }
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FADD_DOUBLE_REAL(eaa); break;
                        case 1: prev.next = new FPUOps.FMUL_DOUBLE_REAL(eaa); break;
                        case 2: prev.next = new FPUOps.FCOM_DOUBLE_REAL(eaa, false); break;
                        case 3: prev.next = new FPUOps.FCOM_DOUBLE_REAL(eaa, true); break;
                        case 4: prev.next = new FPUOps.FSUB_DOUBLE_REAL(eaa); break;
                        case 5: prev.next = new FPUOps.FSUBR_DOUBLE_REAL(eaa); break;
                        case 6: prev.next = new FPUOps.FDIV_DOUBLE_REAL(eaa); break;
                        case 7: prev.next = new FPUOps.FDIVR_DOUBLE_REAL(eaa); break;
                    }
                }
                return true;
            }
        };
        decoder[0x2dc] = decoder[0xdc];

        /* FPU ESC 5 */
        decoder[0xdd] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm = cpu.fetchb();
                if (rm >= 0xc0) {
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FFREE_STi(rm); break;
                        case 1: prev.next = new FPUOps.FXCH_STi(rm); break;
                        case 2: prev.next = new FPUOps.FST_STi(rm, false); break;
                        case 3: prev.next = new FPUOps.FST_STi(rm, true); break;
                        case 4: prev.next = new FPUOps.FUCOM_STi(rm, false); break;
                        case 5: prev.next = new FPUOps.FUCOM_STi(rm, true); break;
                        default:
                            prev.next = null;
                            Log.warn("ESC 5:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                            break;
                    }
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FLD_DOUBLE_REAL(eaa); break;
                        case 1: prev.next = new FPUOps.FISTTP64(eaa); break;
                        case 2: prev.next = new FPUOps.FST_DOUBLE_REAL(eaa, false); break;
                        case 3: prev.next = new FPUOps.FST_DOUBLE_REAL(eaa, true); break;
                        case 4: prev.next = new FPUOps.FRSTOR(eaa); break;
                        case 5:
                            prev.next = null;
                            Log.warn("ESC 5 EA:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                            break;
                        case 6: prev.next = new FPUOps.FNSAVE(eaa); break;
                        case 7: prev.next = new FPUOps.FNSTSW(eaa); break;
                    }
                }
                return true;
            }
        };
        decoder[0x2dd] = decoder[0xdd];

        /* FPU ESC 6 */
        decoder[0xde] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm = cpu.fetchb();
                if (rm >= 0xc0) {
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FADD_STi_ST0(rm, true); break;
                        case 1: prev.next = new FPUOps.FMUL_STi_ST0(rm, true); break;
                        case 2: prev.next = new FPUOps.FCOM_STi(rm, true); break;
                        case 3:
                        {
                            if ((rm & 7) == 1)
                                prev.next = new FPUOps.FCOMPP();
                            else {
                                prev.next = null;
                                Log.warn("ESC 6:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                            }
                        }
                        break;
                        case 4: prev.next = new FPUOps.FSUBR_STi_ST0(rm, true); break;
                        case 5: prev.next = new FPUOps.FSUB_STi_ST0(rm, true); break;
                        case 6: prev.next = new FPUOps.FDIVR_STi_ST0(rm, true); break;
                        case 7: prev.next = new FPUOps.FDIV_STi_ST0(rm, true); break;
                    }
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FIADD_WORD_INTEGER(eaa); break;
                        case 1: prev.next = new FPUOps.FIMUL_WORD_INTEGER(eaa); break;
                        case 2: prev.next = new FPUOps.FICOM_WORD_INTEGER(eaa, false); break;
                        case 3: prev.next = new FPUOps.FICOM_WORD_INTEGER(eaa, true); break;
                        case 4: prev.next = new FPUOps.FISUB_WORD_INTEGER(eaa); break;
                        case 5: prev.next = new FPUOps.FISUBR_WORD_INTEGER(eaa); break;
                        case 6: prev.next = new FPUOps.FIDIV_WORD_INTEGER(eaa); break;
                        case 7: prev.next = new FPUOps.FIDIVR_WORD_INTEGER(eaa); break;
                    }
                }
                return true;
            }
        };
        decoder[0x2de] = decoder[0xde];

        /* FPU ESC 7 */
        decoder[0xdf] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm = cpu.fetchb();
                if (rm >= 0xc0) {
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FFREEP_STi(rm); break;
                        case 1: prev.next = new FPUOps.FXCH_STi(rm); break;
                        case 2:
                        case 3: prev.next = new FPUOps.FST_STi(rm, true); break;
                        case 4:
                        {
                            if ((rm & 7)==0)
                                prev.next = new FPUOps.FNSTSW_AX();
                            else {
                                prev.next = null;
                                Log.warn("ESC 7:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                            }
                        }
                        break;
                        case 5: prev.next = new FPUOps.FUCOMI_ST0_STj(rm, true); break;
                        case 6: prev.next = new FPUOps.FCOMI_ST0_STj(rm, true); break;
                        case 7: prev.next = null;
                            Log.warn("ESC 7:Unhandled group " + ((rm >> 3) & 7) + " subfunction " + (rm & 7));
                            break;
                    }
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    switch ((rm >> 3) & 7) {
                        case 0: prev.next = new FPUOps.FILD_WORD_INTEGER(eaa); break;
                        case 1: prev.next = new FPUOps.FISTTP16(eaa); break;
                        case 2: prev.next = new FPUOps.FIST_WORD_INTEGER(eaa, false); break;
                        case 3: prev.next = new FPUOps.FIST_WORD_INTEGER(eaa, true); break;
                        case 4: prev.next = new FPUOps.FBLD_PACKED_BCD(eaa); break;
                        case 5: prev.next = new FPUOps.FILD_QWORD_INTEGER(eaa); break;
                        case 6: prev.next = new FPUOps.FBSTP_PACKED_BCD(eaa); break;
                        case 7: prev.next = new FPUOps.FISTP_QWORD_INTEGER(eaa); break;
                    }
                }
                return true;
            }
        };
        decoder[0x2df] = decoder[0xdf];

        /* LOOPNZ */
        decoder[0x0e0] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    prev.next = new Ops.Loopnz32(cpu.fetchbs());
                } else {
                    prev.next = new Ops.Loopnz16(cpu.fetchbs());
                }
                return false;
            }
        };
        decoder[0x2e0] = decoder[0x0e0];

        /* LOOPZ */
        decoder[0x0e1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    prev.next = new Ops.Loopz32(cpu.fetchbs());
                } else {
                    prev.next = new Ops.Loopz16(cpu.fetchbs());
                }
                return false;
            }
        };
        decoder[0x2e1] = decoder[0x0e1];

        /* LOOP */
        decoder[0x0e2] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    prev.next = new Ops.Loop32(cpu.fetchbs());
                } else {
                    prev.next = new Ops.Loop16(cpu.fetchbs());
                }
                return false;
            }
        };
        decoder[0x2e2] = decoder[0x0e2];

        /* JCXZ */
        decoder[0x0e3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                if ((cpu.prefixes & CPU.PREFIX_ADDR)!=0) {
                    prev.next = new Ops.Jecxz(cpu.fetchbs());
                } else {
                    prev.next = new Ops.Jcxz(cpu.fetchbs());
                }
                return false;
            }
        };
        decoder[0x2e3] = decoder[0x0e3];

        /* CALL Jw */
        decoder[0x0e8] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.CallJw16(cpu.fetchws());
                return false;
            }
        };

        /* CALL Jd */
        decoder[0x2e8] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.CallJd32(cpu.fetchd());
                return false;
            }
        };

        /* JMP Jw */
        decoder[0x0e9] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Jmp16(cpu.fetchws());
                return false;
            }
        };

        /* JMP Jd */
        decoder[0x2e9] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Jmp32(cpu.fetchd());
                return false;
            }
        };

        /* JMP Jb */
        decoder[0x0eb] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Jmp16(cpu.fetchbs());
                return false;
            }
        };
        decoder[0x2eb] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Jmp32(cpu.fetchbs());
                return false;
            }
        };

        /* LOCK */
        decoder[0x0f0] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                boolean result = decoder[cpu.op_index+cpu.fetchb()].call(cpu, prev);
                prev.next.lock = true;
                return result;
            }
        };
        decoder[0x2f0] = decoder[0x0f0];

        /* REPNZ */
        decoder[0x0f2] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                cpu.prefixes|=CPU.PREFIX_REP;
                cpu.repeat_zero = false;
                boolean result = decoder[cpu.op_index+cpu.fetchb()].call(cpu, prev);
                cpu.prefixes&=~CPU.PREFIX_REP;
                return result;
            }
        };
        decoder[0x2f2] = decoder[0x0f2];

        /* REPNZ */
        decoder[0x0f3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                cpu.prefixes|=CPU.PREFIX_REP;
                cpu.repeat_zero = true;
                boolean result = decoder[cpu.op_index+cpu.fetchb()].call(cpu, prev);
                cpu.prefixes&=~CPU.PREFIX_REP;
                return result;
            }
        };
        decoder[0x2f3] = decoder[0x0f3];

        /* CMC */
        decoder[0x0f5] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Cmc();
                return true;
            }
        };
        decoder[0x2f5] = decoder[0x0f5];

        /* GRP3 Eb(,Ib) */
        decoder[0x0f6] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:											/* TEST Eb,Ib */
                    case 0x01:											/* TEST Eb,Ib Undocumented*/
                        EbIb_noresult(cpu, prev, Instructions.andb, rm);
                        break;
                    case 0x02:											/* NOT Eb */
                        Eb(cpu, prev, Instructions.not, rm);
                        break;
                    case 0x03:                                          /* NEG Eb */
                        Eb(cpu, prev, Instructions.negb, rm);
                        break;
                    case 0x04:											/* MUL AL,Eb */
                        EbCustom(cpu, prev, Instructions.mulb, rm);
                        break;
                    case 0x05:											/* IMUL AL,Eb */
                        EbCustom(cpu, prev, Instructions.imulb, rm);
                        break;
                    case 0x06:											/* DIV Eb */
                        EbCustom(cpu, prev, Instructions.divb, rm);
                        break;
                    case 0x07:											/* IDIV Eb */
                        EbCustom(cpu, prev, Instructions.idivb, rm);
                        break;
                }
                return true;
            }
        };
        decoder[0x2f6] = decoder[0x0f6];

        /* GRP3 Ew(,Iw) */
        decoder[0x0f7] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:											/* TEST Ew,Iw */
                    case 0x01:											/* TEST Ew,Iw Undocumented*/
                        EwIw_noresult(cpu, prev, Instructions.andw, rm);
                        break;
                    case 0x02:											/* NOT Ew */
                        Ew(cpu, prev, Instructions.not, rm);
                        break;
                    case 0x03:                                          /* NEG Ew */
                        Ew(cpu, prev, Instructions.negw, rm);
                        break;
                    case 0x04:											/* MUL AX,Ew */
                        EwCustom(cpu, prev, Instructions.mulw, rm);
                        break;
                    case 0x05:											/* IMUL AX,Ew */
                        EwCustom(cpu, prev, Instructions.imulw, rm);
                        break;
                    case 0x06:											/* DIV Ew */
                        EwCustom(cpu, prev, Instructions.divw, rm);
                        break;
                    case 0x07:											/* IDIV Ew */
                        EwCustom(cpu, prev, Instructions.idivw, rm);
                        break;
                }
                return true;
            }
        };

        /* GRP3 Ed(,Id) */
        decoder[0x2f7] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:											/* TEST Ed,Id */
                    case 0x01:											/* TEST Ed,Id Undocumented*/
                        EdId_noresult(cpu, prev, Instructions.andd, rm);
                        break;
                    case 0x02:											/* NOT Ed */
                        Ed(cpu, prev, Instructions.not, rm);
                        break;
                    case 0x03:                                          /* NEG Ed */
                        Ed(cpu, prev, Instructions.negd, rm);
                        break;
                    case 0x04:											/* MUL EAX,Ed */
                        EdCustom(cpu, prev, Instructions.muld, rm);
                        break;
                    case 0x05:											/* IMUL EAX,Ed */
                        EdCustom(cpu, prev, Instructions.imuld, rm);
                        break;
                    case 0x06:											/* DIV Ed */
                        EdCustom(cpu, prev, Instructions.divd, rm);
                        break;
                    case 0x07:											/* IDIV Ed */
                        EdCustom(cpu, prev, Instructions.idivd, rm);
                        break;
                }
                return true;
            }
        };

        /* CLC */
        decoder[0x0f8] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Clc();
                return true;
            }
        };
        decoder[0x2f8] = decoder[0x0f8];

        /* STC */
        decoder[0x0f9] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Stc();
                return true;
            }
        };
        decoder[0x2f9] = decoder[0x0f9];

         /* CLD */
        decoder[0x0fc] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Cld();
                return true;
            }
        };
        decoder[0x2fc] = decoder[0x0fc];

        /* STD */
        decoder[0x0fd] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.Std();
                return true;
            }
        };
        decoder[0x2fd] = decoder[0x0fd];

        /* GRP4 Eb */
        decoder[0x0fe] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:										/* INC Eb */
                        Eb(cpu, prev, Instructions.incb, rm);
                        break;
                    case 0x01:										/* DEC Eb */
                        Eb(cpu, prev, Instructions.decb, rm);
                        break;
                    case 0x07:										/* CallBack */
                        if (!cpu.big)
                            Log.panic("Callback not implemented for 16-bit yet");
                        prev.next = new Ops.CallbackOp(cpu.thread.process.loader.getCallback(cpu.fetchw()));
                        return false;
                    default:
                        Log.panic("Illegal GRP4 Call "+((rm>>3) & 7));
                }
                return true;
            }
        };
        decoder[0x2fe] = decoder[0x0fe];

        /* GRP5 Ew */
        decoder[0x0ff] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:										/* INC Ew */
                        Ew(cpu, prev, Instructions.incw, rm);
                        return true;
                    case 0x01:										/* DEC Ew */
                        Ew(cpu, prev, Instructions.decw, rm);
                        return true;
                    case 0x02:										/* CALL Ev */
                        if (rm >= 0xc0 ) {
                            prev.next = new Ops.CallEv_reg(ew(cpu, rm));
                        }
                        else {
                            prev.next = new Ops.CallEv_mem(getEaa(cpu,rm));
                        }
                        return false;
                    case 0x03:										/* CALL Ep */
                        Log.panic("Call Ep (0xFF) not implemented");
                        return false;
                    case 0x04:										/* JMP Ev */
                        if (rm >= 0xc0) {
                            prev.next = new Ops.JmpEv_reg(ew(cpu, rm));
                        } else {
                            prev.next = new Ops.JmpEv_mem(getEaa(cpu,rm));
                        }
                        return false;
                    case 0x05:										/* JMP Ep */
                        Log.panic("Jmp Ep (0xFF) not implemented");
                        return false;
                    case 0x06:										/* PUSH Ev */
                        if (rm >= 0xc0) {
                            prev.next = new Ops.PushEv_reg(ew(cpu, rm));
                        } else {
                            prev.next = new Ops.PushEv_mem(getEaa(cpu,rm));
                        }
                        return true;
                    default:
                        Log.panic("CPU:GRP5:Illegal Call "+Integer.toString(which,16));
                        return false;
                }
            }
        };

        /* GRP 5 Ed */
        decoder[0x2ff] = new Decode() {
            final public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                int which=(rm>>3)&7;
                switch (which) {
                    case 0x00:											/* INC Ed */
                        Ed(cpu, prev, Instructions.incd, rm);
                        return true;
                    case 0x01:											/* DEC Ed */
                        Ed(cpu, prev, Instructions.decd, rm);
                        return true;
                    case 0x02:											/* CALL NEAR Ed */
                        if (rm >= 0xc0 ) {
                            prev.next = new Ops.CallNearEd_reg(ed(cpu, rm));
                        } else {
                            prev.next = new Ops.CallNearEd_mem(getEaa(cpu, rm));
                        }
                        return false;
                    case 0x03:											/* CALL FAR Ed */
                        Log.panic("CALL FAR Ed not implemented");
                        return false;
                    case 0x04:											/* JMP NEAR Ed */
                        if (rm >= 0xc0 ) {
                            prev.next = new Ops.JmpNearEd_reg(ed(cpu, rm));
                        } else {
                            prev.next = new Ops.JmpNearEd_mem(getEaa(cpu, rm));
                        }
                        return false;
                    case 0x05:											/* JMP FAR Ed */
                        Log.panic("JMP FAR Ed not implemented");
                        return false;
                    case 0x06:											/* Push Ed */
                        if (rm >= 0xc0) {
                            prev.next = new Ops.PushEd_reg(ed(cpu, rm));
                        } else {
                            prev.next = new Ops.PushEd_mem(getEaa(cpu, rm));
                        }
                        return true;
                    default:
                        Log.panic("CPU:66:GRP5:Illegal call "+Integer.toString(which,16));
                        return false;
                }
            }
        };

        for (int i=0;i<16;i++) {
            final int index = i;
            decoder[0x340+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    int rm = cpu.fetchb();
                    if (rm >= 0xc0 ) {
                        prev.next = new Ops.ConditionalMov_Reg(Conditions.get(index), ed(cpu, rm), gd(cpu, rm));
                    } else {
                        prev.next = new Ops.ConditionalMov_Mem(Conditions.get(index), getEaa(cpu, rm), gd(cpu, rm));
                    }
                    return true;
                }
            };
        }

        for (int i=0;i<16;i++) {
            final int index = i;
            decoder[0x180+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Jump(Conditions.get(index), cpu.fetchws());
                    return false;
                }
            };
        }

        for (int i=0;i<16;i++) {
            final int index = i;
            decoder[0x380+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    prev.next = new Ops.Jump(Conditions.get(index), cpu.fetchd());
                    return false;
                }
            };
        }

        for (int i=0;i<16;i++) {
            final int index = i;
            decoder[0x190+i] = new Decode() {
                public boolean call(CPU cpu, Op prev) {
                    int rm = cpu.fetchb();
                    if (rm >= 0xc0) {
                        prev.next = new Ops.SetFlag_Reg8(eb(cpu, rm), Conditions.get(index));
                    } else {
                        prev.next = new Ops.SetFlag_Mem8(getEaa(cpu, rm), Conditions.get(index));
                    }
                    return true;
                }
            };
            decoder[0x390+i] = decoder[0x190+i];
        }

        /* CPUID */
        decoder[0x1a2] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                prev.next = new Ops.CPUID();
                return true;
            }
        };
        decoder[0x3a2] = decoder[0x1a2];

        /* BT Ew,Gw */
        decoder[0x1a3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.BtEwGw_reg(ew(cpu, rm), gw(cpu, rm));
                } else {
                    prev.next = new Ops.BtEwGw_mem(getEaa(cpu, rm), gw(cpu, rm));
                }
                return true;
            }
        };

        /* BT Ed,Gd */
        decoder[0x3a3] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.BtEdGd_reg(ed(cpu, rm), gd(cpu, rm));
                } else {
                    prev.next = new Ops.BtEdGd_mem(getEaa(cpu, rm), gd(cpu, rm));
                }
                return true;
            }
        };

        /* SHLD Ew,Gw,Ib */
        decoder[0x1a4] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Instruction_Reg16_Reg16_Value_3(Instructions.dshlw, ew(cpu, rm), gw(cpu, rm), cpu.fetchb());
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    prev.next = new Ops.Instruction_Mem16_Reg16_Value_3(Instructions.dshlw, eaa, gw(cpu, rm), cpu.fetchb());
                }
                return true;
            }
        };

        /* SHLD Ed,Gd,Ib */
        decoder[0x3a4] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Instruction_Reg32_Reg32_Value_3(Instructions.dshld, ed(cpu, rm), gd(cpu, rm), cpu.fetchb());
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    prev.next = new Ops.Instruction_Mem32_Reg32_Value_3(Instructions.dshld, eaa, gd(cpu, rm), cpu.fetchb());
                }
                return true;
            }
        };

        /* SHRD Ew,Gw,Ib */
        decoder[0x1ac] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Instruction_Reg16_Reg16_Value_3(Instructions.dshrw, ew(cpu, rm), gw(cpu, rm), cpu.fetchb());
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    prev.next = new Ops.Instruction_Mem16_Reg16_Value_3(Instructions.dshrw, eaa, gw(cpu, rm), cpu.fetchb());
                }
                return true;
            }
        };

        /* SHRD Ed,Gd,Ib */
        decoder[0x3ac] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Instruction_Reg32_Reg32_Value_3(Instructions.dshrd, ed(cpu, rm), gd(cpu, rm), cpu.fetchb());
                } else {
                    EaaBase eaa = getEaa(cpu, rm);
                    prev.next = new Ops.Instruction_Mem32_Reg32_Value_3(Instructions.dshrd, eaa, gd(cpu, rm), cpu.fetchb());
                }
                return true;
            }
        };

        /* SHRD Ew,Gw,CL */
        decoder[0x1ad] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Instruction_Reg16_Reg16_Reg8(Instructions.dshrw, ew(cpu, rm), gw(cpu, rm), cpu.ecx);
                } else {
                    prev.next = new Ops.Instruction_Mem16_Reg16_Reg8(Instructions.dshrw, getEaa(cpu, rm), gw(cpu, rm), cpu.ecx);
                }
                return true;
            }
        };

        /* SHRD Ed,Gd,CL */
        decoder[0x3ad] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Instruction_Reg32_Reg32_Reg8(Instructions.dshrd, ed(cpu, rm), gd(cpu, rm), cpu.ecx);
                } else {
                    prev.next = new Ops.Instruction_Mem32_Reg32_Reg8(Instructions.dshrd, getEaa(cpu, rm), gd(cpu, rm), cpu.ecx);
                }
                return true;
            }
        };

        /* IMUL Gw,Ew */
        decoder[0x1af] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GdEd(cpu, prev, Instructions.dimulw);
                return true;
            }
        };

        /* IMUL Gd,Ed */
        decoder[0x3af] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                GdEd(cpu, prev, Instructions.dimuld);
                return true;
            }
        };

        /* CMPXCHG Ew,Gw */
        decoder[0x1b1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.CmpxchgEwGw_reg(ew(cpu, rm), gw(cpu, rm));
                } else {
                    prev.next = new Ops.CmpxchgEwGw_mem(getEaa(cpu, rm), gw(cpu, rm));
                }
                return true;
            }
        };

        /* CMPXCHG Ed,Gd */
        decoder[0x3b1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.CmpxchgEdGd_reg(ed(cpu, rm), gd(cpu, rm));
                } else {
                    prev.next = new Ops.CmpxchgEdGd_mem(getEaa(cpu, rm), gd(cpu, rm));
                }
                return true;
            }
        };

        /* MOVZX Gw,Eb */
        decoder[0x1b6] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg16_Reg8(gd(cpu, rm), eb(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg16_Mem8(gd(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* MOVZX Gd,Eb */
        decoder[0x3b6] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg32_Reg8(gd(cpu, rm), eb(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg32_Mem8(gd(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* MOVXZ Gd,Ew */
        decoder[0x3b7] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg32_Reg16(gd(cpu, rm), ew(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg32_Mem16(gd(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };

         /* BTC Ed,Gd */
        decoder[0x3bb] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.BtcEdGd_reg(ed(cpu, rm), gd(cpu, rm));
                } else {
                    prev.next = new Ops.BtcEdGd_mem(getEaa(cpu, rm), gd(cpu, rm));
                }
                return true;
            }
        };

        /* BSF Gd,Ed */
        decoder[0x3bc] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.BsfGdEd_reg(ed(cpu, rm), gd(cpu, rm));
                } else {
                    prev.next = new Ops.BsfGdEd_mem(getEaa(cpu, rm), gd(cpu, rm));
                }
                return true;
            }
        };

        /* BSR Gd,Ed */
        decoder[0x3bd] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.BsrGdEd_reg(ed(cpu, rm), gd(cpu, rm));
                } else {
                    prev.next = new Ops.BsrGdEd_mem(getEaa(cpu, rm), gd(cpu, rm));
                }
                return true;
            }
        };

        /* MOVSX Gw,Eb */
        decoder[0x1be] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg16_Reg8_sx(gd(cpu, rm), eb(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg16_Mem8_sx(gd(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* MOVSX Gd,Eb */
        decoder[0x3be] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg32_Reg8_sx(gd(cpu, rm), eb(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg32_Mem8_sx(gd(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* MOVSX Gd,Ew */
        decoder[0x3bf] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Mov_Reg32_Reg16_sx(gd(cpu, rm), ew(cpu, rm));
                } else {
                    prev.next = new Ops.Mov_Reg32_Mem16_sx(gd(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* XADD Gb,Eb */
        decoder[0x1c0] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Xadd_Reg8_Reg8(gb(cpu, rm), eb(cpu, rm));
                } else {
                    prev.next = new Ops.Xadd_Reg8_Mem8(gb(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };
        decoder[0x3c0] = decoder[0x1c0];

        /* XADD Gw,Ew */
        decoder[0x1c1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Xadd_Reg16_Reg16(gw(cpu, rm), ew(cpu, rm));
                } else {
                    prev.next = new Ops.Xadd_Reg16_Mem16(gw(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };

        /* XADD Gd,Ed */
        decoder[0x3c1] = new Decode() {
            public boolean call(CPU cpu, Op prev) {
                int rm=cpu.fetchb();
                if (rm >= 0xc0 ) {
                    prev.next = new Ops.Xadd_Reg32_Reg32(gd(cpu, rm), ed(cpu, rm));
                } else {
                    prev.next = new Ops.Xadd_Reg32_Mem32(gd(cpu, rm), getEaa(cpu, rm));
                }
                return true;
            }
        };
    }

    
    static public Op decode(CPU cpu) {
        Op result = new Op() { public Block call(CPU cpu){return null;}};
        Op op = result;
        boolean done = false;

        while (!done) {
            int start = cpu.cseip;
            done = !decoder[cpu.op_index +cpu.fetchb()].call(cpu, op);
            if (op.next!=null)
                op = op.next;
            op.eip_count = cpu.cseip-start;
            op.eip = start-cpu.cs.dword;
        }
        return result.next;
    }
}

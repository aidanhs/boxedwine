package wine.emulation;

// adapted from jdosbox which was ported from Dosbox
public class FPU {
    static private final boolean LOG = false;

    private static final int FMASK_TEST	= (CPU.CF | CPU.PF | CPU.AF | CPU.ZF | CPU.SF | CPU.OF);
    
    private double[] regs = new double[9];
    private int[] tags = new int[9];
    private int cw;
    private int cw_mask_all;
    private int sw;
    private int top;
    private int round;
    final private Memory memory;
    final private CPU cpu;
    
    public FPU(CPU cpu, Memory memory) {
        this.memory = memory;
        this.cpu = cpu;
        FPU_FINIT();
    }
    
    private static class FPU_P_Reg {
        long m1;
        long m2;
        int m3;

        int d1;
        long d2;
    }

    private static final int TAG_Valid = 0;
    private static final int TAG_Zero = 1;
    private static final int TAG_Weird = 2;
    private static final int TAG_Empty = 3;

    public static final int ROUND_Nearest = 0;
    public static final int ROUND_Down = 1;
    public static final int ROUND_Up = 2;
    public static final int ROUND_Chop = 3;

    //get pi from a real library
    public static final double PI = 3.14159265358979323846;
    public static final double L2E = 1.4426950408889634;
    public static final double L2T = 3.3219280948873623;
    public static final double LN2 = 0.69314718055994531;
    public static final double LG2 = 0.3010299956639812;


    private int STV(int i) {
        return ((top + (i)) & 7);
    }

    private void FPU_SetTag(int tag) {
        for (int i = 0; i < 8; i++)
            tags[i] = ((tag >> (2 * i)) & 3);
    }

    private void FPU_SetCW(int word) {
        cw = word;
        cw_mask_all = word | 0x3f;
        round = ((word >>> 10) & 3);
    }

    private int FPU_GET_TOP() {
        return (sw & 0x3800) >> 11;
    }

    private void FPU_SET_TOP(int val) {
        sw &= ~0x3800;
        sw |= (val & 7) << 11;
    }


    private void FPU_SET_C0(int C) {
        sw &= ~0x0100;
        if (C != 0) sw |= 0x0100;
    }

    private void FPU_SET_C1(int C) {
        sw &= ~0x0200;
        if (C != 0) sw |= 0x0200;
    }

    private void FPU_SET_C2(int C) {
        sw &= ~0x0400;
        if (C != 0) sw |= 0x0400;
    }

    private void FPU_SET_C3(int C) {
        sw &= ~0x4000;
        if (C != 0) sw |= 0x4000;
    }

    private void FPU_FINIT() {
        FPU_SetCW(0x37F);
        sw = 0;
        top = FPU_GET_TOP();
        tags[0] = TAG_Empty;
        tags[1] = TAG_Empty;
        tags[2] = TAG_Empty;
        tags[3] = TAG_Empty;
        tags[4] = TAG_Empty;
        tags[5] = TAG_Empty;
        tags[6] = TAG_Empty;
        tags[7] = TAG_Empty;
        tags[8] = TAG_Valid; // is only used by us
    }

    private void FPU_FCLEX() {
        sw &= 0x7f00;            //should clear exceptions
    }

    private void FPU_FNOP() {
    }

    private void FPU_PUSH(double in) {
        top = (top - 1) & 7;
        //actually check if empty
        tags[top] = TAG_Valid;
        regs[top] = in;
    }

    private void FPU_PREP_PUSH() {
        top = (top - 1) & 7;
        tags[top] = TAG_Valid;
    }

    public void FPU_FPOP() {
        tags[top] = TAG_Empty;
        //maybe set zero in it as well
        top = ((top + 1) & 7);
    }

    private double FROUND(double in) {
        switch (round) {
            case ROUND_Nearest:
                if (in - Math.floor(in) > 0.5) return (Math.floor(in) + 1);
                else if (in - Math.floor(in) < 0.5) return (Math.floor(in));
                else return ((((long) (Math.floor(in))) & 1) != 0) ? (Math.floor(in) + 1) : (Math.floor(in));
            case ROUND_Down:
                return (Math.floor(in));
            case ROUND_Up:
                return (Math.ceil(in));
            case ROUND_Chop:
                return in; //the cast afterwards will do it right maybe cast here
            default:
                return in;
        }
    }

    static private final int BIAS80 = 16383;
    static private final int BIAS64 = 1023;

    static private double FPU_FLD80(long eind, int begin) {
        long exp64 = (((begin & 0x7fff) - BIAS80));
        long blah = ((exp64 > 0) ? exp64 : -exp64) & 0x3ff;
        long exp64final = ((exp64 > 0) ? blah : -blah) + BIAS64;

        // 0x3FFF is for rounding
        int round = 0;
        if (round == ROUND_Nearest)
            round = 0x3FF;
        else if (round == ROUND_Up) {
            round = 0x7FF;
        }

        long mant64 = ((eind + round) >>> 11) & 0xfffffffffffffl;
        long sign = (begin & 0x8000) != 0 ? 1 : 0;
        double result = Double.longBitsToDouble((sign << 63) | (exp64final << 52) | mant64);

        if (eind == 0x8000000000000000l && (begin & 0x7fff) == 0x7fff) {
            //Detect INF and -INF (score 3.11 when drawing a slur.)
            result = sign != 0 ? Double.NEGATIVE_INFINITY : Double.POSITIVE_INFINITY;
        }
        return result;

        //mant64= test.mant80/2***64    * 2 **53
    }

    private void FPU_ST80(int addr, int reg) {
        long value = Double.doubleToRawLongBits(regs[reg]);
        long sign80 = (value & (0x8000000000000000l)) != 0 ? 1 : 0;
        long exp80 = value & (0x7ff0000000000000l);
        long exp80final = (exp80 >> 52);
        long mant80 = value & (0x000fffffffffffffl);
        long mant80final = (mant80 << 11);
        if (regs[reg] != 0) { //Zero is a special case
            // Elvira wants the 8 and tcalc doesn't
            mant80final |= 0x8000000000000000l;
            //Ca-cyber doesn't like this when result is zero.
            exp80final += (BIAS80 - BIAS64);
        }
        memory.writed(addr, (int) mant80final);
        memory.writed(addr + 4, (int) (mant80final >>> 32));
        memory.writew(addr + 8, (int) ((sign80 << 15) | (exp80final)));
    }


    private void FPU_FLD_F32(int value, int store_to) {
        regs[store_to] = Float.intBitsToFloat(value);
    }

    private void FPU_FLD_F64(long value, int store_to) {
        regs[store_to] = Double.longBitsToDouble(value);
    }

    private void FPU_FLD_F80(long low, int high) {
        regs[top] = FPU_FLD80(low, high);
    }

    private void FPU_FLD_I16(short value, int store_to) {
        regs[store_to] = value;
    }

    private void FPU_FLD_I32(int value, int store_to) {
        regs[store_to] = value;
    }

    private void FPU_FLD_I64(long value, int store_to) {
        regs[store_to] = value;
    }

    private void FPU_FBLD(byte[] data, int store_to) {
        long val = 0;
        int in = 0;
        long base = 1;
        for (int i = 0; i < 9; i++) {
            in = data[i] & 0xFF;
            val += ((in & 0xf) * base); //in&0xf shouldn't be higher then 9
            base *= 10;
            val += (((in >> 4) & 0xf) * base);
            base *= 10;
        }

        //last number, only now convert to float in order to get
        //the best signification
        double temp = (double) (val);
        in = data[9] & 0xFF;
        temp += ((in & 0xf) * base);
        if ((in & 0x80) != 0) temp *= -1.0;
        regs[store_to] = temp;
    }


    private void FPU_FLD_F32_EA(int addr) {
        FPU_FLD_F32(memory.readd(addr), 8);
    }

    private void FPU_FLD_F64_EA(int addr) {
        FPU_FLD_F64(memory.readq(addr), 8);
    }

    private void FPU_FLD_I32_EA(int addr) {
        FPU_FLD_I32(memory.readd(addr), 8);
    }

    private void FPU_FLD_I16_EA(int addr) {
        FPU_FLD_I16((short) memory.readw(addr), 8);
    }

    private void FPU_FST_F32(int addr) {
        //should depend on rounding method
        memory.writed(addr, Float.floatToRawIntBits((float) regs[top]));
    }

    private void FPU_FST_F64(int addr) {
        memory.writeq(addr, Double.doubleToRawLongBits(regs[top]));
    }

    private void FPU_FST_F80(int addr) {
        FPU_ST80(addr, top);
    }

    private void FPU_FST_I16(int addr) {
        memory.writew(addr, (short) (FROUND(regs[top])));
    }

    private void FPU_FST_I32(int addr) {
        memory.writed(addr, (int) (FROUND(regs[top])));
    }

    private void FPU_FST_I64(int addr) {
        memory.writeq(addr, (long) FROUND(regs[top]));
    }

    private void FPU_FBST(int addr) {
        boolean sign = false;
        double val = regs[top];
        if ((Double.doubleToRawLongBits(val) & 0x8000000000000000l) != 0) { //sign
            sign = true;
            val = -val;
        }
        
        //numbers from back to front        
        double temp = val;        
        int p;
        
        for (int i = 0; i < 9; i++) {
            val = temp;
            temp = (double) ((long) (Math.floor(val / 10.0)));
            p = (int) (val - 10.0 * temp);
            val = temp;
            temp = (double) ((long) (Math.floor(val / 10.0)));
            p |= ((int) (val - 10.0 * temp) << 4);

            memory.writeb(addr + i, p);
        }
        val = temp;
        temp = (double) ((long) (Math.floor(val / 10.0)));
        p = (int) (val - 10.0 * temp);
        if (sign)
            p |= 0x80;
        memory.writeb(addr + 9, p);
    }

    private void FPU_FADD(int op1, int op2) {
        regs[op1] += regs[op2];
        //flags and such :)
    }

    private void FPU_FSIN() {
        regs[top] = Math.sin(regs[top]);
        FPU_SET_C2(0);
        //flags and such :)
    }

    private void FPU_FSINCOS() {        
        double temp = regs[top];
        regs[top] = Math.sin(temp);
        FPU_PUSH(Math.cos(temp));
        FPU_SET_C2(0);
        //flags and such :)
    }

    private void FPU_FCOS() {
        regs[top] = Math.cos(regs[top]);
        FPU_SET_C2(0);
        //flags and such :)
    }

    private void FPU_FSQRT() {
        regs[top] = Math.sqrt(regs[top]);
        //flags and such :)
    }

    private void FPU_FPATAN() {
        regs[STV(1)] = Math.atan2(regs[STV(1)], regs[top]);
        FPU_FPOP();
        //flags and such :)
    }

    private void FPU_FPTAN() {
        regs[top] = Math.tan(regs[top]);
        FPU_PUSH(1.0);
        FPU_SET_C2(0);
        //flags and such :)
    }

    private void FPU_FDIV(int st, int other) {
        regs[st] = regs[st] / regs[other];
        //flags and such :)
    }

    private void FPU_FDIVR(int st, int other) {
        regs[st] = regs[other] / regs[st];
        // flags and such :)
    }

    private void FPU_FMUL(int st, int other) {
        regs[st] *= regs[other];
        //flags and such :)
    }

    private void FPU_FSUB(int st, int other) {
        regs[st] = regs[st] - regs[other];
        //flags and such :)
    }

    private void FPU_FSUBR(int st, int other) {
        regs[st] = regs[other] - regs[st];
        //flags and such :)
    }

    private void FPU_FXCH(int st, int other) {
        int tag = tags[other];
        double reg = regs[other];
        tags[other] = tags[st];
        regs[other] = regs[st];
        tags[st] = tag;
        regs[st] = reg;
    }

    private void FPU_FST(int st, int other) {
        tags[other] = tags[st];
        regs[other] = regs[st];
    }

    private void setFlags(int newFlags) {
        cpu.fillFlags();
        cpu.flags &= ~FMASK_TEST;
        cpu.flags |= (newFlags & FMASK_TEST);
    }

    private void FPU_FCOMI(int st, int other) {
        if (((tags[st] != TAG_Valid) && (tags[st] != TAG_Zero)) ||
                ((tags[other] != TAG_Valid) && (tags[other] != TAG_Zero)) || Double.isNaN(regs[st]) || Double.isNaN(regs[other])) {
            setFlags(CPU.ZF | CPU.PF | CPU.CF);
            return;
        }
        if (regs[st] == regs[other]) {
            setFlags(CPU.ZF);
            return;
        }
        if (regs[st] < regs[other]) {
            setFlags(CPU.CF);
            return;
        }
        // st > other
        setFlags(0);
    }

    private void FPU_FCOM(int st, int other) {
        if (((tags[st] != TAG_Valid) && (tags[st] != TAG_Zero)) ||
                ((tags[other] != TAG_Valid) && (tags[other] != TAG_Zero)) || Double.isNaN(regs[st]) || Double.isNaN(regs[other])) {
            FPU_SET_C3(1);
            FPU_SET_C2(1);
            FPU_SET_C0(1);
            return;
        }
        if (regs[st] == regs[other]) {
            FPU_SET_C3(1);
            FPU_SET_C2(0);
            FPU_SET_C0(0);
            return;
        }
        if (regs[st] < regs[other]) {
            FPU_SET_C3(0);
            FPU_SET_C2(0);
            FPU_SET_C0(1);
            return;
        }
        // st > other
        FPU_SET_C3(0);
        FPU_SET_C2(0);
        FPU_SET_C0(0);
    }

    private void FPU_FUCOM(int st, int other) {
        //does atm the same as fcom
        FPU_FCOM(st, other);
    }

    private void FPU_FRNDINT() {        
        long temp = (long) (FROUND(regs[top]));
        regs[top] = (double) (temp);
    }

    private void FPU_FPREM() {        
        double valtop = regs[top];        
        double valdiv = regs[STV(1)];        
        long ressaved = (long) ((valtop / valdiv));
        // Some backups
        //	double res=valtop - ressaved*valdiv;
        //      res= fmod(valtop,valdiv);
        regs[top] = valtop - ressaved * valdiv;
        FPU_SET_C0((int) (ressaved & 4));
        FPU_SET_C3((int) (ressaved & 2));
        FPU_SET_C1((int) (ressaved & 1));
        FPU_SET_C2(0);
    }

    private void FPU_FPREM1() {        
        double valtop = regs[top];        
        double valdiv = regs[STV(1)];
        double quot = valtop / valdiv;
        double quotf = Math.floor(quot);        
        long ressaved;
        
        if (quot - quotf > 0.5) ressaved = (long) (quotf + 1);
        else if (quot - quotf < 0.5) ressaved = (long) (quotf);
        else ressaved = (long) (((((long) (quotf)) & 1) != 0) ? (quotf + 1) : (quotf));
        regs[top] = valtop - ressaved * valdiv;
        FPU_SET_C0((int) (ressaved & 4));
        FPU_SET_C3((int) (ressaved & 2));
        FPU_SET_C1((int) (ressaved & 1));
        FPU_SET_C2(0);
    }

    private void FPU_FXAM() {
        long bits = Double.doubleToRawLongBits(regs[top]);
        if ((bits & 0x8000000000000000l) != 0)    //sign
        {
            FPU_SET_C1(1);
        } else {
            FPU_SET_C1(0);
        }

        if (tags[top] == TAG_Empty) {
            FPU_SET_C3(1);
            FPU_SET_C2(0);
            FPU_SET_C0(1);
            return;
        }
        if (Double.isNaN(regs[top])) {
            FPU_SET_C3(0);
            FPU_SET_C2(0);
            FPU_SET_C0(1);
        } else if (Double.isInfinite(regs[top])) {
            FPU_SET_C3(0);
            FPU_SET_C2(1);
            FPU_SET_C0(1);
        } else if (regs[top] == 0.0)        //zero or normalized number.
        {
            FPU_SET_C3(1);
            FPU_SET_C2(0);
            FPU_SET_C0(0);
        } else {
            FPU_SET_C3(0);
            FPU_SET_C2(1);
            FPU_SET_C0(0);
        }
    }


    private void FPU_F2XM1() {
        regs[top] = Math.pow(2.0, regs[top]) - 1;
    }

    private void FPU_FYL2X() {
        regs[STV(1)] *= Math.log(regs[top]) / Math.log(2.0);
        FPU_FPOP();
    }

    private void FPU_FYL2XP1() {
        regs[STV(1)] *= Math.log(regs[top] + 1.0) / Math.log(2.0);
        FPU_FPOP();
    }

    private void FPU_FSCALE() {
        regs[top] *= Math.pow(2.0, (double) ((long) (regs[STV(1)])));
        //2^x where x is chopped.
    }

    private void FPU_FSTENV(int addr) {
        FPU_SET_TOP(top);
        if (!cpu.big) {
            memory.writew(addr + 0, (cw));
            memory.writew(addr + 2, (sw));
            memory.writew(addr + 4, (FPU_GetTag()));
        } else {
            memory.writed(addr + 0, (cw));
            memory.writed(addr + 4, (sw));
            memory.writed(addr + 8, (FPU_GetTag()));
        }
    }

    private void FPU_FLDENV(int addr) {
        int tag;
        long tagbig;        
        int cw;
        
        if (!cpu.big) {
            cw = memory.readw(addr + 0);
            sw = memory.readw(addr + 2);
            tag = memory.readw(addr + 4);
        } else {
            cw = memory.readd(addr + 0);
            sw = memory.readd(addr + 4);
            tagbig = memory.readd(addr + 8) & 0xFFFFFFFFl;
            tag = (int) (tagbig);
        }
        FPU_SetTag(tag);
        FPU_SetCW(cw);
        top = FPU_GET_TOP();
    }

    private void FPU_FSAVE(int addr) {
        FPU_FSTENV(addr);
        
        int start = (cpu.big ? 28 : 14);
        for (int i = 0; i < 8; i++) {
            FPU_ST80(addr + start, STV(i));
            start += 10;
        }
        FPU_FINIT();
    }

    private void FPU_FRSTOR(int addr) {
        FPU_FLDENV(addr);
        
        int start = (cpu.big ? 28 : 14);
        for (int i = 0; i < 8; i++) {
            regs[STV(i)] = FPU_FLD80(memory.readq(addr + start), memory.readw(addr + start + 8));
            start += 10;
        }
    }

    private void FPU_FXTRACT() {
        // function stores real bias in st and
        // pushes the significant number onto the stack
        // if double ever uses a different base please correct this function
        long bits = Double.doubleToRawLongBits(regs[top]);        
        long exp80 = bits & 0x7ff0000000000000l;        
        long exp80final = (exp80 >> 52) - BIAS64;        
        double mant = regs[top] / (Math.pow(2.0, (double) (exp80final)));
        
        regs[top] = (double) (exp80final);
        FPU_PUSH(mant);
    }

    private void FPU_FCHS() {
        regs[top] = -1.0 * (regs[top]);
    }

    private void FPU_FABS() {
        regs[top] = Math.abs(regs[top]);
    }

    private void FPU_FTST() {
        regs[8] = 0.0;
        FPU_FCOM(top, 8);
    }

    private void FPU_FLD1() {
        FPU_PREP_PUSH();
        regs[top] = 1.0;
    }

    private void FPU_FLDL2T() {
        FPU_PREP_PUSH();
        regs[top] = L2T;
    }

    private void FPU_FLDL2E() {
        FPU_PREP_PUSH();
        regs[top] = L2E;
    }

    private void FPU_FLDPI() {
        FPU_PREP_PUSH();
        regs[top] = PI;
    }

    private void FPU_FLDLG2() {
        FPU_PREP_PUSH();
        regs[top] = LG2;
    }

    private void FPU_FLDLN2() {
        FPU_PREP_PUSH();
        regs[top] = LN2;
    }

    private void FPU_FLDZ() {
        FPU_PREP_PUSH();
        regs[top] = 0.0;
        tags[top] = TAG_Zero;
    }


    private void FPU_FADD_EA(int op1) {
        FPU_FADD(op1, 8);
    }

    private void FPU_FMUL_EA(int op1) {
        FPU_FMUL(op1, 8);
    }

    private void FPU_FSUB_EA(int op1) {
        FPU_FSUB(op1, 8);
    }

    private void FPU_FSUBR_EA(int op1) {
        FPU_FSUBR(op1, 8);
    }

    private void FPU_FDIV_EA(int op1) {
        FPU_FDIV(op1, 8);
    }

    private void FPU_FDIVR_EA(int op1) {
        FPU_FDIVR(op1, 8);
    }

    private void FPU_FCOM_EA(int op1) {
        FPU_FCOM(op1, 8);
    }

    private void FPU_FLDCW(int addr) {
        int temp = memory.readw(addr);
        FPU_SetCW(temp);
    }

    private int FPU_GetTag() {        
        int tag = 0;
        for (int i = 0; i < 8; i++)
            tag |= ((tags[i] & 3) << (2 * i));
        return tag;
    }

/* WATCHIT : ALWAYS UPDATE REGISTERS BEFORE AND AFTER USING THEM
            STATUS WORD =>	FPU_SET_TOP(fpu.top) BEFORE a read
			fpu.top=FPU_GET_TOP() after a write;
			*/

    public void FADD_SINGLE_REAL(int addr) {
        FPU_FLD_F32_EA(addr);
        FPU_FADD_EA(top);
    }

    public void FMUL_SINGLE_REAL(int addr) {
        FPU_FLD_F32_EA(addr);
        FPU_FMUL_EA(top);
    }

    public void FCOM_SINGLE_REAL(int addr, boolean pop) {
        FPU_FLD_F32_EA(addr);
        FPU_FCOM_EA(top);
        if (pop)
            FPU_FPOP();
    }

    public void FSUB_SINGLE_REAL(int addr) {
        FPU_FLD_F32_EA(addr);
        FPU_FSUB_EA(top);
    }

    public void FSUBR_SINGLE_REAL(int addr) {
        FPU_FLD_F32_EA(addr);
        FPU_FSUBR_EA(top);
    }

    public void FDIV_SINGLE_REAL(int addr) {
        FPU_FLD_F32_EA(addr);
        FPU_FDIV_EA(top);
    }

    public void FDIVR_SINGLE_REAL(int addr) {
        FPU_FLD_F32_EA(addr);
        FPU_FDIVR_EA(top);
    }

    public void FADD_ST0_STj(int sub) {
        FPU_FADD(top, STV(sub));
    }

    public void FMUL_ST0_STj(int sub) {
        FPU_FMUL(top, STV(sub));
    }

    public void FCOM_STi(int sub, boolean pop) {
        FPU_FCOM(top, STV(sub));
        if (pop)
            FPU_FPOP();
    }

    public void FSUB_ST0_STj(int sub) {
        FPU_FSUB(top, STV(sub));
    }

    public void FSUBR_ST0_STj(int sub) {
        FPU_FSUBR(top, STV(sub));
    }

    public void FDIV_ST0_STj(int sub) {
        FPU_FDIV(top, STV(sub));
    }

    public void FDIVR_ST0_STj(int sub) {
        FPU_FDIVR(top, STV(sub));
    }

    public void FLD_SINGLE_REAL(int address) {
        int value = memory.readd(address); // might generate PF, so do before we adjust the stack
        FPU_PREP_PUSH();
        FPU_FLD_F32(value, top);
    }

    public void FST_SINGLE_REAL(int address, boolean pop) {
        FPU_FST_F32(address);
        if (pop)
            FPU_FPOP();
    }

    public void FLDENV(int address) {
        FPU_FLDENV(address);
    }

    public void FLDCW(int address) {
        FPU_FLDCW(address);
    }

    public void FNSTENV(int address) {
        FPU_FSTENV(address);
    }

    public void FNSTCW(int address) {
        memory.writew(address, cw);
    }

    public void FLD_STi(int sub) {
        int reg_from = STV(sub);
        FPU_PREP_PUSH();
        FPU_FST(reg_from, top);
    }

    public void FXCH_STi(int sub) {
        FPU_FXCH(top, STV(sub));
    }

    public void FNOP() {

    }

    public void FCHS() {
        FPU_FCHS();
    }

    public void FABS() {
        FPU_FABS();
    }

    public void FTST() {
        FPU_FTST();
    }

    public void FXAM() {
        FPU_FXAM();
    }

    public void FLD1() {
        FPU_FLD1();
    }

    public void FLDL2T() {
        FPU_FLDL2T();
    }

    public void FLDL2E() {
        FPU_FLDL2E();
    }

    public void FLDPI() {
        FPU_FLDPI();
    }

    public void FLDLG2() {
        FPU_FLDLG2();
    }

    public void FLDLN2() {
        FPU_FLDLN2();
    }

    public void FLDZ() {
        FPU_FLDZ();
    }

    public void F2XM1() {
        FPU_F2XM1();
    }

    public void FYL2X() {
        FPU_FYL2X();
    }

    public void FPTAN() {
        FPU_FPTAN();
    }

    public void FPATAN() {
        FPU_FPATAN();
    }

    public void FXTRACT() {
        FPU_FXTRACT();
    }

    public void FPREM(boolean bRoundNearest) {
        if (bRoundNearest)
            FPU_FPREM1();
        else
            FPU_FPREM();
    }

    public void FDECSTP() {
        top = (top - 1) & 7;
    }

    public void FINCSTP() {
        top = (top + 1) & 7;
    }

    public void FYL2XP1() {
        FPU_FYL2XP1();
    }

    public void FSQRT() {
        FPU_FSQRT();
    }

    public void FSINCOS() {
        FPU_FSINCOS();
    }

    public void FRNDINT() {
        FPU_FRNDINT();
    }

    public void FSCALE() {
        FPU_FSCALE();
    }

    public void FSIN() {
        FPU_FSIN();
    }

    public void FCOS() {
        FPU_FCOS();
    }

    public void FST_STi(int sub, boolean pop) {
        FPU_FST(top, STV(sub));
        if (pop)
            FPU_FPOP();
    }

    public void FIADD_DWORD_INTEGER(int address) {
        FPU_FLD_I32_EA(address);
        FPU_FADD_EA(top);
    }

    public void FIMUL_DWORD_INTEGER(int address) {
        FPU_FLD_I32_EA(address);
        FPU_FMUL_EA(top);
    }

    public void FICOM_DWORD_INTEGER(int address, boolean pop) {
        FPU_FLD_I32_EA(address);
        FPU_FCOM_EA(top);
        if (pop)
            FPU_FPOP();
    }

    public void FISUB_DWORD_INTEGER(int address) {
        FPU_FLD_I32_EA(address);
        FPU_FSUB_EA(top);
    }

    public void FISUBR_DWORD_INTEGER(int address) {
        FPU_FLD_I32_EA(address);
        FPU_FSUBR_EA(top);
    }

    public void FIDIV_DWORD_INTEGER(int address) {
        FPU_FLD_I32_EA(address);
        FPU_FDIV_EA(top);
    }

    public void FIDIVR_DWORD_INTEGER(int address) {
        FPU_FLD_I32_EA(address);
        FPU_FDIVR_EA(top);
    }

    public void FCMOV_ST0_STj(int rm, boolean condition) {
        if (condition)
            FPU_FST(STV(rm), 0);
    }

    public void FUCOMPP() {
        FPU_FUCOM(top, STV(1));
        FPU_FPOP();
        FPU_FPOP();
    }

    public void FILD_DWORD_INTEGER(int address) {
        int value = memory.readd(address); // might generate PF, so do before we adjust the stack
        FPU_PREP_PUSH();
        FPU_FLD_I32(value, top);
    }

    public void FISTTP32(int address) {
        memory.writed(address, (int) regs[STV(0)]);
        FPU_FPOP();
    }

    public void FIST_DWORD_INTEGER(int address, boolean pop) {
        FPU_FST_I32(address);
        if (pop)
            FPU_FPOP();
    }

    public void FLD_EXTENDED_REAL(int address) {
        long low = memory.readq(address); // might generate PF, so do before we adjust the stack
        int high = memory.readw(address + 8);
        FPU_PREP_PUSH();
        FPU_FLD_F80(low, high);
    }

    public void FSTP_EXTENDED_REAL(int address) {
        FPU_FST_F80(address);
        FPU_FPOP();
    }

    public void FNCLEX() {
        FPU_FCLEX();
    }

    public void FNINIT() {
        FPU_FINIT();
    }

    // Quiet compare
    public void FUCOMI_ST0_STj(int rm, boolean pop) {
        FPU_FCOMI(top, STV(rm));
        if (pop)
            FPU_FPOP();
    }

    // Signaling compare :TODO:
    public void FCOMI_ST0_STj(int rm, boolean pop) {
        FPU_FCOMI(top, STV(rm));
        if (pop)
            FPU_FPOP();
    }

    public void FADD_DOUBLE_REAL(int address) {
        FPU_FLD_F64_EA(address);
        FPU_FADD_EA(top);
    }

    public void FMUL_DOUBLE_REAL(int address) {
        FPU_FLD_F64_EA(address);
        FPU_FMUL_EA(top);
    }

    public void FCOM_DOUBLE_REAL(int address, boolean pop) {
        FPU_FLD_F64_EA(address);
        FPU_FCOM_EA(top);
        if (pop)
            FPU_FPOP();
    }

    public void FSUB_DOUBLE_REAL(int address) {
        FPU_FLD_F64_EA(address);
        FPU_FSUB_EA(top);
    }

    public void FSUBR_DOUBLE_REAL(int address) {
        FPU_FLD_F64_EA(address);
        FPU_FSUBR_EA(top);
    }

    public void FDIV_DOUBLE_REAL(int address) {
        FPU_FLD_F64_EA(address);
        FPU_FDIV_EA(top);
    }

    public void FDIVR_DOUBLE_REAL(int address) {
        FPU_FLD_F64_EA(address);
        FPU_FDIVR_EA(top);
    }

    public void FADD_STi_ST0(int rm, boolean pop) {
        FPU_FADD(STV(rm), top);
        if (pop)
            FPU_FPOP();
    }

    public void FMUL_STi_ST0(int rm, boolean pop) {
        FPU_FMUL(STV(rm), top);
        if (pop)
            FPU_FPOP();
    }

    public void FSUBR_STi_ST0(int rm, boolean pop) {
        FPU_FSUBR(STV(rm), top);
        if (pop)
            FPU_FPOP();
    }

    public void FSUB_STi_ST0(int rm, boolean pop) {
        FPU_FSUB(STV(rm), top);
        if (pop)
            FPU_FPOP();
    }

    public void FDIVR_STi_ST0(int rm, boolean pop) {
        FPU_FDIVR(STV(rm), top);
        if (pop)
            FPU_FPOP();
    }

    public void FDIV_STi_ST0(int rm, boolean pop) {
        FPU_FDIV(STV(rm), top);
        if (pop)
            FPU_FPOP();
    }

    public void FLD_DOUBLE_REAL(int address) {
        long value = memory.readq(address); // might generate PF, so do before we adjust the stack
        FPU_PREP_PUSH();
        FPU_FLD_F64(value, top);
    }

    public void FISTTP64(int address) {
        memory.writeq(address, (long) regs[STV(0)]);
        FPU_FPOP();
    }

    public void FST_DOUBLE_REAL(int address, boolean pop) {
        FPU_FST_F64(address);
        if (pop)
            FPU_FPOP();
    }

    public void FRSTOR(int address) {
        FPU_FRSTOR(address);
    }

    public void FNSAVE(int address) {
        FPU_FSAVE(address);
    }

    public void FNSTSW(int address) {
        FPU_SET_TOP(top);
        memory.writew(address, sw);
    }

    public void FFREE_STi(int rm) {
        tags[STV(rm)] = TAG_Empty;
    }

    public void FUCOM_STi(int rm, boolean pop) {
        FPU_FUCOM(top, STV(rm));
        if (pop)
            FPU_FPOP();
    }

    public void FIADD_WORD_INTEGER(int addr) {
        FPU_FLD_I16_EA(addr);
        FPU_FADD_EA(top);
    }

    public void FIMUL_WORD_INTEGER(int addr) {
        FPU_FLD_I16_EA(addr);
        FPU_FMUL_EA(top);
    }

    public void FICOM_WORD_INTEGER(int addr, boolean pop) {
        FPU_FLD_I16_EA(addr);
        FPU_FCOM_EA(top);
        if (pop)
            FPU_FPOP();
    }

    public void FISUB_WORD_INTEGER(int addr) {
        FPU_FLD_I16_EA(addr);
        FPU_FSUB_EA(top);
    }

    public void FISUBR_WORD_INTEGER(int addr) {
        FPU_FLD_I16_EA(addr);
        FPU_FSUBR_EA(top);
    }

    public void FIDIV_WORD_INTEGER(int addr) {
        FPU_FLD_I16_EA(addr);
        FPU_FDIV_EA(top);
    }

    public void FIDIVR_WORD_INTEGER(int addr) {
        FPU_FLD_I16_EA(addr);
        FPU_FDIVR_EA(top);
    }

    public void FCOMPP() {
        FPU_FCOM(top, STV(1));
        FPU_FPOP();
        FPU_FPOP();
    }

    public void FILD_WORD_INTEGER(int address) {
        short value = (short) memory.readw(address); // might generate PF, so do before we adjust the stack
        FPU_PREP_PUSH();
        FPU_FLD_I16(value, top);
    }

    public void FISTTP16(int address) {
        memory.writew(address, (short) regs[STV(0)]);
        FPU_FPOP();
    }

    public void FIST_WORD_INTEGER(int address, boolean pop) {
        FPU_FST_I16(address);
        if (pop)
            FPU_FPOP();
    }

    public void FBLD_PACKED_BCD(int address) {
        byte[] value = new byte[10];
        memory.memcpy(value, 0, 10, address); // might generate PF, so do before we adjust the stack
        FPU_PREP_PUSH();
        FPU_FBLD(value, top);
    }

    public void FILD_QWORD_INTEGER(int address) {
        long value = memory.readq(address); // might generate PF, so do before we adjust the stack
        FPU_PREP_PUSH();
        FPU_FLD_I64(value, top);
    }

    public void FBSTP_PACKED_BCD(int address) {
        FPU_FBST(address);
        FPU_FPOP();
    }

    public void FISTP_QWORD_INTEGER(int address) {
        FPU_FST_I64(address);
        FPU_FPOP();
    }

    public void FFREEP_STi(int rm) {
        tags[STV(rm)] = TAG_Empty;
        FPU_FPOP();
    }

    public void FNSTSW_AX() {
        FPU_SET_TOP(top);
        cpu.eax.u16(sw);
    }
}

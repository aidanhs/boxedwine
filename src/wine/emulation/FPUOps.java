package wine.emulation;

public class FPUOps {
    abstract static protected class FpuOpEA extends Op {
        final EaaBase eaa;

        public FpuOpEA(EaaBase eaa) {
            this.eaa = eaa;
        }
    }

    abstract static protected class FpuOpNormal extends Op {
        final int rm;

        public FpuOpNormal(int rm) {
            this.rm = rm;
        }
    }

    final static public class FADD_SINGLE_REAL extends FpuOpEA {
        public FADD_SINGLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FADD_SINGLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FADD_SINGLE_REAL";
        }
    }

    final static public class FMUL_SINGLE_REAL extends FpuOpEA {
        public FMUL_SINGLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FMUL_SINGLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FMUL_SINGLE_REAL";
        }
    }

    final static public class FCOM_SINGLE_REAL extends FpuOpEA {
        final boolean pop;

        public FCOM_SINGLE_REAL(EaaBase eaa, boolean pop) {
            super(eaa);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCOM_SINGLE_REAL(eaa.call(cpu), pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FCOM_SINGLE_REAL";
        }
    }

    final static public class FSUB_SINGLE_REAL extends FpuOpEA {
        public FSUB_SINGLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSUB_SINGLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FSUB_SINGLE_REAL";
        }
    }

    final static public class FSUBR_SINGLE_REAL extends FpuOpEA {
        public FSUBR_SINGLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSUBR_SINGLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FSUBR_SINGLE_REAL";
        }
    }

    final static public class FDIV_SINGLE_REAL extends FpuOpEA {
        public FDIV_SINGLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FDIV_SINGLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FDIV_SINGLE_REAL";
        }
    }

    final static public class FDIVR_SINGLE_REAL extends FpuOpEA {
        public FDIVR_SINGLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FDIVR_SINGLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FDIVR_SINGLE_REAL";
        }
    }

    final static public class FADD_ST0_STj extends FpuOpNormal {
        public FADD_ST0_STj(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FADD_ST0_STj(rm);
            return next.call(cpu);
        }

        public String toString() {
            return "FADD_ST0_STj";
        }
    }

    final static public class FMUL_ST0_STj extends FpuOpNormal {
        public FMUL_ST0_STj(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FMUL_ST0_STj(rm);
            return next.call(cpu);
        }

        public String toString() {
            return "FMUL_ST0_STj";
        }
    }

    final static public class FCOM_STi extends FpuOpNormal {
        boolean pop;

        public FCOM_STi(int rm, boolean pop) {
            super(rm);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCOM_STi(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FCOM_STi";
        }
    }

    final static public class FSUB_ST0_STj extends FpuOpNormal {
        public FSUB_ST0_STj(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSUB_ST0_STj(rm);
            return next.call(cpu);
        }

        public String toString() {
            return "FSUB_ST0_STj";
        }
    }

    final static public class FSUBR_ST0_STj extends FpuOpNormal {
        public FSUBR_ST0_STj(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSUBR_ST0_STj(rm);
            return next.call(cpu);
        }

        public String toString() {
            return "FSUBR_ST0_STj";
        }
    }

    final static public class FDIV_ST0_STj extends FpuOpNormal {
        public FDIV_ST0_STj(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FDIV_ST0_STj(rm);
            return next.call(cpu);
        }

        public String toString() {
            return "FDIV_ST0_STj";
        }
    }

    final static public class FDIVR_ST0_STj extends FpuOpNormal {
        public FDIVR_ST0_STj(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FDIVR_ST0_STj(rm);
            return next.call(cpu);
        }

        public String toString() {
            return "FDIVR_ST0_STj";
        }
    }

    final static public class FLD_SINGLE_REAL extends FpuOpEA {
        public FLD_SINGLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLD_SINGLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FLD_SINGLE_REAL";
        }
    }

    final static public class FST_SINGLE_REAL extends FpuOpEA {
        boolean pop;

        public FST_SINGLE_REAL(EaaBase eaa, boolean pop) {
            super(eaa);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FST_SINGLE_REAL(eaa.call(cpu), pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FST_SINGLE_REAL";
        }
    }

    final static public class FLDENV extends FpuOpEA {
        public FLDENV(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLDENV(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FLDENV";
        }
    }

    final static public class FLDCW extends FpuOpEA {
        public FLDCW(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLDCW(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FLDCW";
        }
    }

    final static public class FNSTENV extends FpuOpEA {
        public FNSTENV(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FNSTENV(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FNSTENV";
        }
    }

    final static public class FNSTCW extends FpuOpEA {
        public FNSTCW(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FNSTCW(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FNSTCW";
        }
    }

    final static public class FLD_STi extends FpuOpNormal {
        public FLD_STi(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLD_STi(rm);
            return next.call(cpu);
        }

        public String toString() {
            return "FLD_STi";
        }
    }

    final static public class FXCH_STi extends FpuOpNormal {
        public FXCH_STi(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FXCH_STi(rm);
            return next.call(cpu);
        }

        public String toString() {
            return "FXCH_STi";
        }
    }

    final static public class FNOP extends FpuOpNormal {
        public FNOP() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FNOP();
            return next.call(cpu);
        }

        public String toString() {
            return "FNOP";
        }
    }

    final static public class FST_STi extends FpuOpNormal {
        boolean pop;

        public FST_STi(int rm, boolean pop) {
            super(rm);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FST_STi(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FST_STi";
        }
    }

    final static public class FCHS extends FpuOpNormal {
        public FCHS() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCHS();
            return next.call(cpu);
        }

        public String toString() {
            return "FCHS";
        }
    }

    final static public class FABS extends FpuOpNormal {
        public FABS() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FABS();
            return next.call(cpu);
        }

        public String toString() {
            return "FABS";
        }
    }

    final static public class FTST extends FpuOpNormal {
        public FTST() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FTST();
            return next.call(cpu);
        }

        public String toString() {
            return "FTST";
        }
    }

    final static public class FXAM extends FpuOpNormal {
        public FXAM() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FXAM();
            return next.call(cpu);
        }

        public String toString() {
            return "FXAM";
        }
    }

    final static public class FLD1 extends FpuOpNormal {
        public FLD1() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLD1();
            return next.call(cpu);
        }

        public String toString() {
            return "FLD1";
        }
    }

    final static public class FLDL2T extends FpuOpNormal {
        public FLDL2T() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLDL2T();
            return next.call(cpu);
        }

        public String toString() {
            return "FLDL2T";
        }
    }

    final static public class FLDL2E extends FpuOpNormal {
        public FLDL2E() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLDL2E();
            return next.call(cpu);
        }

        public String toString() {
            return "FLDL2E";
        }
    }

    final static public class FLDPI extends FpuOpNormal {
        public FLDPI() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLDPI();
            return next.call(cpu);
        }

        public String toString() {
            return "FLDPI";
        }
    }

    final static public class FLDLG2 extends FpuOpNormal {
        public FLDLG2() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLDLG2();
            return next.call(cpu);
        }

        public String toString() {
            return "FLDLG2";
        }
    }

    final static public class FLDLN2 extends FpuOpNormal {
        public FLDLN2() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLDLN2();
            return next.call(cpu);
        }

        public String toString() {
            return "FLDLN2";
        }
    }

    final static public class FLDZ extends FpuOpNormal {
        public FLDZ() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLDZ();
            return next.call(cpu);
        }

        public String toString() {
            return "FLDZ";
        }
    }

    final static public class F2XM1 extends FpuOpNormal {
        public F2XM1() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.F2XM1();
            return next.call(cpu);
        }

        public String toString() {
            return "F2XM1";
        }
    }

    final static public class FYL2X extends FpuOpNormal {
        public FYL2X() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FYL2X();
            return next.call(cpu);
        }

        public String toString() {
            return "FYL2X";
        }
    }

    final static public class FPTAN extends FpuOpNormal {
        public FPTAN() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FPTAN();
            return next.call(cpu);
        }

        public String toString() {
            return "FPTAN";
        }
    }

    final static public class FPATAN extends FpuOpNormal {
        public FPATAN() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FPATAN();
            return next.call(cpu);
        }

        public String toString() {
            return "FPATAN";
        }
    }

    final static public class FXTRACT extends FpuOpNormal {
        public FXTRACT() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FXTRACT();
            return next.call(cpu);
        }

        public String toString() {
            return "FXTRACT";
        }
    }

    final static public class FPREM extends FpuOpNormal {
        boolean bRoundNearest;

        public FPREM(boolean bRoundNearest) {
            super(0);
            this.bRoundNearest = bRoundNearest;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FPREM(bRoundNearest);
            return next.call(cpu);
        }

        public String toString() {
            return "FPREM" + (bRoundNearest ? " round nearest" : "");
        }
    }

    final static public class FDECSTP extends FpuOpNormal {
        public FDECSTP() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FDECSTP();
            return next.call(cpu);
        }

        public String toString() {
            return "FDECSTP";
        }
    }

    final static public class FINCSTP extends FpuOpNormal {
        public FINCSTP() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FINCSTP();
            return next.call(cpu);
        }

        public String toString() {
            return "FINCSTP";
        }
    }

    final static public class FYL2XP1 extends FpuOpNormal {
        public FYL2XP1() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FYL2XP1();
            return next.call(cpu);
        }

        public String toString() {
            return "FYL2XP1";
        }
    }

    final static public class FSQRT extends FpuOpNormal {
        public FSQRT() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSQRT();
            return next.call(cpu);
        }

        public String toString() {
            return "FSQRT";
        }
    }

    final static public class FSINCOS extends FpuOpNormal {
        public FSINCOS() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSINCOS();
            return next.call(cpu);
        }

        public String toString() {
            return "FSINCOS";
        }
    }

    final static public class FRNDINT extends FpuOpNormal {
        public FRNDINT() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FRNDINT();
            return next.call(cpu);
        }

        public String toString() {
            return "FRNDINT";
        }
    }

    final static public class FSCALE extends FpuOpNormal {
        public FSCALE() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSCALE();
            return next.call(cpu);
        }

        public String toString() {
            return "FSCALE";
        }
    }

    final static public class FSIN extends FpuOpNormal {
        public FSIN() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSIN();
            return next.call(cpu);
        }

        public String toString() {
            return "FSIN";
        }
    }

    final static public class FCOS extends FpuOpNormal {
        public FCOS() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCOS();
            return next.call(cpu);
        }

        public String toString() {
            return "FCOS";
        }
    }

    final static public class FIADD_DWORD_INTEGER extends FpuOpEA {
        public FIADD_DWORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FIADD_DWORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FIADD_DWORD_INTEGER";
        }
    }

    final static public class FIMUL_DWORD_INTEGER extends FpuOpEA {
        public FIMUL_DWORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FIMUL_DWORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FIMUL_DWORD_INTEGER";
        }
    }

    final static public class FICOM_DWORD_INTEGER extends FpuOpEA {
        boolean pop;

        public FICOM_DWORD_INTEGER(EaaBase eaa, boolean pop) {
            super(eaa);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FICOM_DWORD_INTEGER(eaa.call(cpu), pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FICOM_DWORD_INTEGER";
        }
    }

    final static public class FISUB_DWORD_INTEGER extends FpuOpEA {
        public FISUB_DWORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FISUB_DWORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FISUB_DWORD_INTEGER";
        }
    }

    final static public class FISUBR_DWORD_INTEGER extends FpuOpEA {
        public FISUBR_DWORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FISUBR_DWORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FISUBR_DWORD_INTEGER";
        }
    }

    final static public class FIDIV_DWORD_INTEGER extends FpuOpEA {
        public FIDIV_DWORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FIDIV_DWORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FIDIV_DWORD_INTEGER";
        }
    }

    final static public class FIDIVR_DWORD_INTEGER extends FpuOpEA {
        public FIDIVR_DWORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FIDIVR_DWORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FIDIVR_DWORD_INTEGER";
        }
    }

    final static public class FCMOV_ST0_STj_CF extends FpuOpNormal {
        public FCMOV_ST0_STj_CF(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCMOV_ST0_STj(rm, cpu.CF());
            return next.call(cpu);
        }

        public String toString() {
            return "FCMOV_ST0_STj_CF";
        }
    }

    final static public class FCMOV_ST0_STj_ZF extends FpuOpNormal {
        public FCMOV_ST0_STj_ZF(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCMOV_ST0_STj(rm, cpu.ZF());
            return next.call(cpu);
        }

        public String toString() {
            return "FCMOV_ST0_STj_ZF";
        }
    }

    final static public class FCMOV_ST0_STj_CF_OR_ZF extends FpuOpNormal {
        public FCMOV_ST0_STj_CF_OR_ZF(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCMOV_ST0_STj(rm, cpu.CF() || cpu.ZF());
            return next.call(cpu);
        }

        public String toString() {
            return "FCMOV_ST0_STj_CF_OR_ZF";
        }
    }

    final static public class FCMOV_ST0_STj_PF extends FpuOpNormal {
        public FCMOV_ST0_STj_PF(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCMOV_ST0_STj(rm, cpu.PF());
            return next.call(cpu);
        }

        public String toString() {
            return "FCMOV_ST0_STj_PF";
        }
    }

    final static public class FUCOMPP extends FpuOpNormal {
        public FUCOMPP() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FUCOMPP();
            return next.call(cpu);
        }

        public String toString() {
            return "FUCOMPP";
        }
    }

    final static public class FILD_DWORD_INTEGER extends FpuOpEA {
        public FILD_DWORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FILD_DWORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FILD_DWORD_INTEGER";
        }
    }

    final static public class FISTTP32 extends FpuOpEA {
        public FISTTP32(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FISTTP32(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FISTTP32";
        }
    }

    final static public class FIST_DWORD_INTEGER extends FpuOpEA {
        boolean pop;

        public FIST_DWORD_INTEGER(EaaBase eaa, boolean pop) {
            super(eaa);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FIST_DWORD_INTEGER(eaa.call(cpu), pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FIST_DWORD_INTEGER";
        }
    }

    final static public class FLD_EXTENDED_REAL extends FpuOpEA {
        public FLD_EXTENDED_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLD_EXTENDED_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FLD_EXTENDED_REAL";
        }
    }

    final static public class FSTP_EXTENDED_REAL extends FpuOpEA {
        public FSTP_EXTENDED_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSTP_EXTENDED_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FSTP_EXTENDED_REAL";
        }
    }

    final static public class FCMOV_ST0_STj_NCF extends FpuOpNormal {
        public FCMOV_ST0_STj_NCF(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCMOV_ST0_STj(rm, !cpu.CF());
            return next.call(cpu);
        }

        public String toString() {
            return "FCMOV_ST0_STj_NCF";
        }
    }

    final static public class FCMOV_ST0_STj_NZF extends FpuOpNormal {
        public FCMOV_ST0_STj_NZF(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCMOV_ST0_STj(rm, !cpu.ZF());
            return next.call(cpu);
        }

        public String toString() {
            return "FCMOV_ST0_STj_NZF";
        }
    }

    final static public class FCMOV_ST0_STj_NCF_AND_NZF extends FpuOpNormal {
        public FCMOV_ST0_STj_NCF_AND_NZF(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCMOV_ST0_STj(rm, !cpu.CF() && !cpu.ZF());
            return next.call(cpu);
        }

        public String toString() {
            return "FCMOV_ST0_STj_NCF_AND_NZF";
        }
    }

    final static public class FCMOV_ST0_STj_NPF extends FpuOpNormal {
        public FCMOV_ST0_STj_NPF(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCMOV_ST0_STj(rm, !cpu.PF());
            return next.call(cpu);
        }

        public String toString() {
            return "FCMOV_ST0_STj_NPF";
        }
    }

    final static public class FNCLEX extends FpuOpNormal {
        public FNCLEX() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FNCLEX();
            return next.call(cpu);
        }

        public String toString() {
            return "FNCLEX";
        }
    }

    final static public class FNINIT extends FpuOpNormal {
        public FNINIT() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FNINIT();
            return next.call(cpu);
        }

        public String toString() {
            return "FNINIT";
        }
    }

    final static public class FUCOMI_ST0_STj extends FpuOpNormal {
        boolean pop;

        public FUCOMI_ST0_STj(int rm, boolean pop) {
            super(0);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FUCOMI_ST0_STj(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FUCOMI_ST0_STj";
        }
    }

    final static public class FCOMI_ST0_STj extends FpuOpNormal {
        boolean pop;

        public FCOMI_ST0_STj(int rm, boolean pop) {
            super(0);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCOMI_ST0_STj(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FCOMI_ST0_STj";
        }
    }

    final static public class FADD_DOUBLE_REAL extends FpuOpEA {
        public FADD_DOUBLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FADD_DOUBLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FADD_DOUBLE_REAL";
        }
    }

    final static public class FMUL_DOUBLE_REAL extends FpuOpEA {
        public FMUL_DOUBLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FMUL_DOUBLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FMUL_DOUBLE_REAL";
        }
    }

    final static public class FCOM_DOUBLE_REAL extends FpuOpEA {
        boolean pop;

        public FCOM_DOUBLE_REAL(EaaBase eaa, boolean pop) {
            super(eaa);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCOM_DOUBLE_REAL(eaa.call(cpu), pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FCOM_DOUBLE_REAL";
        }
    }

    final static public class FSUB_DOUBLE_REAL extends FpuOpEA {
        public FSUB_DOUBLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSUB_DOUBLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FSUB_DOUBLE_REAL";
        }
    }

    final static public class FSUBR_DOUBLE_REAL extends FpuOpEA {
        public FSUBR_DOUBLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSUBR_DOUBLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FSUBR_DOUBLE_REAL";
        }
    }

    final static public class FDIV_DOUBLE_REAL extends FpuOpEA {
        public FDIV_DOUBLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FDIV_DOUBLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FDIV_DOUBLE_REAL";
        }
    }

    final static public class FDIVR_DOUBLE_REAL extends FpuOpEA {
        public FDIVR_DOUBLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FDIVR_DOUBLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FDIVR_DOUBLE_REAL";
        }
    }

    final static public class FADD_STi_ST0 extends FpuOpNormal {
        boolean pop;
        public FADD_STi_ST0(int rm, boolean pop) {
            super(rm);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FADD_STi_ST0(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FADD_STi_ST0";
        }
    }

    final static public class FMUL_STi_ST0 extends FpuOpNormal {
        boolean pop;
        public FMUL_STi_ST0(int rm, boolean pop) {
            super(rm);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FMUL_STi_ST0(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FMUL_STi_ST0";
        }
    }

    final static public class FSUBR_STi_ST0 extends FpuOpNormal {
        boolean pop;
        public FSUBR_STi_ST0(int rm, boolean pop) {
            super(rm);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSUBR_STi_ST0(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FSUBR_STi_ST0";
        }
    }

    final static public class FSUB_STi_ST0 extends FpuOpNormal {
        boolean pop;
        public FSUB_STi_ST0(int rm, boolean pop) {
            super(rm);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FSUB_STi_ST0(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FSUB_STi_ST0";
        }
    }

    final static public class FDIVR_STi_ST0 extends FpuOpNormal {
        boolean pop;
        public FDIVR_STi_ST0(int rm, boolean pop) {
            super(rm);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FDIVR_STi_ST0(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FDIVR_STi_ST0";
        }
    }

    final static public class FDIV_STi_ST0 extends FpuOpNormal {
        boolean pop;
        public FDIV_STi_ST0(int rm, boolean pop) {
            super(rm);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FDIV_STi_ST0(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FDIV_STi_ST0";
        }
    }

    final static public class FLD_DOUBLE_REAL extends FpuOpEA {
        public FLD_DOUBLE_REAL(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FLD_DOUBLE_REAL(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FLD_DOUBLE_REAL";
        }
    }

    final static public class FISTTP64 extends FpuOpEA {
        public FISTTP64(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FISTTP64(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FISTTP64";
        }
    }

    final static public class FST_DOUBLE_REAL extends FpuOpEA {
        boolean pop;
        public FST_DOUBLE_REAL(EaaBase eaa, boolean pop) {
            super(eaa);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FST_DOUBLE_REAL(eaa.call(cpu), pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FST_DOUBLE_REAL";
        }
    }

    final static public class FRSTOR extends FpuOpEA {
        public FRSTOR(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FRSTOR(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FRSTOR";
        }
    }

    final static public class FNSAVE extends FpuOpEA {
        public FNSAVE(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FNSAVE(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FNSAVE";
        }
    }

    final static public class FNSTSW extends FpuOpEA {
        public FNSTSW(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FNSTSW(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FNSTSW";
        }
    }

    final static public class FFREE_STi extends FpuOpNormal {
        public FFREE_STi(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FFREE_STi(rm);
            return next.call(cpu);
        }

        public String toString() {
            return "FFREE_STi";
        }
    }

    final static public class FUCOM_STi extends FpuOpNormal {
        boolean pop;
        public FUCOM_STi(int rm, boolean pop) {
            super(rm);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FUCOM_STi(rm, pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FUCOM_STi";
        }
    }

    final static public class FIADD_WORD_INTEGER extends FpuOpEA {
        public FIADD_WORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FIADD_WORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FIADD_WORD_INTEGER";
        }
    }

    final static public class FIMUL_WORD_INTEGER extends FpuOpEA {
        public FIMUL_WORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FIMUL_WORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FIMUL_WORD_INTEGER";
        }
    }

    final static public class FICOM_WORD_INTEGER extends FpuOpEA {
        boolean pop;

        public FICOM_WORD_INTEGER(EaaBase eaa, boolean pop) {
            super(eaa);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FICOM_WORD_INTEGER(eaa.call(cpu), pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FICOM_WORD_INTEGER";
        }
    }

    final static public class FISUB_WORD_INTEGER extends FpuOpEA {
        public FISUB_WORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FISUB_WORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FISUB_WORD_INTEGER";
        }
    }

    final static public class FISUBR_WORD_INTEGER extends FpuOpEA {
        public FISUBR_WORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FISUBR_WORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FISUBR_WORD_INTEGER";
        }
    }

    final static public class FIDIV_WORD_INTEGER extends FpuOpEA {
        public FIDIV_WORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FIDIV_WORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FIDIV_WORD_INTEGER";
        }
    }

    final static public class FIDIVR_WORD_INTEGER extends FpuOpEA {
        public FIDIVR_WORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FIDIVR_WORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FIDIVR_WORD_INTEGER";
        }
    }

    final static public class FCOMPP extends FpuOpNormal {
        public FCOMPP() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FCOMPP();
            return next.call(cpu);
        }

        public String toString() {
            return "FCOMPP";
        }
    }

    final static public class FILD_WORD_INTEGER extends FpuOpEA {
        public FILD_WORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FILD_WORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FILD_WORD_INTEGER";
        }
    }

    final static public class FISTTP16 extends FpuOpEA {
        public FISTTP16(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FISTTP16(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FISTTP16";
        }
    }

    final static public class FIST_WORD_INTEGER extends FpuOpEA {
        boolean pop;

        public FIST_WORD_INTEGER(EaaBase eaa, boolean pop) {
            super(eaa);
            this.pop = pop;
        }

        public Block call(CPU cpu) {
            cpu.fpu.FIST_WORD_INTEGER(eaa.call(cpu), pop);
            return next.call(cpu);
        }

        public String toString() {
            return "FIST_WORD_INTEGER";
        }
    }

    final static public class FBLD_PACKED_BCD extends FpuOpEA {
        public FBLD_PACKED_BCD(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FBLD_PACKED_BCD(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FBLD_PACKED_BCD";
        }
    }

    final static public class FILD_QWORD_INTEGER extends FpuOpEA {
        public FILD_QWORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FILD_QWORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FILD_QWORD_INTEGER";
        }
    }

    final static public class FBSTP_PACKED_BCD extends FpuOpEA {
        public FBSTP_PACKED_BCD(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FBSTP_PACKED_BCD(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FBSTP_PACKED_BCD";
        }
    }

    final static public class FISTP_QWORD_INTEGER extends FpuOpEA {
        public FISTP_QWORD_INTEGER(EaaBase eaa) {
            super(eaa);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FISTP_QWORD_INTEGER(eaa.call(cpu));
            return next.call(cpu);
        }

        public String toString() {
            return "FISTP_QWORD_INTEGER";
        }
    }

    final static public class FFREEP_STi extends FpuOpNormal {
        public FFREEP_STi(int rm) {
            super(rm);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FFREEP_STi(rm);
            return next.call(cpu);
        }

        public String toString() {
            return "FFREEP_STi";
        }
    }

    final static public class FNSTSW_AX extends FpuOpNormal {
        public FNSTSW_AX() {
            super(0);
        }

        public Block call(CPU cpu) {
            cpu.fpu.FNSTSW_AX();
            return next.call(cpu);
        }

        public String toString() {
            return "FNSTSW_AX";
        }
    }
}

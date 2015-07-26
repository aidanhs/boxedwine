package x86;

import java.io.FileOutputStream;
import java.io.IOException;

public class Shift extends Base {
    boolean flags;

    public void decode(FileOutputStream fos) throws IOException {

    }

    public void decode_noflags(FileOutputStream fos) throws IOException {
        flags = false;
        shiftInst8(fos, "rol8", "op->data1", "1", "3", rol8_noflags, 0, 0, false);
        shiftInst8(fos, "rol8cl", "CL & 0x1f", "4", "4", rol8_noflags, 7, 0, true);
        shiftInst16(fos, "rol16", "op->data1", "1", "3", rol16_noflags, 0, 0, false);
        shiftInst16(fos, "rol16cl", "CL & 0x1f", "4", "4", rol16_noflags, 0xf, 0, true);
        shiftInst32(fos, "rol32", "op->data1", "1", "3", rol32_noflags, 0, 0, false);
        shiftInst32(fos, "rol32cl", "CL & 0x1f", "4", "4", rol32_noflags, 0, 0, true);

        shiftInst8(fos, "ror8", "op->data1", "1", "3", ror8_noflags, 0, 0, false);
        shiftInst8(fos, "ror8cl", "CL & 0x1f", "4", "4", ror8_noflags, 7, 0, true);
        shiftInst16(fos, "ror16", "op->data1", "1", "3", ror16_noflags, 0, 0, false);
        shiftInst16(fos, "ror16cl", "CL & 0x1f", "4", "4", ror16_noflags, 0xf, 0, true);
        shiftInst32(fos, "ror32", "op->data1", "1", "3", ror32_noflags, 0, 0, false);
        shiftInst32(fos, "ror32cl", "CL & 0x1f", "4", "4", ror32_noflags, 0, 0, true);

        shiftInst8(fos, "rcl8", "op->data1", "8", "10", rcl8_noflags, 0, 0, false);
        shiftInst8(fos, "rcl8cl", "CL & 0x1f", "7", "9", rcl8_noflags, 0, 9, true);
        shiftInst16(fos, "rcl16", "op->data1", "8", "10", rcl16_noflags, 0, 0, false);
        shiftInst16(fos, "rcl16cl", "CL & 0x1f", "7", "9", rcl16_noflags, 0, 17, true);
        shiftInst32(fos, "rcl32", "op->data1", "8", "10", rcl32_noflags, 0, 0, false);
        shiftInst32(fos, "rcl32cl", "CL & 0x1f", "7", "9", rcl32_noflags, 0, 0, true);

        shiftInst8(fos, "rcr8", "op->data1", "8", "10", rcr8_noflags, 0, 0, false);
        shiftInst8(fos, "rcr8cl", "CL & 0x1f", "7", "9", rcr8_noflags, 0, 9, true);
        shiftInst16(fos, "rcr16", "op->data1", "8", "10", rcr16_noflags, 0, 0, false);
        shiftInst16(fos, "rcr16cl", "CL & 0x1f", "7", "9", rcr16_noflags, 0, 17, true);
        shiftInst32(fos, "rcr32", "op->data1", "8", "10", rcr32_noflags, 0, 0, false);
        shiftInst32(fos, "rcr32cl", "CL & 0x1f", "7", "9", rcr32_noflags, 0, 0, true);

        shiftInst8(fos, "shl8", "op->data1", "1", "3", shl8_noflags, 0, 0, false);
        shiftInst8(fos, "shl8cl", "CL & 0x1f", "4", "4", shl8_noflags, 0, 0, true);
        shiftInst16(fos, "shl16", "op->data1", "1", "3", shl16_noflags, 0, 0, false);
        shiftInst16(fos, "shl16cl", "CL & 0x1f", "4", "4", shl16_noflags, 0, 0, true);
        shiftInst32(fos, "shl32", "op->data1", "1", "3", shl32_noflags, 0, 0, false);
        shiftInst32(fos, "shl32cl", "CL & 0x1f", "4", "4", shl32_noflags, 0, 0, true);

        shiftInst8(fos, "shr8", "op->data1", "1", "3", shr8_noflags, 0, 0, false);
        shiftInst8(fos, "shr8cl", "CL & 0x1f", "4", "4", shr8_noflags, 0, 0, true);
        shiftInst16(fos, "shr16", "op->data1", "1", "3", shr16_noflags, 0, 0, false);
        shiftInst16(fos, "shr16cl", "CL & 0x1f", "4", "4", shr16_noflags, 0, 0, true);
        shiftInst32(fos, "shr32", "op->data1", "1", "3", shr32_noflags, 0, 0, false);
        shiftInst32(fos, "shr32cl", "CL & 0x1f", "4", "4", shr32_noflags, 0, 0, true);

        shiftInst8(fos, "sar8", "op->data1", "1", "3", sar8_noflags, 0, 0, false);
        shiftInst8(fos, "sar8cl", "CL & 0x1f", "4", "4", sar8_noflags, 0, 0, true);
        shiftInst16(fos, "sar16", "op->data1", "1", "3", sar16_noflags, 0, 0, false);
        shiftInst16(fos, "sar16cl", "CL & 0x1f", "4", "4", sar16_noflags, 0, 0, true);
        shiftInst32(fos, "sar32", "op->data1", "1", "3", sar32_noflags, 0, 0, false);
        shiftInst32(fos, "sar32cl", "CL & 0x1f", "4", "4", sar32_noflags, 0, 0, true);
    }
    public void generate() {
        try {
            FileOutputStream fos = new FileOutputStream("shift.h");
            flags = true;
            shiftInst8(fos, "rol8", "op->data1", "1", "3", rol8, 0, 0, false);
            shiftInst8(fos, "rol8cl", "CL & 0x1f", "4", "4", rol8, 7, 0, true);
            shiftInst16(fos, "rol16", "op->data1", "1", "3", rol16, 0, 0, false);
            shiftInst16(fos, "rol16cl", "CL & 0x1f", "4", "4", rol16, 0xf, 0, true);
            shiftInst32(fos, "rol32", "op->data1", "1", "3", rol32, 0, 0, false);
            shiftInst32(fos, "rol32cl", "CL & 0x1f", "4", "4", rol32, 0, 0, true);

            shiftInst8(fos, "ror8", "op->data1", "1", "3", ror8, 0, 0, false);
            shiftInst8(fos, "ror8cl", "CL & 0x1f", "4", "4", ror8, 7, 0, true);
            shiftInst16(fos, "ror16", "op->data1", "1", "3", ror16, 0, 0, false);
            shiftInst16(fos, "ror16cl", "CL & 0x1f", "4", "4", ror16, 0xf, 0, true);
            shiftInst32(fos, "ror32", "op->data1", "1", "3", ror32, 0, 0, false);
            shiftInst32(fos, "ror32cl", "CL & 0x1f", "4", "4", ror32, 0, 0, true);

            shiftInst8(fos, "rcl8", "op->data1", "8", "10", rcl8, 0, 0, false);
            shiftInst8(fos, "rcl8cl", "CL & 0x1f", "7", "9", rcl8, 0, 9, true);
            shiftInst16(fos, "rcl16", "op->data1", "8", "10", rcl16, 0, 0, false);
            shiftInst16(fos, "rcl16cl", "CL & 0x1f", "7", "9", rcl16, 0, 17, true);
            shiftInst32(fos, "rcl32", "op->data1", "8", "10", rcl32, 0, 0, false);
            shiftInst32(fos, "rcl32cl", "CL & 0x1f", "7", "9", rcl32, 0, 0, true);

            shiftInst8(fos, "rcr8", "op->data1", "8", "10", rcr8, 0, 0, false);
            shiftInst8(fos, "rcr8cl", "CL & 0x1f", "7", "9", rcr8, 0, 9, true);
            shiftInst16(fos, "rcr16", "op->data1", "8", "10", rcr16, 0, 0, false);
            shiftInst16(fos, "rcr16cl", "CL & 0x1f", "7", "9", rcr16, 0, 17, true);
            shiftInst32(fos, "rcr32", "op->data1", "8", "10", rcr32, 0, 0, false);
            shiftInst32(fos, "rcr32cl", "CL & 0x1f", "7", "9", rcr32, 0, 0, true);

            shiftInst8(fos, "shl8", "op->data1", "1", "3", shl8, 0, 0, false);
            shiftInst8(fos, "shl8cl", "CL & 0x1f", "4", "4", shl8, 0, 0, true);
            shiftInst16(fos, "shl16", "op->data1", "1", "3", shl16, 0, 0, false);
            shiftInst16(fos, "shl16cl", "CL & 0x1f", "4", "4", shl16, 0, 0, true);
            shiftInst32(fos, "shl32", "op->data1", "1", "3", shl32, 0, 0, false);
            shiftInst32(fos, "shl32cl", "CL & 0x1f", "4", "4", shl32, 0, 0, true);

            shiftInst8(fos, "shr8", "op->data1", "1", "3", shr8, 0, 0, false);
            shiftInst8(fos, "shr8cl", "CL & 0x1f", "4", "4", shr8, 0, 0, true);
            shiftInst16(fos, "shr16", "op->data1", "1", "3", shr16, 0, 0, false);
            shiftInst16(fos, "shr16cl", "CL & 0x1f", "4", "4", shr16, 0, 0, true);
            shiftInst32(fos, "shr32", "op->data1", "1", "3", shr32, 0, 0, false);
            shiftInst32(fos, "shr32cl", "CL & 0x1f", "4", "4", shr32, 0, 0, true);

            shiftInst8(fos, "sar8", "op->data1", "1", "3", sar8, 0, 0, false);
            shiftInst8(fos, "sar8cl", "CL & 0x1f", "4", "4", sar8, 0, 0, true);
            shiftInst16(fos, "sar16", "op->data1", "1", "3", sar16, 0, 0, false);
            shiftInst16(fos, "sar16cl", "CL & 0x1f", "4", "4", sar16, 0, 0, true);
            shiftInst32(fos, "sar32", "op->data1", "1", "3", sar32, 0, 0, false);
            shiftInst32(fos, "sar32cl", "CL & 0x1f", "4", "4", sar32, 0, 0, true);
            fos.close();
            fos = null;
        } catch (IOException e) {

        }
    }

    static public String rol8 = "fillFlagsNoCFOF(cpu);\r\n    result = (var1 << var2) | (var1 >> (8 - var2));\r\n    setCF(cpu, result & 1);\r\n    setOF(cpu, (result & 1) ^ (result >> 7));";
    static public String rol16 = "fillFlagsNoCFOF(cpu);\r\n    result = (var1 << var2) | (var1 >> (16 - var2));\r\n    setCF(cpu, result & 1);\r\n    setOF(cpu, (result & 1) ^ (result >> 15));";
    static public String rol32 = "fillFlagsNoCFOF(cpu);\r\n    result = (var1 << var2) | (var1 >> (32 - var2));\r\n    setCF(cpu, result & 1);\r\n    setOF(cpu, (result & 1) ^ (result >> 31));";
    static public String ror8 = "fillFlagsNoCFOF(cpu);\r\n    result = (var1 >> var2) | (var1 << (8 - var2));\r\n    setCF(cpu, result & 0x80);\r\n    setOF(cpu, (result ^ (result<<1)) & 0x80);";
    static public String ror16 = "fillFlagsNoCFOF(cpu);\r\n    result = (var1 >> var2) | (var1 << (16 - var2));\r\n    setCF(cpu, result & 0x8000);\r\n    setOF(cpu, (result ^ (result<<1)) & 0x8000);";
    static public String ror32 = "fillFlagsNoCFOF(cpu);\r\n    result = (var1 >> var2) | (var1 << (32 - var2));\r\n    setCF(cpu, result & 0x80000000);\r\n    setOF(cpu, (result ^ (result<<1)) & 0x80000000);";
    static public String rcl8 = "fillFlagsNoOF(cpu);\r\n    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (9-var2));\r\n    setCF(cpu, ((var1 >> (8-var2)) & 1));\r\n    setOF(cpu, (cpu->flags & CF) ^ (result >> 7));";
    static public String rcl16 = "fillFlagsNoOF(cpu);\r\n    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (17-var2));\r\n    setCF(cpu, ((var1 >> (16-var2)) & 1));\r\n    setOF(cpu, (cpu->flags & CF) ^ (result >> 15));";
    static public String rcl32 = "fillFlagsNoOF(cpu);\r\n    if (var2==1) {\r\n        result = (var1 << var2) | (cpu->flags & CF);\r\n    } else {\r\n        result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (33-var2));\r\n    }\r\n    setCF(cpu, ((var1 >> (32-var2)) & 1));\r\n    setOF(cpu, (cpu->flags & CF) ^ (result >> 31));";
    static public String rcr8 = "fillFlagsNoOF(cpu);\r\n    result = (var1 >> var2) | ((cpu->flags & CF) << (8-var2)) | (var1 << (9-var2));\r\n    setCF(cpu, (var1 >> (var2 - 1)) & 1);\r\n    setOF(cpu, (result ^ (result<<1)) & 0x80);";
    static public String rcr16 = "fillFlagsNoOF(cpu);\r\n    result = (var1 >> var2) | ((cpu->flags & CF) << (16-var2)) | (var1 << (17-var2));\r\n    setCF(cpu, (var1 >> (var2 - 1)) & 1);\r\n    setOF(cpu, (result ^ (result<<1)) & 0x8000);";
    static public String rcr32 = "fillFlagsNoOF(cpu);\r\n    if (var2==1) {\r\n        result = (var1 >> var2) | ((cpu->flags & CF) << 31);\r\n    } else {\r\n        result = (var1 >> var2) | ((cpu->flags & CF) << (32-var2)) | (var1 << (33-var2));\r\n    }\r\n    setCF(cpu, (var1 >> (var2 - 1)) & 1);\r\n    setOF(cpu, (result ^ (result<<1)) & 0x80000000);";
    static public String shl8 = "result = var1 << var2;\r\n    cpu->lazyFlags = FLAGS_SHL8;\r\n    cpu->result.u8 = result;\r\n    cpu->src.u8=var2;\r\n    cpu->dst.u8 = var1;";
    static public String shl16 = "result = var1 << var2;\r\n    cpu->lazyFlags = FLAGS_SHL16;\r\n    cpu->result.u16 = result;\r\n    cpu->src.u16=var2;\r\n    cpu->dst.u16 = var1;";
    static public String shl32 = "result = var1 << var2;\r\n    cpu->lazyFlags = FLAGS_SHL32;\r\n    cpu->result.u32 = result;\r\n    cpu->src.u32=var2;\r\n    cpu->dst.u32 = var1;";
    static public String shr8 = "result = var1 >> var2;\r\n    cpu->lazyFlags = FLAGS_SHR8;\r\n    cpu->result.u8 = result;\r\n    cpu->src.u8=var2;\r\n    cpu->dst.u8 = var1;";
    static public String shr16 = "result = var1 >> var2;\r\n    cpu->lazyFlags = FLAGS_SHR16;\r\n    cpu->result.u16 = result;\r\n    cpu->src.u16=var2;\r\n    cpu->dst.u16 = var1;";
    static public String shr32 = "result = var1 >> var2;\r\n    cpu->lazyFlags = FLAGS_SHR32;\r\n    cpu->result.u32 = result;\r\n    cpu->src.u32=var2;\r\n    cpu->dst.u32 = var1;";
    static public String sar8 = "result = (S8)var1 >> var2;\r\n    cpu->lazyFlags = FLAGS_SAR8;\r\n    cpu->result.u8 = result;\r\n    cpu->src.u8=var2;\r\n    cpu->dst.u8 = var1;";
    static public String sar16 = "result = (S16)var1 >> var2;\r\n    cpu->lazyFlags = FLAGS_SAR16;\r\n    cpu->result.u16 = result;\r\n    cpu->src.u16=var2;\r\n    cpu->dst.u16 = var1;";
    static public String sar32 = "result = (S32)var1 >> var2;\r\n    cpu->lazyFlags = FLAGS_SAR32;\r\n    cpu->result.u32 = result;\r\n    cpu->src.u32=var2;\r\n    cpu->dst.u32 = var1;";

    static public String rol8_noflags = "result = (var1 << var2) | (var1 >> (8 - var2));";
    static public String rol16_noflags = "result = (var1 << var2) | (var1 >> (16 - var2));";
    static public String rol32_noflags = "result = (var1 << var2) | (var1 >> (32 - var2));";
    static public String ror8_noflags = "result = (var1 >> var2) | (var1 << (8 - var2));";
    static public String ror16_noflags = "result = (var1 >> var2) | (var1 << (16 - var2));";
    static public String ror32_noflags = "result = (var1 >> var2) | (var1 << (32 - var2));";
    static public String rcl8_noflags = "result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (9-var2));";
    static public String rcl16_noflags = "result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (17-var2));";
    static public String rcl32_noflags = "if (var2==1) {\r\n        result = (var1 << var2) | (cpu->flags & CF);\r\n    } else {\r\n        result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (33-var2));\r\n    }";
    static public String rcr8_noflags = "result = (var1 >> var2) | ((cpu->flags & CF) << (8-var2)) | (var1 << (9-var2));";
    static public String rcr16_noflags = "result = (var1 >> var2) | ((cpu->flags & CF) << (16-var2)) | (var1 << (17-var2));";
    static public String rcr32_noflags = "if (var2==1) {\r\n        result = (var1 >> var2) | ((cpu->flags & CF) << 31);\r\n    } else {\r\n        result = (var1 >> var2) | ((cpu->flags & CF) << (32-var2)) | (var1 << (33-var2));\r\n    }";
    static public String shl8_noflags = "result = var1 << var2;";
    static public String shl16_noflags = "result = var1 << var2;";
    static public String shl32_noflags = "result = var1 << var2;";
    static public String shr8_noflags = "result = var1 >> var2;";
    static public String shr16_noflags = "result = var1 >> var2;";
    static public String shr32_noflags = "result = var1 >> var2;";
    static public String sar8_noflags = "result = (S8)var1 >> var2;";
    static public String sar16_noflags = "result = (S16)var1 >> var2;";
    static public String sar32_noflags = "result = (S32)var1 >> var2;";

    public void shiftInst8(FileOutputStream fos, String name, String source, String rcycles, String mcycles, String inst, int mask, int mod, boolean checkForZero) throws IOException {
        shiftBase(fos, name+"_reg", false, false, "8", source, "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", rcycles, inst, mask, mod, checkForZero);
        shiftBase(fos, name+"_mem16", true, false, "8", source, "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
        shiftBase(fos, name+"_mem32", false, true, "8", source, "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
    }

    public void shiftInst16(FileOutputStream fos, String name, String source, String rcycles, String mcycles, String inst, int mask, int mod, boolean checkForZero) throws IOException {
        shiftBase(fos, name+"_reg", false, false, "16", source, "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", rcycles, inst, mask, mod, checkForZero);
        shiftBase(fos, name+"_mem16", true, false, "16", source, "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
        shiftBase(fos, name+"_mem32", false, true, "16", source, "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
    }

    public void shiftInst32(FileOutputStream fos, String name, String source, String rcycles, String mcycles, String inst, int mask, int mod, boolean checkForZero) throws IOException {
        shiftBase(fos, name+"_reg", false, false, "32", source, "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", rcycles, inst, mask, mod, checkForZero);
        shiftBase(fos, name+"_mem16", true, false, "32", source, "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
        shiftBase(fos, name+"_mem32", false, true, "32", source, "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
    }

    public void shiftBase(FileOutputStream fos, String name, boolean eaa16, boolean eaa32, String bits, String shiftSource, String source, String destSave1, String destSave2, String cycles, String inst, int mask, int mod, boolean checkForZero) throws IOException {
        if (!flags)
            name+="_noflags";
        out(fos, "void OPCALL "+name+"(struct CPU* cpu, struct Op* op) {");
        out(fos, "    U"+bits+" result;");
        if (eaa16)
            out(fos, "    U32 eaa = eaa16(cpu, op);");
        if (eaa32)
            out(fos, "    U32 eaa = eaa32(cpu, op);");
        out(fos, "    U"+bits+" var1"+((mask==0 && mod==0)?"="+source:"")+";");
        out(fos, "    U"+bits+" var2 = "+shiftSource+";");
        if (checkForZero) {
            out(fos, "    if (var2) {");
        }
        if (mask!=0 || mod!=0) {

            if (mask!=0)
                out(fos, "    var2&="+mask+";");
            if (mod!=0)
                out(fos, "    var2=var2 % "+mod+";");
            out(fos, "    var1 = "+source+";");
        }
        out(fos, "    "+inst);
        out(fos, "    "+destSave1+"result"+destSave2+";");
        if (checkForZero) {
            out(fos, "    }");
        }
        out(fos, "    CYCLES("+cycles+");");
        out(fos, "    NEXT();");
        out(fos, "}");
    }
}

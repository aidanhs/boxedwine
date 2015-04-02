package x86;

import java.io.FileOutputStream;
import java.io.IOException;

public class Main {
    static FileOutputStream fos;
    static public void main(String[] args) {
        try {
            fos = new FileOutputStream("arith.h");
            arith("add", "+", false, true, "1", "3", "2", "1", "3");
            arith("or", "|", false, true, "1", "3", "2", "1", "3");
            arith("adc", "+", true, true, "1", "3", "2", "1", "3");
            arith("sbb", "-", true, true, "1", "3", "2", "1", "3");
            arith("and", "&", false, true, "1", "3", "2", "1", "3");
            arith("sub", "-", false, true, "1", "3", "2", "1", "3");
            arith("xor", "^", false, true, "1", "3", "2", "1", "3");
            arith("cmp", "-", false, false, "1", "2", "2", "1", "2");
            arith("test", "&", false, false, "1", "2", "2", "1", "2");
            fos.close();
            fos = new FileOutputStream("incdec.h");
            incDec("inc", "+");
            incDec("dec", "-");
            fos.close();
            fos = new FileOutputStream("pushpop.h");
            pushPop();
            fos.close();
            fos = new FileOutputStream("strings.h");
            strings();
            fos.close();
            fos = new FileOutputStream("shift.h");
            shift();
            fos.close();
            fos = new FileOutputStream("conditions.h");
            conditions();
            fos.close();
            fos = new FileOutputStream("setcc.h");
            setCC();
            fos.close();
            fos = new FileOutputStream("decode.h");
            decode();
            fos.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    static public void eg(String inst, String name, int bits) throws IOException {
        eg(inst, name, bits, null);
    }
    static public void eg(String inst, String name, int bits, String extra) throws IOException {
        if (bits==8)
            out("// "+name.toUpperCase()+" Eb,Gb");
        else if (bits==16)
            out("// "+name.toUpperCase()+" Ew,Gw");
        else
            out("// "+name.toUpperCase()+" Ed,Gd");
        out("BOOL decode"+inst+"(struct DecodeData* data) {");
        out("    U8 rm = FETCH8(data);");
        out("    if (rm>=0xC0) {");
        out("        data->op->func = "+name+"r"+bits+"r"+bits+";");
        out("        data->op->r1 = E(rm);");
        out("        data->op->r2 = G(rm);");
        out("        LOG_OP2(\""+name.toUpperCase()+"\", R"+bits+"(data->op->r1),R"+bits+"(data->op->r2));");
        out("    } else if (data->ea16) {");
        out("        data->op->func = "+name+"e"+bits+"r"+bits+"_16;");
        out("        data->op->r1 = G(rm);");
        out("        decodeEa16(data, rm);");
        out("        LOG_OP2(\""+name.toUpperCase()+"\", M"+bits+"(data, rm, data->op),R"+bits+"(data->op->r1));");
        out("    } else {");
        out("        data->op->func = "+name+"e"+bits+"r"+bits+"_32;");
        out("        data->op->r1 = G(rm);");
        out("        decodeEa32(data, rm);");
        out("        LOG_OP2(\""+name.toUpperCase()+"\", M"+bits+"(data, rm, data->op),R"+bits+"(data->op->r1));");
        out("    }");
        if (extra!=null)
            out(extra);
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }
    static public void ge(String inst, String name, int bits) throws IOException {
        ge(inst, name, bits, 0, null);
    }

    static public void ge(String inst, String name, int bits, int data, String comment) throws IOException {
        if (comment!=null) {
            out("// "+comment);
        } else if (bits==8)
            out("// "+name.toUpperCase()+" Gb,Eb");
        else if (bits==16)
            out("// "+name.toUpperCase()+" Gw,Ew");
        else
            out("// "+name.toUpperCase()+" Gd,Ed");
        out("BOOL decode"+inst+"(struct DecodeData* data) {");
        out("    U8 rm = FETCH8(data);");
        out("    if (rm>=0xC0) {");
        out("        data->op->func = "+name+"r"+bits+"r"+bits+";");
        out("        data->op->r1 = G(rm);");
        out("        data->op->r2 = E(rm);");
        out("        LOG_OP2(\""+name.toUpperCase()+"\", R"+bits+"(data->op->r1),R"+bits+"(data->op->r2));");
        out("    } else if (data->ea16) {");
        out("        data->op->func = "+name+"r"+bits+"e"+bits+"_16;");
        out("        data->op->r1 = G(rm);");
        out("        decodeEa16(data, rm);");
        out("        LOG_OP2(\""+name.toUpperCase()+"\", R"+bits+"(data->op->r1),M"+bits+"(data, rm, data->op));");
        out("    } else {");
        out("        data->op->func = "+name+"r"+bits+"e"+bits+"_32;");
        out("        data->op->r1 = G(rm);");
        out("        decodeEa32(data, rm);");
        out("        LOG_OP2(\""+name.toUpperCase()+"\", R"+bits+"(data->op->r1),M"+bits+"(data, rm, data->op));");
        out("    }");
        decodeData(data);
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }

    static public void ad(String inst, String name, int bits) throws IOException {
        if (bits==8)
            out("// "+name.toUpperCase()+" Al,Ib");
        else if (bits==16)
            out("// "+name.toUpperCase()+" Ax,Iw");
        else
            out("// "+name.toUpperCase()+" Eax,Id");
        out("BOOL decode"+inst+"(struct DecodeData* data) {");
        out("    data->op->func = "+name+bits+"_reg;");
        out("    data->op->r1 = 0;");
        out("    data->op->data1 = FETCH"+bits+"(data);");
        out("    LOG_OP2(\""+name.toUpperCase()+"\", R"+bits+"(0), itoa(data->op->data1, tmp, 16));");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }
    static public void decodeArith(int base, String name) throws IOException {
        String h = String.valueOf(base/16);
        int o = base % 16;
        eg("0"+h+Integer.toHexString(o), name, 8);
        eg("0"+h+Integer.toHexString(o+1), name, 16);
        eg("2"+h+Integer.toHexString(o+1), name, 32);

        ge("0"+h+Integer.toHexString(o+2), name, 8);
        ge("0"+h+Integer.toHexString(o+3), name, 16);
        ge("2"+h+Integer.toHexString(o+3), name, 32);

        ad("0"+h+Integer.toHexString(o+4), name, 8);
        ad("0"+h+Integer.toHexString(o+5), name, 16);
        ad("2"+h+Integer.toHexString(o+5), name, 32);
    }

    static public void decodePushSeg(String base, String seg) throws IOException {
        out("// PUSH "+seg.toUpperCase());
        out("BOOL decode0"+base+"(struct DecodeData* data) {");
        out("    data->op->func = pushSeg16;");
        out("    data->op->r1 = "+seg.toUpperCase()+";");
        out("    LOG_OP1(\"PUSH\", EABASE(data->op->r1));");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
        out("// PUSH "+seg.toUpperCase());
        out("BOOL decode2"+base+"(struct DecodeData* data) {");
        out("    data->op->func = pushSeg32;");
        out("    data->op->r1 = "+seg.toUpperCase()+";");
        out("    LOG_OP1(\"PUSH\", EABASE(data->op->r1));");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }

    static public void decodePopSeg(String base, String seg) throws IOException {
        out("// POP "+seg.toUpperCase());
        out("BOOL decode0"+base+"(struct DecodeData* data) {");
        out("    data->op->func = popSeg16;");
        out("    data->op->r1 = "+seg.toUpperCase()+";");
        out("    LOG_OP1(\"POP\", EABASE(data->op->r1));");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
        out("// POP "+seg.toUpperCase());
        out("BOOL decode2"+base+"(struct DecodeData* data) {");
        out("    data->op->func = popSeg32;");
        out("    data->op->r1 = "+seg.toUpperCase()+";");
        out("    LOG_OP1(\"POP\", EABASE(data->op->r1));");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }

    static public void decodeSeg(String base, String seg) throws IOException {
        out("// SEG "+seg.toUpperCase());
        out("BOOL decode0"+base+"(struct DecodeData* data) {");
        out("    data->ds = "+seg.toUpperCase()+";");
        out("    data->ss = "+seg.toUpperCase()+";");
        out("    return TRUE;");
        out("}");
    }

    static public void decodeFunc(String base, String func, String asm) throws IOException {
        out("// "+func.toUpperCase());
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    data->op->func = "+func+";");
        out("    LOG_OP(\""+asm+"\");");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }

    static public void decodeFuncEndOfBlock(String base, String func, String asm) throws IOException {
        out("// "+func.toUpperCase());
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    data->op->func = "+func+";");
        out("    LOG_OP(\""+asm+"\");");
        out("    FINISH_OP(data);");
        out("    return FALSE;");
        out("}");
    }

    static public void decodeE(String asm, String comment, String base, String func, int data, int bits) throws IOException {
        out("// "+comment.toUpperCase());
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    U8 rm = FETCH8(data);");
        out("    DECODE_E("+func+"_reg, "+func+"_mem16, "+func+"_mem32);");
        decodeData(data);
        if (data!=0)
        out("    LOG_E"+bits+"C(\""+asm+"\", rm, data);");
        else
        out("    LOG_E"+bits+"(\""+asm+"\", rm, data);");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }

    static public void decodeFuncEAdata(String comment, String base, String func, String log) throws IOException {
        out("// "+comment);
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    data->op->func = "+func+";");
        out("    if (data->ea16) {");
        out("        data->op->data1 = FETCH16(data);");
        out("    } else {");
        out("        data->op->data1 = FETCH32(data);");
        out("    }");
        out("    data->op->base = data->ds;");
        out("    "+log);
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }

    static public void decodeString(String base, String name) throws IOException {
        out("// "+name.toUpperCase());
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    if (data->ea16) {");
        out("        if (data->rep) {");
        out("            data->op->func = "+name+"16_r;");
        out("        } else {");
        out("            data->op->func = "+name+"16;");
        out("        }");
        out("    } else {");
        out("        if (data->rep) {");
        out("            data->op->func = "+name+"32_r;");
        out("        } else {");
        out("            data->op->func = "+name+"32;");
        out("        }");
        out("    }");
        out("    data->op->data1 = data->rep_zero;");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }
    static public void decodeFunc(String comment, String base, String func, int reg) throws IOException {
        decodeFunc(comment, base, func, String.valueOf(reg));
    }
    static public void decodeFunc(String comment, String base, String func, String reg) throws IOException {
        out("// "+comment);
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    data->op->func = "+func+";");
        out("    data->op->r1 = "+reg+";");
        out("    LOG_OP(\""+comment+"\");");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }
    static public void decodeFuncRegData(String asm, String comment, String base, String func, int reg, int bits) throws IOException {
        out("// "+comment);
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    data->op->func = "+func+";");
        out("    data->op->r1 = "+reg+";");
        decodeData(bits);
        out("    LOG_OP2(\""+asm+"\", R"+bits+"(data->op->r1), itoa(data->op->data1, tmp, 16));");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }
    static public void decodeFunc(String comment, String base, String func, int reg1, int reg2) throws IOException {
        out("// "+comment);
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    data->op->func = "+func+";");
        out("    data->op->r1 = "+reg1+";");
        out("    data->op->r2 = "+reg2+";");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }

    static public void decodeData(int bits) throws IOException {
        if (bits==8) {
            out("    data->op->data1 = FETCH8(data);");
        } else if (bits==16) {
            out("    data->op->data1 = FETCH16(data);");
        } else if (bits==32) {
            out("    data->op->data1 = FETCH32(data);");
        } else if (bits == -8) {
            out("    data->op->data1 = FETCH_S8(data);");
        } else if (bits == -16) {
            out("    data->op->data1 = FETCH_S16(data);");
        }
    }
    static public void decodeFuncData(String asm, String comment, String base, String func, int bits) throws IOException {
        out("// "+comment);
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    data->op->func = "+func+";");
        decodeData(bits);
        out("    LOG_OP1(\""+asm+"\", itoa(data->op->data1, tmp, 16));");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }

    static public void decodeFuncDataEndOfBlock(String asm, String comment, String base, String func, int bits) throws IOException {
        out("// "+comment);
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    data->op->func = "+func+";");
        decodeData(bits);
        out("    LOG_OP1(\""+asm+"\", itoa(data->op->data1, tmp, 16));");
        out("    FINISH_OP(data);");
        out("    return FALSE;");
        out("}");
    }

    static public void decodeJump(String base, String c, int bits) throws IOException {
        out("// J"+c);
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    data->op->func = jump"+c+";");
        decodeData(bits);
        out("    LOG_OP1(\"J"+c+"\", itoa((int)data->op->data1, tmp, 10));");
        out("    FINISH_OP(data);");
        out("    return FALSE;");
        out("}");
    }

    static public void decodeGroup3(String comment, String base, int bits) throws IOException {
        out("// "+comment);
        out("BOOL decode"+base+"(struct DecodeData* data) {");
        out("    U8 rm = FETCH8(data);");
        out("    switch (G(rm)) {");
        out("    case 0x00:");
        out("    case 0x01:");
        out("        DECODE_E(test"+bits+"_reg, test"+bits+"_mem16, test"+bits+"_mem32);");
        out("        data->op->data1 = FETCH"+bits+"(data);");
        out("        LOG_E"+bits+"C(\"TEST\", rm, data);");
        out("        break;");
        out("    case 0x02:");
        out("        DECODE_E(not"+bits+"_reg, not"+bits+"_mem16, not"+bits+"_mem32);");
        out("        LOG_E"+bits+"(\"NOT\", rm, data);");
        out("        break;");
        out("    case 0x03:");
        out("        DECODE_E(neg"+bits+"_reg, neg"+bits+"_mem16, neg"+bits+"_mem32);");
        out("        LOG_E"+bits+"(\"NEG\", rm, data);");
        out("        break;");
        out("    case 0x04:");
        out("        DECODE_E(mul"+bits+"_reg, mul"+bits+"_mem16, mul"+bits+"_mem32);");
        out("        LOG_E"+bits+"(\"MUL\", rm, data);");
        out("        break;");
        out("    case 0x05:");
        out("        DECODE_E(imul"+bits+"_reg, imul"+bits+"_mem16, imul"+bits+"_mem32);");
        out("        LOG_E"+bits+"(\"IMUL\", rm, data);");
        out("        break;");
        out("    case 0x06:");
        out("        DECODE_E(div"+bits+"_reg, div"+bits+"_mem16, div"+bits+"_mem32);");
        out("        LOG_E"+bits+"(\"DIV\", rm, data);");
        out("        break;");
        out("    case 0x07:");
        out("        DECODE_E(idiv"+bits+"_reg, idiv"+bits+"_mem16, idiv"+bits+"_mem32);");
        out("        LOG_E"+bits+"(\"IDIV\", rm, data);");
        out("        break;");
        out("    }");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }

    static public void decodeCmov(String inst, String c, int bits) throws IOException {
        out("// CMOV"+c);
        out("BOOL decode"+inst+"(struct DecodeData* data) {");
        out("    U8 rm = FETCH8(data);");
        out("    if (rm>=0xC0) {");
        out("        data->op->func = cmov"+c+"_"+bits+"_reg;");
        out("        data->op->r1 = G(rm);");
        out("        data->op->r2 = E(rm);");
        out("        LOG_OP2(\"CMOV\", R"+bits+"(data->op->r1), R"+bits+"(data->op->r2));");
        out("    } else if (data->ea16) {");
        out("        data->op->func = cmov"+c+"_"+bits+"_mem16;");
        out("        data->op->r1 = G(rm);");
        out("        decodeEa16(data, rm);");
        out("        LOG_OP2(\"CMOV\", R"+bits+"(data->op->r1), M"+bits+"(data, rm, data->op));");
        out("    } else {");
        out("        data->op->func = cmov"+c+"_"+bits+"_mem32;");
        out("        data->op->r1 = G(rm);");
        out("        decodeEa32(data, rm);");
        out("        LOG_OP2(\"CMOV\", R"+bits+"(data->op->r1), M"+bits+"(data, rm, data->op));");
        out("    }");
        out("    NEXT_OP(data);");
        out("    return TRUE;");
        out("}");
    }

    static public void decode() throws IOException {
        decodeArith(0, "add");
        decodePushSeg("06", "es");
        decodePopSeg("07", "es");
        decodeArith(8, "or");
        decodePushSeg("0e", "cs");
        decodeArith(0x10, "adc");
        decodePushSeg("16", "ss");
        decodePopSeg("17", "ss");
        decodeArith(0x18, "sbb");
        decodePushSeg("1e", "ds");
        decodePopSeg("1f", "ds");
        decodeArith(0x20, "and");
        decodeSeg("26", "es");
        decodeFunc("027", "daa", "DAA");
        decodeArith(0x28, "sub");
        decodeSeg("2e", "cs");
        decodeFunc("02f", "das", "DAS");
        decodeArith(0x30, "xor");
        decodeSeg("36", "ss");
        decodeFunc("037", "aaa", "AAA");
        decodeArith(0x38, "cmp");
        decodeSeg("3e", "ds");
        decodeFunc("03f", "aas", "AAS");
        decodeFunc("INC AX", "040", "inc16_reg", 0);
        decodeFunc("INC EAX", "240", "inc32_reg", 0);
        decodeFunc("INC CX", "041", "inc16_reg", 1);
        decodeFunc("INC ECX", "241", "inc32_reg", 1);
        decodeFunc("INC DX", "042", "inc16_reg", 2);
        decodeFunc("INC EDX", "242", "inc32_reg", 2);
        decodeFunc("INC BX", "043", "inc16_reg", 3);
        decodeFunc("INC EBX", "243", "inc32_reg", 3);
        decodeFunc("INC SP", "044", "inc16_reg", 4);
        decodeFunc("INC ESP", "244", "inc32_reg", 4);
        decodeFunc("INC BP", "045", "inc16_reg", 5);
        decodeFunc("INC EBP", "245", "inc32_reg", 5);
        decodeFunc("INC SI", "046", "inc16_reg", 6);
        decodeFunc("INC ESI", "246", "inc32_reg", 6);
        decodeFunc("INC DI", "047", "inc16_reg", 7);
        decodeFunc("INC EDI", "247", "inc32_reg", 7);
        decodeFunc("DEC AX", "048", "dec16_reg", 0);
        decodeFunc("DEC EAX", "248", "dec32_reg", 0);
        decodeFunc("DEC CX", "049", "dec16_reg", 1);
        decodeFunc("DEC ECX", "249", "dec32_reg", 1);
        decodeFunc("DEC DX", "04a", "dec16_reg", 2);
        decodeFunc("DEC EDX", "24a", "dec32_reg", 2);
        decodeFunc("DEC BX", "04b", "dec16_reg", 3);
        decodeFunc("DEC EBX", "24b", "dec32_reg", 3);
        decodeFunc("DEC SP", "04c", "dec16_reg", 4);
        decodeFunc("DEC ESP", "24c", "dec32_reg", 4);
        decodeFunc("DEC BP", "04d", "dec16_reg", 5);
        decodeFunc("DEC EBP", "24d", "dec32_reg", 5);
        decodeFunc("DEC SI", "04e", "dec16_reg", 6);
        decodeFunc("DEC ESI", "24e", "dec32_reg", 6);
        decodeFunc("DEC DI", "04f", "dec16_reg", 7);
        decodeFunc("DEC EDI", "24f", "dec32_reg", 7);

        decodeFunc("PUSH AX", "050", "pushAx", 0);
        decodeFunc("PUSH EAX", "250", "pushEax", 0);
        decodeFunc("PUSH CX", "051", "pushCx", 1);
        decodeFunc("PUSH ECX", "251", "pushEcx", 1);
        decodeFunc("PUSH DX", "052", "pushDx", 2);
        decodeFunc("PUSH EDX", "252", "pushEdx", 2);
        decodeFunc("PUSH BX", "053", "pushBx", 3);
        decodeFunc("PUSH EBX", "253", "pushEbx", 3);
        decodeFunc("PUSH SP", "054", "pushSp", 4);
        decodeFunc("PUSH ESP", "254", "pushEsp", 4);
        decodeFunc("PUSH BP", "055", "pushBp", 5);
        decodeFunc("PUSH EBP", "255", "pushEbp", 5);
        decodeFunc("PUSH SI", "056", "pushSi", 6);
        decodeFunc("PUSH ESI", "256", "pushEsi", 6);
        decodeFunc("PUSH DI", "057", "pushDi", 7);
        decodeFunc("PUSH EDI", "257", "pushEdi", 7);
        decodeFunc("PUSH AX", "058", "popAx", 0);
        decodeFunc("POP EAX", "258", "popEax", 0);
        decodeFunc("POP CX", "059", "popCx", 1);
        decodeFunc("POP ECX", "259", "popEcx", 1);
        decodeFunc("POP DX", "05a", "popDx", 2);
        decodeFunc("POP EDX", "25a", "popEdx", 2);
        decodeFunc("POP BX", "05b", "popBx", 3);
        decodeFunc("POP EBX", "25b", "popEbx", 3);
        decodeFunc("POP SP", "05c", "popSp", 4);
        decodeFunc("POP ESP", "25c", "popEsp", 4);
        decodeFunc("POP BP", "05d", "popBp", 5);
        decodeFunc("POP EBP", "25d", "popEbp", 5);
        decodeFunc("POP SI", "05e", "popSi", 6);
        decodeFunc("POP ESI", "25e", "popEsi", 6);
        decodeFunc("POP DI", "05f", "popDi", 7);
        decodeFunc("POP EDI", "25f", "popEdi", 7);
        decodeFunc("060", "pusha", "PUSHA");
        decodeFunc("260", "pushad", "PUSHA");
        decodeFunc("061", "popa", "POPA");
        decodeFunc("261", "popad", "POPA");
        decodeSeg("64", "fs");
        decodeSeg("65", "gs");
        decodeFuncData("PUSH", "PUSH Iw", "068", "push16data", 16);
        decodeFuncData("PUSH", "PUSH Id", "268", "push32data", 32);
        ge("069", "dimulc", 16, -16, "IMUL Gw,Ew,Iw");
        ge("269", "dimulc", 32, 32, "IMUL Gd,Ed,Id");
        decodeFuncData("PUSH", "PUSH Ib", "06a", "push16data", -8);
        decodeFuncData("PUSH", "PUSH Ib", "26a", "push32data", -8);
        ge("06b", "dimulc", 16, -8, "IMUL Gw,Ew,Ib");
        ge("26b", "dimulc", 32, -8, "IMUL Gd,Ed,Ib");
        decodeJump("070", "O", -8);
        decodeJump("071", "NO", -8);
        decodeJump("072", "B", -8);
        decodeJump("073", "NB", -8);
        decodeJump("074", "Z", -8);
        decodeJump("075", "NZ", -8);
        decodeJump("076", "BE", -8);
        decodeJump("077", "NBE", -8);
        decodeJump("078", "S", -8);
        decodeJump("079", "NS", -8);
        decodeJump("07a", "P", -8);
        decodeJump("07b", "NP", -8);
        decodeJump("07c", "L", -8);
        decodeJump("07d", "NL", -8);
        decodeJump("07e", "LE", -8);
        decodeJump("07f", "NLE", -8);
        eg("084", "test", 8);
        eg("085", "test", 16);
        eg("285", "test", 32);
        eg("086", "xchg", 8);
        eg("087", "xchg", 16);
        eg("287", "xchg", 32);
        eg("088", "mov", 8);
        eg("089", "mov", 16);
        eg("289", "mov", 32);
        ge("08a", "mov", 8);
        ge("08b", "mov", 16);
        ge("28b", "mov", 32);

        decodeFunc("XCHG CX,AX", "091", "xchgr16r16", 0, 1);
        decodeFunc("XCHG ECX,EAX", "291", "xchgr32r32", 0, 1);
        decodeFunc("XCHG DX,AX", "092", "xchgr16r16", 0, 2);
        decodeFunc("XCHG EDX,EAX", "292", "xchgr32r32", 0, 2);
        decodeFunc("XCHG BX,AX", "093", "xchgr16r16", 0, 3);
        decodeFunc("XCHG EBX,EAX", "293", "xchgr32r32", 0, 3);
        decodeFunc("XCHG SP,AX", "094", "xchgr16r16", 0, 4);
        decodeFunc("XCHG ESP,EAX", "294", "xchgr32r32", 0, 4);
        decodeFunc("XCHG BP,AX", "095", "xchgr16r16", 0, 5);
        decodeFunc("XCHG EBP,EAX", "295", "xchgr32r32", 0, 5);
        decodeFunc("XCHG SI,AX", "096", "xchgr16r16", 0, 6);
        decodeFunc("XCHG ESI,EAX", "296", "xchgr32r32", 0, 6);
        decodeFunc("XCHG DI,AX", "097", "xchgr16r16", 0, 7);
        decodeFunc("XCHG EDI,EAX", "297", "xchgr32r32", 0, 7);
        decodeFunc("098", "cbw", "CBW");
        decodeFunc("298", "cbwe", "CBWE");
        decodeFunc("099", "cwd", "CWD");
        decodeFunc("299", "cwq", "CWQ");
        decodeFunc("09c", "pushf16", "PUSHF");
        decodeFunc("29c", "pushf32", "PUSHF");
        decodeFunc("09d", "popf16", "POPF");
        decodeFunc("29d", "popf32", "POPF");
        decodeFunc("09e", "sahf", "SAHF");
        decodeFunc("09f", "lahf", "LAHF");
        decodeFuncEAdata("MOV AL,Ob", "0a0", "movAl", "LOG_OP2(\"MOV\", \"AL\", O8(data->op));");
        decodeFuncEAdata("MOV AX,Ow", "0a1", "movAx", "LOG_OP2(\"MOV\", \"AX\", O16(data->op));");
        decodeFuncEAdata("MOV EAX,Od", "2a1", "movEax", "LOG_OP2(\"MOV\", \"EAX\", O32(data->op));");
        decodeFuncEAdata("MOV Ob,Al", "0a2", "movDirectAl", "LOG_OP2(\"MOV\", O8(data->op), \"AL\");");
        decodeFuncEAdata("MOV Ow,Ax", "0a3", "movDirectAx", "LOG_OP2(\"MOV\", O16(data->op), \"AX\");");
        decodeFuncEAdata("MOV Od,Eax", "2a3", "movDirectEax", "LOG_OP2(\"MOV\", O32(data->op), \"EAX\");");

        decodeString("0a4", "movsb");
        decodeString("0a5", "movsw");
        decodeString("2a5", "movsd");
        decodeString("0a6", "cmpsb");
        decodeString("0a7", "cmpsw");
        decodeString("2a7", "cmpsd");
        decodeFuncRegData("TEST", "TEST AL,Ib", "0a8", "test8_reg", 0, 8);
        decodeFuncRegData("TEST", "TEST AX,Iw", "0a9", "test16_reg", 0, 16);
        decodeFuncRegData("TEST", "TEST EAX,Id", "2a9", "test32_reg", 0, 32);
        decodeString("0aa", "stosb");
        decodeString("0ab", "stosw");
        decodeString("2ab", "stosd");
        decodeString("0ac", "lodsb");
        decodeString("0ad", "lodsw");
        decodeString("2ad", "lodsd");
        decodeString("0ae", "scasb");
        decodeString("0af", "scasw");
        decodeString("2af", "scasd");
        decodeFuncRegData("MOV", "MOV AL,Ib", "0b0", "mov8_reg", 0, 8);
        decodeFuncRegData("MOV", "MOV CL,Ib", "0b1", "mov8_reg", 1, 8);
        decodeFuncRegData("MOV", "MOV DL,Ib", "0b2", "mov8_reg", 2, 8);
        decodeFuncRegData("MOV", "MOV BL,Ib", "0b3", "mov8_reg", 3, 8);
        decodeFuncRegData("MOV", "MOV AH,Ib", "0b4", "mov8_reg", 4, 8);
        decodeFuncRegData("MOV", "MOV CH,Ib", "0b5", "mov8_reg", 5, 8);
        decodeFuncRegData("MOV", "MOV DH,Ib", "0b6", "mov8_reg", 6, 8);
        decodeFuncRegData("MOV", "MOV BH,Ib", "0b7", "mov8_reg", 7, 8);
        decodeFuncRegData("MOV", "MOV AX,Iw", "0b8", "mov16_reg", 0, 16);
        decodeFuncRegData("MOV", "MOV EAX,Id", "2b8", "mov32_reg", 0, 32);
        decodeFuncRegData("MOV", "MOV CX,Iw", "0b9", "mov16_reg", 1, 16);
        decodeFuncRegData("MOV", "MOV ECX,Id", "2b9", "mov32_reg", 1, 32);
        decodeFuncRegData("MOV", "MOV DX,Iw", "0ba", "mov16_reg", 2, 16);
        decodeFuncRegData("MOV", "MOV EDX,Id", "2ba", "mov32_reg", 2, 32);
        decodeFuncRegData("MOV", "MOV BX,Iw", "0bb", "mov16_reg", 3, 16);
        decodeFuncRegData("MOV", "MOV EBX,Id", "2bb", "mov32_reg", 3, 32);
        decodeFuncRegData("MOV", "MOV SP,Iw", "0bc", "mov16_reg", 4, 16);
        decodeFuncRegData("MOV", "MOV ESP,Id", "2bc", "mov32_reg", 4, 32);
        decodeFuncRegData("MOV", "MOV BP,Iw", "0bd", "mov16_reg", 5, 16);
        decodeFuncRegData("MOV", "MOV EBP,Id", "2bd", "mov32_reg", 5, 32);
        decodeFuncRegData("MOV", "MOV SI,Iw", "0be", "mov16_reg", 6, 16);
        decodeFuncRegData("MOV", "MOV ESI,Id", "2be", "mov32_reg", 6, 32);
        decodeFuncRegData("MOV", "MOV DI,Iw", "0bf", "mov16_reg", 7, 16);
        decodeFuncRegData("MOV", "MOV EDI,Id", "2bf", "mov32_reg", 7, 32);
        decodeFuncDataEndOfBlock("RETN", "RETN Iw", "0c2", "retnIw16", 16);
        decodeFuncDataEndOfBlock("RETN", "RETN Iw", "2c2", "retnIw32", 16);
        decodeFuncEndOfBlock("0c3", "retn16", "RETN");
        decodeFuncEndOfBlock("2c3", "retn32", "RETN");
        decodeE("MOV", "MOV Eb,Ib", "0c6", "mov8", 8, 8);
        decodeE("MOV", "MOV Ew,Iw", "0c7", "mov16", 16, 16);
        decodeE("MOV", "MOV Ed,Id", "2c7", "mov32", 32, 32);
        decodeFunc("0c9", "leave16", "LEAVE");
        decodeFunc("2c9", "leave32", "LEAVE");
        decodeFunc("0d6", "salc", "SALC");
        decodeFuncDataEndOfBlock("CALL", "CALL Jw ", "0e8", "callJw", -16);
        decodeFuncDataEndOfBlock("CALL", "CALL Jd ", "2e8", "callJd", 32);
        decodeFuncDataEndOfBlock("JMP", "JMP Jw ", "0e9", "jump", -16);
        decodeFuncDataEndOfBlock("JMP", "JMP Jd ", "2e9", "jump", 32);
        decodeFuncDataEndOfBlock("JMP", "JMP Jb ", "0eb", "jump", -8);
        decodeFunc("0f5", "cmc", "CMC");
        decodeGroup3("GRP3 Eb(,Ib)", "0f6", 8);
        decodeGroup3("GRP3 Ew(,Iw)", "0f7", 16);
        decodeGroup3("GRP3 Ed(,Id)", "2f7", 32);
        decodeFunc("0f8", "clc", "CLC");
        decodeFunc("0f9", "stc", "STC");
        decodeFunc("0fc", "cld", "CLD");
        decodeFunc("0fd", "std", "STD");
        decodeFunc("131", "rdtsc", "RDTSC");

        decodeCmov("140", "O", 16);
        decodeCmov("340", "O", 32);
        decodeCmov("141", "NO", 16);
        decodeCmov("341", "NO", 32);
        decodeCmov("142", "B", 16);
        decodeCmov("342", "B", 32);
        decodeCmov("143", "NB", 16);
        decodeCmov("343", "NB", 32);
        decodeCmov("144", "Z", 16);
        decodeCmov("344", "Z", 32);
        decodeCmov("145", "NZ", 16);
        decodeCmov("345", "NZ", 32);
        decodeCmov("146", "BE", 16);
        decodeCmov("346", "BE", 32);
        decodeCmov("147", "NBE", 16);
        decodeCmov("347", "NBE", 32);
        decodeCmov("148", "S", 16);
        decodeCmov("348", "S", 32);
        decodeCmov("149", "NS", 16);
        decodeCmov("349", "NS", 32);
        decodeCmov("14a", "P", 16);
        decodeCmov("34a", "P", 32);
        decodeCmov("14b", "NP", 16);
        decodeCmov("34b", "NP", 32);
        decodeCmov("14c", "L", 16);
        decodeCmov("34c", "L", 32);
        decodeCmov("14d", "NL", 16);
        decodeCmov("34d", "NL", 32);
        decodeCmov("14e", "LE", 16);
        decodeCmov("34e", "LE", 32);
        decodeCmov("14f", "NLE", 16);
        decodeCmov("34f", "NLE", 32);

        decodeJump("180", "O", -16);
        decodeJump("181", "NO", -16);
        decodeJump("182", "B", -16);
        decodeJump("183", "NB", -16);
        decodeJump("184", "Z", -16);
        decodeJump("185", "NZ", -16);
        decodeJump("186", "BE", -16);
        decodeJump("187", "NBE", -16);
        decodeJump("188", "S", -16);
        decodeJump("189", "NS", -16);
        decodeJump("18a", "P", -16);
        decodeJump("18b", "NP", -16);
        decodeJump("18c", "L", -16);
        decodeJump("18d", "NL", -16);
        decodeJump("18e", "LE", -16);
        decodeJump("18f", "NLE", -16);

        decodeJump("380", "O", 32);
        decodeJump("381", "NO", 32);
        decodeJump("382", "B", 32);
        decodeJump("383", "NB", 32);
        decodeJump("384", "Z", 32);
        decodeJump("385", "NZ", 32);
        decodeJump("386", "BE", 32);
        decodeJump("387", "NBE", 32);
        decodeJump("388", "S", 32);
        decodeJump("389", "NS", 32);
        decodeJump("38a", "P", 32);
        decodeJump("38b", "NP", 32);
        decodeJump("38c", "L", 32);
        decodeJump("38d", "NL", 32);
        decodeJump("38e", "LE", 32);
        decodeJump("38f", "NLE", 32);

        decodeE("SETO", "SETO", "190", "setO", 0, 8);
        decodeE("SETNO", "SETNO", "191", "setNO", 0, 8);
        decodeE("SETB", "SETB", "192", "setB", 0, 8);
        decodeE("SETNB", "SETNB", "193", "setNB", 0, 8);
        decodeE("SETZ", "SETZ", "194", "setZ", 0, 8);
        decodeE("SETNZ", "SETNZ", "195", "setNZ", 0, 8);
        decodeE("SETBE", "SETBE", "196", "setBE", 0, 8);
        decodeE("SETNBE", "SETNBE", "197", "setNBE", 0, 8);
        decodeE("SETS", "SETS", "198", "setS", 0, 8);
        decodeE("SETNS", "SETNS", "199", "setNS", 0, 8);
        decodeE("SETP", "SETP", "19a", "setP", 0, 8);
        decodeE("SETNP", "SETNP", "19b", "setNP", 0, 8);
        decodeE("SETL", "SETL", "19c", "setL", 0, 8);
        decodeE("SETNL", "SETNL", "19d", "setNL", 0, 8);
        decodeE("SETLE", "SETLE", "19e", "setLE", 0, 8);
        decodeE("SETNLE", "SETNLE", "19f", "setNLE", 0, 8);

        decodeFunc("POP FS", "1a1", "popSeg16", "FS");
        decodeFunc("POP FS", "3a1", "popSeg32", "FS");
        decodeFunc("1a2", "cpuid", "CPUID");
        eg("1a3", "bt", 16);
        eg("3a3", "bt", 32);
        eg("1a4", "dshl", 16, "    data->op->data1 = FETCH8(data);\r\n    data->op->data1 &= 0x1f;\r\n    if (data->op->data1 == 0) {\r\n        RESTART(data);\r\n        return TRUE;\r\n    }");
        eg("3a4", "dshl", 32, "    data->op->data1 = FETCH8(data);\r\n    data->op->data1 &= 0x1f;\r\n    if (data->op->data1 == 0) {\r\n        RESTART(data);\r\n        return TRUE;\r\n    }");
        eg("1a5", "dshlcl", 16);
        eg("3a5", "dshlcl", 32);
        decodeFunc("POP GS", "1a9", "popSeg16", "GS");
        decodeFunc("POP GS", "3a9", "popSeg32", "GS");
        eg("1ab", "bts", 16);
        eg("3ab", "bts", 32);
        eg("1ac", "dshr", 16, "    data->op->data1 = FETCH8(data);\r\n    data->op->data1 &= 0x1f;\r\n    if (data->op->data1 == 0) {\r\n        RESTART(data);\r\n        return TRUE;\r\n    }");
        eg("3ac", "dshr", 32, "    data->op->data1 = FETCH8(data);\r\n    data->op->data1 &= 0x1f;\r\n    if (data->op->data1 == 0) {\r\n        RESTART(data);\r\n        return TRUE;\r\n    }");
        eg("1ad", "dshrcl", 16);
        eg("3ad", "dshrcl", 32);
        ge("1af", "dimul", 16);
        ge("3af", "dimul", 32);
        eg("1b1", "cmpxchg", 16);
        eg("3b1", "cmpxchg", 32);
        ge("1b6", "movxz8", 16);
        ge("3b6", "movxz8", 32);
        ge("3b7", "movxz16", 32);

        eg("3bb", "btc", 32);
        ge("3bc", "bsf", 32);

        ge("1bd", "bsr", 16);
        ge("3bd", "bsr", 32);

        // :TODO: op log isn't correct, the src should be 8 bits
        ge("1be", "movsx8", 16);
        ge("3be", "movsx8", 32);
        ge("3bf", "movsx16", 32);

        decodeFunc("BSWAP", "3c8", "bswap32", "0");
        decodeFunc("BSWAP", "3c9", "bswap32", "1");
        decodeFunc("BSWAP", "3ca", "bswap32", "2");
        decodeFunc("BSWAP", "3cb", "bswap32", "3");
        decodeFunc("BSWAP", "3cc", "bswap32", "4");
        decodeFunc("BSWAP", "3cd", "bswap32", "5");
        decodeFunc("BSWAP", "3ce", "bswap32", "6");
        decodeFunc("BSWAP", "3cf", "bswap32", "7");
    }

    static public void setCC() throws IOException {
        cc("setO", "getOF(cpu)");
        cc("setNO", "!getOF(cpu)");
        cc("setB", "getCF(cpu)");
        cc("setNB", "!getCF(cpu)");
        cc("setZ", "getZF(cpu)");
        cc("setNZ", "!getZF(cpu)");
        cc("setBE", "getZF(cpu) || getCF(cpu)");
        cc("setNBE", "!getZF(cpu) && !getCF(cpu)");
        cc("setS", "getSF(cpu)");
        cc("setNS", "!getSF(cpu)");
        cc("setP", "getPF(cpu)");
        cc("setNP", "!getPF(cpu)");
        cc("setL", "getSF(cpu)!=getOF(cpu)");
        cc("setNL", "getSF(cpu)==getOF(cpu)");
        cc("setLE", "getZF(cpu) || getSF(cpu)!=getOF(cpu)");
        cc("setNLE", "!getZF(cpu) && getSF(cpu)==getOF(cpu)");
    }

    static public void cc(String name, String condition) throws IOException {
        out("void "+name+"_reg(struct CPU* cpu, struct Op* op) {");
        out("    if ("+condition+") {");
        out("        *cpu->reg8[op->r1] = 1;");
        out("    } else {");
        out("        *cpu->reg8[op->r1] = 0;");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_mem16(struct CPU* cpu, struct Op* op) {");
        out("    if ("+condition+") {");
        out("        writeb(cpu->memory, eaa16(cpu, op), 1);");
        out("    } else {");
        out("        writeb(cpu->memory, eaa16(cpu, op), 0);");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_mem32(struct CPU* cpu, struct Op* op) {");
        out("    if ("+condition+") {");
        out("        writeb(cpu->memory, eaa32(cpu, op), 1);");
        out("    } else {");
        out("        writeb(cpu->memory, eaa32(cpu, op), 0);");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
    }

    static public void conditions() throws IOException {
        condition("cmovO", "getOF(cpu)");
        condition("cmovNO", "!getOF(cpu)");
        condition("cmovB", "getCF(cpu)");
        condition("cmovNB", "!getCF(cpu)");
        condition("cmovZ", "getZF(cpu)");
        condition("cmovNZ", "!getZF(cpu)");
        condition("cmovBE", "getZF(cpu) || getCF(cpu)");
        condition("cmovNBE", "!getZF(cpu) && !getCF(cpu)");
        condition("cmovS", "getSF(cpu)");
        condition("cmovNS", "!getSF(cpu)");
        condition("cmovP", "getPF(cpu)");
        condition("cmovNP", "!getPF(cpu)");
        condition("cmovL", "getSF(cpu)!=getOF(cpu)");
        condition("cmovNL", "getSF(cpu)==getOF(cpu)");
        condition("cmovLE", "getZF(cpu) || getSF(cpu)!=getOF(cpu)");
        condition("cmovNLE", "!getZF(cpu) && getSF(cpu)==getOF(cpu)");
    }

    static public void condition(String name, String condition) throws IOException {
        out("void "+name+"_16_reg(struct CPU* cpu, struct Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_16_mem16(struct CPU* cpu, struct Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_16_mem32(struct CPU* cpu, struct Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_32_reg(struct CPU* cpu, struct Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_32_mem16(struct CPU* cpu, struct Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_32_mem32(struct CPU* cpu, struct Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
    }
    static public void strings() throws IOException {
        movs("movsb32_r", "b", "ESI", "EDI", "ECX", 0, true);
        movs("movsb16_r", "b", "SI", "DI", "CX", 0, true);
        movs("movsb32", "b", "ESI", "EDI", "ECX", 0, false);
        movs("movsb16", "b", "SI", "DI", "CX", 0, false);

        movs("movsw32_r", "w", "ESI", "EDI", "ECX", 1, true);
        movs("movsw16_r", "w", "SI", "DI", "CX", 1, true);
        movs("movsw32", "w", "ESI", "EDI", "ECX", 1, false);
        movs("movsw16", "w", "SI", "DI", "CX", 1, false);

        movs("movsd32_r", "d", "ESI", "EDI", "ECX", 2, true);
        movs("movsd16_r", "d", "SI", "DI", "CX", 2, true);
        movs("movsd32", "d", "ESI", "EDI", "ECX", 2, false);
        movs("movsd16", "d", "SI", "DI", "CX", 2, false);

        cmps("cmpsb32_r", "b", "8", "ESI", "EDI", "ECX", 0, true);
        cmps("cmpsb16_r", "b", "8", "SI", "DI", "CX", 0, true);
        cmps("cmpsb32", "b", "8", "ESI", "EDI", "ECX", 0, false);
        cmps("cmpsb16", "b", "8", "SI", "DI", "CX", 0, false);

        cmps("cmpsw32_r", "w", "16", "ESI", "EDI", "ECX", 1, true);
        cmps("cmpsw16_r", "w", "16", "SI", "DI", "CX", 1, true);
        cmps("cmpsw32", "w", "16", "ESI", "EDI", "ECX", 1, false);
        cmps("cmpsw16", "w", "16", "SI", "DI", "CX", 1, false);

        cmps("cmpsd32_r", "d", "32", "ESI", "EDI", "ECX", 2, true);
        cmps("cmpsd16_r", "d", "32", "SI", "DI", "CX", 2, true);
        cmps("cmpsd32", "d", "32", "ESI", "EDI", "ECX", 2, false);
        cmps("cmpsd16", "d", "32", "SI", "DI", "CX", 2, false);

        stos("stosb32_r", "b", "EDI", "ECX", "AL", 0, true);
        stos("stosb16_r", "b", "DI", "CX", "AL", 0, true);
        stos("stosb32", "b", "EDI", "ECX", "AL", 0, false);
        stos("stosb16", "b", "DI", "CX", "AL", 0, false);

        stos("stosw32_r", "w", "EDI", "ECX", "AX", 1, true);
        stos("stosw16_r", "w", "DI", "CX", "AX", 1, true);
        stos("stosw32", "w", "EDI", "ECX", "AX", 1, false);
        stos("stosw16", "w", "DI", "CX", "AX", 1, false);

        stos("stosd32_r", "d", "EDI", "ECX", "EAX", 2, true);
        stos("stosd16_r", "d", "DI", "CX", "EAX", 2, true);
        stos("stosd32", "d", "EDI", "ECX", "EAX", 2, false);
        stos("stosd16", "d", "DI", "CX", "EAX", 2, false);

        lods("lodsb32_r", "b", "ESI", "ECX", "AL", 0, true);
        lods("lodsb16_r", "b", "SI", "CX", "AL", 0, true);
        lods("lodsb32", "b", "ESI", "ECX", "AL", 0, false);
        lods("lodsb16", "b", "SI", "CX", "AL", 0, false);

        lods("lodsw32_r", "w", "ESI", "ECX", "AX", 1, true);
        lods("lodsw16_r", "w", "SI", "CX", "AX", 1, true);
        lods("lodsw32", "w", "ESI", "ECX", "AX", 1, false);
        lods("lodsw16", "w", "SI", "CX", "AX", 1, false);

        lods("lodsd32_r", "d", "ESI", "ECX", "EAX", 2, true);
        lods("lodsd16_r", "d", "SI", "CX", "EAX", 2, true);
        lods("lodsd32", "d", "ESI", "ECX", "EAX", 2, false);
        lods("lodsd16", "d", "SI", "CX", "EAX", 2, false);

        scas("scasb32_r", "b", "8", "AL", "EDI", "ECX", 0, true);
        scas("scasb16_r", "b", "8", "AL", "DI", "CX", 0, true);
        scas("scasb32", "b", "8", "AL", "EDI", "ECX", 0, false);
        scas("scasb16", "b", "8", "AL", "DI", "CX", 0, false);

        scas("scasw32_r", "w", "16", "AX", "EDI", "ECX", 1, true);
        scas("scasw16_r", "w", "16", "AX", "DI", "CX", 1, true);
        scas("scasw32", "w", "16", "AX", "EDI", "ECX", 1, false);
        scas("scasw16", "w", "16", "AX", "DI", "CX", 1, false);

        scas("scasd32_r", "d", "32", "EAX", "EDI", "ECX", 2, true);
        scas("scasd16_r", "d", "32", "EAX", "DI", "CX", 2, true);
        scas("scasd32", "d", "32", "EAX", "EDI", "ECX", 2, false);
        scas("scasd16", "d", "32", "EAX", "DI", "CX", 2, false);
    }

    static public void movs(String name, String bits, String SI, String DI, String CX, int inc, boolean repeat) throws  IOException {
        out("void "+name+"(struct CPU* cpu, struct Op* op) {");
        out("    U32 dBase = cpu->segAddress[ES];");
        out("    U32 sBase = cpu->segAddress[op->base];");
        out("    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out("    struct Memory* memory = cpu->memory;");
        if (repeat) {
        out("    U32 count = " + CX + ";");
        out("    U32 i;");
        out("    for (i=0;i<count;i++) {");
        }
        out("        write"+bits+"(memory, dBase+"+DI+", read"+bits+"(memory, sBase+"+SI+"));");
        out("        "+DI+"+=inc;");
        out("        "+SI+"+=inc;");
        if (repeat) {
        out("    }");
        out("    " + CX + "=0;");
        out("    CYCLES(3+count);");
        } else {
        out("    CYCLES(4);");
        }
        out("    NEXT();");
        out("}");
    }

    static public void cmps(String name, String width, String bits, String SI, String DI, String CX, int inc, boolean repeat) throws  IOException {
        out("void "+name+"(struct CPU* cpu, struct Op* op) {");
        out("    U32 dBase = cpu->segAddress[ES];");
        out("    U32 sBase = cpu->segAddress[op->base];");
        out("    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out("    struct Memory* memory = cpu->memory;");
        out("    U"+bits+" v1;");
        out("    U"+bits+" v2;");
        if (repeat) {
            out("    U32 count = " + CX + ";");
            out("    U32 i;");
            out("    U32 rep_zero = op->data1;");
            out("    if (count) {");
            out("        for (i=0;i<count;i++) {");
        }
        out("            v1 = read"+width+"(memory, dBase+"+DI+");");
        out("            v2 = read"+width+"(memory, sBase+"+SI+");");
        out("            "+DI+"+=inc;");
        out("            "+SI+"+=inc;");
        if (repeat) {
            out("            "+CX+"--;");
            out("            if ((v1==v2)!=rep_zero) break;");
            out("        }");
        }
        out("        cpu->dst.u"+bits+" = v1;");
        out("        cpu->src.u"+bits+" = v2;");
        out("        cpu->result.u"+bits+" = v1 - v2;");
        out("        cpu->inst = FLAGS_SUB"+bits+";");
        if (repeat) {
            out("    }");
            out("    CYCLES(9+4*count);");
        } else {
            out("    CYCLES(5);");
        }
        out("    NEXT();");
        out("}");
    }

    static public void scas(String name, String width, String bits, String AX, String DI, String CX, int inc, boolean repeat) throws  IOException {
        out("void "+name+"(struct CPU* cpu, struct Op* op) {");
        out("    U32 dBase = cpu->segAddress[ES];");
        out("    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out("    struct Memory* memory = cpu->memory;");
        out("    U"+bits+" v1;");
        if (repeat) {
            out("    U32 count = " + CX + ";");
            out("    U32 i;");
            out("    U32 rep_zero = op->data1;");
            out("    if (count) {");
            out("        for (i=0;i<count;i++) {");
        }
        out("            v1 = read"+width+"(memory, dBase+"+DI+");");
        out("            "+DI+"+=inc;");
        if (repeat) {
            out("            "+CX+"--;");
            out("            if (("+AX+"==v1)!=rep_zero) break;");
            out("        }");
        }
        out("        cpu->dst.u"+bits+" = "+AX+";");
        out("        cpu->src.u"+bits+" = v1;");
        out("        cpu->result.u"+bits+" = "+AX+" - v1;");
        out("        cpu->inst = FLAGS_SUB"+bits+";");
        if (repeat) {
            out("    }");
            out("    CYCLES(8+4*count);");
        } else {
            out("    CYCLES(4);");
        }
        out("    NEXT();");
        out("}");
    }

    static public void lods(String name, String bits, String SI, String CX, String AX, int inc, boolean repeat) throws  IOException {
        out("void "+name+"(struct CPU* cpu, struct Op* op) {");
        if (repeat) {
            out("    U32 sBase = cpu->segAddress[op->base];");
            out("    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
            out("    struct Memory* memory = cpu->memory;");
            out("    U32 count = " + CX + ";");
            out("    U32 i;");
            out("    for (i=0;i<count;i++) {");
            out("        "+AX+" = read"+bits+"(memory, sBase+"+SI+");");
            out("        "+SI+"+=inc;");
            out("    }");
            out("    " + CX + "=0;");
        } else {
            out("    "+AX+" = read"+bits+"(cpu->memory, cpu->segAddress[op->base]+"+SI+");");
            out("    "+SI+"+=cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        }
        out("    CYCLES(2);");
        out("    NEXT();");
        out("}");
    }

    static public void stos(String name, String bits, String DI, String CX, String AX, int inc, boolean repeat) throws  IOException {
        out("void "+name+"(struct CPU* cpu, struct Op* op) {");
        if (repeat) {
        out("    U32 dBase = cpu->segAddress[ES];");
        out("    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out("    struct Memory* memory = cpu->memory;");
        out("    U32 count = " + CX + ";");
        out("    U32 i;");
        out("    for (i=0;i<count;i++) {");
        out("        write"+bits+"(memory, dBase+"+DI+", "+AX+");");
        out("        "+DI+"+=inc;");
        out("    }");
        out("    " + CX + "=0;");
        out("    CYCLES(3+count);");
        } else {
            out("    write"+bits+"(cpu->memory, cpu->segAddress[ES]+"+DI+", "+AX+");");
            out("    "+DI+"+=cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
            out("    CYCLES(3);");
        }
        out("    NEXT();");
        out("}");
    }

    static public void pushPop() throws IOException {
        pushPopReg("Ax", "16");
        pushPopReg("Eax", "32");
        pushPopReg("Cx", "16");
        pushPopReg("Ecx", "32");
        pushPopReg("Dx", "16");
        pushPopReg("Edx", "32");
        pushPopReg("Bx", "16");
        pushPopReg("Ebx", "32");
        pushPopReg("Sp", "16");
        pushPopReg("Esp", "32");
        pushPopReg("Bp", "16");
        pushPopReg("Ebp", "32");
        pushPopReg("Si", "16");
        pushPopReg("Esi", "32");
        pushPopReg("Di", "16");
        pushPopReg("Edi", "32");
    }

    static public void pushPopReg(String name, String bits) throws IOException {
        out("void push"+name+"(struct CPU* cpu, struct Op* op){");
        out("    push"+bits+"(cpu, "+name.toUpperCase()+");");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void pop"+name+"(struct CPU * cpu, struct Op * op){");
        out("    "+name.toUpperCase()+" = pop"+bits+"(cpu);");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
    }

    static public void incDec(String name, String op) throws IOException {
        incDec8(name, op);
        incDec16(name, op);
        incDec32(name, op);
    }

    static public void incDec8(String name, String op) throws IOException {
        incDecBase(name+"8_reg", "FLAGS_"+name.toUpperCase()+"8", "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", op, "1", false, false, "8");
        incDecBase(name+"8_mem16", "FLAGS_"+name.toUpperCase()+"8", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, "3", true, false, "8");
        incDecBase(name+"8_mem32", "FLAGS_"+name.toUpperCase()+"8", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, "3", false, true, "8");
    }

    static public void incDec16(String name, String op) throws IOException {
        incDecBase(name+"16_reg", "FLAGS_"+name.toUpperCase()+"16", "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", op, "1", false, false, "16");
        incDecBase(name+"16_mem16", "FLAGS_"+name.toUpperCase()+"16", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, "3", true, false, "16");
        incDecBase(name+"16_mem32", "FLAGS_"+name.toUpperCase()+"16", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, "3", false, true, "16");
    }

    static public void incDec32(String name, String op) throws IOException {
        incDecBase(name+"32_reg", "FLAGS_"+name.toUpperCase()+"32", "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", op, "1", false, false, "32");
        incDecBase(name+"32_mem16", "FLAGS_"+name.toUpperCase()+"32", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, "3", true, false, "32");
        incDecBase(name+"32_mem32", "FLAGS_"+name.toUpperCase()+"32", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, "3", false, true, "32");
    }

    static public void incDecBase(String name, String flagName, String destLoad, String destSave1, String destSave2, String op, String cycles, boolean eaa16, boolean eaa32, String bits) throws IOException {
        out("void "+name+"(struct CPU* cpu, struct Op* op) {");
        if (eaa16)
        out("    U32 eaa = eaa16(cpu, op);");
        if (eaa32)
        out("    U32 eaa = eaa32(cpu, op);");
        out("    cpu->oldcf=getCF(cpu);");
        out("    cpu->dst.u"+bits+"="+destLoad+";");
        out("    cpu->result.u"+bits+"=cpu->dst.u"+bits+" "+op+" 1;");
        out("    cpu->inst = "+flagName+";");
        out("    "+destSave1+"cpu->result.u"+bits+destSave2+";");
        out("    CYCLES("+cycles+");");
        out("    NEXT();");
        out("}");
    }

    static public void out(String line) throws IOException {
        line=line+"\r\n";
        fos.write(line.getBytes());
    }

    static public void arith(String name, String op, boolean cf, boolean result, String rrCycles, String mrCycles, String rmCycles, String rdCycles, String mdCycles)  throws IOException {
        arith8(name, op, cf, result, "8", rrCycles, mrCycles, rmCycles, rdCycles, mdCycles);
        arith16(name, op, cf, result, "16", rrCycles, mrCycles, rmCycles, rdCycles, mdCycles);
        arith32(name, op, cf, result, "32", rrCycles, mrCycles, rmCycles, rdCycles, mdCycles);
    }
    static public void arith32(String name, String op, boolean cf, boolean result, String bits, String rrCycles, String mrCycles, String rmCycles, String rdCycles, String mdCycles)  throws IOException {
        arithbase(name + "r32r32", "FLAGS_" + name.toUpperCase() + bits, "cpu->reg[op->r2].u32", "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", op, cf, result, false, false, bits, rrCycles);
        arithbase(name + "e32r32_16", "FLAGS_" + name.toUpperCase() + bits, "cpu->reg[op->r1].u32", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mrCycles);
        arithbase(name + "e32r32_32", "FLAGS_" + name.toUpperCase() + bits, "cpu->reg[op->r1].u32", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mrCycles);
        arithbase(name + "r32e32_16", "FLAGS_" + name.toUpperCase() + bits, "readd(cpu->memory, eaa16(cpu, op))", "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", op, cf, result, false, false, bits, rmCycles);
        arithbase(name + "r32e32_32", "FLAGS_" + name.toUpperCase() + bits, "readd(cpu->memory, eaa32(cpu, op))", "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", op, cf, result, false, false, bits, rmCycles);
        arithbase(name + "32_reg", "FLAGS_" + name.toUpperCase() + bits, "op->data1", "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", op, cf, result, false, false, bits, rdCycles);
        arithbase(name + "32_mem16", "FLAGS_" + name.toUpperCase() + bits, "op->data1", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mdCycles);
        arithbase(name + "32_mem32", "FLAGS_" + name.toUpperCase() + bits, "op->data1", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mdCycles);
    }
    static public void arith16(String name, String op, boolean cf, boolean result, String bits, String rrCycles, String mrCycles, String rmCycles, String rdCycles, String mdCycles)  throws IOException {
        arithbase(name+"r16r16", "FLAGS_"+name.toUpperCase()+bits, "cpu->reg[op->r2].u16", "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", op, cf, result, false, false, bits, rrCycles);
        arithbase(name+"e16r16_16", "FLAGS_"+name.toUpperCase()+bits, "cpu->reg[op->r1].u16", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mrCycles);
        arithbase(name+"e16r16_32", "FLAGS_"+name.toUpperCase()+bits, "cpu->reg[op->r1].u16", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mrCycles);
        arithbase(name+"r16e16_16", "FLAGS_"+name.toUpperCase()+bits, "readw(cpu->memory, eaa16(cpu, op))", "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", op, cf, result, false, false, bits, rmCycles);
        arithbase(name+"r16e16_32", "FLAGS_"+name.toUpperCase()+bits, "readw(cpu->memory, eaa32(cpu, op))", "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", op, cf, result, false, false, bits, rmCycles);
        arithbase(name+"16_reg", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", op, cf, result, false, false, bits, rdCycles);
        arithbase(name+"16_mem16", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mdCycles);
        arithbase(name+"16_mem32", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mdCycles);
    }
    static public void arith8(String name, String op, boolean cf, boolean result, String bits, String rrCycles, String mrCycles, String rmCycles, String rdCycles, String mdCycles)  throws IOException {
        arithbase(name+"r8r8", "FLAGS_"+name.toUpperCase()+bits, "*cpu->reg8[op->r2]", "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", op, cf, result, false, false, bits, rrCycles);
        arithbase(name+"e8r8_16", "FLAGS_"+name.toUpperCase()+bits, "*cpu->reg8[op->r1]", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mrCycles);
        arithbase(name+"e8r8_32", "FLAGS_"+name.toUpperCase()+bits, "*cpu->reg8[op->r1]", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mrCycles);
        arithbase(name+"r8e8_16", "FLAGS_"+name.toUpperCase()+bits, "readb(cpu->memory, eaa16(cpu, op))", "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", op, cf, result, false, false, bits, rmCycles);
        arithbase(name+"r8e8_32", "FLAGS_"+name.toUpperCase()+bits, "readb(cpu->memory, eaa32(cpu, op))", "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", op, cf, result, false, false, bits, rmCycles);
        arithbase(name+"8_reg", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", op, cf, result, false, false, bits, rdCycles);
        arithbase(name+"8_mem16", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mdCycles);
        arithbase(name+"8_mem32", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mdCycles);
    }
    static public void arithbase(String functionName, String flagName, String source, String loadDest, String saveDest1, String saveDest2, String op, boolean cf, boolean result, boolean eaa16, boolean eaa32, String bits, String cycles)  throws IOException {
        out("void "+functionName+"(struct CPU* cpu, struct Op* op) {");
        if (eaa16)
            out("    U32 eaa = eaa16(cpu, op);");
        if (eaa32)
            out("    U32 eaa = eaa32(cpu, op);");
        if (cf)
        out("    cpu->oldcf = getCF(cpu);");
        out("    cpu->dst.u"+bits+" = "+loadDest+";");
        out("    cpu->src.u"+bits+" = "+source+";");
        out("    cpu->result.u"+bits+" = cpu->dst.u"+bits+" "+op+" cpu->src.u"+bits+(cf?" "+op+" cpu->oldcf;":";"));
        out("    cpu->inst = "+flagName+";");
        if (result)
        out("    "+saveDest1+" cpu->result.u"+bits+saveDest2+";");
        out("    CYCLES("+cycles+");");
        out("    NEXT();");
        out("}");
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
    static public String shl8 = "result = var1 << var2;\r\n    cpu->inst = FLAGS_SHL8;\r\n    cpu->result.u8 = result;\r\n    cpu->src.u8=var2;\r\n    cpu->dst.u8 = var1;";
    static public String shl16 = "result = var1 << var2;\r\n    cpu->inst = FLAGS_SHL16;\r\n    cpu->result.u16 = result;\r\n    cpu->src.u16=var2;\r\n    cpu->dst.u16 = var1;";
    static public String shl32 = "result = var1 << var2;\r\n    cpu->inst = FLAGS_SHL32;\r\n    cpu->result.u32 = result;\r\n    cpu->src.u32=var2;\r\n    cpu->dst.u32 = var1;";
    static public String shr8 = "result = var1 >> var2;\r\n    cpu->inst = FLAGS_SHR8;\r\n    cpu->result.u8 = result;\r\n    cpu->src.u8=var2;\r\n    cpu->dst.u8 = var1;";
    static public String shr16 = "result = var1 >> var2;\r\n    cpu->inst = FLAGS_SHR16;\r\n    cpu->result.u16 = result;\r\n    cpu->src.u16=var2;\r\n    cpu->dst.u16 = var1;";
    static public String shr32 = "result = var1 >> var2;\r\n    cpu->inst = FLAGS_SHR32;\r\n    cpu->result.u32 = result;\r\n    cpu->src.u32=var2;\r\n    cpu->dst.u32 = var1;";
    static public String sar8 = "result = (S8)var1 >> var2;\r\n    cpu->inst = FLAGS_SAR8;\r\n    cpu->result.u8 = result;\r\n    cpu->src.u8=var2;\r\n    cpu->dst.u8 = var1;";
    static public String sar16 = "result = (S16)var1 >> var2;\r\n    cpu->inst = FLAGS_SAR16;\r\n    cpu->result.u16 = result;\r\n    cpu->src.u16=var2;\r\n    cpu->dst.u16 = var1;";
    static public String sar32 = "result = (S32)var1 >> var2;\r\n    cpu->inst = FLAGS_SAR32;\r\n    cpu->result.u32 = result;\r\n    cpu->src.u32=var2;\r\n    cpu->dst.u32 = var1;";

    static public void shift() throws IOException {
        shiftInst8("rol8", "op->data1", "1", "3", rol8, 0, 0, false);
        shiftInst8("rol8cl", "CL & 0x1f", "4", "4", rol8, 7, 0, true);
        shiftInst16("rol16", "op->data1", "1", "3", rol16, 0, 0, false);
        shiftInst16("rol16cl", "CL & 0x1f", "4", "4", rol16, 0xf, 0, true);
        shiftInst32("rol32", "op->data1", "1", "3", rol32, 0, 0, false);
        shiftInst32("rol32cl", "CL & 0x1f", "4", "4", rol32, 0, 0, true);

        shiftInst8("ror8", "op->data1", "1", "3", ror8, 0, 0, false);
        shiftInst8("ror8cl", "CL & 0x1f", "4", "4", ror8, 7, 0, true);
        shiftInst16("ror16", "op->data1", "1", "3", ror16, 0, 0, false);
        shiftInst16("ror16cl", "CL & 0x1f", "4", "4", ror16, 0xf, 0, true);
        shiftInst32("ror32", "op->data1", "1", "3", ror32, 0, 0, false);
        shiftInst32("ror32cl", "CL & 0x1f", "4", "4", ror32, 0, 0, true);

        shiftInst8("rcl8", "op->data1", "8", "10", rcl8, 0, 0, false);
        shiftInst8("rcl8cl", "CL & 0x1f", "7", "9", rcl8, 0, 9, true);
        shiftInst16("rcl16", "op->data1", "8", "10", rcl16, 0, 0, false);
        shiftInst16("rcl16cl", "CL & 0x1f", "7", "9", rcl16, 0, 17, true);
        shiftInst32("rcl32", "op->data1", "8", "10", rcl32, 0, 0, false);
        shiftInst32("rcl32cl", "CL & 0x1f", "7", "9", rcl32, 0, 0, true);

        shiftInst8("rcr8", "op->data1", "8", "10", rcr8, 0, 0, false);
        shiftInst8("rcr8cl", "CL & 0x1f", "7", "9", rcr8, 0, 9, true);
        shiftInst16("rcr16", "op->data1", "8", "10", rcr16, 0, 0, false);
        shiftInst16("rcr16cl", "CL & 0x1f", "7", "9", rcr16, 0, 17, true);
        shiftInst32("rcr32", "op->data1", "8", "10", rcr32, 0, 0, false);
        shiftInst32("rcr32cl", "CL & 0x1f", "7", "9", rcr32, 0, 0, true);

        shiftInst8("shl8", "op->data1", "1", "3", shl8, 0, 0, false);
        shiftInst8("shl8cl", "CL & 0x1f", "4", "4", shl8, 0, 0, true);
        shiftInst16("shl16", "op->data1", "1", "3", shl16, 0, 0, false);
        shiftInst16("shl16cl", "CL & 0x1f", "4", "4", shl16, 0, 0, true);
        shiftInst32("shl32", "op->data1", "1", "3", shl32, 0, 0, false);
        shiftInst32("shl32cl", "CL & 0x1f", "4", "4", shl32, 0, 0, true);

        shiftInst8("shr8", "op->data1", "1", "3", shr8, 0, 0, false);
        shiftInst8("shr8cl", "CL & 0x1f", "4", "4", shr8, 0, 0, true);
        shiftInst16("shr16", "op->data1", "1", "3", shr16, 0, 0, false);
        shiftInst16("shr16cl", "CL & 0x1f", "4", "4", shr16, 0, 0, true);
        shiftInst32("shr32", "op->data1", "1", "3", shr32, 0, 0, false);
        shiftInst32("shr32cl", "CL & 0x1f", "4", "4", shr32, 0, 0, true);

        shiftInst8("sar8", "op->data1", "1", "3", sar8, 0, 0, false);
        shiftInst8("sar8cl", "CL & 0x1f", "4", "4", sar8, 0, 0, true);
        shiftInst16("sar16", "op->data1", "1", "3", sar16, 0, 0, false);
        shiftInst16("sar16cl", "CL & 0x1f", "4", "4", sar16, 0, 0, true);
        shiftInst32("sar32", "op->data1", "1", "3", sar32, 0, 0, false);
        shiftInst32("sar32cl", "CL & 0x1f", "4", "4", sar32, 0, 0, true);
    }
    static public void shiftInst8(String name, String source, String rcycles, String mcycles, String inst, int mask, int mod, boolean checkForZero) throws IOException {
        shiftBase(name+"_reg", false, false, "8", source, "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", rcycles, inst, mask, mod, checkForZero);
        shiftBase(name+"_mem16", true, false, "8", source, "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
        shiftBase(name+"_mem32", false, true, "8", source, "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
    }

    static public void shiftInst16(String name, String source, String rcycles, String mcycles, String inst, int mask, int mod, boolean checkForZero) throws IOException {
        shiftBase(name+"_reg", false, false, "16", source, "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", rcycles, inst, mask, mod, checkForZero);
        shiftBase(name+"_mem16", true, false, "16", source, "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
        shiftBase(name+"_mem32", false, true, "16", source, "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
    }

    static public void shiftInst32(String name, String source, String rcycles, String mcycles, String inst, int mask, int mod, boolean checkForZero) throws IOException {
        shiftBase(name+"_reg", false, false, "32", source, "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", rcycles, inst, mask, mod, checkForZero);
        shiftBase(name+"_mem16", true, false, "32", source, "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
        shiftBase(name+"_mem32", false, true, "32", source, "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", mcycles, inst, mask, mod, checkForZero);
    }

    static public void shiftBase(String name, boolean eaa16, boolean eaa32, String bits, String shiftSource, String source, String destSave1, String destSave2, String cycles, String inst, int mask, int mod, boolean checkForZero) throws IOException {
        out("void "+name+"(struct CPU* cpu, struct Op* op) {");
        out("    U"+bits+" result;");
        if (eaa16)
            out("    U32 eaa = eaa16(cpu, op);");
        if (eaa32)
            out("    U32 eaa = eaa32(cpu, op);");
        out("    U"+bits+" var1"+((mask==0 && mod==0)?"="+source:"")+";");
        out("    U"+bits+" var2 = "+shiftSource+";");
        if (checkForZero) {
            out("    if (var2) {");
        }
        if (mask!=0 || mod!=0) {

            if (mask!=0)
                out("    var2&="+mask+";");
            if (mod!=0)
                out("    var2=var2 % "+mod+";");
            out("    var1 = "+source+";");
        }
        out("    "+inst);
        out("    "+destSave1+"result"+destSave2+";");
        if (checkForZero) {
            out("    }");
        }
        out("    CYCLES("+cycles+");");
        out("    NEXT();");
        out("}");
    }
}

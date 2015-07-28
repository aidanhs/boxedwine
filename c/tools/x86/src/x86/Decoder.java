package x86;

import java.io.FileOutputStream;
import java.io.IOException;

public class Decoder extends Base {
    public void decode(FileOutputStream fos) throws IOException {
        decodePushSeg(fos, "06", "es");
        decodePopSeg(fos, "07", "es");
        decodePushSeg(fos, "0e", "cs");
        decodePushSeg(fos, "16", "ss");
        decodePopSeg(fos, "17", "ss");
        decodePushSeg(fos, "1e", "ds");
        decodePopSeg(fos, "1f", "ds");
        decodeSeg(fos, "26", "es");
        decodeFunc(fos, "027", "daa", "DAA");
        decodeSeg(fos, "2e", "cs");
        decodeFunc(fos, "02f", "das", "DAS");
        decodeSeg(fos, "36", "ss");
        decodeFunc(fos, "037", "aaa", "AAA");
        decodeSeg(fos, "3e", "ds");
        decodeFunc(fos, "03f", "aas", "AAS");


        decodeSeg(fos, "64", "fs");
        decodeSeg(fos, "65", "gs");
        decodeFuncData(fos, "PUSH", "PUSH Iw", "068", "push16data", 16);
        decodeFuncData(fos, "PUSH", "PUSH Id", "268", "push32data", 32);
        ge(fos, "069", "dimulc", 16, -16, "IMUL Gw,Ew,Iw");
        ge(fos, "269", "dimulc", 32, 32, "IMUL Gd,Ed,Id");
        decodeFuncData(fos, "PUSH", "PUSH Ib", "06a", "push16data", -8);
        decodeFuncData(fos, "PUSH", "PUSH Ib", "26a", "push32data", -8);
        ge(fos, "06b", "dimulc", 16, -8, "IMUL Gw,Ew,Ib");
        ge(fos, "26b", "dimulc", 32, -8, "IMUL Gd,Ed,Ib");
        decodeJump(fos, "070", "O", -8);
        decodeJump(fos, "071", "NO", -8);
        decodeJump(fos, "072", "B", -8);
        decodeJump(fos, "073", "NB", -8);
        decodeJump(fos, "074", "Z", -8);
        decodeJump(fos, "075", "NZ", -8);
        decodeJump(fos, "076", "BE", -8);
        decodeJump(fos, "077", "NBE", -8);
        decodeJump(fos, "078", "S", -8);
        decodeJump(fos, "079", "NS", -8);
        decodeJump(fos, "07a", "P", -8);
        decodeJump(fos, "07b", "NP", -8);
        decodeJump(fos, "07c", "L", -8);
        decodeJump(fos, "07d", "NL", -8);
        decodeJump(fos, "07e", "LE", -8);
        decodeJump(fos, "07f", "NLE", -8);
        eg(fos, "084", "test", 8);
        eg(fos, "085", "test", 16);
        eg(fos, "285", "test", 32);
        eg(fos, "086", "xchg", 8);
        eg(fos, "087", "xchg", 16);
        eg(fos, "287", "xchg", 32);
        eg(fos, "088", "mov", 8);
        eg(fos, "089", "mov", 16);
        eg(fos, "289", "mov", 32);
        ge(fos, "08a", "mov", 8);
        ge(fos, "08b", "mov", 16);
        ge(fos, "28b", "mov", 32);

        decodeFunc(fos, "XCHG CX,AX", "091", "xchgr16r16", 0, 1);
        decodeFunc(fos, "XCHG ECX,EAX", "291", "xchgr32r32", 0, 1);
        decodeFunc(fos, "XCHG DX,AX", "092", "xchgr16r16", 0, 2);
        decodeFunc(fos, "XCHG EDX,EAX", "292", "xchgr32r32", 0, 2);
        decodeFunc(fos, "XCHG BX,AX", "093", "xchgr16r16", 0, 3);
        decodeFunc(fos, "XCHG EBX,EAX", "293", "xchgr32r32", 0, 3);
        decodeFunc(fos, "XCHG SP,AX", "094", "xchgr16r16", 0, 4);
        decodeFunc(fos, "XCHG ESP,EAX", "294", "xchgr32r32", 0, 4);
        decodeFunc(fos, "XCHG BP,AX", "095", "xchgr16r16", 0, 5);
        decodeFunc(fos, "XCHG EBP,EAX", "295", "xchgr32r32", 0, 5);
        decodeFunc(fos, "XCHG SI,AX", "096", "xchgr16r16", 0, 6);
        decodeFunc(fos, "XCHG ESI,EAX", "296", "xchgr32r32", 0, 6);
        decodeFunc(fos, "XCHG DI,AX", "097", "xchgr16r16", 0, 7);
        decodeFunc(fos, "XCHG EDI,EAX", "297", "xchgr32r32", 0, 7);
        decodeFunc(fos, "098", "cbw", "CBW");
        decodeFunc(fos, "298", "cbwe", "CBWE");
        decodeFunc(fos, "099", "cwd", "CWD");
        decodeFunc(fos, "299", "cwq", "CWQ");
        decodeFunc(fos, "09c", "pushf16", "PUSHF");
        decodeFunc(fos, "29c", "pushf32", "PUSHF");
        decodeFunc(fos, "09d", "popf16", "POPF");
        decodeFunc(fos, "29d", "popf32", "POPF");
        decodeFunc(fos, "09e", "sahf", "SAHF");
        decodeFunc(fos, "09f", "lahf", "LAHF");
        decodeFuncEAdata(fos, "MOV AL,Ob", "0a0", "movAl", "LOG_OP2(\"MOV\", \"AL\", O8(data->op));");
        decodeFuncEAdata(fos, "MOV AX,Ow", "0a1", "movAx", "LOG_OP2(\"MOV\", \"AX\", O16(data->op));");
        decodeFuncEAdata(fos, "MOV EAX,Od", "2a1", "movEax", "LOG_OP2(\"MOV\", \"EAX\", O32(data->op));");
        decodeFuncEAdata(fos, "MOV Ob,Al", "0a2", "movDirectAl", "LOG_OP2(\"MOV\", O8(data->op), \"AL\");");
        decodeFuncEAdata(fos, "MOV Ow,Ax", "0a3", "movDirectAx", "LOG_OP2(\"MOV\", O16(data->op), \"AX\");");
        decodeFuncEAdata(fos, "MOV Od,Eax", "2a3", "movDirectEax", "LOG_OP2(\"MOV\", O32(data->op), \"EAX\");");


        decodeFuncRegData(fos, "TEST", "TEST AL,Ib", "0a8", "test8_reg", 0, 8);
        decodeFuncRegData(fos, "TEST", "TEST AX,Iw", "0a9", "test16_reg", 0, 16);
        decodeFuncRegData(fos, "TEST", "TEST EAX,Id", "2a9", "test32_reg", 0, 32);

        decodeFuncRegData(fos, "MOV", "MOV AL,Ib", "0b0", "mov8_reg", 0, 8);
        decodeFuncRegData(fos, "MOV", "MOV CL,Ib", "0b1", "mov8_reg", 1, 8);
        decodeFuncRegData(fos, "MOV", "MOV DL,Ib", "0b2", "mov8_reg", 2, 8);
        decodeFuncRegData(fos, "MOV", "MOV BL,Ib", "0b3", "mov8_reg", 3, 8);
        decodeFuncRegData(fos, "MOV", "MOV AH,Ib", "0b4", "mov8_reg", 4, 8);
        decodeFuncRegData(fos, "MOV", "MOV CH,Ib", "0b5", "mov8_reg", 5, 8);
        decodeFuncRegData(fos, "MOV", "MOV DH,Ib", "0b6", "mov8_reg", 6, 8);
        decodeFuncRegData(fos, "MOV", "MOV BH,Ib", "0b7", "mov8_reg", 7, 8);
        decodeFuncRegData(fos, "MOV", "MOV AX,Iw", "0b8", "mov16_reg", 0, 16);
        decodeFuncRegData(fos, "MOV", "MOV EAX,Id", "2b8", "mov32_reg", 0, 32);
        decodeFuncRegData(fos, "MOV", "MOV CX,Iw", "0b9", "mov16_reg", 1, 16);
        decodeFuncRegData(fos, "MOV", "MOV ECX,Id", "2b9", "mov32_reg", 1, 32);
        decodeFuncRegData(fos, "MOV", "MOV DX,Iw", "0ba", "mov16_reg", 2, 16);
        decodeFuncRegData(fos, "MOV", "MOV EDX,Id", "2ba", "mov32_reg", 2, 32);
        decodeFuncRegData(fos, "MOV", "MOV BX,Iw", "0bb", "mov16_reg", 3, 16);
        decodeFuncRegData(fos, "MOV", "MOV EBX,Id", "2bb", "mov32_reg", 3, 32);
        decodeFuncRegData(fos, "MOV", "MOV SP,Iw", "0bc", "mov16_reg", 4, 16);
        decodeFuncRegData(fos, "MOV", "MOV ESP,Id", "2bc", "mov32_reg", 4, 32);
        decodeFuncRegData(fos, "MOV", "MOV BP,Iw", "0bd", "mov16_reg", 5, 16);
        decodeFuncRegData(fos, "MOV", "MOV EBP,Id", "2bd", "mov32_reg", 5, 32);
        decodeFuncRegData(fos, "MOV", "MOV SI,Iw", "0be", "mov16_reg", 6, 16);
        decodeFuncRegData(fos, "MOV", "MOV ESI,Id", "2be", "mov32_reg", 6, 32);
        decodeFuncRegData(fos, "MOV", "MOV DI,Iw", "0bf", "mov16_reg", 7, 16);
        decodeFuncRegData(fos, "MOV", "MOV EDI,Id", "2bf", "mov32_reg", 7, 32);
        decodeFuncDataEndOfBlock(fos, "RETN", "RETN Iw", "0c2", "retnIw16", 16);
        decodeFuncDataEndOfBlock(fos, "RETN", "RETN Iw", "2c2", "retnIw32", 16);
        decodeFuncEndOfBlock(fos, "0c3", "retn16", "RETN");
        decodeFuncEndOfBlock(fos, "2c3", "retn32", "RETN");
        decodeE(fos, "MOV", "MOV Eb,Ib", "0c6", "mov8", 8, 8);
        decodeE(fos, "MOV", "MOV Ew,Iw", "0c7", "mov16", 16, 16);
        decodeE(fos, "MOV", "MOV Ed,Id", "2c7", "mov32", 32, 32);
        decodeFunc(fos, "0c9", "leave16", "LEAVE");
        decodeFunc(fos, "2c9", "leave32", "LEAVE");
        decodeFunc(fos, "0d6", "salc", "SALC");
        decodeFuncDataEndOfBlock(fos, "CALL", "CALL Jw ", "0e8", "callJw", -16);
        decodeFuncDataEndOfBlock(fos, "CALL", "CALL Jd ", "2e8", "callJd", 32);
        decodeFuncDataEndOfBlock(fos, "JMP", "JMP Jw ", "0e9", "jump", -16);
        decodeFuncDataEndOfBlock(fos, "JMP", "JMP Jd ", "2e9", "jump", 32);
        decodeFuncDataEndOfBlock(fos, "JMP", "JMP Jb ", "0eb", "jump", -8);
        decodeFunc(fos, "0f5", "cmc", "CMC");
        decodeGroup3(fos, "GRP3 Eb(,Ib)", "0f6", 8);
        decodeGroup3(fos, "GRP3 Ew(,Iw)", "0f7", 16);
        decodeGroup3(fos, "GRP3 Ed(,Id)", "2f7", 32);
        decodeFunc(fos, "0f8", "clc", "CLC");
        decodeFunc(fos, "0f9", "stc", "STC");
        decodeFunc(fos, "0fc", "cld", "CLD");
        decodeFunc(fos, "0fd", "std", "STD");
        decodeFunc(fos, "131", "rdtsc", "RDTSC");

        decodeJump(fos, "180", "O", -16);
        decodeJump(fos, "181", "NO", -16);
        decodeJump(fos, "182", "B", -16);
        decodeJump(fos, "183", "NB", -16);
        decodeJump(fos, "184", "Z", -16);
        decodeJump(fos, "185", "NZ", -16);
        decodeJump(fos, "186", "BE", -16);
        decodeJump(fos, "187", "NBE", -16);
        decodeJump(fos, "188", "S", -16);
        decodeJump(fos, "189", "NS", -16);
        decodeJump(fos, "18a", "P", -16);
        decodeJump(fos, "18b", "NP", -16);
        decodeJump(fos, "18c", "L", -16);
        decodeJump(fos, "18d", "NL", -16);
        decodeJump(fos, "18e", "LE", -16);
        decodeJump(fos, "18f", "NLE", -16);

        decodeJump(fos, "380", "O", 32);
        decodeJump(fos, "381", "NO", 32);
        decodeJump(fos, "382", "B", 32);
        decodeJump(fos, "383", "NB", 32);
        decodeJump(fos, "384", "Z", 32);
        decodeJump(fos, "385", "NZ", 32);
        decodeJump(fos, "386", "BE", 32);
        decodeJump(fos, "387", "NBE", 32);
        decodeJump(fos, "388", "S", 32);
        decodeJump(fos, "389", "NS", 32);
        decodeJump(fos, "38a", "P", 32);
        decodeJump(fos, "38b", "NP", 32);
        decodeJump(fos, "38c", "L", 32);
        decodeJump(fos, "38d", "NL", 32);
        decodeJump(fos, "38e", "LE", 32);
        decodeJump(fos, "38f", "NLE", 32);

        decodeE(fos, "SETO", "SETO", "190", "setO", 0, 8);
        decodeE(fos, "SETNO", "SETNO", "191", "setNO", 0, 8);
        decodeE(fos, "SETB", "SETB", "192", "setB", 0, 8);
        decodeE(fos, "SETNB", "SETNB", "193", "setNB", 0, 8);
        decodeE(fos, "SETZ", "SETZ", "194", "setZ", 0, 8);
        decodeE(fos, "SETNZ", "SETNZ", "195", "setNZ", 0, 8);
        decodeE(fos, "SETBE", "SETBE", "196", "setBE", 0, 8);
        decodeE(fos, "SETNBE", "SETNBE", "197", "setNBE", 0, 8);
        decodeE(fos, "SETS", "SETS", "198", "setS", 0, 8);
        decodeE(fos, "SETNS", "SETNS", "199", "setNS", 0, 8);
        decodeE(fos, "SETP", "SETP", "19a", "setP", 0, 8);
        decodeE(fos, "SETNP", "SETNP", "19b", "setNP", 0, 8);
        decodeE(fos, "SETL", "SETL", "19c", "setL", 0, 8);
        decodeE(fos, "SETNL", "SETNL", "19d", "setNL", 0, 8);
        decodeE(fos, "SETLE", "SETLE", "19e", "setLE", 0, 8);
        decodeE(fos, "SETNLE", "SETNLE", "19f", "setNLE", 0, 8);

        decodeFunc(fos, "PUSH FS", "1a0", "pushSeg16", "FS");
        decodeFunc(fos, "PUSH FS", "3a0", "pushSeg32", "FS");
        decodeFunc(fos, "POP FS", "1a1", "popSeg16", "FS");
        decodeFunc(fos, "POP FS", "3a1", "popSeg32", "FS");
        decodeFunc(fos, "1a2", "cpuid", "CPUID");
        eg(fos, "1a3", "bt", 16);
        eg(fos, "3a3", "bt", 32);
        eg(fos, "1a4", "dshl", 16, "    data->op->data1 = FETCH8(data);\r\n    data->op->data1 &= 0x1f;\r\n    if (data->op->data1 == 0) {\r\n        RESTART(data);\r\n    }");
        eg(fos, "3a4", "dshl", 32, "    data->op->data1 = FETCH8(data);\r\n    data->op->data1 &= 0x1f;\r\n    if (data->op->data1 == 0) {\r\n        RESTART(data);\r\n    }");
        eg(fos, "1a5", "dshlcl", 16);
        eg(fos, "3a5", "dshlcl", 32);
        decodeFunc(fos, "PUSH GS", "1a8", "pushSeg16", "GS");
        decodeFunc(fos, "PUSH GS", "3a8", "pushSeg32", "GS");
        decodeFunc(fos, "POP GS", "1a9", "popSeg16", "GS");
        decodeFunc(fos, "POP GS", "3a9", "popSeg32", "GS");
        eg(fos, "1ab", "bts", 16);
        eg(fos, "3ab", "bts", 32);
        eg(fos, "1ac", "dshr", 16, "    data->op->data1 = FETCH8(data);\r\n    data->op->data1 &= 0x1f;\r\n    if (data->op->data1 == 0) {\r\n        RESTART(data);\r\n    }");
        eg(fos, "3ac", "dshr", 32, "    data->op->data1 = FETCH8(data);\r\n    data->op->data1 &= 0x1f;\r\n    if (data->op->data1 == 0) {\r\n        RESTART(data);\r\n    }");
        eg(fos, "1ad", "dshrcl", 16);
        eg(fos, "3ad", "dshrcl", 32);
        ge(fos, "1af", "dimul", 16);
        ge(fos, "3af", "dimul", 32);
        eg(fos, "1b1", "cmpxchg", 16);
        eg(fos, "3b1", "cmpxchg", 32);
        eg(fos, "3b3", "btr", 32);
        ge(fos, "1b6", "movxz8", 16);
        ge(fos, "3b6", "movxz8", 32);
        ge(fos, "3b7", "movxz16", 32);

        eg(fos, "3bb", "btc", 32);
        ge(fos, "3bc", "bsf", 32);

        ge(fos, "1bd", "bsr", 16);
        ge(fos, "3bd", "bsr", 32);

        // :TODO: op log isn't correct, the src should be 8 bits
        ge(fos, "1be", "movsx8", 16);
        ge(fos, "3be", "movsx8", 32);
        ge(fos, "3bf", "movsx16", 32);

        ge(fos, "3c1", "xadd32", 32, 0, "XADD");

        decodeFunc(fos, "BSWAP", "3c8", "bswap32", "0");
        decodeFunc(fos, "BSWAP", "3c9", "bswap32", "1");
        decodeFunc(fos, "BSWAP", "3ca", "bswap32", "2");
        decodeFunc(fos, "BSWAP", "3cb", "bswap32", "3");
        decodeFunc(fos, "BSWAP", "3cc", "bswap32", "4");
        decodeFunc(fos, "BSWAP", "3cd", "bswap32", "5");
        decodeFunc(fos, "BSWAP", "3ce", "bswap32", "6");
        decodeFunc(fos, "BSWAP", "3cf", "bswap32", "7");
    }

    public void decodeFuncRegData(FileOutputStream fos, String asm, String comment, String base, String func, int reg, int bits) throws IOException {
        out(fos, "// "+comment);
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = "+func+";");
        out(fos, "    data->op->r1 = "+reg+";");
        decodeData(fos, bits);
        out(fos, "    LOG_OP2(\""+asm+"\", R"+bits+"(data->op->r1), itoa(data->op->data1, tmp, 16));");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void decodeFuncData(FileOutputStream fos, String asm, String comment, String base, String func, int bits) throws IOException {
        out(fos, "// "+comment);
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = "+func+";");
        decodeData(fos, bits);
        out(fos, "    LOG_OP1(\""+asm+"\", itoa(data->op->data1, tmp, 16));");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void decodeFuncDataEndOfBlock(FileOutputStream fos, String asm, String comment, String base, String func, int bits) throws IOException {
        out(fos, "// "+comment);
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = "+func+";");
        decodeData(fos, bits);
        out(fos, "    LOG_OP1(\""+asm+"\", itoa(data->op->data1, tmp, 16));");
        out(fos, "    FINISH_OP(data);");
        out(fos, "}");
    }

    public void decodeJump(FileOutputStream fos, String base, String c, int bits) throws IOException {
        out(fos, "// J"+c);
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = jump"+c+";");
        decodeData(fos, bits);
        out(fos, "    LOG_OP1(\"J"+c+"\", itoa((int)data->op->data1, tmp, 10));");
        out(fos, "    FINISH_OP(data);");
        out(fos, "}");
    }

    public void decodeGroup3(FileOutputStream fos, String comment, String base, int bits) throws IOException {
        out(fos, "// "+comment);
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    U8 rm = FETCH8(data);");
        out(fos, "    data->op->subInst = G(rm);");
        out(fos, "    switch (G(rm)) {");
        out(fos, "    case 0x00:");
        out(fos, "    case 0x01:");
        out(fos, "        DECODE_E(test"+bits+"_reg, test"+bits+"_mem16, test"+bits+"_mem32);");
        out(fos, "        data->op->data1 = FETCH"+bits+"(data);");
        out(fos, "        LOG_E"+bits+"C(\"TEST\", rm, data);");
        out(fos, "        break;");
        out(fos, "    case 0x02:");
        out(fos, "        DECODE_E(not"+bits+"_reg, not"+bits+"_mem16, not"+bits+"_mem32);");
        out(fos, "        LOG_E"+bits+"(\"NOT\", rm, data);");
        out(fos, "        break;");
        out(fos, "    case 0x03:");
        out(fos, "        DECODE_E(neg"+bits+"_reg, neg"+bits+"_mem16, neg"+bits+"_mem32);");
        out(fos, "        LOG_E"+bits+"(\"NEG\", rm, data);");
        out(fos, "        break;");
        out(fos, "    case 0x04:");
        out(fos, "        DECODE_E(mul"+bits+"_reg, mul"+bits+"_mem16, mul"+bits+"_mem32);");
        out(fos, "        LOG_E"+bits+"(\"MUL\", rm, data);");
        out(fos, "        break;");
        out(fos, "    case 0x05:");
        out(fos, "        DECODE_E(imul"+bits+"_reg, imul"+bits+"_mem16, imul"+bits+"_mem32);");
        out(fos, "        LOG_E"+bits+"(\"IMUL\", rm, data);");
        out(fos, "        break;");
        out(fos, "    case 0x06:");
        out(fos, "        DECODE_E(div"+bits+"_reg, div"+bits+"_mem16, div"+bits+"_mem32);");
        out(fos, "        LOG_E"+bits+"(\"DIV\", rm, data);");
        out(fos, "        break;");
        out(fos, "    case 0x07:");
        out(fos, "        DECODE_E(idiv"+bits+"_reg, idiv"+bits+"_mem16, idiv"+bits+"_mem32);");
        out(fos, "        LOG_E"+bits+"(\"IDIV\", rm, data);");
        out(fos, "        break;");
        out(fos, "    }");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void decodeE(FileOutputStream fos, String asm, String comment, String base, String func, int data, int bits) throws IOException {
        out(fos, "// "+comment.toUpperCase());
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    U8 rm = FETCH8(data);");
        out(fos, "    DECODE_E("+func+"_reg, "+func+"_mem16, "+func+"_mem32);");
        decodeData(fos, data);
        if (data!=0)
            out(fos, "    LOG_E"+bits+"C(\""+asm+"\", rm, data);");
        else
            out(fos, "    LOG_E"+bits+"(\""+asm+"\", rm, data);");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void decodePushSeg(FileOutputStream fos, String base, String seg) throws IOException {
        out(fos, "// PUSH "+seg.toUpperCase());
        out(fos, "void decode0"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = pushSeg16;");
        out(fos, "    data->op->r1 = "+seg.toUpperCase()+";");
        out(fos, "    LOG_OP1(\"PUSH\", EABASE(data->op->r1));");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
        out(fos, "// PUSH "+seg.toUpperCase());
        out(fos, "void decode2"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = pushSeg32;");
        out(fos, "    data->op->r1 = "+seg.toUpperCase()+";");
        out(fos, "    LOG_OP1(\"PUSH\", EABASE(data->op->r1));");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void decodePopSeg(FileOutputStream fos, String base, String seg) throws IOException {
        out(fos, "// POP "+seg.toUpperCase());
        out(fos, "void decode0"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = popSeg16;");
        out(fos, "    data->op->r1 = "+seg.toUpperCase()+";");
        out(fos, "    LOG_OP1(\"POP\", EABASE(data->op->r1));");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
        out(fos, "// POP "+seg.toUpperCase());
        out(fos, "void decode2"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = popSeg32;");
        out(fos, "    data->op->r1 = "+seg.toUpperCase()+";");
        out(fos, "    LOG_OP1(\"POP\", EABASE(data->op->r1));");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void decodeSeg(FileOutputStream fos, String base, String seg) throws IOException {
        out(fos, "// SEG "+seg.toUpperCase());
        out(fos, "void decode0"+base+"(struct DecodeData* data) {");
        out(fos, "    data->ds = "+seg.toUpperCase()+";");
        out(fos, "    data->ss = "+seg.toUpperCase()+";");
        out(fos, "    RESTART_OP(data);");
        out(fos, "}");
    }

    public void decodeFuncEAdata(FileOutputStream fos, String comment, String base, String func, String log) throws IOException {
        out(fos, "// "+comment);
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = "+func+";");
        out(fos, "    if (data->ea16) {");
        out(fos, "        data->op->data1 = FETCH16(data);");
        out(fos, "    } else {");
        out(fos, "        data->op->data1 = FETCH32(data);");
        out(fos, "    }");
        out(fos, "    data->op->base = data->ds;");
        out(fos, "    "+log);
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void decodeFuncEndOfBlock(FileOutputStream fos, String base, String func, String asm) throws IOException {
        out(fos, "// "+func.toUpperCase());
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = "+func+";");
        out(fos, "    LOG_OP(\""+asm+"\");");
        out(fos, "    FINISH_OP(data);");
        out(fos, "}");
    }

    public void decodeFunc(FileOutputStream fos, String comment, String base, String func, int reg1, int reg2) throws IOException {
        out(fos, "// "+comment);
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = "+func+";");
        out(fos, "    data->op->r1 = "+reg1+";");
        out(fos, "    data->op->r2 = "+reg2+";");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void generate() {

    }
}

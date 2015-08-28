package x86;

import java.io.FileOutputStream;
import java.io.IOException;

public class Conditions extends Base {
    public void decode(FileOutputStream fos) throws IOException {
        decodeCmov(fos, "140", "O", 16);
        decodeCmov(fos, "340", "O", 32);
        decodeCmov(fos, "141", "NO", 16);
        decodeCmov(fos, "341", "NO", 32);
        decodeCmov(fos, "142", "B", 16);
        decodeCmov(fos, "342", "B", 32);
        decodeCmov(fos, "143", "NB", 16);
        decodeCmov(fos, "343", "NB", 32);
        decodeCmov(fos, "144", "Z", 16);
        decodeCmov(fos, "344", "Z", 32);
        decodeCmov(fos, "145", "NZ", 16);
        decodeCmov(fos, "345", "NZ", 32);
        decodeCmov(fos, "146", "BE", 16);
        decodeCmov(fos, "346", "BE", 32);
        decodeCmov(fos, "147", "NBE", 16);
        decodeCmov(fos, "347", "NBE", 32);
        decodeCmov(fos, "148", "S", 16);
        decodeCmov(fos, "348", "S", 32);
        decodeCmov(fos, "149", "NS", 16);
        decodeCmov(fos, "349", "NS", 32);
        decodeCmov(fos, "14a", "P", 16);
        decodeCmov(fos, "34a", "P", 32);
        decodeCmov(fos, "14b", "NP", 16);
        decodeCmov(fos, "34b", "NP", 32);
        decodeCmov(fos, "14c", "L", 16);
        decodeCmov(fos, "34c", "L", 32);
        decodeCmov(fos, "14d", "NL", 16);
        decodeCmov(fos, "34d", "NL", 32);
        decodeCmov(fos, "14e", "LE", 16);
        decodeCmov(fos, "34e", "LE", 32);
        decodeCmov(fos, "14f", "NLE", 16);
        decodeCmov(fos, "34f", "NLE", 32);
    }

    public void decodeCmov(FileOutputStream fos, String inst, String c, int bits) throws IOException {
        out(fos, "// CMOV"+c);
        out(fos, "void decode"+inst+"(struct DecodeData* data) {");
        out(fos, "    U8 rm = FETCH8(data);");
        out(fos, "    if (rm>=0xC0) {");
        out(fos, "        data->op->func = cmov"+c+"_"+bits+"_reg;");
        out(fos, "        data->op->r1 = G(rm);");
        out(fos, "        data->op->r2 = E(rm);");
        out(fos, "        LOG_OP2(\"CMOV\", R"+bits+"(data->op->r1), R"+bits+"(data->op->r2));");
        out(fos, "    } else if (data->ea16) {");
        out(fos, "        data->op->func = cmov"+c+"_"+bits+"_mem16;");
        out(fos, "        data->op->r1 = G(rm);");
        out(fos, "        decodeEa16(data, rm);");
        out(fos, "        LOG_OP2(\"CMOV\", R"+bits+"(data->op->r1), M"+bits+"(data, rm, data->op));");
        out(fos, "    } else {");
        out(fos, "        data->op->func = cmov"+c+"_"+bits+"_mem32;");
        out(fos, "        data->op->r1 = G(rm);");
        out(fos, "        decodeEa32(data, rm);");
        out(fos, "        LOG_OP2(\"CMOV\", R"+bits+"(data->op->r1), M"+bits+"(data, rm, data->op));");
        out(fos, "    }");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void generate() {
        try {
            FileOutputStream fos = new FileOutputStream("conditions.h");
            condition(fos, "cmovO", "getOF(cpu)");
            condition(fos, "cmovNO", "!getOF(cpu)");
            condition(fos, "cmovB", "getCF(cpu)");
            condition(fos, "cmovNB", "!getCF(cpu)");
            condition(fos, "cmovZ", "getZF(cpu)");
            condition(fos, "cmovNZ", "!getZF(cpu)");
            condition(fos, "cmovBE", "getZF(cpu) || getCF(cpu)");
            condition(fos, "cmovNBE", "!getZF(cpu) && !getCF(cpu)");
            condition(fos, "cmovS", "getSF(cpu)");
            condition(fos, "cmovNS", "!getSF(cpu)");
            condition(fos, "cmovP", "getPF(cpu)");
            condition(fos, "cmovNP", "!getPF(cpu)");
            condition(fos, "cmovL", "getSF(cpu)!=getOF(cpu)");
            condition(fos, "cmovNL", "getSF(cpu)==getOF(cpu)");
            condition(fos, "cmovLE", "getZF(cpu) || getSF(cpu)!=getOF(cpu)");
            condition(fos, "cmovNLE", "!getZF(cpu) && getSF(cpu)==getOF(cpu)");
            fos.close();
            fos = null;
        } catch (IOException e) {

        }
    }

    public void gensrc(FileOutputStream fos) throws IOException {
        condsrc(fos, "140", "O", "0", 16);
        condsrc(fos, "340", "O", "0", 32);
        condsrc(fos, "141", "NO", "1", 16);
        condsrc(fos, "341", "NO", "1", 32);
        condsrc(fos, "142", "B", "2", 16);
        condsrc(fos, "342", "B", "2", 32);
        condsrc(fos, "143", "NB", "3", 16);
        condsrc(fos, "343", "NB", "3", 32);
        condsrc(fos, "144", "Z", "4", 16);
        condsrc(fos, "344", "Z", "4", 32);
        condsrc(fos, "145", "NZ", "5", 16);
        condsrc(fos, "345", "NZ", "5", 32);
        condsrc(fos, "146", "BE", "6", 16);
        condsrc(fos, "346", "BE", "6", 32);
        condsrc(fos, "147", "NBE", "7", 16);
        condsrc(fos, "347", "NBE", "7", 32);
        condsrc(fos, "148", "S", "8", 16);
        condsrc(fos, "348", "S", "8", 32);
        condsrc(fos, "149", "NS", "9", 16);
        condsrc(fos, "349", "NS", "9", 32);
        condsrc(fos, "14a", "P", "10", 16);
        condsrc(fos, "34a", "P", "10", 32);
        condsrc(fos, "14b", "NP",  "11", 16);
        condsrc(fos, "34b", "NP",  "11", 32);
        condsrc(fos, "14c", "L", "12", 16);
        condsrc(fos, "34c", "L", "12", 32);
        condsrc(fos, "14d", "NL", "13", 16);
        condsrc(fos, "34d", "NL", "13", 32);
        condsrc(fos, "14e", "LE", "14", 16);
        condsrc(fos, "34e", "LE", "14", 32);
        condsrc(fos, "14f", "NLE", "15", 16);
        condsrc(fos, "34f", "NLE", "15", 32);
    }

    public void condsrc(FileOutputStream fos, String inst, String c, String cond, int bits) throws IOException {
        String width = "w";
        if (bits==32)
            width = "d";

        out(fos, "void OPCALL cmov"+c+"_"+bits+"_reg(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL cmov"+c+"_"+bits+"_mem16(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL cmov"+c+"_"+bits+"_mem32(struct CPU* cpu, struct Op* op);");
        out(fos, "void gen"+inst+"(struct GenData* data, struct Op* op) {");
        out(fos, "    if (op->func == cmov"+c+"_"+bits+"_reg) {");
        out(fos, "        out(data, \"if (\");");
        out(fos, "        out(data, getCondition(data, "+cond+"));");
        out(fos, "        out(data, \") {\");");
        out(fos, "        out(data, r"+bits+"(op->r1));");
        out(fos, "        out(data, \" = \");");
        out(fos, "        out(data, r"+bits+"(op->r2));");
        out(fos, "        out(data, \";} CYCLES(1);\");");
        out(fos, "    } else if (op->func == cmov"+c+"_"+bits+"_mem16) {");
        out(fos, "        out(data, \"if (\");");
        out(fos, "        out(data, getCondition(data, "+cond+"));");
        out(fos, "        out(data, \") {\");");
        out(fos, "        out(data, r"+bits+"(op->r1));");
        out(fos, "        out(data, \" = read"+width+"(cpu->memory, \");");
        out(fos, "        out(data, getEaa16(op));");
        out(fos, "        out(data, \");} CYCLES(1);\");");
        out(fos, "    } else if (op->func == cmov"+c+"_"+bits+"_mem32) {");
        out(fos, "        out(data, \"if (\");");
        out(fos, "        out(data, getCondition(data, "+cond+"));");
        out(fos, "        out(data, \") {\");");
        out(fos, "        out(data, r"+bits+"(op->r1));");
        out(fos, "        out(data, \" = read"+width+"(cpu->memory, \");");
        out(fos, "        out(data, getEaa32(op));");
        out(fos, "        out(data, \");} CYCLES(1);\");");
        out(fos, "    } else {");
        out(fos, "        kpanic(\"gen"+inst+"\");");
        out(fos, "    }");
        out(fos, "}");
    }

    public void condition(FileOutputStream fos, String name, String condition) throws IOException {
        out(fos, "void OPCALL "+name+"_16_reg(struct CPU* cpu, struct Op* op) {");
        out(fos, "    if ("+condition+") {");
        out(fos, "        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;");
        out(fos, "    }");
        out(fos, "    CYCLES(1);");
        out(fos, "    NEXT();");
        out(fos, "}");
        out(fos, "void OPCALL "+name+"_16_mem16(struct CPU* cpu, struct Op* op) {");
        out(fos, "    if ("+condition+") {");
        out(fos, "        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));");
        out(fos, "    }");
        out(fos, "    CYCLES(1);");
        out(fos, "    NEXT();");
        out(fos, "}");
        out(fos, "void OPCALL "+name+"_16_mem32(struct CPU* cpu, struct Op* op) {");
        out(fos, "    if ("+condition+") {");
        out(fos, "        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));");
        out(fos, "    }");
        out(fos, "    CYCLES(1);");
        out(fos, "    NEXT();");
        out(fos, "}");
        out(fos, "void OPCALL "+name+"_32_reg(struct CPU* cpu, struct Op* op) {");
        out(fos, "    if ("+condition+") {");
        out(fos, "        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;");
        out(fos, "    }");
        out(fos, "    CYCLES(1);");
        out(fos, "    NEXT();");
        out(fos, "}");
        out(fos, "void OPCALL "+name+"_32_mem16(struct CPU* cpu, struct Op* op) {");
        out(fos, "    if ("+condition+") {");
        out(fos, "        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));");
        out(fos, "    }");
        out(fos, "    CYCLES(1);");
        out(fos, "    NEXT();");
        out(fos, "}");
        out(fos, "void OPCALL "+name+"_32_mem32(struct CPU* cpu, struct Op* op) {");
        out(fos, "    if ("+condition+") {");
        out(fos, "        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));");
        out(fos, "    }");
        out(fos, "    CYCLES(1);");
        out(fos, "    NEXT();");
        out(fos, "}");
    }
}

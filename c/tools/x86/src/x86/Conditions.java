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

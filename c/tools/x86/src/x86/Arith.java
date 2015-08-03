package x86;

import java.io.FileOutputStream;
import java.io.IOException;

public class Arith extends Base {
    public void decode(FileOutputStream fos) throws IOException {
        decodeArith(fos, 0, "add");
        decodeArith(fos, 8, "or");
        decodeArith(fos, 0x10, "adc");
        decodeArith(fos, 0x18, "sbb");
        decodeArith(fos, 0x20, "and");
        decodeArith(fos, 0x28, "sub");
        decodeArith(fos, 0x30, "xor");
        decodeArith(fos, 0x38, "cmp");
    }

    public void gensrc(FileOutputStream fos) throws IOException {
        String helpers = "void genArithRR(const char* op, const char* flags, const char* bits, const char* r1, const char* r2, U32 useResult, U32 useCF, const char* cycles) {\n" +
                "    out(\"cpu->dst.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = \");\n" +
                "    out(r1);\n" +
                "    out(\"; cpu->src.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = \");\n" +
                "    out(r2);\n" +
                "    out(\"; cpu->result.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = cpu->dst.u\");\n" +
                "    out(bits);\n" +
                "    out(\" \");\n" +
                "    out(op);\n" +
                "    out(\" cpu->src.u\");\n" +
                "    out(bits);\n" +
                "    if (useCF) {\n" +
                "        out(\" \");\n" +
                "        out(op);\n" +
                "        out(\" \");\n" +
                "        out(\"cpu->oldcf\");\n" +
                "        out(\"; cpu->oldcf = getCF(cpu)\");\n" +
                "    }\n" +
                "    out(\"; cpu->lazyFlags = \");\n" +
                "    out(flags);\n" +
                "    if (useResult) {\n" +
                "        out(\";\");\n" +
                "        out(r1);\n" +
                "        out(\" = cpu->result.u\");\n" +
                "        out(bits);        \n" +
                "    }\n" +
                "    out(\";CYCLES(\");\n" +
                "    out(cycles);\n" +
                "    out(\");\");\n" +
                "}\n" +
                "\n" +
                "void genArithRR_noflags(const char* op, const char* bits, const char* r1, const char* r2, U32 useCF, const char* cycles) {\n" +
                "    out(r1);\n" +
                "    out(\" = \");\n" +
                "    out(r1);\n" +
                "    out(\" \");\n" +
                "    out(op);\n" +
                "    out(\" \");\n" +
                "    out(r2);\n" +
                "    if (useCF) {\n" +
                "        out(\" \");\n" +
                "        out(op);\n" +
                "        out(\" \");\n" +
                "        out(\"cpu->oldcf\");\n" +
                "    }\n" +
                "    out(\";CYCLES(\");\n" +
                "    out(cycles);\n" +
                "    out(\");\");\n" +
                "}\n" +
                "\n" +
                "void genArithER(const char* op, const char* flags, const char* bits, const char* address, const char* memWidth, const char* r1, U32 useResult, U32 useCF, const char* cycles) {\n" +
                "    out(\"eaa = \");\n" +
                "    out(address);\n" +
                "    out(\"; cpu->dst.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = read\");\n" +
                "    out(memWidth);\n" +
                "    out(\"(cpu->memory, eaa); cpu->src.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = \");\n" +
                "    out(r1);\n" +
                "    out(\"; cpu->result.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = cpu->dst.u\");\n" +
                "    out(bits);\n" +
                "    out(\" \");\n" +
                "    out(op);\n" +
                "    out(\" cpu->src.u\");\n" +
                "    out(bits);\n" +
                "    if (useCF) {\n" +
                "        out(\" \");\n" +
                "        out(op);\n" +
                "        out(\" \");\n" +
                "        out(\"cpu->oldcf\");\n" +
                "        out(\"; cpu->oldcf = getCF(cpu)\");\n" +
                "    }\n" +
                "    out(\"; cpu->lazyFlags = \");\n" +
                "    out(flags);\n" +
                "    if (useResult) {\n" +
                "        out(\"; write\");\n" +
                "        out(memWidth);\n" +
                "        out(\"(cpu->memory, eaa,  cpu->result.u\");\n" +
                "        out(bits);\n" +
                "        out(\")\");\n" +
                "    }\n" +
                "    out(\";CYCLES(\");\n" +
                "    out(cycles);\n" +
                "    out(\");\");\n" +
                "}\n" +
                "\n" +
                "void genArithER_noflags(const char* op, const char* bits, const char* address, const char* memWidth, const char* r1, U32 useCF, const char* cycles) {\n" +
                "    out(\"eaa = \");\n" +
                "    out(address);\n" +
                "    out(\"; write\");\n" +
                "    out(memWidth);\n" +
                "    out(\"(cpu->memory, eaa, read\");\n" +
                "    out(memWidth);\n" +
                "    out(\"(cpu->memory, eaa) \");\n" +
                "    out(op);\n" +
                "    out(\" \");\n" +
                "    out(r1);\n" +
                "    if (useCF) {\n" +
                "        out(\" \");\n" +
                "        out(op);\n" +
                "        out(\" \");\n" +
                "        out(\"cpu->oldcf\");\n" +
                "    }\n" +
                "    out(\";CYCLES(\");\n" +
                "    out(cycles);\n" +
                "    out(\");\");\n" +
                "}\n" +
                "\n" +
                "void genArithRE(const char* op, const char* flags, const char* bits, const char* address, const char* memWidth, const char* r1, U32 useResult, U32 useCF, const char* cycles) {\n" +
                "    out(\"cpu->dst.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = \");\n" +
                "    out(r1);\n" +
                "    out(\"; cpu->src.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = read\");\n" +
                "    out(memWidth);\n" +
                "    out(\"(cpu->memory, \");\n" +
                "    out(address);\n" +
                "    out(\"); cpu->result.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = cpu->dst.u\");\n" +
                "    out(bits);\n" +
                "    out(\" \");\n" +
                "    out(op);\n" +
                "    out(\" cpu->src.u\");\n" +
                "    out(bits);\n" +
                "    if (useCF) {\n" +
                "        out(\" \");\n" +
                "        out(op);\n" +
                "        out(\" \");\n" +
                "        out(\"cpu->oldcf\");\n" +
                "        out(\"; cpu->oldcf = getCF(cpu)\");\n" +
                "    }\n" +
                "    out(\"; cpu->lazyFlags = \");\n" +
                "    out(flags);\n" +
                "    if (useResult) {\n" +
                "        out(\"; \");\n" +
                "        out(r1);\n" +
                "        out(\" = cpu->result.u\");\n" +
                "        out(bits);\n" +
                "    }\n" +
                "    out(\";CYCLES(\");\n" +
                "    out(cycles);\n" +
                "    out(\");\");\n" +
                "}\n" +
                "\n" +
                "void genArithRE_noflags(const char* op, const char* bits, const char* address, const char* memWidth, const char* r1, U32 useCF, const char* cycles) {\n" +
                "    out(r1);\n" +
                "    out(\" = \");\n" +
                "    out(r1);\n" +
                "    out(\" \");\n" +
                "    out(op);\n" +
                "    out(\" read\");\n" +
                "    out(memWidth);\n" +
                "    out(\"(cpu->memory, \");\n" +
                "    out(address);\n" +
                "    out(\")\");\n" +
                "    if (useCF) {\n" +
                "        out(\" \");\n" +
                "        out(op);\n" +
                "        out(\" \");\n" +
                "        out(\"cpu->oldcf\");\n" +
                "    }\n" +
                "    out(\";CYCLES(\");\n" +
                "    out(cycles);\n" +
                "    out(\");\");\n" +
                "}\n" +
                "\n" +
                "void genArithR(const char* op, const char* flags, const char* bits, const char* r1, unsigned int value, U32 useResult, U32 useCF, const char* cycles) {\n" +
                "    char tmp[16];\n" +
                "\n" +
                "    out(\"cpu->dst.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = \");\n" +
                "    out(r1);\n" +
                "    out(\"; cpu->src.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = 0x\");\n" +
                "    itoa(value, tmp, 16);\n" +
                "    out(tmp);\n" +
                "    out(\"; cpu->result.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = cpu->dst.u\");\n" +
                "    out(bits);\n" +
                "    out(\" \");\n" +
                "    out(op);\n" +
                "    out(\" cpu->src.u\");\n" +
                "    out(bits);\n" +
                "    if (useCF) {\n" +
                "        out(\" \");\n" +
                "        out(op);\n" +
                "        out(\" \");\n" +
                "        out(\"cpu->oldcf\");\n" +
                "        out(\"; cpu->oldcf = getCF(cpu)\");\n" +
                "    }\n" +
                "    out(\"; cpu->lazyFlags = \");\n" +
                "    out(flags);\n" +
                "    if (useResult) {\n" +
                "        out(\"; \");\n" +
                "        out(r1);\n" +
                "        out(\" = cpu->result.u\");\n" +
                "        out(bits);\n" +
                "    }\n" +
                "    out(\";CYCLES(\");\n" +
                "    out(cycles);\n" +
                "    out(\");\");\n" +
                "}\n" +
                "\n" +
                "void genArithR_noflags(const char* op, const char* bits, const char* r1, unsigned int value, U32 useCF, const char* cycles) {\n" +
                "    char tmp[16];\n" +
                "\n" +
                "    out(r1);\n" +
                "    out(\" = \");\n" +
                "    out(r1);\n" +
                "    out(\" \");\n" +
                "    out(op);\n" +
                "    out(\" 0x\");\n" +
                "    itoa(value, tmp, 16);\n" +
                "    out(tmp);\n" +
                "    if (useCF) {\n" +
                "        out(\" \");\n" +
                "        out(op);\n" +
                "        out(\" \");\n" +
                "        out(\"cpu->oldcf\");\n" +
                "    }\n" +
                "    out(\";CYCLES(\");\n" +
                "    out(cycles);\n" +
                "    out(\");\");\n" +
                "}\n" +
                "\n" +
                "void genArithE(const char* op, const char* flags, const char* bits, const char* address, const char* memWidth, unsigned int value, U32 useResult, U32 useCF, const char* cycles) {\n" +
                "    char tmp[16];\n" +
                "\n" +
                "    out(\"eaa = \");\n" +
                "    out(address);\n" +
                "    out(\"; cpu->dst.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = read\");\n" +
                "    out(memWidth);\n" +
                "    out(\"(cpu->memory, eaa); cpu->src.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = 0x\");\n" +
                "    itoa(value, tmp, 16);\n" +
                "    out(tmp);\n" +
                "    out(\"; cpu->result.u\");\n" +
                "    out(bits);\n" +
                "    out(\" = cpu->dst.u\");\n" +
                "    out(bits);\n" +
                "    out(\" \");\n" +
                "    out(op);\n" +
                "    out(\" cpu->src.u\");\n" +
                "    out(bits);\n" +
                "    if (useCF) {\n" +
                "        out(\" \");\n" +
                "        out(op);\n" +
                "        out(\" \");\n" +
                "        out(\"cpu->oldcf\");\n" +
                "        out(\"; cpu->oldcf = getCF(cpu)\");\n" +
                "    }\n" +
                "    out(\"; cpu->lazyFlags = \");\n" +
                "    out(flags);\n" +
                "    if (useResult) {\n" +
                "        out(\"; write\");\n" +
                "        out(memWidth);\n" +
                "        out(\"(cpu->memory, eaa,  cpu->result.u\");\n" +
                "        out(bits);\n" +
                "        out (\")\");\n" +
                "    }\n" +
                "    out(\";CYCLES(\");\n" +
                "    out(cycles);\n" +
                "    out(\");\");\n" +
                "}\n" +
                "\n" +
                "void genArithE_noflags(const char* op, const char* bits, const char* address, const char* memWidth, unsigned int value, U32 useCF, const char* cycles) {\n" +
                "    char tmp[16];\n" +
                "\n" +
                "    out(\"eaa = \");\n" +
                "    out(address);\n" +
                "    out(\"; write\");\n" +
                "    out(memWidth);\n" +
                "    out(\"(cpu->memory, eaa, read\");\n" +
                "    out(memWidth);\n" +
                "    out(\"(cpu->memory, eaa) \");\n" +
                "    out(op);\n" +
                "    out(\" 0x\");\n" +
                "    itoa(value, tmp, 16);\n" +
                "    out(tmp);\n" +
                "    if (useCF) {\n" +
                "        out(\" \");\n" +
                "        out(op);\n" +
                "        out(\" \");\n" +
                "        out(\"cpu->oldcf\");\n" +
                "    }\n" +
                "    out(\";CYCLES(\");\n" +
                "    out(cycles);\n" +
                "    out(\");\");\n" +
                "}";
        out(fos, helpers);

        srcArith(fos, 0, "add", "+", true, false, "1", "3", "2", "1", "3");
        srcArith(fos, 8, "or", "|", true, false, "1", "3", "2", "1", "3");
        srcArith(fos, 0x10, "adc", "+", true, true, "1", "3", "2", "1", "3");
        srcArith(fos, 0x18, "sbb", "-", true, true, "1", "3", "2", "1", "3");
        srcArith(fos, 0x20, "and", "&", true, false, "1", "3", "2", "1", "3");
        srcArith(fos, 0x28, "sub", "-", true, false, "1", "3", "2", "1", "3");
        srcArith(fos, 0x30, "xor", "^", true, false, "1", "2", "2", "1", "2");
        srcArith(fos, 0x38, "cmp", "-", false, false, "1", "2", "2", "1", "2");
    }

    public void decode_noflags(FileOutputStream fos) throws IOException {
        generateAll(fos, false);
        decodeArith_noflags(fos, 0, "add");
        decodeArith_noflags(fos, 8, "or");
        decodeArith_noflags(fos, 0x10, "adc");
        decodeArith_noflags(fos, 0x18, "sbb");
        decodeArith_noflags(fos, 0x20, "and");
        decodeArith_noflags(fos, 0x28, "sub");
        decodeArith_noflags(fos, 0x30, "xor");
    }

    public void generate() {
        try {
            FileOutputStream fos = new FileOutputStream("arith.h");
            generateAll(fos, true);
            fos.close();
            fos = null;
        } catch (IOException e) {

        }
    }

    public void generateAll(FileOutputStream fos, boolean flags) throws IOException {
        arith(fos, "add", "+", false, true, "1", "3", "2", "1", "3", flags);
        arith(fos, "or", "|", false, true, "1", "3", "2", "1", "3", flags);
        arith(fos, "adc", "+", true, true, "1", "3", "2", "1", "3", flags);
        arith(fos, "sbb", "-", true, true, "1", "3", "2", "1", "3", flags);
        arith(fos, "and", "&", false, true, "1", "3", "2", "1", "3", flags);
        arith(fos, "sub", "-", false, true, "1", "3", "2", "1", "3", flags);
        arith(fos, "xor", "^", false, true, "1", "3", "2", "1", "3", flags);
        arith(fos, "cmp", "-", false, false, "1", "2", "2", "1", "2", flags);
        arith(fos, "test", "&", false, false, "1", "2", "2", "1", "2", flags);
    }

    public void srcArith(FileOutputStream fos, int base, String name, String op, boolean useResult, boolean useCF, String rrCycles, String mrCycles, String rmCycles, String rdCycles, String mdCycles) throws IOException {
        String h = String.valueOf(base/16);
        int o = base % 16;
        srcArithEG(fos, "0"+h+Integer.toHexString(o), name, 8, op, "b", useResult, useCF, rrCycles, mrCycles);
        srcArithEG(fos, "0"+h+Integer.toHexString(o+1), name, 16, op, "w", useResult, useCF, rrCycles, mrCycles);
        srcArithEG(fos, "2"+h+Integer.toHexString(o+1), name, 32, op, "d", useResult, useCF, rrCycles, mrCycles);

        srcArithGE(fos, "0"+h+Integer.toHexString(o+2), name, 8, op, "b", useResult, useCF, rrCycles, rmCycles);
        srcArithGE(fos, "0"+h+Integer.toHexString(o+3), name, 16, op, "w", useResult, useCF, rrCycles, rmCycles);
        srcArithGE(fos, "2"+h+Integer.toHexString(o+3), name, 32, op, "d", useResult, useCF, rrCycles, rmCycles);

        srcArithAD(fos, "0"+h+Integer.toHexString(o+4), name, 8, op, "b", useResult, useCF, rdCycles);
        srcArithAD(fos, "0"+h+Integer.toHexString(o+5), name, 16, op, "w", useResult, useCF, rdCycles);
        srcArithAD(fos, "2"+h+Integer.toHexString(o+5), name, 32, op, "d", useResult, useCF, rdCycles);

    }

    public void srcArithEG(FileOutputStream fos, String inst, String name, int bits, String op, String memWidth, boolean useResult, boolean useCF, String rrCycles, String mrCycles) throws IOException {
        out(fos, "void OPCALL "+name+"r"+bits+"r"+bits+"(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+"r"+bits+"r"+bits+"_noflags(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+"e"+bits+"r"+bits+"_16(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+"e"+bits+"r"+bits+"_16_noflags(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+"e"+bits+"r"+bits+"_32(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+"e"+bits+"r"+bits+"_32_noflags(struct CPU* cpu, struct Op* op);");
        out(fos, "void gen"+inst+"(struct Op* op) {");
        out(fos, "    if (op->func=="+name+"r"+bits+"r"+bits+") {");
        out(fos, "        genArithRR(\""+op+"\", \"FLAGS_"+name.toUpperCase()+bits+"\", \""+bits+"\", r"+bits+"(op->r1), r"+bits+"(op->r2), "+(useResult?"1":"0")+", "+(useCF?"1":"0")+",\""+rrCycles+"\");");
        out(fos, "    } else if (op->func=="+name+"r"+bits+"r"+bits+"_noflags) {");
        out(fos, "        genArithRR_noflags(\""+op+"\", \""+bits+"\", r"+bits+"(op->r1), r"+bits+"(op->r2), "+(useCF?"1":"0")+",\""+rrCycles+"\");");
        out(fos, "    } else if (op->func=="+name+"e"+bits+"r"+bits+"_16) {");
        out(fos, "        genArithER(\""+op+"\", \"FLAGS_"+name.toUpperCase()+bits+"\", \""+bits+"\", getEaa16(op), \""+memWidth+"\", r"+bits+"(op->r1), "+(useResult?"1":"0")+", "+(useCF?"1":"0")+",\""+mrCycles+"\");");
        out(fos, "    } else if (op->func=="+name+"e"+bits+"r"+bits+"_16_noflags) {");
        out(fos, "        genArithER_noflags(\""+op+"\", \""+bits+"\", getEaa16(op), \""+memWidth+"\", r"+bits+"(op->r1), "+(useCF?"1":"0")+",\""+mrCycles+"\");");
        out(fos, "    } else if (op->func=="+name+"e"+bits+"r"+bits+"_32) {");
        out(fos, "        genArithER(\"+\", \"FLAGS_"+name.toUpperCase()+bits+"\", \""+bits+"\", getEaa32(op), \""+memWidth+"\", r"+bits+"(op->r1), "+(useResult?"1":"0")+", "+(useCF?"1":"0")+",\""+mrCycles+"\");");
        out(fos, "    } else if (op->func=="+name+"e"+bits+"r"+bits+"_32_noflags) {");
        out(fos, "        genArithER_noflags(\""+op+"\", \""+bits+"\", getEaa32(op), \""+memWidth+"\", r"+bits+"(op->r1), "+(useCF?"1":"0")+",\""+mrCycles+"\");");
        out(fos, "    }");
        out(fos, "}");
    }

    public void srcArithGE(FileOutputStream fos, String inst, String name, int bits, String op, String memWidth, boolean useResult, boolean useCF, String rrCycles, String rmCycles) throws IOException {
        out(fos, "void OPCALL "+name+"r"+bits+"r"+bits+"(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+"r"+bits+"r"+bits+"_noflags(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+"r"+bits+"e"+bits+"_16(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+"r"+bits+"e"+bits+"_16_noflags(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+"r"+bits+"e"+bits+"_32(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+"r"+bits+"e"+bits+"_32_noflags(struct CPU* cpu, struct Op* op);");
        out(fos, "void gen"+inst+"(struct Op* op) {");
        out(fos, "    if (op->func=="+name+"r"+bits+"r"+bits+") {");
        out(fos, "        genArithRR(\""+op+"\", \"FLAGS_"+name.toUpperCase()+bits+"\", \""+bits+"\", r"+bits+"(op->r1), r"+bits+"(op->r2), "+(useResult?"1":"0")+", "+(useCF?"1":"0")+",\""+rrCycles+"\");");
        out(fos, "    } else if (op->func=="+name+"r"+bits+"r"+bits+"_noflags) {");
        out(fos, "        genArithRR_noflags(\""+op+"\", \""+bits+"\", r"+bits+"(op->r1), r"+bits+"(op->r2), "+(useCF?"1":"0")+",\""+rrCycles+"\");");
        out(fos, "    } else if (op->func=="+name+"r"+bits+"e"+bits+"_16) {");
        out(fos, "        genArithRE(\""+op+"\", \"FLAGS_"+name.toUpperCase()+bits+"\", \""+bits+"\", getEaa16(op), \""+memWidth+"\", r"+bits+"(op->r1), "+(useResult?"1":"0")+", "+(useCF?"1":"0")+",\""+rmCycles+"\");");
        out(fos, "    } else if (op->func=="+name+"r"+bits+"e"+bits+"_16_noflags) {");
        out(fos, "        genArithRE_noflags(\""+op+"\", \""+bits+"\", getEaa16(op), \""+memWidth+"\", r"+bits+"(op->r1), "+(useCF?"1":"0")+",\""+rmCycles+"\");");
        out(fos, "    } else if (op->func=="+name+"r"+bits+"e"+bits+"_32) {");
        out(fos, "        genArithRE(\"+\", \"FLAGS_"+name.toUpperCase()+bits+"\", \""+bits+"\", getEaa32(op), \""+memWidth+"\", r"+bits+"(op->r1), "+(useResult?"1":"0")+", "+(useCF?"1":"0")+",\""+rmCycles+"\");");
        out(fos, "    } else if (op->func=="+name+"r"+bits+"e"+bits+"_32_noflags) {");
        out(fos, "        genArithRE_noflags(\""+op+"\", \""+bits+"\", getEaa32(op), \""+memWidth+"\", r"+bits+"(op->r1), "+(useCF?"1":"0")+",\""+rmCycles+"\");");
        out(fos, "    }");
        out(fos, "}");
    }

    public void srcArithAD(FileOutputStream fos, String inst, String name, int bits, String op, String memWidth, boolean useResult, boolean useCF, String rdCycles) throws IOException {
        out(fos, "void OPCALL "+name+bits+"_reg(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+name+bits+"_reg_noflags(struct CPU* cpu, struct Op* op);");
        out(fos, "void gen"+inst+"(struct Op* op) {");
        out(fos, "    if (op->func=="+name+bits+"_reg) {");
        out(fos, "        genArithR(\""+op+"\", \"FLAGS_"+name.toUpperCase()+bits+"\", \""+bits+"\", r"+bits+"(op->r1), op->data1, "+(useResult?"1":"0")+", "+(useCF?"1":"0")+",\""+rdCycles+"\");");
        out(fos, "    } else if (op->func=="+name+bits+"_reg_noflags) {");
        out(fos, "        genArithR_noflags(\""+op+"\", \""+bits+"\", r"+bits+"(op->r1), op->data1, "+(useCF?"1":"0")+",\""+rdCycles+"\");");
        out(fos, "    }");
        out(fos, "}");
    }

    public void decodeArith(FileOutputStream fos, int base, String name) throws IOException {
        String h = String.valueOf(base/16);
        int o = base % 16;
        eg(fos, "0"+h+Integer.toHexString(o), name, 8);
        eg(fos, "0"+h+Integer.toHexString(o+1), name, 16);
        eg(fos, "2"+h+Integer.toHexString(o+1), name, 32);

        ge(fos, "0"+h+Integer.toHexString(o+2), name, 8);
        ge(fos, "0"+h+Integer.toHexString(o+3), name, 16);
        ge(fos, "2"+h+Integer.toHexString(o+3), name, 32);

        ad(fos, "0"+h+Integer.toHexString(o+4), name, 8);
        ad(fos, "0"+h+Integer.toHexString(o+5), name, 16);
        ad(fos, "2"+h+Integer.toHexString(o+5), name, 32);
    }

    public void decodeArith_noflags(FileOutputStream fos, int base, String name) throws IOException {
        String h = String.valueOf(base/16);
        int o = base % 16;
        eg_noflags(fos, "0" + h + Integer.toHexString(o), name, 8);
        eg_noflags(fos, "0" + h + Integer.toHexString(o + 1), name, 16);
        eg_noflags(fos, "2" + h + Integer.toHexString(o + 1), name, 32);

        ge_noflags(fos, "0" + h + Integer.toHexString(o + 2), name, 8);
        ge_noflags(fos, "0" + h + Integer.toHexString(o + 3), name, 16);
        ge_noflags(fos, "2" + h + Integer.toHexString(o + 3), name, 32);

        ad_noflags(fos, "0" + h + Integer.toHexString(o + 4), name, 8);
        ad_noflags(fos, "0" + h + Integer.toHexString(o + 5), name, 16);
        ad_noflags(fos, "2" + h + Integer.toHexString(o + 5), name, 32);
    }

    public void arith(FileOutputStream fos, String name, String op, boolean cf, boolean result, String rrCycles, String mrCycles, String rmCycles, String rdCycles, String mdCycles, boolean flags)  throws IOException {
        arith8(fos, name, op, cf, result, "8", rrCycles, mrCycles, rmCycles, rdCycles, mdCycles, flags);
        arith16(fos, name, op, cf, result, "16", rrCycles, mrCycles, rmCycles, rdCycles, mdCycles, flags);
        arith32(fos, name, op, cf, result, "32", rrCycles, mrCycles, rmCycles, rdCycles, mdCycles, flags);
    }

    public void arith32(FileOutputStream fos, String name, String op, boolean cf, boolean result, String bits, String rrCycles, String mrCycles, String rmCycles, String rdCycles, String mdCycles, boolean flags)  throws IOException {
        arithbase(fos, name + "r32r32", "FLAGS_" + name.toUpperCase() + bits, "cpu->reg[op->r2].u32", "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", op, cf, result, false, false, bits, rrCycles, flags);
        arithbase(fos, name + "e32r32_16", "FLAGS_" + name.toUpperCase() + bits, "cpu->reg[op->r1].u32", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mrCycles, flags);
        arithbase(fos, name + "e32r32_32", "FLAGS_" + name.toUpperCase() + bits, "cpu->reg[op->r1].u32", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mrCycles, flags);
        arithbase(fos, name + "r32e32_16", "FLAGS_" + name.toUpperCase() + bits, "readd(cpu->memory, eaa16(cpu, op))", "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", op, cf, result, false, false, bits, rmCycles, flags);
        arithbase(fos, name + "r32e32_32", "FLAGS_" + name.toUpperCase() + bits, "readd(cpu->memory, eaa32(cpu, op))", "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", op, cf, result, false, false, bits, rmCycles, flags);
        arithbase(fos, name + "32_reg", "FLAGS_" + name.toUpperCase() + bits, "op->data1", "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", op, cf, result, false, false, bits, rdCycles, flags);
        arithbase(fos, name + "32_mem16", "FLAGS_" + name.toUpperCase() + bits, "op->data1", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mdCycles, flags);
        arithbase(fos, name + "32_mem32", "FLAGS_" + name.toUpperCase() + bits, "op->data1", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mdCycles, flags);
    }

    public void arith16(FileOutputStream fos, String name, String op, boolean cf, boolean result, String bits, String rrCycles, String mrCycles, String rmCycles, String rdCycles, String mdCycles, boolean flags)  throws IOException {
        arithbase(fos, name+"r16r16", "FLAGS_"+name.toUpperCase()+bits, "cpu->reg[op->r2].u16", "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", op, cf, result, false, false, bits, rrCycles, flags);
        arithbase(fos, name+"e16r16_16", "FLAGS_"+name.toUpperCase()+bits, "cpu->reg[op->r1].u16", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mrCycles, flags);
        arithbase(fos, name+"e16r16_32", "FLAGS_"+name.toUpperCase()+bits, "cpu->reg[op->r1].u16", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mrCycles, flags);
        arithbase(fos, name+"r16e16_16", "FLAGS_"+name.toUpperCase()+bits, "readw(cpu->memory, eaa16(cpu, op))", "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", op, cf, result, false, false, bits, rmCycles, flags);
        arithbase(fos, name+"r16e16_32", "FLAGS_"+name.toUpperCase()+bits, "readw(cpu->memory, eaa32(cpu, op))", "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", op, cf, result, false, false, bits, rmCycles, flags);
        arithbase(fos, name+"16_reg", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", op, cf, result, false, false, bits, rdCycles, flags);
        arithbase(fos, name+"16_mem16", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mdCycles, flags);
        arithbase(fos, name+"16_mem32", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mdCycles, flags);
    }

    public void arith8(FileOutputStream fos, String name, String op, boolean cf, boolean result, String bits, String rrCycles, String mrCycles, String rmCycles, String rdCycles, String mdCycles, boolean flags)  throws IOException {
        arithbase(fos, name+"r8r8", "FLAGS_"+name.toUpperCase()+bits, "*cpu->reg8[op->r2]", "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", op, cf, result, false, false, bits, rrCycles, flags);
        arithbase(fos, name+"e8r8_16", "FLAGS_"+name.toUpperCase()+bits, "*cpu->reg8[op->r1]", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mrCycles, flags);
        arithbase(fos, name+"e8r8_32", "FLAGS_"+name.toUpperCase()+bits, "*cpu->reg8[op->r1]", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mrCycles, flags);
        arithbase(fos, name+"r8e8_16", "FLAGS_"+name.toUpperCase()+bits, "readb(cpu->memory, eaa16(cpu, op))", "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", op, cf, result, false, false, bits, rmCycles, flags);
        arithbase(fos, name+"r8e8_32", "FLAGS_"+name.toUpperCase()+bits, "readb(cpu->memory, eaa32(cpu, op))", "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", op, cf, result, false, false, bits, rmCycles, flags);
        arithbase(fos, name+"8_reg", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", op, cf, result, false, false, bits, rdCycles, flags);
        arithbase(fos, name+"8_mem16", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, cf, result, true, false, bits, mdCycles, flags);
        arithbase(fos, name+"8_mem32", "FLAGS_"+name.toUpperCase()+bits, "op->data1", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, cf, result, false, true, bits, mdCycles, flags);
    }

    public void arithbase(FileOutputStream fos, String functionName, String flagName, String source, String loadDest, String saveDest1, String saveDest2, String op, boolean cf, boolean result, boolean eaa16, boolean eaa32, String bits, String cycles, boolean flags)  throws IOException {
        if (!flags)
            functionName+="_noflags";
        out(fos, "void OPCALL "+functionName+"(struct CPU* cpu, struct Op* op) {");
        if (eaa16)
            out(fos, "    U32 eaa = eaa16(cpu, op);");
        if (eaa32)
            out(fos, "    U32 eaa = eaa32(cpu, op);");
        if (flags) {
            if (cf)
                out(fos, "    cpu->oldcf = getCF(cpu);");
            out(fos, "    cpu->dst.u" + bits + " = " + loadDest + ";");
            out(fos, "    cpu->src.u" + bits + " = " + source + ";");
            out(fos, "    cpu->result.u" + bits + " = cpu->dst.u" + bits + " " + op + " cpu->src.u" + bits + (cf ? " " + op + " cpu->oldcf;" : ";"));
            out(fos, "    cpu->lazyFlags = " + flagName + ";");
            if (result)
                out(fos, "    " + saveDest1 + " cpu->result.u" + bits + saveDest2 + ";");
        } else {
            out(fos, "    " + saveDest1 + loadDest + " " + op + " " + source + (cf ? " " + op + " getCF(cpu)" : "") + saveDest2 + ";");
        }
        out(fos, "    CYCLES("+cycles+");");
        out(fos, "    NEXT();");
        out(fos, "}");
    }
}

package x86;

import java.io.FileOutputStream;
import java.io.IOException;

public class SetCC extends Base {
    public void decode(FileOutputStream fos) throws IOException {

    }

    public void generate() {
        try {
            FileOutputStream fos = new FileOutputStream("setcc.h");
            cc(fos, "setO", "getOF(cpu)");
            cc(fos, "setNO", "!getOF(cpu)");
            cc(fos, "setB", "getCF(cpu)");
            cc(fos, "setNB", "!getCF(cpu)");
            cc(fos, "setZ", "getZF(cpu)");
            cc(fos, "setNZ", "!getZF(cpu)");
            cc(fos, "setBE", "getZF(cpu) || getCF(cpu)");
            cc(fos, "setNBE", "!getZF(cpu) && !getCF(cpu)");
            cc(fos, "setS", "getSF(cpu)");
            cc(fos, "setNS", "!getSF(cpu)");
            cc(fos, "setP", "getPF(cpu)");
            cc(fos, "setNP", "!getPF(cpu)");
            cc(fos, "setL", "getSF(cpu)!=getOF(cpu)");
            cc(fos, "setNL", "getSF(cpu)==getOF(cpu)");
            cc(fos, "setLE", "getZF(cpu) || getSF(cpu)!=getOF(cpu)");
            cc(fos, "setNLE", "!getZF(cpu) && getSF(cpu)==getOF(cpu)");
            fos.close();
            fos = null;
        } catch (IOException e) {

        }
    }

    public void gensrc(FileOutputStream fos) throws IOException {
        ccsrc(fos, "190", "O", "0");
        ccsrc(fos, "191", "NO", "1");
        ccsrc(fos, "192", "B", "2");
        ccsrc(fos, "193", "NB", "3");
        ccsrc(fos, "194", "Z", "4");
        ccsrc(fos, "195", "NZ", "5");
        ccsrc(fos, "196", "BE", "6");
        ccsrc(fos, "197", "NBE", "7");
        ccsrc(fos, "198", "S", "8");
        ccsrc(fos, "199", "NS", "9");
        ccsrc(fos, "19a", "P", "10");
        ccsrc(fos, "19b", "NP",  "11");
        ccsrc(fos, "19c", "L", "12");
        ccsrc(fos, "19d", "NL", "13");
        ccsrc(fos, "19e", "LE", "14");
        ccsrc(fos, "19f", "NLE", "15");
    }

    public void ccsrc(FileOutputStream fos, String inst, String c, String cond) throws IOException {
        out(fos, "void OPCALL set"+c+"_reg(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL set"+c+"_mem16(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL set"+c+"_mem32(struct CPU* cpu, struct Op* op);");
        out(fos, "void gen"+inst+"(struct Op* op) {");
        out(fos, "    if (op->func == set"+c+"_reg) {");
        out(fos, "        out(\"if (\");");
        out(fos, "        out(getCondition("+cond+"));");
        out(fos, "        out(\") {\");");
        out(fos, "        out(r8(op->r1));");
        out(fos, "        out(\" = 1;} else {\");");
        out(fos, "        out(r8(op->r1));");
        out(fos, "        out(\" = 0;} CYCLES(2);\");");
        out(fos, "    } else if (op->func == set"+c+"_mem16) {");
        out(fos, "        out(\"if (\");");
        out(fos, "        out(getCondition("+cond+"));");
        out(fos, "        out(\") {writeb(cpu->memory, \");");
        out(fos, "        out(getEaa16(op));");
        out(fos, "        out(\", 1);} else {writeb(cpu->memory, \");");
        out(fos, "        out(getEaa16(op));");
        out(fos, "        out(\", 0);} CYCLES(2);\");");
        out(fos, "    } else if (op->func == set"+c+"_mem32) {");
        out(fos, "        out(\"if (\");");
        out(fos, "        out(getCondition("+cond+"));");
        out(fos, "        out(\") {writeb(cpu->memory, \");");
        out(fos, "        out(getEaa32(op));");
        out(fos, "        out(\", 1);} else {writeb(cpu->memory, \");");
        out(fos, "        out(getEaa32(op));");
        out(fos, "        out(\", 0);} CYCLES(2);\");");
        out(fos, "    } else {");
        out(fos, "        kpanic(\"gen"+inst+"\");");
        out(fos, "    }");
        out(fos, "}");
    }

    public void cc(FileOutputStream fos, String name, String condition) throws IOException {
        out(fos, "void OPCALL "+name+"_reg(struct CPU* cpu, struct Op* op) {");
        out(fos, "    if ("+condition+") {");
        out(fos, "        *cpu->reg8[op->r1] = 1;");
        out(fos, "    } else {");
        out(fos, "        *cpu->reg8[op->r1] = 0;");
        out(fos, "    }");
        out(fos, "    CYCLES(2);");
        out(fos, "    NEXT();");
        out(fos, "}");
        out(fos, "void OPCALL "+name+"_mem16(struct CPU* cpu, struct Op* op) {");
        out(fos, "    if ("+condition+") {");
        out(fos, "        writeb(cpu->memory, eaa16(cpu, op), 1);");
        out(fos, "    } else {");
        out(fos, "        writeb(cpu->memory, eaa16(cpu, op), 0);");
        out(fos, "    }");
        out(fos, "    CYCLES(2);");
        out(fos, "    NEXT();");
        out(fos, "}");
        out(fos, "void OPCALL "+name+"_mem32(struct CPU* cpu, struct Op* op) {");
        out(fos, "    if ("+condition+") {");
        out(fos, "        writeb(cpu->memory, eaa32(cpu, op), 1);");
        out(fos, "    } else {");
        out(fos, "        writeb(cpu->memory, eaa32(cpu, op), 0);");
        out(fos, "    }");
        out(fos, "    CYCLES(2);");
        out(fos, "    NEXT();");
        out(fos, "}");
    }
}

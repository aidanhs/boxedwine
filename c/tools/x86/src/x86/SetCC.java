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

package x86;

import java.io.FileOutputStream;
import java.io.IOException;

public class PushPop extends Base {
    public void decode(FileOutputStream fos) throws IOException {
        decodeFunc(fos, "PUSH AX", "050", "pushReg16", 0);
        decodeFunc(fos, "PUSH EAX", "250", "pushReg32", 0);
        decodeFunc(fos, "PUSH CX", "051", "pushReg16", 1);
        decodeFunc(fos, "PUSH ECX", "251", "pushReg32", 1);
        decodeFunc(fos, "PUSH DX", "052", "pushReg16", 2);
        decodeFunc(fos, "PUSH EDX", "252", "pushReg32", 2);
        decodeFunc(fos, "PUSH BX", "053", "pushReg16", 3);
        decodeFunc(fos, "PUSH EBX", "253", "pushReg32", 3);
        decodeFunc(fos, "PUSH SP", "054", "pushReg16", 4);
        decodeFunc(fos, "PUSH ESP", "254", "pushReg32", 4);
        decodeFunc(fos, "PUSH BP", "055", "pushReg16", 5);
        decodeFunc(fos, "PUSH EBP", "255", "pushReg32", 5);
        decodeFunc(fos, "PUSH SI", "056", "pushReg16", 6);
        decodeFunc(fos, "PUSH ESI", "256", "pushReg32", 6);
        decodeFunc(fos, "PUSH DI", "057", "pushReg16", 7);
        decodeFunc(fos, "PUSH EDI", "257", "pushReg32", 7);
        decodeFunc(fos, "PUSH AX", "058", "popReg16", 0);
        decodeFunc(fos, "POP EAX", "258", "popReg32", 0);
        decodeFunc(fos, "POP CX", "059", "popReg16", 1);
        decodeFunc(fos, "POP ECX", "259", "popReg32", 1);
        decodeFunc(fos, "POP DX", "05a", "popReg16", 2);
        decodeFunc(fos, "POP EDX", "25a", "popReg32", 2);
        decodeFunc(fos, "POP BX", "05b", "popReg16", 3);
        decodeFunc(fos, "POP EBX", "25b", "popReg32", 3);
        decodeFunc(fos, "POP SP", "05c", "popReg16", 4);
        decodeFunc(fos, "POP ESP", "25c", "popReg32", 4);
        decodeFunc(fos, "POP BP", "05d", "popReg16", 5);
        decodeFunc(fos, "POP EBP", "25d", "popReg32", 5);
        decodeFunc(fos, "POP SI", "05e", "popReg16", 6);
        decodeFunc(fos, "POP ESI", "25e", "popReg32", 6);
        decodeFunc(fos, "POP DI", "05f", "popReg16", 7);
        decodeFunc(fos, "POP EDI", "25f", "popReg32", 7);
        decodeFunc(fos, "060", "pusha", "PUSHA");
        decodeFunc(fos, "260", "pushad", "PUSHA");
        decodeFunc(fos, "061", "popa", "POPA");
        decodeFunc(fos, "261", "popad", "POPA");
    }

    public void generate() {
        try {
            FileOutputStream fos = new FileOutputStream("pushpop.h");
            pushPopReg(fos, "Reg16", "16");
            pushPopReg(fos, "Reg32", "32");
            fos.close();
            fos = null;
        } catch (IOException e) {

        }
    }

    public void gensrc(FileOutputStream fos) throws IOException {
        genSegSrc(fos, "006", "206", "007", "207", "ES");
        genSegSrc(fos, "00e", "20e", null, null, "CS");
        genSegSrc(fos, "016", "216", "017", "217", "SS");
        genSegSrc(fos, "01e", "21e", "01f", "21f", "DS");
        genSegSrc(fos, "1a0", "3a0", "1a1", "3a1", "FS");
        genSegSrc(fos, "1a8", "3a8", "1a9", "3a9", "GS");
    }

    public void genSegSrc(FileOutputStream fos, String push16, String push32, String pop16, String pop32, String seg) throws IOException {
        out(fos, "void gen"+push16+"(struct GenData* data, struct Op* op) {");
        out(fos, "    out(data, \"push16(cpu, cpu->segValue["+seg+"]);CYCLES(1);\");");
        out(fos, "}");
        out(fos, "void gen"+push32+"(struct GenData* data, struct Op* op) {");
        out(fos, "    out(data, \"push32(cpu, cpu->segValue["+seg+"]);CYCLES(1);\");");
        out(fos, "}");
        if (pop16!=null) {
            out(fos, "void gen" + pop16 + "(struct GenData* data, struct Op* op) {");
            out(fos, "    out(data, \"cpu->segValue[" + seg + "] = pop16(cpu); cpu->segAddress[" + seg + "] = cpu->ldt[cpu->segValue[" + seg + "] >> 3].base_addr;CYCLES(3);\");");
            out(fos, "}");
        }
        if (pop32!=null) {
            out(fos, "void gen" + pop32 + "(struct GenData* data, struct Op* op) {");
            out(fos, "    out(data, \"cpu->segValue[" + seg + "] = pop32(cpu); cpu->segAddress[" + seg + "] = cpu->ldt[cpu->segValue[" + seg + "] >> 3].base_addr;CYCLES(3);\");");
            out(fos, "}");
        }
    }

    public void pushPopReg(FileOutputStream fos, String name, String bits) throws IOException {
        out(fos, "void OPCALL push"+name+"(struct CPU* cpu, struct Op* op){");
        out(fos, "    push"+bits+"(cpu, cpu->reg[op->r1].u"+bits+");");
        out(fos, "    CYCLES(1);");
        out(fos, "    NEXT();");
        out(fos, "}");
        out(fos, "void OPCALL pop"+name+"(struct CPU * cpu, struct Op * op){");
        out(fos, "    cpu->reg[op->r1].u"+bits+" = pop"+bits+"(cpu);");
        out(fos, "    CYCLES(1);");
        out(fos, "    NEXT();");
        out(fos, "}");
    }
}

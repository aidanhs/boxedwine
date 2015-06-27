package x86;

import java.io.FileOutputStream;
import java.io.IOException;

public class PushPop extends Base {
    public void decode(FileOutputStream fos) throws IOException {
        decodeFunc(fos, "PUSH AX", "050", "pushAx", 0);
        decodeFunc(fos, "PUSH EAX", "250", "pushEax", 0);
        decodeFunc(fos, "PUSH CX", "051", "pushCx", 1);
        decodeFunc(fos, "PUSH ECX", "251", "pushEcx", 1);
        decodeFunc(fos, "PUSH DX", "052", "pushDx", 2);
        decodeFunc(fos, "PUSH EDX", "252", "pushEdx", 2);
        decodeFunc(fos, "PUSH BX", "053", "pushBx", 3);
        decodeFunc(fos, "PUSH EBX", "253", "pushEbx", 3);
        decodeFunc(fos, "PUSH SP", "054", "pushSp", 4);
        decodeFunc(fos, "PUSH ESP", "254", "pushEsp", 4);
        decodeFunc(fos, "PUSH BP", "055", "pushBp", 5);
        decodeFunc(fos, "PUSH EBP", "255", "pushEbp", 5);
        decodeFunc(fos, "PUSH SI", "056", "pushSi", 6);
        decodeFunc(fos, "PUSH ESI", "256", "pushEsi", 6);
        decodeFunc(fos, "PUSH DI", "057", "pushDi", 7);
        decodeFunc(fos, "PUSH EDI", "257", "pushEdi", 7);
        decodeFunc(fos, "PUSH AX", "058", "popAx", 0);
        decodeFunc(fos, "POP EAX", "258", "popEax", 0);
        decodeFunc(fos, "POP CX", "059", "popCx", 1);
        decodeFunc(fos, "POP ECX", "259", "popEcx", 1);
        decodeFunc(fos, "POP DX", "05a", "popDx", 2);
        decodeFunc(fos, "POP EDX", "25a", "popEdx", 2);
        decodeFunc(fos, "POP BX", "05b", "popBx", 3);
        decodeFunc(fos, "POP EBX", "25b", "popEbx", 3);
        decodeFunc(fos, "POP SP", "05c", "popSp", 4);
        decodeFunc(fos, "POP ESP", "25c", "popEsp", 4);
        decodeFunc(fos, "POP BP", "05d", "popBp", 5);
        decodeFunc(fos, "POP EBP", "25d", "popEbp", 5);
        decodeFunc(fos, "POP SI", "05e", "popSi", 6);
        decodeFunc(fos, "POP ESI", "25e", "popEsi", 6);
        decodeFunc(fos, "POP DI", "05f", "popDi", 7);
        decodeFunc(fos, "POP EDI", "25f", "popEdi", 7);
        decodeFunc(fos, "060", "pusha", "PUSHA");
        decodeFunc(fos, "260", "pushad", "PUSHA");
        decodeFunc(fos, "061", "popa", "POPA");
        decodeFunc(fos, "261", "popad", "POPA");
    }

    public void generate() {
        try {
            FileOutputStream fos = new FileOutputStream("pushpop.h");
            pushPopReg(fos, "Ax", "16");
            pushPopReg(fos, "Eax", "32");
            pushPopReg(fos, "Cx", "16");
            pushPopReg(fos, "Ecx", "32");
            pushPopReg(fos, "Dx", "16");
            pushPopReg(fos, "Edx", "32");
            pushPopReg(fos, "Bx", "16");
            pushPopReg(fos, "Ebx", "32");
            pushPopReg(fos, "Sp", "16");
            pushPopReg(fos, "Esp", "32");
            pushPopReg(fos, "Bp", "16");
            pushPopReg(fos, "Ebp", "32");
            pushPopReg(fos, "Si", "16");
            pushPopReg(fos, "Esi", "32");
            pushPopReg(fos, "Di", "16");
            pushPopReg(fos, "Edi", "32");
            fos.close();
            fos = null;
        } catch (IOException e) {

        }
    }

    public void pushPopReg(FileOutputStream fos, String name, String bits) throws IOException {
        out(fos, "void OPCALL push"+name+"(struct CPU* cpu, struct Op* op){");
        out(fos, "    push"+bits+"(cpu, "+name.toUpperCase()+");");
        out(fos, "    CYCLES(1);");
        out(fos, "    NEXT();");
        out(fos, "}");
        out(fos, "void OPCALL pop"+name+"(struct CPU * cpu, struct Op * op){");
        out(fos, "    "+name.toUpperCase()+" = pop"+bits+"(cpu);");
        out(fos, "    CYCLES(1);");
        out(fos, "    NEXT();");
        out(fos, "}");
    }
}

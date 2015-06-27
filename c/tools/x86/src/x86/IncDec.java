package x86;

import java.io.FileOutputStream;
import java.io.IOException;

public class IncDec extends Base {
    public void decode(FileOutputStream fos) throws IOException {
        decodeFunc(fos, "INC AX", "040", "inc16_reg", 0);
        decodeFunc(fos, "INC EAX", "240", "inc32_reg", 0);
        decodeFunc(fos, "INC CX", "041", "inc16_reg", 1);
        decodeFunc(fos, "INC ECX", "241", "inc32_reg", 1);
        decodeFunc(fos, "INC DX", "042", "inc16_reg", 2);
        decodeFunc(fos, "INC EDX", "242", "inc32_reg", 2);
        decodeFunc(fos, "INC BX", "043", "inc16_reg", 3);
        decodeFunc(fos, "INC EBX", "243", "inc32_reg", 3);
        decodeFunc(fos, "INC SP", "044", "inc16_reg", 4);
        decodeFunc(fos, "INC ESP", "244", "inc32_reg", 4);
        decodeFunc(fos, "INC BP", "045", "inc16_reg", 5);
        decodeFunc(fos, "INC EBP", "245", "inc32_reg", 5);
        decodeFunc(fos, "INC SI", "046", "inc16_reg", 6);
        decodeFunc(fos, "INC ESI", "246", "inc32_reg", 6);
        decodeFunc(fos, "INC DI", "047", "inc16_reg", 7);
        decodeFunc(fos, "INC EDI", "247", "inc32_reg", 7);
        decodeFunc(fos, "DEC AX", "048", "dec16_reg", 0);
        decodeFunc(fos, "DEC EAX", "248", "dec32_reg", 0);
        decodeFunc(fos, "DEC CX", "049", "dec16_reg", 1);
        decodeFunc(fos, "DEC ECX", "249", "dec32_reg", 1);
        decodeFunc(fos, "DEC DX", "04a", "dec16_reg", 2);
        decodeFunc(fos, "DEC EDX", "24a", "dec32_reg", 2);
        decodeFunc(fos, "DEC BX", "04b", "dec16_reg", 3);
        decodeFunc(fos, "DEC EBX", "24b", "dec32_reg", 3);
        decodeFunc(fos, "DEC SP", "04c", "dec16_reg", 4);
        decodeFunc(fos, "DEC ESP", "24c", "dec32_reg", 4);
        decodeFunc(fos, "DEC BP", "04d", "dec16_reg", 5);
        decodeFunc(fos, "DEC EBP", "24d", "dec32_reg", 5);
        decodeFunc(fos, "DEC SI", "04e", "dec16_reg", 6);
        decodeFunc(fos, "DEC ESI", "24e", "dec32_reg", 6);
        decodeFunc(fos, "DEC DI", "04f", "dec16_reg", 7);
        decodeFunc(fos, "DEC EDI", "24f", "dec32_reg", 7);
    }

    public void generate() {
        try {
            FileOutputStream fos = new FileOutputStream("incdec.h");
            incDec(fos, "inc", "+");
            incDec(fos, "dec", "-");
            fos.close();
            fos = null;
        } catch (IOException e) {

        }
    }

    public void incDec(FileOutputStream fos, String name, String op) throws IOException {
        incDec8(fos, name, op);
        incDec16(fos, name, op);
        incDec32(fos, name, op);
    }

    public void incDec8(FileOutputStream fos, String name, String op) throws IOException {
        incDecBase(fos, name+"8_reg", "FLAGS_"+name.toUpperCase()+"8", "*cpu->reg8[op->r1]", "*cpu->reg8[op->r1] = ", "", op, "1", false, false, "8");
        incDecBase(fos, name+"8_mem16", "FLAGS_"+name.toUpperCase()+"8", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, "3", true, false, "8");
        incDecBase(fos, name+"8_mem32", "FLAGS_"+name.toUpperCase()+"8", "readb(cpu->memory, eaa)", "writeb(cpu->memory, eaa, ", ")", op, "3", false, true, "8");
    }

    public void incDec16(FileOutputStream fos, String name, String op) throws IOException {
        incDecBase(fos, name+"16_reg", "FLAGS_"+name.toUpperCase()+"16", "cpu->reg[op->r1].u16", "cpu->reg[op->r1].u16 = ", "", op, "1", false, false, "16");
        incDecBase(fos, name+"16_mem16", "FLAGS_"+name.toUpperCase()+"16", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, "3", true, false, "16");
        incDecBase(fos, name+"16_mem32", "FLAGS_"+name.toUpperCase()+"16", "readw(cpu->memory, eaa)", "writew(cpu->memory, eaa, ", ")", op, "3", false, true, "16");
    }

    public void incDec32(FileOutputStream fos, String name, String op) throws IOException {
        incDecBase(fos, name+"32_reg", "FLAGS_"+name.toUpperCase()+"32", "cpu->reg[op->r1].u32", "cpu->reg[op->r1].u32 = ", "", op, "1", false, false, "32");
        incDecBase(fos, name+"32_mem16", "FLAGS_"+name.toUpperCase()+"32", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, "3", true, false, "32");
        incDecBase(fos, name+"32_mem32", "FLAGS_"+name.toUpperCase()+"32", "readd(cpu->memory, eaa)", "writed(cpu->memory, eaa, ", ")", op, "3", false, true, "32");
    }

    public void incDecBase(FileOutputStream fos, String name, String flagName, String destLoad, String destSave1, String destSave2, String op, String cycles, boolean eaa16, boolean eaa32, String bits) throws IOException {
        out(fos, "void OPCALL "+name+"(struct CPU* cpu, struct Op* op) {");
        if (eaa16)
            out(fos, "    U32 eaa = eaa16(cpu, op);");
        if (eaa32)
            out(fos, "    U32 eaa = eaa32(cpu, op);");
        out(fos, "    cpu->oldcf=getCF(cpu);");
        out(fos, "    cpu->dst.u"+bits+"="+destLoad+";");
        out(fos, "    cpu->result.u"+bits+"=cpu->dst.u"+bits+" "+op+" 1;");
        out(fos, "    cpu->lazyFlags = "+flagName+";");
        out(fos, "    "+destSave1+"cpu->result.u"+bits+destSave2+";");
        out(fos, "    CYCLES("+cycles+");");
        out(fos, "    NEXT();");
        out(fos, "}");
    }
}

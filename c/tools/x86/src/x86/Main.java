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
        } catch (Exception e) {
            e.printStackTrace();
        }
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
        out("void "+name+"_16_reg(CPU* cpu, Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_16_mem16(CPU* cpu, Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_16_mem32(CPU* cpu, Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_32_reg(CPU* cpu, Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_32_mem16(CPU* cpu, Op* op) {");
        out("    if ("+condition+") {");
        out("        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));");
        out("    }");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void "+name+"_32_mem32(CPU* cpu, Op* op) {");
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
        out("void "+name+"(CPU* cpu, Op* op) {");
        out("    U32 dBase = cpu->segAddress[ES];");
        out("    U32 sBase = cpu->segAddress[op->base];");
        out("    U32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out("    Memory* memory = cpu->memory;");
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
        out("void "+name+"(CPU* cpu, Op* op) {");
        out("    U32 dBase = cpu->segAddress[ES];");
        out("    U32 sBase = cpu->segAddress[op->base];");
        out("    U32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out("    Memory* memory = cpu->memory;");
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
        out("void "+name+"(CPU* cpu, Op* op) {");
        out("    U32 dBase = cpu->segAddress[ES];");
        out("    U32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out("    Memory* memory = cpu->memory;");
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
        out("void "+name+"(CPU* cpu, Op* op) {");
        if (repeat) {
            out("    U32 sBase = cpu->segAddress[op->base];");
            out("    U32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
            out("    Memory* memory = cpu->memory;");
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
        out("void "+name+"(CPU* cpu, Op* op) {");
        if (repeat) {
        out("    U32 dBase = cpu->segAddress[ES];");
        out("    U32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out("    Memory* memory = cpu->memory;");
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
        out("void push"+name+"(CPU * cpu, Op * op){");
        out("    push"+bits+"(cpu, "+name.toUpperCase()+");");
        out("    CYCLES(1);");
        out("    NEXT();");
        out("}");
        out("void pop"+name+"(CPU * cpu, Op * op){");
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
        out("void "+name+"(CPU* cpu, Op* op) {");
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
        out("void "+functionName+"(CPU* cpu, Op* op) {");
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
        out("void "+name+"(CPU* cpu, Op* op) {");
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

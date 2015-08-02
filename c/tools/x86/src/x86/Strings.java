package x86;

import java.io.FileOutputStream;
import java.io.IOException;

public class Strings extends Base {
    public void decode(FileOutputStream fos) throws IOException {
        decodeString(fos, "0a4", "movsb");
        decodeString(fos, "0a5", "movsw");
        decodeString(fos, "2a5", "movsd");
        decodeString(fos, "0a6", "cmpsb");
        decodeString(fos, "0a7", "cmpsw");
        decodeString(fos, "2a7", "cmpsd");
        decodeString(fos, "0aa", "stosb");
        decodeString(fos, "0ab", "stosw");
        decodeString(fos, "2ab", "stosd");
        decodeString(fos, "0ac", "lodsb");
        decodeString(fos, "0ad", "lodsw");
        decodeString(fos, "2ad", "lodsd");
        decodeString(fos, "0ae", "scasb");
        decodeString(fos, "0af", "scasw");
        decodeString(fos, "2af", "scasd");
    }

    public void decodeString(FileOutputStream fos, String base, String name) throws IOException {
        out(fos, "// "+name.toUpperCase());
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    if (data->ea16) {");
        out(fos, "        if (data->rep) {");
        out(fos, "            data->op->func = "+name+"16_r_op;");
        out(fos, "        } else {");
        out(fos, "            data->op->func = "+name+"16_op;");
        out(fos, "        }");
        out(fos, "    } else {");
        out(fos, "        if (data->rep) {");
        out(fos, "            data->op->func = "+name+"32_r_op;");
        out(fos, "        } else {");
        out(fos, "            data->op->func = "+name+"32_op;");
        out(fos, "        }");
        out(fos, "    }");
        out(fos, "    data->op->data1 = data->rep_zero;");
        out(fos, "    data->op->base = data->ds;");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void generate() {
        try {
            FileOutputStream fos_c = new FileOutputStream("strings.c");
            FileOutputStream fos_h = new FileOutputStream("strings.h");
            FileOutputStream fos_op = new FileOutputStream("strings_op.h");

            out(fos_c, "#include \"cpu.h\"");
            out(fos_c, "#include \"decoder.h\"");
            out(fos_h, "#ifndef __STRINGS_OP_H__");
            out(fos_h, "#define __STRINGS_OP_H__");
            out(fos_h, "#include \"cpu.h\"");
            out(fos_op, "#include \"strings.h\"");

            movs(fos_c, fos_h, fos_op, "movsb32_r", "b", "ESI", "EDI", "ECX", 0, true);
            movs(fos_c, fos_h, fos_op, "movsb16_r", "b", "SI", "DI", "CX", 0, true);
            movs(fos_c, fos_h, fos_op, "movsb32", "b", "ESI", "EDI", "ECX", 0, false);
            movs(fos_c, fos_h, fos_op, "movsb16", "b", "SI", "DI", "CX", 0, false);

            movs(fos_c, fos_h, fos_op, "movsw32_r", "w", "ESI", "EDI", "ECX", 1, true);
            movs(fos_c, fos_h, fos_op, "movsw16_r", "w", "SI", "DI", "CX", 1, true);
            movs(fos_c, fos_h, fos_op, "movsw32", "w", "ESI", "EDI", "ECX", 1, false);
            movs(fos_c, fos_h, fos_op, "movsw16", "w", "SI", "DI", "CX", 1, false);

            movs(fos_c, fos_h, fos_op, "movsd32_r", "d", "ESI", "EDI", "ECX", 2, true);
            movs(fos_c, fos_h, fos_op, "movsd16_r", "d", "SI", "DI", "CX", 2, true);
            movs(fos_c, fos_h, fos_op, "movsd32", "d", "ESI", "EDI", "ECX", 2, false);
            movs(fos_c, fos_h, fos_op, "movsd16", "d", "SI", "DI", "CX", 2, false);

            cmps(fos_c, fos_h, fos_op, "cmpsb32_r", "b", "8", "ESI", "EDI", "ECX", 0, true);
            cmps(fos_c, fos_h, fos_op, "cmpsb16_r", "b", "8", "SI", "DI", "CX", 0, true);
            cmps(fos_c, fos_h, fos_op, "cmpsb32", "b", "8", "ESI", "EDI", "ECX", 0, false);
            cmps(fos_c, fos_h, fos_op, "cmpsb16", "b", "8", "SI", "DI", "CX", 0, false);

            cmps(fos_c, fos_h, fos_op, "cmpsw32_r", "w", "16", "ESI", "EDI", "ECX", 1, true);
            cmps(fos_c, fos_h, fos_op, "cmpsw16_r", "w", "16", "SI", "DI", "CX", 1, true);
            cmps(fos_c, fos_h, fos_op, "cmpsw32", "w", "16", "ESI", "EDI", "ECX", 1, false);
            cmps(fos_c, fos_h, fos_op, "cmpsw16", "w", "16", "SI", "DI", "CX", 1, false);

            cmps(fos_c, fos_h, fos_op, "cmpsd32_r", "d", "32", "ESI", "EDI", "ECX", 2, true);
            cmps(fos_c, fos_h, fos_op, "cmpsd16_r", "d", "32", "SI", "DI", "CX", 2, true);
            cmps(fos_c, fos_h, fos_op, "cmpsd32", "d", "32", "ESI", "EDI", "ECX", 2, false);
            cmps(fos_c, fos_h, fos_op, "cmpsd16", "d", "32", "SI", "DI", "CX", 2, false);

            stos(fos_c, fos_h, fos_op, "stosb32_r", "b", "EDI", "ECX", "AL", 0, true);
            stos(fos_c, fos_h, fos_op, "stosb16_r", "b", "DI", "CX", "AL", 0, true);
            stos(fos_c, fos_h, fos_op, "stosb32", "b", "EDI", "ECX", "AL", 0, false);
            stos(fos_c, fos_h, fos_op, "stosb16", "b", "DI", "CX", "AL", 0, false);

            stos(fos_c, fos_h, fos_op, "stosw32_r", "w", "EDI", "ECX", "AX", 1, true);
            stos(fos_c, fos_h, fos_op, "stosw16_r", "w", "DI", "CX", "AX", 1, true);
            stos(fos_c, fos_h, fos_op, "stosw32", "w", "EDI", "ECX", "AX", 1, false);
            stos(fos_c, fos_h, fos_op, "stosw16", "w", "DI", "CX", "AX", 1, false);

            stos(fos_c, fos_h, fos_op, "stosd32_r", "d", "EDI", "ECX", "EAX", 2, true);
            stos(fos_c, fos_h, fos_op, "stosd16_r", "d", "DI", "CX", "EAX", 2, true);
            stos(fos_c, fos_h, fos_op, "stosd32", "d", "EDI", "ECX", "EAX", 2, false);
            stos(fos_c, fos_h, fos_op, "stosd16", "d", "DI", "CX", "EAX", 2, false);

            lods(fos_c, fos_h, fos_op, "lodsb32_r", "b", "ESI", "ECX", "AL", 0, true);
            lods(fos_c, fos_h, fos_op, "lodsb16_r", "b", "SI", "CX", "AL", 0, true);
            lods(fos_c, fos_h, fos_op, "lodsb32", "b", "ESI", "ECX", "AL", 0, false);
            lods(fos_c, fos_h, fos_op, "lodsb16", "b", "SI", "CX", "AL", 0, false);

            lods(fos_c, fos_h, fos_op, "lodsw32_r", "w", "ESI", "ECX", "AX", 1, true);
            lods(fos_c, fos_h, fos_op, "lodsw16_r", "w", "SI", "CX", "AX", 1, true);
            lods(fos_c, fos_h, fos_op, "lodsw32", "w", "ESI", "ECX", "AX", 1, false);
            lods(fos_c, fos_h, fos_op, "lodsw16", "w", "SI", "CX", "AX", 1, false);

            lods(fos_c, fos_h, fos_op, "lodsd32_r", "d", "ESI", "ECX", "EAX", 2, true);
            lods(fos_c, fos_h, fos_op, "lodsd16_r", "d", "SI", "CX", "EAX", 2, true);
            lods(fos_c, fos_h, fos_op, "lodsd32", "d", "ESI", "ECX", "EAX", 2, false);
            lods(fos_c, fos_h, fos_op, "lodsd16", "d", "SI", "CX", "EAX", 2, false);

            scas(fos_c, fos_h, fos_op, "scasb32_r", "b", "8", "AL", "EDI", "ECX", 0, true);
            scas(fos_c, fos_h, fos_op, "scasb16_r", "b", "8", "AL", "DI", "CX", 0, true);
            scas(fos_c, fos_h, fos_op, "scasb32", "b", "8", "AL", "EDI", "ECX", 0, false);
            scas(fos_c, fos_h, fos_op, "scasb16", "b", "8", "AL", "DI", "CX", 0, false);

            scas(fos_c, fos_h, fos_op, "scasw32_r", "w", "16", "AX", "EDI", "ECX", 1, true);
            scas(fos_c, fos_h, fos_op, "scasw16_r", "w", "16", "AX", "DI", "CX", 1, true);
            scas(fos_c, fos_h, fos_op, "scasw32", "w", "16", "AX", "EDI", "ECX", 1, false);
            scas(fos_c, fos_h, fos_op, "scasw16", "w", "16", "AX", "DI", "CX", 1, false);

            scas(fos_c, fos_h, fos_op, "scasd32_r", "d", "32", "EAX", "EDI", "ECX", 2, true);
            scas(fos_c, fos_h, fos_op, "scasd16_r", "d", "32", "EAX", "DI", "CX", 2, true);
            scas(fos_c, fos_h, fos_op, "scasd32", "d", "32", "EAX", "EDI", "ECX", 2, false);
            scas(fos_c, fos_h, fos_op, "scasd16", "d", "32", "EAX", "DI", "CX", 2, false);
            fos_c.close();
            fos_c = null;
            out(fos_h, "#endif");
            fos_h.close();
            fos_op.close();
        } catch (IOException e) {

        }
    }

    public void movs(FileOutputStream fos, FileOutputStream fos_h, FileOutputStream fos_op, String name, String bits, String SI, String DI, String CX, int inc, boolean repeat) throws  IOException {
        out(fos_h, "void "+name+"(struct CPU* cpu, U32 base);");
        out(fos_op, "void OPCALL "+name+"_op(struct CPU* cpu, struct Op* op) {");
        out(fos_op, "    "+name+"(cpu, op->base);");
        out(fos_op, "    NEXT();");
        out(fos_op, "}");

        out(fos, "void "+name+"(struct CPU* cpu, U32 base) {");
        out(fos, "    U32 dBase = cpu->segAddress[ES];");
        out(fos, "    U32 sBase = cpu->segAddress[base];");
        out(fos, "    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out(fos, "    struct Memory* memory = cpu->memory;");
        if (repeat) {
            out(fos, "    U32 count = " + CX + ";");
            out(fos, "    U32 i;");
            out(fos, "    for (i=0;i<count;i++) {");
        }
        out(fos, "        write"+bits+"(memory, dBase+"+DI+", read"+bits+"(memory, sBase+"+SI+"));");
        out(fos, "        "+DI+"+=inc;");
        out(fos, "        "+SI+"+=inc;");
        if (repeat) {
            out(fos, "    }");
            out(fos, "    " + CX + "=0;");
            out(fos, "    CYCLES(3+count);");
        } else {
            out(fos, "    CYCLES(4);");
        }
        out(fos, "}");
    }

    public void cmps(FileOutputStream fos, FileOutputStream fos_h, FileOutputStream fos_op, String name, String width, String bits, String SI, String DI, String CX, int inc, boolean repeat) throws  IOException {
        out(fos_h, "void "+name+"(struct CPU* cpu, U32 rep_zero, U32 base);");
        out(fos_op, "void OPCALL "+name+"_op(struct CPU* cpu, struct Op* op) {");
        out(fos_op, "    "+name+"(cpu, op->data1, op->base);");
        out(fos_op, "    NEXT();");
        out(fos_op, "}");

        out(fos, "void "+name+"(struct CPU* cpu, U32 rep_zero, U32 base) {");
        out(fos, "    U32 dBase = cpu->segAddress[ES];");
        out(fos, "    U32 sBase = cpu->segAddress[base];");
        out(fos, "    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out(fos, "    struct Memory* memory = cpu->memory;");
        out(fos, "    U"+bits+" v1;");
        out(fos, "    U"+bits+" v2;");
        if (repeat) {
            out(fos, "    U32 count = " + CX + ";");
            out(fos, "    U32 i;");
            out(fos, "    if (count) {");
            out(fos, "        for (i=0;i<count;i++) {");
        }
        out(fos, "            v1 = read"+width+"(memory, dBase+"+DI+");");
        out(fos, "            v2 = read"+width+"(memory, sBase+"+SI+");");
        out(fos, "            "+DI+"+=inc;");
        out(fos, "            "+SI+"+=inc;");
        if (repeat) {
            out(fos, "            "+CX+"--;");
            out(fos, "            if ((v1==v2)!=rep_zero) break;");
            out(fos, "        }");
        }
        out(fos, "        cpu->dst.u"+bits+" = v2;");
        out(fos, "        cpu->src.u"+bits+" = v1;");
        out(fos, "        cpu->result.u"+bits+" = cpu->dst.u"+bits+" - cpu->src.u"+bits+";");
        out(fos, "        cpu->lazyFlags = FLAGS_SUB"+bits+";");
        if (repeat) {
            out(fos, "    }");
            out(fos, "    CYCLES(9+4*count);");
        } else {
            out(fos, "    CYCLES(5);");
        }
        out(fos, "}");
    }

    public void scas(FileOutputStream fos, FileOutputStream fos_h, FileOutputStream fos_op, String name, String width, String bits, String AX, String DI, String CX, int inc, boolean repeat) throws  IOException {
        out(fos_h, "void "+name+"(struct CPU* cpu, U32 rep_zero);");
        out(fos_op, "void OPCALL "+name+"_op(struct CPU* cpu, struct Op* op) {");
        out(fos_op, "    "+name+"(cpu, op->data1);");
        out(fos_op, "    NEXT();");
        out(fos_op, "}");

        out(fos, "void "+name+"(struct CPU* cpu, U32 rep_zero) {");
        out(fos, "    U32 dBase = cpu->segAddress[ES];");
        out(fos, "    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out(fos, "    struct Memory* memory = cpu->memory;");
        out(fos, "    U"+bits+" v1;");
        if (repeat) {
            out(fos, "    U32 count = " + CX + ";");
            out(fos, "    U32 i;");
            out(fos, "    if (count) {");
            out(fos, "        for (i=0;i<count;i++) {");
        }
        out(fos, "            v1 = read"+width+"(memory, dBase+"+DI+");");
        out(fos, "            "+DI+"+=inc;");
        if (repeat) {
            out(fos, "            "+CX+"--;");
            out(fos, "            if (("+AX+"==v1)!=rep_zero) break;");
            out(fos, "        }");
        }
        out(fos, "        cpu->dst.u"+bits+" = "+AX+";");
        out(fos, "        cpu->src.u"+bits+" = v1;");
        out(fos, "        cpu->result.u"+bits+" = "+AX+" - v1;");
        out(fos, "        cpu->lazyFlags = FLAGS_SUB"+bits+";");
        if (repeat) {
            out(fos, "    }");
            out(fos, "    CYCLES(8+4*count);");
        } else {
            out(fos, "    CYCLES(4);");
        }
        out(fos, "}");
    }

    public void lods(FileOutputStream fos, FileOutputStream fos_h, FileOutputStream fos_op, String name, String bits, String SI, String CX, String AX, int inc, boolean repeat) throws  IOException {
        out(fos_h, "void "+name+"(struct CPU* cpu, U32 base);");
        out(fos_op, "void OPCALL "+name+"_op(struct CPU* cpu, struct Op* op) {");
        out(fos_op, "    "+name+"(cpu, op->base);");
        out(fos_op, "    NEXT();");
        out(fos_op, "}");

        out(fos, "void "+name+"(struct CPU* cpu, U32 base) {");
        if (repeat) {
            out(fos, "    U32 sBase = cpu->segAddress[base];");
            out(fos, "    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
            out(fos, "    struct Memory* memory = cpu->memory;");
            out(fos, "    U32 count = " + CX + ";");
            out(fos, "    U32 i;");
            out(fos, "    for (i=0;i<count;i++) {");
            out(fos, "        "+AX+" = read"+bits+"(memory, sBase+"+SI+");");
            out(fos, "        "+SI+"+=inc;");
            out(fos, "    }");
            out(fos, "    " + CX + "=0;");
        } else {
            out(fos, "    "+AX+" = read"+bits+"(cpu->memory, cpu->segAddress[base]+"+SI+");");
            out(fos, "    "+SI+"+=cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        }
        out(fos, "    CYCLES(2);");
        out(fos, "}");
    }

    public void stos(FileOutputStream fos, FileOutputStream fos_h, FileOutputStream fos_op, String name, String bits, String DI, String CX, String AX, int inc, boolean repeat) throws  IOException {
        out(fos_h, "void "+name+"(struct CPU* cpu);");
        out(fos_op, "void OPCALL "+name+"_op(struct CPU* cpu, struct Op* op) {");
        out(fos_op, "    "+name+"(cpu);");
        out(fos_op, "    NEXT();");
        out(fos_op, "}");

        out(fos, "void "+name+"(struct CPU* cpu) {");
        if (repeat) {
            out(fos, "    U32 dBase = cpu->segAddress[ES];");
            out(fos, "    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
            out(fos, "    struct Memory* memory = cpu->memory;");
            out(fos, "    U32 count = " + CX + ";");
            out(fos, "    U32 i;");
            out(fos, "    for (i=0;i<count;i++) {");
            out(fos, "        write"+bits+"(memory, dBase+"+DI+", "+AX+");");
            out(fos, "        "+DI+"+=inc;");
            out(fos, "    }");
            out(fos, "    " + CX + "=0;");
            out(fos, "    CYCLES(3+count);");
        } else {
            out(fos, "    write"+bits+"(cpu->memory, cpu->segAddress[ES]+"+DI+", "+AX+");");
            out(fos, "    "+DI+"+=cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
            out(fos, "    CYCLES(3);");
        }
        out(fos, "}");
    }
}

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
        out(fos, "            data->op->func = "+name+"16_r;");
        out(fos, "        } else {");
        out(fos, "            data->op->func = "+name+"16;");
        out(fos, "        }");
        out(fos, "    } else {");
        out(fos, "        if (data->rep) {");
        out(fos, "            data->op->func = "+name+"32_r;");
        out(fos, "        } else {");
        out(fos, "            data->op->func = "+name+"32;");
        out(fos, "        }");
        out(fos, "    }");
        out(fos, "    data->op->data1 = data->rep_zero;");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void generate() {
        try {
            FileOutputStream fos = new FileOutputStream("strings.h");
            movs(fos, "movsb32_r", "b", "ESI", "EDI", "ECX", 0, true);
            movs(fos, "movsb16_r", "b", "SI", "DI", "CX", 0, true);
            movs(fos, "movsb32", "b", "ESI", "EDI", "ECX", 0, false);
            movs(fos, "movsb16", "b", "SI", "DI", "CX", 0, false);

            movs(fos, "movsw32_r", "w", "ESI", "EDI", "ECX", 1, true);
            movs(fos, "movsw16_r", "w", "SI", "DI", "CX", 1, true);
            movs(fos, "movsw32", "w", "ESI", "EDI", "ECX", 1, false);
            movs(fos, "movsw16", "w", "SI", "DI", "CX", 1, false);

            movs(fos, "movsd32_r", "d", "ESI", "EDI", "ECX", 2, true);
            movs(fos, "movsd16_r", "d", "SI", "DI", "CX", 2, true);
            movs(fos, "movsd32", "d", "ESI", "EDI", "ECX", 2, false);
            movs(fos, "movsd16", "d", "SI", "DI", "CX", 2, false);

            cmps(fos, "cmpsb32_r", "b", "8", "ESI", "EDI", "ECX", 0, true);
            cmps(fos, "cmpsb16_r", "b", "8", "SI", "DI", "CX", 0, true);
            cmps(fos, "cmpsb32", "b", "8", "ESI", "EDI", "ECX", 0, false);
            cmps(fos, "cmpsb16", "b", "8", "SI", "DI", "CX", 0, false);

            cmps(fos, "cmpsw32_r", "w", "16", "ESI", "EDI", "ECX", 1, true);
            cmps(fos, "cmpsw16_r", "w", "16", "SI", "DI", "CX", 1, true);
            cmps(fos, "cmpsw32", "w", "16", "ESI", "EDI", "ECX", 1, false);
            cmps(fos, "cmpsw16", "w", "16", "SI", "DI", "CX", 1, false);

            cmps(fos, "cmpsd32_r", "d", "32", "ESI", "EDI", "ECX", 2, true);
            cmps(fos, "cmpsd16_r", "d", "32", "SI", "DI", "CX", 2, true);
            cmps(fos, "cmpsd32", "d", "32", "ESI", "EDI", "ECX", 2, false);
            cmps(fos, "cmpsd16", "d", "32", "SI", "DI", "CX", 2, false);

            stos(fos, "stosb32_r", "b", "EDI", "ECX", "AL", 0, true);
            stos(fos, "stosb16_r", "b", "DI", "CX", "AL", 0, true);
            stos(fos, "stosb32", "b", "EDI", "ECX", "AL", 0, false);
            stos(fos, "stosb16", "b", "DI", "CX", "AL", 0, false);

            stos(fos, "stosw32_r", "w", "EDI", "ECX", "AX", 1, true);
            stos(fos, "stosw16_r", "w", "DI", "CX", "AX", 1, true);
            stos(fos, "stosw32", "w", "EDI", "ECX", "AX", 1, false);
            stos(fos, "stosw16", "w", "DI", "CX", "AX", 1, false);

            stos(fos, "stosd32_r", "d", "EDI", "ECX", "EAX", 2, true);
            stos(fos, "stosd16_r", "d", "DI", "CX", "EAX", 2, true);
            stos(fos, "stosd32", "d", "EDI", "ECX", "EAX", 2, false);
            stos(fos, "stosd16", "d", "DI", "CX", "EAX", 2, false);

            lods(fos, "lodsb32_r", "b", "ESI", "ECX", "AL", 0, true);
            lods(fos, "lodsb16_r", "b", "SI", "CX", "AL", 0, true);
            lods(fos, "lodsb32", "b", "ESI", "ECX", "AL", 0, false);
            lods(fos, "lodsb16", "b", "SI", "CX", "AL", 0, false);

            lods(fos, "lodsw32_r", "w", "ESI", "ECX", "AX", 1, true);
            lods(fos, "lodsw16_r", "w", "SI", "CX", "AX", 1, true);
            lods(fos, "lodsw32", "w", "ESI", "ECX", "AX", 1, false);
            lods(fos, "lodsw16", "w", "SI", "CX", "AX", 1, false);

            lods(fos, "lodsd32_r", "d", "ESI", "ECX", "EAX", 2, true);
            lods(fos, "lodsd16_r", "d", "SI", "CX", "EAX", 2, true);
            lods(fos, "lodsd32", "d", "ESI", "ECX", "EAX", 2, false);
            lods(fos, "lodsd16", "d", "SI", "CX", "EAX", 2, false);

            scas(fos, "scasb32_r", "b", "8", "AL", "EDI", "ECX", 0, true);
            scas(fos, "scasb16_r", "b", "8", "AL", "DI", "CX", 0, true);
            scas(fos, "scasb32", "b", "8", "AL", "EDI", "ECX", 0, false);
            scas(fos, "scasb16", "b", "8", "AL", "DI", "CX", 0, false);

            scas(fos, "scasw32_r", "w", "16", "AX", "EDI", "ECX", 1, true);
            scas(fos, "scasw16_r", "w", "16", "AX", "DI", "CX", 1, true);
            scas(fos, "scasw32", "w", "16", "AX", "EDI", "ECX", 1, false);
            scas(fos, "scasw16", "w", "16", "AX", "DI", "CX", 1, false);

            scas(fos, "scasd32_r", "d", "32", "EAX", "EDI", "ECX", 2, true);
            scas(fos, "scasd16_r", "d", "32", "EAX", "DI", "CX", 2, true);
            scas(fos, "scasd32", "d", "32", "EAX", "EDI", "ECX", 2, false);
            scas(fos, "scasd16", "d", "32", "EAX", "DI", "CX", 2, false);
            fos.close();
            fos = null;
        } catch (IOException e) {

        }
    }

    public void movs(FileOutputStream fos, String name, String bits, String SI, String DI, String CX, int inc, boolean repeat) throws  IOException {
        out(fos, "void OPCALL "+name+"(struct CPU* cpu, struct Op* op) {");
        out(fos, "    U32 dBase = cpu->segAddress[ES];");
        out(fos, "    U32 sBase = cpu->segAddress[op->base];");
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
        out(fos, "    NEXT();");
        out(fos, "}");
    }

    public void cmps(FileOutputStream fos, String name, String width, String bits, String SI, String DI, String CX, int inc, boolean repeat) throws  IOException {
        out(fos, "void OPCALL "+name+"(struct CPU* cpu, struct Op* op) {");
        out(fos, "    U32 dBase = cpu->segAddress[ES];");
        out(fos, "    U32 sBase = cpu->segAddress[op->base];");
        out(fos, "    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out(fos, "    struct Memory* memory = cpu->memory;");
        out(fos, "    U"+bits+" v1;");
        out(fos, "    U"+bits+" v2;");
        if (repeat) {
            out(fos, "    U32 count = " + CX + ";");
            out(fos, "    U32 i;");
            out(fos, "    U32 rep_zero = op->data1;");
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
        out(fos, "    NEXT();");
        out(fos, "}");
    }

    public void scas(FileOutputStream fos, String name, String width, String bits, String AX, String DI, String CX, int inc, boolean repeat) throws  IOException {
        out(fos, "void OPCALL "+name+"(struct CPU* cpu, struct Op* op) {");
        out(fos, "    U32 dBase = cpu->segAddress[ES];");
        out(fos, "    S32 inc = cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        out(fos, "    struct Memory* memory = cpu->memory;");
        out(fos, "    U"+bits+" v1;");
        if (repeat) {
            out(fos, "    U32 count = " + CX + ";");
            out(fos, "    U32 i;");
            out(fos, "    U32 rep_zero = op->data1;");
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
        out(fos, "    NEXT();");
        out(fos, "}");
    }

    public void lods(FileOutputStream fos, String name, String bits, String SI, String CX, String AX, int inc, boolean repeat) throws  IOException {
        out(fos, "void OPCALL "+name+"(struct CPU* cpu, struct Op* op) {");
        if (repeat) {
            out(fos, "    U32 sBase = cpu->segAddress[op->base];");
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
            out(fos, "    "+AX+" = read"+bits+"(cpu->memory, cpu->segAddress[op->base]+"+SI+");");
            out(fos, "    "+SI+"+=cpu->df"+(inc>0?" << "+String.valueOf(inc):"")+";");
        }
        out(fos, "    CYCLES(2);");
        out(fos, "    NEXT();");
        out(fos, "}");
    }

    public void stos(FileOutputStream fos, String name, String bits, String DI, String CX, String AX, int inc, boolean repeat) throws  IOException {
        out(fos, "void OPCALL "+name+"(struct CPU* cpu, struct Op* op) {");
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
        out(fos, "    NEXT();");
        out(fos, "}");
    }
}

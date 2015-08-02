package x86;

import java.io.FileOutputStream;
import java.io.IOException;

abstract public class Base {
    abstract public void decode(FileOutputStream fos) throws IOException;
    abstract public void generate();

    public void decode_noflags(FileOutputStream fos) throws IOException {

    }

    public void gensrc(FileOutputStream fos) throws IOException {

    }

    public void out(FileOutputStream fos, String line) throws IOException {
        line=line+"\r\n";
        fos.write(line.getBytes());
    }

    public void eg(FileOutputStream fos, String inst, String name, int bits) throws IOException {
        eg(fos, inst, name, bits, null);
    }
    public void eg(FileOutputStream fos, String inst, String name, int bits, String extra) throws IOException {
        if (bits==8)
            out(fos, "// "+name.toUpperCase()+" Eb,Gb");
        else if (bits==16)
            out(fos, "// "+name.toUpperCase()+" Ew,Gw");
        else
            out(fos, "// "+name.toUpperCase()+" Ed,Gd");
        out(fos, "void decode"+inst+"(struct DecodeData* data) {");
        out(fos, "    U8 rm = FETCH8(data);");
        out(fos, "    if (rm>=0xC0) {");
        out(fos, "        data->op->func = "+name+"r"+bits+"r"+bits+";");
        out(fos, "        data->op->r1 = E(rm);");
        out(fos, "        data->op->r2 = G(rm);");
        out(fos, "        LOG_OP2(\""+name.toUpperCase()+"\", R"+bits+"(data->op->r1),R"+bits+"(data->op->r2));");
        out(fos, "    } else if (data->ea16) {");
        out(fos, "        data->op->func = "+name+"e"+bits+"r"+bits+"_16;");
        out(fos, "        data->op->r1 = G(rm);");
        out(fos, "        decodeEa16(data, rm);");
        out(fos, "        LOG_OP2(\""+name.toUpperCase()+"\", M"+bits+"(data, rm, data->op),R"+bits+"(data->op->r1));");
        out(fos, "    } else {");
        out(fos, "        data->op->func = "+name+"e"+bits+"r"+bits+"_32;");
        out(fos, "        data->op->r1 = G(rm);");
        out(fos, "        decodeEa32(data, rm);");
        out(fos, "        LOG_OP2(\""+name.toUpperCase()+"\", M"+bits+"(data, rm, data->op),R"+bits+"(data->op->r1));");
        out(fos, "    }");
        if (extra!=null)
            out(fos, extra);
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }
    public void ge(FileOutputStream fos, String inst, String name, int bits) throws IOException {
        ge(fos, inst, name, bits, 0, null);
    }

    public void ge(FileOutputStream fos, String inst, String name, int bits, int data, String comment) throws IOException {
        if (comment!=null) {
            out(fos, "// "+comment);
        } else if (bits==8)
            out(fos, "// "+name.toUpperCase()+" Gb,Eb");
        else if (bits==16)
            out(fos, "// "+name.toUpperCase()+" Gw,Ew");
        else
            out(fos, "// "+name.toUpperCase()+" Gd,Ed");
        out(fos, "void decode"+inst+"(struct DecodeData* data) {");
        out(fos, "    U8 rm = FETCH8(data);");
        out(fos, "    if (rm>=0xC0) {");
        out(fos, "        data->op->func = "+name+"r"+bits+"r"+bits+";");
        out(fos, "        data->op->r1 = G(rm);");
        out(fos, "        data->op->r2 = E(rm);");
        out(fos, "        LOG_OP2(\""+name.toUpperCase()+"\", R"+bits+"(data->op->r1),R"+bits+"(data->op->r2));");
        out(fos, "    } else if (data->ea16) {");
        out(fos, "        data->op->func = "+name+"r"+bits+"e"+bits+"_16;");
        out(fos, "        data->op->r1 = G(rm);");
        out(fos, "        decodeEa16(data, rm);");
        out(fos, "        LOG_OP2(\""+name.toUpperCase()+"\", R"+bits+"(data->op->r1),M"+bits+"(data, rm, data->op));");
        out(fos, "    } else {");
        out(fos, "        data->op->func = "+name+"r"+bits+"e"+bits+"_32;");
        out(fos, "        data->op->r1 = G(rm);");
        out(fos, "        decodeEa32(data, rm);");
        out(fos, "        LOG_OP2(\""+name.toUpperCase()+"\", R"+bits+"(data->op->r1),M"+bits+"(data, rm, data->op));");
        out(fos, "    }");
        decodeData(fos, data);
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void ad(FileOutputStream fos, String inst, String name, int bits) throws IOException {
        if (bits==8)
            out(fos, "// "+name.toUpperCase()+" Al,Ib");
        else if (bits==16)
            out(fos, "// "+name.toUpperCase()+" Ax,Iw");
        else
            out(fos, "// "+name.toUpperCase()+" Eax,Id");
        out(fos, "void decode"+inst+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = "+name+bits+"_reg;");
        out(fos, "    data->op->r1 = 0;");
        out(fos, "    data->op->data1 = FETCH"+bits+"(data);");
        out(fos, "    LOG_OP2(\""+name.toUpperCase()+"\", R"+bits+"(0), itoa(data->op->data1, tmp, 16));");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void eg_noflags(FileOutputStream fos, String inst, String name, int bits) throws IOException {
        String func_rr = name+"r"+bits+"r"+bits;
        String func_er = name+"e"+bits+"r"+bits;

        out(fos, "void OPCALL "+func_rr+"(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+func_er+"_32(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+func_er+"_16(struct CPU* cpu, struct Op* op);");
        if (bits==8)
            out(fos, "// "+name.toUpperCase()+" Eb,Gb");
        else if (bits==16)
            out(fos, "// "+name.toUpperCase()+" Ew,Gw");
        else
            out(fos, "// "+name.toUpperCase()+" Ed,Gd");
        out(fos, "void decode"+inst+"_noflags(struct Op* op) {");
        out(fos, "    if (op->func == "+func_rr+") {");
        out(fos, "        op->func = "+func_rr+"_noflags;");
        out(fos, "    } else if (op->func == "+func_er+"_32) {");
        out(fos, "        op->func = "+func_er+"_32_noflags;");
        out(fos, "    } else if (op->func == "+func_er+"_16) {");
        out(fos, "        op->func = "+func_er+"_16_noflags;");
        out(fos, "    } else {");
        out(fos, "        kpanic(\"decode"+inst+"_noflags error\");");
        out(fos, "    }");
        out(fos, "}");
    }

    public void ge_noflags(FileOutputStream fos, String inst, String name, int bits) throws IOException {
        String func_rr = name+"r"+bits+"r"+bits;
        String func_re = name+"r"+bits+"e"+bits;

        out(fos, "void OPCALL "+func_rr+"(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+func_re+"_32(struct CPU* cpu, struct Op* op);");
        out(fos, "void OPCALL "+func_re+"_16(struct CPU* cpu, struct Op* op);");
        if (bits==8)
            out(fos, "// "+name.toUpperCase()+" Gb,Eb");
        else if (bits==16)
            out(fos, "// "+name.toUpperCase()+" Gw,Ew");
        else
            out(fos, "// "+name.toUpperCase()+" Gd,Ed");
        out(fos, "void decode"+inst+"_noflags(struct Op* op) {");
        out(fos, "    if (op->func == "+func_rr+") {");
        out(fos, "        op->func = "+func_rr+"_noflags;");
        out(fos, "    } else if (op->func == "+func_re+"_32) {");
        out(fos, "        op->func = "+func_re+"_32_noflags;");
        out(fos, "    } else if (op->func == "+func_re+"_16) {");
        out(fos, "        op->func = "+func_re+"_16_noflags;");
        out(fos, "    } else {");
        out(fos, "        kpanic(\"decode"+inst+"_noflags error\");");
        out(fos, "    }");
        out(fos, "}");

    }

    public void ad_noflags(FileOutputStream fos, String inst, String name, int bits) throws IOException {
        String func = name+bits+"_reg";

        out(fos, "void OPCALL "+func+"(struct CPU* cpu, struct Op* op);");
        if (bits==8)
            out(fos, "// "+name.toUpperCase()+" Al,Ib");
        else if (bits==16)
            out(fos, "// "+name.toUpperCase()+" Ax,Iw");
        else
            out(fos, "// "+name.toUpperCase()+" Eax,Id");
        out(fos, "void decode"+inst+"_noflags(struct Op* op) {");
        out(fos, "    if (op->func == "+func+") {");
        out(fos, "        op->func = "+func+"_noflags;");
        out(fos, "    } else {");
        out(fos, "        kpanic(\"decode"+inst+"_noflags error\");");
        out(fos, "    }");
        out(fos, "}");
    }
    public void decodeFunc(FileOutputStream fos, String comment, String base, String func, int reg) throws IOException {
        decodeFunc(fos, comment, base, func, String.valueOf(reg));
    }
    public void decodeFunc(FileOutputStream fos, String comment, String base, String func, String reg) throws IOException {
        out(fos, "// "+comment);
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = "+func+";");
        out(fos, "    data->op->r1 = "+reg+";");
        out(fos, "    LOG_OP(\""+comment+"\");");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void decodeFunc(FileOutputStream fos, String base, String func, String asm) throws IOException {
        out(fos, "// "+func.toUpperCase());
        out(fos, "void decode"+base+"(struct DecodeData* data) {");
        out(fos, "    data->op->func = "+func+";");
        out(fos, "    LOG_OP(\""+asm+"\");");
        out(fos, "    NEXT_OP(data);");
        out(fos, "}");
    }

    public void decodeData(FileOutputStream fos, int bits) throws IOException {
        if (bits==8) {
            out(fos, "    data->op->data1 = FETCH8(data);");
        } else if (bits==16) {
            out(fos, "    data->op->data1 = FETCH16(data);");
        } else if (bits==32) {
            out(fos, "    data->op->data1 = FETCH32(data);");
        } else if (bits == -8) {
            out(fos, "    data->op->data1 = FETCH_S8(data);");
        } else if (bits == -16) {
            out(fos, "    data->op->data1 = FETCH_S16(data);");
        }
    }
}

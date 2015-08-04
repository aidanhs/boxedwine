#ifdef GENERATE_SOURCE
#include "op.h"
#include "block.h"
#include "cpu.h"
#include "kalloc.h"

#include <stdio.h>
#include <stdlib.h>

char* sourceBuffer;
U32 sourceBufferLen;
U32 sourceBufferPos;

void outfp(FILE* fp, const char* str) {
    fwrite(str, strlen(str), 1, fp);
}

void out(const char* str) {
    U32 len = strlen(str);
    if (sourceBufferPos+len>sourceBufferLen) {
        char* tmp = kalloc(sourceBufferLen*2);
        strcpy(tmp, sourceBuffer);
        kfree(sourceBuffer);
        sourceBuffer = tmp;
        sourceBufferLen*=2;
    }
    strcpy(sourceBuffer+sourceBufferPos, str);
    sourceBufferPos+=len;
}

const char* r8(int r) {
    switch (r) {
    case 0: return "AL";
    case 1: return "CL";
    case 2: return "DL";
    case 3: return "BL";
    case 4: return "AH";
    case 5: return "CH";
    case 6: return "DH";
    case 7: return "BH";
    }
    return "r8";
}

const char* r16(int r) {
    switch (r) {
    case 0: return "AX";
    case 1: return "CX";
    case 2: return "DX";
    case 3: return "BX";
    case 4: return "SP";
    case 5: return "BP";
    case 6: return "SI";
    case 7: return "DI";
    }
    return "r16";
}

const char* r32(int r) {
    switch (r) {
    case 0: return "EAX";
    case 1: return "ECX";
    case 2: return "EDX";
    case 3: return "EBX";
    case 4: return "ESP";
    case 5: return "EBP";
    case 6: return "ESI";
    case 7: return "EDI";
    }
    return "r32";
}

const char* getBase(int base) {
    switch(base) {
        case ES: return "ES";
        case CS: return "CS";
        case SS: return "SS";
        case DS: return "DS";
        case FS: return "FS";
        case GS: return "GS";
    }
    return "getBase";
}

char eaaStr[256];

const char* getEaa16(struct Op* op) {
    eaaStr[0]=0;

    if (op->base<6) {
        strcat(eaaStr, "cpu->segAddress[");
        strcat(eaaStr, getBase(op->base));
        strcat(eaaStr, "] + ");
    }
    strcat(eaaStr, "(U16)(");
    strcat(eaaStr, r16(op->e1));
    if (op->e2<8) {
        strcat(eaaStr, " + (S16)");
        strcat(eaaStr, r16(op->e2));
    }
    if (op->eData) {
        char tmp[32];
        strcat(eaaStr, " + 0x");
        itoa(op->eData, tmp, 16);
        strcat(eaaStr, tmp);
    }
    strcat(eaaStr, ")");
    return eaaStr;
}

const char* getEaa32(struct Op* op) {
    eaaStr[0]=0;

    if (op->base<6) {
        strcat(eaaStr, "cpu->segAddress[");
        strcat(eaaStr, getBase(op->base));
        strcat(eaaStr, "]");
    }
    if (op->e1<8) {
        if (eaaStr[0])
            strcat(eaaStr, " + ");
        strcat(eaaStr, r32(op->e1));
    }    
    if (op->e2<8) {
        char tmp[32];

        if (eaaStr[0])
            strcat(eaaStr, " + ");
        if (op->eSib)
            strcat(eaaStr, "(");
        strcat(eaaStr, r32(op->e2));
        if (op->eSib) {
            strcat(eaaStr, " << ");
            itoa(op->eSib, tmp, 10);
            strcat(eaaStr, tmp);
            strcat(eaaStr, ")");
        }
    }
    if (op->eData) {
        char tmp[32];

        if (eaaStr[0])
            strcat(eaaStr, " + ");
        strcat(eaaStr, "0x");
        itoa(op->eData, tmp, 16);
        strcat(eaaStr, tmp);
    }
    return eaaStr;
}

void gen027(struct Op* op) {
    out("instruction_daa(cpu);CYCLES(3);");
}

void gen02f(struct Op* op) {
    out("instruction_das(cpu);CYCLES(3);");
}

void gen037(struct Op* op) {
    out("instruction_aaa(cpu);CYCLES(3);");
}

void gen03f(struct Op* op) {
    out("instruction_aas(cpu);CYCLES(3);");
}

void OPCALL inc16_reg(struct CPU* cpu, struct Op* op);
void OPCALL inc16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen040(struct Op* op) {
    if (op->func==inc16_reg) {
        out("cpu->oldcf=getCF(cpu); cpu->dst.u16 = ");
        out(r16(op->r1));
        out("; cpu->result.u16=cpu->dst.u16 + 1; cpu->lazyFlags = FLAGS_INC16; ");
        out(r16(op->r1));
        out(" = cpu->result.u16;CYCLES(1);");
    } else if (op->func==inc16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" + 1;CYCLES(1);");
    }
}

void OPCALL dec16_reg(struct CPU* cpu, struct Op* op);
void OPCALL dec16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen048(struct Op* op) {
    if (op->func==inc16_reg) {
        out("cpu->oldcf=getCF(cpu); cpu->dst.u16 = ");
        out(r16(op->r1));
        out("; cpu->result.u16=cpu->dst.u16 - 1; cpu->lazyFlags = FLAGS_DEC16; ");
        out(r16(op->r1));
        out(" = cpu->result.u16;CYCLES(1);");
    } else if (op->func==inc16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" - 1;CYCLES(1);");
    }
}

void OPCALL inc32_reg(struct CPU* cpu, struct Op* op);
void OPCALL inc32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen240(struct Op* op) {
    if (op->func==inc32_reg) {
        out("cpu->oldcf=getCF(cpu); cpu->dst.u32 = ");
        out(r32(op->r1));
        out("; cpu->result.u32=cpu->dst.u32 + 1; cpu->lazyFlags = FLAGS_INC32; ");
        out(r32(op->r1));
        out(" = cpu->result.u32;CYCLES(1);");
    } else if (op->func==inc32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" + 1;CYCLES(1);");
    }
}

void OPCALL dec32_reg(struct CPU* cpu, struct Op* op);
void OPCALL dec32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen248(struct Op* op) {
    if (op->func==inc32_reg) {
        out("cpu->oldcf=getCF(cpu); cpu->dst.u32 = ");
        out(r32(op->r1));
        out("; cpu->result.u32=cpu->dst.u32 - 1; cpu->lazyFlags = FLAGS_DEC32; ");
        out(r32(op->r1));
        out(" = cpu->result.u32;CYCLES(1);");
    } else if (op->func==inc32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" - 1;CYCLES(1);");
    }
}

void gen050(struct Op* op) {
    out("push16(cpu, ");
    out(r16(op->r1));
    out(");CYCLES(1);");
}

void gen058(struct Op* op) {
    out(r16(op->r1));
    out(" = pop16(cpu);CYCLES(1);");
}

void gen250(struct Op* op) {
    out("push32(cpu, ");
    out(r32(op->r1));
    out(");CYCLES(1);");
}

void gen258(struct Op* op) {
    out(r32(op->r1));
    out(" = pop32(cpu);CYCLES(1);");
}

void gen068(struct Op* op) {
    char tmp[16];

    out("push16(cpu, 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out(");CYCLES(1);");
}

void gen268(struct Op* op) {
    char tmp[16];

    out("push32(cpu, 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out(");CYCLES(1);");
}

void OPCALL dimulcr16r16(struct CPU* cpu, struct Op* op);
void OPCALL dimulcr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL dimulcr16e16_32(struct CPU* cpu, struct Op* op);
void gen069(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 16);
    out("tmp32=(S16)(");
    if (op->func == dimulcr16r16) {
        out(r16(op->r2));        
    } else if (op->func == dimulcr16e16_16) {
        out("readw(cpu->memory, ");
        out(getEaa16(op));
        out(")");
    } else if (op->func == dimulcr16e16_32) {
        out("readw(cpu->memory, ");
        out(getEaa32(op));
        out(")");
    }
    
    out(") * (S32)0x");
    out(tmp);
    out("; fillFlagsNoCFOF(cpu); if ((tmp32 >= -32767) && (tmp32 <= 32767)) { removeFlag(CF|OF); } else { addFlag(CF|OF); } ");
    out(r16(op->r1));
    out(" = tmp32;CYCLES(10);");
}

void OPCALL dimulcr32r32(struct CPU* cpu, struct Op* op);
void OPCALL dimulcr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL dimulcr32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL dimulcr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dimulcr32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dimulcr32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen269(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    if (op->func == dimulcr32r32_noflags) {
        out(r32(op->r1));
        out(" = (S32)");
        out(r32(op->r2));
        out(" * ((S32)0x");
        out(tmp);
        out(");CYCLES(10);");
        return;
    } else if (op->func == dimulcr32e32_16_noflags) {
        out(r32(op->r1));
        out(" = (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out(") * ((S32)0x");
        out(tmp);
        out(");CYCLES(10);");
        return;
    } else if (op->func == dimulcr32e32_32_noflags) {
        out(r32(op->r1));
        out(" = (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out(") * ((S32)0x");
        out(tmp);
        out(");CYCLES(10);");
        return;
    }
    out("tmp64=(S32)(");
    if (op->func == dimulcr32r32) {
        out(r32(op->r2));    
    } else if (op->func == dimulcr32e32_16) {
        out("readd(cpu->memory, ");
        out(getEaa16(op));
        out(")");
    } else if (op->func == dimulcr32e32_32) {
        out("readd(cpu->memory, ");
        out(getEaa32(op));
        out(")");
    }
    out(") * (S64)((S32)0x");
    out(tmp);
    out("); fillFlagsNoCFOF(cpu); if ((tmp32 >= -2147483647l) && (tmp32 <= 2147483647l)) { removeFlag(CF|OF); } else { addFlag(CF|OF); } ");
    out(r32(op->r1));
    out(" = (S32)tmp64;CYCLES(10);");
}

void genJump(struct Op* op, const char* condition) {
    char tmp[16];
    itoa(op->data1+op->eipCount, tmp, 10);

    out("if (");
    out(condition);
    out(") {cpu->eip.u32+=");
    out(tmp);    
    out("; cpu->nextBlock = getBlock2(cpu);} else {cpu->eip.u32+=");
    itoa(op->eipCount, tmp, 10);
    out(tmp);
    out(";cpu->nextBlock = getBlock1(cpu);}CYCLES(1);");
}

void gen070(struct Op* op) {
    genJump(op, "getOF(cpu)");
}

void gen071(struct Op* op) {
    genJump(op, "!getOF(cpu)");
}

void gen072(struct Op* op) {
    genJump(op, "getCF(cpu)");
}

void gen073(struct Op* op) {
    genJump(op, "!getCF(cpu)");
}

void gen074(struct Op* op) {
    genJump(op, "getZF(cpu)");
}

void gen075(struct Op* op) {
    genJump(op, "!getZF(cpu)");
}

void gen076(struct Op* op) {
    genJump(op, "getZF(cpu) || getCF(cpu)");
}

void gen077(struct Op* op) {
    genJump(op, "!getZF(cpu) && !getCF(cpu)");
}

void gen078(struct Op* op) {
    genJump(op, "getSF(cpu)");
}

void gen079(struct Op* op) {
    genJump(op, "!getSF(cpu)");
}

void gen07a(struct Op* op) {
    genJump(op, "getPF(cpu)");
}

void gen07b(struct Op* op) {
    genJump(op, "!getPF(cpu)");
}

void gen07c(struct Op* op) {
    genJump(op, "getSF(cpu)!=getOF(cpu)");
}

void gen07d(struct Op* op) {
    genJump(op, "getSF(cpu)==getOF(cpu)");
}

void gen07e(struct Op* op) {
    genJump(op, "getZF(cpu) || getSF(cpu)!=getOF(cpu)");
}

void gen07f(struct Op* op) {
    genJump(op, "!getZF(cpu) && getSF(cpu)==getOF(cpu)");
}

#include "gensrc.h"

void OPCALL add8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL add8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL add8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL add8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL or8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL or8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL or8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL or8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adc8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL adc8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL adc8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adc8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbb8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL sbb8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL sbb8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbb8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL and8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL and8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL and8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL and8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sub8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL sub8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL sub8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sub8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xor8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL xor8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL xor8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xor8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmp8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmp8_mem32(struct CPU* cpu, struct Op* op);
void gen080(struct Op* op) {
    if (op->func==add8_reg) {
        genArithR("+", "FLAGS_ADD8", "8", r8(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==add8_mem16) {
        genArithE("+", "FLAGS_ADD8", "8", getEaa16(op), "b", op->data1, 1, 0, "3");
    } else if (op->func==add8_mem32) {
        genArithE("+", "FLAGS_ADD8", "8", getEaa32(op), "b", op->data1, 1, 0, "3");
    } else if (op->func==add8_reg_noflags) {
        genArithR_noflags("+", "8", r8(op->r1), op->data1, 0, "1");
    } else if (op->func==add8_mem16_noflags) {
        genArithE_noflags("+", "8", getEaa16(op), "b", op->data1, 0, "3");
    } else if (op->func==add8_mem32_noflags) {
        genArithE_noflags("+", "8", getEaa32(op), "b", op->data1, 0, "3");
    }

    else if (op->func==or8_reg) {
        genArithR("|", "FLAGS_OR8", "8", r8(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==or8_mem16) {
        genArithE("|", "FLAGS_OR8", "8", getEaa16(op), "b", op->data1, 1, 0, "3");
    } else if (op->func==or8_mem32) {
        genArithE("|", "FLAGS_OR8", "8", getEaa32(op), "b", op->data1, 1, 0, "3");
    } else if (op->func==or8_reg_noflags) {
        genArithR_noflags("|", "8", r8(op->r1), op->data1, 0, "1");
    } else if (op->func==or8_mem16_noflags) {
        genArithE_noflags("|", "8", getEaa16(op), "b", op->data1, 0, "3");
    } else if (op->func==or8_mem32_noflags) {
        genArithE_noflags("|", "8", getEaa32(op), "b", op->data1, 0, "3");
    }

    else if (op->func==adc8_reg) {
        genArithR("+", "FLAGS_ADC8", "8", r8(op->r1), op->data1, 1, 1, "1");
    } else if (op->func==adc8_mem16) {
        genArithE("+", "FLAGS_ADC8", "8", getEaa16(op), "b", op->data1, 1, 1, "3");
    } else if (op->func==adc8_mem32) {
        genArithE("+", "FLAGS_ADC8", "8", getEaa32(op), "b", op->data1, 1, 1, "3");
    } else if (op->func==adc8_reg_noflags) {
        genArithR_noflags("+", "8", r8(op->r1), op->data1, 1, "1");
    } else if (op->func==adc8_mem16_noflags) {
        genArithE_noflags("+", "8", getEaa16(op), "b", op->data1, 1, "3");
    } else if (op->func==adc8_mem32_noflags) {
        genArithE_noflags("+", "8", getEaa32(op), "b", op->data1, 1, "3");
    }

    else if (op->func==sbb8_reg) {
        genArithR("-", "FLAGS_SBB8", "8", r8(op->r1), op->data1, 1, 1, "1");
    } else if (op->func==sbb8_mem16) {
        genArithE("-", "FLAGS_SBB8", "8", getEaa16(op), "b", op->data1, 1, 1, "3");
    } else if (op->func==sbb8_mem32) {
        genArithE("-", "FLAGS_SBB8", "8", getEaa32(op), "b", op->data1, 1, 1, "3");
    } else if (op->func==sbb8_reg_noflags) {
        genArithR_noflags("-", "8", r8(op->r1), op->data1, 1, "1");
    } else if (op->func==sbb8_mem16_noflags) {
        genArithE_noflags("-", "8", getEaa16(op), "b", op->data1, 1, "3");
    } else if (op->func==sbb8_mem32_noflags) {
        genArithE_noflags("-", "8", getEaa32(op), "b", op->data1, 1, "3");
    }

    else if (op->func==and8_reg) {
        genArithR("&", "FLAGS_AND8", "8", r8(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==and8_mem16) {
        genArithE("&", "FLAGS_AND8", "8", getEaa16(op), "b", op->data1, 1, 0, "3");
    } else if (op->func==and8_mem32) {
        genArithE("&", "FLAGS_AND8", "8", getEaa32(op), "b", op->data1, 1, 0, "3");
    } else if (op->func==and8_reg_noflags) {
        genArithR_noflags("&", "8", r8(op->r1), op->data1, 0, "1");
    } else if (op->func==and8_mem16_noflags) {
        genArithE_noflags("&", "8", getEaa16(op), "b", op->data1, 0, "3");
    } else if (op->func==and8_mem32_noflags) {
        genArithE_noflags("&", "8", getEaa32(op), "b", op->data1, 0, "3");
    }

    else if (op->func==sub8_reg) {
        genArithR("-", "FLAGS_SUB8", "8", r8(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==sub8_mem16) {
        genArithE("-", "FLAGS_SUB8", "8", getEaa16(op), "b", op->data1, 1, 0, "3");
    } else if (op->func==sub8_mem32) {
        genArithE("-", "FLAGS_SUB8", "8", getEaa32(op), "b", op->data1, 1, 0, "3");
    } else if (op->func==sub8_reg_noflags) {
        genArithR_noflags("-", "8", r8(op->r1), op->data1, 0, "1");
    } else if (op->func==sub8_mem16_noflags) {
        genArithE_noflags("-", "8", getEaa16(op), "b", op->data1, 0, "3");
    } else if (op->func==sub8_mem32_noflags) {
        genArithE_noflags("-", "8", getEaa32(op), "b", op->data1, 0, "3");
    }

    else if (op->func==xor8_reg) {
        genArithR("^", "FLAGS_XOR8", "8", r8(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==xor8_mem16) {
        genArithE("^", "FLAGS_XOR8", "8", getEaa16(op), "b", op->data1, 1, 0, "3");
    } else if (op->func==xor8_mem32) {
        genArithE("^", "FLAGS_XOR8", "8", getEaa32(op), "b", op->data1, 1, 0, "3");
    } else if (op->func==xor8_reg_noflags) {
        genArithR_noflags("^", "8", r8(op->r1), op->data1, 0, "1");
    } else if (op->func==xor8_mem16_noflags) {
        genArithE_noflags("^", "8", getEaa16(op), "b", op->data1, 0, "3");
    } else if (op->func==xor8_mem32_noflags) {
        genArithE_noflags("^", "8", getEaa32(op), "b", op->data1, 0, "3");
    }

    else if (op->func==cmp8_reg) {
        genArithR("-", "FLAGS_CMP8", "8", r8(op->r1), op->data1, 0, 0, "1");
    } else if (op->func==cmp8_mem16) {
        genArithE("-", "FLAGS_CMP8", "8", getEaa16(op), "b", op->data1, 0, 0, "2");
    } else if (op->func==cmp8_mem32) {
        genArithE("-", "FLAGS_CMP8", "8", getEaa32(op), "b", op->data1, 0, 0, "2");
    } else {
        kpanic("gen080");
    }
}

void OPCALL add16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL add16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL add16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL add16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL or16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL or16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL or16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL or16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adc16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL adc16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL adc16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adc16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbb16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL sbb16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL sbb16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbb16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL and16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL and16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL and16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL and16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sub16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL sub16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL sub16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sub16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xor16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL xor16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL xor16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xor16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmp16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmp16_mem32(struct CPU* cpu, struct Op* op);
void gen081(struct Op* op) {
    if (op->func==add16_reg) {
        genArithR("+", "FLAGS_ADD16", "16", r16(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==add16_mem16) {
        genArithE("+", "FLAGS_ADD16", "16", getEaa16(op), "w", op->data1, 1, 0, "3");
    } else if (op->func==add16_mem32) {
        genArithE("+", "FLAGS_ADD16", "16", getEaa32(op), "w", op->data1, 1, 0, "3");
    } else if (op->func==add16_reg_noflags) {
        genArithR_noflags("+", "16", r16(op->r1), op->data1, 0, "1");
    } else if (op->func==add16_mem16_noflags) {
        genArithE_noflags("+", "16", getEaa16(op), "w", op->data1, 0, "3");
    } else if (op->func==add16_mem32_noflags) {
        genArithE_noflags("+", "16", getEaa32(op), "w", op->data1, 0, "3");
    }

    else if (op->func==or16_reg) {
        genArithR("|", "FLAGS_OR16", "16", r16(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==or16_mem16) {
        genArithE("|", "FLAGS_OR16", "16", getEaa16(op), "w", op->data1, 1, 0, "3");
    } else if (op->func==or16_mem32) {
        genArithE("|", "FLAGS_OR16", "16", getEaa32(op), "w", op->data1, 1, 0, "3");
    } else if (op->func==or16_reg_noflags) {
        genArithR_noflags("|", "16", r16(op->r1), op->data1, 0, "1");
    } else if (op->func==or16_mem16_noflags) {
        genArithE_noflags("|", "16", getEaa16(op), "w", op->data1, 0, "3");
    } else if (op->func==or16_mem32_noflags) {
        genArithE_noflags("|", "16", getEaa32(op), "w", op->data1, 0, "3");
    }

    else if (op->func==adc16_reg) {
        genArithR("+", "FLAGS_ADC16", "16", r16(op->r1), op->data1, 1, 1, "1");
    } else if (op->func==adc16_mem16) {
        genArithE("+", "FLAGS_ADC16", "16", getEaa16(op), "w", op->data1, 1, 1, "3");
    } else if (op->func==adc16_mem32) {
        genArithE("+", "FLAGS_ADC16", "16", getEaa32(op), "w", op->data1, 1, 1, "3");
    } else if (op->func==adc16_reg_noflags) {
        genArithR_noflags("+", "16", r16(op->r1), op->data1, 1, "1");
    } else if (op->func==adc16_mem16_noflags) {
        genArithE_noflags("+", "16", getEaa16(op), "w", op->data1, 1, "3");
    } else if (op->func==adc16_mem32_noflags) {
        genArithE_noflags("+", "16", getEaa32(op), "w", op->data1, 1, "3");
    }

    else if (op->func==sbb16_reg) {
        genArithR("-", "FLAGS_SBB16", "16", r16(op->r1), op->data1, 1, 1, "1");
    } else if (op->func==sbb16_mem16) {
        genArithE("-", "FLAGS_SBB16", "16", getEaa16(op), "w", op->data1, 1, 1, "3");
    } else if (op->func==sbb16_mem32) {
        genArithE("-", "FLAGS_SBB16", "16", getEaa32(op), "w", op->data1, 1, 1, "3");
    } else if (op->func==sbb16_reg_noflags) {
        genArithR_noflags("-", "16", r16(op->r1), op->data1, 1, "1");
    } else if (op->func==sbb16_mem16_noflags) {
        genArithE_noflags("-", "16", getEaa16(op), "w", op->data1, 1, "3");
    } else if (op->func==sbb16_mem32_noflags) {
        genArithE_noflags("-", "16", getEaa32(op), "w", op->data1, 1, "3");
    }

    else if (op->func==and16_reg) {
        genArithR("&", "FLAGS_AND16", "16", r16(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==and16_mem16) {
        genArithE("&", "FLAGS_AND16", "16", getEaa16(op), "w", op->data1, 1, 0, "3");
    } else if (op->func==and16_mem32) {
        genArithE("&", "FLAGS_AND16", "16", getEaa32(op), "w", op->data1, 1, 0, "3");
    } else if (op->func==and16_reg_noflags) {
        genArithR_noflags("&", "16", r16(op->r1), op->data1, 0, "1");
    } else if (op->func==and16_mem16_noflags) {
        genArithE_noflags("&", "16", getEaa16(op), "w", op->data1, 0, "3");
    } else if (op->func==and16_mem32_noflags) {
        genArithE_noflags("&", "16", getEaa32(op), "w", op->data1, 0, "3");
    }

    else if (op->func==sub16_reg) {
        genArithR("-", "FLAGS_SUB16", "16", r16(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==sub16_mem16) {
        genArithE("-", "FLAGS_SUB16", "16", getEaa16(op), "w", op->data1, 1, 0, "3");
    } else if (op->func==sub16_mem32) {
        genArithE("-", "FLAGS_SUB16", "16", getEaa32(op), "w", op->data1, 1, 0, "3");
    } else if (op->func==sub16_reg_noflags) {
        genArithR_noflags("-", "16", r16(op->r1), op->data1, 0, "1");
    } else if (op->func==sub16_mem16_noflags) {
        genArithE_noflags("-", "16", getEaa16(op), "w", op->data1, 0, "3");
    } else if (op->func==sub16_mem32_noflags) {
        genArithE_noflags("-", "16", getEaa32(op), "w", op->data1, 0, "3");
    }

    else if (op->func==xor16_reg) {
        genArithR("^", "FLAGS_XOR16", "16", r16(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==xor16_mem16) {
        genArithE("^", "FLAGS_XOR16", "16", getEaa16(op), "w", op->data1, 1, 0, "3");
    } else if (op->func==xor16_mem32) {
        genArithE("^", "FLAGS_XOR16", "16", getEaa32(op), "w", op->data1, 1, 0, "3");
    } else if (op->func==xor16_reg_noflags) {
        genArithR_noflags("^", "16", r16(op->r1), op->data1, 0, "1");
    } else if (op->func==xor16_mem16_noflags) {
        genArithE_noflags("^", "16", getEaa16(op), "w", op->data1, 0, "3");
    } else if (op->func==xor16_mem32_noflags) {
        genArithE_noflags("^", "16", getEaa32(op), "w", op->data1, 0, "3");
    }

    else if (op->func==cmp16_reg) {
        genArithR("-", "FLAGS_CMP16", "16", r16(op->r1), op->data1, 0, 0, "1");
    } else if (op->func==cmp16_mem16) {
        genArithE("-", "FLAGS_CMP16", "16", getEaa16(op), "w", op->data1, 0, 0, "2");
    } else if (op->func==cmp16_mem32) {
        genArithE("-", "FLAGS_CMP16", "16", getEaa32(op), "w", op->data1, 0, 0, "2");
    } else {
        kpanic("gen081");
    }
}

void OPCALL add32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL add32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL add32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL add32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL or32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL or32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL or32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL or32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adc32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL adc32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL adc32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adc32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbb32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL sbb32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL sbb32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbb32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL and32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL and32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL and32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL and32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sub32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL sub32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL sub32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sub32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xor32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL xor32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL xor32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xor32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmp32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmp32_mem32(struct CPU* cpu, struct Op* op);
void gen281(struct Op* op) {
    if (op->func==add32_reg) {
        genArithR("+", "FLAGS_ADD32", "32", r32(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==add32_mem16) {
        genArithE("+", "FLAGS_ADD32", "32", getEaa16(op), "d", op->data1, 1, 0, "3");
    } else if (op->func==add32_mem32) {
        genArithE("+", "FLAGS_ADD32", "32", getEaa32(op), "d", op->data1, 1, 0, "3");
    } else if (op->func==add32_reg_noflags) {
        genArithR_noflags("+", "32", r32(op->r1), op->data1, 0, "1");
    } else if (op->func==add32_mem16_noflags) {
        genArithE_noflags("+", "32", getEaa16(op), "d", op->data1, 0, "3");
    } else if (op->func==add32_mem32_noflags) {
        genArithE_noflags("+", "32", getEaa32(op), "d", op->data1, 0, "3");
    }

    else if (op->func==or32_reg) {
        genArithR("|", "FLAGS_OR32", "32", r32(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==or32_mem16) {
        genArithE("|", "FLAGS_OR32", "32", getEaa16(op), "d", op->data1, 1, 0, "3");
    } else if (op->func==or32_mem32) {
        genArithE("|", "FLAGS_OR32", "32", getEaa32(op), "d", op->data1, 1, 0, "3");
    } else if (op->func==or32_reg_noflags) {
        genArithR_noflags("|", "32", r32(op->r1), op->data1, 0, "1");
    } else if (op->func==or32_mem16_noflags) {
        genArithE_noflags("|", "32", getEaa16(op), "d", op->data1, 0, "3");
    } else if (op->func==or32_mem32_noflags) {
        genArithE_noflags("|", "32", getEaa32(op), "d", op->data1, 0, "3");
    }

    else if (op->func==adc32_reg) {
        genArithR("+", "FLAGS_ADC32", "32", r32(op->r1), op->data1, 1, 1, "1");
    } else if (op->func==adc32_mem16) {
        genArithE("+", "FLAGS_ADC32", "32", getEaa16(op), "d", op->data1, 1, 1, "3");
    } else if (op->func==adc32_mem32) {
        genArithE("+", "FLAGS_ADC32", "32", getEaa32(op), "d", op->data1, 1, 1, "3");
    } else if (op->func==adc32_reg_noflags) {
        genArithR_noflags("+", "32", r32(op->r1), op->data1, 1, "1");
    } else if (op->func==adc32_mem16_noflags) {
        genArithE_noflags("+", "32", getEaa16(op), "d", op->data1, 1, "3");
    } else if (op->func==adc32_mem32_noflags) {
        genArithE_noflags("+", "32", getEaa32(op), "d", op->data1, 1, "3");
    }

    else if (op->func==sbb32_reg) {
        genArithR("-", "FLAGS_SBB32", "32", r32(op->r1), op->data1, 1, 1, "1");
    } else if (op->func==sbb32_mem16) {
        genArithE("-", "FLAGS_SBB32", "32", getEaa16(op), "d", op->data1, 1, 1, "3");
    } else if (op->func==sbb32_mem32) {
        genArithE("-", "FLAGS_SBB32", "32", getEaa32(op), "d", op->data1, 1, 1, "3");
    } else if (op->func==sbb32_reg_noflags) {
        genArithR_noflags("-", "32", r32(op->r1), op->data1, 1, "1");
    } else if (op->func==sbb32_mem16_noflags) {
        genArithE_noflags("-", "32", getEaa16(op), "d", op->data1, 1, "3");
    } else if (op->func==sbb32_mem32_noflags) {
        genArithE_noflags("-", "32", getEaa32(op), "d", op->data1, 1, "3");
    }

    else if (op->func==and32_reg) {
        genArithR("&", "FLAGS_AND32", "32", r32(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==and32_mem16) {
        genArithE("&", "FLAGS_AND32", "32", getEaa16(op), "d", op->data1, 1, 0, "3");
    } else if (op->func==and32_mem32) {
        genArithE("&", "FLAGS_AND32", "32", getEaa32(op), "d", op->data1, 1, 0, "3");
    } else if (op->func==and32_reg_noflags) {
        genArithR_noflags("&", "32", r32(op->r1), op->data1, 0, "1");
    } else if (op->func==and32_mem16_noflags) {
        genArithE_noflags("&", "32", getEaa16(op), "d", op->data1, 0, "3");
    } else if (op->func==and32_mem32_noflags) {
        genArithE_noflags("&", "32", getEaa32(op), "d", op->data1, 0, "3");
    }

    else if (op->func==sub32_reg) {
        genArithR("-", "FLAGS_SUB32", "32", r32(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==sub32_mem16) {
        genArithE("-", "FLAGS_SUB32", "32", getEaa16(op), "d", op->data1, 1, 0, "3");
    } else if (op->func==sub32_mem32) {
        genArithE("-", "FLAGS_SUB32", "32", getEaa32(op), "d", op->data1, 1, 0, "3");
    } else if (op->func==sub32_reg_noflags) {
        genArithR_noflags("-", "32", r32(op->r1), op->data1, 0, "1");
    } else if (op->func==sub32_mem16_noflags) {
        genArithE_noflags("-", "32", getEaa16(op), "d", op->data1, 0, "3");
    } else if (op->func==sub32_mem32_noflags) {
        genArithE_noflags("-", "32", getEaa32(op), "d", op->data1, 0, "3");
    }

    else if (op->func==xor32_reg) {
        genArithR("^", "FLAGS_XOR32", "32", r32(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==xor32_mem16) {
        genArithE("^", "FLAGS_XOR32", "32", getEaa16(op), "d", op->data1, 1, 0, "3");
    } else if (op->func==xor32_mem32) {
        genArithE("^", "FLAGS_XOR32", "32", getEaa32(op), "d", op->data1, 1, 0, "3");
    } else if (op->func==xor32_reg_noflags) {
        genArithR_noflags("^", "32", r32(op->r1), op->data1, 0, "1");
    } else if (op->func==xor32_mem16_noflags) {
        genArithE_noflags("^", "32", getEaa16(op), "d", op->data1, 0, "3");
    } else if (op->func==xor32_mem32_noflags) {
        genArithE_noflags("^", "32", getEaa32(op), "d", op->data1, 0, "3");
    }

    else if (op->func==cmp32_reg) {
        genArithR("-", "FLAGS_CMP32", "32", r32(op->r1), op->data1, 0, 0, "1");
    } else if (op->func==cmp32_mem16) {
        genArithE("-", "FLAGS_CMP32", "32", getEaa16(op), "d", op->data1, 0, 0, "2");
    } else if (op->func==cmp32_mem32) {
        genArithE("-", "FLAGS_CMP32", "32", getEaa32(op), "d", op->data1, 0, 0, "2");
    } else {
        kpanic("gen0320");
    }
}

void OPCALL testr8r8(struct CPU* cpu, struct Op* op);
void OPCALL teste8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL teste8r8_32(struct CPU* cpu, struct Op* op);
void gen084(struct Op* op) {
    if (op->func == testr8r8) {
        genArithRR("&", "FLAGS_TEST8", "8", r8(op->r1), r8(op->r2), 0, 0, "1");
    } else if (op->func==teste8r8_16) {
        genArithER("&", "FLAGS_TEST8", "8", getEaa16(op), "b", r8(op->r1), 0, 0,"2");
    } else if (op->func==teste8r8_32) {
        genArithER("&", "FLAGS_TEST8", "8", getEaa32(op), "b", r8(op->r1), 0, 0,"2");
    } else {
        kpanic("gen084");
    }
}

void OPCALL testr16r16(struct CPU* cpu, struct Op* op);
void OPCALL teste16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL teste16r16_32(struct CPU* cpu, struct Op* op);
void gen085(struct Op* op) {
    if (op->func == testr16r16) {
        genArithRR("&", "FLAGS_TEST16", "16", r16(op->r1), r16(op->r2), 0, 0, "1");
    } else if (op->func==teste16r16_16) {
        genArithER("&", "FLAGS_TEST16", "16", getEaa16(op), "w", r16(op->r1), 0, 0,"2");
    } else if (op->func==teste16r16_32) {
        genArithER("&", "FLAGS_TEST16", "16", getEaa32(op), "w", r16(op->r1), 0, 0,"2");
    } else {
        kpanic("gen085");
    }
}

void OPCALL testr32r32(struct CPU* cpu, struct Op* op);
void OPCALL teste32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL teste32r32_32(struct CPU* cpu, struct Op* op);
void gen285(struct Op* op) {
    if (op->func == testr32r32) {
        genArithRR("&", "FLAGS_TEST32", "32", r32(op->r1), r32(op->r2), 0, 0, "1");
    } else if (op->func==teste32r32_16) {
        genArithER("&", "FLAGS_TEST32", "32", getEaa16(op), "d", r32(op->r1), 0, 0,"2");
    } else if (op->func==teste32r32_32) {
        genArithER("&", "FLAGS_TEST32", "32", getEaa32(op), "d", r32(op->r1), 0, 0,"2");
    } else {
        kpanic("gen285");
    }
}

void OPCALL xchgr8r8(struct CPU* cpu, struct Op* op);
void OPCALL xchge8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL xchge8r8_32(struct CPU* cpu, struct Op* op);
void gen086(struct Op* op) {
    if (op->func == xchgr8r8) {
        out("tmp8 = ");
        out(r8(op->r1));
        out("; ");
        out(r8(op->r1));
        out(" = ");
        out(r8(op->r2));
        out("; ");
        out(r8(op->r2));
        out(" = tmp8; CYCLES(3);");
    } else {        
        out("eaa = ");
        if (op->func == xchge8r8_16) {
            out(getEaa16(op));
        } else if (op->func == xchge8r8_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen086");
        }
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, ");
        out(r8(op->r1));
        out("); ");
        out(r8(op->r1));
        out(" = tmp8; CYCLES(4);");
    }
}

void OPCALL xchgr16r16(struct CPU* cpu, struct Op* op);
void OPCALL xchge16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL xchge16r16_32(struct CPU* cpu, struct Op* op);
void gen087(struct Op* op) {
    if (op->func == xchgr16r16) {
        out("tmp16 = ");
        out(r16(op->r1));
        out("; ");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out("; ");
        out(r16(op->r2));
        out(" = tmp16; CYCLES(3);");
    } else {        
        out("eaa = ");
        if (op->func == xchge16r16_16) {
            out(getEaa16(op));
        } else if (op->func == xchge16r16_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen087");
        }
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, ");
        out(r16(op->r1));
        out("); ");
        out(r16(op->r1));
        out(" = tmp16; CYCLES(4);");
    }
}

void OPCALL xchgr32r32(struct CPU* cpu, struct Op* op);
void OPCALL xchge32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL xchge32r32_32(struct CPU* cpu, struct Op* op);
void gen287(struct Op* op) {
    if (op->func == xchgr32r32) {
        out("tmp32 = ");
        out(r32(op->r1));
        out("; ");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out("; ");
        out(r32(op->r2));
        out(" = tmp32; CYCLES(3);");
    } else {        
        out("eaa = ");
        if (op->func == xchge32r32_16) {
            out(getEaa16(op));
        } else if (op->func == xchge32r32_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen287");
        }
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, ");
        out(r32(op->r1));
        out("); ");
        out(r32(op->r1));
        out(" = tmp32; CYCLES(4);");
    }
}


void OPCALL movr8r8(struct CPU* cpu, struct Op* op);
void OPCALL move8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL move8r8_32(struct CPU* cpu, struct Op* op);
void gen088(struct Op* op) {
    if (op->func == movr8r8) {
        out(r8(op->r1));
        out(" = ");
        out(r8(op->r2));
        out("; CYCLES(1);");
    } else {                
        out("writeb(cpu->memory, ");
        if (op->func == move8r8_16) {
            out(getEaa16(op));
        } else if (op->func == move8r8_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen088");
        }
        out(", ");
        out(r8(op->r1));
        out(");CYCLES(1);");
    }
}

void OPCALL movr16r16(struct CPU* cpu, struct Op* op);
void OPCALL move16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL move16r16_32(struct CPU* cpu, struct Op* op);
void gen089(struct Op* op) {
    if (op->func == movr16r16) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out("; CYCLES(1);");
    } else {        
        out("writew(cpu->memory, ");
        if (op->func == move16r16_16) {
            out(getEaa16(op));
        } else if (op->func == move16r16_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen089");
        }
        out(", ");
        out(r16(op->r1));
        out(");CYCLES(1);");
    }
}

void OPCALL movr32r32(struct CPU* cpu, struct Op* op);
void OPCALL move32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL move32r32_32(struct CPU* cpu, struct Op* op);
void gen289(struct Op* op) {
    if (op->func == movr32r32) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out("; CYCLES(1);");
    } else {        
        out("writed(cpu->memory, ");
        if (op->func == move32r32_16) {
            out(getEaa16(op));
        } else if (op->func == move32r32_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen289");
        }
        out(", ");
        out(r32(op->r1));
        out(");CYCLES(1);");
    }
}

void OPCALL movr8e8_16(struct CPU* cpu, struct Op* op);
void OPCALL movr8e8_32(struct CPU* cpu, struct Op* op);
void gen08a(struct Op* op) {
    if (op->func == movr8r8) {
        out(r8(op->r1));
        out(" = ");
        out(r8(op->r2));
        out("; CYCLES(1);");
    } else {                
        out(r8(op->r1));
        out(" = readb(cpu->memory, ");
        if (op->func == movr8e8_16) {
            out(getEaa16(op));
        } else if (op->func == movr8e8_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen08a");
        }
        out(");CYCLES(1);");
    }
}

void OPCALL movr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL movr16e16_32(struct CPU* cpu, struct Op* op);
void gen08b(struct Op* op) {
    if (op->func == movr16r16) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out("; CYCLES(1);");
    } else {                
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        if (op->func == movr16e16_16) {
            out(getEaa16(op));
        } else if (op->func == movr16e16_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen08b");
        }
        out(");CYCLES(1);");
    }
}

void OPCALL movr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL movr32e32_32(struct CPU* cpu, struct Op* op);
void gen28b(struct Op* op) {
    if (op->func == movr32r32) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out("; CYCLES(1);");
    } else {                
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        if (op->func == movr32e32_16) {
            out(getEaa16(op));
        } else if (op->func == movr32e32_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen28b");
        }
        out(");CYCLES(1);");
    }
}

void OPCALL movr16s16(struct CPU* cpu, struct Op* op);
void OPCALL movr32s16(struct CPU* cpu, struct Op* op);
void OPCALL move16s16_16(struct CPU* cpu, struct Op* op);
void OPCALL move16s16_32(struct CPU* cpu, struct Op* op);
void gen08c(struct Op* op) {
    if (op->func == movr16s16) {
        out(r16(op->r1));
        out(" = ");
        out("cpu->segValue[");
        out(getBase(op->r2));
        out("]; CYCLES(1);");
    } else if (op->func == movr32s16) {
        out(r32(op->r1));
        out(" = ");
        out("cpu->segValue[");
        out(getBase(op->r2));
        out("]; CYCLES(1);");
    } else {                
        out("writew(cpu->memory, ");
        if (op->func == move16s16_16) {
            out(getEaa16(op));
        } else if (op->func == move16s16_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen08b");
        }
        out(", ");
        out("cpu->segValue[");
        out(getBase(op->r2));
        out("]);CYCLES(1);");
    }
}

void OPCALL lear16_32(struct CPU* cpu, struct Op* op);
void OPCALL lear16_16(struct CPU* cpu, struct Op* op);
void gen08d(struct Op* op) {
    out(r16(op->r1));
    out(" = ");
    if (op->func == lear16_16) {
        out(getEaa16(op));        
    } else if (op->func == lear16_32) {        
        out(getEaa32(op));
    } else {
        kpanic("gen08d");
    }
    out("; CYCLES(1);");
}

void OPCALL lear32_16(struct CPU* cpu, struct Op* op);
void OPCALL lear32_32(struct CPU* cpu, struct Op* op);
void gen28d(struct Op* op) {
    out(r32(op->r1));
    out(" = ");
    if (op->func == lear32_16) {
        out(getEaa16(op));        
    } else if (op->func == lear32_32) {        
        out(getEaa32(op));
    } else {
        kpanic("gen28d");
    }
    out("; CYCLES(1);");
}

void OPCALL movs16r16(struct CPU* cpu, struct Op* op);
void OPCALL movs16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL movs16e16_32(struct CPU* cpu, struct Op* op);
void gen08e(struct Op* op) {
    if (op->func == movs16r16) {
        out("cpu->segValue[");
        out(getBase(op->r2));
        out("] = ");
        out(r16(op->r1));
        out("; cpu->segAddress[");
        out(getBase(op->r2));
        out("] = cpu->thread->process->ldt[cpu->segValue[");
        out(getBase(op->r2));
        out("] >> 3].base_addr; CYCLES(2);");
    } else {                
        out("cpu->segValue[");
        out(getBase(op->r2));
        out("] = readw(cpu->memory, ");
        if (op->func == movs16e16_16) {
            out(getEaa16(op));
        } else if (op->func == movs16e16_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen08e");
        }
        out("); cpu->segAddress[");
        out(getBase(op->r2));
        out("] = cpu->thread->process->ldt[cpu->segValue[");
        out(getBase(op->r2));
        out("] >> 3].base_addr; CYCLES(3);");
    }
}

void OPCALL popReg16(struct CPU* cpu, struct Op* op);
void OPCALL pope16_16(struct CPU* cpu, struct Op* op);
void OPCALL pope16_32(struct CPU* cpu, struct Op* op);
void gen08f(struct Op* op) {
    if (op->func == popReg16) {
        out(r16(op->r1));
        out(" = pop16(cpu); CYCLES(1);");
    } else {                
        out("writew(cpu->memory, ");
        if (op->func == pope16_16) {
            out(getEaa16(op));
        } else if (op->func == pope16_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen08f");
        }
        out(", pop16(cpu)); CYCLES(3);");
    }
}

void OPCALL popReg32(struct CPU* cpu, struct Op* op);
void OPCALL pope32_16(struct CPU* cpu, struct Op* op);
void OPCALL pope32_32(struct CPU* cpu, struct Op* op);
void gen28f(struct Op* op) {
    if (op->func == popReg32) {
        out(r32(op->r1));
        out(" = pop32(cpu); CYCLES(1);");
    } else {                
        out("writed(cpu->memory, ");
        if (op->func == pope32_16) {
            out(getEaa16(op));
        } else if (op->func == pope32_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen28f");
        }
        out(", pop32(cpu)); CYCLES(3);");
    }
}

void gen090(struct Op* op) {
    out("CYCLES(1);");
}

void gen091(struct Op* op) {
    out("tmp16 = ");
    out(r16(op->r1));
    out("; ");
    out(r16(op->r1));
    out(" = ");
    out(r16(op->r2));
    out("; ");
    out(r16(op->r2));
    out(" = tmp16; CYCLES(3);");
}

void gen291(struct Op* op) {
    out("tmp32 = ");
    out(r32(op->r1));
    out("; ");
    out(r32(op->r1));
    out(" = ");
    out(r32(op->r2));
    out("; ");
    out(r32(op->r2));
    out(" = tmp32; CYCLES(3);");
}

void gen098(struct Op* op) {
    out("AX = (S8)AL; CYCLES(3);");
}

void gen298(struct Op* op) {
    out("EAX = (S16)AX; CYCLES(3);");
}

void gen099(struct Op* op) {
    out ("if (((S16)AX) < 0) DX = 0xFFFF; else DX = 0; CYCLES(2);");
}

void gen299(struct Op* op) {
    out("if (((S32)EAX) < 0) EDX = 0xFFFFFFFF; else EDX = 0; CYCLES(2);");
}

void gen09a(struct Op* op) {
    char tmp[16];

    out("cpu->eip.u32+=");
    itoa(op->eipCount, tmp, 10);
    out(tmp);
	out(";cpu_call(cpu, 0, 0x");
    itoa(op->eData, tmp, 16);
    out(tmp);
    out(", 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out(", cpu->eip.u32); CYCLES(4);");
}

void gen09c(struct Op* op) {
    out("fillFlags(cpu); push16(cpu, cpu->flags|2); CYCLES(3);");
}

void gen29c(struct Op* op) {
    out("fillFlags(cpu); push32(cpu, (cpu->flags|2) & 0xFCFFFF); CYCLES(3);");
}

void gen09d(struct Op* op) {
    out("cpu->lazyFlags = FLAGS_NONE; setFlags(cpu, pop16(cpu), FMASK_ALL & 0xFFFF); CYCLES(4);");
}

void gen29d(struct Op* op) {
    out("cpu->lazyFlags = FLAGS_NONE; setFlags(cpu, pop32(cpu), FMASK_ALL); CYCLES(4);");
}

void gen09e(struct Op* op) {
    out("fillFlags(cpu); setFlags(cpu, AH, FMASK_ALL & 0xFF); CYCLES(2);");
}

void gen09f(struct Op* op) {
    out("fillFlags(cpu); AH = cpu->flags & (SF|ZF|AF|PF|CF); CYCLES(2);");
}

void gen0a0(struct Op* op) {
    char tmp[16];

    out("AL = readb(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out("); CYCLES(1);");
}

void gen0a1(struct Op* op) {
    char tmp[16];

    out("AX = readw(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out("); CYCLES(1);");
}

void gen2a1(struct Op* op) {
    char tmp[16];

    out("EAX = readd(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out("); CYCLES(1);");
}

void gen0a2(struct Op* op) {
    char tmp[16];

    out("writeb(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out(", AL); CYCLES(1);");
}

void gen0a3(struct Op* op) {
    char tmp[16];

    out("writew(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out(", AX); CYCLES(1);");
}

void gen2a3(struct Op* op) {
    char tmp[16];

    out("writed(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out(", EAX); CYCLES(1);");
}

void OPCALL movsb16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL movsb16_op(struct CPU* cpu, struct Op* op);
void OPCALL movsb32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL movsb32_op(struct CPU* cpu, struct Op* op);
void gen0a4(struct Op* op) {
    if (op->func==movsb16_r_op) {
        out("movsb16_r(cpu, ");        
    } else if (op->func==movsb16_op) {
        out("movsb16(cpu, ");
    } else if (op->func==movsb32_r_op) {
        out("movsb32_r(cpu, ");
    } else if (op->func==movsb32_op) {
        out("movsb32(cpu, ");
    } else {
        kpanic("gen0a4");
    }
    out(getBase(op->base));
    out(");");
}

void OPCALL movsw16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL movsw16_op(struct CPU* cpu, struct Op* op);
void OPCALL movsw32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL movsw32_op(struct CPU* cpu, struct Op* op);
void gen0a5(struct Op* op) {
    if (op->func==movsw16_r_op) {
        out("movsw16_r(cpu, ");        
    } else if (op->func==movsw16_op) {
        out("movsw16(cpu, ");
    } else if (op->func==movsw32_r_op) {
        out("movsw32_r(cpu, ");
    } else if (op->func==movsw32_op) {
        out("movsw32(cpu, ");
    } else {
        kpanic("gen0a5");
    }
    out(getBase(op->base));
    out(");");
}

void OPCALL movsd16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL movsd16_op(struct CPU* cpu, struct Op* op);
void OPCALL movsd32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL movsd32_op(struct CPU* cpu, struct Op* op);
void gen2a5(struct Op* op) {
    if (op->func==movsd16_r_op) {
        out("movsd16_r(cpu, ");
    } else if (op->func==movsd16_op) {
        out("movsd16(cpu, ");
    } else if (op->func==movsd32_r_op) {
        out("movsd32_r(cpu, ");
    } else if (op->func==movsd32_op) {
        out("movsd32(cpu, ");        
    } else {
        kpanic("gen2a5");
    }
    out(getBase(op->base));
    out(");");
}

void OPCALL cmpsb16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL cmpsb16_op(struct CPU* cpu, struct Op* op);
void OPCALL cmpsb32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL cmpsb32_op(struct CPU* cpu, struct Op* op);
void gen0a6(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    if (op->func==cmpsb16_r_op) {
        out("cmpsb16_r(cpu, ");        
    } else if (op->func==cmpsb16_op) {
        out("cmpsb16(cpu, ");
    } else if (op->func==cmpsb32_r_op) {
        out("cmpsb32_r(cpu, ");
    } else if (op->func==cmpsb32_op) {
        out("cmpsb32(cpu, ");
    } else {
        kpanic("gen0a6");
    }
    out(tmp);
    out(", ");
    out(getBase(op->base));
    out(");");
}

void OPCALL cmpsw16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL cmpsw16_op(struct CPU* cpu, struct Op* op);
void OPCALL cmpsw32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL cmpsw32_op(struct CPU* cpu, struct Op* op);
void gen0a7(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    if (op->func==cmpsw16_r_op) {
        out("cmpsw16_r(cpu, ");
    } else if (op->func==cmpsw16_op) {
        out("cmpsw16(cpu, ");
    } else if (op->func==cmpsw32_r_op) {
        out("cmpsw32_r(cpu, ");
    } else if (op->func==cmpsw32_op) {
        out("cmpsw32(cpu, ");
    } else {
        kpanic("gen0a7");
    }
    out(tmp);
    out(", ");
    out(getBase(op->base));
    out(");");
}

void OPCALL cmpsd16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL cmpsd16_op(struct CPU* cpu, struct Op* op);
void OPCALL cmpsd32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL cmpsd32_op(struct CPU* cpu, struct Op* op);
void gen2a7(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    if (op->func==cmpsd16_r_op) {
        out("cmpsd16_r(cpu, ");
    } else if (op->func==cmpsd16_op) {
        out("cmpsd16(cpu, ");
    } else if (op->func==cmpsd32_r_op) {
        out("cmpsd32_r(cpu, ");
    } else if (op->func==cmpsd32_op) {
        out("cmpsd32(cpu, ");
    } else {
        kpanic("gen2a7");
    }
    out(tmp);
    out(", ");
    out(getBase(op->base));
    out(");");
}

void OPCALL stosb16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL stosb16_op(struct CPU* cpu, struct Op* op);
void OPCALL stosb32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL stosb32_op(struct CPU* cpu, struct Op* op);
void gen0aa(struct Op* op) {
    if (op->func==stosb16_r_op) {
        out("stosb16_r(cpu);");        
    } else if (op->func==stosb16_op) {
        out("stosb16(cpu);");
    } else if (op->func==stosb32_r_op) {
        out("stosb32_r(cpu);");
    } else if (op->func==stosb32_op) {
        out("stosb32(cpu);");
    } else {
        kpanic("gen0aa");
    }
}

void OPCALL stosw16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL stosw16_op(struct CPU* cpu, struct Op* op);
void OPCALL stosw32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL stosw32_op(struct CPU* cpu, struct Op* op);
void gen0ab(struct Op* op) {
    if (op->func==stosw16_r_op) {
        out("stosw16_r(cpu);");
    } else if (op->func==stosw16_op) {
        out("stosw16(cpu);");
    } else if (op->func==stosw32_r_op) {
        out("stosw32_r(cpu);");
    } else if (op->func==stosw32_op) {
        out("stosw32(cpu);");
    } else {
        kpanic("gen0ab");
    }
}

void OPCALL stosd16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL stosd16_op(struct CPU* cpu, struct Op* op);
void OPCALL stosd32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL stosd32_op(struct CPU* cpu, struct Op* op);
void gen2ab(struct Op* op) {
    if (op->func==stosd16_r_op) {
        out("stosd16_r(cpu);");
    } else if (op->func==stosd16_op) {
        out("stosd16(cpu);");
    } else if (op->func==stosd32_r_op) {
        out("stosd32_r(cpu);");
    } else if (op->func==stosd32_op) {
        out("stosd32(cpu);");
    } else {
        kpanic("gen2ab");
    }
}

void OPCALL lodsb16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsb16_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsb32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsb32_op(struct CPU* cpu, struct Op* op);
void gen0ac(struct Op* op) {
    if (op->func==lodsb16_r_op) {
        out("lodsb16_r(cpu, ");        
    } else if (op->func==lodsb16_op) {
        out("lodsb16(cpu, ");
    } else if (op->func==lodsb32_r_op) {
        out("lodsb32_r(cpu, ");
    } else if (op->func==lodsb32_op) {
        out("lodsb32(cpu, ");
    } else {
        kpanic("gen0ac");
    }
    out(getBase(op->base));
    out(");");
}

void OPCALL lodsw16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsw16_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsw32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsw32_op(struct CPU* cpu, struct Op* op);
void gen0ad(struct Op* op) {
    if (op->func==lodsw16_r_op) {
        out("lodsw16_r(cpu, ");        
    } else if (op->func==lodsw16_op) {
        out("lodsw16(cpu, ");
    } else if (op->func==lodsw32_r_op) {
        out("lodsw32_r(cpu, ");
    } else if (op->func==lodsw32_op) {
        out("lodsw32(cpu, ");
    } else {
        kpanic("gen0ad");
    }
    out(getBase(op->base));
    out(");");
}

void OPCALL lodsd16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsd16_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsd32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsd32_op(struct CPU* cpu, struct Op* op);
void gen2ad(struct Op* op) {
    if (op->func==lodsd16_r_op) {
        out("lodsd16_r(cpu, ");
    } else if (op->func==lodsd16_op) {
        out("lodsd16(cpu, ");
    } else if (op->func==lodsd32_r_op) {
        out("lodsd32_r(cpu, ");
    } else if (op->func==lodsd32_op) {
        out("lodsd32(cpu, ");        
    } else {
        kpanic("gen2ad");
    }
    out(getBase(op->base));
    out(");");
}

void OPCALL scasb16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasb16_op(struct CPU* cpu, struct Op* op);
void OPCALL scasb32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasb32_op(struct CPU* cpu, struct Op* op);
void gen0ae(struct Op* op) {
    if (op->func==scasb16_r_op) {
        out("scasb16_r(cpu, ");        
    } else if (op->func==scasb16_op) {
        out("scasb16(cpu, ");
    } else if (op->func==scasb32_r_op) {
        out("scasb32_r(cpu, ");
    } else if (op->func==scasb32_op) {
        out("scasb32(cpu, ");
    } else {
        kpanic("gen0a4");
    }
    out(getBase(op->data1));
    out(");");
}

void OPCALL scasw16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasw16_op(struct CPU* cpu, struct Op* op);
void OPCALL scasw32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasw32_op(struct CPU* cpu, struct Op* op);
void gen0af(struct Op* op) {
    if (op->func==scasw16_r_op) {
        out("scasw16_r(cpu, ");        
    } else if (op->func==scasw16_op) {
        out("scasw16(cpu, ");
    } else if (op->func==scasw32_r_op) {
        out("scasw32_r(cpu, ");
    } else if (op->func==scasw32_op) {
        out("scasw32(cpu, ");
    } else {
        kpanic("gen0a5");
    }
    out(getBase(op->data1));
    out(");");
}

void OPCALL scasd16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasd16_op(struct CPU* cpu, struct Op* op);
void OPCALL scasd32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasd32_op(struct CPU* cpu, struct Op* op);
void gen2af(struct Op* op) {
    if (op->func==scasd16_r_op) {
        out("scasd16_r(cpu, ");
    } else if (op->func==scasd16_op) {
        out("scasd16(cpu, ");
    } else if (op->func==scasd32_r_op) {
        out("scasd32_r(cpu, ");
    } else if (op->func==scasd32_op) {
        out("scasd32(cpu, ");        
    } else {
        kpanic("gen2a5");
    }
    out(getBase(op->data1));
    out(");");
}


void gen0a8(struct Op* op) {
    genArithR("&", "FLAGS_TEST8", "8", r8(0), op->data1, 0, 0, "1");
}

void gen0a9(struct Op* op) {
    genArithR("&", "FLAGS_TEST16", "16", r16(0), op->data1, 0, 0, "1");
}

void gen2a9(struct Op* op) {
    genArithR("&", "FLAGS_TEST32", "32", r16(0), op->data1, 0, 0, "1");
}

void gen0b0(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    out(r8(op->r1));
    out(" = 0x");
    out(tmp);
    out("; CYCLES(1);");
}

void gen0b8(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    out(r16(op->r1));
    out(" = 0x");
    out(tmp);
    out("; CYCLES(1);");
}

void gen2b8(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    out(r32(op->r1));
    out(" = 0x");
    out(tmp);
    out("; CYCLES(1);");
}

void OPCALL rol8_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rol8_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rol8_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL ror8_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL ror8_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL ror8_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl8_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl8_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl8_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr8_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr8_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr8_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shl8_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shl8_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shl8_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shr8_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shr8_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shr8_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL sar8_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL sar8_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL sar8_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rol8_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror8_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl8_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr8_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl8_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr8_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar8_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar8_mem32_noflags(struct CPU* cpu, struct Op* op);
void gen0c0(struct Op* op) {
    char reg[16];
    char value[16];
    itoa(op->r1, reg, 10);
    itoa(op->data1, value, 10);

    if (op->func == rol8_reg_op) {
        out("rol8_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rol8_mem16_op) {
        out("rol8_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rol8_mem32_op) {
        out("rol8_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == ror8_reg_op) {
        out("ror8_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == ror8_mem16_op) {
        out("rol8_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == ror8_mem32_op) {
        out("rol8_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcl8_reg_op) {
        out("rcl8_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcl8_mem16_op) {
        out("rcl8_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcl8_mem32_op) {
        out("rcl8_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcr8_reg_op) {
        out("rcr8_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcr8_mem16_op) {
        out("rcl8_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcr8_mem32_op) {
        out("rcl8_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shl8_reg_op) {
        out("shl8_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shl8_mem16_op) {
        out("shl8_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shl8_mem32_op) {
        out("shl8_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shr8_reg_op) {
        out("shr8_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shr8_mem16_op) {
        out("shr8_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shr8_mem32_op) {
        out("shr8_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == sar8_reg_op) {
        out("sar8_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == sar8_mem16_op) {
        out("sar8_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == sar8_mem32_op) {
        out("sar8_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    }

    else if (op->func == rol8_reg_noflags) {
        out(r8(op->r1));
        out(" = (");
        out(r8(op->r1));
        out(" << ");
        out(value);
        out(") | (");
        out(r8(op->r1));
        out(" >> (8 - ");
        out(value);
        out(")); CYCLES(1);");
    } else if (op->func == rol8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << ");
        out(value);
        out(") | (tmp8 >> (8 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == rol8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << ");
        out(value);
        out(") | (tmp8 >> (8 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == ror8_reg_noflags) {
        out(r8(op->r1));
        out(" = (");
        out(r8(op->r1));
        out(" >> ");
        out(value);
        out(") | (");
        out(r8(op->r1));
        out(" << (8 - ");
        out(value);
        out(")); CYCLES(1);");
    } else if (op->func == ror8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> ");
        out(value);
        out(") | (tmp8 << (8 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == ror8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> ");
        out(value);
        out(") | (tmp8 << (8 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == rcl8_reg_noflags) {
        out(r8(op->r1));
        out(" = (");
        out(r8(op->r1));
        out(" << ");
        out(value);
        out(") | (");
        out(r8(op->r1));
        out(" >> (9 - ");
        out(value);
        out(")) | (getCF(cpu) << (");
        out(value);
        out(" - 1)); CYCLES(8);");
    } else if (op->func == rcl8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << ");
        out(value);
        out(") | (");
        out(r8(op->r1));
        out(" >> (9 - ");
        out(value);
        out(")) | (getCF(cpu) << (");
        out(value);
        out(" - 1))); CYCLES(10);");
    } else if (op->func == rcl8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << ");
        out(value);
        out(") | (");
        out(r8(op->r1));
        out(" >> (9 - ");
        out(value);
        out(")) | (getCF(cpu) << (");
        out(value);
        out(" - 1))); CYCLES(10);");
    } else if (op->func == rcr8_reg_noflags) {
        out(r8(op->r1));
        out(" = (");
        out(r8(op->r1));
        out(" >> ");
        out(value);
        out(") | (");
        out(r8(op->r1));
        out(" << (9 - ");
        out(value);
        out(")) | (getCF(cpu) << (8 - ");
        out(value);
        out(")); CYCLES(8);");
    } else if (op->func == rcr8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> ");
        out(value);
        out(") | (");
        out(r8(op->r1));
        out(" << (9 - ");
        out(value);
        out(")) | (getCF(cpu) << (8 - ");
        out(value);
        out("))); CYCLES(10);");        
    } else if (op->func == rcr8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> ");
        out(value);
        out(") | (");
        out(r8(op->r1));
        out(" << (9 - ");
        out(value);
        out(")) | (getCF(cpu) << (8 - ");
        out(value);
        out("))); CYCLES(10);"); 
    } else if (op->func == shl8_reg_noflags) {
        out(r8(op->r1));
        out(" = ");
        out(r8(op->r1));
        out(" << ");
        out(value);
        out("; CYCLES(1);");
    } else if (op->func == shl8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) << ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == shl8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) << ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == shr8_reg_noflags) {
        out(r8(op->r1));
        out(" = ");
        out(r8(op->r1));
        out(" >> ");
        out(value);
        out("; CYCLES(1);");
    } else if (op->func == shr8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == shr8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == sar8_reg_noflags) {
        out(r8(op->r1));
        out(" = (S8)");
        out(r8(op->r1));
        out(" >> ");
        out(value);
        out("; CYCLES(1);");
    } else if (op->func == sar8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writeb(cpu->memory, eaa, (S8)readb(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == sar8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writeb(cpu->memory, eaa, (S8)readb(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else {
        kpanic("gen0c0");
    }
}

void OPCALL rol16_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rol16_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rol16_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL ror16_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL ror16_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL ror16_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl16_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl16_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl16_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr16_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr16_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr16_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shl16_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shl16_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shl16_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shr16_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shr16_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shr16_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL sar16_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL sar16_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL sar16_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rol16_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror16_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl16_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr16_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl16_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr16_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar16_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar16_mem32_noflags(struct CPU* cpu, struct Op* op);
void gen0c1(struct Op* op) {
    char reg[16];
    char value[16];
    itoa(op->r1, reg, 10);
    itoa(op->data1, value, 10);

    if (op->func == rol16_reg_op) {
        out("rol16_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rol16_mem16_op) {
        out("rol16_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rol16_mem32_op) {
        out("rol16_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == ror16_reg_op) {
        out("ror16_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == ror16_mem16_op) {
        out("rol16_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == ror16_mem32_op) {
        out("rol16_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcl16_reg_op) {
        out("rcl16_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcl16_mem16_op) {
        out("rcl16_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcl16_mem32_op) {
        out("rcl16_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcr16_reg_op) {
        out("rcr16_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcr16_mem16_op) {
        out("rcl16_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcr16_mem32_op) {
        out("rcl16_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shl16_reg_op) {
        out("shl16_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shl16_mem16_op) {
        out("shl16_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shl16_mem32_op) {
        out("shl16_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shr16_reg_op) {
        out("shr16_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shr16_mem16_op) {
        out("shr16_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shr16_mem32_op) {
        out("shr16_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == sar16_reg_op) {
        out("sar16_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == sar16_mem16_op) {
        out("sar16_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == sar16_mem32_op) {
        out("sar16_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    }

    else if (op->func == rol16_reg_noflags) {
        out(r16(op->r1));
        out(" = (");
        out(r16(op->r1));
        out(" << ");
        out(value);
        out(") | (");
        out(r16(op->r1));
        out(" >> (16 - ");
        out(value);
        out(")); CYCLES(1);");
    } else if (op->func == rol16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << ");
        out(value);
        out(") | (tmp16 >> (16 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == rol16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << ");
        out(value);
        out(") | (tmp16 >> (16 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == ror16_reg_noflags) {
        out(r16(op->r1));
        out(" = (");
        out(r16(op->r1));
        out(" >> ");
        out(value);
        out(") | (");
        out(r16(op->r1));
        out(" << (16 - ");
        out(value);
        out(")); CYCLES(1);");
    } else if (op->func == ror16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> ");
        out(value);
        out(") | (tmp16 << (16 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == ror16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> ");
        out(value);
        out(") | (tmp16 << (16 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == rcl16_reg_noflags) {
        out(r16(op->r1));
        out(" = (");
        out(r16(op->r1));
        out(" << ");
        out(value);
        out(") | (");
        out(r16(op->r1));
        out(" >> (17 - ");
        out(value);
        out(")) | (getCF(cpu) << (");
        out(value);
        out(" - 1)); CYCLES(8);");
    } else if (op->func == rcl16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << ");
        out(value);
        out(") | (");
        out(r16(op->r1));
        out(" >> (17 - ");
        out(value);
        out(")) | (getCF(cpu) << (");
        out(value);
        out(" - 1))); CYCLES(10);");
    } else if (op->func == rcl16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << ");
        out(value);
        out(") | (");
        out(r16(op->r1));
        out(" >> (17 - ");
        out(value);
        out(")) | (getCF(cpu) << (");
        out(value);
        out(" - 1))); CYCLES(10);");
    } else if (op->func == rcr16_reg_noflags) {
        out(r16(op->r1));
        out(" = (");
        out(r16(op->r1));
        out(" >> ");
        out(value);
        out(") | (");
        out(r16(op->r1));
        out(" << (17 - ");
        out(value);
        out(")) | (getCF(cpu) << (16 - ");
        out(value);
        out(")); CYCLES(8);");
    } else if (op->func == rcr16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> ");
        out(value);
        out(") | (");
        out(r16(op->r1));
        out(" << (17 - ");
        out(value);
        out(")) | (getCF(cpu) << (16 - ");
        out(value);
        out("))); CYCLES(10);");        
    } else if (op->func == rcr16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> ");
        out(value);
        out(") | (");
        out(r16(op->r1));
        out(" << (17 - ");
        out(value);
        out(")) | (getCF(cpu) << (16 - ");
        out(value);
        out("))); CYCLES(10);"); 
    } else if (op->func == shl16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" << ");
        out(value);
        out("; CYCLES(1);");
    } else if (op->func == shl16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) << ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == shl16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) << ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == shr16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" >> ");
        out(value);
        out("; CYCLES(1);");
    } else if (op->func == shr16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == shr16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == sar16_reg_noflags) {
        out(r16(op->r1));
        out(" = (S16)");
        out(r16(op->r1));
        out(" >> ");
        out(value);
        out("; CYCLES(1);");
    } else if (op->func == sar16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writew(cpu->memory, eaa, (S16)readw(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == sar16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writew(cpu->memory, eaa, (S16)readw(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else {
        kpanic("gen0c1");
    }
}

void OPCALL rol32_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rol32_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rol32_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL ror32_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL ror32_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL ror32_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl32_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl32_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl32_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr32_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr32_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr32_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shl32_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shl32_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shl32_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shr32_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shr32_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shr32_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL sar32_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL sar32_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL sar32_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rol32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar32_mem32_noflags(struct CPU* cpu, struct Op* op);
void gen2c1(struct Op* op) {
    char reg[16];
    char value[16];
    itoa(op->r1, reg, 10);
    itoa(op->data1, value, 10);

    if (op->func == rol32_reg_op) {
        out("rol32_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rol32_mem16_op) {
        out("rol32_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rol32_mem32_op) {
        out("rol32_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == ror32_reg_op) {
        out("ror32_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == ror32_mem16_op) {
        out("rol32_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == ror32_mem32_op) {
        out("rol32_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcl32_reg_op) {
        out("rcl32_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcl32_mem16_op) {
        out("rcl32_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcl32_mem32_op) {
        out("rcl32_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcr32_reg_op) {
        out("rcr32_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcr32_mem16_op) {
        out("rcl32_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == rcr32_mem32_op) {
        out("rcl32_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shl32_reg_op) {
        out("shl32_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shl32_mem16_op) {
        out("shl32_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shl32_mem32_op) {
        out("shl32_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shr32_reg_op) {
        out("shr32_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shr32_mem16_op) {
        out("shr32_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == shr32_mem32_op) {
        out("shr32_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == sar32_reg_op) {
        out("sar32_reg(cpu, ");
        out(reg);
        out(", ");
        out(value);
        out(");");
    } else if (op->func == sar32_mem16_op) {
        out("sar32_mem16(cpu, ");
        out(getEaa16(op));
        out(", ");
        out(value);
        out(");");
    } else if (op->func == sar32_mem32_op) {
        out("sar32_mem32(cpu, ");
        out(getEaa32(op));
        out(", ");
        out(value);
        out(");");
    }

    else if (op->func == rol32_reg_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" << ");
        out(value);
        out(") | (");
        out(r32(op->r1));
        out(" >> (32 - ");
        out(value);
        out(")); CYCLES(1);");
    } else if (op->func == rol32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << ");
        out(value);
        out(") | (tmp32 >> (32 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == rol32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << ");
        out(value);
        out(") | (tmp32 >> (32 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == ror32_reg_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" >> ");
        out(value);
        out(") | (");
        out(r32(op->r1));
        out(" << (32 - ");
        out(value);
        out(")); CYCLES(1);");
    } else if (op->func == ror32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> ");
        out(value);
        out(") | (tmp32 << (32 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == ror32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> ");
        out(value);
        out(") | (tmp32 << (32 - ");
        out(value);
        out(")); CYCLES(3);");
    } else if (op->func == rcl32_reg_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" << ");
        out(value);
        out(") | (");
        out(r32(op->r1));
        out(" >> (33 - ");
        out(value);
        out(")) | (getCF(cpu) << (");
        out(value);
        out(" - 1)); CYCLES(8);");
    } else if (op->func == rcl32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << ");
        out(value);
        out(") | (");
        out(r32(op->r1));
        out(" >> (33 - ");
        out(value);
        out(")) | (getCF(cpu) << (");
        out(value);
        out(" - 1))); CYCLES(10);");
    } else if (op->func == rcl32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << ");
        out(value);
        out(") | (");
        out(r32(op->r1));
        out(" >> (33 - ");
        out(value);
        out(")) | (getCF(cpu) << (");
        out(value);
        out(" - 1))); CYCLES(10);");
    } else if (op->func == rcr32_reg_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" >> ");
        out(value);
        out(") | (");
        out(r32(op->r1));
        out(" << (33 - ");
        out(value);
        out(")) | (getCF(cpu) << (32 - ");
        out(value);
        out(")); CYCLES(8);");
    } else if (op->func == rcr32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> ");
        out(value);
        out(") | (");
        out(r32(op->r1));
        out(" << (33 - ");
        out(value);
        out(")) | (getCF(cpu) << (32 - ");
        out(value);
        out("))); CYCLES(10);");        
    } else if (op->func == rcr32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> ");
        out(value);
        out(") | (");
        out(r32(op->r1));
        out(" << (33 - ");
        out(value);
        out(")) | (getCF(cpu) << (32 - ");
        out(value);
        out("))); CYCLES(10);"); 
    } else if (op->func == shl32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" << ");
        out(value);
        out("; CYCLES(1);");
    } else if (op->func == shl32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) << ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == shl32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) << ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == shr32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" >> ");
        out(value);
        out("; CYCLES(1);");
    } else if (op->func == shr32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == shr32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == sar32_reg_noflags) {
        out(r32(op->r1));
        out(" = (S32)");
        out(r32(op->r1));
        out(" >> ");
        out(value);
        out("; CYCLES(1);");
    } else if (op->func == sar32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writed(cpu->memory, eaa, (S32)readd(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else if (op->func == sar32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writed(cpu->memory, eaa, (S32)readd(cpu->memory, eaa) >> ");
        out(value);
        out("); CYCLES(3);"); 
    } else {
        kpanic("gen2c1");
    }
}

void OPCALL rol8cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rol8cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rol8cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL ror8cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL ror8cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL ror8cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl8cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl8cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl8cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr8cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr8cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr8cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shl8cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shl8cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shl8cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shr8cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shr8cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shr8cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL sar8cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL sar8cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL sar8cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rol8cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol8cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol8cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror8cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror8cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror8cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl8cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl8cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl8cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr8cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr8cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr8cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl8cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl8cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl8cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr8cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr8cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr8cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar8cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar8cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar8cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void gen0d2(struct Op* op) {
    char reg[16];
    itoa(op->r1, reg, 10);

    if (op->func == rol8cl_reg_op) {
        out("rol8cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == rol8cl_mem16_op) {
        out("rol8cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == rol8cl_mem32_op) {
        out("rol8cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == ror8cl_reg_op) {
        out("ror8cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == ror8cl_mem16_op) {
        out("rol8cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == ror8cl_mem32_op) {
        out("rol8cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcl8cl_reg_op) {
        out("rcl8cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == rcl8cl_mem16_op) {
        out("rcl8cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcl8cl_mem32_op) {
        out("rcl8cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcr8cl_reg_op) {
        out("rcr8cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == rcr8cl_mem16_op) {
        out("rcl8cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcr8cl_mem32_op) {
        out("rcl8cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == shl8cl_reg_op) {
        out("shl8cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == shl8cl_mem16_op) {
        out("shl8cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == shl8cl_mem32_op) {
        out("shl8cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == shr8cl_reg_op) {
        out("shr8cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == shr8cl_mem16_op) {
        out("shr8cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == shr8cl_mem32_op) {
        out("shr8cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == sar8cl_reg_op) {
        out("sar8cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == sar8cl_mem16_op) {
        out("sar8cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == sar8cl_mem32_op) {
        out("sar8cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    }

    else if (op->func == rol8cl_reg_noflags) {
        out(r8(op->r1));
        out(" = (");
        out(r8(op->r1));
        out(" << (CL & 0x1F) | (");
        out(r8(op->r1));
        out(" >> (8 - (CL & 7))); CYCLES(1);");
    } else if (op->func == rol8cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << (CL & 7)) | (tmp8 >> (8 - (CL & 7))); CYCLES(3);");
    } else if (op->func == rol8cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << (CL & 7)) | (tmp8 >> (8 - (CL & 7))); CYCLES(3);");
    } else if (op->func == ror8cl_reg_noflags) {
        out(r8(op->r1));
        out(" = (");
        out(r8(op->r1));
        out(" >> (CL & 7)) | (");
        out(r8(op->r1));
        out(" << (8 - (CL & 7))); CYCLES(1);");
    } else if (op->func == ror8cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> (CL & 7)) | (tmp8 << (8 - (CL & 7))); CYCLES(3);");
    } else if (op->func == ror8cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> (CL & 7)) | (tmp8 << (8 - (CL & 7))); CYCLES(3);");
    } else if (op->func == rcl8cl_reg_noflags) {
        out(r8(op->r1));
        out(" = (");
        out(r8(op->r1));
        out(" << (CL & 7)) | (");
        out(r8(op->r1));
        out(" >> (9 - (CL & 7))) | (getCF(cpu) << ((CL & 7) - 1)); CYCLES(8);");
    } else if (op->func == rcl8cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << (CL & 7)) | (");
        out(r8(op->r1));
        out(" >> (9 - (CL & 7))) | (getCF(cpu) << ((CL & 7) - 1))); CYCLES(10);");
    } else if (op->func == rcl8cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << (CL & 7)) | (");
        out(r8(op->r1));
        out(" >> (9 - (CL & 7))) | (getCF(cpu) << ((CL & 7) - 1))); CYCLES(10);");
    } else if (op->func == rcr8cl_reg_noflags) {
        out(r8(op->r1));
        out(" = (");
        out(r8(op->r1));
        out(" >> (CL & 7)) | (");
        out(r8(op->r1));
        out(" << (9 - (CL & 7))) | (getCF(cpu) << (8 - (CL & 7))); CYCLES(8);");
    } else if (op->func == rcr8cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> (CL & 7)) | (");
        out(r8(op->r1));
        out(" << (9 - (CL & 7))) | (getCF(cpu) << (8 - (CL & 7)))); CYCLES(10);");        
    } else if (op->func == rcr8cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> (CL & 7)) | (");
        out(r8(op->r1));
        out(" << (9 - (CL & 7))) | (getCF(cpu) << (8 - (CL & 7)))); CYCLES(10);"); 
    } else if (op->func == shl8cl_reg_noflags) {
        out(r8(op->r1));
        out(" = ");
        out(r8(op->r1));
        out(" << (CL & 7); CYCLES(1);");
    } else if (op->func == shl8cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) << (CL & 7)); CYCLES(3);"); 
    } else if (op->func == shl8cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) << (CL & 7)); CYCLES(3);"); 
    } else if (op->func == shr8cl_reg_noflags) {
        out(r8(op->r1));
        out(" = ");
        out(r8(op->r1));
        out(" >> (CL & 7); CYCLES(1);");
    } else if (op->func == shr8cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) >> (CL & 7)); CYCLES(3);"); 
    } else if (op->func == shr8cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) >> (CL & 7)); CYCLES(3);"); 
    } else if (op->func == sar8cl_reg_noflags) {
        out(r8(op->r1));
        out(" = (S8)");
        out(r8(op->r1));
        out(" >> (CL & 7); CYCLES(1);");
    } else if (op->func == sar8cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writeb(cpu->memory, eaa, (S8)readb(cpu->memory, eaa) >> (CL & 7)); CYCLES(3);"); 
    } else if (op->func == sar8cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writeb(cpu->memory, eaa, (S8)readb(cpu->memory, eaa) >> (CL & 7)); CYCLES(3);"); 
    } else {
        kpanic("gen0d2");
    }
}

void OPCALL rol16cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rol16cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rol16cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL ror16cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL ror16cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL ror16cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl16cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl16cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl16cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr16cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr16cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr16cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shl16cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shl16cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shl16cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shr16cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shr16cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shr16cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL sar16cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL sar16cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL sar16cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rol16cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol16cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol16cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror16cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror16cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror16cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl16cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl16cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl16cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr16cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr16cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr16cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl16cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl16cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl16cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr16cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr16cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr16cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar16cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar16cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar16cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void gen0d3(struct Op* op) {
    char reg[16];
    itoa(op->r1, reg, 10);

    if (op->func == rol16cl_reg_op) {
        out("rol16cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == rol16cl_mem16_op) {
        out("rol16cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == rol16cl_mem32_op) {
        out("rol16cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == ror16cl_reg_op) {
        out("ror16cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == ror16cl_mem16_op) {
        out("rol16cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == ror16cl_mem32_op) {
        out("rol16cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcl16cl_reg_op) {
        out("rcl16cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == rcl16cl_mem16_op) {
        out("rcl16cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcl16cl_mem32_op) {
        out("rcl16cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcr16cl_reg_op) {
        out("rcr16cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == rcr16cl_mem16_op) {
        out("rcl16cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcr16cl_mem32_op) {
        out("rcl16cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == shl16cl_reg_op) {
        out("shl16cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == shl16cl_mem16_op) {
        out("shl16cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == shl16cl_mem32_op) {
        out("shl16cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == shr16cl_reg_op) {
        out("shr16cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == shr16cl_mem16_op) {
        out("shr16cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == shr16cl_mem32_op) {
        out("shr16cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == sar16cl_reg_op) {
        out("sar16cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == sar16cl_mem16_op) {
        out("sar16cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == sar16cl_mem32_op) {
        out("sar16cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    }

    else if (op->func == rol16cl_reg_noflags) {
        out(r16(op->r1));
        out(" = (");
        out(r16(op->r1));
        out(" << (CL & 0x1F) | (");
        out(r16(op->r1));
        out(" >> (16 - (CL & 15))); CYCLES(1);");
    } else if (op->func == rol16cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << (CL & 15)) | (tmp16 >> (16 - (CL & 15))); CYCLES(3);");
    } else if (op->func == rol16cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << (CL & 15)) | (tmp16 >> (16 - (CL & 15))); CYCLES(3);");
    } else if (op->func == ror16cl_reg_noflags) {
        out(r16(op->r1));
        out(" = (");
        out(r16(op->r1));
        out(" >> (CL & 15)) | (");
        out(r16(op->r1));
        out(" << (16 - (CL & 15))); CYCLES(1);");
    } else if (op->func == ror16cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> (CL & 15)) | (tmp16 << (16 - (CL & 15))); CYCLES(3);");
    } else if (op->func == ror16cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> (CL & 15)) | (tmp16 << (16 - (CL & 15))); CYCLES(3);");
    } else if (op->func == rcl16cl_reg_noflags) {
        out(r16(op->r1));
        out(" = (");
        out(r16(op->r1));
        out(" << (CL & 15)) | (");
        out(r16(op->r1));
        out(" >> (17 - (CL & 15))) | (getCF(cpu) << ((CL & 15) - 1)); CYCLES(8);");
    } else if (op->func == rcl16cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << (CL & 15)) | (");
        out(r16(op->r1));
        out(" >> (17 - (CL & 15))) | (getCF(cpu) << ((CL & 15) - 1))); CYCLES(10);");
    } else if (op->func == rcl16cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << (CL & 15)) | (");
        out(r16(op->r1));
        out(" >> (17 - (CL & 15))) | (getCF(cpu) << ((CL & 15) - 1))); CYCLES(10);");
    } else if (op->func == rcr16cl_reg_noflags) {
        out(r16(op->r1));
        out(" = (");
        out(r16(op->r1));
        out(" >> (CL & 15)) | (");
        out(r16(op->r1));
        out(" << (17 - (CL & 15))) | (getCF(cpu) << (16 - (CL & 15))); CYCLES(8);");
    } else if (op->func == rcr16cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> (CL & 15)) | (");
        out(r16(op->r1));
        out(" << (17 - (CL & 15))) | (getCF(cpu) << (16 - (CL & 15)))); CYCLES(10);");        
    } else if (op->func == rcr16cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> (CL & 15)) | (");
        out(r16(op->r1));
        out(" << (17 - (CL & 15))) | (getCF(cpu) << (16 - (CL & 15)))); CYCLES(10);"); 
    } else if (op->func == shl16cl_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" << (CL & 15); CYCLES(1);");
    } else if (op->func == shl16cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) << (CL & 15)); CYCLES(3);"); 
    } else if (op->func == shl16cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) << (CL & 15)); CYCLES(3);"); 
    } else if (op->func == shr16cl_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" >> (CL & 15); CYCLES(1);");
    } else if (op->func == shr16cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) >> (CL & 15)); CYCLES(3);"); 
    } else if (op->func == shr16cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) >> (CL & 15)); CYCLES(3);"); 
    } else if (op->func == sar16cl_reg_noflags) {
        out(r16(op->r1));
        out(" = (S16)");
        out(r16(op->r1));
        out(" >> (CL & 15); CYCLES(1);");
    } else if (op->func == sar16cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writew(cpu->memory, eaa, (S16)readw(cpu->memory, eaa) >> (CL & 15)); CYCLES(3);"); 
    } else if (op->func == sar16cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writew(cpu->memory, eaa, (S16)readw(cpu->memory, eaa) >> (CL & 15)); CYCLES(3);"); 
    } else {
        kpanic("gen0d3");
    }
}

void OPCALL rol32cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rol32cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rol32cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL ror32cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL ror32cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL ror32cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl32cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl32cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcl32cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr32cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr32cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL rcr32cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shl32cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shl32cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shl32cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL shr32cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL shr32cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL shr32cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL sar32cl_reg_op(struct CPU* cpu, struct Op* op);
void OPCALL sar32cl_mem16_op(struct CPU* cpu, struct Op* op);
void OPCALL sar32cl_mem32_op(struct CPU* cpu, struct Op* op);
void OPCALL rol32cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol32cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rol32cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror32cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror32cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ror32cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl32cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl32cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcl32cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr32cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr32cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL rcr32cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl32cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl32cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shl32cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr32cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr32cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL shr32cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar32cl_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar32cl_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sar32cl_mem32_noflags(struct CPU* cpu, struct Op* op);
void gen2d3(struct Op* op) {
    char reg[16];
    itoa(op->r1, reg, 10);

    if (op->func == rol32cl_reg_op) {
        out("rol32cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == rol32cl_mem16_op) {
        out("rol32cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == rol32cl_mem32_op) {
        out("rol32cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == ror32cl_reg_op) {
        out("ror32cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == ror32cl_mem16_op) {
        out("rol32cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == ror32cl_mem32_op) {
        out("rol32cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcl32cl_reg_op) {
        out("rcl32cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == rcl32cl_mem16_op) {
        out("rcl32cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcl32cl_mem32_op) {
        out("rcl32cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcr32cl_reg_op) {
        out("rcr32cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == rcr32cl_mem16_op) {
        out("rcl32cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == rcr32cl_mem32_op) {
        out("rcl32cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == shl32cl_reg_op) {
        out("shl32cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == shl32cl_mem16_op) {
        out("shl32cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == shl32cl_mem32_op) {
        out("shl32cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == shr32cl_reg_op) {
        out("shr32cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == shr32cl_mem16_op) {
        out("shr32cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == shr32cl_mem32_op) {
        out("shr32cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    } else if (op->func == sar32cl_reg_op) {
        out("sar32cl_reg(cpu, ");
        out(reg);
        out(", CL & 0x1F);");
    } else if (op->func == sar32cl_mem16_op) {
        out("sar32cl_mem16(cpu, ");
        out(getEaa16(op));
        out(", CL & 0x1F);");
    } else if (op->func == sar32cl_mem32_op) {
        out("sar32cl_mem32(cpu, ");
        out(getEaa32(op));
        out(", CL & 0x1F);");
    }

    else if (op->func == rol32cl_reg_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" << (CL & 0x1F) | (");
        out(r32(op->r1));
        out(" >> (32 - (CL & 31))); CYCLES(1);");
    } else if (op->func == rol32cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << (CL & 31)) | (tmp32 >> (32 - (CL & 31))); CYCLES(3);");
    } else if (op->func == rol32cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << (CL & 31)) | (tmp32 >> (32 - (CL & 31))); CYCLES(3);");
    } else if (op->func == ror32cl_reg_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" >> (CL & 31)) | (");
        out(r32(op->r1));
        out(" << (32 - (CL & 31))); CYCLES(1);");
    } else if (op->func == ror32cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> (CL & 31)) | (tmp32 << (32 - (CL & 31))); CYCLES(3);");
    } else if (op->func == ror32cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> (CL & 31)) | (tmp32 << (32 - (CL & 31))); CYCLES(3);");
    } else if (op->func == rcl32cl_reg_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" << (CL & 31)) | (");
        out(r32(op->r1));
        out(" >> (33 - (CL & 31))) | (getCF(cpu) << ((CL & 31) - 1)); CYCLES(8);");
    } else if (op->func == rcl32cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << (CL & 31)) | (");
        out(r32(op->r1));
        out(" >> (33 - (CL & 31))) | (getCF(cpu) << ((CL & 31) - 1))); CYCLES(10);");
    } else if (op->func == rcl32cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << (CL & 31)) | (");
        out(r32(op->r1));
        out(" >> (33 - (CL & 31))) | (getCF(cpu) << ((CL & 31) - 1))); CYCLES(10);");
    } else if (op->func == rcr32cl_reg_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" >> (CL & 31)) | (");
        out(r32(op->r1));
        out(" << (33 - (CL & 31))) | (getCF(cpu) << (32 - (CL & 31))); CYCLES(8);");
    } else if (op->func == rcr32cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> (CL & 31)) | (");
        out(r32(op->r1));
        out(" << (33 - (CL & 31))) | (getCF(cpu) << (32 - (CL & 31)))); CYCLES(10);");        
    } else if (op->func == rcr32cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> (CL & 31)) | (");
        out(r32(op->r1));
        out(" << (33 - (CL & 31))) | (getCF(cpu) << (32 - (CL & 31)))); CYCLES(10);"); 
    } else if (op->func == shl32cl_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" << (CL & 31); CYCLES(1);");
    } else if (op->func == shl32cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) << (CL & 31)); CYCLES(3);"); 
    } else if (op->func == shl32cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) << (CL & 31)); CYCLES(3);"); 
    } else if (op->func == shr32cl_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" >> (CL & 31); CYCLES(1);");
    } else if (op->func == shr32cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) >> (CL & 31)); CYCLES(3);"); 
    } else if (op->func == shr32cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) >> (CL & 31)); CYCLES(3);"); 
    } else if (op->func == sar32cl_reg_noflags) {
        out(r32(op->r1));
        out(" = (S32)");
        out(r32(op->r1));
        out(" >> (CL & 31); CYCLES(1);");
    } else if (op->func == sar32cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writed(cpu->memory, eaa, (S32)readd(cpu->memory, eaa) >> (CL & 31)); CYCLES(3);"); 
    } else if (op->func == sar32cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writed(cpu->memory, eaa, (S32)readd(cpu->memory, eaa) >> (CL & 31)); CYCLES(3);"); 
    } else {
        kpanic("gen2d3");
    }
}

void gen0c2(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 16);

    out("cpu->eip.u32 = pop16(cpu); SP = SP + 0x");
    out(tmp);
    out("; CYCLES(3); cpu->nextBlock = getBlock(cpu);");
}

void gen2c2(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 16);

    out("cpu->eip.u32 = pop32(cpu); ESP = ESP + 0x");
    out(tmp);
    out("; CYCLES(3); cpu->nextBlock = getBlock(cpu);");
}

void gen0c3(struct Op* op) {
    out("cpu->eip.u32 = pop16(cpu); CYCLES(2); cpu->nextBlock = getBlock(cpu);");
}

void gen2c3(struct Op* op) {
    out("cpu->eip.u32 = pop32(cpu); CYCLES(2); cpu->nextBlock = getBlock(cpu);");
}

void OPCALL mov8_reg(struct CPU* cpu, struct Op* op);
void OPCALL mov8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL mov8_mem32(struct CPU* cpu, struct Op* op);
void gen0c6(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 16);

    if (op->func == mov8_reg) {
        out(r8(op->r1));
        out(" = 0x");
        out(tmp);
        out("; CYCLES(1);");
    } else {
        out("writeb(cpu->memory, ");
        if (op->func == mov8_mem16) {
            out(getEaa16(op));
        } else if (op->func == mov8_mem32) {
            out(getEaa32(op));
        } else {
            kpanic("gen0c6");
        }
        out(", 0x");
        out(tmp);
        out("); CYCLES(1);");
    }
}

void OPCALL mov16_reg(struct CPU* cpu, struct Op* op);
void OPCALL mov16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL mov16_mem32(struct CPU* cpu, struct Op* op);
void gen0c7(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 16);

    if (op->func == mov16_reg) {
        out(r16(op->r1));
        out(" = 0x");
        out(tmp);
        out("; CYCLES(1);");
    } else {
        out("writew(cpu->memory, ");
        if (op->func == mov16_mem16) {
            out(getEaa16(op));
        } else if (op->func == mov16_mem32) {
            out(getEaa32(op));
        } else {
            kpanic("gen0c7");
        }
        out(", 0x");
        out(tmp);
        out("); CYCLES(1);");
    }
}

void OPCALL mov32_reg(struct CPU* cpu, struct Op* op);
void OPCALL mov32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL mov32_mem32(struct CPU* cpu, struct Op* op);
void gen2c7(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 16);

    if (op->func == mov32_reg) {
        out(r32(op->r1));
        out(" = 0x");
        out(tmp);
        out("; CYCLES(1);");
    } else {
        out("writed(cpu->memory, ");
        if (op->func == mov32_mem16) {
            out(getEaa16(op));
        } else if (op->func == mov32_mem32) {
            out(getEaa32(op));
        } else {
            kpanic("gen2c7");
        }
        out(", 0x");
        out(tmp);
        out("); CYCLES(1);");
    }
}

void gen0c9(struct Op* op) {
    out("SP = BP; BP = pop16(cpu); CYCLES(3);");
}

void gen2c9(struct Op* op) {
    out("ESP = EBP; EBP = pop32(cpu); CYCLES(3);");
}

void gen0cb(struct Op* op) {
    char tmp[16];
    itoa(op->eipCount, tmp, 10);
    out("fillFlags(cpu); cpu->eip.u32+=");
    out(tmp);
    out("; cpu_ret(cpu, 0, cpu->eip.u32); CYCLES(4); cpu->nextBlock = getBlock(cpu);");
}

void OPCALL syscall_op(struct CPU* cpu, struct Op* op);
void OPCALL int99(struct CPU* cpu, struct Op* op);
void gen0cd(struct Op* op) {
    char tmp[16];

    itoa(op->eipCount, tmp, 10);
    if (op->func == syscall_op) {        
        out("syscall(cpu, ");
        out(tmp);
        out(");");
        // syscall will set nextBlock
    } else if (op->func == int99) {
	    out("int99Callback[peek32(cpu, 0)](cpu); cpu->eip.u32+=");
        out(tmp);
        out("; cpu->nextBlock = getBlock(cpu);");
    } else {
        kpanic("gen0cd");
    }
}

void gen0d4(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    out("instruction_aam(cpu, ");
    out(tmp);
    out("); CYCLES(18);");
}

void gen0d5(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    out("instruction_aad(cpu, ");
    out(tmp);
    out("); CYCLES(10);");
}

void gen0d6(struct Op* op) {
    out("if (getCF(cpu)) AL = 0xFF; else AL = 0; CYCLES(2);");
}

void OPCALL xlat16(struct CPU* cpu, struct Op* op);
void OPCALL xlat32(struct CPU* cpu, struct Op* op);
void gen0d7(struct Op* op) {
    if (op->func == xlat16) {
        out("AL = readb(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("] + (U16)(BX + AL)); CYCLES(4);");
    } else if (op->func == xlat32) {
        out("AL = readb(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("] + EBX + AL); CYCLES(4);");
    } else {
        kpanic("gen0d7");
    }
}
typedef void (*SRC_GEN)(struct Op* op);

SRC_GEN srcgen[] = {
    // 000
    gen000, gen001, gen002, gen003, gen004, gen005, gen006, gen007,
	gen008, gen009, gen00a, gen00b, gen00c, gen00d, gen00e, 0,
	gen010, gen011, gen012, gen013, gen014, gen015, gen016, gen017,
	gen018, gen019, gen01a, gen01b, gen01c, gen01d, gen01e, gen01f,
	gen020, gen021, gen022, gen023, gen024, gen025, 0, gen027,
	gen028, gen029, gen02a, gen02b, gen02c, gen02d, 0, gen02f,
	gen030, gen031, gen032, gen033, gen034, gen035, 0, gen037,
	gen038, gen039, gen03a, gen03b, gen03c, gen03d, 0, gen03f,
	gen040, gen040, gen040, gen040, gen040, gen040, gen040, gen040,
	gen048, gen048, gen048, gen048, gen048, gen048, gen048, gen048,
	gen050, gen050, gen050, gen050, gen050, gen050, gen050, gen050,
	gen058, gen058, gen058, gen058, gen058, gen058, gen058, gen058,
	0, 0, 0, 0, 0, 0, 0, 0,
	gen068, gen069, gen068, gen069, 0, 0, 0, 0,
	gen070, gen071, gen072, gen073, gen074, gen075, gen076, gen077,
	gen078, gen079, gen07a, gen07b, gen07c, gen07d, gen07e, gen07f,
    // 080
    gen080, gen081, gen080, gen081, gen084, gen085, gen086, gen087,
	gen088, gen089, gen08a, gen08b, gen08c, gen08d, gen08e, gen08f,
	gen090, gen091, gen091, gen091, gen091, gen091, gen091, gen091,
	gen098, gen099, gen09a, gen090, gen09c, gen09d, gen09e, gen09f,
	gen0a0, gen0a1, gen0a2, gen0a3, gen0a4, gen0a5, gen0a6, gen0a7,
	gen0a8, gen0a9, gen0aa, gen0ab, gen0ac, gen0ad, gen0ae, gen0af,
	gen0b0, gen0b0, gen0b0, gen0b0, gen0b0, gen0b0, gen0b0, gen0b0,
	gen0b8, gen0b8, gen0b8, gen0b8, gen0b8, gen0b8, gen0b8, gen0b8,
	gen0c0, gen0c1, gen0c2, gen0c3, 0, 0, gen0c6, gen0c7,
	0, gen0c9, 0, gen0cb, 0, gen0cd, 0, 0,
	gen0c0, gen0c1, gen0d2, gen0d3, gen0d4, gen0d5, gen0d6, gen0d7,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    // 100
    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    // 180
    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	gen1a0, gen1a1, 0, 0, 0, 0, 0, 0,
	gen1a8, gen1a9, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    // 200
    gen000, gen201, gen002, gen203, gen004, gen205, gen206, gen207,
	gen008, gen209, gen00a, gen20b, gen00c, gen20d, gen20e, 0,
	gen010, gen211, gen012, gen213, gen014, gen215, gen216, gen217,
	gen018, gen219, gen01a, gen21b, gen01c, gen21d, gen21e, gen21f,
	gen020, gen221, gen022, gen223, gen024, gen225, 0, gen027,
	gen028, gen229, gen02a, gen22b, gen02c, gen22d, 0, gen02f,
	gen030, gen231, gen032, gen233, gen034, gen235, 0, gen037,
	gen038, gen239, gen03a, gen23b, gen03c, gen23d, 0, gen03f,
	gen240, gen240, gen240, gen240, gen240, gen240, gen240, gen240,
	gen248, gen248, gen248, gen248, gen248, gen248, gen248, gen248,
	gen250, gen250, gen250, gen250, gen250, gen250, gen250, gen250,
	gen258, gen258, gen258, gen258, gen258, gen258, gen258, gen258,
	0, 0, 0, 0, 0, 0, 0, 0,
	gen268, gen269, gen268, gen269, 0, 0, 0, 0,
	gen070, gen071, gen072, gen073, gen074, gen075, gen076, gen077,
	gen078, gen079, gen07a, gen07b, gen07c, gen07d, gen07e, gen07f,
    // 280
    gen080, gen281, gen080, gen281, gen084, gen285, gen086, gen287,
	gen088, gen289, gen08a, gen28b, gen08c, gen28d, gen08e, gen28f,
	gen090, gen291, gen291, gen291, gen291, gen291, gen291, gen291,
	gen298, gen299, 0, gen090, gen29c, gen29d, gen09e, gen09f,
	gen0a0, gen2a1, gen0a2, gen2a3, gen0a4, gen2a5, gen0a6, gen2a7,
	gen0a8, gen2a9, gen0aa, gen2ab, gen0ac, gen2ad, gen0ae, gen2af,
	gen0b0, gen0b0, gen0b0, gen0b0, gen0b0, gen0b0, gen0b0, gen0b0,
	gen2b8, gen2b8, gen2b8, gen2b8, gen2b8, gen2b8, gen2b8, gen2b8,
	gen0c0, gen2c1, gen2c2, gen2c3, 0, 0, gen0c6, gen2c7,
	0, gen2c9, 0, 0, 0, 0, 0, 0,
	gen0c0, gen2c1, gen0d2, gen2d3, gen0d4, gen0d5, gen0d6, gen0d7,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    // 300
    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    // 380
    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	gen3a0, gen3a1, 0, 0, 0, 0, 0, 0,
	gen3a8, gen3a9, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    0,
};

static int generatedBlocks;

#include "kprocess.h"
#include "crc.h"

#define STR(x) #x
#define OUT_DEFINE(v) out("#define ");out(#v);out(" ");out(STR(v));out("\n")

struct CompiledBlocks {
    U32 crc;
    char func[16];
    char* bytes;
    U32 byteLen;
};

struct CompiledBlocks compiledBlocks[32*1024];
U32 compiledBlockCount;

void addBlock(char* func, U32 crc, char* bytes, U32 byteLen) {
    int i = compiledBlockCount;  
  
    while (i>0 && crc<compiledBlocks[i-1].crc) {
        compiledBlocks[i] = compiledBlocks[i-1];
        i--;
    }
    compiledBlocks[i].crc = crc;
    compiledBlocks[i].bytes = bytes;
    compiledBlocks[i].byteLen = byteLen;
    strcpy(compiledBlocks[i].func, func);
    compiledBlockCount++;
}

BOOL doesBlockExist(U32 crc, char* bytes, U32 byteLen) {
    U32 i;

    for (i=0;i<compiledBlockCount;i++) {
        if (compiledBlocks[i].crc==crc) {
            while (compiledBlocks[i].crc==crc) {
                if (compiledBlocks[i].byteLen == byteLen && !memcmp(bytes, compiledBlocks[i].bytes, byteLen)) {
                    return 1;
                }
                i++;
            }
            break;
        }
        if (compiledBlocks[i].crc>crc)
            break;
    }
    return 0;
}

void writeSource() {
    FILE* fp = fopen("gen.c", "w");  
    U32 i,j;
    char tmp[1024];

    fwrite(sourceBuffer, sourceBufferPos, 1, fp);

    for (i=0;i<compiledBlockCount;i++) {
        outfp(fp, "const unsigned char data");
        itoa(i, tmp, 10);
        outfp(fp, tmp);
        outfp(fp, "[] = {");
        for (j=0;j<compiledBlocks[i].byteLen;j++) {
            if (j>0)
                outfp(fp, ", ");
            sprintf(tmp, "0x%.02X", ((U32)compiledBlocks[i].bytes[j]) & 0xFF);
            outfp(fp, tmp);
        }
        outfp(fp, "};\n");
    }


    outfp(fp, "typedef void (OPCALL *OpCallback)(struct CPU* cpu, struct Op* op);");
    outfp(fp, "struct CompiledCode {U32 crc; OpCallback func; const unsigned char* bytes; unsigned int byteLen;};\n");
    outfp(fp, "static struct CompiledCode compiledCode[] = {\n");
        
    for (i=0;i<compiledBlockCount;i++) {
        outfp(fp, "{0x");
        itoa(compiledBlocks[i].crc, tmp, 16);
        outfp(fp, tmp);
        outfp(fp, ", ");
        outfp(fp, compiledBlocks[i].func);
        outfp(fp, ", data");
        itoa(i, tmp, 10);
        outfp(fp, tmp);
        outfp(fp, ", ");
        itoa(compiledBlocks[i].byteLen, tmp, 10);
        outfp(fp, tmp);
        outfp(fp, "},\n");
    }
    outfp(fp, "};\n");        

    outfp(fp, "// :TODO: compiledCode is sorted, use a binary search\n");
    outfp(fp, "OpCallback getCompiledFunction(U32 crc, const char* bytes, U32 byteLen) {\n");
    outfp(fp, "    int i;\n");
    outfp(fp, "    int count = sizeof(compiledCode) / sizeof(struct CompiledCode);\n");
    outfp(fp, "    for (i=0;i<count;i++) {\n");
    outfp(fp, "        if (compiledCode[i].crc==crc) {\n");
    outfp(fp, "            while (compiledCode[i].crc==crc) {\n");
    outfp(fp, "                if (compiledCode[i].byteLen == byteLen && !memcmp(bytes, compiledCode[i].bytes, byteLen)) {\n");
    outfp(fp, "                    return compiledCode[i].func;\n");
    outfp(fp, "                }\n");
    outfp(fp, "                i++;\n");
    outfp(fp, "            }\n");
    outfp(fp, "            break;\n");
    outfp(fp, "        }\n");
    outfp(fp, "    }\n");
    outfp(fp, "    return 0;\n");
    outfp(fp, "}\n");

    fflush(fp);
    fclose(fp);
}

void generateSource(struct CPU* cpu, U32 eip, struct Block* block) {
    struct Op* op = block->ops;
    char name[256];
    char tmp[1024];
    unsigned char ops[1024];
    int opPos = 0;
    U32 ip = eip;
    int i;
    U32 crc;

    // if the block has only one op then don't compile it
    if (!block->ops->next) {
        return;
    }
    while (op) {
        if (!srcgen[op->inst]) {
            klog("missing instruction for recompiler: %X", op->inst);
            return;
        }
        for (i=0;i<op->eipCount;i++)
            ops[opPos++] = readb(cpu->memory, ip++);
        op = op->next;
    }
    crc = crc32b(ops, opPos);

    if (doesBlockExist(crc, ops, opPos))
        return;

    op = block->ops;
    if (!sourceBuffer) {
        sourceBufferLen = 1024*1024*10;
        sourceBuffer = kalloc(sourceBufferLen)       ;
        sourceBufferPos = 0;
        OUT_DEFINE(U8);
        OUT_DEFINE(U16);
        OUT_DEFINE(U32);
        OUT_DEFINE(S32);
        OUT_DEFINE(U64);
        OUT_DEFINE(S64);
        OUT_DEFINE(BOOL);
        OUT_DEFINE(OPCALL);
        OUT_DEFINE(ES);
        OUT_DEFINE(CS);
        OUT_DEFINE(SS);
        OUT_DEFINE(DS);
        OUT_DEFINE(FS);
        OUT_DEFINE(GS);
        OUT_DEFINE(SEG_ZERO);
        OUT_DEFINE(AL);
        OUT_DEFINE(AH);
        OUT_DEFINE(CL);
        OUT_DEFINE(CH);
        OUT_DEFINE(DL);
        OUT_DEFINE(DH);
        OUT_DEFINE(BL);
        OUT_DEFINE(BH);
        OUT_DEFINE(AX);
        OUT_DEFINE(CX);
        OUT_DEFINE(DX);
        OUT_DEFINE(BX);
        OUT_DEFINE(SP);
        OUT_DEFINE(BP);
        OUT_DEFINE(SI);
        OUT_DEFINE(DI);
        OUT_DEFINE(EAX);
        OUT_DEFINE(ECX);
        OUT_DEFINE(EDX);
        OUT_DEFINE(EBX);
        OUT_DEFINE(ESP);
        OUT_DEFINE(EBP);
        OUT_DEFINE(ESI);
        OUT_DEFINE(EDI);
        
        out("#define CYCLES(x) cpu->blockCounter += x\n");

        out("extern struct LazyFlags* FLAGS_NONE;\n");
        out("extern struct LazyFlags* FLAGS_ADD8;\n");
        out("extern struct LazyFlags* FLAGS_ADD16;\n");
        out("extern struct LazyFlags* FLAGS_ADD32;\n");
        out("extern struct LazyFlags* FLAGS_OR8;\n");
        out("extern struct LazyFlags* FLAGS_OR16;\n");
        out("extern struct LazyFlags* FLAGS_OR32;\n");
        out("extern struct LazyFlags* FLAGS_ADC8;\n");
        out("extern struct LazyFlags* FLAGS_ADC16;\n");
        out("extern struct LazyFlags* FLAGS_ADC32;\n");
        out("extern struct LazyFlags* FLAGS_SBB8;\n");
        out("extern struct LazyFlags* FLAGS_SBB16;\n");
        out("extern struct LazyFlags* FLAGS_SBB32;\n");
        out("extern struct LazyFlags* FLAGS_AND8;\n");
        out("extern struct LazyFlags* FLAGS_AND16;\n");
        out("extern struct LazyFlags* FLAGS_AND32;\n");
        out("extern struct LazyFlags* FLAGS_SUB8;\n");
        out("extern struct LazyFlags* FLAGS_SUB16;\n");
        out("extern struct LazyFlags* FLAGS_SUB32;\n");
        out("extern struct LazyFlags* FLAGS_XOR8;\n");
        out("extern struct LazyFlags* FLAGS_XOR16;\n");
        out("extern struct LazyFlags* FLAGS_XOR32;\n");
        out("extern struct LazyFlags* FLAGS_INC8;\n");
        out("extern struct LazyFlags* FLAGS_INC16;\n");
        out("extern struct LazyFlags* FLAGS_INC32;\n");
        out("extern struct LazyFlags* FLAGS_DEC8;\n");
        out("extern struct LazyFlags* FLAGS_DEC16;\n");
        out("extern struct LazyFlags* FLAGS_DEC32;\n");
        out("extern struct LazyFlags* FLAGS_SHL8;\n");
        out("extern struct LazyFlags* FLAGS_SHL16;\n");
        out("extern struct LazyFlags* FLAGS_SHL32;\n");
        out("extern struct LazyFlags* FLAGS_SHR8;\n");
        out("extern struct LazyFlags* FLAGS_SHR16;\n");
        out("extern struct LazyFlags* FLAGS_SHR32;\n");
        out("extern struct LazyFlags* FLAGS_SAR8;\n");
        out("extern struct LazyFlags* FLAGS_SAR16;\n");
        out("extern struct LazyFlags* FLAGS_SAR32;\n");
        out("extern struct LazyFlags* FLAGS_CMP8;\n");
        out("extern struct LazyFlags* FLAGS_CMP16;\n");
        out("extern struct LazyFlags* FLAGS_CMP32;\n");
        out("extern struct LazyFlags* FLAGS_TEST8;\n");
        out("extern struct LazyFlags* FLAGS_TEST16;\n");
        out("extern struct LazyFlags* FLAGS_TEST32;\n");
        out("extern struct LazyFlags* FLAGS_DSHL16;\n");
        out("extern struct LazyFlags* FLAGS_DSHL32;\n");
        out("extern struct LazyFlags* FLAGS_DSHR16;\n");
        out("extern struct LazyFlags* FLAGS_DSHR32;\n");
        out("extern struct LazyFlags* FLAGS_NEG8;\n");
        out("extern struct LazyFlags* FLAGS_NEG16;\n");
        out("extern struct LazyFlags* FLAGS_NEG32;\n");        

        out("#define getCF(x) cpu->lazyFlags->getCF(cpu)\n");
        out("#define getOF(x) cpu->lazyFlags->getOF(cpu)\n");
        out("#define getAF(x) cpu->lazyFlags->getAF(cpu)\n");
        out("#define getZF(x) cpu->lazyFlags->getZF(cpu)\n");
        out("#define getSF(x) cpu->lazyFlags->getSF(cpu)\n");
        out("#define getPF(x) cpu->lazyFlags->getPF(cpu)\n");
        out("#define addFlag(f) cpu->flags |= (f)\n");
        out("#define removeFlag(f) cpu->flags &=~ (f)\n");
        out("#define CF		0x00000001\n");
        out("#define PF		0x00000004\n");
        out("#define AF		0x00000010\n");
        out("#define ZF		0x00000040\n");
        out("#define SF		0x00000080\n");
        out("#define OF		0x00000800\n");

        out("struct Reg {union {U32 u32;union {union {U16 u16;struct {U8 u8;U8 h8;};};U16 h16;};};};\n");
        out("struct FPU_Reg {union {double d;U64 l;};};\n");
        out("struct FPU {struct FPU_Reg regs[9];U32 tags[9];U32 cw;U32 cw_mask_all;U32 sw;U32 top;U32 round;};\n");
        out("struct CPU {struct Reg reg[9];U8* reg8[8]; U32 segAddress[6];U32 segValue[7];U32 flags;struct Reg eip;struct Memory* memory;struct KThread* thread;struct Reg src;struct Reg dst;struct Reg dst2;struct Reg result;struct LazyFlags* lazyFlags;int df;U32 oldcf;U32 big;struct FPU fpu;struct Block* nextBlock;struct Block* currentBlock;U64 timeStampCounter;U32 blockCounter;BOOL log;U32 cpl;};\n");
        out("struct LazyFlags {U32 (*getCF)(struct CPU* cpu);U32 (*getOF)(struct CPU* cpu);U32 (*getAF)(struct CPU* cpu);U32 (*getZF)(struct CPU* cpu);U32 (*getSF)(struct CPU* cpu);U32 (*getPF)(struct CPU* cpu);};\n");
        out("U8 readb(struct Memory* memory, U32 address);\n");
        out("void writeb(struct Memory* memory, U32 address, U8 value);\n");
        out("U16 readw(struct Memory* memory, U32 address);\n");
        out("void writew(struct Memory* memory, U32 address, U16 value);\n");
        out("U32 readd(struct Memory* memory, U32 address);\n");
        out("void writed(struct Memory* memory, U32 address, U32 value);\n");
        out("struct Block* getBlock(struct CPU* cpu);\n");
        out("struct Block* getBlock1(struct CPU* cpu);\n");
        out("struct Block* getBlock2(struct CPU* cpu);\n");
        out("void push16(struct CPU* cpu, U16 value);\n");
        out("void push32(struct CPU* cpu, U32 value);\n");
        out("U16 pop16(struct CPU* cpu);\n");
        out("U32 pop32(struct CPU* cpu);\n");
        out("void fillFlagsNoCFOF(struct CPU* cpu);\n");

        out("void rol8_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rol8_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol8_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol8cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rol8cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol8cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol16_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rol16_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol16_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol16cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rol16cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol16cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol32_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rol32_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol32_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol32cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rol32cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rol32cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror8_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void ror8_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror8_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror8cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void ror8cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror8cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror16_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void ror16_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror16_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror16cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void ror16cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror16cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror32_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void ror32_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror32_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror32cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void ror32cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void ror32cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl8_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcl8_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl8_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl8cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcl8cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl8cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl16_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcl16_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl16_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl16cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcl16cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl16cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl32_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcl32_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl32_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl32cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcl32cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcl32cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr8_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcr8_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr8_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr8cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcr8cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr8cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr16_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcr16_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr16_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr16cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcr16cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr16cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr32_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcr32_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr32_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr32cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void rcr32cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void rcr32cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl8_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shl8_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl8_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl8cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shl8cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl8cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl16_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shl16_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl16_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl16cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shl16cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl16cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl32_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shl32_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl32_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl32cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shl32cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shl32cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr8_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shr8_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr8_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr8cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shr8cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr8cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr16_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shr16_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr16_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr16cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shr16cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr16cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr32_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shr32_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr32_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr32cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void shr32cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void shr32cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar8_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void sar8_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar8_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar8cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void sar8cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar8cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar16_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void sar16_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar16_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar16cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void sar16cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar16cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar32_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void sar32_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar32_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar32cl_reg(struct CPU* cpu, U32 reg, U32 value);\n");
        out("void sar32cl_mem16(struct CPU* cpu, U32 eaa, U32 value);\n");
        out("void sar32cl_mem32(struct CPU* cpu, U32 eaa, U32 value);\n");

        out("void movsb32_r(struct CPU* cpu, U32 base);\n");
        out("void movsb16_r(struct CPU* cpu, U32 base);\n");
        out("void movsb32(struct CPU* cpu, U32 base);\n");
        out("void movsb16(struct CPU* cpu, U32 base);\n");
        out("void movsw32_r(struct CPU* cpu, U32 base);\n");
        out("void movsw16_r(struct CPU* cpu, U32 base);\n");
        out("void movsw32(struct CPU* cpu, U32 base);\n");
        out("void movsw16(struct CPU* cpu, U32 base);\n");
        out("void movsd32_r(struct CPU* cpu, U32 base);\n");
        out("void movsd16_r(struct CPU* cpu, U32 base);\n");
        out("void movsd32(struct CPU* cpu, U32 base);\n");
        out("void movsd16(struct CPU* cpu, U32 base);\n");
        out("void cmpsb32_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsb16_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsb32(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsb16(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsw32_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsw16_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsw32(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsw16(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsd32_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsd16_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsd32(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsd16(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void stosb32_r(struct CPU* cpu);\n");
        out("void stosb16_r(struct CPU* cpu);\n");
        out("void stosb32(struct CPU* cpu);\n");
        out("void stosb16(struct CPU* cpu);\n");
        out("void stosw32_r(struct CPU* cpu);\n");
        out("void stosw16_r(struct CPU* cpu);\n");
        out("void stosw32(struct CPU* cpu);\n");
        out("void stosw16(struct CPU* cpu);\n");
        out("void stosd32_r(struct CPU* cpu);\n");
        out("void stosd16_r(struct CPU* cpu);\n");
        out("void stosd32(struct CPU* cpu);\n");
        out("void stosd16(struct CPU* cpu);\n");
        out("void lodsb32_r(struct CPU* cpu, U32 base);\n");
        out("void lodsb16_r(struct CPU* cpu, U32 base);\n");
        out("void lodsb32(struct CPU* cpu, U32 base);\n");
        out("void lodsb16(struct CPU* cpu, U32 base);\n");
        out("void lodsw32_r(struct CPU* cpu, U32 base);\n");
        out("void lodsw16_r(struct CPU* cpu, U32 base);\n");
        out("void lodsw32(struct CPU* cpu, U32 base);\n");
        out("void lodsw16(struct CPU* cpu, U32 base);\n");
        out("void lodsd32_r(struct CPU* cpu, U32 base);\n");
        out("void lodsd16_r(struct CPU* cpu, U32 base);\n");
        out("void lodsd32(struct CPU* cpu, U32 base);\n");
        out("void lodsd16(struct CPU* cpu, U32 base);\n");
        out("void scasb32_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasb16_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasb32(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasb16(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasw32_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasw16_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasw32(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasw16(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasd32_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasd16_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasd32(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasd16(struct CPU* cpu, U32 rep_zero);\n");


        out("static U32 eaa;\n");
        out("static U8 tmp8;\n");
        out("static U16 tmp16;\n");
        out("static U32 tmp32;\n");
        out("static U64 tmp64;\n\n");
    }
    sprintf(tmp, "// 0x%.8x CRC=%.08X %s at 0x%.8x\n", eip, crc, getModuleName(cpu, eip), getModuleEip(cpu, eip));
    out(tmp);
    sprintf(name, "generated%X", compiledBlockCount);
    out("void OPCALL ");
    out(name);
    out("(struct CPU* cpu, struct Op* op) {\n");    
    while (op) {
        out("    ");
        srcgen[op->inst](op);
        // the last op is responsible for handling the eip adjustment
        if (op->next) {
            out(" cpu->eip.u32+=");
            itoa(op->eipCount, tmp, 10);
            out(tmp);    
            out(";\n");
        } else {
            out("\n");
        }
        op = op->next;
    }
    out("}\n\n");    

    {
        char* bytes = (char*)kalloc(opPos);
        memcpy(bytes, ops, opPos);
        addBlock(name, crc, bytes, opPos);
    }
}
#endif
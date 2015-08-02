#ifdef GENERATE_SOURCE
#include "op.h"
#include "block.h"
#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>

FILE* fpGenSrc;

void out(const char* str) {
    fwrite(str, strlen(str), 1, fpGenSrc);
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
        strcat(eaaStr, "] + ");
    }
    strcat(eaaStr, r32(op->e1));
    if (op->e2<8 && op->eSib) {
        char tmp[32];

        strcat(eaaStr, " + (");
        strcat(eaaStr, r32(op->e2));
        strcat(eaaStr, " << ");
        itoa(op->eSib, tmp, 10);
        strcat(eaaStr, tmp);
        strcat(eaaStr, ")");
    }
    if (op->eData) {
        char tmp[32];
        strcat(eaaStr, " + 0x");
        itoa(op->eData, tmp, 16);
        strcat(eaaStr, tmp);
    }
    return eaaStr;
}

void gen027(struct Op* op) {
    out("instruction_daa(cpu);CYCLES(3);\n");
}

void gen02f(struct Op* op) {
    out("instruction_das(cpu);CYCLES(3);\n");
}

void gen037(struct Op* op) {
    out("instruction_aaa(cpu);CYCLES(3);\n");
}

void gen03f(struct Op* op) {
    out("instruction_aas(cpu);CYCLES(3);\n");
}

void OPCALL inc16_reg(struct CPU* cpu, struct Op* op);
void OPCALL inc16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen040(struct Op* op) {
    if (op->func==inc16_reg) {
        out("cpu->oldcf=getCF(cpu); cpu->dst.u16 = ");
        out(r16(op->r1));
        out("; cpu->result.u16=cpu->dst.u16 + 1; cpu->lazyFlags = FLAGS_INC16; ");
        out(r16(op->r1));
        out(" = cpu->result.u16;CYCLES(1);\n");
    } else if (op->func==inc16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" + 1;CYCLES(1);\n");
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
        out(" = cpu->result.u16;CYCLES(1);\n");
    } else if (op->func==inc16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" - 1;CYCLES(1);\n");
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
        out(" = cpu->result.u32;CYCLES(1);\n");
    } else if (op->func==inc32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" + 1;CYCLES(1);\n");
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
        out(" = cpu->result.u32;CYCLES(1);\n");
    } else if (op->func==inc32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" - 1;CYCLES(1);\n");
    }
}

void gen050(struct Op* op) {
    out("push16(cpu, ");
    out(r16(op->r1));
    out(");CYCLES(1);\n");
}

void gen058(struct Op* op) {
    out(r16(op->r1));
    out(" = pop16(cpu);CYCLES(1);\n");
}

void gen250(struct Op* op) {
    out("push32(cpu, ");
    out(r32(op->r1));
    out(");CYCLES(1);\n");
}

void gen258(struct Op* op) {
    out(r32(op->r1));
    out(" = pop32(cpu);CYCLES(1);\n");
}

void gen068(struct Op* op) {
    char tmp[16];

    out("push16(cpu, 0x");
    itoa(op->data1, tmp, 16);
    out(");CYCLES(1);");
}

void gen268(struct Op* op) {
    char tmp[16];

    out("push32(cpu, 0x");
    itoa(op->data1, tmp, 16);
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
    out(" = tmp32;CYCLES(10);\n");
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
        out(");CYCLES(10);\n");
        return;
    } else if (op->func == dimulcr32e32_16_noflags) {
        out(r32(op->r1));
        out(" = (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out(") * ((S32)0x");
        out(tmp);
        out(");CYCLES(10);\n");
        return;
    } else if (op->func == dimulcr32e32_32_noflags) {
        out(r32(op->r1));
        out(" = (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out(") * ((S32)0x");
        out(tmp);
        out(");CYCLES(10);\n");
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
    out(" = (S32)tmp64;CYCLES(10);\n");
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
    out(";cpu->nextBlock = getBlock1(cpu);}CYCLES(1);\n");
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
        genArithR("-", "FLAGS_CMP8", "8", r8(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==cmp8_mem16) {
        genArithE("-", "FLAGS_CMP8", "8", getEaa16(op), "b", op->data1, 1, 0, "2");
    } else if (op->func==cmp8_mem32) {
        genArithE("-", "FLAGS_CMP8", "8", getEaa32(op), "b", op->data1, 1, 0, "2");
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
        genArithR("-", "FLAGS_CMP16", "16", r16(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==cmp16_mem16) {
        genArithE("-", "FLAGS_CMP16", "16", getEaa16(op), "w", op->data1, 1, 0, "2");
    } else if (op->func==cmp16_mem32) {
        genArithE("-", "FLAGS_CMP16", "16", getEaa32(op), "w", op->data1, 1, 0, "2");
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
        genArithR("-", "FLAGS_CMP32", "32", r32(op->r1), op->data1, 1, 0, "1");
    } else if (op->func==cmp32_mem16) {
        genArithE("-", "FLAGS_CMP32", "32", getEaa16(op), "d", op->data1, 1, 0, "2");
    } else if (op->func==cmp32_mem32) {
        genArithE("-", "FLAGS_CMP32", "32", getEaa32(op), "d", op->data1, 1, 0, "2");
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
        out(" = tmp8; CYCLES(3);\n");
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
        out(" = tmp8; CYCLES(4);\n");
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
        out(" = tmp16; CYCLES(3);\n");
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
        out(" = tmp16; CYCLES(4);\n");
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
        out(" = tmp32; CYCLES(3);\n");
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
        out(" = tmp32; CYCLES(4);\n");
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
        out("; CYCLES(1);\n");
    } else {        
        out(r8(op->r1));
        out(" = readb(cpu->memory, ");
        if (op->func == move8r8_16) {
            out(getEaa16(op));
        } else if (op->func == move8r8_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen088");
        }
        out(");CYCLES(1);\n");
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
        out("; CYCLES(1);\n");
    } else {        
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        if (op->func == move16r16_16) {
            out(getEaa16(op));
        } else if (op->func == move16r16_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen089");
        }
        out(");CYCLES(1);\n");
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
        out("; CYCLES(1);\n");
    } else {        
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        if (op->func == move32r32_16) {
            out(getEaa16(op));
        } else if (op->func == move32r32_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen289");
        }
        out(");CYCLES(1);\n");
    }
}

void OPCALL movr8e8_16(struct CPU* cpu, struct Op* op);
void OPCALL movr8e8_32(struct CPU* cpu, struct Op* op);
void gen08a(struct Op* op) {
    if (op->func == movr8r8) {
        out(r8(op->r1));
        out(" = ");
        out(r8(op->r2));
        out("; CYCLES(1);\n");
    } else {                
        out("writeb(cpu->memory, ");
        if (op->func == movr8e8_16) {
            out(getEaa16(op));
        } else if (op->func == movr8e8_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen08a");
        }
        out(", ");
        out(r8(op->r1));
        out(");CYCLES(1);\n");
    }
}

void OPCALL movr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL movr16e16_32(struct CPU* cpu, struct Op* op);
void gen08b(struct Op* op) {
    if (op->func == movr16r16) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out("; CYCLES(1);\n");
    } else {                
        out("writew(cpu->memory, ");
        if (op->func == movr16e16_16) {
            out(getEaa16(op));
        } else if (op->func == movr16e16_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen08b");
        }
        out(", ");
        out(r16(op->r1));
        out(");CYCLES(1);\n");
    }
}

void OPCALL movr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL movr32e32_32(struct CPU* cpu, struct Op* op);
void gen28b(struct Op* op) {
    if (op->func == movr32r32) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out("; CYCLES(1);\n");
    } else {                
        out("writed(cpu->memory, ");
        if (op->func == movr32e32_16) {
            out(getEaa16(op));
        } else if (op->func == movr32e32_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen28b");
        }
        out(", ");
        out(r32(op->r1));
        out(");CYCLES(1);\n");
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
        out("]; CYCLES(1);\n");
    } else if (op->func == movr32s16) {
        out(r32(op->r1));
        out(" = ");
        out("cpu->segValue[");
        out(getBase(op->r2));
        out("]; CYCLES(1);\n");
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
        out("]);CYCLES(1);\n");
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
    out("; CYCLES(1);\n");
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
    out("; CYCLES(1);\n");
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
        out("] >> 3].base_addr; CYCLES(2);\n");
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
        out("] >> 3].base_addr; CYCLES(3);\n");
    }
}

void OPCALL popReg16(struct CPU* cpu, struct Op* op);
void OPCALL pope16_16(struct CPU* cpu, struct Op* op);
void OPCALL pope16_32(struct CPU* cpu, struct Op* op);
void gen08f(struct Op* op) {
    if (op->func == popReg16) {
        out(r16(op->r1));
        out(" = pop16(cpu); CYCLES(1);\n");
    } else {                
        out("writew(cpu->memory, ");
        if (op->func == pope16_16) {
            out(getEaa16(op));
        } else if (op->func == pope16_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen08f");
        }
        out(", pop16(cpu)); CYCLES(3);\n");
    }
}

void OPCALL popReg32(struct CPU* cpu, struct Op* op);
void OPCALL pope32_16(struct CPU* cpu, struct Op* op);
void OPCALL pope32_32(struct CPU* cpu, struct Op* op);
void gen28f(struct Op* op) {
    if (op->func == popReg32) {
        out(r32(op->r1));
        out(" = pop32(cpu); CYCLES(1);\n");
    } else {                
        out("writed(cpu->memory, ");
        if (op->func == pope32_16) {
            out(getEaa16(op));
        } else if (op->func == pope32_32) {
            out(getEaa32(op));
        } else {
            kpanic("gen28f");
        }
        out(", pop32(cpu)); CYCLES(3);\n");
    }
}

void gen090(struct Op* op) {
    out("CYCLES(1);\n");
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
    out(" = tmp16; CYCLES(3);\n");
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
    out(" = tmp32; CYCLES(3);\n");
}

void gen098(struct Op* op) {
    out("AX = (S8)AL; CYCLES(3);\n");
}

void gen298(struct Op* op) {
    out("EAX = (S16)AX; CYCLES(3);\n");
}

void gen099(struct Op* op) {
    out ("if (((S16)AX) < 0) DX = 0xFFFF; else DX = 0; CYCLES(2);\n");
}

void gen299(struct Op* op) {
    out("if (((S32)EAX) < 0) EDX = 0xFFFFFFFF; else EDX = 0; CYCLES(2);\n");
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
    out(", cpu->eip.u32); CYCLES(4);\n");
}

void gen09c(struct Op* op) {
    out("fillFlags(cpu); push16(cpu, cpu->flags|2); CYCLES(3);\n");
}

void gen29c(struct Op* op) {
    out("fillFlags(cpu); push32(cpu, (cpu->flags|2) & 0xFCFFFF); CYCLES(3);\n");
}

void gen09d(struct Op* op) {
    out("cpu->lazyFlags = FLAGS_NONE; setFlags(cpu, pop16(cpu), FMASK_ALL & 0xFFFF); CYCLES(4);\n");
}

void gen29d(struct Op* op) {
    out("cpu->lazyFlags = FLAGS_NONE; setFlags(cpu, pop32(cpu), FMASK_ALL); CYCLES(4);\n");
}

void gen09e(struct Op* op) {
    out("fillFlags(cpu); setFlags(cpu, AH, FMASK_ALL & 0xFF); CYCLES(2);");
}

void gen09f(struct Op* op) {
    out("fillFlags(cpu); AH = cpu->flags & (SF|ZF|AF|PF|CF); CYCLES(2);\n");
}

void gen0a0(struct Op* op) {
    char tmp[16];

    out("AL = readb(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out("); CYCLES(1);\n");
}

void gen0a1(struct Op* op) {
    char tmp[16];

    out("AX = readw(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out("); CYCLES(1);\n");
}

void gen2a1(struct Op* op) {
    char tmp[16];

    out("EAX = readd(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out("); CYCLES(1);\n");
}

void gen0a2(struct Op* op) {
    char tmp[16];

    out("writeb(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out(", AL); CYCLES(1);\n");
}

void gen0a3(struct Op* op) {
    char tmp[16];

    out("writew(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out(", AX); CYCLES(1);\n");
}

void gen2a3(struct Op* op) {
    char tmp[16];

    out("writed(cpu->memory, cpu->segAddress[");
    out(getBase(op->base));
    out("] + 0x");
    itoa(op->data1, tmp, 16);
    out(tmp);
    out(", EAX); CYCLES(1);\n");
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
    out(");\n");
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
    out(");\n");
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
    out(");\n");
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
    out(");\n");
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
    out(");\n");
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
    out(");\n");
}

void OPCALL stosb16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL stosb16_op(struct CPU* cpu, struct Op* op);
void OPCALL stosb32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL stosb32_op(struct CPU* cpu, struct Op* op);
void gen0aa(struct Op* op) {
    if (op->func==stosb16_r_op) {
        out("stosb16_r(cpu);\n");        
    } else if (op->func==stosb16_op) {
        out("stosb16(cpu);\n");
    } else if (op->func==stosb32_r_op) {
        out("stosb32_r(cpu);\n");
    } else if (op->func==stosb32_op) {
        out("stosb32(cpu);\n");
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
        out("stosw16_r(cpu);\n");
    } else if (op->func==stosw16_op) {
        out("stosw16(cpu);\n");
    } else if (op->func==stosw32_r_op) {
        out("stosw32_r(cpu);\n");
    } else if (op->func==stosw32_op) {
        out("stosw32(cpu);\n");
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
        out("stosd16_r(cpu);\n");
    } else if (op->func==stosd16_op) {
        out("stosd16(cpu);\n");
    } else if (op->func==stosd32_r_op) {
        out("stosd32_r(cpu);\n");
    } else if (op->func==stosd32_op) {
        out("stosd32(cpu);\n");
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
    out(");\n");
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
    out(");\n");
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
    out(");\n");
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
    out(");\n");
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
    out(");\n");
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
    out(");\n");
}


void gen0a8(struct Op* op) {
    genArithR( op, "FLAGS_TEST8", "8", "0", op->data1, 0, 0, "1");
}

void gen0a9(struct Op* op) {
    genArithR( op, "FLAGS_TEST16", "16", "0", op->data1, 0, 0, "1");
}

void gen2a9(struct Op* op) {
    genArithR( op, "FLAGS_TEST32", "32", "0", op->data1, 0, 0, "1");
}

void gen0b0(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    out(r8(op->r1));
    out(" = 0x");
    out(tmp);
    out("; CYCLES(1);\n");
}

void gen0b8(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    out(r16(op->r1));
    out(" = 0x");
    out(tmp);
    out("; CYCLES(1);\n");
}

void gen2b8(struct Op* op) {
    char tmp[16];

    itoa(op->data1, tmp, 16);
    out(r32(op->r1));
    out(" = 0x");
    out(tmp);
    out("; CYCLES(1);\n");
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
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
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
	gen068, gen269, gen268, gen269, 0, 0, 0, 0,
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
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
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

void generateSource(struct CPU* cpu, U32 eip, struct Block* block) {
    struct Op* op = block->ops;
    char tmp[256];

    if (!fpGenSrc) {
        fpGenSrc = fopen("gen.c", "w");
        out("static U32 eaa\n");
        out("static U8 tmp8\n");
        out("static U16 tmp16\n");
        out("static U32 tmp32\n");
        out("static U64 tmp64\n\n");
    }
    sprintf(tmp, "// 0x%.8x %s at 0x%.8x\n", cpu->eip.u32, getModuleName(cpu, eip), getModuleEip(cpu, eip));
    out(tmp);
    sprintf(tmp, "generated%X", generatedBlocks++);
    out("void OPCALL ");
    out(tmp);
    out("(struct CPU* cpu, struct Op* op) {\n");    
    while (op) {
        if (srcgen[op->inst]) {
            out("    ");
            srcgen[op->inst](op);
        } else {
            char tmp[16];
            itoa(op->inst, tmp, 16);
            out("    // missing instruction: ");
            out(tmp);
            out("\n");
        }
        // the last op is responsible for handling the eip adjustment
        if (op->next) {
            out("    cpu->eip.u32+=");
            itoa(op->eipCount, tmp, 10);
            out(tmp);    
            out(";\n");
        }
        op = op->next;
    }
    out("}\n\n");
    fflush(fpGenSrc);
}
#endif
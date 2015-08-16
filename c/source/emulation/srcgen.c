#ifdef GENERATE_SOURCE
#include "op.h"
#include "block.h"
#include "cpu.h"
#include "kalloc.h"
#include "jit.h"

#include <stdio.h>
#include <stdlib.h>

#define sFLAGS_NONE 0
#define sFLAGS_ADD8 1
#define sFLAGS_ADD16 2
#define sFLAGS_ADD32 3
#define sFLAGS_OR8 4
#define sFLAGS_OR16 5
#define sFLAGS_OR32 6
#define sFLAGS_ADC8 7
#define sFLAGS_ADC16 8
#define sFLAGS_ADC32 9
#define sFLAGS_SBB8 10
#define sFLAGS_SBB16 11
#define sFLAGS_SBB32 12
#define sFLAGS_AND8 13
#define sFLAGS_AND16 14
#define sFLAGS_AND32 15
#define sFLAGS_SUB8 16
#define sFLAGS_SUB16 17
#define sFLAGS_SUB32 18
#define sFLAGS_XOR8 19
#define sFLAGS_XOR16 20
#define sFLAGS_XOR32 21
#define sFLAGS_INC8 22
#define sFLAGS_INC16 23
#define sFLAGS_INC32 24
#define sFLAGS_DEC8 25
#define sFLAGS_DEC16 26
#define sFLAGS_DEC32 27
#define sFLAGS_SHL8 28
#define sFLAGS_SHL16 29
#define sFLAGS_SHL32 30
#define sFLAGS_SHR8 31
#define sFLAGS_SHR16 32
#define sFLAGS_SHR32 33
#define sFLAGS_SAR8 34
#define sFLAGS_SAR16 35
#define sFLAGS_SAR32 36
#define sFLAGS_CMP8 37
#define sFLAGS_CMP16 38
#define sFLAGS_CMP32 39
#define sFLAGS_TEST8 40
#define sFLAGS_TEST16 41
#define sFLAGS_TEST32 42
#define sFLAGS_DSHL16 43
#define sFLAGS_DSHL32 44
#define sFLAGS_DSHR16 45
#define sFLAGS_DSHR32 46
#define sFLAGS_NEG8 47
#define sFLAGS_NEG16 48
#define sFLAGS_NEG32 49

char* sourceBuffer;
U32 sourceBufferLen;
U32 sourceBufferPos;
struct CPU* genCPU;
struct Block* currentGenBlock;
void writeOps(struct Op* op);
U32 currentLazyFlags;

U32 needsToSetFlag(struct Op* op, U32 flag);

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

const char* srcGetZF() {
    switch (currentLazyFlags) {
    case sFLAGS_NONE:
        return "(cpu->flags & ZF)";
    case sFLAGS_ADD8:
    case sFLAGS_OR8:
    case sFLAGS_ADC8:
    case sFLAGS_SBB8:
    case sFLAGS_AND8:
    case sFLAGS_SUB8:
    case sFLAGS_XOR8:
    case sFLAGS_INC8:
    case sFLAGS_DEC8:
    case sFLAGS_SHL8:
    case sFLAGS_SHR8:
    case sFLAGS_SAR8:
    case sFLAGS_CMP8:
    case sFLAGS_TEST8:
    case sFLAGS_NEG8:
        return "(cpu->result.u8 == 0)";

    case sFLAGS_ADD16:
    case sFLAGS_OR16:
    case sFLAGS_ADC16:
    case sFLAGS_SBB16:
    case sFLAGS_AND16:
    case sFLAGS_SUB16:
    case sFLAGS_XOR16:
    case sFLAGS_INC16:
    case sFLAGS_DEC16:
    case sFLAGS_SHL16:
    case sFLAGS_SHR16:
    case sFLAGS_SAR16:
    case sFLAGS_CMP16:
    case sFLAGS_TEST16:
    case sFLAGS_DSHL16:
    case sFLAGS_DSHR16:
    case sFLAGS_NEG16:
        return "(cpu->result.u16 == 0)";

    case sFLAGS_ADD32:
    case sFLAGS_OR32:
    case sFLAGS_ADC32:
    case sFLAGS_SBB32:
    case sFLAGS_AND32:
    case sFLAGS_SUB32:
    case sFLAGS_XOR32:
    case sFLAGS_INC32:
    case sFLAGS_DEC32:
    case sFLAGS_SHL32:
    case sFLAGS_SHR32:
    case sFLAGS_SAR32:
    case sFLAGS_CMP32:
    case sFLAGS_TEST32:
    case sFLAGS_DSHL32:
    case sFLAGS_DSHR32:
    case sFLAGS_NEG32:
        return "(cpu->result.u32 == 0)";
    }
    kpanic("srcGetZF %d", currentLazyFlags);
    return "";
}

const char* srcGetSF() {
    switch (currentLazyFlags) {
    case sFLAGS_NONE:
        return "(cpu->flags & SF)";
    case sFLAGS_ADD8:
    case sFLAGS_OR8:
    case sFLAGS_ADC8:
    case sFLAGS_SBB8:
    case sFLAGS_AND8:
    case sFLAGS_SUB8:
    case sFLAGS_XOR8:
    case sFLAGS_INC8:
    case sFLAGS_DEC8:
    case sFLAGS_SHL8:
    case sFLAGS_SHR8:
    case sFLAGS_SAR8:
    case sFLAGS_CMP8:
    case sFLAGS_TEST8:
    case sFLAGS_NEG8:
        return "(cpu->result.u8 & 0x80)";

    case sFLAGS_ADD16:
    case sFLAGS_OR16:
    case sFLAGS_ADC16:
    case sFLAGS_SBB16:
    case sFLAGS_AND16:
    case sFLAGS_SUB16:
    case sFLAGS_XOR16:
    case sFLAGS_INC16:
    case sFLAGS_DEC16:
    case sFLAGS_SHL16:
    case sFLAGS_SHR16:
    case sFLAGS_SAR16:
    case sFLAGS_CMP16:
    case sFLAGS_TEST16:
    case sFLAGS_DSHL16:
    case sFLAGS_DSHR16:
    case sFLAGS_NEG16:
        return "(cpu->result.u16 & 0x8000)";

    case sFLAGS_ADD32:
    case sFLAGS_OR32:
    case sFLAGS_ADC32:
    case sFLAGS_SBB32:
    case sFLAGS_AND32:
    case sFLAGS_SUB32:
    case sFLAGS_XOR32:
    case sFLAGS_INC32:
    case sFLAGS_DEC32:
    case sFLAGS_SHL32:
    case sFLAGS_SHR32:
    case sFLAGS_SAR32:
    case sFLAGS_CMP32:
    case sFLAGS_TEST32:
    case sFLAGS_DSHL32:
    case sFLAGS_DSHR32:
    case sFLAGS_NEG32:
        return "(cpu->result.u32 & 0x80000000)";
    }
    kpanic("srcGetSF %d", currentLazyFlags);
    return "";
}

const char* getFlag(int flag) {
    if (flag == OF)
        return "getOF(cpu)";
    else if (flag == CF)
        return "getCF(cpu)";
    else if (flag == SF)
        return srcGetSF();
    else if (flag == PF)
        return "getPF(cpu)";
    else if (flag == ZF)
        return srcGetZF();
    else if (flag == AF)
        return "getAF(cpu)";
    else
        kpanic("outGetFlag %d", flag);
    return "";
}

char tmpCondition[256];

const char* getCondition(int condition) {
    switch (condition) {
    case 0: return getFlag(OF);
    case 1: strcpy(tmpCondition, "!"); strcat(tmpCondition, getFlag(OF)); return tmpCondition;
    case 2: return getFlag(CF);
    case 3: strcpy(tmpCondition, "!"); strcat(tmpCondition, getFlag(CF)); return tmpCondition;
    case 4: return getFlag(ZF);
    case 5: strcpy(tmpCondition, "!"); strcat(tmpCondition, getFlag(ZF)); return tmpCondition;
    case 6: strcpy(tmpCondition, getFlag(ZF)); strcat(tmpCondition, " || "); strcat(tmpCondition, getFlag(CF)); return tmpCondition;
    case 7: strcpy(tmpCondition, "!"); strcat(tmpCondition, getFlag(ZF)); strcat(tmpCondition, " && !"); strcat(tmpCondition, getFlag(CF)); return tmpCondition;
    case 8: return getFlag(SF);
    case 9: strcpy(tmpCondition, "!"); strcat(tmpCondition, getFlag(SF)); return tmpCondition;
    case 10: return getFlag(PF);
    case 11: strcpy(tmpCondition, "!"); strcat(tmpCondition, getFlag(PF)); return tmpCondition;
    case 12: strcpy(tmpCondition, getFlag(SF)); strcat(tmpCondition, " != "); strcat(tmpCondition, getFlag(OF)); return tmpCondition;
    case 13: strcpy(tmpCondition, getFlag(SF)); strcat(tmpCondition, " == "); strcat(tmpCondition, getFlag(OF)); return tmpCondition;
    case 14: strcpy(tmpCondition, getFlag(ZF)); strcat(tmpCondition, " || "), strcat(tmpCondition, getFlag(SF)); strcat(tmpCondition, " != "); strcat(tmpCondition, getFlag(OF)); return tmpCondition;
    case 15: strcpy(tmpCondition, "!"); strcat(tmpCondition, getFlag(ZF)); strcat(tmpCondition, " && "), strcat(tmpCondition, getFlag(SF)); strcat(tmpCondition, " == "); strcat(tmpCondition, getFlag(OF)); return tmpCondition;
    default: kpanic("getCondition %d", condition); return "";
    }
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
        out("cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u16 = ");
        out(r16(op->r1));
        out("; cpu->result.u16=cpu->dst.u16 + 1; cpu->lazyFlags = FLAGS_INC16; ");
        currentLazyFlags = sFLAGS_INC16;
        out(r16(op->r1));
        out(" = cpu->result.u16;CYCLES(1);");
    } else if (op->func==inc16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" + 1;CYCLES(1);");
    } else {
        kpanic("gen040");
    }
}

void OPCALL dec16_reg(struct CPU* cpu, struct Op* op);
void OPCALL dec16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen048(struct Op* op) {
    if (op->func==dec16_reg) {
        out("cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u16 = ");
        out(r16(op->r1));
        out("; cpu->result.u16=cpu->dst.u16 - 1; cpu->lazyFlags = FLAGS_DEC16; ");
        currentLazyFlags = sFLAGS_DEC16;
        out(r16(op->r1));
        out(" = cpu->result.u16;CYCLES(1);");
    } else if (op->func==dec16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" - 1;CYCLES(1);");
    } else {
        kpanic("gen048");
    }
}

void OPCALL inc32_reg(struct CPU* cpu, struct Op* op);
void OPCALL inc32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen240(struct Op* op) {
    if (op->func==inc32_reg) {
        out("cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u32 = ");
        out(r32(op->r1));
        out("; cpu->result.u32=cpu->dst.u32 + 1; cpu->lazyFlags = FLAGS_INC32; ");
        currentLazyFlags = sFLAGS_INC32;
        out(r32(op->r1));
        out(" = cpu->result.u32;CYCLES(1);");
    } else if (op->func==inc32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" + 1;CYCLES(1);");
    } else {
        kpanic("gen240");
    }
}

void OPCALL dec32_reg(struct CPU* cpu, struct Op* op);
void OPCALL dec32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen248(struct Op* op) {
    if (op->func==dec32_reg) {
        out("cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u32 = ");
        out(r32(op->r1));
        out("; cpu->result.u32=cpu->dst.u32 - 1; cpu->lazyFlags = FLAGS_DEC32; ");
        currentLazyFlags = sFLAGS_DEC32;
        out(r32(op->r1));
        out(" = cpu->result.u32;CYCLES(1);");
    } else if (op->func==dec32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" - 1;CYCLES(1);");
    } else {
        kpanic("gen248");
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
    currentLazyFlags = sFLAGS_NONE;
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
    currentLazyFlags = sFLAGS_NONE;
    out(r32(op->r1));
    out(" = (S32)tmp64;CYCLES(10);");
}

void genJump(struct Op* op, int condition) {
    char tmp[16];
    itoa(op->data1+op->eipCount, tmp, 10);

    out("if (");
    out(getCondition(condition));
    out(") {cpu->eip.u32+=");
    out(tmp);    
    out("; cpu->nextBlock = getBlock2(cpu);} else {cpu->eip.u32+=");
    itoa(op->eipCount, tmp, 10);
    out(tmp);
    out(";cpu->nextBlock = getBlock1(cpu);}CYCLES(1);");
}

void gen070(struct Op* op) {
    genJump(op, 0);
}

void gen071(struct Op* op) {
    genJump(op, 1);
}

void gen072(struct Op* op) {
    genJump(op, 2);
}

void gen073(struct Op* op) {
    genJump(op, 3);
}

void gen074(struct Op* op) {
    genJump(op, 4);
}

void gen075(struct Op* op) {
    genJump(op, 5);
}

void gen076(struct Op* op) {
    genJump(op, 6);
}

void gen077(struct Op* op) {
    genJump(op, 7);
}

void gen078(struct Op* op) {
    genJump(op, 8);
}

void gen079(struct Op* op) {
    genJump(op, 9);
}

void gen07a(struct Op* op) {
    genJump(op, 10);
}

void gen07b(struct Op* op) {
    genJump(op, 11);
}

void gen07c(struct Op* op) {
    genJump(op, 12);
}

void gen07d(struct Op* op) {
    genJump(op, 13);
}

void gen07e(struct Op* op) {
    genJump(op, 14);
}

void gen07f(struct Op* op) {
    genJump(op, 15);
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
        currentLazyFlags = sFLAGS_ADD8;
    } else if (op->func==add8_mem16) {
        genArithE("+", "FLAGS_ADD8", "8", getEaa16(op), "b", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_ADD8;
    } else if (op->func==add8_mem32) {
        genArithE("+", "FLAGS_ADD8", "8", getEaa32(op), "b", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_ADD8;
    } else if (op->func==add8_reg_noflags) {
        genArithR_noflags("+", "8", r8(op->r1), op->data1, 0, "1");
    } else if (op->func==add8_mem16_noflags) {
        genArithE_noflags("+", "8", getEaa16(op), "b", op->data1, 0, "3");
    } else if (op->func==add8_mem32_noflags) {
        genArithE_noflags("+", "8", getEaa32(op), "b", op->data1, 0, "3");
    }

    else if (op->func==or8_reg) {
        genArithR("|", "FLAGS_OR8", "8", r8(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_OR8;
    } else if (op->func==or8_mem16) {
        genArithE("|", "FLAGS_OR8", "8", getEaa16(op), "b", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_OR8;
    } else if (op->func==or8_mem32) {
        genArithE("|", "FLAGS_OR8", "8", getEaa32(op), "b", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_OR8;
    } else if (op->func==or8_reg_noflags) {
        genArithR_noflags("|", "8", r8(op->r1), op->data1, 0, "1");
    } else if (op->func==or8_mem16_noflags) {
        genArithE_noflags("|", "8", getEaa16(op), "b", op->data1, 0, "3");
    } else if (op->func==or8_mem32_noflags) {
        genArithE_noflags("|", "8", getEaa32(op), "b", op->data1, 0, "3");
    }

    else if (op->func==adc8_reg) {
        genArithR("+", "FLAGS_ADC8", "8", r8(op->r1), op->data1, 1, 1, "1");
        currentLazyFlags = sFLAGS_ADC8;
    } else if (op->func==adc8_mem16) {
        genArithE("+", "FLAGS_ADC8", "8", getEaa16(op), "b", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_ADC8;
    } else if (op->func==adc8_mem32) {
        genArithE("+", "FLAGS_ADC8", "8", getEaa32(op), "b", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_ADC8;
    } else if (op->func==adc8_reg_noflags) {
        genArithR_noflags("+", "8", r8(op->r1), op->data1, 1, "1");
    } else if (op->func==adc8_mem16_noflags) {
        genArithE_noflags("+", "8", getEaa16(op), "b", op->data1, 1, "3");
    } else if (op->func==adc8_mem32_noflags) {
        genArithE_noflags("+", "8", getEaa32(op), "b", op->data1, 1, "3");
    }

    else if (op->func==sbb8_reg) {
        genArithR("-", "FLAGS_SBB8", "8", r8(op->r1), op->data1, 1, 1, "1");
        currentLazyFlags = sFLAGS_SBB8;
    } else if (op->func==sbb8_mem16) {
        genArithE("-", "FLAGS_SBB8", "8", getEaa16(op), "b", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_SBB8;
    } else if (op->func==sbb8_mem32) {
        genArithE("-", "FLAGS_SBB8", "8", getEaa32(op), "b", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_SBB8;
    } else if (op->func==sbb8_reg_noflags) {
        genArithR_noflags("-", "8", r8(op->r1), op->data1, 1, "1");
    } else if (op->func==sbb8_mem16_noflags) {
        genArithE_noflags("-", "8", getEaa16(op), "b", op->data1, 1, "3");
    } else if (op->func==sbb8_mem32_noflags) {
        genArithE_noflags("-", "8", getEaa32(op), "b", op->data1, 1, "3");
    }

    else if (op->func==and8_reg) {
        genArithR("&", "FLAGS_AND8", "8", r8(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_AND8;
    } else if (op->func==and8_mem16) {
        genArithE("&", "FLAGS_AND8", "8", getEaa16(op), "b", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_AND8;
    } else if (op->func==and8_mem32) {
        genArithE("&", "FLAGS_AND8", "8", getEaa32(op), "b", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_AND8;
    } else if (op->func==and8_reg_noflags) {
        genArithR_noflags("&", "8", r8(op->r1), op->data1, 0, "1");
    } else if (op->func==and8_mem16_noflags) {
        genArithE_noflags("&", "8", getEaa16(op), "b", op->data1, 0, "3");
    } else if (op->func==and8_mem32_noflags) {
        genArithE_noflags("&", "8", getEaa32(op), "b", op->data1, 0, "3");
    }

    else if (op->func==sub8_reg) {
        genArithR("-", "FLAGS_SUB8", "8", r8(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_SUB8;
    } else if (op->func==sub8_mem16) {
        genArithE("-", "FLAGS_SUB8", "8", getEaa16(op), "b", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_SUB8;
    } else if (op->func==sub8_mem32) {
        genArithE("-", "FLAGS_SUB8", "8", getEaa32(op), "b", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_SUB8;
    } else if (op->func==sub8_reg_noflags) {
        genArithR_noflags("-", "8", r8(op->r1), op->data1, 0, "1");
    } else if (op->func==sub8_mem16_noflags) {
        genArithE_noflags("-", "8", getEaa16(op), "b", op->data1, 0, "3");
    } else if (op->func==sub8_mem32_noflags) {
        genArithE_noflags("-", "8", getEaa32(op), "b", op->data1, 0, "3");
    }

    else if (op->func==xor8_reg) {
        genArithR("^", "FLAGS_XOR8", "8", r8(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_XOR8;
    } else if (op->func==xor8_mem16) {
        genArithE("^", "FLAGS_XOR8", "8", getEaa16(op), "b", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_XOR8;
    } else if (op->func==xor8_mem32) {
        genArithE("^", "FLAGS_XOR8", "8", getEaa32(op), "b", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_XOR8;
    } else if (op->func==xor8_reg_noflags) {
        genArithR_noflags("^", "8", r8(op->r1), op->data1, 0, "1");
    } else if (op->func==xor8_mem16_noflags) {
        genArithE_noflags("^", "8", getEaa16(op), "b", op->data1, 0, "3");
    } else if (op->func==xor8_mem32_noflags) {
        genArithE_noflags("^", "8", getEaa32(op), "b", op->data1, 0, "3");
    }

    else if (op->func==cmp8_reg) {
        genArithR("-", "FLAGS_CMP8", "8", r8(op->r1), op->data1, 0, 0, "1");
        currentLazyFlags = sFLAGS_CMP8;
    } else if (op->func==cmp8_mem16) {
        genArithE("-", "FLAGS_CMP8", "8", getEaa16(op), "b", op->data1, 0, 0, "2");
        currentLazyFlags = sFLAGS_CMP8;
    } else if (op->func==cmp8_mem32) {
        genArithE("-", "FLAGS_CMP8", "8", getEaa32(op), "b", op->data1, 0, 0, "2");
        currentLazyFlags = sFLAGS_CMP8;
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
        currentLazyFlags = sFLAGS_ADD16;
    } else if (op->func==add16_mem16) {
        genArithE("+", "FLAGS_ADD16", "16", getEaa16(op), "w", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_ADD16;
    } else if (op->func==add16_mem32) {
        genArithE("+", "FLAGS_ADD16", "16", getEaa32(op), "w", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_ADD16;
    } else if (op->func==add16_reg_noflags) {
        genArithR_noflags("+", "16", r16(op->r1), op->data1, 0, "1");
    } else if (op->func==add16_mem16_noflags) {
        genArithE_noflags("+", "16", getEaa16(op), "w", op->data1, 0, "3");
    } else if (op->func==add16_mem32_noflags) {
        genArithE_noflags("+", "16", getEaa32(op), "w", op->data1, 0, "3");
    }

    else if (op->func==or16_reg) {
        genArithR("|", "FLAGS_OR16", "16", r16(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_OR16;
    } else if (op->func==or16_mem16) {
        genArithE("|", "FLAGS_OR16", "16", getEaa16(op), "w", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_OR16;
    } else if (op->func==or16_mem32) {
        genArithE("|", "FLAGS_OR16", "16", getEaa32(op), "w", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_OR16;
    } else if (op->func==or16_reg_noflags) {
        genArithR_noflags("|", "16", r16(op->r1), op->data1, 0, "1");
    } else if (op->func==or16_mem16_noflags) {
        genArithE_noflags("|", "16", getEaa16(op), "w", op->data1, 0, "3");
    } else if (op->func==or16_mem32_noflags) {
        genArithE_noflags("|", "16", getEaa32(op), "w", op->data1, 0, "3");
    }

    else if (op->func==adc16_reg) {
        genArithR("+", "FLAGS_ADC16", "16", r16(op->r1), op->data1, 1, 1, "1");
        currentLazyFlags = sFLAGS_ADC16;
    } else if (op->func==adc16_mem16) {
        genArithE("+", "FLAGS_ADC16", "16", getEaa16(op), "w", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_ADC16;
    } else if (op->func==adc16_mem32) {
        genArithE("+", "FLAGS_ADC16", "16", getEaa32(op), "w", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_ADC16;
    } else if (op->func==adc16_reg_noflags) {
        genArithR_noflags("+", "16", r16(op->r1), op->data1, 1, "1");
    } else if (op->func==adc16_mem16_noflags) {
        genArithE_noflags("+", "16", getEaa16(op), "w", op->data1, 1, "3");
    } else if (op->func==adc16_mem32_noflags) {
        genArithE_noflags("+", "16", getEaa32(op), "w", op->data1, 1, "3");
    }

    else if (op->func==sbb16_reg) {
        genArithR("-", "FLAGS_SBB16", "16", r16(op->r1), op->data1, 1, 1, "1");
        currentLazyFlags = sFLAGS_SBB16;
    } else if (op->func==sbb16_mem16) {
        genArithE("-", "FLAGS_SBB16", "16", getEaa16(op), "w", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_SBB16;
    } else if (op->func==sbb16_mem32) {
        genArithE("-", "FLAGS_SBB16", "16", getEaa32(op), "w", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_SBB16;
    } else if (op->func==sbb16_reg_noflags) {
        genArithR_noflags("-", "16", r16(op->r1), op->data1, 1, "1");
    } else if (op->func==sbb16_mem16_noflags) {
        genArithE_noflags("-", "16", getEaa16(op), "w", op->data1, 1, "3");
    } else if (op->func==sbb16_mem32_noflags) {
        genArithE_noflags("-", "16", getEaa32(op), "w", op->data1, 1, "3");
    }

    else if (op->func==and16_reg) {
        genArithR("&", "FLAGS_AND16", "16", r16(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_AND16;
    } else if (op->func==and16_mem16) {
        genArithE("&", "FLAGS_AND16", "16", getEaa16(op), "w", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_AND16;
    } else if (op->func==and16_mem32) {
        genArithE("&", "FLAGS_AND16", "16", getEaa32(op), "w", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_AND16;
    } else if (op->func==and16_reg_noflags) {
        genArithR_noflags("&", "16", r16(op->r1), op->data1, 0, "1");
    } else if (op->func==and16_mem16_noflags) {
        genArithE_noflags("&", "16", getEaa16(op), "w", op->data1, 0, "3");
    } else if (op->func==and16_mem32_noflags) {
        genArithE_noflags("&", "16", getEaa32(op), "w", op->data1, 0, "3");
    }

    else if (op->func==sub16_reg) {
        genArithR("-", "FLAGS_SUB16", "16", r16(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_SUB16;
    } else if (op->func==sub16_mem16) {
        genArithE("-", "FLAGS_SUB16", "16", getEaa16(op), "w", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_SUB16;
    } else if (op->func==sub16_mem32) {
        genArithE("-", "FLAGS_SUB16", "16", getEaa32(op), "w", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_SUB16;
    } else if (op->func==sub16_reg_noflags) {
        genArithR_noflags("-", "16", r16(op->r1), op->data1, 0, "1");
    } else if (op->func==sub16_mem16_noflags) {
        genArithE_noflags("-", "16", getEaa16(op), "w", op->data1, 0, "3");
    } else if (op->func==sub16_mem32_noflags) {
        genArithE_noflags("-", "16", getEaa32(op), "w", op->data1, 0, "3");
    }

    else if (op->func==xor16_reg) {
        genArithR("^", "FLAGS_XOR16", "16", r16(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_XOR16;
    } else if (op->func==xor16_mem16) {
        genArithE("^", "FLAGS_XOR16", "16", getEaa16(op), "w", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_XOR16;
    } else if (op->func==xor16_mem32) {
        genArithE("^", "FLAGS_XOR16", "16", getEaa32(op), "w", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_XOR16;
    } else if (op->func==xor16_reg_noflags) {
        genArithR_noflags("^", "16", r16(op->r1), op->data1, 0, "1");
    } else if (op->func==xor16_mem16_noflags) {
        genArithE_noflags("^", "16", getEaa16(op), "w", op->data1, 0, "3");
    } else if (op->func==xor16_mem32_noflags) {
        genArithE_noflags("^", "16", getEaa32(op), "w", op->data1, 0, "3");
    }

    else if (op->func==cmp16_reg) {
        genArithR("-", "FLAGS_CMP16", "16", r16(op->r1), op->data1, 0, 0, "1");
        currentLazyFlags = sFLAGS_CMP16;
    } else if (op->func==cmp16_mem16) {
        genArithE("-", "FLAGS_CMP16", "16", getEaa16(op), "w", op->data1, 0, 0, "2");
        currentLazyFlags = sFLAGS_CMP16;
    } else if (op->func==cmp16_mem32) {
        genArithE("-", "FLAGS_CMP16", "16", getEaa32(op), "w", op->data1, 0, 0, "2");
        currentLazyFlags = sFLAGS_CMP16;
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
        currentLazyFlags = sFLAGS_ADD32;
    } else if (op->func==add32_mem16) {
        genArithE("+", "FLAGS_ADD32", "32", getEaa16(op), "d", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_ADD32;
    } else if (op->func==add32_mem32) {
        genArithE("+", "FLAGS_ADD32", "32", getEaa32(op), "d", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_ADD32;
    } else if (op->func==add32_reg_noflags) {
        genArithR_noflags("+", "32", r32(op->r1), op->data1, 0, "1");
    } else if (op->func==add32_mem16_noflags) {
        genArithE_noflags("+", "32", getEaa16(op), "d", op->data1, 0, "3");
    } else if (op->func==add32_mem32_noflags) {
        genArithE_noflags("+", "32", getEaa32(op), "d", op->data1, 0, "3");
    }

    else if (op->func==or32_reg) {
        genArithR("|", "FLAGS_OR32", "32", r32(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_OR32;
    } else if (op->func==or32_mem16) {
        genArithE("|", "FLAGS_OR32", "32", getEaa16(op), "d", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_OR32;
    } else if (op->func==or32_mem32) {
        genArithE("|", "FLAGS_OR32", "32", getEaa32(op), "d", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_OR32;
    } else if (op->func==or32_reg_noflags) {
        genArithR_noflags("|", "32", r32(op->r1), op->data1, 0, "1");
    } else if (op->func==or32_mem16_noflags) {
        genArithE_noflags("|", "32", getEaa16(op), "d", op->data1, 0, "3");
    } else if (op->func==or32_mem32_noflags) {
        genArithE_noflags("|", "32", getEaa32(op), "d", op->data1, 0, "3");
    }

    else if (op->func==adc32_reg) {
        genArithR("+", "FLAGS_ADC32", "32", r32(op->r1), op->data1, 1, 1, "1");
        currentLazyFlags = sFLAGS_ADC32;
    } else if (op->func==adc32_mem16) {
        genArithE("+", "FLAGS_ADC32", "32", getEaa16(op), "d", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_ADC32;
    } else if (op->func==adc32_mem32) {
        genArithE("+", "FLAGS_ADC32", "32", getEaa32(op), "d", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_ADC32;
    } else if (op->func==adc32_reg_noflags) {
        genArithR_noflags("+", "32", r32(op->r1), op->data1, 1, "1");
    } else if (op->func==adc32_mem16_noflags) {
        genArithE_noflags("+", "32", getEaa16(op), "d", op->data1, 1, "3");
    } else if (op->func==adc32_mem32_noflags) {
        genArithE_noflags("+", "32", getEaa32(op), "d", op->data1, 1, "3");
    }

    else if (op->func==sbb32_reg) {
        genArithR("-", "FLAGS_SBB32", "32", r32(op->r1), op->data1, 1, 1, "1");
        currentLazyFlags = sFLAGS_SBB32;
    } else if (op->func==sbb32_mem16) {
        genArithE("-", "FLAGS_SBB32", "32", getEaa16(op), "d", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_SBB32;
    } else if (op->func==sbb32_mem32) {
        genArithE("-", "FLAGS_SBB32", "32", getEaa32(op), "d", op->data1, 1, 1, "3");
        currentLazyFlags = sFLAGS_SBB32;
    } else if (op->func==sbb32_reg_noflags) {
        genArithR_noflags("-", "32", r32(op->r1), op->data1, 1, "1");
    } else if (op->func==sbb32_mem16_noflags) {
        genArithE_noflags("-", "32", getEaa16(op), "d", op->data1, 1, "3");
    } else if (op->func==sbb32_mem32_noflags) {
        genArithE_noflags("-", "32", getEaa32(op), "d", op->data1, 1, "3");
    }

    else if (op->func==and32_reg) {
        genArithR("&", "FLAGS_AND32", "32", r32(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_AND32;
    } else if (op->func==and32_mem16) {
        genArithE("&", "FLAGS_AND32", "32", getEaa16(op), "d", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_AND32;
    } else if (op->func==and32_mem32) {
        genArithE("&", "FLAGS_AND32", "32", getEaa32(op), "d", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_AND32;
    } else if (op->func==and32_reg_noflags) {
        genArithR_noflags("&", "32", r32(op->r1), op->data1, 0, "1");
    } else if (op->func==and32_mem16_noflags) {
        genArithE_noflags("&", "32", getEaa16(op), "d", op->data1, 0, "3");
    } else if (op->func==and32_mem32_noflags) {
        genArithE_noflags("&", "32", getEaa32(op), "d", op->data1, 0, "3");
    }

    else if (op->func==sub32_reg) {
        genArithR("-", "FLAGS_SUB32", "32", r32(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_SUB32;
    } else if (op->func==sub32_mem16) {
        genArithE("-", "FLAGS_SUB32", "32", getEaa16(op), "d", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_SUB32;
    } else if (op->func==sub32_mem32) {
        genArithE("-", "FLAGS_SUB32", "32", getEaa32(op), "d", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_SUB32;
    } else if (op->func==sub32_reg_noflags) {
        genArithR_noflags("-", "32", r32(op->r1), op->data1, 0, "1");
    } else if (op->func==sub32_mem16_noflags) {
        genArithE_noflags("-", "32", getEaa16(op), "d", op->data1, 0, "3");
    } else if (op->func==sub32_mem32_noflags) {
        genArithE_noflags("-", "32", getEaa32(op), "d", op->data1, 0, "3");
    }

    else if (op->func==xor32_reg) {
        genArithR("^", "FLAGS_XOR32", "32", r32(op->r1), op->data1, 1, 0, "1");
        currentLazyFlags = sFLAGS_XOR32;
    } else if (op->func==xor32_mem16) {
        genArithE("^", "FLAGS_XOR32", "32", getEaa16(op), "d", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_XOR32;
    } else if (op->func==xor32_mem32) {
        genArithE("^", "FLAGS_XOR32", "32", getEaa32(op), "d", op->data1, 1, 0, "3");
        currentLazyFlags = sFLAGS_XOR32;
    } else if (op->func==xor32_reg_noflags) {
        genArithR_noflags("^", "32", r32(op->r1), op->data1, 0, "1");
    } else if (op->func==xor32_mem16_noflags) {
        genArithE_noflags("^", "32", getEaa16(op), "d", op->data1, 0, "3");
    } else if (op->func==xor32_mem32_noflags) {
        genArithE_noflags("^", "32", getEaa32(op), "d", op->data1, 0, "3");
    }

    else if (op->func==cmp32_reg) {
        genArithR("-", "FLAGS_CMP32", "32", r32(op->r1), op->data1, 0, 0, "1");
        currentLazyFlags = sFLAGS_CMP32;
    } else if (op->func==cmp32_mem16) {
        genArithE("-", "FLAGS_CMP32", "32", getEaa16(op), "d", op->data1, 0, 0, "2");
        currentLazyFlags = sFLAGS_CMP32;
    } else if (op->func==cmp32_mem32) {
        genArithE("-", "FLAGS_CMP32", "32", getEaa32(op), "d", op->data1, 0, 0, "2");
        currentLazyFlags = sFLAGS_CMP32;
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
    currentLazyFlags = sFLAGS_TEST8;
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
    currentLazyFlags = sFLAGS_TEST16;
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
    currentLazyFlags = sFLAGS_TEST32;
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
        out("] = cpu->ldt[cpu->segValue[");
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
        out("] = cpu->ldt[cpu->segValue[");
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
    currentLazyFlags = sFLAGS_NONE;
}

void gen29c(struct Op* op) {
    out("fillFlags(cpu); push32(cpu, (cpu->flags|2) & 0xFCFFFF); CYCLES(3);");
    currentLazyFlags = sFLAGS_NONE;
}

void gen09d(struct Op* op) {
    out("cpu->lazyFlags = FLAGS_NONE; setFlags(cpu, pop16(cpu), FMASK_ALL & 0xFFFF); CYCLES(4);");
    currentLazyFlags = sFLAGS_NONE;
}

void gen29d(struct Op* op) {
    out("cpu->lazyFlags = FLAGS_NONE; setFlags(cpu, pop32(cpu), FMASK_ALL); CYCLES(4);");
    currentLazyFlags = sFLAGS_NONE;
}

void gen09e(struct Op* op) {
    out("fillFlags(cpu); setFlags(cpu, AH, FMASK_ALL & 0xFF); CYCLES(2);");
    currentLazyFlags = sFLAGS_NONE;
}

void gen09f(struct Op* op) {
    out("fillFlags(cpu); AH = cpu->flags & (SF|ZF|AF|PF|CF); CYCLES(2);");
    currentLazyFlags = sFLAGS_NONE;
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
        out(getBase(op->base));
        out(");");
    } else if (op->func==movsb16_op) {
        out("writeb(cpu->memory, cpu->segAddress[ES]+DI, readb(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+SI)); DI+=cpu->df; SI+=cpu->df; CYCLES(4);");
    } else if (op->func==movsb32_r_op) {
        out("movsb32_r(cpu, ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==movsb32_op) {
        out("writeb(cpu->memory, cpu->segAddress[ES]+EDI, readb(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+ESI)); EDI+=cpu->df; ESI+=cpu->df; CYCLES(4);");
    } else {
        kpanic("gen0a4");
    }    
}

void OPCALL movsw16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL movsw16_op(struct CPU* cpu, struct Op* op);
void OPCALL movsw32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL movsw32_op(struct CPU* cpu, struct Op* op);
void gen0a5(struct Op* op) {
    if (op->func==movsw16_r_op) {
        out("movsw16_r(cpu, ");        
        out(getBase(op->base));
        out(");");
    } else if (op->func==movsw16_op) {
        out("writew(cpu->memory, cpu->segAddress[ES]+DI, readw(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+SI)); DI+=cpu->df<<1; SI+=cpu->df<<1; CYCLES(4);");
    } else if (op->func==movsw32_r_op) {
        out("movsw32_r(cpu, ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==movsw32_op) {
        out("writew(cpu->memory, cpu->segAddress[ES]+EDI, readw(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+ESI)); EDI+=cpu->df<<1; ESI+=cpu->df<<1; CYCLES(4);");
    } else {
        kpanic("gen0a5");
    }    
}

void OPCALL movsd16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL movsd16_op(struct CPU* cpu, struct Op* op);
void OPCALL movsd32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL movsd32_op(struct CPU* cpu, struct Op* op);
void gen2a5(struct Op* op) {
    if (op->func==movsd16_r_op) {
        out("movsd16_r(cpu, ");
    } else if (op->func==movsd16_op) {
        out("writed(cpu->memory, cpu->segAddress[ES]+DI, readd(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+SI)); DI+=cpu->df<<2; SI+=cpu->df<<2; CYCLES(4);");
    } else if (op->func==movsd32_r_op) {
        out("movsd32_r(cpu, ");
    } else if (op->func==movsd32_op) {
        out("writed(cpu->memory, cpu->segAddress[ES]+EDI, readd(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+ESI)); EDI+=cpu->df<<2; ESI+=cpu->df<<2; CYCLES(4);");
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
        out(tmp);
        out(", ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==cmpsb16_op) {
        out("cpu->dst.u8 = readb(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+SI); cpu->src.u8 = readb(cpu->memory, cpu->segAddress[ES]+DI); cpu->result.u8 = cpu->dst.u8 - cpu->src.u8; cpu->lazyFlags = FLAGS_SUB8; DI+=cpu->df; SI+=cpu->df; CYCLES(5);");        
    } else if (op->func==cmpsb32_r_op) {
        out("cmpsb32_r(cpu, ");
        out(tmp);
        out(", ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==cmpsb32_op) {
        out("cpu->dst.u8 = readb(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+ESI); cpu->src.u8 = readb(cpu->memory, cpu->segAddress[ES]+EDI); cpu->result.u8 = cpu->dst.u8 - cpu->src.u8; cpu->lazyFlags = FLAGS_SUB8; EDI+=cpu->df; ESI+=cpu->df; CYCLES(5);");
    } else {
        kpanic("gen0a6");
    }   
    currentLazyFlags = sFLAGS_SUB8;
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
        out(tmp);
        out(", ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==cmpsw16_op) {
        out("cpu->dst.u16 = readw(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+SI); cpu->src.u16 = readw(cpu->memory, cpu->segAddress[ES]+DI); cpu->result.u16 = cpu->dst.u16 - cpu->src.u16; cpu->lazyFlags = FLAGS_SUB16; DI+=cpu->df<<1; SI+=cpu->df<<1; CYCLES(5);");
    } else if (op->func==cmpsw32_r_op) {
        out("cmpsw32_r(cpu, ");
        out(tmp);
        out(", ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==cmpsw32_op) {
        out("cpu->dst.u16 = readw(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+ESI); cpu->src.u16 = readw(cpu->memory, cpu->segAddress[ES]+EDI); cpu->result.u16 = cpu->dst.u16 - cpu->src.u16; cpu->lazyFlags = FLAGS_SUB16; EDI+=cpu->df<<1; ESI+=cpu->df<<1; CYCLES(5);");
    } else {
        kpanic("gen0a7");
    }    
    currentLazyFlags = sFLAGS_SUB16;
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
        out(tmp);
        out(", ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==cmpsd16_op) {
        out("cpu->dst.u32 = readd(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+SI); cpu->src.u32 = readd(cpu->memory, cpu->segAddress[ES]+DI); cpu->result.u32 = cpu->dst.u32 - cpu->src.u32; cpu->lazyFlags = FLAGS_SUB32; DI+=cpu->df<<2; SI+=cpu->df<<2; CYCLES(5);");
    } else if (op->func==cmpsd32_r_op) {
        out("cmpsd32_r(cpu, ");
        out(tmp);
        out(", ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==cmpsd32_op) {
        out("cpu->dst.u32 = readd(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+ESI); cpu->src.u32 = readd(cpu->memory, cpu->segAddress[ES]+EDI); cpu->result.u32 = cpu->dst.u32 - cpu->src.u32; cpu->lazyFlags = FLAGS_SUB32; EDI+=cpu->df<<2; ESI+=cpu->df<<2; CYCLES(5);");
    } else {
        kpanic("gen2a7");
    }   
    currentLazyFlags = sFLAGS_SUB32;
}

void OPCALL stosb16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL stosb16_op(struct CPU* cpu, struct Op* op);
void OPCALL stosb32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL stosb32_op(struct CPU* cpu, struct Op* op);
void gen0aa(struct Op* op) {
    if (op->func==stosb16_r_op) {
        out("stosb16_r(cpu);");        
    } else if (op->func==stosb16_op) {
        out("writeb(cpu->memory, cpu->segAddress[ES]+DI, AL); DI+=cpu->df; CYCLES(3);");
    } else if (op->func==stosb32_r_op) {
        out("stosb32_r(cpu);");
    } else if (op->func==stosb32_op) {
        out("writeb(cpu->memory, cpu->segAddress[ES]+EDI, AL); EDI+=cpu->df; CYCLES(3);");
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
        out("writew(cpu->memory, cpu->segAddress[ES]+DI, AX); DI+=cpu->df<<1; CYCLES(3);");
    } else if (op->func==stosw32_r_op) {
        out("stosw32_r(cpu);");
    } else if (op->func==stosw32_op) {
        out("writew(cpu->memory, cpu->segAddress[ES]+EDI, AX); EDI+=cpu->df<<1; CYCLES(3);");
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
        out("writed(cpu->memory, cpu->segAddress[ES]+DI, EAX); DI+=cpu->df<<2; CYCLES(3);");
    } else if (op->func==stosd32_r_op) {
        out("stosd32_r(cpu);");
    } else if (op->func==stosd32_op) {
        out("writed(cpu->memory, cpu->segAddress[ES]+EDI, EAX); EDI+=cpu->df<<2; CYCLES(3);");
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
        out(getBase(op->base));
        out(");");
    } else if (op->func==lodsb16_op) {
        out("AL = readb(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+SI); SI+=cpu->df; CYCLES(2);");
    } else if (op->func==lodsb32_r_op) {
        out("lodsb32_r(cpu, ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==lodsb32_op) {
        out("AL = readb(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+ESI); ESI+=cpu->df; CYCLES(2);");
    } else {
        kpanic("gen0ac");
    }    
}

void OPCALL lodsw16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsw16_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsw32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsw32_op(struct CPU* cpu, struct Op* op);
void gen0ad(struct Op* op) {
    if (op->func==lodsw16_r_op) {
        out("lodsw16_r(cpu, ");        
        out(getBase(op->base));
        out(");");
    } else if (op->func==lodsw16_op) {
        out("AX = readw(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+SI); SI+=cpu->df<<1; CYCLES(2);");
    } else if (op->func==lodsw32_r_op) {
        out("lodsw32_r(cpu, ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==lodsw32_op) {
        out("AX = readw(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+ESI); ESI+=cpu->df<<1; CYCLES(2);");
    } else {
        kpanic("gen0ad");
    }    
}

void OPCALL lodsd16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsd16_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsd32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL lodsd32_op(struct CPU* cpu, struct Op* op);
void gen2ad(struct Op* op) {
    if (op->func==lodsd16_r_op) {
        out("lodsd16_r(cpu, ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==lodsd16_op) {
        out("EAX = readd(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+SI); SI+=cpu->df<<2; CYCLES(2);");
    } else if (op->func==lodsd32_r_op) {
        out("lodsd32_r(cpu, ");
        out(getBase(op->base));
        out(");");
    } else if (op->func==lodsd32_op) {
        out("EAX = readd(cpu->memory, cpu->segAddress[");
        out(getBase(op->base));
        out("]+ESI); ESI+=cpu->df<<2; CYCLES(2);");
    } else {
        kpanic("gen2ad");
    }    
}

void OPCALL scasb16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasb16_op(struct CPU* cpu, struct Op* op);
void OPCALL scasb32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasb32_op(struct CPU* cpu, struct Op* op);
void gen0ae(struct Op* op) {
    if (op->func==scasb16_r_op) {
        out("scasb16_r(cpu, ");        
        out(getBase(op->data1));
        out(");");
    } else if (op->func==scasb16_op) {
        out("cpu->dst.u8 = AL; cpu->src.u8 = readb(cpu->memory,  cpu->segAddress[ES]+DI); cpu->result.u8 = cpu->dst.u8 - cpu->src.u8; cpu->lazyFlags = FLAGS_SUB8; DI+=cpu->df; CYCLES(4);");
    } else if (op->func==scasb32_r_op) {
        out("scasb32_r(cpu, ");
        out(getBase(op->data1));
        out(");");
    } else if (op->func==scasb32_op) {
        out("cpu->dst.u8 = AL; cpu->src.u8 = readb(cpu->memory,  cpu->segAddress[ES]+EDI); cpu->result.u8 = cpu->dst.u8 - cpu->src.u8; cpu->lazyFlags = FLAGS_SUB8; EDI+=cpu->df; CYCLES(4);");
    } else {
        kpanic("gen0a4");
    }   
    currentLazyFlags = sFLAGS_SUB8;
}

void OPCALL scasw16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasw16_op(struct CPU* cpu, struct Op* op);
void OPCALL scasw32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasw32_op(struct CPU* cpu, struct Op* op);
void gen0af(struct Op* op) {
    if (op->func==scasw16_r_op) {
        out("scasw16_r(cpu, ");        
        out(getBase(op->data1));
        out(");");
    } else if (op->func==scasw16_op) {
        out("cpu->dst.u16 = AX; cpu->src.u16 = readw(cpu->memory,  cpu->segAddress[ES]+DI); cpu->result.u16 = cpu->dst.u16 - cpu->src.u16; cpu->lazyFlags = FLAGS_SUB16; DI+=cpu->df<<1; CYCLES(4);");
    } else if (op->func==scasw32_r_op) {
        out("scasw32_r(cpu, ");
        out(getBase(op->data1));
        out(");");
    } else if (op->func==scasw32_op) {
        out("cpu->dst.u16 = AX; cpu->src.u16 = readw(cpu->memory,  cpu->segAddress[ES]+EDI); cpu->result.u16 = cpu->dst.u16 - cpu->src.u16; cpu->lazyFlags = FLAGS_SUB16; EDI+=cpu->df<<1; CYCLES(4);");
    } else {
        kpanic("gen0a5");
    }   
    currentLazyFlags = sFLAGS_SUB16;
}

void OPCALL scasd16_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasd16_op(struct CPU* cpu, struct Op* op);
void OPCALL scasd32_r_op(struct CPU* cpu, struct Op* op);
void OPCALL scasd32_op(struct CPU* cpu, struct Op* op);
void gen2af(struct Op* op) {
    if (op->func==scasd16_r_op) {
        out(getBase(op->data1));
        out(");");
    } else if (op->func==scasd16_op) {
        out("cpu->dst.u32 = EAX; cpu->src.u32 = readd(cpu->memory,  cpu->segAddress[ES]+DI); cpu->result.u32 = cpu->dst.u32 - cpu->src.u32; cpu->lazyFlags = FLAGS_SUB32; DI+=cpu->df<<2; CYCLES(4);");
    } else if (op->func==scasd32_r_op) {
        out(getBase(op->data1));
        out(");");
    } else if (op->func==scasd32_op) {
        out("cpu->dst.u32 = EAX; cpu->src.u32 = readd(cpu->memory,  cpu->segAddress[ES]+EDI); cpu->result.u32 = cpu->dst.u32 - cpu->src.u32; cpu->lazyFlags = FLAGS_SUB32; EDI+=cpu->df<<2; CYCLES(4);");
    } else {
        kpanic("gen2a5");
    }   
    currentLazyFlags = sFLAGS_SUB32;
}


void gen0a8(struct Op* op) {
    genArithR("&", "FLAGS_TEST8", "8", r8(0), op->data1, 0, 0, "1");
    currentLazyFlags = sFLAGS_TEST8;
}

void gen0a9(struct Op* op) {
    genArithR("&", "FLAGS_TEST16", "16", r16(0), op->data1, 0, 0, "1");
    currentLazyFlags = sFLAGS_TEST16;
}

void gen2a9(struct Op* op) {
    genArithR("&", "FLAGS_TEST32", "32", r32(0), op->data1, 0, 0, "1");
    currentLazyFlags = sFLAGS_TEST32;
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
        if (op->data1 == 1) {
            out(") | ");
            out(getFlag(CF));
            out("; CYCLES(8);");
        } else {
            out(") | (");
            out(r8(op->r1));
            out(" >> (9 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (");
            out(value);
            out(" - 1)); CYCLES(8);");
        }
    } else if (op->func == rcl8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << ");
        out(value);
        if (op->data1 == 1) {
            out(") | ");
            out(getFlag(CF));
            out("; CYCLES(10);");
        } else {
            out(") | (");
            out(r8(op->r1));
            out(" >> (9 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (");
            out(value);
            out(" - 1)); CYCLES(10);");
        }
    } else if (op->func == rcl8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << ");
        out(value);
        if (op->data1 == 1) {
            out(") | ");
            out(getFlag(CF));
            out("; CYCLES(10);");
        } else {
            out(") | (");
            out(r8(op->r1));
            out(" >> (9 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (");
            out(value);
            out(" - 1)); CYCLES(10);");
        }
    } else if (op->func == rcr8_reg_noflags) {
        out(r8(op->r1));
        out(" = (");
        out(r8(op->r1));
        out(" >> ");
        out(value);
        if (op->data1 == 1) {
            out(") | (");
            out(getFlag(CF));
            out(" << 7); CYCLES(8);");
        } else {
            out(") | (");
            out(r8(op->r1));
            out(" << (9 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (8 - ");
            out(value);
            out(")); CYCLES(8);");
        }
    } else if (op->func == rcr8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> ");
        out(value);
        if (op->data1 == 1) {
            out(") | (");
            out(getFlag(CF));
            out(" << 7); CYCLES(10);");
        } else {
            out(") | (");
            out(r8(op->r1));
            out(" << (9 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (8 - ");
            out(value);
            out(")); CYCLES(10);");
        }       
    } else if (op->func == rcr8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> ");
        out(value);
        if (op->data1 == 1) {
            out(") | (");
            out(getFlag(CF));
            out(" << 7); CYCLES(10);");
        } else {
            out(") | (");
            out(r8(op->r1));
            out(" << (9 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (8 - ");
            out(value);
            out(")); CYCLES(10);");
        } 
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
        if (op->data1 == 1) {
            out(") | ");
            out(getFlag(CF));
            out("; CYCLES(8);");
        } else {
            out(") | (");
            out(r16(op->r1));
            out(" >> (17 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (");
            out(value);
            out(" - 1)); CYCLES(8);");
        }
    } else if (op->func == rcl16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << ");
        out(value);
        if (op->data1 == 1) {
            out(") | ");
            out(getFlag(CF));
            out("; CYCLES(10);");
        } else {
            out(") | (");
            out(r16(op->r1));
            out(" >> (17 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (");
            out(value);
            out(" - 1)); CYCLES(10);");
        }
    } else if (op->func == rcl16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << ");
        out(value);
        if (op->data1 == 1) {
            out(") | ");
            out(getFlag(CF));
            out("; CYCLES(10);");
        } else {
            out(") | (");
            out(r16(op->r1));
            out(" >> (17 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (");
            out(value);
            out(" - 1)); CYCLES(10);");
        }
    } else if (op->func == rcr16_reg_noflags) {
        out(r16(op->r1));
        out(" = (");
        out(r16(op->r1));
        out(" >> ");
        out(value);
        if (op->data1 == 1) {
            out(") | (");
            out(getFlag(CF));
            out(" << 15); CYCLES(8);");
        } else {
            out(") | (");
            out(r16(op->r1));
            out(" << (17 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (16 - ");
            out(value);
            out(")); CYCLES(8);");
        }
    } else if (op->func == rcr16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> ");
        out(value);
        if (op->data1 == 1) {
            out(") | (");
            out(getFlag(CF));
            out(" << 15); CYCLES(10);");
        } else {
            out(") | (");
            out(r16(op->r1));
            out(" << (17 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (16 - ");
            out(value);
            out(")); CYCLES(10);");
        }       
    } else if (op->func == rcr16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> ");
        out(value);
        if (op->data1 == 1) {
            out(") | (");
            out(getFlag(CF));
            out(" << 15); CYCLES(10);");
        } else {
            out(") | (");
            out(r16(op->r1));
            out(" << (17 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (16 - ");
            out(value);
            out(")); CYCLES(10);");
        }
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
        if (op->data1==1) {
            out(") | ");
            out(getFlag(CF));
            out("; CYCLES(8);");
        } else {
            out(") | (");
            out(r32(op->r1));
            out(" >> (33 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (");
            out(value);
            out(" - 1))); CYCLES(8);");
        }
    } else if (op->func == rcl32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << ");
        out(value);
        if (op->data1==1) {
            out(") | ");
            out(getFlag(CF));
            out("; CYCLES(10);");
        } else {
            out(") | (");
            out(r32(op->r1));
            out(" >> (33 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (");
            out(value);
            out(" - 1))); CYCLES(10);");
        }
    } else if (op->func == rcl32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << ");
        out(value);
        if (op->data1==1) {
            out(") | ");
            out(getFlag(CF));
            out("; CYCLES(10);");
        } else {
            out(") | (");
            out(r32(op->r1));
            out(" >> (33 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (");
            out(value);
            out(" - 1))); CYCLES(10);");
        }
    } else if (op->func == rcr32_reg_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" >> ");
        out(value);
        if (op->data1==1) {
            out(") | (");
            out(getFlag(CF));
            out(" << 31); CYCLES(8);");
        } else {
            out(") | (");
            out(r32(op->r1));
            out(" << (33 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (32 - ");
            out(value);
            out(")); CYCLES(8);");
        }
    } else if (op->func == rcr32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> ");
        out(value);
        if (op->data1==1) {
            out(") | (");
            out(getFlag(CF));
            out(" << 31); CYCLES(10);");
        } else {
            out(") | (");
            out(r32(op->r1));
            out(" << (33 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (32 - ");
            out(value);
            out(")); CYCLES(10);");
        }      
    } else if (op->func == rcr32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> ");
        out(value);
        if (op->data1==1) {
            out(") | (");
            out(getFlag(CF));
            out(" << 31); CYCLES(10);");
        } else {
            out(") | (");
            out(r32(op->r1));
            out(" << (33 - ");
            out(value);
            out(")) | (");
            out(getFlag(CF));
            out(" << (32 - ");
            out(value);
            out(")); CYCLES(10);");
        }
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
        out(" >> (9 - (CL & 7))) | (");
        out(getFlag(CF));
        out(" << ((CL & 7) - 1)); CYCLES(8);");
    } else if (op->func == rcl8cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << (CL & 7)) | (");
        out(r8(op->r1));
        out(" >> (9 - (CL & 7))) | (");
        out(getFlag(CF));
        out(" << ((CL & 7) - 1))); CYCLES(10);");
    } else if (op->func == rcl8cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 << (CL & 7)) | (");
        out(r8(op->r1));
        out(" >> (9 - (CL & 7))) | (");
        out(getFlag(CF));
        out(" << ((CL & 7) - 1))); CYCLES(10);");
    } else if (op->func == rcr8cl_reg_noflags) {
        out(r8(op->r1));
        out(" = (");
        out(r8(op->r1));
        out(" >> (CL & 7)) | (");
        out(r8(op->r1));
        out(" << (9 - (CL & 7))) | (");
        out(getFlag(CF));
        out(" << (8 - (CL & 7))); CYCLES(8);");
    } else if (op->func == rcr8cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> (CL & 7)) | (");
        out(r8(op->r1));
        out(" << (9 - (CL & 7))) | (");
        out(getFlag(CF));
        out(" << (8 - (CL & 7)))); CYCLES(10);");        
    } else if (op->func == rcr8cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp8 = readb(cpu->memory, eaa); writeb(cpu->memory, eaa, (tmp8 >> (CL & 7)) | (");
        out(r8(op->r1));
        out(" << (9 - (CL & 7))) | (");
        out(getFlag(CF));
        out(" << (8 - (CL & 7)))); CYCLES(10);"); 
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
        out(" >> (17 - (CL & 15))) | (");
        out(getFlag(CF));
        out(" << ((CL & 15) - 1)); CYCLES(8);");
    } else if (op->func == rcl16cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << (CL & 15)) | (");
        out(r16(op->r1));
        out(" >> (17 - (CL & 15))) | (");
        out(getFlag(CF));
        out(" << ((CL & 15) - 1))); CYCLES(10);");
    } else if (op->func == rcl16cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 << (CL & 15)) | (");
        out(r16(op->r1));
        out(" >> (17 - (CL & 15))) | (");
        out(getFlag(CF));
        out(" << ((CL & 15) - 1))); CYCLES(10);");
    } else if (op->func == rcr16cl_reg_noflags) {
        out(r16(op->r1));
        out(" = (");
        out(r16(op->r1));
        out(" >> (CL & 15)) | (");
        out(r16(op->r1));
        out(" << (17 - (CL & 15))) | (");
        out(getFlag(CF));
        out(" << (16 - (CL & 15))); CYCLES(8);");
    } else if (op->func == rcr16cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> (CL & 15)) | (");
        out(r16(op->r1));
        out(" << (17 - (CL & 15))) | (");
        out(getFlag(CF));
        out(" << (16 - (CL & 15)))); CYCLES(10);");        
    } else if (op->func == rcr16cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp16 = readw(cpu->memory, eaa); writew(cpu->memory, eaa, (tmp16 >> (CL & 15)) | (");
        out(r16(op->r1));
        out(" << (17 - (CL & 15))) | (");
        out(getFlag(CF));
        out(" << (16 - (CL & 15)))); CYCLES(10);"); 
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
        out(" << (CL & 0x1F)) | (");
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
        out(" >> (33 - (CL & 31))) | (");
        out(getFlag(CF));
        out(" << ((CL & 31) - 1)); CYCLES(8);");
    } else if (op->func == rcl32cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << (CL & 31)) | (");
        out(r32(op->r1));
        out(" >> (33 - (CL & 31))) | (");
        out(getFlag(CF));
        out(" << ((CL & 31) - 1))); CYCLES(10);");
    } else if (op->func == rcl32cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 << (CL & 31)) | (");
        out(r32(op->r1));
        out(" >> (33 - (CL & 31))) | (");
        out(getFlag(CF));
        out(" << ((CL & 31) - 1))); CYCLES(10);");
    } else if (op->func == rcr32cl_reg_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" >> (CL & 31)) | (");
        out(r32(op->r1));
        out(" << (33 - (CL & 31))) | (");
        out(getFlag(CF));
        out(" << (32 - (CL & 31))); CYCLES(8);");
    } else if (op->func == rcr32cl_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> (CL & 31)) | (");
        out(r32(op->r1));
        out(" << (33 - (CL & 31))) | (");
        out(getFlag(CF));
        out(" << (32 - (CL & 31)))); CYCLES(10);");        
    } else if (op->func == rcr32cl_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, (tmp32 >> (CL & 31)) | (");
        out(r32(op->r1));
        out(" << (33 - (CL & 31))) | (");
        out(getFlag(CF));
        out(" << (32 - (CL & 31)))); CYCLES(10);"); 
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

void gen0c8(struct Op* op) {
    char tmp[16];

    out("cpu_enter16(cpu, ");
    itoa(op->data1, tmp, 10);
    out(tmp);
    out(", ");
    itoa(op->r1, tmp, 10);
    out(tmp);
    out("); CYCLES(15);");
}

void gen2c8(struct Op* op) {
    char tmp[16];

    out("cpu_enter32(cpu, ");
    itoa(op->data1, tmp, 10);
    out(tmp);
    out(", ");
    itoa(op->r1, tmp, 10);
    out(tmp);
    out("); CYCLES(15);");
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
    currentLazyFlags = sFLAGS_NONE;
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
    out("if (");
    out(getFlag(CF));
    out(") AL = 0xFF; else AL = 0; CYCLES(2);");
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

void OPCALL FADD_ST0_STj(struct CPU* cpu, struct Op* op);
void OPCALL FMUL_ST0_STj(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_STi(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_STi_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FSUB_ST0_STj(struct CPU* cpu, struct Op* op);
void OPCALL FSUBR_ST0_STj(struct CPU* cpu, struct Op* op);
void OPCALL FDIV_ST0_STj(struct CPU* cpu, struct Op* op);
void OPCALL FDIVR_ST0_STj(struct CPU* cpu, struct Op* op);
void OPCALL FADD_SINGLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FMUL_SINGLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_SINGLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_SINGLE_REAL_32_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FSUB_SINGLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FSUBR_SINGLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FDIV_SINGLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FDIVR_SINGLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FADD_SINGLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FMUL_SINGLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_SINGLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_SINGLE_REAL_16_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FSUB_SINGLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FSUBR_SINGLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FDIV_SINGLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FDIVR_SINGLE_REAL_16(struct CPU* cpu, struct Op* op);
void gen0d8(struct Op* op) {
    char tmp[16];
    itoa(op->r1, tmp, 10);
    if (op->func == FADD_ST0_STj) {
        out("cpu->fpu.regs[cpu->fpu.top].d += cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d; CYCLES(1);");
    } else if (op->func == FMUL_ST0_STj) {
        out("cpu->fpu.regs[cpu->fpu.top].d *= cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d; CYCLES(1);");
    } else if (op->func == FCOM_STi) {
        out("FPU_FCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out("));CYCLES(1);");
    } else if (op->func == FCOM_STi_Pop) {
        out("FPU_FCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out("));FPU_FPOP(cpu);CYCLES(1);");
    } else if (op->func == FSUB_ST0_STj) {
        out("cpu->fpu.regs[cpu->fpu.top].d -= cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d; CYCLES(1);");
    } else if (op->func == FSUBR_ST0_STj) {
        out("cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d - cpu->fpu.regs[cpu->fpu.top].d; CYCLES(1);");
    } else if (op->func == FDIV_ST0_STj) {
        out("cpu->fpu.regs[cpu->fpu.top].d /= cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d; CYCLES(39);");
    } else if (op->func == FDIVR_ST0_STj) {
        out("cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d / cpu->fpu.regs[cpu->fpu.top].d; CYCLES(39);");
    } else if (op->func == FADD_SINGLE_REAL_32) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d += f2i.f; CYCLES(1);");
    } else if (op->func == FMUL_SINGLE_REAL_32) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d *= f2i.f; CYCLES(1);");
    } else if (op->func == FCOM_SINGLE_REAL_32) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[8].d = f2i.f; FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); CYCLES(1);");
    } else if (op->func == FCOM_SINGLE_REAL_32_Pop) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[8].d = f2i.f; FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FSUB_SINGLE_REAL_32) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d -= f2i.f; CYCLES(1);");
    } else if (op->func == FSUBR_SINGLE_REAL_32) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d = f2i.f - cpu->fpu.regs[cpu->fpu.top].d; CYCLES(39);");
    } else if (op->func == FDIV_SINGLE_REAL_32) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d /= f2i.f; CYCLES(1);");
    } else if (op->func == FDIVR_SINGLE_REAL_32) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d = f2i.f / cpu->fpu.regs[cpu->fpu.top].d; CYCLES(39);");
    } else if (op->func == FADD_SINGLE_REAL_16) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d += f2i.f; CYCLES(1);");
    } else if (op->func == FMUL_SINGLE_REAL_16) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d *= f2i.f; CYCLES(1);");
    } else if (op->func == FCOM_SINGLE_REAL_16) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[8].d = f2i.f; FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); CYCLES(1);");
    } else if (op->func == FCOM_SINGLE_REAL_16_Pop) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[8].d = f2i.f; FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FSUB_SINGLE_REAL_16) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d -= f2i.f; CYCLES(1);");
    } else if (op->func == FSUBR_SINGLE_REAL_16) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d = f2i.f - cpu->fpu.regs[cpu->fpu.top].d; CYCLES(39);");
    } else if (op->func == FDIV_SINGLE_REAL_16) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d /= f2i.f; CYCLES(1);");
    } else if (op->func == FDIVR_SINGLE_REAL_16) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d = f2i.f / cpu->fpu.regs[cpu->fpu.top].d; CYCLES(39);");
    } else {
        kpanic("gen0d8");
    }
}

void OPCALL FLD_STi(struct CPU* cpu, struct Op* op);
void OPCALL FXCH_STi(struct CPU* cpu, struct Op* op);
void OPCALL FNOP(struct CPU* cpu, struct Op* op);
void OPCALL FST_STi_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FCHS(struct CPU* cpu, struct Op* op);
void OPCALL FABS(struct CPU* cpu, struct Op* op);
void OPCALL FTST(struct CPU* cpu, struct Op* op);
void OPCALL FXAM(struct CPU* cpu, struct Op* op);
void OPCALL FLD1(struct CPU* cpu, struct Op* op);
void OPCALL FLDL2T(struct CPU* cpu, struct Op* op);
void OPCALL FLDL2E(struct CPU* cpu, struct Op* op);
void OPCALL FLDPI(struct CPU* cpu, struct Op* op);
void OPCALL FLDLG2(struct CPU* cpu, struct Op* op);
void OPCALL FLDLN2(struct CPU* cpu, struct Op* op);
void OPCALL FLDZ(struct CPU* cpu, struct Op* op);
void OPCALL F2XM1(struct CPU* cpu, struct Op* op);
void OPCALL FYL2X(struct CPU* cpu, struct Op* op);
void OPCALL FPTAN(struct CPU* cpu, struct Op* op);
void OPCALL FPATAN(struct CPU* cpu, struct Op* op);
void OPCALL FXTRACT(struct CPU* cpu, struct Op* op);
void OPCALL FPREM_nearest(struct CPU* cpu, struct Op* op);
void OPCALL FDECSTP(struct CPU* cpu, struct Op* op);
void OPCALL FINCSTP(struct CPU* cpu, struct Op* op);
void OPCALL FPREM(struct CPU* cpu, struct Op* op);
void OPCALL FYL2XP1(struct CPU* cpu, struct Op* op);
void OPCALL FSQRT(struct CPU* cpu, struct Op* op);
void OPCALL FSINCOS(struct CPU* cpu, struct Op* op);
void OPCALL FRNDINT(struct CPU* cpu, struct Op* op);
void OPCALL FSCALE(struct CPU* cpu, struct Op* op);
void OPCALL FSIN(struct CPU* cpu, struct Op* op);
void OPCALL FCOS(struct CPU* cpu, struct Op* op);
void OPCALL FLD_SINGLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FST_SINGLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FST_SINGLE_REAL_16_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FLDENV_16(struct CPU* cpu, struct Op* op);
void OPCALL FLDCW_16(struct CPU* cpu, struct Op* op);
void OPCALL FNSTENV_16(struct CPU* cpu, struct Op* op);
void OPCALL FNSTCW_16(struct CPU* cpu, struct Op* op);
void OPCALL FLD_SINGLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FST_SINGLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FST_SINGLE_REAL_32_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FLDENV_32(struct CPU* cpu, struct Op* op);
void OPCALL FLDCW_32(struct CPU* cpu, struct Op* op);
void OPCALL FNSTENV_32(struct CPU* cpu, struct Op* op);
void OPCALL FNSTCW_32(struct CPU* cpu, struct Op* op);
void gen0d9(struct Op* op) {
    char tmp[16];
    itoa(op->r1, tmp, 10);

    if (op->func == FLD_STi) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.top = (cpu->fpu.top - 1) & 7; cpu->fpu.tags[cpu->fpu.top] = cpu->fpu.tags[tmp32]; cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[tmp32].d; CYCLES(1);");
    } else if (op->func == FXCH_STi) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); tmp32_2 = cpu->fpu.tags[tmp32]; tmpd = cpu->fpu.regs[tmp32].d; cpu->fpu.tags[tmp32] = cpu->fpu.tags[cpu->fpu.top]; cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d; cpu->fpu.tags[cpu->fpu.top] = tmp32_2; cpu->fpu.regs[cpu->fpu.top].d = tmpd; CYCLES(1);");
    } else if (op->func == FNOP) {
        out("CYCLES(1);");
    } else if (op->func == FST_STi_Pop) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[tmp32] = cpu->fpu.tags[cpu->fpu.top]; cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d;FPU_FPOP(cpu);CYCLES(1);");
    } else if (op->func == FCHS) {
        out("cpu->fpu.regs[cpu->fpu.top].d = -1.0 * (cpu->fpu.regs[cpu->fpu.top].d);CYCLES(1);");
    } else if (op->func == FABS) {
        // don't inline because of calling convention of fabs
        out("FPU_FABS(&cpu->fpu);CYCLES(1);");
    } else if (op->func == FTST) {
        out("cpu->fpu.regs[8].d = 0.0;FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8);CYCLES(1);");
    } else if (op->func == FXAM) {
        out("FPU_FXAM(&cpu->fpu); CYCLES(21);");
    } else if (op->func == FLD1) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = 1.0; CYCLES(2);");
    } else if (op->func == FLDL2T) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = 3.3219280948873623; CYCLES(3);");
    } else if (op->func == FLDL2E) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = 1.4426950408889634; CYCLES(3);");
    } else if (op->func == FLDPI) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = 3.14159265358979323846; CYCLES(3);");
    } else if (op->func == FLDLG2) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = 0.3010299956639812; CYCLES(3);");
    } else if (op->func == FLDLN2) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = 0.69314718055994531; CYCLES(3);");
    } else if (op->func == FLDZ) {
        out("cpu->fpu.top = (cpu->fpu.top - 1) & 7; cpu->fpu.regs[cpu->fpu.top].d = 0.0; cpu->fpu.tags[cpu->fpu.top] = TAG_Zero; CYCLES(2);");
    } else if (op->func == F2XM1) {
        out("FPU_F2XM1(&cpu->fpu); CYCLES(13);");
    } else if (op->func == FYL2X) {
        out("FPU_FYL2X(&cpu->fpu); CYCLES(22);");
    } else if (op->func == FPTAN) {
        out("FPU_FPTAN(&cpu->fpu); CYCLES(17);");
    } else if (op->func == FPATAN) {
        out("FPU_FPATAN(&cpu->fpu); CYCLES(17);");
    } else if (op->func == FXTRACT) {
        out("FPU_FXTRACT(&cpu->fpu); CYCLES(13);");
    } else if (op->func == FPREM_nearest) {
        out("FPU_FPREM1(&cpu->fpu); CYCLES(17);");
    } else if (op->func == FDECSTP) {
        out("cpu->fpu.top = (cpu->fpu.top - 1) & 7; CYCLES(1);");
    } else if (op->func == FINCSTP) {
        out("cpu->fpu.top = (cpu->fpu.top + 1) & 7; CYCLES(1);");
    } else if (op->func == FPREM) {
        out("FPU_FPREM(&cpu->fpu); CYCLES(16);");
    } else if (op->func == FYL2XP1) {
        out("FPU_FYL2XP1(&cpu->fpu);CYCLES(22);");
    } else if (op->func == FSQRT) {
        out("FPU_FSQRT(&cpu->fpu);CYCLES(70);");
    } else if (op->func == FSINCOS) {
        out("FPU_FSINCOS(&cpu->fpu); CYCLES(17);");
    } else if (op->func == FRNDINT) {
        out("FPU_FRNDINT(&cpu->fpu); CYCLES(9);");
    } else if (op->func == FSCALE) {
        out("FPU_FSCALE(&cpu->fpu); CYCLES(20);");
    } else if (op->func == FSIN) {
        out("FPU_FSIN(&cpu->fpu); CYCLES(16);");
    } else if (op->func == FCOS) {
        out("FPU_FCOS(&cpu->fpu); CYCLES(16);");
    } else if (op->func == FLD_SINGLE_REAL_16) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa16(op));
        out("); FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = f2i.f; CYCLES(1);");	    
    } else if (op->func == FST_SINGLE_REAL_16) {
        out("f2i.f = (float)cpu->fpu.regs[cpu->fpu.top].d; writed(cpu->memory, ");
        out(getEaa16(op));
        out(", f2i.i); CYCLES(2);");
    } else if (op->func == FST_SINGLE_REAL_16_Pop) {
        out("f2i.f = (float)cpu->fpu.regs[cpu->fpu.top].d; writed(cpu->memory, ");
        out(getEaa16(op));
        out(", f2i.i); FPU_FPOP(cpu); CYCLES(2);");
    } else if (op->func == FLDENV_16) {
        out("FPU_FLDENV(cpu, "); 
        out(getEaa16(op));
        out("); CYCLES(32);");
    } else if (op->func == FLDCW_16) {
        out("FPU_SetCW(&cpu->fpu, readw(cpu->memory, ");
        out(getEaa16(op));
        out(")); CYCLES(7);");
    } else if (op->func == FNSTENV_16) {
        out("FPU_FSTENV(cpu, ");
        out(getEaa16(op));
        out("); CYCLES(48);");
    } else if (op->func == FNSTCW_16) {
        out("writew(cpu->memory, ");
        out(getEaa16(op));
        out(", cpu->fpu.cw);CYCLES(2);");
    } else if (op->func == FLD_SINGLE_REAL_32) {
        out("f2i.i = readd(cpu->memory, ");
        out(getEaa32(op));
        out("); FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = f2i.f; CYCLES(1);");	    
    } else if (op->func == FST_SINGLE_REAL_32) {
        out("f2i.f = (float)cpu->fpu.regs[cpu->fpu.top].d; writed(cpu->memory, ");
        out(getEaa32(op));
        out(", f2i.i); CYCLES(2);");
    } else if (op->func == FST_SINGLE_REAL_32_Pop) {
        out("f2i.f = (float)cpu->fpu.regs[cpu->fpu.top].d; writed(cpu->memory, ");
        out(getEaa32(op));
        out(", f2i.i); FPU_FPOP(cpu); CYCLES(2);");
    } else if (op->func == FLDENV_32) {
        out("FPU_FLDENV(cpu, "); 
        out(getEaa32(op));
        out("); CYCLES(32);");
    } else if (op->func == FLDCW_32) {
        out("FPU_SetCW(&cpu->fpu, readw(cpu->memory, ");
        out(getEaa32(op));
        out(")); CYCLES(7);");
    } else if (op->func == FNSTENV_32) {
        out("FPU_FSTENV(cpu, ");
        out(getEaa32(op));
        out("); CYCLES(48);");
    } else if (op->func == FNSTCW_32) {
        out("writew(cpu->memory, ");
        out(getEaa32(op));
        out(", cpu->fpu.cw);CYCLES(2);");
    } else {
        kpanic("gen0d9");
    }
}

void OPCALL FCMOV_ST0_STj_CF(struct CPU* cpu, struct Op* op);
void OPCALL FCMOV_ST0_STj_ZF(struct CPU* cpu, struct Op* op);
void OPCALL FCMOV_ST0_STj_CF_OR_ZF(struct CPU* cpu, struct Op* op);
void OPCALL FCMOV_ST0_STj_PF(struct CPU* cpu, struct Op* op);
void OPCALL FUCOMPP(struct CPU* cpu, struct Op* op);
void OPCALL FIADD_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FIMUL_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FICOM_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FICOM_DWORD_INTEGER_16_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FISUB_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FISUBR_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FIDIV_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FIDIVR_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FIADD_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FIMUL_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FICOM_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FICOM_DWORD_INTEGER_32_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FISUB_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FISUBR_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FIDIV_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FIDIVR_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void gen0da(struct Op* op) {
    char tmp[16];
    itoa(op->r1, tmp, 10);

    if (op->func == FCMOV_ST0_STj_CF) {
        out("if (");
        out(getFlag(CF));
        out(") {tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[cpu->fpu.top] = cpu->fpu.tags[tmp32]; cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[tmp32].d;} CYCLES(2);");
    } else if (op->func == FCMOV_ST0_STj_ZF) {
        out("if (");
        out(getFlag(ZF));
        out(") {tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[cpu->fpu.top] = cpu->fpu.tags[tmp32]; cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[tmp32].d;} CYCLES(2);");
    } else if (op->func == FCMOV_ST0_STj_CF_OR_ZF) {
        out("if (");
        out(getFlag(CF));
        out(" || ");
        out(getFlag(ZF));
        out(") {tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[cpu->fpu.top] = cpu->fpu.tags[tmp32]; cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[tmp32].d;} CYCLES(2);");
    } else if (op->func == FCMOV_ST0_STj_PF) {
        out("if (");
        out(getFlag(PF));
        out(") {tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[cpu->fpu.top] = cpu->fpu.tags[tmp32]; cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[tmp32].d;} CYCLES(2);");
    } else if (op->func == FUCOMPP) {
        out("FPU_FUCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, 1)); FPU_FPOP(cpu); FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FIADD_DWORD_INTEGER_16) {
        out("cpu->fpu.regs[cpu->fpu.top].d += (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(4);");
    } else if (op->func == FIMUL_DWORD_INTEGER_16) {
        out("cpu->fpu.regs[cpu->fpu.top].d *= (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(4);");
    } else if (op->func == FICOM_DWORD_INTEGER_16) {
        out("cpu->fpu.regs[8].d = (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out("); FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); CYCLES(4);");
    } else if (op->func == FICOM_DWORD_INTEGER_16_Pop) {
        out("cpu->fpu.regs[8].d = (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out("); FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); FPU_FPOP(cpu); CYCLES(4);");
    } else if (op->func == FISUB_DWORD_INTEGER_16) {
        out("cpu->fpu.regs[cpu->fpu.top].d -= (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(4);");
    } else if (op->func == FISUBR_DWORD_INTEGER_16) {
        out("cpu->fpu.regs[cpu->fpu.top].d = (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out(") - cpu->fpu.regs[cpu->fpu.top].d; CYCLES(4);");
    } else if (op->func == FIDIV_DWORD_INTEGER_16) {
        out("cpu->fpu.regs[cpu->fpu.top].d /= (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(42);");
    } else if (op->func == FIDIVR_DWORD_INTEGER_16) {
        out("cpu->fpu.regs[cpu->fpu.top].d = (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out(") / cpu->fpu.regs[cpu->fpu.top].d; CYCLES(42);");
    } else if (op->func == FIADD_DWORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d += (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(4);");
    } else if (op->func == FIMUL_DWORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d *= (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(4);");
    } else if (op->func == FICOM_DWORD_INTEGER_32) {
        out("cpu->fpu.regs[8].d = (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out("); FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); CYCLES(4);");
    } else if (op->func == FICOM_DWORD_INTEGER_32_Pop) {
        out("cpu->fpu.regs[8].d = (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out("); FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); FPU_FPOP(cpu); CYCLES(4);");
    } else if (op->func == FISUB_DWORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d -= (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(4);");
    } else if (op->func == FISUBR_DWORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d = (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out(") - cpu->fpu.regs[cpu->fpu.top].d; CYCLES(4);");
    } else if (op->func == FIDIV_DWORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d /= (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(42);");
    } else if (op->func == FIDIVR_DWORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d = (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out(") / cpu->fpu.regs[cpu->fpu.top].d; CYCLES(42);");
    } else {
        kpanic("gen0da");
    }
}

void OPCALL FCMOV_ST0_STj_NCF(struct CPU* cpu, struct Op* op);
void OPCALL FCMOV_ST0_STj_NZF(struct CPU* cpu, struct Op* op);
void OPCALL FCMOV_ST0_STj_NCF_AND_NZF(struct CPU* cpu, struct Op* op);
void OPCALL FCMOV_ST0_STj_NPF(struct CPU* cpu, struct Op* op);
void OPCALL FNCLEX(struct CPU* cpu, struct Op* op);
void OPCALL FNINIT(struct CPU* cpu, struct Op* op);
void OPCALL FUCOMI_ST0_STj(struct CPU* cpu, struct Op* op);
void OPCALL FCOMI_ST0_STj_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FILD_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FISTTP32_16(struct CPU* cpu, struct Op* op);
void OPCALL FIST_DWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FIST_DWORD_INTEGER_16_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FLD_EXTENDED_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FSTP_EXTENDED_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FILD_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FISTTP32_32(struct CPU* cpu, struct Op* op);
void OPCALL FIST_DWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FIST_DWORD_INTEGER_32_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FLD_EXTENDED_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FSTP_EXTENDED_REAL_32(struct CPU* cpu, struct Op* op);
void gen0db(struct Op* op) {
    char tmp[16];
    itoa(op->r1, tmp, 10);

    if (op->func == FCMOV_ST0_STj_NCF) {
        out("if (!");
        out(getFlag(CF));
        out(") {tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[cpu->fpu.top] = cpu->fpu.tags[tmp32]; cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[tmp32].d;} CYCLES(2);");
    } else if (op->func == FCMOV_ST0_STj_NZF) {
        out("if (!");
        out(getFlag(ZF));
        out(") {tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[cpu->fpu.top] = cpu->fpu.tags[tmp32]; cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[tmp32].d;} CYCLES(2);");
    } else if (op->func == FCMOV_ST0_STj_NCF_AND_NZF) {
        out("if (!");
        out(getFlag(CF));
        out(" && !");
        out(getFlag(ZF));
        out(") {tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[cpu->fpu.top] = cpu->fpu.tags[tmp32]; cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[tmp32].d;} CYCLES(2);");
    } else if (op->func == FCMOV_ST0_STj_NPF) {
        out("if (!");
        out(getFlag(PF));
        out(") {tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[cpu->fpu.top] = cpu->fpu.tags[tmp32]; cpu->fpu.regs[cpu->fpu.top].d = cpu->fpu.regs[tmp32].d;} CYCLES(2);");
    } else if (op->func == FNCLEX) {
        out("cpu->fpu.sw &= 0x7f00; CYCLES(9);");
    } else if (op->func == FNINIT) {
        out("FPU_FINIT(&cpu->fpu); CYCLES(12);");
    } else if (op->func == FUCOMI_ST0_STj) {
        out("FPU_FCOMI(cpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out(")); CYCLES(1);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == FCOMI_ST0_STj_Pop) {
        out("FPU_FCOMI(cpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out(")); FPU_FPOP(cpu); CYCLES(1);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == FILD_DWORD_INTEGER_16) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(1);");
    } else if (op->func == FISTTP32_16) {
        out("writed(cpu->memory, ");
        out(getEaa16(op));
        out(", (S32)cpu->fpu.regs[cpu->fpu.top].d); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FIST_DWORD_INTEGER_16) {
        out("FPU_FST_I32(cpu, ");
        out(getEaa16(op));
        out("); CYCLES(6);");
    } else if (op->func == FIST_DWORD_INTEGER_16_Pop) {
        out("FPU_FST_I32(cpu, ");
        out(getEaa16(op));
        out("); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FLD_EXTENDED_REAL_16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = FPU_FLD80(readq(cpu->memory, eaa), readw(cpu->memory, eaa + 8)); CYCLES(3);");
    } else if (op->func == FSTP_EXTENDED_REAL_16) {
        out("FPU_ST80(cpu, ");
        out(getEaa16(op));
        out(", cpu->ftp.top); FPU_FPOP(cpu); CYCLES(3);");
    } else if (op->func == FILD_DWORD_INTEGER_32) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(1);");
    } else if (op->func == FISTTP32_32) {
        out("writed(cpu->memory, ");
        out(getEaa32(op));
        out(", (S32)cpu->fpu.regs[cpu->fpu.top].d); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FIST_DWORD_INTEGER_32) {
        out("FPU_FST_I32(cpu, ");
        out(getEaa32(op));
        out("); CYCLES(6);");
    } else if (op->func == FIST_DWORD_INTEGER_32_Pop) {
        out("FPU_FST_I32(cpu, ");
        out(getEaa32(op));
        out("); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FLD_EXTENDED_REAL_32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = FPU_FLD80(readq(cpu->memory, eaa), readw(cpu->memory, eaa + 8)); CYCLES(3);");
    } else if (op->func == FSTP_EXTENDED_REAL_32) {
        out("FPU_ST80(cpu, ");
        out(getEaa32(op));
        out(", cpu->ftp.top); FPU_FPOP(cpu); CYCLES(3);");
    } else {
        kpanic("gen0db");
    }
}

void OPCALL FADD_STi_ST0(struct CPU* cpu, struct Op* op);
void OPCALL FMUL_STi_ST0(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_STi(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_STi_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FSUBR_STi_ST0(struct CPU* cpu, struct Op* op);
void OPCALL FSUB_STi_ST0(struct CPU* cpu, struct Op* op);
void OPCALL FDIVR_STi_ST0(struct CPU* cpu, struct Op* op);
void OPCALL FDIV_STi_ST0(struct CPU* cpu, struct Op* op);
void OPCALL FADD_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FMUL_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_DOUBLE_REAL_16_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FSUB_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FSUBR_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FDIV_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FDIVR_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FADD_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FMUL_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_DOUBLE_REAL_32_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FSUB_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FSUBR_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FDIV_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FDIVR_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op);
void gen0dc(struct Op* op) {
    char tmp[16];
    itoa(op->r1, tmp, 10);

    if (op->func == FADD_STi_ST0) {
        out("cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d += cpu->fpu.regs[cpu->fpu.top].d; CYCLES(1);");
    } else if (op->func == FMUL_STi_ST0) {
        out("cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d *= cpu->fpu.regs[cpu->fpu.top].d; CYCLES(1);");
    } else if (op->func == FCOM_STi) {
        out("FPU_FCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out("));CYCLES(1);");
    } else if (op->func == FCOM_STi_Pop) {
        out("FPU_FCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out("));FPU_FPOP(cpu);CYCLES(1);");
    } else if (op->func == FSUBR_STi_ST0) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d - cpu->fpu.regs[tmp32].d; CYCLES(1);");
    } else if (op->func == FSUB_STi_ST0) {
        out("cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d -= cpu->fpu.regs[cpu->fpu.top].d; CYCLES(1);");
    } else if (op->func == FDIVR_STi_ST0) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d / cpu->fpu.regs[tmp32].d; CYCLES(39);");
    } else if (op->func == FDIV_STi_ST0) {
        out("cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d /= cpu->fpu.regs[cpu->fpu.top].d; CYCLES(39);");
    } else if (op->func == FADD_DOUBLE_REAL_16) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d += d2l.d; CYCLES(1);");
    } else if (op->func == FMUL_DOUBLE_REAL_16) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d *= d2l.d; CYCLES(1);");
    } else if (op->func == FCOM_DOUBLE_REAL_16) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[8].d = d2l.d; FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); CYCLES(1);");
    } else if (op->func == FCOM_DOUBLE_REAL_16_Pop) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[8].d = d2l.d; FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FSUB_DOUBLE_REAL_16) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d -= d2l.d; CYCLES(1);");
    } else if (op->func == FSUBR_DOUBLE_REAL_16) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d = d2l.d - cpu->fpu.regs[cpu->fpu.top].d; CYCLES(39);");
    } else if (op->func == FDIV_DOUBLE_REAL_16) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d /= d2l.d; CYCLES(1);");
    } else if (op->func == FDIVR_DOUBLE_REAL_16) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa16(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d = d2l.d / cpu->fpu.regs[cpu->fpu.top].d; CYCLES(39);");
    } else if (op->func == FADD_DOUBLE_REAL_32) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d += d2l.d; CYCLES(1);");
    } else if (op->func == FMUL_DOUBLE_REAL_32) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d *= d2l.d; CYCLES(1);");
    } else if (op->func == FCOM_DOUBLE_REAL_32) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[8].d = d2l.d; FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); CYCLES(1);");
    } else if (op->func == FCOM_DOUBLE_REAL_32_Pop) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[8].d = d2l.d; FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FSUB_DOUBLE_REAL_32) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d -= d2l.d; CYCLES(1);");
    } else if (op->func == FSUBR_DOUBLE_REAL_32) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d = d2l.d - cpu->fpu.regs[cpu->fpu.top].d; CYCLES(39);");
    } else if (op->func == FDIV_DOUBLE_REAL_32) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d /= d2l.d; CYCLES(1);");
    } else if (op->func == FDIVR_DOUBLE_REAL_32) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa32(op));
        out(");cpu->fpu.regs[cpu->fpu.top].d = d2l.d / cpu->fpu.regs[cpu->fpu.top].d; CYCLES(39);");
    } else {
        kpanic("gen0dc");
    }
}

void OPCALL FFREE_STi(struct CPU* cpu, struct Op* op);
void OPCALL FXCH_STi(struct CPU* cpu, struct Op* op);
void OPCALL FST_STi(struct CPU* cpu, struct Op* op);
void OPCALL FST_STi_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FUCOM_STi(struct CPU* cpu, struct Op* op);
void OPCALL FUCOM_STi_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FLD_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FISTTP64_16(struct CPU* cpu, struct Op* op);
void OPCALL FST_DOUBLE_REAL_16(struct CPU* cpu, struct Op* op);
void OPCALL FST_DOUBLE_REAL_16_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FRSTOR_16(struct CPU* cpu, struct Op* op);
void OPCALL FNSAVE_16(struct CPU* cpu, struct Op* op);
void OPCALL FNSTSW_16(struct CPU* cpu, struct Op* op);
void OPCALL FLD_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FISTTP64_32(struct CPU* cpu, struct Op* op);
void OPCALL FST_DOUBLE_REAL_32(struct CPU* cpu, struct Op* op);
void OPCALL FST_DOUBLE_REAL_32_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FRSTOR_32(struct CPU* cpu, struct Op* op);
void OPCALL FNSAVE_32(struct CPU* cpu, struct Op* op);
void OPCALL FNSTSW_32(struct CPU* cpu, struct Op* op);
void gen0dd(struct Op* op) {
    char tmp[16];
    itoa(op->r1, tmp, 10);

    if (op->func == FFREE_STi) {
        out("cpu->fpu.tags[STV(&cpu->fpu, ");
        out(tmp);
        out(")] = TAG_Empty; CYCLES(1);");
    } else if (op->func == FXCH_STi) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); tmp32_2 = cpu->fpu.tags[tmp32]; tmpd = cpu->fpu.regs[tmp32].d; cpu->fpu.tags[tmp32] = cpu->fpu.tags[cpu->fpu.top]; cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d; cpu->fpu.tags[cpu->fpu.top] = tmp32_2; cpu->fpu.regs[cpu->fpu.top].d = tmpd; CYCLES(1);");
    } else if (op->func == FST_STi) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[tmp32] = cpu->fpu.tags[cpu->fpu.top]; cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d;CYCLES(1);");
    } else if (op->func == FST_STi_Pop) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[tmp32] = cpu->fpu.tags[cpu->fpu.top]; cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d;FPU_FPOP(cpu);CYCLES(1);");
    } else if (op->func == FUCOM_STi) {
        out("FPU_FUCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out(")); CYCLES(1);");
    } else if (op->func == FUCOM_STi_Pop) {
        out("FPU_FUCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out(")); FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FLD_DOUBLE_REAL_16) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa16(op));
        out("); FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = d2l.d; CYCLES(1);");	    
    } else if (op->func == FISTTP64_16) {
        out("writeq(cpu->memory, ");
        out(getEaa16(op));
        out(", (S64)cpu->fpu.regs[cpu->fpu.top].d); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FST_DOUBLE_REAL_16) {
        out("writeq(cpu->memory, ");
        out(getEaa16(op));
        out(", cpu->fpu.regs[cpu->fpu.top].l); CYCLES(2);");
    } else if (op->func == FST_DOUBLE_REAL_16_Pop) {
        out("writeq(cpu->memory, ");
        out(getEaa16(op));
        out(", cpu->fpu.regs[cpu->fpu.top].l); FPU_FPOP(cpu); CYCLES(2);");
    } else if (op->func == FRSTOR_16) {
        out("FPU_FRSTOR(cpu, ");
        out(getEaa16(op));
        out("); CYCLES(75);");
    } else if (op->func == FNSAVE_16) {
        out("FPU_FSAVE(cpu, ");
        out(getEaa16(op));
        out("); CYCLES(127);");
    } else if (op->func == FNSTSW_16) {
        out("FPU_SET_TOP(&cpu->fpu, cpu->fpu.top); writew(cpu->memory, ");
        out(getEaa16(op));
        out(", cpu->fpu.sw); CYCLES(2);");
    } else if (op->func == FLD_DOUBLE_REAL_32) {
        out("d2l.l = readq(cpu->memory, ");
        out(getEaa32(op));
        out("); FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = d2l.d; CYCLES(1);");	    
    } else if (op->func == FISTTP64_32) {
        out("writeq(cpu->memory, ");
        out(getEaa32(op));
        out(", (S64)cpu->fpu.regs[cpu->fpu.top].d); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FST_DOUBLE_REAL_32) {
        out("writeq(cpu->memory, ");
        out(getEaa32(op));
        out(", cpu->fpu.regs[cpu->fpu.top].l); CYCLES(2);");
    } else if (op->func == FST_DOUBLE_REAL_32_Pop) {
        out("writeq(cpu->memory, ");
        out(getEaa32(op));
        out(", cpu->fpu.regs[cpu->fpu.top].l); FPU_FPOP(cpu); CYCLES(2);");
    } else if (op->func == FRSTOR_32) {
        out("FPU_FRSTOR(cpu, ");
        out(getEaa32(op));
        out("); CYCLES(75);");
    } else if (op->func == FNSAVE_32) {
        out("FPU_FSAVE(cpu, ");
        out(getEaa32(op));
        out("); CYCLES(127);");
    } else if (op->func == FNSTSW_32) {
        out("FPU_SET_TOP(&cpu->fpu, cpu->fpu.top); writew(cpu->memory, ");
        out(getEaa32(op));
        out(", cpu->fpu.sw); CYCLES(2);");
    } else {
        kpanic("gen0dd");
    }
}

void OPCALL FADD_STi_ST0_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FMUL_STi_ST0_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FCOM_STi_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FCOMPP(struct CPU* cpu, struct Op* op);
void OPCALL FSUBR_STi_ST0_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FSUB_STi_ST0_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FDIVR_STi_ST0_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FDIV_STi_ST0_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FIADD_WORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FIMUL_WORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FICOM_WORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FICOM_WORD_INTEGER_16_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FISUB_WORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FISUBR_WORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FIDIV_WORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FIDIVR_WORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FIADD_WORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FIMUL_WORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FICOM_WORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FICOM_WORD_INTEGER_32_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FISUB_WORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FISUBR_WORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FIDIV_WORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FIDIVR_WORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void gen0de(struct Op* op) {
    char tmp[16];
    itoa(op->r1, tmp, 10);

    if (op->func == FADD_STi_ST0_Pop) {
        out("cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d += cpu->fpu.regs[cpu->fpu.top].d; FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FMUL_STi_ST0_Pop) {
        out("cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d *= cpu->fpu.regs[cpu->fpu.top].d; FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FCOM_STi_Pop) {
        out("FPU_FCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out(")); FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FCOMPP) {
        out("FPU_FCOM(&cpu->fpu, cpu->fpu.top, STV(&cpu->fpu, 1));FPU_FPOP(cpu);FPU_FPOP(cpu);CYCLES(1);");
    } else if (op->func == FSUBR_STi_ST0_Pop) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d - cpu->fpu.regs[tmp32].d; FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FSUB_STi_ST0_Pop) {
        out("cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d -= cpu->fpu.regs[cpu->fpu.top].d; FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FDIVR_STi_ST0_Pop) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d / cpu->fpu.regs[tmp32].d; FPU_FPOP(cpu); CYCLES(39);");
    } else if (op->func == FDIV_STi_ST0_Pop) {
        out("cpu->fpu.regs[STV(&cpu->fpu, ");
        out(tmp);
        out(")].d /= cpu->fpu.regs[cpu->fpu.top].d; FPU_FPOP(cpu); CYCLES(39);");
    } else if (op->func == FIADD_WORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d += (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(4);");
    } else if (op->func == FIMUL_WORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d *= (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(4);");
    } else if (op->func == FICOM_WORD_INTEGER_32) {
        out("cpu->fpu.regs[8].d = (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out("); FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); CYCLES(4);");
    } else if (op->func == FICOM_WORD_INTEGER_32_Pop) {
        out("cpu->fpu.regs[8].d = (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out("); FPU_FCOM(&cpu->fpu, cpu->fpu.top, 8); FPU_FPOP(cpu); CYCLES(4);");
    } else if (op->func == FISUB_WORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d -= (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(4);");
    } else if (op->func == FISUBR_WORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d = (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out(") - cpu->fpu.regs[cpu->fpu.top].d; CYCLES(4);");
    } else if (op->func == FIDIV_WORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d /= (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(42);");
    } else if (op->func == FIDIVR_WORD_INTEGER_32) {
        out("cpu->fpu.regs[cpu->fpu.top].d = (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out(") / cpu->fpu.regs[cpu->fpu.top].d; CYCLES(42);");
    }  else {
        kpanic("gen0de");
    }
}

void OPCALL FFREEP_STi(struct CPU* cpu, struct Op* op);
void OPCALL FXCH_STi(struct CPU* cpu, struct Op* op);
void OPCALL FST_STi_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FNSTSW_AX(struct CPU* cpu, struct Op* op);
void OPCALL FUCOMI_ST0_STj_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FCOMI_ST0_STj_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FILD_WORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FISTTP16_32(struct CPU* cpu, struct Op* op);
void OPCALL FIST_WORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FIST_WORD_INTEGER_32_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FBLD_PACKED_BCD_32(struct CPU* cpu, struct Op* op);
void OPCALL FILD_QWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FBSTP_PACKED_BCD_32(struct CPU* cpu, struct Op* op);
void OPCALL FISTP_QWORD_INTEGER_32(struct CPU* cpu, struct Op* op);
void OPCALL FILD_WORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FISTTP16_16(struct CPU* cpu, struct Op* op);
void OPCALL FIST_WORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FIST_WORD_INTEGER_16_Pop(struct CPU* cpu, struct Op* op);
void OPCALL FBLD_PACKED_BCD_16(struct CPU* cpu, struct Op* op);
void OPCALL FILD_QWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void OPCALL FBSTP_PACKED_BCD_16(struct CPU* cpu, struct Op* op);
void OPCALL FISTP_QWORD_INTEGER_16(struct CPU* cpu, struct Op* op);
void gen0df(struct Op* op) {
    char tmp[16];
    itoa(op->r1, tmp, 10);

    if (op->func == FFREEP_STi) {
        out("cpu->fpu.tags[STV(&cpu->fpu, ");
        out(tmp);
        out("] = TAG_Empty; FPU_FPOP(cpu); CYCLES(1);");
    } else if (op->func == FXCH_STi) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); tmp32_2 = cpu->fpu.tags[tmp32]; tmpd = cpu->fpu.regs[tmp32].d; cpu->fpu.tags[tmp32] = cpu->fpu.tags[cpu->fpu.top]; cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d; cpu->fpu.tags[cpu->fpu.top] = tmp32_2; cpu->fpu.regs[cpu->fpu.top].d = tmpd; CYCLES(1);");
    } else if (op->func == FNSTSW_AX) {
        out("FPU_SET_TOP(&cpu->fpu, cpu->fpu.top); AX = cpu->fpu.sw; CYCLES(2);");
    } else if (op->func == FST_STi_Pop) {
        out("tmp32 = STV(&cpu->fpu, ");
        out(tmp);
        out("); cpu->fpu.tags[tmp32] = cpu->fpu.tags[cpu->fpu.top]; cpu->fpu.regs[tmp32].d = cpu->fpu.regs[cpu->fpu.top].d;FPU_FPOP(cpu);CYCLES(1);");
    } else if (op->func == FUCOMI_ST0_STj_Pop) {
        out("FPU_FCOMI(cpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out(")); FPU_FPOP(cpu); CYCLES(1);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == FCOMI_ST0_STj_Pop) {
        out("FPU_FCOMI(cpu, cpu->fpu.top, STV(&cpu->fpu, ");
        out(tmp);
        out(")); FPU_FPOP(cpu); CYCLES(1);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == FILD_WORD_INTEGER_16) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = (S16)readw(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(1);");
    } else if (op->func == FISTTP16_16) {
        out("writew(cpu->memory, ");
        out(getEaa16(op));
        out(", (S16)cpu->fpu.regs[cpu->fpu.top].d); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FIST_WORD_INTEGER_16) {
        out("FPU_FST_I16(cpu, ");
        out(getEaa16(op));
        out("); CYCLES(6);");
    } else if (op->func == FIST_WORD_INTEGER_16_Pop) {
        out("FPU_FST_I16(cpu, ");
        out(getEaa16(op));
        out("); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FBLD_PACKED_BCD_16) {
        out("FBLD_PACKED_BCD(cpu, ");
        out(getEaa16(op));
        out("); CYCLES(48);");
    } else if (op->func == FILD_QWORD_INTEGER_16) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = (double)((S64)readq(cpu->memory, ");
        out(getEaa16(op));
        out(")); CYCLES(1);");
    } else if (op->func == FBSTP_PACKED_BCD_16) {
        out("FPU_FBST(cpu, ");
        out(getEaa16(op));
        out("); FPU_FPOP(cpu); CYCLES(148);");
    } else if (op->func == FISTP_QWORD_INTEGER_16) {
        out("FPU_FST_I64(cpu, ");
        out(getEaa16(op));
        out("); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FILD_WORD_INTEGER_32) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(1);");
    } else if (op->func == FISTTP16_32) {
        out("writew(cpu->memory, ");
        out(getEaa32(op));
        out(", (S16)cpu->fpu.regs[cpu->fpu.top].d); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FIST_WORD_INTEGER_32) {
        out("FPU_FST_I16(cpu, ");
        out(getEaa32(op));
        out("); CYCLES(6);");
    } else if (op->func == FIST_WORD_INTEGER_32_Pop) {
        out("FPU_FST_I16(cpu, ");
        out(getEaa32(op));
        out("); FPU_FPOP(cpu); CYCLES(6);");
    } else if (op->func == FBLD_PACKED_BCD_32) {
        out("FBLD_PACKED_BCD(cpu, ");
        out(getEaa32(op));
        out("); CYCLES(48);");
    } else if (op->func == FILD_QWORD_INTEGER_32) {
        out("FPU_PREP_PUSH(cpu); cpu->fpu.regs[cpu->fpu.top].d = (double)((S64)readq(cpu->memory, ");
        out(getEaa32(op));
        out(")); CYCLES(1);");
    } else if (op->func == FBSTP_PACKED_BCD_32) {
        out("FPU_FBST(cpu, ");
        out(getEaa32(op));
        out("); FPU_FPOP(cpu); CYCLES(148);");
    } else if (op->func == FISTP_QWORD_INTEGER_32) {
        out("FPU_FST_I64(cpu, ");
        out(getEaa32(op));
        out("); FPU_FPOP(cpu); CYCLES(6);");
    } else {
        kpanic("gen0df");
    }
}

void OPCALL loopnz16(struct CPU* cpu, struct Op* op);
void OPCALL loopnz32(struct CPU* cpu, struct Op* op);
void gen0e0(struct Op* op) {
    char tmp[16];

    if (op->func == loopnz16) {
        out("CX--; if (CX!=0 && !");
        out(getFlag(ZF));
        out(") {cpu->eip.u32+=");
    } else if (op->func == loopnz32) {
        out("ECX--; if (ECX!=0 && !");
        out(getFlag(ZF));
        out(") {cpu->eip.u32+=");
    } else {
        kpanic("gen0e0");
    }
    itoa(op->eipCount+op->data1, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock2(cpu);} else {cpu->eip.u32+=");
    itoa(op->eipCount, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock1(cpu);} CYCLES(7);");
}

void OPCALL loopz16(struct CPU* cpu, struct Op* op);
void OPCALL loopz32(struct CPU* cpu, struct Op* op);
void gen0e1(struct Op* op) {
    char tmp[16];

    if (op->func == loopz16) {
        out("CX--; if (CX!=0 && ");
        out(getFlag(ZF));
        out(") {cpu->eip.u32+=");
    } else if (op->func == loopz32) {
        out("ECX--; if (ECX!=0 && ");
        out(getFlag(ZF));
        out(") {cpu->eip.u32+=");
    } else {
        kpanic("gen0e1");
    }
    itoa(op->eipCount+op->data1, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock2(cpu);} else {cpu->eip.u32+=");
    itoa(op->eipCount, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock1(cpu);} CYCLES(7);");
}

void OPCALL loop16(struct CPU* cpu, struct Op* op);
void OPCALL loop32(struct CPU* cpu, struct Op* op);
void gen0e2(struct Op* op) {
    char tmp[16];

    if (op->func == loop16) {
        out("CX--; if (CX!=0) {cpu->eip.u32+=");
    } else if (op->func == loop32) {
        out("ECX--; if (ECX!=0) {cpu->eip.u32+=");
    } else {
        kpanic("gen0e2");
    }
    itoa(op->eipCount+op->data1, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock2(cpu);} else {cpu->eip.u32+=");
    itoa(op->eipCount, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock1(cpu);} CYCLES(7);");
}

void OPCALL jcxz16(struct CPU* cpu, struct Op* op);
void OPCALL jcxz32(struct CPU* cpu, struct Op* op);
void gen0e3(struct Op* op) {
    char tmp[16];

    if (op->func == jcxz16) {
        out("if (CX==0) {cpu->eip.u32+=");
    } else if (op->func == jcxz32) {
        out("if (ECX==0) {cpu->eip.u32+=");
    } else {
        kpanic("gen0e3");
    }
    itoa(op->eipCount+op->data1, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock2(cpu);} else {cpu->eip.u32+=");
    itoa(op->eipCount, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock1(cpu);} CYCLES(7);");
}

void gen0e8(struct Op* op) {
    char tmp[16];
    itoa(op->eipCount, tmp, 10);
    out("push16(cpu, cpu->eip.u32 + ");
    out(tmp);
    out("); cpu->eip.u32 += ");
    out(tmp);
    out(" + ");
    itoa(op->data1, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock1(cpu); CYCLES(1);");
}

void gen2e8(struct Op* op) {
    char tmp[16];
    itoa(op->eipCount, tmp, 10);
    out("push32(cpu, cpu->eip.u32 + ");
    out(tmp);
    out("); cpu->eip.u32 += ");
    out(tmp);
    out(" + ");
    itoa(op->data1, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock1(cpu); CYCLES(1);");
}

void gen0e9(struct Op* op) {
    char tmp[16];
    itoa(op->eipCount, tmp, 10);
    out("cpu->eip.u32 += ");
    out(tmp);
    out(" + ");
    itoa(op->data1, tmp, 10);
    out(tmp);
    out("; cpu->nextBlock = getBlock1(cpu); CYCLES(1);");
}

void gen0f5(struct Op* op) {
    out("fillFlags(cpu); setCF(cpu, !(cpu->flags & CF)); CYCLES(2);");
    currentLazyFlags = sFLAGS_NONE;
}

void OPCALL test8_reg(struct CPU* cpu, struct Op* op);
void OPCALL test8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL test8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL not8_reg(struct CPU* cpu, struct Op* op);
void OPCALL not8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL not8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL neg8_reg(struct CPU* cpu, struct Op* op);
void OPCALL neg8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL neg8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL mul8_reg(struct CPU* cpu, struct Op* op);
void OPCALL mul8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL mul8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL imul8_reg(struct CPU* cpu, struct Op* op);
void OPCALL imul8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL imul8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL div8_reg(struct CPU* cpu, struct Op* op);
void OPCALL div8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL div8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL idiv8_reg(struct CPU* cpu, struct Op* op);
void OPCALL idiv8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL idiv8_mem32(struct CPU* cpu, struct Op* op);
void gen0f6(struct Op* op) {
    char data[16];

    itoa(op->data1, data, 16);

    if (op->func == test8_reg) {
        out("cpu->dst.u8 = ");
        out(r8(op->r1));
        out("; cpu->src.u8 = 0x");
        out(data);
        out("; cpu->result.u8 = cpu->dst.u8 & cpu->src.u8; cpu->lazyFlags = FLAGS_TEST8; CYCLES(1);");
        currentLazyFlags = sFLAGS_TEST8;
    } else if (op->func == test8_mem16) {
        out("cpu->dst.u8 = readb(cpu->memory, ");
        out(getEaa16(op));
        out("); cpu->src.u8 = 0x");
        out(data);
        out("; cpu->result.u8 = cpu->dst.u8 & cpu->src.u8; cpu->lazyFlags = FLAGS_TEST8; CYCLES(2);");
        currentLazyFlags = sFLAGS_TEST8;
    } else if (op->func == test8_mem32) {
        out("cpu->dst.u8 = readb(cpu->memory, ");
        out(getEaa32(op));
        out("); cpu->src.u8 = 0x");
        out(data);
        out("; cpu->result.u8 = cpu->dst.u8 & cpu->src.u8; cpu->lazyFlags = FLAGS_TEST8; CYCLES(2);");
        currentLazyFlags = sFLAGS_TEST8;
    } else if (op->func == not8_reg) {
        out(r8(op->r1));
        out(" = ~ ");
        out(r8(op->r1));
	    out("; CYCLES(1);");
    } else if (op->func == not8_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writeb(cpu->memory, eaa, ~readb(cpu->memory, eaa)); CYCLES(3);");
    } else if (op->func == not8_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writeb(cpu->memory, eaa, ~readb(cpu->memory, eaa)); CYCLES(3);");
    } else if (op->func == neg8_reg) {
        out("cpu->dst.u8 = ");
        out(r8(op->r1));
	    out("; cpu->result.u8 = 0-cpu->dst.u8; ");
	    out(r8(op->r1));
        out(" = cpu->result.u8; cpu->lazyFlags = FLAGS_NEG8; CYCLES(1);");
        currentLazyFlags = sFLAGS_NEG8;
    } else if (op->func == neg8_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; cpu->dst.u8 = readb(cpu->memory, eaa); cpu->result.u8 = 0-cpu->dst.u8; writeb(cpu->memory, eaa, cpu->result.u8); cpu->lazyFlags = FLAGS_NEG8; CYCLES(3);");
        currentLazyFlags = sFLAGS_NEG8;
    } else if (op->func == neg8_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; cpu->dst.u8 = readb(cpu->memory, eaa); cpu->result.u8 = 0-cpu->dst.u8; writeb(cpu->memory, eaa, cpu->result.u8); cpu->lazyFlags = FLAGS_NEG8; CYCLES(3);");
        currentLazyFlags = sFLAGS_NEG8;
    } else if (op->func == mul8_reg) {
        out("AX = AL * ");
        out(r8(op->r1));
        out("; fillFlagsNoCFOF(cpu); if (AX>0xFF) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == mul8_mem16) {
        out("AX = AL * readb(cpu->memory, ");
        out(getEaa16(op));
        out("); fillFlagsNoCFOF(cpu); if (AX>0xFF) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == mul8_mem32) {
        out("AX = AL * readb(cpu->memory, ");
        out(getEaa32(op));
        out("); fillFlagsNoCFOF(cpu); if (AX>0xFF) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == imul8_reg) {
        out("AX = (S16)((S8)AL) * (S8)(");
        out(r8(op->r1));
        out("); fillFlagsNoCFOF(cpu);if ((S16)AX<-128 || (S16)AX>127) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == imul8_mem16) {
        out("AX = (S16)((S8)AL) * (S8)readb(cpu->memory, ");
        out(getEaa16(op));
        out("); fillFlagsNoCFOF(cpu); if ((S16)AX<-128 || (S16)AX>127) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == imul8_mem32) {
        out("AX = (S16)((S8)AL) * (S8)readb(cpu->memory, ");
        out(getEaa32(op));
        out("); fillFlagsNoCFOF(cpu); if ((S16)AX<-128 || (S16)AX>127) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == div8_reg) {
        out("div8(cpu, ");
        out(r8(op->r1));
        out("); CYCLES(17);");
    } else if (op->func == div8_mem16) {
        out("div8(cpu, readb(cpu->memory, ");
        out(getEaa16(op));
        out(")); CYCLES(17);");
    } else if (op->func == div8_mem32) {
        out("div8(cpu, readb(cpu->memory, ");
        out(getEaa32(op));
        out(")); CYCLES(17);");
    } else if (op->func == idiv8_reg) {
        out("idiv8(cpu, (S8)");
        out(r8(op->r1));
        out("); CYCLES(22);");
    } else if (op->func == idiv8_mem16) {
        out("idiv8(cpu, (S8)readb(cpu->memory, ");
        out(getEaa16(op));
        out(")); CYCLES(22);");
    } else if (op->func == idiv8_mem32) {
        out("idiv8(cpu, (S8)readb(cpu->memory, ");
        out(getEaa32(op));
        out(")); CYCLES(22);");
    } else {
        kpanic("gen0f6");
    }
}

void OPCALL test16_reg(struct CPU* cpu, struct Op* op);
void OPCALL test16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL test16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL not16_reg(struct CPU* cpu, struct Op* op);
void OPCALL not16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL not16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL neg16_reg(struct CPU* cpu, struct Op* op);
void OPCALL neg16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL neg16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL mul16_reg(struct CPU* cpu, struct Op* op);
void OPCALL mul16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL mul16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL imul16_reg(struct CPU* cpu, struct Op* op);
void OPCALL imul16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL imul16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL div16_reg(struct CPU* cpu, struct Op* op);
void OPCALL div16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL div16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL idiv16_reg(struct CPU* cpu, struct Op* op);
void OPCALL idiv16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL idiv16_mem32(struct CPU* cpu, struct Op* op);
void gen0f7(struct Op* op) {
    char data[16];

    itoa(op->data1, data, 16);

    if (op->func == test16_reg) {
        out("cpu->dst.u16 = ");
        out(r16(op->r1));
        out("; cpu->src.u16 = 0x");
        out(data);
        out("; cpu->result.u16 = cpu->dst.u16 & cpu->src.u16; cpu->lazyFlags = FLAGS_TEST16; CYCLES(1);");
        currentLazyFlags = sFLAGS_TEST16;
    } else if (op->func == test16_mem16) {
        out("cpu->dst.u16 = readw(cpu->memory, ");
        out(getEaa16(op));
        out("); cpu->src.u16 = 0x");
        out(data);
        out("; cpu->result.u16 = cpu->dst.u16 & cpu->src.u16; cpu->lazyFlags = FLAGS_TEST16; CYCLES(2);");
        currentLazyFlags = sFLAGS_TEST16;
    } else if (op->func == test16_mem32) {
        out("cpu->dst.u16 = readw(cpu->memory, ");
        out(getEaa32(op));
        out("); cpu->src.u16 = 0x");
        out(data);
        out("; cpu->result.u16 = cpu->dst.u16 & cpu->src.u16; cpu->lazyFlags = FLAGS_TEST16; CYCLES(2);");
        currentLazyFlags = sFLAGS_TEST16;
    } else if (op->func == not16_reg) {
        out(r16(op->r1));
        out(" = ~ ");
        out(r16(op->r1));
	    out("; CYCLES(1);");
    } else if (op->func == not16_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writew(cpu->memory, eaa, ~readw(cpu->memory, eaa)); CYCLES(3);");
    } else if (op->func == not16_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writew(cpu->memory, eaa, ~readw(cpu->memory, eaa)); CYCLES(3);");
    } else if (op->func == neg16_reg) {
        out("cpu->dst.u16 = ");
        out(r16(op->r1));
	    out("; cpu->result.u16 = 0-cpu->dst.u16; ");
	    out(r16(op->r1));
        out(" = cpu->result.u16; cpu->lazyFlags = FLAGS_NEG16; CYCLES(1);");
        currentLazyFlags = sFLAGS_NEG16;
    } else if (op->func == neg16_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; cpu->dst.u16 = readw(cpu->memory, eaa); cpu->result.u16 = 0-cpu->dst.u16; writew(cpu->memory, eaa, cpu->result.u16); cpu->lazyFlags = FLAGS_NEG16; CYCLES(3);");
        currentLazyFlags = sFLAGS_NEG16;
    } else if (op->func == neg16_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; cpu->dst.u16 = readw(cpu->memory, eaa); cpu->result.u16 = 0-cpu->dst.u16; writew(cpu->memory, eaa, cpu->result.u16); cpu->lazyFlags = FLAGS_NEG16; CYCLES(3);");
        currentLazyFlags = sFLAGS_NEG16;
    } else if (op->func == mul16_reg) {
        out("tmp32 = (U32)AX * ");
        out(r16(op->r1));
        out("; AX = (U16)tmp32; DX = (U16)(tmp32 >> 16); fillFlagsNoCFOF(cpu); if (DX) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == mul16_mem16) {
        out("tmp32 = (U32)AX * readw(cpu->memory, ");
        out(getEaa32(op));
        out("); AX = (U16)tmp32; DX = (U16)(tmp32 >> 16); fillFlagsNoCFOF(cpu); if (DX) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == mul16_mem32) {
        out("tmp32 = (U32)AX * readw(cpu->memory, ");
        out(getEaa32(op));
        out("); AX = (U16)tmp32; DX = (U16)(tmp32 >> 16); fillFlagsNoCFOF(cpu); if (DX) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == imul16_reg) {
        out("tmps32 = (S32)((S16)AX) * (S16)");
        out(r16(op->r1));
        out("; AX = (S16)tmps32; DX = (S16)(tmps32 >> 16); fillFlagsNoCFOF(cpu); if (tmps32>32767 || tmps32<-32768) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF); } CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == imul16_mem16) {
        out("tmps32 = (S32)((S16)AX) * (S16)readw(cpu->memory, ");
        out(getEaa16(op));
        out("); AX = (S16)tmps32; DX = (S16)(tmps32 >> 16); fillFlagsNoCFOF(cpu); if (tmps32>32767 || tmps32<-32768) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF); } CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == imul16_mem32) {
        out("tmps32 = (S32)((S16)AX) * (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out("); AX = (S16)tmps32; DX = (S16)(tmps32 >> 16); fillFlagsNoCFOF(cpu); if (tmps32>32767 || tmps32<-32768) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF); } CYCLES(11);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == div16_reg) {
        out("div16(cpu, ");
        out(r16(op->r1));
        out("); CYCLES(25);");
    } else if (op->func == div16_mem16) {
        out("div16(cpu, readw(cpu->memory, ");
        out(getEaa16(op));
        out(")); CYCLES(25);");
    } else if (op->func == div16_mem32) {
        out("div16(cpu, readw(cpu->memory, ");
        out(getEaa32(op));
        out(")); CYCLES(25);");
    } else if (op->func == idiv16_reg) {
        out("idiv16(cpu, (S16)");
        out(r16(op->r1));
        out("); CYCLES(30);");
    } else if (op->func == idiv16_mem16) {
        out("idiv16(cpu, (S16)readw(cpu->memory, ");
        out(getEaa16(op));
        out(")); CYCLES(30);");
    } else if (op->func == idiv16_mem32) {
        out("idiv16(cpu, (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out(")); CYCLES(30);");
    } else {
        kpanic("gen0f7");
    }
}

void OPCALL test32_reg(struct CPU* cpu, struct Op* op);
void OPCALL test32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL test32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL not32_reg(struct CPU* cpu, struct Op* op);
void OPCALL not32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL not32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL neg32_reg(struct CPU* cpu, struct Op* op);
void OPCALL neg32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL neg32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL mul32_reg(struct CPU* cpu, struct Op* op);
void OPCALL mul32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL mul32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL imul32_reg(struct CPU* cpu, struct Op* op);
void OPCALL imul32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL imul32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL div32_reg(struct CPU* cpu, struct Op* op);
void OPCALL div32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL div32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL idiv32_reg(struct CPU* cpu, struct Op* op);
void OPCALL idiv32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL idiv32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL neg32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL neg32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL neg32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL mul32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL mul32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL mul32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL imul32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL imul32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL imul32_mem32_noflags(struct CPU* cpu, struct Op* op);
void gen2f7(struct Op* op) {
    char data[16];

    itoa(op->data1, data, 16);

    if (op->func == test32_reg) {
        out("cpu->dst.u32 = ");
        out(r32(op->r1));
        out("; cpu->src.u32 = 0x");
        out(data);
        out("; cpu->result.u32 = cpu->dst.u32 & cpu->src.u32; cpu->lazyFlags = FLAGS_TEST32; CYCLES(1);");
        currentLazyFlags = sFLAGS_TEST32;
    } else if (op->func == test32_mem16) {
        out("cpu->dst.u32 = readd(cpu->memory, ");
        out(getEaa16(op));
        out("); cpu->src.u32 = 0x");
        out(data);
        out("; cpu->result.u32 = cpu->dst.u32 & cpu->src.u32; cpu->lazyFlags = FLAGS_TEST32; CYCLES(2);");
        currentLazyFlags = sFLAGS_TEST32;
    } else if (op->func == test32_mem32) {
        out("cpu->dst.u32 = readd(cpu->memory, ");
        out(getEaa32(op));
        out("); cpu->src.u32 = 0x");
        out(data);
        out("; cpu->result.u32 = cpu->dst.u32 & cpu->src.u32; cpu->lazyFlags = FLAGS_TEST32; CYCLES(2);");
        currentLazyFlags = sFLAGS_TEST32;
    } else if (op->func == not32_reg) {
        out(r32(op->r1));
        out(" = ~ ");
        out(r32(op->r1));
	    out("; CYCLES(1);");
    } else if (op->func == not32_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writed(cpu->memory, eaa, ~readd(cpu->memory, eaa)); CYCLES(3);");
    } else if (op->func == not32_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writed(cpu->memory, eaa, ~readd(cpu->memory, eaa)); CYCLES(3);");
    } else if (op->func == neg32_reg) {
        out("cpu->dst.u32 = ");
        out(r32(op->r1));
	    out("; cpu->result.u32 = 0-cpu->dst.u32; ");
	    out(r32(op->r1));
        out(" = cpu->result.u32; cpu->lazyFlags = FLAGS_NEG32; CYCLES(1);");
        currentLazyFlags = sFLAGS_NEG32;
    } else if (op->func == neg32_reg_noflags) {
        out("cpu->dst.u32 = ");
        out(r32(op->r1));
	    out(" = 0 - ");
	    out(r32(op->r1));
        out("; CYCLES(1);");
    } else if (op->func == neg32_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32 = 0-cpu->dst.u32; writed(cpu->memory, eaa, cpu->result.u32); cpu->lazyFlags = FLAGS_NEG32; CYCLES(3);");
        currentLazyFlags = sFLAGS_NEG32;
    } else if (op->func == neg32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writed(cpu->memory, eaa, 0-readd(cpu->memory, eaa)); CYCLES(3);");
    } else if (op->func == neg32_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32 = 0-cpu->dst.u32; writed(cpu->memory, eaa, cpu->result.u32); cpu->lazyFlags = FLAGS_NEG32; CYCLES(3);");
        currentLazyFlags = sFLAGS_NEG32;
    } else if (op->func == neg32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writed(cpu->memory, eaa, 0-readd(cpu->memory, eaa)); CYCLES(3);");
    } else if (op->func == mul32_reg) {
        out("tmp64 = (U64)EAX * ");
        out(r32(op->r1));
        out("; EAX = (U32)tmp64; EDX = (U32)(tmp64 >> 32); fillFlagsNoCFOF(cpu); if (EDX) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(10);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == mul32_reg_noflags) {
        out("tmp64 = (U64)EAX * ");
        out(r32(op->r1));
        out("; EAX = (U32)tmp64; EDX = (U32)(tmp64 >> 32); CYCLES(10);");
    } else if (op->func == mul32_mem16) {
        out("tmp64 = (U64)EAX * readd(cpu->memory, ");
        out(getEaa16(op));
        out("); EAX = (U32)tmp64; EDX = (U32)(tmp64 >> 32); fillFlagsNoCFOF(cpu); if (EDX) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(10);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == mul32_mem16_noflags) {
        out("tmp64 = (U64)EAX * readd(cpu->memory, ");
        out(getEaa16(op));
        out("); EAX = (U32)tmp64; EDX = (U32)(tmp64 >> 32); CYCLES(10);");
    } else if (op->func == mul32_mem32) {
        out("tmp64 = (U64)EAX * readd(cpu->memory, ");
        out(getEaa16(op));
        out("); EAX = (U32)tmp64; EDX = (U32)(tmp64 >> 32); fillFlagsNoCFOF(cpu); if (EDX) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(10);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == mul32_mem32_noflags) {
        out("tmp64 = (U64)EAX * readd(cpu->memory, ");
        out(getEaa32(op));
        out("); EAX = (U32)tmp64; EDX = (U32)(tmp64 >> 32); CYCLES(10);");
    } else if (op->func == imul32_reg) {
        out("tmps64 = (S64)((S32)EAX) * (S32)");
        out(r32(op->r1));
        out("; EAX = (S32)tmps64; EDX = (S32)(tmps64 >> 32); fillFlagsNoCFOF(cpu); if (tmps64>0x7fffffffl || tmps64<-0x7fffffffl) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(10);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == imul32_reg_noflags) {
        out("tmps64 = (S64)((S32)EAX) * (S32)");
        out(r32(op->r1));
        out("; EAX = (S32)tmps64; EDX = (S32)(tmps64 >> 32); CYCLES(10);");
    } else if (op->func == imul32_mem16) {
        out("tmps64 = (S64)((S32)EAX) * (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out("); EAX = (S32)tmps64; EDX = (S32)(tmps64 >> 32); fillFlagsNoCFOF(cpu); if (tmps64>0x7fffffffl || tmps64<-0x7fffffffl) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(10);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == imul32_mem16_noflags) {
        out("tmps64 = (S64)((S32)EAX) * (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out("); EAX = (S32)tmps64; EDX = (S32)(tmps64 >> 32); CYCLES(10);");
    } else if (op->func == imul32_mem32) {
        out("tmps64 = (S64)((S32)EAX) * (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out("); EAX = (S32)tmps64; EDX = (S32)(tmps64 >> 32); fillFlagsNoCFOF(cpu); if (tmps64>0x7fffffffl || tmps64<-0x7fffffffl) {cpu->flags|=CF|OF;} else {cpu->flags&=~(CF|OF);} CYCLES(10);");
        currentLazyFlags = sFLAGS_NONE;
    } else if (op->func == imul32_mem32_noflags) {
        out("tmps64 = (S64)((S32)EAX) * (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out("); EAX = (S32)tmps64; EDX = (S32)(tmps64 >> 32); CYCLES(10);");
    } else if (op->func == div32_reg) {
        out("div32(cpu, ");
        out(r32(op->r1));
        out("); CYCLES(41);");
    } else if (op->func == div32_mem16) {
        out("div32(cpu, readd(cpu->memory, ");
        out(getEaa16(op));
        out(")); CYCLES(41);");
    } else if (op->func == div32_mem32) {
        out("div32(cpu, readd(cpu->memory, ");
        out(getEaa32(op));
        out(")); CYCLES(41);");
    } else if (op->func == idiv32_reg) {
        out("idiv32(cpu, (S32)");
        out(r32(op->r1));
        out("); CYCLES(46);");
    } else if (op->func == idiv32_mem16) {
        out("idiv32(cpu, (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out(")); CYCLES(46);");
    } else if (op->func == idiv32_mem32) {
        out("idiv32(cpu, (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out(")); CYCLES(46);");
    } else {
        kpanic("gen2f7");
    }
}

void gen0f8(struct Op* op) {
    out("fillFlags(cpu); cpu->flags &= ~CF; CYCLES(2);");
    currentLazyFlags = sFLAGS_NONE;
}

void gen0f9(struct Op* op) {
    out("fillFlags(cpu); cpu->flags |= CF; CYCLES(2);");
    currentLazyFlags = sFLAGS_NONE;
}

void gen0fc(struct Op* op) {
    out("removeFlag(DF); cpu->df = 1; CYCLES(2);");
}

void gen0fd(struct Op* op) {
    out("addFlag(DF); cpu->df = -1; CYCLES(2);");
}

void OPCALL inc8_reg(struct CPU* cpu, struct Op* op);
void OPCALL inc8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL inc8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL dec8_reg(struct CPU* cpu, struct Op* op);
void OPCALL dec8_mem16(struct CPU* cpu, struct Op* op);
void OPCALL dec8_mem32(struct CPU* cpu, struct Op* op);
void OPCALL inc8_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL inc8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL inc8_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dec8_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dec8_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dec8_mem32_noflags(struct CPU* cpu, struct Op* op);
void gen0fe(struct Op* op) {
    if (op->func == inc8_reg) {
        out("cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u8=");
        out(r8(op->r1));
        out("; cpu->result.u8=cpu->dst.u8 + 1; cpu->lazyFlags = FLAGS_INC8; ");
        currentLazyFlags = sFLAGS_INC8;
        out(r8(op->r1));
        out(" = cpu->result.u8; CYCLES(1);");
    } else if (op->func == inc8_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u8 = readb(cpu->memory, eaa); cpu->result.u8=cpu->dst.u8 + 1; cpu->lazyFlags = FLAGS_INC8; writeb(cpu->memory, eaa, cpu->result.u8); CYCLES(3);");
        currentLazyFlags = sFLAGS_INC8;
    } else if (op->func == inc8_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u8 = readb(cpu->memory, eaa); cpu->result.u8=cpu->dst.u8 + 1; cpu->lazyFlags = FLAGS_INC8; writeb(cpu->memory, eaa, cpu->result.u8); CYCLES(3);");
        currentLazyFlags = sFLAGS_INC8;
    } else if (op->func == dec8_reg) {
        out("cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u8=");
        out(r8(op->r1));
        out("; cpu->result.u8=cpu->dst.u8 - 1; cpu->lazyFlags = FLAGS_DEC8; ");
        currentLazyFlags = sFLAGS_DEC8;
        out(r8(op->r1));
        out(" = cpu->result.u8; CYCLES(1);");
    } else if (op->func == dec8_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u8 = readb(cpu->memory, eaa); cpu->result.u8=cpu->dst.u8 - 1; cpu->lazyFlags = FLAGS_DEC8; writeb(cpu->memory, eaa, cpu->result.u8); CYCLES(3);");
        currentLazyFlags = sFLAGS_DEC8;
    } else if (op->func == dec8_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u8 = readb(cpu->memory, eaa); cpu->result.u8=cpu->dst.u8 - 1; cpu->lazyFlags = FLAGS_DEC8; writeb(cpu->memory, eaa, cpu->result.u8); CYCLES(3);");
        currentLazyFlags = sFLAGS_DEC8;
    } else if (op->func == inc8_reg_noflags) {
        out(r8(op->r1));
        out(" = ");
        out(r8(op->r1));
        out(" + 1; CYCLES(1);");
    } else if (op->func == inc8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) + 1); CYCLES(3);");
    } else if (op->func == inc8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) + 1); CYCLES(3);");
    } else if (op->func == dec8_reg_noflags) {
        out(r8(op->r1));
        out(" = ");
        out(r8(op->r1));
        out(" - 1; CYCLES(1);");
    } else if (op->func == dec8_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) - 1); CYCLES(3);");
    } else if (op->func == dec8_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writeb(cpu->memory, eaa, readb(cpu->memory, eaa) - 1); CYCLES(3);");
    } else {
        kpanic("gen0fe");
    }
}

void OPCALL inc16_reg(struct CPU* cpu, struct Op* op);
void OPCALL inc16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL inc16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL dec16_reg(struct CPU* cpu, struct Op* op);
void OPCALL dec16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL dec16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL inc16_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL inc16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL inc16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dec16_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dec16_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dec16_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL callEv16_reg(struct CPU* cpu, struct Op* op);
void OPCALL callEv16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL callEv16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL jmpEv16_reg(struct CPU* cpu, struct Op* op);
void OPCALL jmpEv16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL jmpEv16_mem32(struct CPU* cpu, struct Op* op);
void OPCALL pushEv16_reg(struct CPU* cpu, struct Op* op);
void OPCALL pushEv16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL pushEv16_mem32(struct CPU* cpu, struct Op* op);
void gen0ff(struct Op* op) {
    if (op->func == inc16_reg) {
        out("cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u16=");
        out(r16(op->r1));
        out("; cpu->result.u16=cpu->dst.u16 + 1; cpu->lazyFlags = FLAGS_INC16; ");
        currentLazyFlags = sFLAGS_INC16;
        out(r16(op->r1));
        out(" = cpu->result.u16; CYCLES(1);");
    } else if (op->func == inc16_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u16 = readw(cpu->memory, eaa); cpu->result.u16=cpu->dst.u16 + 1; cpu->lazyFlags = FLAGS_INC16; writew(cpu->memory, eaa, cpu->result.u16); CYCLES(3);");
        currentLazyFlags = sFLAGS_INC16;
    } else if (op->func == inc16_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u16 = readw(cpu->memory, eaa); cpu->result.u16=cpu->dst.u16 + 1; cpu->lazyFlags = FLAGS_INC16; writew(cpu->memory, eaa, cpu->result.u16); CYCLES(3);");
        currentLazyFlags = sFLAGS_INC16;
    } else if (op->func == dec16_reg) {
        out("cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u16=");
        out(r16(op->r1));
        out("; cpu->result.u16=cpu->dst.u16 - 1; cpu->lazyFlags = FLAGS_DEC16; ");
        currentLazyFlags = sFLAGS_DEC16;
        out(r16(op->r1));
        out(" = cpu->result.u16; CYCLES(1);");
    } else if (op->func == dec16_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u16 = readw(cpu->memory, eaa); cpu->result.u16=cpu->dst.u16 - 1; cpu->lazyFlags = FLAGS_DEC16; writew(cpu->memory, eaa, cpu->result.u16); CYCLES(3);");
        currentLazyFlags = sFLAGS_DEC16;
    } else if (op->func == dec16_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u16 = readw(cpu->memory, eaa); cpu->result.u16=cpu->dst.u16 - 1; cpu->lazyFlags = FLAGS_DEC16; writew(cpu->memory, eaa, cpu->result.u16); CYCLES(3);");
        currentLazyFlags = sFLAGS_DEC16;
    } else if (op->func == inc16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" + 1; CYCLES(1);");
    } else if (op->func == inc16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) + 1); CYCLES(3);");
    } else if (op->func == inc16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) + 1); CYCLES(3);");
    } else if (op->func == dec16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" - 1; CYCLES(1);");
    } else if (op->func == dec16_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) - 1); CYCLES(3);");
    } else if (op->func == dec16_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writew(cpu->memory, eaa, readw(cpu->memory, eaa) - 1); CYCLES(3);");
    } else if (op->func == callEv16_reg) {
        char tmp[16];

        out("push16(cpu, cpu->eip.u32 + ");
        itoa(op->eipCount, tmp, 10);
        out(tmp);
        out("); cpu->eip.u32 = ");
        out(r16(op->r1));
        out("; CYCLES(2); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == callEv16_mem16) {
        char tmp[16];

        out("tmp32 = cpu->eip.u32 + ");
        itoa(op->eipCount, tmp, 10);
        out(tmp);
	    out("; tmp16 = readw(cpu->memory, ");
        out(getEaa16(op));
        out("); push16(cpu, tmp32); cpu->eip.u32 = tmp16; CYCLES(4); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == callEv16_mem32) {
        char tmp[16];

        out("tmp32 = cpu->eip.u32 + ");
        itoa(op->eipCount, tmp, 10);
        out(tmp);
	    out("; tmp16 = readw(cpu->memory, ");
        out(getEaa32(op));
        out("); push16(cpu, tmp32); cpu->eip.u32 = tmp16; CYCLES(4); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == jmpEv16_reg) {
        out("cpu->eip.u32 = ");
        out(r16(op->r1));
        out("; CYCLES(2); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == jmpEv16_mem16) {
        out("cpu->eip.u32 = readw(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(2); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == jmpEv16_mem32) {
        out("cpu->eip.u32 = readw(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(2); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == pushEv16_reg) {
        out("push16(cpu, ");
        out(r16(op->r1));
        out("); CYCLES(1);");
    } else if (op->func == pushEv16_mem16) {
        out("push16(cpu, readw(cpu->memory, ");
        out(getEaa16(op));
        out(")); CYCLES(2);");
    } else if (op->func == pushEv16_mem32) {
        out("push16(cpu, readw(cpu->memory, ");
        out(getEaa32(op));
        out(")); CYCLES(2);");
    } else {
        kpanic("gen0ff");
    }
}

void OPCALL inc32_reg(struct CPU* cpu, struct Op* op);
void OPCALL inc32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL inc32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL dec32_reg(struct CPU* cpu, struct Op* op);
void OPCALL dec32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL dec32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL inc32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL inc32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL inc32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dec32_reg_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dec32_mem16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dec32_mem32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL callNear32_reg(struct CPU* cpu, struct Op* op);
void OPCALL callNear32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL callNear32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL jmpNear32_reg(struct CPU* cpu, struct Op* op);
void OPCALL jmpNear32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL jmpNear32_mem32(struct CPU* cpu, struct Op* op);
void OPCALL pushEd_reg(struct CPU* cpu, struct Op* op);
void OPCALL pushEd_mem16(struct CPU* cpu, struct Op* op);
void OPCALL pushEd_mem32(struct CPU* cpu, struct Op* op);
void gen2ff(struct Op* op) {
    if (op->func == inc32_reg) {
        out("cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u32=");
        out(r32(op->r1));
        out("; cpu->result.u32=cpu->dst.u32 + 1; cpu->lazyFlags = FLAGS_INC32; ");
        currentLazyFlags = sFLAGS_INC32;
        out(r32(op->r1));
        out(" = cpu->result.u32; CYCLES(1);");
    } else if (op->func == inc32_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32=cpu->dst.u32 + 1; cpu->lazyFlags = FLAGS_INC32; writed(cpu->memory, eaa, cpu->result.u32); CYCLES(3);");
        currentLazyFlags = sFLAGS_INC32;
    } else if (op->func == inc32_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32=cpu->dst.u32 + 1; cpu->lazyFlags = FLAGS_INC32; writed(cpu->memory, eaa, cpu->result.u32); CYCLES(3);");
        currentLazyFlags = sFLAGS_INC32;
    } else if (op->func == dec32_reg) {
        out("cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u32=");
        out(r32(op->r1));
        out("; cpu->result.u32=cpu->dst.u32 - 1; cpu->lazyFlags = FLAGS_DEC32; ");
        currentLazyFlags = sFLAGS_DEC32;
        out(r32(op->r1));
        out(" = cpu->result.u32; CYCLES(1);");
    } else if (op->func == dec32_mem16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32=cpu->dst.u32 - 1; cpu->lazyFlags = FLAGS_DEC32; writed(cpu->memory, eaa, cpu->result.u32); CYCLES(3);");
        currentLazyFlags = sFLAGS_DEC32;
    } else if (op->func == dec32_mem32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; cpu->oldcf=");
        out(getFlag(CF));
        out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32=cpu->dst.u32 - 1; cpu->lazyFlags = FLAGS_DEC32; writed(cpu->memory, eaa, cpu->result.u32); CYCLES(3);");
        currentLazyFlags = sFLAGS_DEC32;
    } else if (op->func == inc32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" + 1; CYCLES(1);");
    } else if (op->func == inc32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) + 1); CYCLES(3);");
    } else if (op->func == inc32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) + 1); CYCLES(3);");
    } else if (op->func == dec32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" - 1; CYCLES(1);");
    } else if (op->func == dec32_mem16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) - 1); CYCLES(3);");
    } else if (op->func == dec32_mem32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
        out("; writed(cpu->memory, eaa, readd(cpu->memory, eaa) - 1); CYCLES(3);");
    } else if (op->func == callNear32_reg) {
        char tmp[16];

        out("push32(cpu, cpu->eip.u32 + ");
        itoa(op->eipCount, tmp, 10);
        out(tmp);
        out("); cpu->eip.u32 = ");
        out(r32(op->r1));
        out("; CYCLES(2); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == callNear32_mem16) {
        char tmp[16];

        out("tmp32 = cpu->eip.u32 + ");
        itoa(op->eipCount, tmp, 10);
        out(tmp);
	    out("; tmp32_2 = readd(cpu->memory, ");
        out(getEaa16(op));
        out("); push32(cpu, tmp32); cpu->eip.u32 = tmp32_2; CYCLES(4); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == callNear32_mem32) {
        char tmp[16];

        out("tmp32 = cpu->eip.u32 + ");
        itoa(op->eipCount, tmp, 10);
        out(tmp);
	    out("; tmp32_2 = readd(cpu->memory, ");
        out(getEaa32(op));
        out("); push32(cpu, tmp32); cpu->eip.u32 = tmp32_2; CYCLES(4); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == jmpNear32_reg) {
        out("cpu->eip.u32 = ");
        out(r32(op->r1));
        out("; CYCLES(2); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == jmpNear32_mem16) {
        out("cpu->eip.u32 = readd(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(4); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == jmpNear32_mem32) {
        out("cpu->eip.u32 = readd(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(4); cpu->nextBlock = getBlock(cpu);");
    } else if (op->func == pushEd_reg) {
        out("push32(cpu, ");
        out(r32(op->r1));
        out("); CYCLES(1);");
    } else if (op->func == pushEd_mem16) {
        out("push32(cpu, readd(cpu->memory, ");
        out(getEaa16(op));
        out(")); CYCLES(2);");
    } else if (op->func == pushEd_mem32) {
        out("push32(cpu, readd(cpu->memory, ");
        out(getEaa32(op));
        out(")); CYCLES(2);");
    } else {
        kpanic("gen2ff");
    }
}

void gen131(struct Op* op) {
    out("tmp64 = cpu->timeStampCounter+cpu->blockCounter; EAX = (U32)tmp64; EDX = (U32)(tmp64 >> 32); CYCLES(1);");
}

void gen1a2(struct Op* op) {
    out("cpuid(cpu); CYCLES(14);");
}

void OPCALL btr16r16(struct CPU* cpu, struct Op* op);
void OPCALL bte16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL bte16r16_32(struct CPU* cpu, struct Op* op);
void gen1a3(struct Op* op) {
    if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
         out("fillFlagsNoCF(cpu); ");
         currentLazyFlags = sFLAGS_NONE;
    }
    if (op->func == btr16r16) {
        out("setCF(cpu, ");
        out(r16(op->r1));
        out(" & (1 << (");
        out(r16(op->r2));
        out(" & 15))); CYCLES(4);");
    } else if (op->func == bte16r16_16) {
        out("setCF(cpu, (readw(cpu->memory, (");
        out(getEaa16(op));
        out(")+(((S16)");
        out(r16(op->r1));
        out(")>>4)*2) & (1 << (");
        out(r16(op->r1));
        out(" & 15)))); CYCLES(9)");
    } else if (op->func == bte16r16_32) {
        out("setCF(cpu, (readw(cpu->memory, (");
        out(getEaa32(op));
        out(")+(((S16)");
        out(r16(op->r1));
        out(")>>4)*2) & (1 << (");
        out(r16(op->r1));
        out(" & 15)))); CYCLES(9)");
    } else {
        kpanic("gen1a3");
    }
} 

void OPCALL btr32r32(struct CPU* cpu, struct Op* op);
void OPCALL bte32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL bte32r32_32(struct CPU* cpu, struct Op* op);
void gen3a3(struct Op* op) {
    if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
         out("fillFlagsNoCF(cpu); ");
         currentLazyFlags = sFLAGS_NONE;
    }
    if (op->func == btr32r32) {
        out("setCF(cpu, ");
        out(r32(op->r1));
        out(" & (1 << (");
        out(r32(op->r2));
        out(" & 31))); CYCLES(4);");
    } else if (op->func == bte32r32_16) {
        out("setCF(cpu, (readd(cpu->memory, (");
        out(getEaa16(op));
        out(")+(((S32)");
        out(r32(op->r1));
        out(")>>5)*4) & (1 << (");
        out(r32(op->r1));
        out(" & 31)))); CYCLES(9)");
    } else if (op->func == bte32r32_32) {
        out("setCF(cpu, (readd(cpu->memory, (");
        out(getEaa32(op));
        out(")+(((S32)");
        out(r32(op->r1));
        out(")>>5)*4) & (1 << (");
        out(r32(op->r1));
        out(" & 31)))); CYCLES(9)");
    } else {
        kpanic("gen3a3");
    }
} 

void OPCALL dshlr16r16(struct CPU* cpu, struct Op* op);
void OPCALL dshle16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL dshle16r16_32(struct CPU* cpu, struct Op* op);
void gen1a4(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 10);

    if (op->func == dshlr16r16) {
        out("cpu->src.u32 = ");
        out(tmp);
        out("; cpu->dst.u32 = ");
        out(r16(op->r1));
        out("; cpu->dst2.u32 = ");
        out(r16(op->r2));
        out("; tmp32=(((cpu->dst.u32<<16)|cpu->dst2.u32) << cpu->src.u8)");
  	    if (op->data1>16) {
            out("|((U32)(");
            out(r16(op->r2));
            out(") << (");
            out(tmp);
            out(" - 16))");
        }
	    out("; cpu->result.u16=(U16)(tmp32 >> 16); ");
        out(r16(op->r1));
	    out(" = cpu->result.u16; cpu->lazyFlags=FLAGS_DSHL16; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHL16;
    } else {
        out("eaa = ");
        if (op->func == dshle16r16_16)
            out(getEaa16(op));
        else if (op->func == dshle16r16_32) 
            out(getEaa32(op));
        else
            kpanic("gen1a4");
        out("; cpu->src.u32 = ");
        out(tmp);
        out("; cpu->dst.u32 = readw(cpu->memory, eaa); cpu->dst2.u32 = ");
        out(r16(op->r1));
        out("; tmp32=(((cpu->dst.u32<<16)|cpu->dst2.u32) << cpu->src.u8)");
  	    if (op->data1>16) {
            out("|((U32)(");
            out(r16(op->r1));
            out(") << (");
            out(tmp);
            out(" - 16))");
        }
	    out("; cpu->result.u16=(U16)(tmp32 >> 16); writew(cpu->memory, eaa, cpu->result.u16); cpu->lazyFlags=FLAGS_DSHL16; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHL16;
    }
} 

void OPCALL dshlr32r32(struct CPU* cpu, struct Op* op);
void OPCALL dshle32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL dshle32r32_32(struct CPU* cpu, struct Op* op);
void gen3a4(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 10);

    if (op->func == dshlr32r32) {
        out("cpu->src.u32 = ");
        out(tmp);
        out("; cpu->dst.u32 = ");
        out(r32(op->r1));
        out("; cpu->result.u32=(");
        out(r32(op->r1));
        out(" << ");
        out(tmp);
        out(") | (");
        out(r32(op->r2));
        out(" >> (32 - ");
        out(tmp);
        out(")); ");
        out(r32(op->r1));
	    out(" = cpu->result.u32; cpu->lazyFlags=FLAGS_DSHL32; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHL32;
    } else {
        out("eaa = ");
        if (op->func == dshle32r32_16)
            out(getEaa16(op));
        else if (op->func == dshle32r32_32) 
            out(getEaa32(op));
        else
            kpanic("gen3a4");
        out("; cpu->src.u32 = ");
        out(tmp);
        out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32=(cpu->dst.u32 << ");
        out(tmp);
        out(") | (");
        out(r32(op->r1));
        out(" >> (32 - ");
        out(tmp);
        out(")); writed(cpu->memory, eaa, cpu->result.u32); cpu->lazyFlags=FLAGS_DSHL32; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHL32;
    }
} 

void OPCALL dshlclr16r16(struct CPU* cpu, struct Op* op);
void OPCALL dshlcle16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL dshlcle16r16_32(struct CPU* cpu, struct Op* op);
void gen1a5(struct Op* op) {
    if (op->func == dshlclr16r16) {
        out("cpu->src.u32 = CL; cpu->dst.u32 = ");
        out(r16(op->r1));
        out("; cpu->dst2.u32 = ");
        out(r16(op->r2));
        out("; tmp32=(((cpu->dst.u32<<16)|cpu->dst2.u32) << cpu->src.u8)");
  	    if (op->data1>16) {
            out("|((U32)(");
            out(r16(op->r2));
            out(") << (CL - 16))");
        }
	    out("; cpu->result.u16=(U16)(tmp32 >> 16); ");
        out(r16(op->r1));
	    out(" = cpu->result.u16; cpu->lazyFlags=FLAGS_DSHL16; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHL16;
    } else {
        out("eaa = ");
        if (op->func == dshlcle16r16_16)
            out(getEaa16(op));
        else if (op->func == dshlcle16r16_32) 
            out(getEaa32(op));
        else
            kpanic("gen1a5");
        out("; cpu->src.u32 = CL; cpu->dst.u32 = readw(cpu->memory, eaa); cpu->dst2.u32 = ");
        out(r16(op->r1));
        out("; tmp32=(((cpu->dst.u32<<16)|cpu->dst2.u32) << cpu->src.u8)");
  	    if (op->data1>16) {
            out("|((U32)(");
            out(r16(op->r1));
            out(") << (cpu->src.u32 - 16))");
        }
	    out("; cpu->result.u16=(U16)(tmp32 >> 16); writew(cpu->memory, eaa, cpu->result.u16); cpu->lazyFlags=FLAGS_DSHL16; CYCLES(5);");
        currentLazyFlags = sFLAGS_DSHL16;
    }
} 

void OPCALL dshlclr32r32(struct CPU* cpu, struct Op* op);
void OPCALL dshlcle32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL dshlcle32r32_32(struct CPU* cpu, struct Op* op);
void gen3a5(struct Op* op) {
    if (op->func == dshlclr32r32) {
        out("cpu->src.u32 = CL; cpu->dst.u32 = ");
        out(r32(op->r1));
        out("; cpu->result.u32=(");
        out(r32(op->r1));
        out(" << CL) | (");
        out(r32(op->r2));
        out(" >> (32 - CL)); ");
        out(r32(op->r1));
	    out(" = cpu->result.u32; cpu->lazyFlags=FLAGS_DSHL32; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHL32;
    } else {
        out("eaa = ");
        if (op->func == dshlcle32r32_16)
            out(getEaa16(op));
        else if (op->func == dshlcle32r32_32) 
            out(getEaa32(op));
        else
            kpanic("gen3a5");
        out("; cpu->src.u32 = CL; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32=(cpu->dst.u32 << CL) | (");
        out(r32(op->r1));
        out(" >> (32 - CL)); writed(cpu->memory, eaa, cpu->result.u32); cpu->lazyFlags=FLAGS_DSHL32; CYCLES(5);");
        currentLazyFlags = sFLAGS_DSHL32;
    }
}

void OPCALL btsr16r16(struct CPU* cpu, struct Op* op);
void OPCALL btse16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL btse16r16_32(struct CPU* cpu, struct Op* op);
void gen1ab(struct Op* op) {
    if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
         out("fillFlagsNoCF(cpu); ");
         currentLazyFlags = sFLAGS_NONE;
    }
    if (op->func == btsr16r16) {
        out("tmp16 = (1 << (");
        out(r16(op->r2));
        out(" & 15))); setCF(cpu, ");
        out(r16(op->r1));
        out(" & tmp16); ");
        out(r16(op->r1));
        out("|=tmp16; CYCLES(7);");
    } else if (op->func == btse16r16_16) {
        out("eaa = ");
        out(getEaa16(op));
        out(")+((S16)");
        out(r16(op->r1));
        out(")>>4)*2; tmp16 = (1 << (");
        out(r16(op->r1));
        out(" & 15))); tmp16_2 = readw(cpu->memory, eaa); setCF(cpu, tmp16_2 & tmp16); writew(cpu->memory, eaa, tmp16_2 | tmp16); CYCLES(13);");
    } else if (op->func == btse16r16_32) {
        out("eaa = ");
        out(getEaa32(op));
        out(")+((S16)");
        out(r16(op->r1));
        out(")>>4)*2; tmp16 = (1 << (");
        out(r16(op->r1));
        out(" & 15))); tmp16_2 = readw(cpu->memory, eaa); setCF(cpu, tmp16_2 & tmp16); writew(cpu->memory, eaa, tmp16_2 | tmp16); CYCLES(13);");
    } else {
        kpanic("gen1ab");
    }
} 

void OPCALL btsr32r32(struct CPU* cpu, struct Op* op);
void OPCALL btse32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL btse32r32_32(struct CPU* cpu, struct Op* op);
void gen3ab(struct Op* op) {
    if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
         out("fillFlagsNoCF(cpu); ");
         currentLazyFlags = sFLAGS_NONE;
    }
    if (op->func == btsr32r32) {
        out("tmp32 = (1 << (");
        out(r32(op->r2));
        out(" & 31))); setCF(cpu, ");
        out(r32(op->r1));
        out(" & tmp32); ");
        out(r32(op->r1));
        out("|=tmp32; CYCLES(7);");
    } else if (op->func == btse32r32_16) {
        out("eaa = ");
        out(getEaa16(op));
        out(")+((S32)");
        out(r32(op->r1));
        out(")>>5)*4; tmp32 = (1 << (");
        out(r32(op->r1));
        out(" & 31))); tmp32_2 = readd(cpu->memory, eaa); setCF(cpu, tmp32_2 & tmp32); writed(cpu->memory, eaa, tmp32_2 | tmp32); CYCLES(13);");
    } else if (op->func == btse32r32_32) {
        out("eaa = ");
        out(getEaa32(op));
        out(")+((S32)");
        out(r32(op->r1));
        out(")>>5)*4; tmp32 = (1 << (");
        out(r32(op->r1));
        out(" & 31))); tmp32_2 = readd(cpu->memory, eaa); setCF(cpu, tmp32_2 & tmp32); writed(cpu->memory, eaa, tmp32_2 | tmp32); CYCLES(13);");
    } else {
        kpanic("gen3ab");
    }
} 

void OPCALL dshrr16r16(struct CPU* cpu, struct Op* op);
void OPCALL dshre16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL dshre16r16_32(struct CPU* cpu, struct Op* op);
void gen1ac(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 10);

    if (op->func == dshrr16r16) {
        out("cpu->src.u32 = ");
        out(tmp);
        out("; cpu->dst.u32 = (");
        out(r16(op->r1));
        out(")|((U32)(");
        out(r16(op->r2));
        out(")<<16); tmp32 = (cpu->dst.u32 >> cpu->src.u8)");
  	    if (op->data1>16) {
            out(" | ((U32)(");
            out(r16(op->r2));
            out(") << (32 - ");
            out(tmp);
            out("))");
        }
	    out("; cpu->result.u16=(U16)tmp32;");
        out(r16(op->r1));
	    out(" = cpu->result.u16; cpu->lazyFlags=FLAGS_DSHR16; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHR16;
    } else {
        out("eaa = ");
        if (op->func == dshre16r16_16)
            out(getEaa16(op));
        else if (op->func == dshre16r16_32) 
            out(getEaa32(op));
        else
            kpanic("gen1ac");
        out("; cpu->src.u32 = ");
        out(tmp);
        out("; cpu->dst.u32 = readw(cpu->memory, eaa) | ((U32)(");
        out(r16(op->r1));
        out(")<<16); tmp32=(cpu->dst.u32 >> cpu->src.u8)");
        if (op->data1>16) {
            out(" | ((U32)(");
            out(r16(op->r1));
            out(") << (32 - ");
            out(tmp);
            out("))");
        }
	    out(";cpu->result.u16=(U16)result; writew(cpu->memory, address, cpu->result.u16); cpu->lazyFlags=FLAGS_DSHR16; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHR16;
    }
} 

void OPCALL dshrr32r32(struct CPU* cpu, struct Op* op);
void OPCALL dshre32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL dshre32r32_32(struct CPU* cpu, struct Op* op);
void OPCALL dshrr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dshre32r32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dshre32r32_32_noflags(struct CPU* cpu, struct Op* op);
void gen3ac(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 10);

    if (op->func == dshrr32r32) {
        out("cpu->src.u32 = ");
        out(tmp);
	    out("; cpu->dst.u32 = ");
        out(r32(op->r1));
	    out("; cpu->result.u32=(");
        out(r32(op->r1));
        out(" >> ");
        out(tmp);
        out(") | (");
        out(r32(op->r2));
        out(" << (32 - ");
        out(tmp);
        out(")); ");
        out(r32(op->r1));
	    out(" = cpu->result.u32; cpu->lazyFlags=FLAGS_DSHR32; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHR32;
    } else if (op->func == dshrr32r32_noflags) {
        out(r32(op->r1));
        out(" = (");
        out(r32(op->r1));
        out(" >> ");
        out(tmp);
        out(") | (");
        out(r32(op->r2));
        out(" << (32 - ");
        out(tmp);
        out(")); CYCLES(4);");
    } else if (op->func == dshre32r32_16_noflags || op->func == dshre32r32_32_noflags) {
        out("eaa = ");
        if (op->func == dshre32r32_16_noflags)
            out(getEaa16(op));
        else
            out(getEaa32(op));
        out("writed(cpu->memory, eaa, (readd(cpu->memory, eaa) >> ");
        out(tmp);
        out(") | (");
        out(r32(op->r1));
        out(" << (32 - ");
        out(tmp);
        out("))); CYCLES(4);");
    } else {
        out("eaa = ");
        if (op->func == dshre32r32_16)
            out(getEaa16(op));
        else if (op->func == dshre32r32_32) 
            out(getEaa32(op));
        else
            kpanic("gen3ac");
        out("; cpu->src.u32 = ");
        out(tmp);
        out(";  cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32=(cpu->dst.u32 >> ");
        out(tmp);
        out(") | (");
        out(r32(op->r1));
        out(" << (32 - ");
        out(tmp);
        out(")); writed(cpu->memory, eaa, cpu->result.u32); cpu->lazyFlags=FLAGS_DSHR32; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHR32;
    }
} 

void OPCALL dshrclr16r16(struct CPU* cpu, struct Op* op);
void OPCALL dshrcle16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL dshrcle16r16_32(struct CPU* cpu, struct Op* op);
void gen1ad(struct Op* op) {
    if (op->func == dshrclr16r16) {
        out("cpu->src.u32 = CL; cpu->dst.u32 = (");
        out(r16(op->r1));
        out(")|((U32)(");
        out(r16(op->r2));
        out(")<<16); tmp32 = (cpu->dst.u32 >> cpu->src.u8)");
  	    if (op->data1>16) {
            out(" | ((U32)(");
            out(r16(op->r2));
            out(") << (32 - CL))");
        }
	    out("; cpu->result.u16=(U16)tmp32;");
        out(r16(op->r1));
	    out(" = cpu->result.u16; cpu->lazyFlags=FLAGS_DSHR16; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHR16;
    } else {
        out("eaa = ");
        if (op->func == dshrcle16r16_16)
            out(getEaa16(op));
        else if (op->func == dshrcle16r16_32) 
            out(getEaa32(op));
        else
            kpanic("gen1ac");
        out("; cpu->src.u32 = CL; cpu->dst.u32 = readw(cpu->memory, eaa) | ((U32)(");
        out(r16(op->r1));
        out(")<<16); tmp32=(cpu->dst.u32 >> cpu->src.u8)");
        if (op->data1>16) {
            out(" | ((U32)(");
            out(r16(op->r1));
            out(") << (32 - CL))");
        }
	    out(";cpu->result.u16=(U16)result; writew(cpu->memory, address, cpu->result.u16); cpu->lazyFlags=FLAGS_DSHR16; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHR16;
    }
} 

void OPCALL dshrclr32r32(struct CPU* cpu, struct Op* op);
void OPCALL dshrcle32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL dshrcle32r32_32(struct CPU* cpu, struct Op* op);
void gen3ad(struct Op* op) {
    if (op->func == dshrclr32r32) {
        out("cpu->src.u32 = CL; cpu->dst.u32 = ");
        out(r32(op->r1));
	    out("; cpu->result.u32=(");
        out(r32(op->r1));
        out(" >> CL) | (");
        out(r32(op->r2));
        out(" << (32 - CL)); ");
        out(r32(op->r1));
	    out(" = cpu->result.u32; cpu->lazyFlags=FLAGS_DSHR32; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHR32;
    } else {
        out("eaa = ");
        if (op->func == dshrcle32r32_16)
            out(getEaa16(op));
        else if (op->func == dshrcle32r32_32) 
            out(getEaa32(op));
        else
            kpanic("gen3ad");
        out("; cpu->src.u32 = CL;  cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32=(cpu->dst.u32 >> CL) | (");
        out(r32(op->r1));
        out(" << (32 - CL)); writed(cpu->memory, eaa, cpu->result.u32); cpu->lazyFlags=FLAGS_DSHR32; CYCLES(4);");
        currentLazyFlags = sFLAGS_DSHR32;
    }
}

void OPCALL dimulr16r16(struct CPU* cpu, struct Op* op);
void OPCALL dimulr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL dimulr16e16_32(struct CPU* cpu, struct Op* op);
void gen1af(struct Op* op) {
    if (op->func == dimulr16r16) {
        out("tmps32 = (S16)(");
        out(r16(op->r2));
        out(") * (S32)");
        out(r16(op->r1));
        out("; fillFlagsNoCFOF(cpu); if ((tmps32 >= -32767) && (tmps32 <= 32767)) {removeFlag(CF|OF);} else {addFlag(CF|OF);}");
        currentLazyFlags = sFLAGS_NONE;
        out(r16(op->r1));
	    out(" = tmps32; CYCLES(10);");
    } else {
        out("tmps32 = (S16)(readw(cpu->memory, ");
        if (op->func == dimulr16e16_16)
            out(getEaa16(op));
        else if (op->func == dimulr16e16_32) 
            out(getEaa32(op));
        else
            kpanic("gen1af");

        out(")) * (S32)");
        out(r16(op->r1));
        out("; fillFlagsNoCFOF(cpu); if ((tmps32 >= -32767) && (tmps32 <= 32767)) {removeFlag(CF|OF);} else {addFlag(CF|OF);}");
        currentLazyFlags = sFLAGS_NONE;
	    out(r16(op->r1));
	    out(" = tmps32; CYCLES(10);");
    }
}

void OPCALL dimulr32r32(struct CPU* cpu, struct Op* op);
void OPCALL dimulr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL dimulr32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL dimulr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dimulr32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL dimulr32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen3af(struct Op* op) {
    if (op->func == dimulr32r32) {
        out("tmps64 = (S32)(");
        out(r32(op->r2));
        out(") * (S64)");
        out(r32(op->r1));
        out("; fillFlagsNoCFOF(cpu); if ((tmps64 >= -2147483647l) && (tmps64 <= 2147483647l)) {removeFlag(CF|OF);} else {addFlag(CF|OF);}");
        currentLazyFlags = sFLAGS_NONE;
        out(r32(op->r1));
	    out(" = (S32)tmps64; CYCLES(10);");
    } else if (op->func == dimulr32r32_noflags) {
        out(r32(op->r1));
        out(" = (S32)(");
        out(r32(op->r2));
        out(") * ((S32)");
        out(r32(op->r1));
        out(");CYCLES(10);");
    } else if (op->func == dimulr32e32_16_noflags || op->func == dimulr32e32_32_noflags) {
        out(r32(op->r1));
        out("= (S32)(readd(cpu->memory, ");
        if (op->func == dimulr32e32_16_noflags)
            out(getEaa16(op));
        else
            out(getEaa32(op));
        out(")) * ((S32)");
        out(r32(op->r1));
        out("); CYCLES(10);");
    } else {
        out("tmps64 = (S32)(readw(cpu->memory, ");
        if (op->func == dimulr32e32_16)
            out(getEaa16(op));
        else if (op->func == dimulr32e32_32) 
            out(getEaa32(op));
        else
            kpanic("gen3af");

        out(")) * (S64)");
        out(r32(op->r1));
        out("; fillFlagsNoCFOF(cpu); if ((tmps64 >= -2147483647l) && (tmps64 <= 2147483647l)) {removeFlag(CF|OF);} else {addFlag(CF|OF);}");
        currentLazyFlags = sFLAGS_NONE;
	    out(r32(op->r1));
	    out(" = (S32)tmps64; CYCLES(10);");
    }
}

void OPCALL cmpxchgr16r16(struct CPU* cpu, struct Op* op);
void OPCALL cmpxchgr16r16(struct CPU* cpu, struct Op* op);
void OPCALL cmpxchge16r16_32(struct CPU* cpu, struct Op* op);
void gen1b1(struct Op* op) {
    if (op->func == cmpxchgr16r16) {
        out("cpu->dst.u16 = ");
        out(r16(op->r1));
        out("; cpu->src.u16 = AX; cpu->result.u16 = cpu->dst.u16 - AX; cpu->lazyFlags = FLAGS_CMP16; if (AX == cpu->dst.u16) {");        
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out("; } else {AX = cpu->dst.u16;} CYCLES(5);");
    } else if (op->func == cmpxchgr16r16) {
        out("eaa = ");
        out(getEaa16(op));
	    out("; cpu->dst.u16 = readw(cpu->memory, eaa); cpu->src.u16 = AX; cpu->result.u16 = cpu->dst.u16 - AX; cpu->lazyFlags = FLAGS_CMP16; if (AX == cpu->dst.u16) {writew(cpu->memory, eaa, ");
        out(r16(op->r1));
        out(");} else {AX = cpu->dst.u16;} CYCLES(6);");
    } else if (op->func == cmpxchge16r16_32) {
        out("eaa = ");
        out(getEaa32(op));
	    out("; cpu->dst.u16 = readw(cpu->memory, eaa); cpu->src.u16 = AX; cpu->result.u16 = cpu->dst.u16 - AX; cpu->lazyFlags = FLAGS_CMP16; if (AX == cpu->dst.u16) {writew(cpu->memory, eaa, ");
        out(r16(op->r1));
        out(");} else {AX = cpu->dst.u16;} CYCLES(6);");
    }
    currentLazyFlags = sFLAGS_CMP16;
}

void OPCALL cmpxchgr32r32(struct CPU* cpu, struct Op* op);
void OPCALL cmpxchge32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL cmpxchge32r32_32(struct CPU* cpu, struct Op* op);
void OPCALL cmpxchgr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpxchge32r32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpxchge32r32_32_noflags(struct CPU* cpu, struct Op* op);
void gen3b1(struct Op* op) {
    if (op->func == cmpxchgr32r32) {
        out("cpu->dst.u32 = ");
        out(r32(op->r1));
        out("; cpu->src.u32 = EAX; cpu->result.u32 = cpu->dst.u32 - EAX; cpu->lazyFlags = FLAGS_CMP32; if (EAX == cpu->dst.u32) {");
        currentLazyFlags = sFLAGS_CMP32;
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} else {EAX = cpu->dst.u32;} CYCLES(5);");
    } else if (op->func == cmpxchgr32r32_noflags) {
        out("if (EAX == ");
        out(r32(op->r1));
        out(") {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} else {EAX = ");
        out(r32(op->r1));
        out(";} CYCLES(5);");
    } else if (op->func == cmpxchge32r32_16) {
        out("eaa = ");
        out(getEaa16(op));
	    out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->src.u32 = EAX; cpu->result.u32 = cpu->dst.u32 - EAX; cpu->lazyFlags = FLAGS_CMP32; if (EAX == cpu->dst.u32) { writed(cpu->memory, eaa, ");
        currentLazyFlags = sFLAGS_CMP32;
        out(r32(op->r1));
        out("); } else {EAX = cpu->dst.u32;} CYCLES(6);");
    } else if (op->func == cmpxchge32r32_16_noflags) {
        out("eaa = ");
        out(getEaa16(op));
	    out("; tmp32 = readd(cpu->memory, eaa); if (EAX == tmp32) {writed(cpu->memory, eaa, ");
        out(r32(op->r1));
        out(");} else {EAX = tmp32;} CYCLES(6);");
    } else if (op->func == cmpxchge32r32_32) {
        out("eaa = ");
        out(getEaa32(op));
	    out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->src.u32 = EAX; cpu->result.u32 = cpu->dst.u32 - EAX; cpu->lazyFlags = FLAGS_CMP32; if (EAX == cpu->dst.u32) { writed(cpu->memory, eaa, ");
        currentLazyFlags = sFLAGS_CMP32;
        out(r32(op->r1));
        out("); } else {EAX = cpu->dst.u32;} CYCLES(6);");
    } else if (op->func == cmpxchge32r32_32_noflags) {
        out("eaa = ");
        out(getEaa32(op));
	    out("; tmp32 = readd(cpu->memory, eaa); if (EAX == tmp32) {writed(cpu->memory, eaa, ");
        out(r32(op->r1));
        out(");} else {EAX = tmp32;} CYCLES(6);");
    } 
}

void OPCALL movxz8r16r16(struct CPU* cpu, struct Op* op);
void OPCALL movxz8r16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL movxz8r16e16_32(struct CPU* cpu, struct Op* op);
void gen1b6(struct Op* op) {
    if (op->func == movxz8r16r16) {
        out(r16(op->r1));
        out(" = ");
        out(r8(op->r2));
        out("; CYCLES(3);");
    } else if (op->func == movxz8r16e16_16) {
        out(r16(op->r1));
        out(" = readb(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(3);");
    } else if (op->func == movxz8r16e16_32) {
        out(r16(op->r1));
        out(" = readb(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(3);");
    } else {
        kpanic("gen1b6");
    }
}

void OPCALL movxz8r32r32(struct CPU* cpu, struct Op* op);
void OPCALL movxz8r32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL movxz8r32e32_32(struct CPU* cpu, struct Op* op);
void gen3b6(struct Op* op) {
    if (op->func == movxz8r32r32) {
        out(r32(op->r1));
        out(" = ");
        out(r8(op->r2));
        out("; CYCLES(3);");
    } else if (op->func == movxz8r32e32_16) {
        out(r32(op->r1));
        out(" = readb(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(3);");
    } else if (op->func == movxz8r32e32_32) {
        out(r32(op->r1));
        out(" = readb(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(3);");
    } else {
        kpanic("gen3b6");
    }
}

void OPCALL movxz16r32r32(struct CPU* cpu, struct Op* op);
void OPCALL movxz16r32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL movxz16r32e32_32(struct CPU* cpu, struct Op* op);
void gen3b7(struct Op* op) {
    if (op->func == movxz16r32r32) {
        out(r32(op->r1));
        out(" = ");
        out(r16(op->r2));
        out("; CYCLES(3);");
    } else if (op->func == movxz16r32e32_16) {
        out(r32(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(3);");
    } else if (op->func == movxz16r32e32_32) {
        out(r32(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(3);");
    } else {
        kpanic("gen3b7");
    }
}

void OPCALL bt_reg(struct CPU* cpu, struct Op* op);
void OPCALL bt_mem16(struct CPU* cpu, struct Op* op);
void OPCALL bt_mem32(struct CPU* cpu, struct Op* op);
void OPCALL bts_reg(struct CPU* cpu, struct Op* op);
void OPCALL bts_mem16(struct CPU* cpu, struct Op* op);
void OPCALL bts_mem32(struct CPU* cpu, struct Op* op);
void OPCALL btr_reg(struct CPU* cpu, struct Op* op);
void OPCALL btr_mem16(struct CPU* cpu, struct Op* op);
void OPCALL btr_mem32(struct CPU* cpu, struct Op* op);
void OPCALL btc_reg(struct CPU* cpu, struct Op* op);
void OPCALL btc_mem16(struct CPU* cpu, struct Op* op);
void OPCALL btc_mem32(struct CPU* cpu, struct Op* op);
void gen3ba(struct Op* op) {
    char tmp[16];
    itoa(op->data1, tmp, 16);
    if (op->func == bt_reg) {       
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("setCF(cpu, ");
        out(r32(op->r1));
        out(" & 0x");
        out(tmp);
        out("); CYCLES(4);");
    } else if (op->func == bt_mem16) {  
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("setCF(cpu, readd(cpu->memory, ");
        out(getEaa16(op));
        out(") & 0x");
        out(tmp);
        out("); CYCLES(4);");
    } else if (op->func == bt_mem32) {
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("setCF(cpu, readd(cpu->memory, ");
        out(getEaa32(op));
        out(") & 0x");
        out(tmp);
        out("); CYCLES(4);");
    } else if (op->func == bts_reg) {  
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("setCF(cpu, ");
        out(r32(op->r1));
        out(" & 0x");
        out(tmp);
        out("); ");
        out(r32(op->r1));
        out(" |= 0x");
        out(tmp);
        out("; CYCLES(7);");
    } else if (op->func == bts_mem16) {        
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("eaa = ");
        getEaa16(op);
        out(";tmp32 = readd(cpu->memory, eaa); setCF(cpu, tmp32 & 0x");
        out(tmp);
        out("); writed(cpu->memory, eaa, value | 0x");
        out(tmp);
        out("); CYCLES(8);");
    } else if (op->func == bts_mem32) {
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("eaa = ");
        getEaa32(op);
        out(";tmp32 = readd(cpu->memory, eaa); setCF(cpu, tmp32 & 0x");
        out(tmp);
        out("); writed(cpu->memory, eaa, value | 0x");
        out(tmp);
        out("); CYCLES(8);");
    } else if (op->func == btr_reg) {        
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("setCF(cpu, ");
        out(r32(op->r1));
        out(" & 0x");
        out(tmp);
        out("); ");
        out(r32(op->r1));
        out(" &= ~0x");
        out(tmp);
        out("; CYCLES(7);");
    } else if (op->func == btr_mem16) {   
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("eaa = ");
        getEaa16(op);
        out(";tmp32 = readd(cpu->memory, eaa); setCF(cpu, tmp32 & 0x");
        out(tmp);
        out("); writed(cpu->memory, eaa, value & ~0x");
        out(tmp);
        out("); CYCLES(8);");
    } else if (op->func == btr_mem32) {
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("eaa = ");
        getEaa32(op);
        out(";tmp32 = readd(cpu->memory, eaa); setCF(cpu, tmp32 & 0x");
        out(tmp);
        out("); writed(cpu->memory, eaa, value & ~0x");
        out(tmp);
        out("); CYCLES(8);");
    } else if (op->func == btc_reg) {   
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("setCF(cpu, ");
        out(r32(op->r1));
        out(" & 0x");
        out(tmp);
        out("); ");
        out(r32(op->r1));
        out(" ^= 0x");
        out(tmp);
        out("; CYCLES(7);");
    } else if (op->func == btc_mem16) {        
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("eaa = ");
        getEaa16(op);
        out(";tmp32 = readd(cpu->memory, eaa); setCF(cpu, tmp32 & 0x");
        out(tmp);
        out("); writed(cpu->memory, eaa, value ^ 0x");
        out(tmp);
        out("); CYCLES(8);");
    } else if (op->func == btc_mem32) {
        if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
            out("fillFlagsNoCF(cpu);");
            currentLazyFlags = sFLAGS_NONE;
        }
        out("eaa = ");
        getEaa32(op);
        out(";tmp32 = readd(cpu->memory, eaa); setCF(cpu, tmp32 & 0x");
        out(tmp);
        out("); writed(cpu->memory, eaa, value ^ 0x");
        out(tmp);
        out("); CYCLES(8);");
    } else {
        kpanic("gen3ba");
    }
}

void OPCALL btcr32r32(struct CPU* cpu, struct Op* op);
void OPCALL btce32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL btce32r32_32(struct CPU* cpu, struct Op* op);
void gen3bb(struct Op* op) {
    if (needsToSetFlag(op, ZF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
        out("fillFlagsNoCF(cpu);");
        currentLazyFlags = sFLAGS_NONE;
    }
    if (op->func == btcr32r32) {        
        out("tmp32 = 1 << ");
        out(r32(op->r2));
        out(" & 31; setCF(cpu, ");
        out(r32(op->r1));
        out(" & tmp32); ");
        out(r32(op->r1));
        out(" ^= tmp32; CYCLES(7);");
    } else if (op->func == btce32r32_16) {        
        out("tmp32 = 1 << ");
        out(r32(op->r1));
        out(" & 31; eaa = ");
        out(getEaa16(op));
	    out("+((((S32)");
        out(r32(op->r1));
        out(")>>3) & ~3); tmp32_2 = readd(cpu->memory, eaa); setCF(cpu, tmp32_2 & tmp32); writed(cpu->memory, eaa, tmp32_2 ^ tmp32); CYCLES(13);");
    } else if (op->func == btce32r32_32) {
        out("tmp32 = 1 << ");
        out(r32(op->r1));
        out(" & 31; eaa = ");
        out(getEaa32(op));
	    out("+((((S32)");
        out(r32(op->r1));
        out(")>>3) & ~3); tmp32_2 = readd(cpu->memory, eaa); setCF(cpu, tmp32_2 & tmp32); writed(cpu->memory, eaa, tmp32_2 ^ tmp32); CYCLES(13);");
    } else {
        kpanic("gen3bb");
    }
}

void OPCALL bsrr16r16(struct CPU* cpu, struct Op* op);
void OPCALL bsrr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL bsrr16e16_32(struct CPU* cpu, struct Op* op);
void gen1bd(struct Op* op) {
    out("tmp16 = ");
    if (op->func == bsrr16r16) {        
        out(r16(op->r2));        
    } else if (op->func == bsrr16e16_16) {        
        out("readw(cpu->memory, ");
        out(getEaa16(op));
        out(")");
    } else if (op->func == bsrr16e16_32) {
        out("readw(cpu->memory, ");
        out(getEaa32(op));
        out(")");
    } else {
        kpanic("gen3bb");
    }
    out("; if (tmp16==0) {addFlag(ZF);} else {tmp32 = 15; while ((tmp16 & 0x8000)==0) { tmp32--; tmp16<<=1; } removeFlag(ZF); ");
    out(r16(op->r1));
    out(" = result;} cpu->lazyFlags = FLAGS_NONE; CYCLES(7);");
    currentLazyFlags = sFLAGS_NONE;
}

void OPCALL bsfr32r32(struct CPU* cpu, struct Op* op);
void OPCALL bsfr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL bsfr32e32_32(struct CPU* cpu, struct Op* op);
void gen3bc(struct Op* op) {
    if (needsToSetFlag(op, CF) || needsToSetFlag(op, SF) || needsToSetFlag(op, OF) || needsToSetFlag(op, PF) || needsToSetFlag(op, AF)) {
        out("fillFlagsNoZF(cpu);");
        currentLazyFlags = sFLAGS_NONE;
    }
    out("tmp32 = ");
    if (op->func == bsfr32r32) {                
        out(r32(op->r2));        
    } else if (op->func == bsfr32e32_16) {        
        out("readd(cpu->memory, ");
        out(getEaa16(op));
        out(")");
    } else if (op->func == bsfr32e32_32) {
        out("readd(cpu->memory, ");
        out(getEaa32(op));
        out(")");
    } else {
        kpanic("gen3bc");
    }    
    out("; if (tmp32==0) {addFlag(ZF);} else {tmp32_2 = 0; while ((tmp32 & 0x01)==0) { tmp32_2++; tmp32>>=1; } removeFlag(ZF); ");
    out(r32(op->r1));
    out("= tmp32_2;} CYCLES(6);");
}

void OPCALL bsrr32r32(struct CPU* cpu, struct Op* op);
void OPCALL bsrr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL bsrr32e32_32(struct CPU* cpu, struct Op* op);
void gen3bd(struct Op* op) {
    out("tmp32 = ");
    if (op->func == bsrr32r32) {                
        out(r32(op->r2));        
    } else if (op->func == bsrr32e32_16) {        
        out("readd(cpu->memory, ");
        out(getEaa16(op));
        out(")");
    } else if (op->func == bsrr32e32_32) {
        out("readd(cpu->memory, ");
        out(getEaa32(op));
        out(")");
    } else {
        kpanic("gen3bd");
    }    
    out("; if (tmp32==0) {addFlag(ZF);} else {tmp32_2 = 31; while ((tmp32 & 0x80000000)==0) { tmp32_2--; tmp32<<=1; } removeFlag(ZF); ");
    out(r32(op->r1));
    out(" = tmp32_2; } cpu->lazyFlags = FLAGS_NONE; CYCLES(7);");
    currentLazyFlags = sFLAGS_NONE;
}

void OPCALL movsx8r16r16(struct CPU* cpu, struct Op* op);
void OPCALL movsx8r16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL movsx8r16e16_32(struct CPU* cpu, struct Op* op);
void gen1be(struct Op* op) {

    if (op->func == movsx8r16r16) {        
        out(r16(op->r1));
        out(" = (S8)");
        out(r8(op->r2));
        out("; CYCLES(3);");	      
    } else if (op->func == movsx8r16e16_16) {        
        out(r16(op->r1));
        out(" = (S8)readb(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(3);");	      
    } else if (op->func == movsx8r16e16_32) {
        out(r16(op->r1));
        out(" = (S8)readb(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(3);");	      
    } else {
        kpanic("gen1be");
    }
}

void OPCALL movsx8r32r32(struct CPU* cpu, struct Op* op);
void OPCALL movsx8r32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL movsx8r32e32_32(struct CPU* cpu, struct Op* op);
void gen3be(struct Op* op) {
    if (op->func == movsx8r32r32) {        
        out(r32(op->r1));
        out(" = (S8)");
        out(r8(op->r2));
        out("; CYCLES(3);");	      
    } else if (op->func == movsx8r32e32_16) {        
        out(r32(op->r1));
        out(" = (S8)readb(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(3);");	      
    } else if (op->func == movsx8r32e32_32) {
        out(r32(op->r1));
        out(" = (S8)readb(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(3);");	      
    } else {
        kpanic("gen3be");
    }
}

void OPCALL movsx16r32r32(struct CPU* cpu, struct Op* op);
void OPCALL movsx16r32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL movsx16r32e32_32(struct CPU* cpu, struct Op* op);
void gen3bf(struct Op* op) {
    if (op->func == movsx16r32r32) {        
        out(r32(op->r1));
        out(" = (S16)");
        out(r16(op->r2));
        out("; CYCLES(3);");	      
    } else if (op->func == movsx16r32e32_16) {        
        out(r32(op->r1));
        out(" = (S16)readw(cpu->memory, ");
        out(getEaa16(op));
        out("); CYCLES(3);");	      
    } else if (op->func == movsx16r32e32_32) {
        out(r32(op->r1));
        out(" = (S16)readw(cpu->memory, ");
        out(getEaa32(op));
        out("); CYCLES(3);");	      
    } else {
        kpanic("gen3bf");
    }
}

void OPCALL xadd32r32r32(struct CPU* cpu, struct Op* op);
void OPCALL xadd32r32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL xadd32r32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL xadd32r32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xadd32r32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xadd32r32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen3c1(struct Op* op) {
    if (op->func == xadd32r32r32) {        
        out("cpu->src.u32 = ");
        out(r32(op->r1));
        out("; cpu->dst.u32 = ");
        out(r32(op->r2));
        out("; cpu->result.u32 = cpu->dst.u32 + cpu->src.u32; cpu->lazyFlags = FLAGS_ADD32; ");
        currentLazyFlags = sFLAGS_ADD32;
	    out(r32(op->r1));
        out(" = cpu->dst.u32; ");
        out(r32(op->r2));
        out(" =  cpu->result.u32; CYCLES(3);");
    } else if (op->func == xadd32r32r32_noflags) {        
      out("tmp32 = ");
      out(r32(op->r2));
      out("; ");
      out(r32(op->r2));
      out(" = ");
      out(r32(op->r1));
      out(" + ");
      out(r32(op->r2));
      out("; ");
      out(r32(op->r1));
      out(" = tmp32; CYCLES(3);");
    } else if (op->func == xadd32r32e32_32) {
        out("eaa = ");
        out(getEaa32(op));
        out("; cpu->src.u32 = ");
        out(r32(op->r1));
        out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32 = cpu->dst.u32 + cpu->src.u32; cpu->lazyFlags = FLAGS_ADD32; ");
        currentLazyFlags = sFLAGS_ADD32;
        out(r32(op->r1));
	    out(" = cpu->dst.u32;  writed(cpu->memory, eaa, cpu->result.u32); CYCLES(4);");
    } else if (op->func == xadd32r32e32_32_noflags) {        
        out("eaa = ");
        out(getEaa32(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, tmp32 + ");
        out(r32(op->r1));
        out("); ");
        out(r32(op->r1));
        out(" = tmp32; CYCLES(4);");
    } else if (op->func == xadd32r32e32_16) {
        out("eaa = ");
        out(getEaa16(op));
        out("; cpu->src.u32 = ");
        out(r32(op->r1));
        out("; cpu->dst.u32 = readd(cpu->memory, eaa); cpu->result.u32 = cpu->dst.u32 + cpu->src.u32; cpu->lazyFlags = FLAGS_ADD32; ");
        currentLazyFlags = sFLAGS_ADD32;
        out(r32(op->r1));
	    out(" = cpu->dst.u32;  writed(cpu->memory, eaa, cpu->result.u32); CYCLES(4);");
    } else if (op->func == xadd32r32e32_16_noflags) {        
        out("eaa = ");
        out(getEaa16(op));
        out("; tmp32 = readd(cpu->memory, eaa); writed(cpu->memory, eaa, tmp32 + ");
        out(r32(op->r1));
        out("); ");
        out(r32(op->r1));
        out(" = tmp32; CYCLES(4);");
    } else {
        kpanic("gen3c1");
    }
}

void gen3c8(struct Op* op) {
    out("tmp32 = ");
    out(r32(op->r1));
    out("; ");
    out(r32(op->r1));
	out(" = (((tmp32 & 0xff000000) >> 24) | ((tmp32 & 0x00ff0000) >>  8) | ((tmp32 & 0x0000ff00) <<  8) | ((tmp32 & 0x000000ff) << 24)); CYCLES(1);");
}

void gen400(struct Op* op) {
    struct Op* firstOp;

    currentGenBlock = currentGenBlock->block1;
    firstOp = currentGenBlock->ops;    
    currentGenBlock->ops = currentGenBlock->ops->next;
    firstOp->next = 0;
    freeOp(firstOp);

    jit(genCPU, currentGenBlock);
    writeOps(currentGenBlock->ops);
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
	gen0d8, gen0d9, gen0da, gen0db, gen0dc, gen0dd, gen0de, gen0df,
	gen0e0, gen0e1, gen0e2, gen0e3, 0, 0, 0, 0,
	gen0e8, gen0e9, 0, gen0e9, 0, 0, 0, 0,
	0, 0, 0, 0, 0, gen0f5, gen0f6, gen0f7,
	gen0f8, gen0f9, 0, 0, gen0fc, gen0fd, gen0fe, gen0ff,
    // 100
    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, gen131, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	gen140, gen141, gen142, gen143, gen144, gen145, gen146, gen147,
	gen148, gen149, gen14a, gen14b, gen14c, gen14d, gen14e, gen14f,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    // 180
    gen070, gen071, gen072, gen073, gen074, gen075, gen076, gen077,
	gen078, gen079, gen07a, gen07b, gen07c, gen07d, gen07e, gen07f,
	gen190, gen191, gen192, gen193, gen194, gen195, gen196, gen197,
	gen198, gen199, gen19a, gen19b, gen19c, gen19d, gen19e, gen19f,
	gen1a0, gen1a1, gen1a2, gen1a3, gen1a4, gen1a5, 0, 0,
	gen1a8, gen1a9, 0, gen1ab, gen1ac, gen1ad, 0, gen1af,
	0, gen1b1, 0, 0, 0, 0, gen1b6, 0,
	0, 0, 0, 0, 0, gen1bd, gen1be, 0,
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
	0, gen2c9, 0, 0, 0, gen0cd, 0, 0,
	gen0c0, gen2c1, gen0d2, gen2d3, gen0d4, gen0d5, gen0d6, gen0d7,
	gen0d8, gen0d9, gen0da, gen0db, gen0dc, gen0dd, gen0de, gen0df,
	gen0e0, gen0e1, gen0e2, gen0e3, 0, 0, 0, 0,
	gen2e8, gen0e9, 0, gen0e9, 0, 0, 0, 0,
	0, 0, 0, 0, 0, gen0f5, gen0f6, gen2f7,
	gen0f8, gen0f9, 0, 0, gen0fc, gen0fd, gen0fe, gen2ff,
    // 300
    0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, gen131, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	gen340, gen341, gen342, gen343, gen344, gen345, gen346, gen347,
	gen348, gen349, gen34a, gen34b, gen34c, gen34d, gen34e, gen34f,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    // 380
    gen070, gen071, gen072, gen073, gen074, gen075, gen076, gen077,
	gen078, gen079, gen07a, gen07b, gen07c, gen07d, gen07e, gen07f,
	gen190, gen191, gen192, gen193, gen194, gen195, gen196, gen197,
	gen198, gen199, gen19a, gen19b, gen19c, gen19d, gen19e, gen19f,
	gen3a0, gen3a1, gen1a2, gen3a3, gen3a4, gen3a5, 0, 0,
	gen3a8, gen3a9, 0, gen3ab, gen3ac, gen3ad, 0, gen3af,
	0, gen3b1, 0, 0, 0, 0, gen3b6, gen3b7,
	0, 0, gen3ba, gen3bb, gen3bc, gen3bd, gen3be, gen3bf,
	0, gen3c1, 0, 0, 0, 0, 0, 0,
	gen3c8, gen3c8, gen3c8, gen3c8, gen3c8, gen3c8, gen3c8, gen3c8,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
    gen400,
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

void writeOps(struct Op* op) {
    char tmp[16];

     while (op) {
        out("    ");
        srcgen[op->inst](op);
        // the last op is responsible for handling the eip adjustment
        if (op->next) {
            out(" cpu->eip.u32+=");
            itoa(op->eipCount, tmp, 10);
            out(tmp);    
            out(";\n");
        } 
        op = op->next;
    }
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

    genCPU = cpu;
    currentGenBlock = block;
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
        OUT_DEFINE(S8);
        OUT_DEFINE(U16);
        OUT_DEFINE(S16);
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
        OUT_DEFINE(FMASK_ALL);
        out("#define setCF(cpu, b) if (b) cpu->flags|=CF; else cpu->flags&=~CF\n");
        out("#define CYCLES(x) cpu->blockCounter += x; cpu->blockInstructionCount++\n");

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
        out("#define DF		0x00000400\n");
        out("#define OF		0x00000800\n");

        out("struct Reg {union {U32 u32;union {union {U16 u16;struct {U8 u8;U8 h8;};};U16 h16;};};};\n");
        out("struct FPU_Reg {union {double d;U64 l;};};\n");
        out("struct FPU {struct FPU_Reg regs[9];U32 tags[9];U32 cw;U32 cw_mask_all;U32 sw;U32 top;U32 round;};\n");
        out("struct user_desc {U32  entry_number;U32 base_addr;U32  limit;union {struct {U32  seg_32bit:1;U32  contents:2;U32  read_exec_only:1;U32  limit_in_pages:1;U32  seg_not_present:1;U32  useable:1;};U32 flags;};};");
        out("struct CPU {struct Reg reg[9];U8* reg8[8]; U32 segAddress[6];U32 segValue[7];U32 flags;struct Reg eip;struct Memory* memory;struct KThread* thread;struct Reg src;struct Reg dst;struct Reg dst2;struct Reg result;struct LazyFlags* lazyFlags;int df;U32 oldcf;U32 big;struct FPU fpu;struct Block* nextBlock;struct Block* currentBlock;U64 timeStampCounter;U32 blockCounter;U32 blockInstructionCount;BOOL log;U32 cpl;U32 stackMask;U32 stackNotMask;struct user_desc* ldt;};\n");
        out("struct LazyFlags {U32 (*getCF)(struct CPU* cpu);U32 (*getOF)(struct CPU* cpu);U32 (*getAF)(struct CPU* cpu);U32 (*getZF)(struct CPU* cpu);U32 (*getSF)(struct CPU* cpu);U32 (*getPF)(struct CPU* cpu);};\n");
        out("U8 readb(struct Memory* memory, U32 address);\n");
        out("void writeb(struct Memory* memory, U32 address, U8 value);\n");
        out("U16 readw(struct Memory* memory, U32 address);\n");
        out("void writew(struct Memory* memory, U32 address, U16 value);\n");
        out("U32 readd(struct Memory* memory, U32 address);\n");
        out("void writed(struct Memory* memory, U32 address, U32 value);\n");
        out("U64 readq(struct Memory* memory, U32 address);\n");
        out("void writeq(struct Memory* memory, U32 address, U64 value);\n");
        out("struct Block* getBlock(struct CPU* cpu);\n");
        out("struct Block* getBlock1(struct CPU* cpu);\n");
        out("struct Block* getBlock2(struct CPU* cpu);\n");
        out("void push16(struct CPU* cpu, U16 value);\n");
        out("void push32(struct CPU* cpu, U32 value);\n");
        out("U32 peek32(struct CPU* cpu, U32 index);\n");
        out("U16 pop16(struct CPU* cpu);\n");
        out("U32 pop32(struct CPU* cpu);\n");
        out("void fillFlagsNoCFOF(struct CPU* cpu);\n");
        out("void fillFlagsNoCF(struct CPU* cpu);\n");
        out("void fillFlags(struct CPU* cpu);\n");
        out("void setFlags(struct CPU* cpu, U32 word, U32 mask);\n");
        out("void cpu_enter16(struct CPU* cpu, U32 bytes, U32 level);\n");
        out("void cpu_enter32(struct CPU* cpu, U32 bytes, U32 level);\n");

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
        out("void movsw32_r(struct CPU* cpu, U32 base);\n");
        out("void movsw16_r(struct CPU* cpu, U32 base);\n");
        out("void movsd32_r(struct CPU* cpu, U32 base);\n");
        out("void movsd16_r(struct CPU* cpu, U32 base);\n");
        out("void cmpsb32_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsb16_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsw32_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsw16_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsd32_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void cmpsd16_r(struct CPU* cpu, U32 rep_zero, U32 base);\n");
        out("void stosb32_r(struct CPU* cpu);\n");
        out("void stosb16_r(struct CPU* cpu);\n");
        out("void stosw32_r(struct CPU* cpu);\n");
        out("void stosw16_r(struct CPU* cpu);\n");
        out("void stosd32_r(struct CPU* cpu);\n");
        out("void stosd16_r(struct CPU* cpu);\n");
        out("void lodsb32_r(struct CPU* cpu, U32 base);\n");
        out("void lodsb16_r(struct CPU* cpu, U32 base);\n");
        out("void lodsw32_r(struct CPU* cpu, U32 base);\n");
        out("void lodsw16_r(struct CPU* cpu, U32 base);\n");
        out("void lodsd32_r(struct CPU* cpu, U32 base);\n");
        out("void lodsd16_r(struct CPU* cpu, U32 base);\n");
        out("void scasb32_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasb16_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasw32_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasw16_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasd32_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void scasd16_r(struct CPU* cpu, U32 rep_zero);\n");
        out("void FPU_FCOM(struct FPU* fpu, int st, int other);\n");
        out("void FPU_FUCOM(struct FPU* fpu, int st, int other);\n");
        out("void FPU_FABS(struct FPU* fpu);\n");
        out("void FPU_FXAM(struct FPU* fpu);\n");
        out("void FPU_F2XM1(struct FPU* fpu);\n");
        out("void FPU_FYL2X(struct FPU* fpu);\n");
        out("void FPU_FPTAN(struct FPU* fpu);\n");
        out("void FPU_FPATAN(struct FPU* fpu);\n");
        out("void FPU_FXTRACT(struct FPU* fpu);\n");
        out("void FPU_FPREM1(struct FPU* fpu);\n");
        out("void FPU_FPREM(struct FPU* fpu);\n");
        out("void FPU_FYL2XP1(struct FPU* fpu);\n");
        out("void FPU_FSQRT(struct FPU* fpu);\n");
        out("void FPU_FSINCOS(struct FPU* fpu);\n");
        out("void FPU_FRNDINT(struct FPU* fpu);\n");
        out("void FPU_FSCALE(struct FPU* fpu);\n");
        out("void FPU_FSIN(struct FPU* fpu);\n");
        out("void FPU_FCOS(struct FPU* fpu);\n");
        out("void FPU_FLDENV(struct CPU* cpu, int addr);\n");
        out("void FPU_SetCW(struct FPU* fpu, U16 word);\n");
        out("void FPU_FSTENV(struct CPU* cpu, int addr);\n");        
        out("void FPU_FINIT(struct FPU* fpu);\n");
        out("void FPU_FCOMI(struct CPU* cpu, int st, int other);\n");
        out("void FPU_FST_I32(struct CPU* cpu, int addr);\n");
        out("double FPU_FLD80(U64 eind, U32 begin);\n");
        out("void FPU_ST80(struct CPU* cpu, int addr, int reg);\n");
        out("void FPU_FRSTOR(struct CPU* cpu, int addr);\n");
        out("void FPU_FSAVE(struct CPU* cpu, int addr);\n");
        out("void FPU_FST_I16(struct CPU* cpu, int addr);\n");
        out("void FBLD_PACKED_BCD(struct CPU* cpu, U32 address);\n");
        out("void FPU_FBST(struct CPU* cpu, int addr);\n");
        out("void FPU_FST_I64(struct CPU* cpu, int addr);\n");
        out("void div8(struct CPU* cpu, U8 src);\n");
        out("void idiv8(struct CPU* cpu, S8 src);\n");
        out("void div16(struct CPU* cpu, U16 src);\n");
        out("void idiv16(struct CPU* cpu, S16 src);\n");
        out("void div32(struct CPU* cpu, U32 src);\n");
        out("void idiv32(struct CPU* cpu, S32 src);\n");
        out("void syscall(struct CPU* cpu, U32 eipCount);\n");
        out("void cpuid(struct CPU* cpu);\n");
        out("typedef void (*Int99Callback)(struct CPU* cpu);\n");
        out("extern Int99Callback* int99Callback;\n");
        out("#define TAG_Valid 0\n");
        out("#define TAG_Zero 1\n");
        out("#define TAG_Empty 3\n");
        out("#define FPU_FPOP(cpu) cpu->fpu.tags[cpu->fpu.top] = TAG_Empty; cpu->fpu.top = ((cpu->fpu.top + 1) & 7)\n");
        out("#define STV(fpu, i) (((fpu)->top + (i)) & 7)\n");
        out("#define FPU_PREP_PUSH(cpu) cpu->fpu.top = (cpu->fpu.top - 1) & 7; cpu->fpu.tags[cpu->fpu.top] = TAG_Valid\n");
        out("#define FPU_SET_TOP(fpu, val) (fpu)->sw &= ~0x3800; (fpu)->sw |= (val & 7) << 11\n");

        out("static U32 eaa;\n");
        out("static U8 tmp8;\n");
        out("static U16 tmp16;\n");
        out("static U32 tmp32;\n");        
        out("static U32 tmp32_2;\n");
        out("static S32 tmps32;\n");
        out("static U64 tmp64;\n\n");
        out("static S64 tmps64;\n\n");
        out("static double tmpd;\n\n");

        out("struct F2I {union {float f;U32 i;};};\n");
        out("struct F2I f2i;");
        out("struct D2L {union {double d;U64 l;};};\n");
        out("struct D2L d2l;");
    }
    sprintf(tmp, "// 0x%.8x CRC=%.08X %s at 0x%.8x\n", eip, crc, getModuleName(cpu, eip), getModuleEip(cpu, eip));
    out(tmp);
    sprintf(name, "generated%X", compiledBlockCount);
    out("void OPCALL ");
    out(name);
    out("(struct CPU* cpu, struct Op* op) {\n");    
    writeOps(op);
    out("\n");
    out("}\n\n");    

    {
        char* bytes = (char*)kalloc(opPos);
        memcpy(bytes, ops, opPos);
        addBlock(name, crc, bytes, opPos);
    }
}
#endif
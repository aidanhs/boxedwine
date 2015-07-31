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
    out("instruction_daa(cpu);\n");
}

void gen02f(struct Op* op) {
    out("instruction_das(cpu);\n");
}

void gen037(struct Op* op) {
    out("instruction_aaa(cpu);\n");
}

void gen03f(struct Op* op) {
    out("instruction_aas(cpu);\n");
}

void OPCALL inc16_reg(struct CPU* cpu, struct Op* op);
void OPCALL inc16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen040(struct Op* op) {
    if (op->func==inc16_reg) {
        out("cpu->oldcf=getCF(cpu); cpu->dst.u16 = ");
        out(r16(op->r1));
        out("; cpu->result.u16=cpu->dst.u16 + 1; cpu->lazyFlags = FLAGS_INC16; ");
        out(r16(op->r1));
        out(" = cpu->result.u16;\n");
    } else if (op->func==inc16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" + 1;\n");
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
        out(" = cpu->result.u16;\n");
    } else if (op->func==inc16_reg_noflags) {
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r1));
        out(" - 1;\n");
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
        out(" = cpu->result.u32;\n");
    } else if (op->func==inc32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" + 1;\n");
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
        out(" = cpu->result.u32;\n");
    } else if (op->func==inc32_reg_noflags) {
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r1));
        out(" - 1;\n");
    }
}

void gen050(struct Op* op) {
    out("push16(cpu, ");
    out(r16(op->r1));
    out(");\n");
}

void gen058(struct Op* op) {
    out(r16(op->r1));
    out(" = pop16(cpu);\n");
}

void gen250(struct Op* op) {
    out("push32(cpu, ");
    out(r32(op->r1));
    out(");\n");
}

void gen258(struct Op* op) {
    out(r32(op->r1));
    out(" = pop32(cpu);\n");
}

void gen068(struct Op* op) {
    char tmp[16];

    out("push16(cpu, 0x");
    itoa(op->data1, tmp, 16);
    out(");");
}

void gen268(struct Op* op) {
    char tmp[16];

    out("push32(cpu, 0x");
    itoa(op->data1, tmp, 16);
    out(");");
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
    out(" = tmp32;\n");
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
        out(");\n");
        return;
    } else if (op->func == dimulcr32e32_16_noflags) {
        out(r32(op->r1));
        out(" = (S32)readd(cpu->memory, ");
        out(getEaa16(op));
        out(") * ((S32)0x");
        out(tmp);
        out(");\n");
        return;
    } else if (op->func == dimulcr32e32_32_noflags) {
        out(r32(op->r1));
        out(" = (S32)readd(cpu->memory, ");
        out(getEaa32(op));
        out(") * ((S32)0x");
        out(tmp);
        out(");\n");
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
    out(" = (S32)tmp64;\n");
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
    out(";cpu->nextBlock = getBlock1(cpu);}\n");
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

void generateSource(struct Block* block) {
    struct Op* op = block->ops;
    char tmp[32];

    if (!fpGenSrc) {
        fpGenSrc = fopen("gen.c", "w");
        out("static U32 eaa\n\n");
        out("static U32 tmp32\n\n");
        out("static U64 tmp64\n\n");
    }
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
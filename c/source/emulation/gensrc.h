void genArithRR(const char* op, const char* flags, const char* bits, const char* r1, const char* r2, U32 useResult, U32 useCF, const char* cycles) {
    if (useCF)
        out("cpu->oldcf = getCF(cpu);");
    out("cpu->dst.u");
    out(bits);
    out(" = ");
    out(r1);
    out("; cpu->src.u");
    out(bits);
    out(" = ");
    out(r2);
    out("; cpu->result.u");
    out(bits);
    out(" = cpu->dst.u");
    out(bits);
    out(" ");
    out(op);
    out(" cpu->src.u");
    out(bits);
    if (useCF) {
        out(" ");
        out(op);
        out(" ");
        out("cpu->oldcf");
    }
    out("; cpu->lazyFlags = ");
    out(flags);
    if (useResult) {
        out(";");
        out(r1);
        out(" = cpu->result.u");
        out(bits);        
    }
    out(";CYCLES(");
    out(cycles);
    out(");");
}

void genArithRR_noflags(const char* op, const char* bits, const char* r1, const char* r2, U32 useCF, const char* cycles) {
    out(r1);
    out(" = ");
    out(r1);
    out(" ");
    out(op);
    out(" ");
    out(r2);
    if (useCF) {
        out(" ");
        out(op);
        out(" ");
        out("getCF(cpu)");
    }
    out(";CYCLES(");
    out(cycles);
    out(");");
}

void genArithER(const char* op, const char* flags, const char* bits, const char* address, const char* memWidth, const char* r1, U32 useResult, U32 useCF, const char* cycles) {
    if (useCF)
        out("cpu->oldcf = getCF(cpu);");
    out("eaa = ");
    out(address);
    out("; cpu->dst.u");
    out(bits);
    out(" = read");
    out(memWidth);
    out("(cpu->memory, eaa); cpu->src.u");
    out(bits);
    out(" = ");
    out(r1);
    out("; cpu->result.u");
    out(bits);
    out(" = cpu->dst.u");
    out(bits);
    out(" ");
    out(op);
    out(" cpu->src.u");
    out(bits);
    if (useCF) {
        out(" ");
        out(op);
        out(" ");
        out("cpu->oldcf");
    }
    out("; cpu->lazyFlags = ");
    out(flags);
    if (useResult) {
        out("; write");
        out(memWidth);
        out("(cpu->memory, eaa,  cpu->result.u");
        out(bits);
        out(")");
    }
    out(";CYCLES(");
    out(cycles);
    out(");");
}

void genArithER_noflags(const char* op, const char* bits, const char* address, const char* memWidth, const char* r1, U32 useCF, const char* cycles) {
    out("eaa = ");
    out(address);
    out("; write");
    out(memWidth);
    out("(cpu->memory, eaa, read");
    out(memWidth);
    out("(cpu->memory, eaa) ");
    out(op);
    out(" ");
    out(r1);
    if (useCF) {
        out(" ");
        out(op);
        out(" ");
        out("getCF(cpu)");
    }
    out(");CYCLES(");
    out(cycles);
    out(");");
}

void genArithRE(const char* op, const char* flags, const char* bits, const char* address, const char* memWidth, const char* r1, U32 useResult, U32 useCF, const char* cycles) {
    if (useCF)
        out("cpu->oldcf = getCF(cpu);");
    out("cpu->dst.u");
    out(bits);
    out(" = ");
    out(r1);
    out("; cpu->src.u");
    out(bits);
    out(" = read");
    out(memWidth);
    out("(cpu->memory, ");
    out(address);
    out("); cpu->result.u");
    out(bits);
    out(" = cpu->dst.u");
    out(bits);
    out(" ");
    out(op);
    out(" cpu->src.u");
    out(bits);
    if (useCF) {
        out(" ");
        out(op);
        out(" ");
        out("cpu->oldcf");
    }
    out("; cpu->lazyFlags = ");
    out(flags);
    if (useResult) {
        out("; ");
        out(r1);
        out(" = cpu->result.u");
        out(bits);
    }
    out(";CYCLES(");
    out(cycles);
    out(");");
}

void genArithRE_noflags(const char* op, const char* bits, const char* address, const char* memWidth, const char* r1, U32 useCF, const char* cycles) {
    out(r1);
    out(" = ");
    out(r1);
    out(" ");
    out(op);
    out(" read");
    out(memWidth);
    out("(cpu->memory, ");
    out(address);
    out(")");
    if (useCF) {
        out(" ");
        out(op);
        out(" ");
        out("getCF(cpu)");
    }
    out(";CYCLES(");
    out(cycles);
    out(");");
}

void genArithR(const char* op, const char* flags, const char* bits, const char* r1, unsigned int value, U32 useResult, U32 useCF, const char* cycles) {
    char tmp[16];

    if (useCF)
        out("cpu->oldcf = getCF(cpu);");
    out("cpu->dst.u");
    out(bits);
    out(" = ");
    out(r1);
    out("; cpu->src.u");
    out(bits);
    out(" = 0x");
    itoa(value, tmp, 16);
    out(tmp);
    out("; cpu->result.u");
    out(bits);
    out(" = cpu->dst.u");
    out(bits);
    out(" ");
    out(op);
    out(" cpu->src.u");
    out(bits);
    if (useCF) {
        out(" ");
        out(op);
        out(" ");
        out("cpu->oldcf");
    }
    out("; cpu->lazyFlags = ");
    out(flags);
    if (useResult) {
        out("; ");
        out(r1);
        out(" = cpu->result.u");
        out(bits);
    }
    out(";CYCLES(");
    out(cycles);
    out(");");
}

void genArithR_noflags(const char* op, const char* bits, const char* r1, unsigned int value, U32 useCF, const char* cycles) {
    char tmp[16];

    out(r1);
    out(" = ");
    out(r1);
    out(" ");
    out(op);
    out(" 0x");
    itoa(value, tmp, 16);
    out(tmp);
    if (useCF) {
        out(" ");
        out(op);
        out(" ");
        out("getCF(cpu)");
    }
    out(";CYCLES(");
    out(cycles);
    out(");");
}

void genArithE(const char* op, const char* flags, const char* bits, const char* address, const char* memWidth, unsigned int value, U32 useResult, U32 useCF, const char* cycles) {
    char tmp[16];

    if (useCF)
        out("cpu->oldcf = getCF(cpu);");
    out("eaa = ");
    out(address);
    out("; cpu->dst.u");
    out(bits);
    out(" = read");
    out(memWidth);
    out("(cpu->memory, eaa); cpu->src.u");
    out(bits);
    out(" = 0x");
    itoa(value, tmp, 16);
    out(tmp);
    out("; cpu->result.u");
    out(bits);
    out(" = cpu->dst.u");
    out(bits);
    out(" ");
    out(op);
    out(" cpu->src.u");
    out(bits);
    if (useCF) {
        out(" ");
        out(op);
        out(" ");
        out("cpu->oldcf");
    }
    out("; cpu->lazyFlags = ");
    out(flags);
    if (useResult) {
        out("; write");
        out(memWidth);
        out("(cpu->memory, eaa,  cpu->result.u");
        out(bits);
        out (")");
    }
    out(";CYCLES(");
    out(cycles);
    out(");");
}

void genArithE_noflags(const char* op, const char* bits, const char* address, const char* memWidth, unsigned int value, U32 useCF, const char* cycles) {
    char tmp[16];

    out("eaa = ");
    out(address);
    out("; write");
    out(memWidth);
    out("(cpu->memory, eaa, read");
    out(memWidth);
    out("(cpu->memory, eaa) ");
    out(op);
    out(" 0x");
    itoa(value, tmp, 16);
    out(tmp);
    if (useCF) {
        out(" ");
        out(op);
        out(" ");
        out("getCF(cpu)");
    }
    out(");CYCLES(");
    out(cycles);
    out(");");
}
void OPCALL addr8r8(struct CPU* cpu, struct Op* op);
void OPCALL addr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adde8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL adde8r8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adde8r8_32(struct CPU* cpu, struct Op* op);
void OPCALL adde8r8_32_noflags(struct CPU* cpu, struct Op* op);
void gen000(struct Op* op) {
    if (op->func==addr8r8) {
        genArithRR("+", "FLAGS_ADD8", "8", r8(op->r1), r8(op->r2), 1, 0,"1");
    } else if (op->func==addr8r8_noflags) {
        genArithRR_noflags("+", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==adde8r8_16) {
        genArithER("+", "FLAGS_ADD8", "8", getEaa16(op), "b", r8(op->r1), 1, 0,"3");
    } else if (op->func==adde8r8_16_noflags) {
        genArithER_noflags("+", "8", getEaa16(op), "b", r8(op->r1), 0,"3");
    } else if (op->func==adde8r8_32) {
        genArithER("+", "FLAGS_ADD8", "8", getEaa32(op), "b", r8(op->r1), 1, 0,"3");
    } else if (op->func==adde8r8_32_noflags) {
        genArithER_noflags("+", "8", getEaa32(op), "b", r8(op->r1), 0,"3");
    }
}
void OPCALL addr16r16(struct CPU* cpu, struct Op* op);
void OPCALL addr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adde16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL adde16r16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adde16r16_32(struct CPU* cpu, struct Op* op);
void OPCALL adde16r16_32_noflags(struct CPU* cpu, struct Op* op);
void gen001(struct Op* op) {
    if (op->func==addr16r16) {
        genArithRR("+", "FLAGS_ADD16", "16", r16(op->r1), r16(op->r2), 1, 0,"1");
    } else if (op->func==addr16r16_noflags) {
        genArithRR_noflags("+", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==adde16r16_16) {
        genArithER("+", "FLAGS_ADD16", "16", getEaa16(op), "w", r16(op->r1), 1, 0,"3");
    } else if (op->func==adde16r16_16_noflags) {
        genArithER_noflags("+", "16", getEaa16(op), "w", r16(op->r1), 0,"3");
    } else if (op->func==adde16r16_32) {
        genArithER("+", "FLAGS_ADD16", "16", getEaa32(op), "w", r16(op->r1), 1, 0,"3");
    } else if (op->func==adde16r16_32_noflags) {
        genArithER_noflags("+", "16", getEaa32(op), "w", r16(op->r1), 0,"3");
    }
}
void OPCALL addr32r32(struct CPU* cpu, struct Op* op);
void OPCALL addr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adde32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL adde32r32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adde32r32_32(struct CPU* cpu, struct Op* op);
void OPCALL adde32r32_32_noflags(struct CPU* cpu, struct Op* op);
void gen201(struct Op* op) {
    if (op->func==addr32r32) {
        genArithRR("+", "FLAGS_ADD32", "32", r32(op->r1), r32(op->r2), 1, 0,"1");
    } else if (op->func==addr32r32_noflags) {
        genArithRR_noflags("+", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==adde32r32_16) {
        genArithER("+", "FLAGS_ADD32", "32", getEaa16(op), "d", r32(op->r1), 1, 0,"3");
    } else if (op->func==adde32r32_16_noflags) {
        genArithER_noflags("+", "32", getEaa16(op), "d", r32(op->r1), 0,"3");
    } else if (op->func==adde32r32_32) {
        genArithER("+", "FLAGS_ADD32", "32", getEaa32(op), "d", r32(op->r1), 1, 0,"3");
    } else if (op->func==adde32r32_32_noflags) {
        genArithER_noflags("+", "32", getEaa32(op), "d", r32(op->r1), 0,"3");
    }
}
void OPCALL addr8r8(struct CPU* cpu, struct Op* op);
void OPCALL addr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL addr8e8_16(struct CPU* cpu, struct Op* op);
void OPCALL addr8e8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL addr8e8_32(struct CPU* cpu, struct Op* op);
void OPCALL addr8e8_32_noflags(struct CPU* cpu, struct Op* op);
void gen002(struct Op* op) {
    if (op->func==addr8r8) {
        genArithRR("+", "FLAGS_ADD8", "8", r8(op->r1), r8(op->r2), 1, 0,"1");
    } else if (op->func==addr8r8_noflags) {
        genArithRR_noflags("+", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==addr8e8_16) {
        genArithRE("+", "FLAGS_ADD8", "8", getEaa16(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==addr8e8_16_noflags) {
        genArithRE_noflags("+", "8", getEaa16(op), "b", r8(op->r1), 0,"2");
    } else if (op->func==addr8e8_32) {
        genArithRE("+", "FLAGS_ADD8", "8", getEaa32(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==addr8e8_32_noflags) {
        genArithRE_noflags("+", "8", getEaa32(op), "b", r8(op->r1), 0,"2");
    }
}
void OPCALL addr16r16(struct CPU* cpu, struct Op* op);
void OPCALL addr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL addr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL addr16e16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL addr16e16_32(struct CPU* cpu, struct Op* op);
void OPCALL addr16e16_32_noflags(struct CPU* cpu, struct Op* op);
void gen003(struct Op* op) {
    if (op->func==addr16r16) {
        genArithRR("+", "FLAGS_ADD16", "16", r16(op->r1), r16(op->r2), 1, 0,"1");
    } else if (op->func==addr16r16_noflags) {
        genArithRR_noflags("+", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==addr16e16_16) {
        genArithRE("+", "FLAGS_ADD16", "16", getEaa16(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==addr16e16_16_noflags) {
        genArithRE_noflags("+", "16", getEaa16(op), "w", r16(op->r1), 0,"2");
    } else if (op->func==addr16e16_32) {
        genArithRE("+", "FLAGS_ADD16", "16", getEaa32(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==addr16e16_32_noflags) {
        genArithRE_noflags("+", "16", getEaa32(op), "w", r16(op->r1), 0,"2");
    }
}
void OPCALL addr32r32(struct CPU* cpu, struct Op* op);
void OPCALL addr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL addr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL addr32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL addr32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL addr32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen203(struct Op* op) {
    if (op->func==addr32r32) {
        genArithRR("+", "FLAGS_ADD32", "32", r32(op->r1), r32(op->r2), 1, 0,"1");
    } else if (op->func==addr32r32_noflags) {
        genArithRR_noflags("+", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==addr32e32_16) {
        genArithRE("+", "FLAGS_ADD32", "32", getEaa16(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==addr32e32_16_noflags) {
        genArithRE_noflags("+", "32", getEaa16(op), "d", r32(op->r1), 0,"2");
    } else if (op->func==addr32e32_32) {
        genArithRE("+", "FLAGS_ADD32", "32", getEaa32(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==addr32e32_32_noflags) {
        genArithRE_noflags("+", "32", getEaa32(op), "d", r32(op->r1), 0,"2");
    }
}
void OPCALL add8_reg(struct CPU* cpu, struct Op* op);
void OPCALL add8_reg_noflags(struct CPU* cpu, struct Op* op);
void gen004(struct Op* op) {
    if (op->func==add8_reg) {
        genArithR("+", "FLAGS_ADD8", "8", r8(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==add8_reg_noflags) {
        genArithR_noflags("+", "8", r8(op->r1), op->data1, 0,"1");
    }
}
void OPCALL add16_reg(struct CPU* cpu, struct Op* op);
void OPCALL add16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen005(struct Op* op) {
    if (op->func==add16_reg) {
        genArithR("+", "FLAGS_ADD16", "16", r16(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==add16_reg_noflags) {
        genArithR_noflags("+", "16", r16(op->r1), op->data1, 0,"1");
    }
}
void OPCALL add32_reg(struct CPU* cpu, struct Op* op);
void OPCALL add32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen205(struct Op* op) {
    if (op->func==add32_reg) {
        genArithR("+", "FLAGS_ADD32", "32", r32(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==add32_reg_noflags) {
        genArithR_noflags("+", "32", r32(op->r1), op->data1, 0,"1");
    }
}
void OPCALL orr8r8(struct CPU* cpu, struct Op* op);
void OPCALL orr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ore8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL ore8r8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ore8r8_32(struct CPU* cpu, struct Op* op);
void OPCALL ore8r8_32_noflags(struct CPU* cpu, struct Op* op);
void gen008(struct Op* op) {
    if (op->func==orr8r8) {
        genArithRR("|", "FLAGS_OR8", "8", r8(op->r1), r8(op->r2), 1, 0,"1");
    } else if (op->func==orr8r8_noflags) {
        genArithRR_noflags("|", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==ore8r8_16) {
        genArithER("|", "FLAGS_OR8", "8", getEaa16(op), "b", r8(op->r1), 1, 0,"3");
    } else if (op->func==ore8r8_16_noflags) {
        genArithER_noflags("|", "8", getEaa16(op), "b", r8(op->r1), 0,"3");
    } else if (op->func==ore8r8_32) {
        genArithER("|", "FLAGS_OR8", "8", getEaa32(op), "b", r8(op->r1), 1, 0,"3");
    } else if (op->func==ore8r8_32_noflags) {
        genArithER_noflags("|", "8", getEaa32(op), "b", r8(op->r1), 0,"3");
    }
}
void OPCALL orr16r16(struct CPU* cpu, struct Op* op);
void OPCALL orr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ore16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL ore16r16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ore16r16_32(struct CPU* cpu, struct Op* op);
void OPCALL ore16r16_32_noflags(struct CPU* cpu, struct Op* op);
void gen009(struct Op* op) {
    if (op->func==orr16r16) {
        genArithRR("|", "FLAGS_OR16", "16", r16(op->r1), r16(op->r2), 1, 0,"1");
    } else if (op->func==orr16r16_noflags) {
        genArithRR_noflags("|", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==ore16r16_16) {
        genArithER("|", "FLAGS_OR16", "16", getEaa16(op), "w", r16(op->r1), 1, 0,"3");
    } else if (op->func==ore16r16_16_noflags) {
        genArithER_noflags("|", "16", getEaa16(op), "w", r16(op->r1), 0,"3");
    } else if (op->func==ore16r16_32) {
        genArithER("|", "FLAGS_OR16", "16", getEaa32(op), "w", r16(op->r1), 1, 0,"3");
    } else if (op->func==ore16r16_32_noflags) {
        genArithER_noflags("|", "16", getEaa32(op), "w", r16(op->r1), 0,"3");
    }
}
void OPCALL orr32r32(struct CPU* cpu, struct Op* op);
void OPCALL orr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ore32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL ore32r32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ore32r32_32(struct CPU* cpu, struct Op* op);
void OPCALL ore32r32_32_noflags(struct CPU* cpu, struct Op* op);
void gen209(struct Op* op) {
    if (op->func==orr32r32) {
        genArithRR("|", "FLAGS_OR32", "32", r32(op->r1), r32(op->r2), 1, 0,"1");
    } else if (op->func==orr32r32_noflags) {
        genArithRR_noflags("|", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==ore32r32_16) {
        genArithER("|", "FLAGS_OR32", "32", getEaa16(op), "d", r32(op->r1), 1, 0,"3");
    } else if (op->func==ore32r32_16_noflags) {
        genArithER_noflags("|", "32", getEaa16(op), "d", r32(op->r1), 0,"3");
    } else if (op->func==ore32r32_32) {
        genArithER("|", "FLAGS_OR32", "32", getEaa32(op), "d", r32(op->r1), 1, 0,"3");
    } else if (op->func==ore32r32_32_noflags) {
        genArithER_noflags("|", "32", getEaa32(op), "d", r32(op->r1), 0,"3");
    }
}
void OPCALL orr8r8(struct CPU* cpu, struct Op* op);
void OPCALL orr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL orr8e8_16(struct CPU* cpu, struct Op* op);
void OPCALL orr8e8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL orr8e8_32(struct CPU* cpu, struct Op* op);
void OPCALL orr8e8_32_noflags(struct CPU* cpu, struct Op* op);
void gen00a(struct Op* op) {
    if (op->func==orr8r8) {
        genArithRR("|", "FLAGS_OR8", "8", r8(op->r1), r8(op->r2), 1, 0,"1");
    } else if (op->func==orr8r8_noflags) {
        genArithRR_noflags("|", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==orr8e8_16) {
        genArithRE("|", "FLAGS_OR8", "8", getEaa16(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==orr8e8_16_noflags) {
        genArithRE_noflags("|", "8", getEaa16(op), "b", r8(op->r1), 0,"2");
    } else if (op->func==orr8e8_32) {
        genArithRE("|", "FLAGS_OR8", "8", getEaa32(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==orr8e8_32_noflags) {
        genArithRE_noflags("|", "8", getEaa32(op), "b", r8(op->r1), 0,"2");
    }
}
void OPCALL orr16r16(struct CPU* cpu, struct Op* op);
void OPCALL orr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL orr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL orr16e16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL orr16e16_32(struct CPU* cpu, struct Op* op);
void OPCALL orr16e16_32_noflags(struct CPU* cpu, struct Op* op);
void gen00b(struct Op* op) {
    if (op->func==orr16r16) {
        genArithRR("|", "FLAGS_OR16", "16", r16(op->r1), r16(op->r2), 1, 0,"1");
    } else if (op->func==orr16r16_noflags) {
        genArithRR_noflags("|", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==orr16e16_16) {
        genArithRE("|", "FLAGS_OR16", "16", getEaa16(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==orr16e16_16_noflags) {
        genArithRE_noflags("|", "16", getEaa16(op), "w", r16(op->r1), 0,"2");
    } else if (op->func==orr16e16_32) {
        genArithRE("|", "FLAGS_OR16", "16", getEaa32(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==orr16e16_32_noflags) {
        genArithRE_noflags("|", "16", getEaa32(op), "w", r16(op->r1), 0,"2");
    }
}
void OPCALL orr32r32(struct CPU* cpu, struct Op* op);
void OPCALL orr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL orr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL orr32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL orr32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL orr32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen20b(struct Op* op) {
    if (op->func==orr32r32) {
        genArithRR("|", "FLAGS_OR32", "32", r32(op->r1), r32(op->r2), 1, 0,"1");
    } else if (op->func==orr32r32_noflags) {
        genArithRR_noflags("|", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==orr32e32_16) {
        genArithRE("|", "FLAGS_OR32", "32", getEaa16(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==orr32e32_16_noflags) {
        genArithRE_noflags("|", "32", getEaa16(op), "d", r32(op->r1), 0,"2");
    } else if (op->func==orr32e32_32) {
        genArithRE("|", "FLAGS_OR32", "32", getEaa32(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==orr32e32_32_noflags) {
        genArithRE_noflags("|", "32", getEaa32(op), "d", r32(op->r1), 0,"2");
    }
}
void OPCALL or8_reg(struct CPU* cpu, struct Op* op);
void OPCALL or8_reg_noflags(struct CPU* cpu, struct Op* op);
void gen00c(struct Op* op) {
    if (op->func==or8_reg) {
        genArithR("|", "FLAGS_OR8", "8", r8(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==or8_reg_noflags) {
        genArithR_noflags("|", "8", r8(op->r1), op->data1, 0,"1");
    }
}
void OPCALL or16_reg(struct CPU* cpu, struct Op* op);
void OPCALL or16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen00d(struct Op* op) {
    if (op->func==or16_reg) {
        genArithR("|", "FLAGS_OR16", "16", r16(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==or16_reg_noflags) {
        genArithR_noflags("|", "16", r16(op->r1), op->data1, 0,"1");
    }
}
void OPCALL or32_reg(struct CPU* cpu, struct Op* op);
void OPCALL or32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen20d(struct Op* op) {
    if (op->func==or32_reg) {
        genArithR("|", "FLAGS_OR32", "32", r32(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==or32_reg_noflags) {
        genArithR_noflags("|", "32", r32(op->r1), op->data1, 0,"1");
    }
}
void OPCALL adcr8r8(struct CPU* cpu, struct Op* op);
void OPCALL adcr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adce8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL adce8r8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adce8r8_32(struct CPU* cpu, struct Op* op);
void OPCALL adce8r8_32_noflags(struct CPU* cpu, struct Op* op);
void gen010(struct Op* op) {
    if (op->func==adcr8r8) {
        genArithRR("+", "FLAGS_ADC8", "8", r8(op->r1), r8(op->r2), 1, 1,"1");
    } else if (op->func==adcr8r8_noflags) {
        genArithRR_noflags("+", "8", r8(op->r1), r8(op->r2), 1,"1");
    } else if (op->func==adce8r8_16) {
        genArithER("+", "FLAGS_ADC8", "8", getEaa16(op), "b", r8(op->r1), 1, 1,"3");
    } else if (op->func==adce8r8_16_noflags) {
        genArithER_noflags("+", "8", getEaa16(op), "b", r8(op->r1), 1,"3");
    } else if (op->func==adce8r8_32) {
        genArithER("+", "FLAGS_ADC8", "8", getEaa32(op), "b", r8(op->r1), 1, 1,"3");
    } else if (op->func==adce8r8_32_noflags) {
        genArithER_noflags("+", "8", getEaa32(op), "b", r8(op->r1), 1,"3");
    }
}
void OPCALL adcr16r16(struct CPU* cpu, struct Op* op);
void OPCALL adcr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adce16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL adce16r16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adce16r16_32(struct CPU* cpu, struct Op* op);
void OPCALL adce16r16_32_noflags(struct CPU* cpu, struct Op* op);
void gen011(struct Op* op) {
    if (op->func==adcr16r16) {
        genArithRR("+", "FLAGS_ADC16", "16", r16(op->r1), r16(op->r2), 1, 1,"1");
    } else if (op->func==adcr16r16_noflags) {
        genArithRR_noflags("+", "16", r16(op->r1), r16(op->r2), 1,"1");
    } else if (op->func==adce16r16_16) {
        genArithER("+", "FLAGS_ADC16", "16", getEaa16(op), "w", r16(op->r1), 1, 1,"3");
    } else if (op->func==adce16r16_16_noflags) {
        genArithER_noflags("+", "16", getEaa16(op), "w", r16(op->r1), 1,"3");
    } else if (op->func==adce16r16_32) {
        genArithER("+", "FLAGS_ADC16", "16", getEaa32(op), "w", r16(op->r1), 1, 1,"3");
    } else if (op->func==adce16r16_32_noflags) {
        genArithER_noflags("+", "16", getEaa32(op), "w", r16(op->r1), 1,"3");
    }
}
void OPCALL adcr32r32(struct CPU* cpu, struct Op* op);
void OPCALL adcr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adce32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL adce32r32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adce32r32_32(struct CPU* cpu, struct Op* op);
void OPCALL adce32r32_32_noflags(struct CPU* cpu, struct Op* op);
void gen211(struct Op* op) {
    if (op->func==adcr32r32) {
        genArithRR("+", "FLAGS_ADC32", "32", r32(op->r1), r32(op->r2), 1, 1,"1");
    } else if (op->func==adcr32r32_noflags) {
        genArithRR_noflags("+", "32", r32(op->r1), r32(op->r2), 1,"1");
    } else if (op->func==adce32r32_16) {
        genArithER("+", "FLAGS_ADC32", "32", getEaa16(op), "d", r32(op->r1), 1, 1,"3");
    } else if (op->func==adce32r32_16_noflags) {
        genArithER_noflags("+", "32", getEaa16(op), "d", r32(op->r1), 1,"3");
    } else if (op->func==adce32r32_32) {
        genArithER("+", "FLAGS_ADC32", "32", getEaa32(op), "d", r32(op->r1), 1, 1,"3");
    } else if (op->func==adce32r32_32_noflags) {
        genArithER_noflags("+", "32", getEaa32(op), "d", r32(op->r1), 1,"3");
    }
}
void OPCALL adcr8r8(struct CPU* cpu, struct Op* op);
void OPCALL adcr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adcr8e8_16(struct CPU* cpu, struct Op* op);
void OPCALL adcr8e8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adcr8e8_32(struct CPU* cpu, struct Op* op);
void OPCALL adcr8e8_32_noflags(struct CPU* cpu, struct Op* op);
void gen012(struct Op* op) {
    if (op->func==adcr8r8) {
        genArithRR("+", "FLAGS_ADC8", "8", r8(op->r1), r8(op->r2), 1, 1,"1");
    } else if (op->func==adcr8r8_noflags) {
        genArithRR_noflags("+", "8", r8(op->r1), r8(op->r2), 1,"1");
    } else if (op->func==adcr8e8_16) {
        genArithRE("+", "FLAGS_ADC8", "8", getEaa16(op), "b", r8(op->r1), 1, 1,"2");
    } else if (op->func==adcr8e8_16_noflags) {
        genArithRE_noflags("+", "8", getEaa16(op), "b", r8(op->r1), 1,"2");
    } else if (op->func==adcr8e8_32) {
        genArithRE("+", "FLAGS_ADC8", "8", getEaa32(op), "b", r8(op->r1), 1, 1,"2");
    } else if (op->func==adcr8e8_32_noflags) {
        genArithRE_noflags("+", "8", getEaa32(op), "b", r8(op->r1), 1,"2");
    }
}
void OPCALL adcr16r16(struct CPU* cpu, struct Op* op);
void OPCALL adcr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adcr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL adcr16e16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adcr16e16_32(struct CPU* cpu, struct Op* op);
void OPCALL adcr16e16_32_noflags(struct CPU* cpu, struct Op* op);
void gen013(struct Op* op) {
    if (op->func==adcr16r16) {
        genArithRR("+", "FLAGS_ADC16", "16", r16(op->r1), r16(op->r2), 1, 1,"1");
    } else if (op->func==adcr16r16_noflags) {
        genArithRR_noflags("+", "16", r16(op->r1), r16(op->r2), 1,"1");
    } else if (op->func==adcr16e16_16) {
        genArithRE("+", "FLAGS_ADC16", "16", getEaa16(op), "w", r16(op->r1), 1, 1,"2");
    } else if (op->func==adcr16e16_16_noflags) {
        genArithRE_noflags("+", "16", getEaa16(op), "w", r16(op->r1), 1,"2");
    } else if (op->func==adcr16e16_32) {
        genArithRE("+", "FLAGS_ADC16", "16", getEaa32(op), "w", r16(op->r1), 1, 1,"2");
    } else if (op->func==adcr16e16_32_noflags) {
        genArithRE_noflags("+", "16", getEaa32(op), "w", r16(op->r1), 1,"2");
    }
}
void OPCALL adcr32r32(struct CPU* cpu, struct Op* op);
void OPCALL adcr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adcr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL adcr32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL adcr32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL adcr32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen213(struct Op* op) {
    if (op->func==adcr32r32) {
        genArithRR("+", "FLAGS_ADC32", "32", r32(op->r1), r32(op->r2), 1, 1,"1");
    } else if (op->func==adcr32r32_noflags) {
        genArithRR_noflags("+", "32", r32(op->r1), r32(op->r2), 1,"1");
    } else if (op->func==adcr32e32_16) {
        genArithRE("+", "FLAGS_ADC32", "32", getEaa16(op), "d", r32(op->r1), 1, 1,"2");
    } else if (op->func==adcr32e32_16_noflags) {
        genArithRE_noflags("+", "32", getEaa16(op), "d", r32(op->r1), 1,"2");
    } else if (op->func==adcr32e32_32) {
        genArithRE("+", "FLAGS_ADC32", "32", getEaa32(op), "d", r32(op->r1), 1, 1,"2");
    } else if (op->func==adcr32e32_32_noflags) {
        genArithRE_noflags("+", "32", getEaa32(op), "d", r32(op->r1), 1,"2");
    }
}
void OPCALL adc8_reg(struct CPU* cpu, struct Op* op);
void OPCALL adc8_reg_noflags(struct CPU* cpu, struct Op* op);
void gen014(struct Op* op) {
    if (op->func==adc8_reg) {
        genArithR("+", "FLAGS_ADC8", "8", r8(op->r1), op->data1, 1, 1,"1");
    } else if (op->func==adc8_reg_noflags) {
        genArithR_noflags("+", "8", r8(op->r1), op->data1, 1,"1");
    }
}
void OPCALL adc16_reg(struct CPU* cpu, struct Op* op);
void OPCALL adc16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen015(struct Op* op) {
    if (op->func==adc16_reg) {
        genArithR("+", "FLAGS_ADC16", "16", r16(op->r1), op->data1, 1, 1,"1");
    } else if (op->func==adc16_reg_noflags) {
        genArithR_noflags("+", "16", r16(op->r1), op->data1, 1,"1");
    }
}
void OPCALL adc32_reg(struct CPU* cpu, struct Op* op);
void OPCALL adc32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen215(struct Op* op) {
    if (op->func==adc32_reg) {
        genArithR("+", "FLAGS_ADC32", "32", r32(op->r1), op->data1, 1, 1,"1");
    } else if (op->func==adc32_reg_noflags) {
        genArithR_noflags("+", "32", r32(op->r1), op->data1, 1,"1");
    }
}
void OPCALL sbbr8r8(struct CPU* cpu, struct Op* op);
void OPCALL sbbr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbe8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL sbbe8r8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbe8r8_32(struct CPU* cpu, struct Op* op);
void OPCALL sbbe8r8_32_noflags(struct CPU* cpu, struct Op* op);
void gen018(struct Op* op) {
    if (op->func==sbbr8r8) {
        genArithRR("-", "FLAGS_SBB8", "8", r8(op->r1), r8(op->r2), 1, 1,"1");
    } else if (op->func==sbbr8r8_noflags) {
        genArithRR_noflags("-", "8", r8(op->r1), r8(op->r2), 1,"1");
    } else if (op->func==sbbe8r8_16) {
        genArithER("-", "FLAGS_SBB8", "8", getEaa16(op), "b", r8(op->r1), 1, 1,"3");
    } else if (op->func==sbbe8r8_16_noflags) {
        genArithER_noflags("-", "8", getEaa16(op), "b", r8(op->r1), 1,"3");
    } else if (op->func==sbbe8r8_32) {
        genArithER("-", "FLAGS_SBB8", "8", getEaa32(op), "b", r8(op->r1), 1, 1,"3");
    } else if (op->func==sbbe8r8_32_noflags) {
        genArithER_noflags("-", "8", getEaa32(op), "b", r8(op->r1), 1,"3");
    }
}
void OPCALL sbbr16r16(struct CPU* cpu, struct Op* op);
void OPCALL sbbr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbe16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL sbbe16r16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbe16r16_32(struct CPU* cpu, struct Op* op);
void OPCALL sbbe16r16_32_noflags(struct CPU* cpu, struct Op* op);
void gen019(struct Op* op) {
    if (op->func==sbbr16r16) {
        genArithRR("-", "FLAGS_SBB16", "16", r16(op->r1), r16(op->r2), 1, 1,"1");
    } else if (op->func==sbbr16r16_noflags) {
        genArithRR_noflags("-", "16", r16(op->r1), r16(op->r2), 1,"1");
    } else if (op->func==sbbe16r16_16) {
        genArithER("-", "FLAGS_SBB16", "16", getEaa16(op), "w", r16(op->r1), 1, 1,"3");
    } else if (op->func==sbbe16r16_16_noflags) {
        genArithER_noflags("-", "16", getEaa16(op), "w", r16(op->r1), 1,"3");
    } else if (op->func==sbbe16r16_32) {
        genArithER("-", "FLAGS_SBB16", "16", getEaa32(op), "w", r16(op->r1), 1, 1,"3");
    } else if (op->func==sbbe16r16_32_noflags) {
        genArithER_noflags("-", "16", getEaa32(op), "w", r16(op->r1), 1,"3");
    }
}
void OPCALL sbbr32r32(struct CPU* cpu, struct Op* op);
void OPCALL sbbr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbe32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL sbbe32r32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbe32r32_32(struct CPU* cpu, struct Op* op);
void OPCALL sbbe32r32_32_noflags(struct CPU* cpu, struct Op* op);
void gen219(struct Op* op) {
    if (op->func==sbbr32r32) {
        genArithRR("-", "FLAGS_SBB32", "32", r32(op->r1), r32(op->r2), 1, 1,"1");
    } else if (op->func==sbbr32r32_noflags) {
        genArithRR_noflags("-", "32", r32(op->r1), r32(op->r2), 1,"1");
    } else if (op->func==sbbe32r32_16) {
        genArithER("-", "FLAGS_SBB32", "32", getEaa16(op), "d", r32(op->r1), 1, 1,"3");
    } else if (op->func==sbbe32r32_16_noflags) {
        genArithER_noflags("-", "32", getEaa16(op), "d", r32(op->r1), 1,"3");
    } else if (op->func==sbbe32r32_32) {
        genArithER("-", "FLAGS_SBB32", "32", getEaa32(op), "d", r32(op->r1), 1, 1,"3");
    } else if (op->func==sbbe32r32_32_noflags) {
        genArithER_noflags("-", "32", getEaa32(op), "d", r32(op->r1), 1,"3");
    }
}
void OPCALL sbbr8r8(struct CPU* cpu, struct Op* op);
void OPCALL sbbr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbr8e8_16(struct CPU* cpu, struct Op* op);
void OPCALL sbbr8e8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbr8e8_32(struct CPU* cpu, struct Op* op);
void OPCALL sbbr8e8_32_noflags(struct CPU* cpu, struct Op* op);
void gen01a(struct Op* op) {
    if (op->func==sbbr8r8) {
        genArithRR("-", "FLAGS_SBB8", "8", r8(op->r1), r8(op->r2), 1, 1,"1");
    } else if (op->func==sbbr8r8_noflags) {
        genArithRR_noflags("-", "8", r8(op->r1), r8(op->r2), 1,"1");
    } else if (op->func==sbbr8e8_16) {
        genArithRE("-", "FLAGS_SBB8", "8", getEaa16(op), "b", r8(op->r1), 1, 1,"2");
    } else if (op->func==sbbr8e8_16_noflags) {
        genArithRE_noflags("-", "8", getEaa16(op), "b", r8(op->r1), 1,"2");
    } else if (op->func==sbbr8e8_32) {
        genArithRE("-", "FLAGS_SBB8", "8", getEaa32(op), "b", r8(op->r1), 1, 1,"2");
    } else if (op->func==sbbr8e8_32_noflags) {
        genArithRE_noflags("-", "8", getEaa32(op), "b", r8(op->r1), 1,"2");
    }
}
void OPCALL sbbr16r16(struct CPU* cpu, struct Op* op);
void OPCALL sbbr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL sbbr16e16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbr16e16_32(struct CPU* cpu, struct Op* op);
void OPCALL sbbr16e16_32_noflags(struct CPU* cpu, struct Op* op);
void gen01b(struct Op* op) {
    if (op->func==sbbr16r16) {
        genArithRR("-", "FLAGS_SBB16", "16", r16(op->r1), r16(op->r2), 1, 1,"1");
    } else if (op->func==sbbr16r16_noflags) {
        genArithRR_noflags("-", "16", r16(op->r1), r16(op->r2), 1,"1");
    } else if (op->func==sbbr16e16_16) {
        genArithRE("-", "FLAGS_SBB16", "16", getEaa16(op), "w", r16(op->r1), 1, 1,"2");
    } else if (op->func==sbbr16e16_16_noflags) {
        genArithRE_noflags("-", "16", getEaa16(op), "w", r16(op->r1), 1,"2");
    } else if (op->func==sbbr16e16_32) {
        genArithRE("-", "FLAGS_SBB16", "16", getEaa32(op), "w", r16(op->r1), 1, 1,"2");
    } else if (op->func==sbbr16e16_32_noflags) {
        genArithRE_noflags("-", "16", getEaa32(op), "w", r16(op->r1), 1,"2");
    }
}
void OPCALL sbbr32r32(struct CPU* cpu, struct Op* op);
void OPCALL sbbr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL sbbr32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sbbr32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL sbbr32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen21b(struct Op* op) {
    if (op->func==sbbr32r32) {
        genArithRR("-", "FLAGS_SBB32", "32", r32(op->r1), r32(op->r2), 1, 1,"1");
    } else if (op->func==sbbr32r32_noflags) {
        genArithRR_noflags("-", "32", r32(op->r1), r32(op->r2), 1,"1");
    } else if (op->func==sbbr32e32_16) {
        genArithRE("-", "FLAGS_SBB32", "32", getEaa16(op), "d", r32(op->r1), 1, 1,"2");
    } else if (op->func==sbbr32e32_16_noflags) {
        genArithRE_noflags("-", "32", getEaa16(op), "d", r32(op->r1), 1,"2");
    } else if (op->func==sbbr32e32_32) {
        genArithRE("-", "FLAGS_SBB32", "32", getEaa32(op), "d", r32(op->r1), 1, 1,"2");
    } else if (op->func==sbbr32e32_32_noflags) {
        genArithRE_noflags("-", "32", getEaa32(op), "d", r32(op->r1), 1,"2");
    }
}
void OPCALL sbb8_reg(struct CPU* cpu, struct Op* op);
void OPCALL sbb8_reg_noflags(struct CPU* cpu, struct Op* op);
void gen01c(struct Op* op) {
    if (op->func==sbb8_reg) {
        genArithR("-", "FLAGS_SBB8", "8", r8(op->r1), op->data1, 1, 1,"1");
    } else if (op->func==sbb8_reg_noflags) {
        genArithR_noflags("-", "8", r8(op->r1), op->data1, 1,"1");
    }
}
void OPCALL sbb16_reg(struct CPU* cpu, struct Op* op);
void OPCALL sbb16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen01d(struct Op* op) {
    if (op->func==sbb16_reg) {
        genArithR("-", "FLAGS_SBB16", "16", r16(op->r1), op->data1, 1, 1,"1");
    } else if (op->func==sbb16_reg_noflags) {
        genArithR_noflags("-", "16", r16(op->r1), op->data1, 1,"1");
    }
}
void OPCALL sbb32_reg(struct CPU* cpu, struct Op* op);
void OPCALL sbb32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen21d(struct Op* op) {
    if (op->func==sbb32_reg) {
        genArithR("-", "FLAGS_SBB32", "32", r32(op->r1), op->data1, 1, 1,"1");
    } else if (op->func==sbb32_reg_noflags) {
        genArithR_noflags("-", "32", r32(op->r1), op->data1, 1,"1");
    }
}
void OPCALL andr8r8(struct CPU* cpu, struct Op* op);
void OPCALL andr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ande8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL ande8r8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ande8r8_32(struct CPU* cpu, struct Op* op);
void OPCALL ande8r8_32_noflags(struct CPU* cpu, struct Op* op);
void gen020(struct Op* op) {
    if (op->func==andr8r8) {
        genArithRR("&", "FLAGS_AND8", "8", r8(op->r1), r8(op->r2), 1, 0,"1");
    } else if (op->func==andr8r8_noflags) {
        genArithRR_noflags("&", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==ande8r8_16) {
        genArithER("&", "FLAGS_AND8", "8", getEaa16(op), "b", r8(op->r1), 1, 0,"3");
    } else if (op->func==ande8r8_16_noflags) {
        genArithER_noflags("&", "8", getEaa16(op), "b", r8(op->r1), 0,"3");
    } else if (op->func==ande8r8_32) {
        genArithER("&", "FLAGS_AND8", "8", getEaa32(op), "b", r8(op->r1), 1, 0,"3");
    } else if (op->func==ande8r8_32_noflags) {
        genArithER_noflags("&", "8", getEaa32(op), "b", r8(op->r1), 0,"3");
    }
}
void OPCALL andr16r16(struct CPU* cpu, struct Op* op);
void OPCALL andr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ande16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL ande16r16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ande16r16_32(struct CPU* cpu, struct Op* op);
void OPCALL ande16r16_32_noflags(struct CPU* cpu, struct Op* op);
void gen021(struct Op* op) {
    if (op->func==andr16r16) {
        genArithRR("&", "FLAGS_AND16", "16", r16(op->r1), r16(op->r2), 1, 0,"1");
    } else if (op->func==andr16r16_noflags) {
        genArithRR_noflags("&", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==ande16r16_16) {
        genArithER("&", "FLAGS_AND16", "16", getEaa16(op), "w", r16(op->r1), 1, 0,"3");
    } else if (op->func==ande16r16_16_noflags) {
        genArithER_noflags("&", "16", getEaa16(op), "w", r16(op->r1), 0,"3");
    } else if (op->func==ande16r16_32) {
        genArithER("&", "FLAGS_AND16", "16", getEaa32(op), "w", r16(op->r1), 1, 0,"3");
    } else if (op->func==ande16r16_32_noflags) {
        genArithER_noflags("&", "16", getEaa32(op), "w", r16(op->r1), 0,"3");
    }
}
void OPCALL andr32r32(struct CPU* cpu, struct Op* op);
void OPCALL andr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ande32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL ande32r32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL ande32r32_32(struct CPU* cpu, struct Op* op);
void OPCALL ande32r32_32_noflags(struct CPU* cpu, struct Op* op);
void gen221(struct Op* op) {
    if (op->func==andr32r32) {
        genArithRR("&", "FLAGS_AND32", "32", r32(op->r1), r32(op->r2), 1, 0,"1");
    } else if (op->func==andr32r32_noflags) {
        genArithRR_noflags("&", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==ande32r32_16) {
        genArithER("&", "FLAGS_AND32", "32", getEaa16(op), "d", r32(op->r1), 1, 0,"3");
    } else if (op->func==ande32r32_16_noflags) {
        genArithER_noflags("&", "32", getEaa16(op), "d", r32(op->r1), 0,"3");
    } else if (op->func==ande32r32_32) {
        genArithER("&", "FLAGS_AND32", "32", getEaa32(op), "d", r32(op->r1), 1, 0,"3");
    } else if (op->func==ande32r32_32_noflags) {
        genArithER_noflags("&", "32", getEaa32(op), "d", r32(op->r1), 0,"3");
    }
}
void OPCALL andr8r8(struct CPU* cpu, struct Op* op);
void OPCALL andr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL andr8e8_16(struct CPU* cpu, struct Op* op);
void OPCALL andr8e8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL andr8e8_32(struct CPU* cpu, struct Op* op);
void OPCALL andr8e8_32_noflags(struct CPU* cpu, struct Op* op);
void gen022(struct Op* op) {
    if (op->func==andr8r8) {
        genArithRR("&", "FLAGS_AND8", "8", r8(op->r1), r8(op->r2), 1, 0,"1");
    } else if (op->func==andr8r8_noflags) {
        genArithRR_noflags("&", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==andr8e8_16) {
        genArithRE("&", "FLAGS_AND8", "8", getEaa16(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==andr8e8_16_noflags) {
        genArithRE_noflags("&", "8", getEaa16(op), "b", r8(op->r1), 0,"2");
    } else if (op->func==andr8e8_32) {
        genArithRE("&", "FLAGS_AND8", "8", getEaa32(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==andr8e8_32_noflags) {
        genArithRE_noflags("&", "8", getEaa32(op), "b", r8(op->r1), 0,"2");
    }
}
void OPCALL andr16r16(struct CPU* cpu, struct Op* op);
void OPCALL andr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL andr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL andr16e16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL andr16e16_32(struct CPU* cpu, struct Op* op);
void OPCALL andr16e16_32_noflags(struct CPU* cpu, struct Op* op);
void gen023(struct Op* op) {
    if (op->func==andr16r16) {
        genArithRR("&", "FLAGS_AND16", "16", r16(op->r1), r16(op->r2), 1, 0,"1");
    } else if (op->func==andr16r16_noflags) {
        genArithRR_noflags("&", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==andr16e16_16) {
        genArithRE("&", "FLAGS_AND16", "16", getEaa16(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==andr16e16_16_noflags) {
        genArithRE_noflags("&", "16", getEaa16(op), "w", r16(op->r1), 0,"2");
    } else if (op->func==andr16e16_32) {
        genArithRE("&", "FLAGS_AND16", "16", getEaa32(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==andr16e16_32_noflags) {
        genArithRE_noflags("&", "16", getEaa32(op), "w", r16(op->r1), 0,"2");
    }
}
void OPCALL andr32r32(struct CPU* cpu, struct Op* op);
void OPCALL andr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL andr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL andr32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL andr32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL andr32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen223(struct Op* op) {
    if (op->func==andr32r32) {
        genArithRR("&", "FLAGS_AND32", "32", r32(op->r1), r32(op->r2), 1, 0,"1");
    } else if (op->func==andr32r32_noflags) {
        genArithRR_noflags("&", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==andr32e32_16) {
        genArithRE("&", "FLAGS_AND32", "32", getEaa16(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==andr32e32_16_noflags) {
        genArithRE_noflags("&", "32", getEaa16(op), "d", r32(op->r1), 0,"2");
    } else if (op->func==andr32e32_32) {
        genArithRE("&", "FLAGS_AND32", "32", getEaa32(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==andr32e32_32_noflags) {
        genArithRE_noflags("&", "32", getEaa32(op), "d", r32(op->r1), 0,"2");
    }
}
void OPCALL and8_reg(struct CPU* cpu, struct Op* op);
void OPCALL and8_reg_noflags(struct CPU* cpu, struct Op* op);
void gen024(struct Op* op) {
    if (op->func==and8_reg) {
        genArithR("&", "FLAGS_AND8", "8", r8(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==and8_reg_noflags) {
        genArithR_noflags("&", "8", r8(op->r1), op->data1, 0,"1");
    }
}
void OPCALL and16_reg(struct CPU* cpu, struct Op* op);
void OPCALL and16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen025(struct Op* op) {
    if (op->func==and16_reg) {
        genArithR("&", "FLAGS_AND16", "16", r16(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==and16_reg_noflags) {
        genArithR_noflags("&", "16", r16(op->r1), op->data1, 0,"1");
    }
}
void OPCALL and32_reg(struct CPU* cpu, struct Op* op);
void OPCALL and32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen225(struct Op* op) {
    if (op->func==and32_reg) {
        genArithR("&", "FLAGS_AND32", "32", r32(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==and32_reg_noflags) {
        genArithR_noflags("&", "32", r32(op->r1), op->data1, 0,"1");
    }
}
void OPCALL subr8r8(struct CPU* cpu, struct Op* op);
void OPCALL subr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sube8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL sube8r8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sube8r8_32(struct CPU* cpu, struct Op* op);
void OPCALL sube8r8_32_noflags(struct CPU* cpu, struct Op* op);
void gen028(struct Op* op) {
    if (op->func==subr8r8) {
        genArithRR("-", "FLAGS_SUB8", "8", r8(op->r1), r8(op->r2), 1, 0,"1");
    } else if (op->func==subr8r8_noflags) {
        genArithRR_noflags("-", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==sube8r8_16) {
        genArithER("-", "FLAGS_SUB8", "8", getEaa16(op), "b", r8(op->r1), 1, 0,"3");
    } else if (op->func==sube8r8_16_noflags) {
        genArithER_noflags("-", "8", getEaa16(op), "b", r8(op->r1), 0,"3");
    } else if (op->func==sube8r8_32) {
        genArithER("-", "FLAGS_SUB8", "8", getEaa32(op), "b", r8(op->r1), 1, 0,"3");
    } else if (op->func==sube8r8_32_noflags) {
        genArithER_noflags("-", "8", getEaa32(op), "b", r8(op->r1), 0,"3");
    }
}
void OPCALL subr16r16(struct CPU* cpu, struct Op* op);
void OPCALL subr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sube16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL sube16r16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sube16r16_32(struct CPU* cpu, struct Op* op);
void OPCALL sube16r16_32_noflags(struct CPU* cpu, struct Op* op);
void gen029(struct Op* op) {
    if (op->func==subr16r16) {
        genArithRR("-", "FLAGS_SUB16", "16", r16(op->r1), r16(op->r2), 1, 0,"1");
    } else if (op->func==subr16r16_noflags) {
        genArithRR_noflags("-", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==sube16r16_16) {
        genArithER("-", "FLAGS_SUB16", "16", getEaa16(op), "w", r16(op->r1), 1, 0,"3");
    } else if (op->func==sube16r16_16_noflags) {
        genArithER_noflags("-", "16", getEaa16(op), "w", r16(op->r1), 0,"3");
    } else if (op->func==sube16r16_32) {
        genArithER("-", "FLAGS_SUB16", "16", getEaa32(op), "w", r16(op->r1), 1, 0,"3");
    } else if (op->func==sube16r16_32_noflags) {
        genArithER_noflags("-", "16", getEaa32(op), "w", r16(op->r1), 0,"3");
    }
}
void OPCALL subr32r32(struct CPU* cpu, struct Op* op);
void OPCALL subr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sube32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL sube32r32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL sube32r32_32(struct CPU* cpu, struct Op* op);
void OPCALL sube32r32_32_noflags(struct CPU* cpu, struct Op* op);
void gen229(struct Op* op) {
    if (op->func==subr32r32) {
        genArithRR("-", "FLAGS_SUB32", "32", r32(op->r1), r32(op->r2), 1, 0,"1");
    } else if (op->func==subr32r32_noflags) {
        genArithRR_noflags("-", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==sube32r32_16) {
        genArithER("-", "FLAGS_SUB32", "32", getEaa16(op), "d", r32(op->r1), 1, 0,"3");
    } else if (op->func==sube32r32_16_noflags) {
        genArithER_noflags("-", "32", getEaa16(op), "d", r32(op->r1), 0,"3");
    } else if (op->func==sube32r32_32) {
        genArithER("-", "FLAGS_SUB32", "32", getEaa32(op), "d", r32(op->r1), 1, 0,"3");
    } else if (op->func==sube32r32_32_noflags) {
        genArithER_noflags("-", "32", getEaa32(op), "d", r32(op->r1), 0,"3");
    }
}
void OPCALL subr8r8(struct CPU* cpu, struct Op* op);
void OPCALL subr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL subr8e8_16(struct CPU* cpu, struct Op* op);
void OPCALL subr8e8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL subr8e8_32(struct CPU* cpu, struct Op* op);
void OPCALL subr8e8_32_noflags(struct CPU* cpu, struct Op* op);
void gen02a(struct Op* op) {
    if (op->func==subr8r8) {
        genArithRR("-", "FLAGS_SUB8", "8", r8(op->r1), r8(op->r2), 1, 0,"1");
    } else if (op->func==subr8r8_noflags) {
        genArithRR_noflags("-", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==subr8e8_16) {
        genArithRE("-", "FLAGS_SUB8", "8", getEaa16(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==subr8e8_16_noflags) {
        genArithRE_noflags("-", "8", getEaa16(op), "b", r8(op->r1), 0,"2");
    } else if (op->func==subr8e8_32) {
        genArithRE("-", "FLAGS_SUB8", "8", getEaa32(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==subr8e8_32_noflags) {
        genArithRE_noflags("-", "8", getEaa32(op), "b", r8(op->r1), 0,"2");
    }
}
void OPCALL subr16r16(struct CPU* cpu, struct Op* op);
void OPCALL subr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL subr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL subr16e16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL subr16e16_32(struct CPU* cpu, struct Op* op);
void OPCALL subr16e16_32_noflags(struct CPU* cpu, struct Op* op);
void gen02b(struct Op* op) {
    if (op->func==subr16r16) {
        genArithRR("-", "FLAGS_SUB16", "16", r16(op->r1), r16(op->r2), 1, 0,"1");
    } else if (op->func==subr16r16_noflags) {
        genArithRR_noflags("-", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==subr16e16_16) {
        genArithRE("-", "FLAGS_SUB16", "16", getEaa16(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==subr16e16_16_noflags) {
        genArithRE_noflags("-", "16", getEaa16(op), "w", r16(op->r1), 0,"2");
    } else if (op->func==subr16e16_32) {
        genArithRE("-", "FLAGS_SUB16", "16", getEaa32(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==subr16e16_32_noflags) {
        genArithRE_noflags("-", "16", getEaa32(op), "w", r16(op->r1), 0,"2");
    }
}
void OPCALL subr32r32(struct CPU* cpu, struct Op* op);
void OPCALL subr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL subr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL subr32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL subr32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL subr32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen22b(struct Op* op) {
    if (op->func==subr32r32) {
        genArithRR("-", "FLAGS_SUB32", "32", r32(op->r1), r32(op->r2), 1, 0,"1");
    } else if (op->func==subr32r32_noflags) {
        genArithRR_noflags("-", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==subr32e32_16) {
        genArithRE("-", "FLAGS_SUB32", "32", getEaa16(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==subr32e32_16_noflags) {
        genArithRE_noflags("-", "32", getEaa16(op), "d", r32(op->r1), 0,"2");
    } else if (op->func==subr32e32_32) {
        genArithRE("-", "FLAGS_SUB32", "32", getEaa32(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==subr32e32_32_noflags) {
        genArithRE_noflags("-", "32", getEaa32(op), "d", r32(op->r1), 0,"2");
    }
}
void OPCALL sub8_reg(struct CPU* cpu, struct Op* op);
void OPCALL sub8_reg_noflags(struct CPU* cpu, struct Op* op);
void gen02c(struct Op* op) {
    if (op->func==sub8_reg) {
        genArithR("-", "FLAGS_SUB8", "8", r8(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==sub8_reg_noflags) {
        genArithR_noflags("-", "8", r8(op->r1), op->data1, 0,"1");
    }
}
void OPCALL sub16_reg(struct CPU* cpu, struct Op* op);
void OPCALL sub16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen02d(struct Op* op) {
    if (op->func==sub16_reg) {
        genArithR("-", "FLAGS_SUB16", "16", r16(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==sub16_reg_noflags) {
        genArithR_noflags("-", "16", r16(op->r1), op->data1, 0,"1");
    }
}
void OPCALL sub32_reg(struct CPU* cpu, struct Op* op);
void OPCALL sub32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen22d(struct Op* op) {
    if (op->func==sub32_reg) {
        genArithR("-", "FLAGS_SUB32", "32", r32(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==sub32_reg_noflags) {
        genArithR_noflags("-", "32", r32(op->r1), op->data1, 0,"1");
    }
}
void OPCALL xorr8r8(struct CPU* cpu, struct Op* op);
void OPCALL xorr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xore8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL xore8r8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xore8r8_32(struct CPU* cpu, struct Op* op);
void OPCALL xore8r8_32_noflags(struct CPU* cpu, struct Op* op);
void gen030(struct Op* op) {
    if (op->func==xorr8r8) {
        genArithRR("^", "FLAGS_XOR8", "8", r8(op->r1), r8(op->r2), 1, 0,"1");
    } else if (op->func==xorr8r8_noflags) {
        genArithRR_noflags("^", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==xore8r8_16) {
        genArithER("^", "FLAGS_XOR8", "8", getEaa16(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==xore8r8_16_noflags) {
        genArithER_noflags("^", "8", getEaa16(op), "b", r8(op->r1), 0,"2");
    } else if (op->func==xore8r8_32) {
        genArithER("^", "FLAGS_XOR8", "8", getEaa32(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==xore8r8_32_noflags) {
        genArithER_noflags("^", "8", getEaa32(op), "b", r8(op->r1), 0,"2");
    }
}
void OPCALL xorr16r16(struct CPU* cpu, struct Op* op);
void OPCALL xorr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xore16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL xore16r16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xore16r16_32(struct CPU* cpu, struct Op* op);
void OPCALL xore16r16_32_noflags(struct CPU* cpu, struct Op* op);
void gen031(struct Op* op) {
    if (op->func==xorr16r16) {
        genArithRR("^", "FLAGS_XOR16", "16", r16(op->r1), r16(op->r2), 1, 0,"1");
    } else if (op->func==xorr16r16_noflags) {
        genArithRR_noflags("^", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==xore16r16_16) {
        genArithER("^", "FLAGS_XOR16", "16", getEaa16(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==xore16r16_16_noflags) {
        genArithER_noflags("^", "16", getEaa16(op), "w", r16(op->r1), 0,"2");
    } else if (op->func==xore16r16_32) {
        genArithER("^", "FLAGS_XOR16", "16", getEaa32(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==xore16r16_32_noflags) {
        genArithER_noflags("^", "16", getEaa32(op), "w", r16(op->r1), 0,"2");
    }
}
void OPCALL xorr32r32(struct CPU* cpu, struct Op* op);
void OPCALL xorr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xore32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL xore32r32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xore32r32_32(struct CPU* cpu, struct Op* op);
void OPCALL xore32r32_32_noflags(struct CPU* cpu, struct Op* op);
void gen231(struct Op* op) {
    if (op->func==xorr32r32) {
        genArithRR("^", "FLAGS_XOR32", "32", r32(op->r1), r32(op->r2), 1, 0,"1");
    } else if (op->func==xorr32r32_noflags) {
        genArithRR_noflags("^", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==xore32r32_16) {
        genArithER("^", "FLAGS_XOR32", "32", getEaa16(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==xore32r32_16_noflags) {
        genArithER_noflags("^", "32", getEaa16(op), "d", r32(op->r1), 0,"2");
    } else if (op->func==xore32r32_32) {
        genArithER("^", "FLAGS_XOR32", "32", getEaa32(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==xore32r32_32_noflags) {
        genArithER_noflags("^", "32", getEaa32(op), "d", r32(op->r1), 0,"2");
    }
}
void OPCALL xorr8r8(struct CPU* cpu, struct Op* op);
void OPCALL xorr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xorr8e8_16(struct CPU* cpu, struct Op* op);
void OPCALL xorr8e8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xorr8e8_32(struct CPU* cpu, struct Op* op);
void OPCALL xorr8e8_32_noflags(struct CPU* cpu, struct Op* op);
void gen032(struct Op* op) {
    if (op->func==xorr8r8) {
        genArithRR("^", "FLAGS_XOR8", "8", r8(op->r1), r8(op->r2), 1, 0,"1");
    } else if (op->func==xorr8r8_noflags) {
        genArithRR_noflags("^", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==xorr8e8_16) {
        genArithRE("^", "FLAGS_XOR8", "8", getEaa16(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==xorr8e8_16_noflags) {
        genArithRE_noflags("^", "8", getEaa16(op), "b", r8(op->r1), 0,"2");
    } else if (op->func==xorr8e8_32) {
        genArithRE("^", "FLAGS_XOR8", "8", getEaa32(op), "b", r8(op->r1), 1, 0,"2");
    } else if (op->func==xorr8e8_32_noflags) {
        genArithRE_noflags("^", "8", getEaa32(op), "b", r8(op->r1), 0,"2");
    }
}
void OPCALL xorr16r16(struct CPU* cpu, struct Op* op);
void OPCALL xorr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xorr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL xorr16e16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xorr16e16_32(struct CPU* cpu, struct Op* op);
void OPCALL xorr16e16_32_noflags(struct CPU* cpu, struct Op* op);
void gen033(struct Op* op) {
    if (op->func==xorr16r16) {
        genArithRR("^", "FLAGS_XOR16", "16", r16(op->r1), r16(op->r2), 1, 0,"1");
    } else if (op->func==xorr16r16_noflags) {
        genArithRR_noflags("^", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==xorr16e16_16) {
        genArithRE("^", "FLAGS_XOR16", "16", getEaa16(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==xorr16e16_16_noflags) {
        genArithRE_noflags("^", "16", getEaa16(op), "w", r16(op->r1), 0,"2");
    } else if (op->func==xorr16e16_32) {
        genArithRE("^", "FLAGS_XOR16", "16", getEaa32(op), "w", r16(op->r1), 1, 0,"2");
    } else if (op->func==xorr16e16_32_noflags) {
        genArithRE_noflags("^", "16", getEaa32(op), "w", r16(op->r1), 0,"2");
    }
}
void OPCALL xorr32r32(struct CPU* cpu, struct Op* op);
void OPCALL xorr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xorr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL xorr32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL xorr32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL xorr32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen233(struct Op* op) {
    if (op->func==xorr32r32) {
        genArithRR("^", "FLAGS_XOR32", "32", r32(op->r1), r32(op->r2), 1, 0,"1");
    } else if (op->func==xorr32r32_noflags) {
        genArithRR_noflags("^", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==xorr32e32_16) {
        genArithRE("^", "FLAGS_XOR32", "32", getEaa16(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==xorr32e32_16_noflags) {
        genArithRE_noflags("^", "32", getEaa16(op), "d", r32(op->r1), 0,"2");
    } else if (op->func==xorr32e32_32) {
        genArithRE("^", "FLAGS_XOR32", "32", getEaa32(op), "d", r32(op->r1), 1, 0,"2");
    } else if (op->func==xorr32e32_32_noflags) {
        genArithRE_noflags("^", "32", getEaa32(op), "d", r32(op->r1), 0,"2");
    }
}
void OPCALL xor8_reg(struct CPU* cpu, struct Op* op);
void OPCALL xor8_reg_noflags(struct CPU* cpu, struct Op* op);
void gen034(struct Op* op) {
    if (op->func==xor8_reg) {
        genArithR("^", "FLAGS_XOR8", "8", r8(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==xor8_reg_noflags) {
        genArithR_noflags("^", "8", r8(op->r1), op->data1, 0,"1");
    }
}
void OPCALL xor16_reg(struct CPU* cpu, struct Op* op);
void OPCALL xor16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen035(struct Op* op) {
    if (op->func==xor16_reg) {
        genArithR("^", "FLAGS_XOR16", "16", r16(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==xor16_reg_noflags) {
        genArithR_noflags("^", "16", r16(op->r1), op->data1, 0,"1");
    }
}
void OPCALL xor32_reg(struct CPU* cpu, struct Op* op);
void OPCALL xor32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen235(struct Op* op) {
    if (op->func==xor32_reg) {
        genArithR("^", "FLAGS_XOR32", "32", r32(op->r1), op->data1, 1, 0,"1");
    } else if (op->func==xor32_reg_noflags) {
        genArithR_noflags("^", "32", r32(op->r1), op->data1, 0,"1");
    }
}
void OPCALL cmpr8r8(struct CPU* cpu, struct Op* op);
void OPCALL cmpr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpe8r8_16(struct CPU* cpu, struct Op* op);
void OPCALL cmpe8r8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpe8r8_32(struct CPU* cpu, struct Op* op);
void OPCALL cmpe8r8_32_noflags(struct CPU* cpu, struct Op* op);
void gen038(struct Op* op) {
    if (op->func==cmpr8r8) {
        genArithRR("-", "FLAGS_CMP8", "8", r8(op->r1), r8(op->r2), 0, 0,"1");
    } else if (op->func==cmpr8r8_noflags) {
        genArithRR_noflags("-", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==cmpe8r8_16) {
        genArithER("-", "FLAGS_CMP8", "8", getEaa16(op), "b", r8(op->r1), 0, 0,"2");
    } else if (op->func==cmpe8r8_16_noflags) {
        genArithER_noflags("-", "8", getEaa16(op), "b", r8(op->r1), 0,"2");
    } else if (op->func==cmpe8r8_32) {
        genArithER("-", "FLAGS_CMP8", "8", getEaa32(op), "b", r8(op->r1), 0, 0,"2");
    } else if (op->func==cmpe8r8_32_noflags) {
        genArithER_noflags("-", "8", getEaa32(op), "b", r8(op->r1), 0,"2");
    }
}
void OPCALL cmpr16r16(struct CPU* cpu, struct Op* op);
void OPCALL cmpr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpe16r16_16(struct CPU* cpu, struct Op* op);
void OPCALL cmpe16r16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpe16r16_32(struct CPU* cpu, struct Op* op);
void OPCALL cmpe16r16_32_noflags(struct CPU* cpu, struct Op* op);
void gen039(struct Op* op) {
    if (op->func==cmpr16r16) {
        genArithRR("-", "FLAGS_CMP16", "16", r16(op->r1), r16(op->r2), 0, 0,"1");
    } else if (op->func==cmpr16r16_noflags) {
        genArithRR_noflags("-", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==cmpe16r16_16) {
        genArithER("-", "FLAGS_CMP16", "16", getEaa16(op), "w", r16(op->r1), 0, 0,"2");
    } else if (op->func==cmpe16r16_16_noflags) {
        genArithER_noflags("-", "16", getEaa16(op), "w", r16(op->r1), 0,"2");
    } else if (op->func==cmpe16r16_32) {
        genArithER("-", "FLAGS_CMP16", "16", getEaa32(op), "w", r16(op->r1), 0, 0,"2");
    } else if (op->func==cmpe16r16_32_noflags) {
        genArithER_noflags("-", "16", getEaa32(op), "w", r16(op->r1), 0,"2");
    }
}
void OPCALL cmpr32r32(struct CPU* cpu, struct Op* op);
void OPCALL cmpr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpe32r32_16(struct CPU* cpu, struct Op* op);
void OPCALL cmpe32r32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpe32r32_32(struct CPU* cpu, struct Op* op);
void OPCALL cmpe32r32_32_noflags(struct CPU* cpu, struct Op* op);
void gen239(struct Op* op) {
    if (op->func==cmpr32r32) {
        genArithRR("-", "FLAGS_CMP32", "32", r32(op->r1), r32(op->r2), 0, 0,"1");
    } else if (op->func==cmpr32r32_noflags) {
        genArithRR_noflags("-", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==cmpe32r32_16) {
        genArithER("-", "FLAGS_CMP32", "32", getEaa16(op), "d", r32(op->r1), 0, 0,"2");
    } else if (op->func==cmpe32r32_16_noflags) {
        genArithER_noflags("-", "32", getEaa16(op), "d", r32(op->r1), 0,"2");
    } else if (op->func==cmpe32r32_32) {
        genArithER("-", "FLAGS_CMP32", "32", getEaa32(op), "d", r32(op->r1), 0, 0,"2");
    } else if (op->func==cmpe32r32_32_noflags) {
        genArithER_noflags("-", "32", getEaa32(op), "d", r32(op->r1), 0,"2");
    }
}
void OPCALL cmpr8r8(struct CPU* cpu, struct Op* op);
void OPCALL cmpr8r8_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpr8e8_16(struct CPU* cpu, struct Op* op);
void OPCALL cmpr8e8_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpr8e8_32(struct CPU* cpu, struct Op* op);
void OPCALL cmpr8e8_32_noflags(struct CPU* cpu, struct Op* op);
void gen03a(struct Op* op) {
    if (op->func==cmpr8r8) {
        genArithRR("-", "FLAGS_CMP8", "8", r8(op->r1), r8(op->r2), 0, 0,"1");
    } else if (op->func==cmpr8r8_noflags) {
        genArithRR_noflags("-", "8", r8(op->r1), r8(op->r2), 0,"1");
    } else if (op->func==cmpr8e8_16) {
        genArithRE("-", "FLAGS_CMP8", "8", getEaa16(op), "b", r8(op->r1), 0, 0,"2");
    } else if (op->func==cmpr8e8_16_noflags) {
        genArithRE_noflags("-", "8", getEaa16(op), "b", r8(op->r1), 0,"2");
    } else if (op->func==cmpr8e8_32) {
        genArithRE("-", "FLAGS_CMP8", "8", getEaa32(op), "b", r8(op->r1), 0, 0,"2");
    } else if (op->func==cmpr8e8_32_noflags) {
        genArithRE_noflags("-", "8", getEaa32(op), "b", r8(op->r1), 0,"2");
    }
}
void OPCALL cmpr16r16(struct CPU* cpu, struct Op* op);
void OPCALL cmpr16r16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpr16e16_16(struct CPU* cpu, struct Op* op);
void OPCALL cmpr16e16_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpr16e16_32(struct CPU* cpu, struct Op* op);
void OPCALL cmpr16e16_32_noflags(struct CPU* cpu, struct Op* op);
void gen03b(struct Op* op) {
    if (op->func==cmpr16r16) {
        genArithRR("-", "FLAGS_CMP16", "16", r16(op->r1), r16(op->r2), 0, 0,"1");
    } else if (op->func==cmpr16r16_noflags) {
        genArithRR_noflags("-", "16", r16(op->r1), r16(op->r2), 0,"1");
    } else if (op->func==cmpr16e16_16) {
        genArithRE("-", "FLAGS_CMP16", "16", getEaa16(op), "w", r16(op->r1), 0, 0,"2");
    } else if (op->func==cmpr16e16_16_noflags) {
        genArithRE_noflags("-", "16", getEaa16(op), "w", r16(op->r1), 0,"2");
    } else if (op->func==cmpr16e16_32) {
        genArithRE("-", "FLAGS_CMP16", "16", getEaa32(op), "w", r16(op->r1), 0, 0,"2");
    } else if (op->func==cmpr16e16_32_noflags) {
        genArithRE_noflags("-", "16", getEaa32(op), "w", r16(op->r1), 0,"2");
    }
}
void OPCALL cmpr32r32(struct CPU* cpu, struct Op* op);
void OPCALL cmpr32r32_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpr32e32_16(struct CPU* cpu, struct Op* op);
void OPCALL cmpr32e32_16_noflags(struct CPU* cpu, struct Op* op);
void OPCALL cmpr32e32_32(struct CPU* cpu, struct Op* op);
void OPCALL cmpr32e32_32_noflags(struct CPU* cpu, struct Op* op);
void gen23b(struct Op* op) {
    if (op->func==cmpr32r32) {
        genArithRR("-", "FLAGS_CMP32", "32", r32(op->r1), r32(op->r2), 0, 0,"1");
    } else if (op->func==cmpr32r32_noflags) {
        genArithRR_noflags("-", "32", r32(op->r1), r32(op->r2), 0,"1");
    } else if (op->func==cmpr32e32_16) {
        genArithRE("-", "FLAGS_CMP32", "32", getEaa16(op), "d", r32(op->r1), 0, 0,"2");
    } else if (op->func==cmpr32e32_16_noflags) {
        genArithRE_noflags("-", "32", getEaa16(op), "d", r32(op->r1), 0,"2");
    } else if (op->func==cmpr32e32_32) {
        genArithRE("-", "FLAGS_CMP32", "32", getEaa32(op), "d", r32(op->r1), 0, 0,"2");
    } else if (op->func==cmpr32e32_32_noflags) {
        genArithRE_noflags("-", "32", getEaa32(op), "d", r32(op->r1), 0,"2");
    }
}
void OPCALL cmp8_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmp8_reg_noflags(struct CPU* cpu, struct Op* op);
void gen03c(struct Op* op) {
    if (op->func==cmp8_reg) {
        genArithR("-", "FLAGS_CMP8", "8", r8(op->r1), op->data1, 0, 0,"1");
    } else if (op->func==cmp8_reg_noflags) {
        genArithR_noflags("-", "8", r8(op->r1), op->data1, 0,"1");
    }
}
void OPCALL cmp16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmp16_reg_noflags(struct CPU* cpu, struct Op* op);
void gen03d(struct Op* op) {
    if (op->func==cmp16_reg) {
        genArithR("-", "FLAGS_CMP16", "16", r16(op->r1), op->data1, 0, 0,"1");
    } else if (op->func==cmp16_reg_noflags) {
        genArithR_noflags("-", "16", r16(op->r1), op->data1, 0,"1");
    }
}
void OPCALL cmp32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmp32_reg_noflags(struct CPU* cpu, struct Op* op);
void gen23d(struct Op* op) {
    if (op->func==cmp32_reg) {
        genArithR("-", "FLAGS_CMP32", "32", r32(op->r1), op->data1, 0, 0,"1");
    } else if (op->func==cmp32_reg_noflags) {
        genArithR_noflags("-", "32", r32(op->r1), op->data1, 0,"1");
    }
}
void gen006(struct Op* op) {
    out("push16(cpu, cpu->segValue[ES]);CYCLES(1);");
}
void gen206(struct Op* op) {
    out("push32(cpu, cpu->segValue[ES]);CYCLES(1);");
}
void gen007(struct Op* op) {
    out("cpu->segValue[ES] = pop16(cpu); cpu->segAddress[ES] = cpu->thread->process->ldt[cpu->segValue[ES] >> 3].base_addr;CYCLES(3);");
}
void gen207(struct Op* op) {
    out("cpu->segValue[ES] = pop32(cpu); cpu->segAddress[ES] = cpu->thread->process->ldt[cpu->segValue[ES] >> 3].base_addr;CYCLES(3);");
}
void gen00e(struct Op* op) {
    out("push16(cpu, cpu->segValue[CS]);CYCLES(1);");
}
void gen20e(struct Op* op) {
    out("push32(cpu, cpu->segValue[CS]);CYCLES(1);");
}
void gen016(struct Op* op) {
    out("push16(cpu, cpu->segValue[SS]);CYCLES(1);");
}
void gen216(struct Op* op) {
    out("push32(cpu, cpu->segValue[SS]);CYCLES(1);");
}
void gen017(struct Op* op) {
    out("cpu->segValue[SS] = pop16(cpu); cpu->segAddress[SS] = cpu->thread->process->ldt[cpu->segValue[SS] >> 3].base_addr;CYCLES(3);");
}
void gen217(struct Op* op) {
    out("cpu->segValue[SS] = pop32(cpu); cpu->segAddress[SS] = cpu->thread->process->ldt[cpu->segValue[SS] >> 3].base_addr;CYCLES(3);");
}
void gen01e(struct Op* op) {
    out("push16(cpu, cpu->segValue[DS]);CYCLES(1);");
}
void gen21e(struct Op* op) {
    out("push32(cpu, cpu->segValue[DS]);CYCLES(1);");
}
void gen01f(struct Op* op) {
    out("cpu->segValue[DS] = pop16(cpu); cpu->segAddress[DS] = cpu->thread->process->ldt[cpu->segValue[DS] >> 3].base_addr;CYCLES(3);");
}
void gen21f(struct Op* op) {
    out("cpu->segValue[DS] = pop32(cpu); cpu->segAddress[DS] = cpu->thread->process->ldt[cpu->segValue[DS] >> 3].base_addr;CYCLES(3);");
}
void gen1a0(struct Op* op) {
    out("push16(cpu, cpu->segValue[FS]);CYCLES(1);");
}
void gen3a0(struct Op* op) {
    out("push32(cpu, cpu->segValue[FS]);CYCLES(1);");
}
void gen1a1(struct Op* op) {
    out("cpu->segValue[FS] = pop16(cpu); cpu->segAddress[FS] = cpu->thread->process->ldt[cpu->segValue[FS] >> 3].base_addr;CYCLES(3);");
}
void gen3a1(struct Op* op) {
    out("cpu->segValue[FS] = pop32(cpu); cpu->segAddress[FS] = cpu->thread->process->ldt[cpu->segValue[FS] >> 3].base_addr;CYCLES(3);");
}
void gen1a8(struct Op* op) {
    out("push16(cpu, cpu->segValue[GS]);CYCLES(1);");
}
void gen3a8(struct Op* op) {
    out("push32(cpu, cpu->segValue[GS]);CYCLES(1);");
}
void gen1a9(struct Op* op) {
    out("cpu->segValue[GS] = pop16(cpu); cpu->segAddress[GS] = cpu->thread->process->ldt[cpu->segValue[GS] >> 3].base_addr;CYCLES(3);");
}
void gen3a9(struct Op* op) {
    out("cpu->segValue[GS] = pop32(cpu); cpu->segAddress[GS] = cpu->thread->process->ldt[cpu->segValue[GS] >> 3].base_addr;CYCLES(3);");
}
void OPCALL cmovO_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovO_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovO_16_mem32(struct CPU* cpu, struct Op* op);
void gen140(struct Op* op) {
    if (op->func == cmovO_16_reg) {
        out("if (getOF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovO_16_mem16) {
        out("if (getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovO_16_mem32) {
        out("if (getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen140");
    }
}
void OPCALL cmovO_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovO_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovO_32_mem32(struct CPU* cpu, struct Op* op);
void gen340(struct Op* op) {
    if (op->func == cmovO_32_reg) {
        out("if (getOF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovO_32_mem16) {
        out("if (getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovO_32_mem32) {
        out("if (getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen340");
    }
}
void OPCALL cmovNO_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNO_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNO_16_mem32(struct CPU* cpu, struct Op* op);
void gen141(struct Op* op) {
    if (op->func == cmovNO_16_reg) {
        out("if (!getOF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNO_16_mem16) {
        out("if (!getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNO_16_mem32) {
        out("if (!getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen141");
    }
}
void OPCALL cmovNO_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNO_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNO_32_mem32(struct CPU* cpu, struct Op* op);
void gen341(struct Op* op) {
    if (op->func == cmovNO_32_reg) {
        out("if (!getOF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNO_32_mem16) {
        out("if (!getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNO_32_mem32) {
        out("if (!getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen341");
    }
}
void OPCALL cmovB_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovB_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovB_16_mem32(struct CPU* cpu, struct Op* op);
void gen142(struct Op* op) {
    if (op->func == cmovB_16_reg) {
        out("if (getCF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovB_16_mem16) {
        out("if (getCF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovB_16_mem32) {
        out("if (getCF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen142");
    }
}
void OPCALL cmovB_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovB_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovB_32_mem32(struct CPU* cpu, struct Op* op);
void gen342(struct Op* op) {
    if (op->func == cmovB_32_reg) {
        out("if (getCF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovB_32_mem16) {
        out("if (getCF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovB_32_mem32) {
        out("if (getCF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen342");
    }
}
void OPCALL cmovNB_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNB_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNB_16_mem32(struct CPU* cpu, struct Op* op);
void gen143(struct Op* op) {
    if (op->func == cmovNB_16_reg) {
        out("if (!getCF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNB_16_mem16) {
        out("if (!getCF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNB_16_mem32) {
        out("if (!getCF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen143");
    }
}
void OPCALL cmovNB_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNB_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNB_32_mem32(struct CPU* cpu, struct Op* op);
void gen343(struct Op* op) {
    if (op->func == cmovNB_32_reg) {
        out("if (!getCF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNB_32_mem16) {
        out("if (!getCF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNB_32_mem32) {
        out("if (!getCF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen343");
    }
}
void OPCALL cmovZ_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovZ_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovZ_16_mem32(struct CPU* cpu, struct Op* op);
void gen144(struct Op* op) {
    if (op->func == cmovZ_16_reg) {
        out("if (getZF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovZ_16_mem16) {
        out("if (getZF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovZ_16_mem32) {
        out("if (getZF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen144");
    }
}
void OPCALL cmovZ_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovZ_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovZ_32_mem32(struct CPU* cpu, struct Op* op);
void gen344(struct Op* op) {
    if (op->func == cmovZ_32_reg) {
        out("if (getZF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovZ_32_mem16) {
        out("if (getZF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovZ_32_mem32) {
        out("if (getZF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen344");
    }
}
void OPCALL cmovNZ_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNZ_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNZ_16_mem32(struct CPU* cpu, struct Op* op);
void gen145(struct Op* op) {
    if (op->func == cmovNZ_16_reg) {
        out("if (!getZF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNZ_16_mem16) {
        out("if (!getZF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNZ_16_mem32) {
        out("if (!getZF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen145");
    }
}
void OPCALL cmovNZ_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNZ_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNZ_32_mem32(struct CPU* cpu, struct Op* op);
void gen345(struct Op* op) {
    if (op->func == cmovNZ_32_reg) {
        out("if (!getZF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNZ_32_mem16) {
        out("if (!getZF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNZ_32_mem32) {
        out("if (!getZF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen345");
    }
}
void OPCALL cmovBE_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovBE_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovBE_16_mem32(struct CPU* cpu, struct Op* op);
void gen146(struct Op* op) {
    if (op->func == cmovBE_16_reg) {
        out("if (getZF(cpu) || getCF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovBE_16_mem16) {
        out("if (getZF(cpu) || getCF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovBE_16_mem32) {
        out("if (getZF(cpu) || getCF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen146");
    }
}
void OPCALL cmovBE_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovBE_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovBE_32_mem32(struct CPU* cpu, struct Op* op);
void gen346(struct Op* op) {
    if (op->func == cmovBE_32_reg) {
        out("if (getZF(cpu) || getCF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovBE_32_mem16) {
        out("if (getZF(cpu) || getCF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovBE_32_mem32) {
        out("if (getZF(cpu) || getCF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen346");
    }
}
void OPCALL cmovNBE_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNBE_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNBE_16_mem32(struct CPU* cpu, struct Op* op);
void gen147(struct Op* op) {
    if (op->func == cmovNBE_16_reg) {
        out("if (!getZF(cpu) && !getCF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNBE_16_mem16) {
        out("if (!getZF(cpu) && !getCF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNBE_16_mem32) {
        out("if (!getZF(cpu) && !getCF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen147");
    }
}
void OPCALL cmovNBE_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNBE_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNBE_32_mem32(struct CPU* cpu, struct Op* op);
void gen347(struct Op* op) {
    if (op->func == cmovNBE_32_reg) {
        out("if (!getZF(cpu) && !getCF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNBE_32_mem16) {
        out("if (!getZF(cpu) && !getCF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNBE_32_mem32) {
        out("if (!getZF(cpu) && !getCF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen347");
    }
}
void OPCALL cmovS_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovS_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovS_16_mem32(struct CPU* cpu, struct Op* op);
void gen148(struct Op* op) {
    if (op->func == cmovS_16_reg) {
        out("if (getSF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovS_16_mem16) {
        out("if (getSF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovS_16_mem32) {
        out("if (getSF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen148");
    }
}
void OPCALL cmovS_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovS_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovS_32_mem32(struct CPU* cpu, struct Op* op);
void gen348(struct Op* op) {
    if (op->func == cmovS_32_reg) {
        out("if (getSF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovS_32_mem16) {
        out("if (getSF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovS_32_mem32) {
        out("if (getSF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen348");
    }
}
void OPCALL cmovNS_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNS_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNS_16_mem32(struct CPU* cpu, struct Op* op);
void gen149(struct Op* op) {
    if (op->func == cmovNS_16_reg) {
        out("if (!getSF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNS_16_mem16) {
        out("if (!getSF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNS_16_mem32) {
        out("if (!getSF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen149");
    }
}
void OPCALL cmovNS_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNS_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNS_32_mem32(struct CPU* cpu, struct Op* op);
void gen349(struct Op* op) {
    if (op->func == cmovNS_32_reg) {
        out("if (!getSF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNS_32_mem16) {
        out("if (!getSF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNS_32_mem32) {
        out("if (!getSF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen349");
    }
}
void OPCALL cmovP_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovP_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovP_16_mem32(struct CPU* cpu, struct Op* op);
void gen14a(struct Op* op) {
    if (op->func == cmovP_16_reg) {
        out("if (getPF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovP_16_mem16) {
        out("if (getPF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovP_16_mem32) {
        out("if (getPF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen14a");
    }
}
void OPCALL cmovP_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovP_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovP_32_mem32(struct CPU* cpu, struct Op* op);
void gen34a(struct Op* op) {
    if (op->func == cmovP_32_reg) {
        out("if (getPF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovP_32_mem16) {
        out("if (getPF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovP_32_mem32) {
        out("if (getPF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen34a");
    }
}
void OPCALL cmovNP_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNP_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNP_16_mem32(struct CPU* cpu, struct Op* op);
void gen14b(struct Op* op) {
    if (op->func == cmovNP_16_reg) {
        out("if (!getPF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNP_16_mem16) {
        out("if (!getPF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNP_16_mem32) {
        out("if (!getPF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen14b");
    }
}
void OPCALL cmovNP_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNP_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNP_32_mem32(struct CPU* cpu, struct Op* op);
void gen34b(struct Op* op) {
    if (op->func == cmovNP_32_reg) {
        out("if (!getPF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNP_32_mem16) {
        out("if (!getPF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNP_32_mem32) {
        out("if (!getPF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen34b");
    }
}
void OPCALL cmovL_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovL_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovL_16_mem32(struct CPU* cpu, struct Op* op);
void gen14c(struct Op* op) {
    if (op->func == cmovL_16_reg) {
        out("if (getSF(cpu)!=getOF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovL_16_mem16) {
        out("if (getSF(cpu)!=getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovL_16_mem32) {
        out("if (getSF(cpu)!=getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen14c");
    }
}
void OPCALL cmovL_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovL_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovL_32_mem32(struct CPU* cpu, struct Op* op);
void gen34c(struct Op* op) {
    if (op->func == cmovL_32_reg) {
        out("if (getSF(cpu)!=getOF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovL_32_mem16) {
        out("if (getSF(cpu)!=getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovL_32_mem32) {
        out("if (getSF(cpu)!=getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen34c");
    }
}
void OPCALL cmovNL_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNL_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNL_16_mem32(struct CPU* cpu, struct Op* op);
void gen14d(struct Op* op) {
    if (op->func == cmovNL_16_reg) {
        out("if (getSF(cpu)==getOF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNL_16_mem16) {
        out("if (getSF(cpu)==getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNL_16_mem32) {
        out("if (getSF(cpu)==getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen14d");
    }
}
void OPCALL cmovNL_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNL_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNL_32_mem32(struct CPU* cpu, struct Op* op);
void gen34d(struct Op* op) {
    if (op->func == cmovNL_32_reg) {
        out("if (getSF(cpu)==getOF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNL_32_mem16) {
        out("if (getSF(cpu)==getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNL_32_mem32) {
        out("if (getSF(cpu)==getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen34d");
    }
}
void OPCALL cmovLE_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovLE_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovLE_16_mem32(struct CPU* cpu, struct Op* op);
void gen14e(struct Op* op) {
    if (op->func == cmovLE_16_reg) {
        out("if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovLE_16_mem16) {
        out("if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovLE_16_mem32) {
        out("if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen14e");
    }
}
void OPCALL cmovLE_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovLE_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovLE_32_mem32(struct CPU* cpu, struct Op* op);
void gen34e(struct Op* op) {
    if (op->func == cmovLE_32_reg) {
        out("if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovLE_32_mem16) {
        out("if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovLE_32_mem32) {
        out("if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen34e");
    }
}
void OPCALL cmovNLE_16_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNLE_16_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNLE_16_mem32(struct CPU* cpu, struct Op* op);
void gen14f(struct Op* op) {
    if (op->func == cmovNLE_16_reg) {
        out("if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {");
        out(r16(op->r1));
        out(" = ");
        out(r16(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNLE_16_mem16) {
        out("if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNLE_16_mem32) {
        out("if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {");
        out(r16(op->r1));
        out(" = readw(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen14f");
    }
}
void OPCALL cmovNLE_32_reg(struct CPU* cpu, struct Op* op);
void OPCALL cmovNLE_32_mem16(struct CPU* cpu, struct Op* op);
void OPCALL cmovNLE_32_mem32(struct CPU* cpu, struct Op* op);
void gen34f(struct Op* op) {
    if (op->func == cmovNLE_32_reg) {
        out("if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {");
        out(r32(op->r1));
        out(" = ");
        out(r32(op->r2));
        out(";} CYCLES(1);");
    } else if (op->func == cmovNLE_32_mem16) {
        out("if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa16(op));
        out(");} CYCLES(1);");
    } else if (op->func == cmovNLE_32_mem32) {
        out("if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {");
        out(r32(op->r1));
        out(" = readd(cpu->memory, ");
        out(getEaa32(op));
        out(");} CYCLES(1);");
    } else {
        kpanic("gen34f");
    }
}
void OPCALL setO_reg(struct CPU* cpu, struct Op* op);
void OPCALL setO_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setO_mem32(struct CPU* cpu, struct Op* op);
void gen190(struct Op* op) {
    if (op->func == setO_reg) {
        out("if (getOF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setO_mem16) {
        out("if (getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setO_mem32) {
        out("if (getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen190");
    }
}
void OPCALL setNO_reg(struct CPU* cpu, struct Op* op);
void OPCALL setNO_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setNO_mem32(struct CPU* cpu, struct Op* op);
void gen191(struct Op* op) {
    if (op->func == setNO_reg) {
        out("if (!getOF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setNO_mem16) {
        out("if (!getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setNO_mem32) {
        out("if (!getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen191");
    }
}
void OPCALL setB_reg(struct CPU* cpu, struct Op* op);
void OPCALL setB_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setB_mem32(struct CPU* cpu, struct Op* op);
void gen192(struct Op* op) {
    if (op->func == setB_reg) {
        out("if (getCF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setB_mem16) {
        out("if (getCF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setB_mem32) {
        out("if (getCF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen192");
    }
}
void OPCALL setNB_reg(struct CPU* cpu, struct Op* op);
void OPCALL setNB_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setNB_mem32(struct CPU* cpu, struct Op* op);
void gen193(struct Op* op) {
    if (op->func == setNB_reg) {
        out("if (!getCF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setNB_mem16) {
        out("if (!getCF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setNB_mem32) {
        out("if (!getCF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen193");
    }
}
void OPCALL setZ_reg(struct CPU* cpu, struct Op* op);
void OPCALL setZ_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setZ_mem32(struct CPU* cpu, struct Op* op);
void gen194(struct Op* op) {
    if (op->func == setZ_reg) {
        out("if (getZF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setZ_mem16) {
        out("if (getZF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setZ_mem32) {
        out("if (getZF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen194");
    }
}
void OPCALL setNZ_reg(struct CPU* cpu, struct Op* op);
void OPCALL setNZ_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setNZ_mem32(struct CPU* cpu, struct Op* op);
void gen195(struct Op* op) {
    if (op->func == setNZ_reg) {
        out("if (!getZF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setNZ_mem16) {
        out("if (!getZF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setNZ_mem32) {
        out("if (!getZF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen195");
    }
}
void OPCALL setBE_reg(struct CPU* cpu, struct Op* op);
void OPCALL setBE_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setBE_mem32(struct CPU* cpu, struct Op* op);
void gen196(struct Op* op) {
    if (op->func == setBE_reg) {
        out("if (getZF(cpu) || getCF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setBE_mem16) {
        out("if (getZF(cpu) || getCF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setBE_mem32) {
        out("if (getZF(cpu) || getCF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen196");
    }
}
void OPCALL setNBE_reg(struct CPU* cpu, struct Op* op);
void OPCALL setNBE_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setNBE_mem32(struct CPU* cpu, struct Op* op);
void gen197(struct Op* op) {
    if (op->func == setNBE_reg) {
        out("if (!getZF(cpu) && !getCF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setNBE_mem16) {
        out("if (!getZF(cpu) && !getCF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setNBE_mem32) {
        out("if (!getZF(cpu) && !getCF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen197");
    }
}
void OPCALL setS_reg(struct CPU* cpu, struct Op* op);
void OPCALL setS_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setS_mem32(struct CPU* cpu, struct Op* op);
void gen198(struct Op* op) {
    if (op->func == setS_reg) {
        out("if (getSF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setS_mem16) {
        out("if (getSF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setS_mem32) {
        out("if (getSF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen198");
    }
}
void OPCALL setNS_reg(struct CPU* cpu, struct Op* op);
void OPCALL setNS_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setNS_mem32(struct CPU* cpu, struct Op* op);
void gen199(struct Op* op) {
    if (op->func == setNS_reg) {
        out("if (!getSF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setNS_mem16) {
        out("if (!getSF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setNS_mem32) {
        out("if (!getSF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen199");
    }
}
void OPCALL setP_reg(struct CPU* cpu, struct Op* op);
void OPCALL setP_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setP_mem32(struct CPU* cpu, struct Op* op);
void gen19a(struct Op* op) {
    if (op->func == setP_reg) {
        out("if (getPF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setP_mem16) {
        out("if (getPF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setP_mem32) {
        out("if (getPF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen19a");
    }
}
void OPCALL setNP_reg(struct CPU* cpu, struct Op* op);
void OPCALL setNP_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setNP_mem32(struct CPU* cpu, struct Op* op);
void gen19b(struct Op* op) {
    if (op->func == setNP_reg) {
        out("if (!getPF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setNP_mem16) {
        out("if (!getPF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setNP_mem32) {
        out("if (!getPF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen19b");
    }
}
void OPCALL setL_reg(struct CPU* cpu, struct Op* op);
void OPCALL setL_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setL_mem32(struct CPU* cpu, struct Op* op);
void gen19c(struct Op* op) {
    if (op->func == setL_reg) {
        out("if (getSF(cpu)!=getOF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setL_mem16) {
        out("if (getSF(cpu)!=getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setL_mem32) {
        out("if (getSF(cpu)!=getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen19c");
    }
}
void OPCALL setNL_reg(struct CPU* cpu, struct Op* op);
void OPCALL setNL_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setNL_mem32(struct CPU* cpu, struct Op* op);
void gen19d(struct Op* op) {
    if (op->func == setNL_reg) {
        out("if (getSF(cpu)==getOF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setNL_mem16) {
        out("if (getSF(cpu)==getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setNL_mem32) {
        out("if (getSF(cpu)==getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen19d");
    }
}
void OPCALL setLE_reg(struct CPU* cpu, struct Op* op);
void OPCALL setLE_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setLE_mem32(struct CPU* cpu, struct Op* op);
void gen19e(struct Op* op) {
    if (op->func == setLE_reg) {
        out("if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setLE_mem16) {
        out("if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setLE_mem32) {
        out("if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen19e");
    }
}
void OPCALL setNLE_reg(struct CPU* cpu, struct Op* op);
void OPCALL setNLE_mem16(struct CPU* cpu, struct Op* op);
void OPCALL setNLE_mem32(struct CPU* cpu, struct Op* op);
void gen19f(struct Op* op) {
    if (op->func == setNLE_reg) {
        out("if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {");
        out(r8(op->r1));
        out(" = 1;} else {");
        out(r8(op->r1));
        out(" = 0;} CYCLES(2);");
    } else if (op->func == setNLE_mem16) {
        out("if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa16(op));
        out(", 0);} CYCLES(2);");
    } else if (op->func == setNLE_mem32) {
        out("if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 1);} else {writeb(cpu->memory, ");
        out(getEaa32(op));
        out(", 0);} CYCLES(2);");
    } else {
        kpanic("gen19f");
    }
}

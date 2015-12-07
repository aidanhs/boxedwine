void OPCALL setO_reg(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setO_mem16(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setO_mem32(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNO_reg(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNO_mem16(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNO_mem32(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setB_reg(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setB_mem16(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setB_mem32(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNB_reg(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNB_mem16(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNB_mem32(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setZ_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setZ_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setZ_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNZ_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNZ_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNZ_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setBE_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setBE_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setBE_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNBE_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNBE_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNBE_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setS_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setS_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setS_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNS_reg(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNS_mem16(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNS_mem32(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setP_reg(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setP_mem16(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setP_mem32(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNP_reg(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNP_mem16(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNP_mem32(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setL_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setL_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setL_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNL_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNL_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNL_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setLE_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setLE_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setLE_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNLE_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNLE_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        writeb(eaa16(cpu, op), 1);
    } else {
        writeb(eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void OPCALL setNLE_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        writeb(eaa32(cpu, op), 1);
    } else {
        writeb(eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}

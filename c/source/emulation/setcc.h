void setO_reg(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setO_mem16(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setO_mem32(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNO_reg(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setNO_mem16(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNO_mem32(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setB_reg(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setB_mem16(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setB_mem32(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNB_reg(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setNB_mem16(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNB_mem32(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setZ_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setZ_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setZ_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNZ_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setNZ_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNZ_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setBE_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setBE_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setBE_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNBE_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setNBE_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNBE_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setS_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setS_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setS_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNS_reg(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setNS_mem16(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNS_mem32(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setP_reg(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setP_mem16(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setP_mem32(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNP_reg(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setNP_mem16(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNP_mem32(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setL_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setL_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setL_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNL_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setNL_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNL_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setLE_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setLE_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setLE_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNLE_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        *cpu->reg8[op->r1] = 1;
    } else {
        *cpu->reg8[op->r1] = 0;
    }
    CYCLES(2);
    NEXT();
}
void setNLE_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        writeb(cpu->memory, eaa16(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa16(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}
void setNLE_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        writeb(cpu->memory, eaa32(cpu, op), 1);
    } else {
        writeb(cpu->memory, eaa32(cpu, op), 0);
    }
    CYCLES(2);
    NEXT();
}

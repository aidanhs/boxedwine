void cmovO_16_reg(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovO_16_mem16(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovO_16_mem32(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovO_32_reg(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovO_32_mem16(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovO_32_mem32(struct CPU* cpu, struct Op* op) {
    if (getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNO_16_reg(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovNO_16_mem16(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNO_16_mem32(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNO_32_reg(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovNO_32_mem16(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNO_32_mem32(struct CPU* cpu, struct Op* op) {
    if (!getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovB_16_reg(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovB_16_mem16(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovB_16_mem32(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovB_32_reg(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovB_32_mem16(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovB_32_mem32(struct CPU* cpu, struct Op* op) {
    if (getCF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNB_16_reg(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovNB_16_mem16(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNB_16_mem32(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNB_32_reg(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovNB_32_mem16(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNB_32_mem32(struct CPU* cpu, struct Op* op) {
    if (!getCF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovZ_16_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovZ_16_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovZ_16_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovZ_32_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovZ_32_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovZ_32_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNZ_16_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovNZ_16_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNZ_16_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNZ_32_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovNZ_32_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNZ_32_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovBE_16_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovBE_16_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovBE_16_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovBE_32_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovBE_32_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovBE_32_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getCF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNBE_16_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovNBE_16_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNBE_16_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNBE_32_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovNBE_32_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNBE_32_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && !getCF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovS_16_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovS_16_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovS_16_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovS_32_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovS_32_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovS_32_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNS_16_reg(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovNS_16_mem16(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNS_16_mem32(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNS_32_reg(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovNS_32_mem16(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNS_32_mem32(struct CPU* cpu, struct Op* op) {
    if (!getSF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovP_16_reg(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovP_16_mem16(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovP_16_mem32(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovP_32_reg(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovP_32_mem16(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovP_32_mem32(struct CPU* cpu, struct Op* op) {
    if (getPF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNP_16_reg(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovNP_16_mem16(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNP_16_mem32(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNP_32_reg(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovNP_32_mem16(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNP_32_mem32(struct CPU* cpu, struct Op* op) {
    if (!getPF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovL_16_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovL_16_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovL_16_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovL_32_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovL_32_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovL_32_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNL_16_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovNL_16_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNL_16_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNL_32_reg(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovNL_32_mem16(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNL_32_mem32(struct CPU* cpu, struct Op* op) {
    if (getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovLE_16_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovLE_16_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovLE_16_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovLE_32_reg(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovLE_32_mem16(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovLE_32_mem32(struct CPU* cpu, struct Op* op) {
    if (getZF(cpu) || getSF(cpu)!=getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNLE_16_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u16 = cpu->reg[op->r2].u16;
    }
    CYCLES(1);
    NEXT();
}
void cmovNLE_16_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNLE_16_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u16 = readw(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNLE_32_reg(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u32 = cpu->reg[op->r2].u32;
    }
    CYCLES(1);
    NEXT();
}
void cmovNLE_32_mem16(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa16(cpu, op));
    }
    CYCLES(1);
    NEXT();
}
void cmovNLE_32_mem32(struct CPU* cpu, struct Op* op) {
    if (!getZF(cpu) && getSF(cpu)==getOF(cpu)) {
        cpu->reg[op->r1].u32 = readd(cpu->memory, eaa32(cpu, op));
    }
    CYCLES(1);
    NEXT();
}

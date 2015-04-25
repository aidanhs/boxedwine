void addr8r8(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = *cpu->reg8[op->r2];
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8;
    cpu->lazyFlags = FLAGS_ADD8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void adde8r8_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8;
    cpu->lazyFlags = FLAGS_ADD8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void adde8r8_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8;
    cpu->lazyFlags = FLAGS_ADD8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void addr8e8_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa16(cpu, op));
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8;
    cpu->lazyFlags = FLAGS_ADD8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void addr8e8_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa32(cpu, op));
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8;
    cpu->lazyFlags = FLAGS_ADD8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void add8_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8;
    cpu->lazyFlags = FLAGS_ADD8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void add8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8;
    cpu->lazyFlags = FLAGS_ADD8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void add8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8;
    cpu->lazyFlags = FLAGS_ADD8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void addr16r16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = cpu->reg[op->r2].u16;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16;
    cpu->lazyFlags = FLAGS_ADD16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void adde16r16_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16;
    cpu->lazyFlags = FLAGS_ADD16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void adde16r16_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16;
    cpu->lazyFlags = FLAGS_ADD16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void addr16e16_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa16(cpu, op));
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16;
    cpu->lazyFlags = FLAGS_ADD16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void addr16e16_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa32(cpu, op));
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16;
    cpu->lazyFlags = FLAGS_ADD16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void add16_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16;
    cpu->lazyFlags = FLAGS_ADD16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void add16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16;
    cpu->lazyFlags = FLAGS_ADD16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void add16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16;
    cpu->lazyFlags = FLAGS_ADD16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void addr32r32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = cpu->reg[op->r2].u32;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32;
    cpu->lazyFlags = FLAGS_ADD32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void adde32r32_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32;
    cpu->lazyFlags = FLAGS_ADD32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void adde32r32_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32;
    cpu->lazyFlags = FLAGS_ADD32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void addr32e32_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa16(cpu, op));
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32;
    cpu->lazyFlags = FLAGS_ADD32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void addr32e32_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa32(cpu, op));
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32;
    cpu->lazyFlags = FLAGS_ADD32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void add32_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32;
    cpu->lazyFlags = FLAGS_ADD32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void add32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32;
    cpu->lazyFlags = FLAGS_ADD32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void add32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32;
    cpu->lazyFlags = FLAGS_ADD32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void orr8r8(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = *cpu->reg8[op->r2];
    cpu->result.u8 = cpu->dst.u8 | cpu->src.u8;
    cpu->lazyFlags = FLAGS_OR8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void ore8r8_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 | cpu->src.u8;
    cpu->lazyFlags = FLAGS_OR8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void ore8r8_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 | cpu->src.u8;
    cpu->lazyFlags = FLAGS_OR8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void orr8e8_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa16(cpu, op));
    cpu->result.u8 = cpu->dst.u8 | cpu->src.u8;
    cpu->lazyFlags = FLAGS_OR8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void orr8e8_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa32(cpu, op));
    cpu->result.u8 = cpu->dst.u8 | cpu->src.u8;
    cpu->lazyFlags = FLAGS_OR8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void or8_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 | cpu->src.u8;
    cpu->lazyFlags = FLAGS_OR8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void or8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 | cpu->src.u8;
    cpu->lazyFlags = FLAGS_OR8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void or8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 | cpu->src.u8;
    cpu->lazyFlags = FLAGS_OR8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void orr16r16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = cpu->reg[op->r2].u16;
    cpu->result.u16 = cpu->dst.u16 | cpu->src.u16;
    cpu->lazyFlags = FLAGS_OR16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void ore16r16_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 | cpu->src.u16;
    cpu->lazyFlags = FLAGS_OR16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void ore16r16_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 | cpu->src.u16;
    cpu->lazyFlags = FLAGS_OR16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void orr16e16_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa16(cpu, op));
    cpu->result.u16 = cpu->dst.u16 | cpu->src.u16;
    cpu->lazyFlags = FLAGS_OR16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void orr16e16_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa32(cpu, op));
    cpu->result.u16 = cpu->dst.u16 | cpu->src.u16;
    cpu->lazyFlags = FLAGS_OR16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void or16_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 | cpu->src.u16;
    cpu->lazyFlags = FLAGS_OR16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void or16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 | cpu->src.u16;
    cpu->lazyFlags = FLAGS_OR16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void or16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 | cpu->src.u16;
    cpu->lazyFlags = FLAGS_OR16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void orr32r32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = cpu->reg[op->r2].u32;
    cpu->result.u32 = cpu->dst.u32 | cpu->src.u32;
    cpu->lazyFlags = FLAGS_OR32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void ore32r32_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 | cpu->src.u32;
    cpu->lazyFlags = FLAGS_OR32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void ore32r32_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 | cpu->src.u32;
    cpu->lazyFlags = FLAGS_OR32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void orr32e32_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa16(cpu, op));
    cpu->result.u32 = cpu->dst.u32 | cpu->src.u32;
    cpu->lazyFlags = FLAGS_OR32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void orr32e32_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa32(cpu, op));
    cpu->result.u32 = cpu->dst.u32 | cpu->src.u32;
    cpu->lazyFlags = FLAGS_OR32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void or32_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 | cpu->src.u32;
    cpu->lazyFlags = FLAGS_OR32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void or32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 | cpu->src.u32;
    cpu->lazyFlags = FLAGS_OR32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void or32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 | cpu->src.u32;
    cpu->lazyFlags = FLAGS_OR32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void adcr8r8(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = *cpu->reg8[op->r2];
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void adce8r8_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void adce8r8_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void adcr8e8_16(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa16(cpu, op));
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void adcr8e8_32(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa32(cpu, op));
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void adc8_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void adc8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void adc8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 + cpu->src.u8 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void adcr16r16(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = cpu->reg[op->r2].u16;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void adce16r16_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void adce16r16_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void adcr16e16_16(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa16(cpu, op));
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void adcr16e16_32(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa32(cpu, op));
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void adc16_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void adc16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void adc16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 + cpu->src.u16 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void adcr32r32(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = cpu->reg[op->r2].u32;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void adce32r32_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void adce32r32_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void adcr32e32_16(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa16(cpu, op));
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void adcr32e32_32(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa32(cpu, op));
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void adc32_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void adc32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void adc32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 + cpu->src.u32 + cpu->oldcf;
    cpu->lazyFlags = FLAGS_ADC32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void sbbr8r8(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = *cpu->reg8[op->r2];
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void sbbe8r8_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void sbbe8r8_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void sbbr8e8_16(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa16(cpu, op));
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void sbbr8e8_32(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa32(cpu, op));
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void sbb8_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void sbb8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void sbb8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void sbbr16r16(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = cpu->reg[op->r2].u16;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void sbbe16r16_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void sbbe16r16_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void sbbr16e16_16(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa16(cpu, op));
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void sbbr16e16_32(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa32(cpu, op));
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void sbb16_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void sbb16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void sbb16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void sbbr32r32(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = cpu->reg[op->r2].u32;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void sbbe32r32_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void sbbe32r32_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void sbbr32e32_16(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa16(cpu, op));
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void sbbr32e32_32(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa32(cpu, op));
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void sbb32_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void sbb32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void sbb32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf = getCF(cpu);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32 - cpu->oldcf;
    cpu->lazyFlags = FLAGS_SBB32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void andr8r8(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = *cpu->reg8[op->r2];
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_AND8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void ande8r8_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_AND8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void ande8r8_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_AND8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void andr8e8_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa16(cpu, op));
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_AND8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void andr8e8_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa32(cpu, op));
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_AND8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void and8_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_AND8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void and8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_AND8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void and8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_AND8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void andr16r16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = cpu->reg[op->r2].u16;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_AND16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void ande16r16_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_AND16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void ande16r16_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_AND16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void andr16e16_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa16(cpu, op));
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_AND16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void andr16e16_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa32(cpu, op));
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_AND16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void and16_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_AND16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void and16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_AND16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void and16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_AND16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void andr32r32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = cpu->reg[op->r2].u32;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_AND32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void ande32r32_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_AND32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void ande32r32_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_AND32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void andr32e32_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa16(cpu, op));
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_AND32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void andr32e32_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa32(cpu, op));
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_AND32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void and32_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_AND32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void and32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_AND32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void and32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_AND32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void subr8r8(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = *cpu->reg8[op->r2];
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_SUB8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void sube8r8_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_SUB8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void sube8r8_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_SUB8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void subr8e8_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa16(cpu, op));
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_SUB8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void subr8e8_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa32(cpu, op));
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_SUB8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void sub8_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_SUB8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void sub8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_SUB8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void sub8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_SUB8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void subr16r16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = cpu->reg[op->r2].u16;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_SUB16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void sube16r16_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_SUB16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void sube16r16_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_SUB16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void subr16e16_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa16(cpu, op));
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_SUB16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void subr16e16_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa32(cpu, op));
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_SUB16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void sub16_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_SUB16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void sub16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_SUB16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void sub16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_SUB16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void subr32r32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = cpu->reg[op->r2].u32;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_SUB32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void sube32r32_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_SUB32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void sube32r32_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_SUB32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void subr32e32_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa16(cpu, op));
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_SUB32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void subr32e32_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa32(cpu, op));
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_SUB32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void sub32_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_SUB32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void sub32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_SUB32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void sub32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_SUB32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void xorr8r8(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = *cpu->reg8[op->r2];
    cpu->result.u8 = cpu->dst.u8 ^ cpu->src.u8;
    cpu->lazyFlags = FLAGS_XOR8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void xore8r8_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 ^ cpu->src.u8;
    cpu->lazyFlags = FLAGS_XOR8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void xore8r8_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 ^ cpu->src.u8;
    cpu->lazyFlags = FLAGS_XOR8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void xorr8e8_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa16(cpu, op));
    cpu->result.u8 = cpu->dst.u8 ^ cpu->src.u8;
    cpu->lazyFlags = FLAGS_XOR8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void xorr8e8_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa32(cpu, op));
    cpu->result.u8 = cpu->dst.u8 ^ cpu->src.u8;
    cpu->lazyFlags = FLAGS_XOR8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(2);
    NEXT();
}
void xor8_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 ^ cpu->src.u8;
    cpu->lazyFlags = FLAGS_XOR8;
    *cpu->reg8[op->r1] =  cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void xor8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 ^ cpu->src.u8;
    cpu->lazyFlags = FLAGS_XOR8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void xor8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 ^ cpu->src.u8;
    cpu->lazyFlags = FLAGS_XOR8;
    writeb(cpu->memory, eaa,  cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void xorr16r16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = cpu->reg[op->r2].u16;
    cpu->result.u16 = cpu->dst.u16 ^ cpu->src.u16;
    cpu->lazyFlags = FLAGS_XOR16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void xore16r16_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 ^ cpu->src.u16;
    cpu->lazyFlags = FLAGS_XOR16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void xore16r16_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 ^ cpu->src.u16;
    cpu->lazyFlags = FLAGS_XOR16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void xorr16e16_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa16(cpu, op));
    cpu->result.u16 = cpu->dst.u16 ^ cpu->src.u16;
    cpu->lazyFlags = FLAGS_XOR16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void xorr16e16_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa32(cpu, op));
    cpu->result.u16 = cpu->dst.u16 ^ cpu->src.u16;
    cpu->lazyFlags = FLAGS_XOR16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(2);
    NEXT();
}
void xor16_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 ^ cpu->src.u16;
    cpu->lazyFlags = FLAGS_XOR16;
    cpu->reg[op->r1].u16 =  cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void xor16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 ^ cpu->src.u16;
    cpu->lazyFlags = FLAGS_XOR16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void xor16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 ^ cpu->src.u16;
    cpu->lazyFlags = FLAGS_XOR16;
    writew(cpu->memory, eaa,  cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void xorr32r32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = cpu->reg[op->r2].u32;
    cpu->result.u32 = cpu->dst.u32 ^ cpu->src.u32;
    cpu->lazyFlags = FLAGS_XOR32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void xore32r32_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 ^ cpu->src.u32;
    cpu->lazyFlags = FLAGS_XOR32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void xore32r32_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 ^ cpu->src.u32;
    cpu->lazyFlags = FLAGS_XOR32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void xorr32e32_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa16(cpu, op));
    cpu->result.u32 = cpu->dst.u32 ^ cpu->src.u32;
    cpu->lazyFlags = FLAGS_XOR32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void xorr32e32_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa32(cpu, op));
    cpu->result.u32 = cpu->dst.u32 ^ cpu->src.u32;
    cpu->lazyFlags = FLAGS_XOR32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(2);
    NEXT();
}
void xor32_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 ^ cpu->src.u32;
    cpu->lazyFlags = FLAGS_XOR32;
    cpu->reg[op->r1].u32 =  cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void xor32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 ^ cpu->src.u32;
    cpu->lazyFlags = FLAGS_XOR32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void xor32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 ^ cpu->src.u32;
    cpu->lazyFlags = FLAGS_XOR32;
    writed(cpu->memory, eaa,  cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void cmpr8r8(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = *cpu->reg8[op->r2];
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_CMP8;
    CYCLES(1);
    NEXT();
}
void cmpe8r8_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_CMP8;
    CYCLES(2);
    NEXT();
}
void cmpe8r8_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_CMP8;
    CYCLES(2);
    NEXT();
}
void cmpr8e8_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa16(cpu, op));
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_CMP8;
    CYCLES(2);
    NEXT();
}
void cmpr8e8_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa32(cpu, op));
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_CMP8;
    CYCLES(2);
    NEXT();
}
void cmp8_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_CMP8;
    CYCLES(1);
    NEXT();
}
void cmp8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_CMP8;
    CYCLES(2);
    NEXT();
}
void cmp8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
    cpu->lazyFlags = FLAGS_CMP8;
    CYCLES(2);
    NEXT();
}
void cmpr16r16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = cpu->reg[op->r2].u16;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_CMP16;
    CYCLES(1);
    NEXT();
}
void cmpe16r16_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_CMP16;
    CYCLES(2);
    NEXT();
}
void cmpe16r16_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_CMP16;
    CYCLES(2);
    NEXT();
}
void cmpr16e16_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa16(cpu, op));
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_CMP16;
    CYCLES(2);
    NEXT();
}
void cmpr16e16_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa32(cpu, op));
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_CMP16;
    CYCLES(2);
    NEXT();
}
void cmp16_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_CMP16;
    CYCLES(1);
    NEXT();
}
void cmp16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_CMP16;
    CYCLES(2);
    NEXT();
}
void cmp16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
    cpu->lazyFlags = FLAGS_CMP16;
    CYCLES(2);
    NEXT();
}
void cmpr32r32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = cpu->reg[op->r2].u32;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_CMP32;
    CYCLES(1);
    NEXT();
}
void cmpe32r32_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_CMP32;
    CYCLES(2);
    NEXT();
}
void cmpe32r32_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_CMP32;
    CYCLES(2);
    NEXT();
}
void cmpr32e32_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa16(cpu, op));
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_CMP32;
    CYCLES(2);
    NEXT();
}
void cmpr32e32_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa32(cpu, op));
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_CMP32;
    CYCLES(2);
    NEXT();
}
void cmp32_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_CMP32;
    CYCLES(1);
    NEXT();
}
void cmp32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_CMP32;
    CYCLES(2);
    NEXT();
}
void cmp32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
    cpu->lazyFlags = FLAGS_CMP32;
    CYCLES(2);
    NEXT();
}
void testr8r8(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = *cpu->reg8[op->r2];
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_TEST8;
    CYCLES(1);
    NEXT();
}
void teste8r8_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_TEST8;
    CYCLES(2);
    NEXT();
}
void teste8r8_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = *cpu->reg8[op->r1];
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_TEST8;
    CYCLES(2);
    NEXT();
}
void testr8e8_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa16(cpu, op));
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_TEST8;
    CYCLES(2);
    NEXT();
}
void testr8e8_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = readb(cpu->memory, eaa32(cpu, op));
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_TEST8;
    CYCLES(2);
    NEXT();
}
void test8_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u8 = *cpu->reg8[op->r1];
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_TEST8;
    CYCLES(1);
    NEXT();
}
void test8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_TEST8;
    CYCLES(2);
    NEXT();
}
void test8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u8 = readb(cpu->memory, eaa);
    cpu->src.u8 = op->data1;
    cpu->result.u8 = cpu->dst.u8 & cpu->src.u8;
    cpu->lazyFlags = FLAGS_TEST8;
    CYCLES(2);
    NEXT();
}
void testr16r16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = cpu->reg[op->r2].u16;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_TEST16;
    CYCLES(1);
    NEXT();
}
void teste16r16_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_TEST16;
    CYCLES(2);
    NEXT();
}
void teste16r16_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = cpu->reg[op->r1].u16;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_TEST16;
    CYCLES(2);
    NEXT();
}
void testr16e16_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa16(cpu, op));
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_TEST16;
    CYCLES(2);
    NEXT();
}
void testr16e16_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = readw(cpu->memory, eaa32(cpu, op));
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_TEST16;
    CYCLES(2);
    NEXT();
}
void test16_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u16 = cpu->reg[op->r1].u16;
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_TEST16;
    CYCLES(1);
    NEXT();
}
void test16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_TEST16;
    CYCLES(2);
    NEXT();
}
void test16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u16 = readw(cpu->memory, eaa);
    cpu->src.u16 = op->data1;
    cpu->result.u16 = cpu->dst.u16 & cpu->src.u16;
    cpu->lazyFlags = FLAGS_TEST16;
    CYCLES(2);
    NEXT();
}
void testr32r32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = cpu->reg[op->r2].u32;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_TEST32;
    CYCLES(1);
    NEXT();
}
void teste32r32_16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_TEST32;
    CYCLES(2);
    NEXT();
}
void teste32r32_32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = cpu->reg[op->r1].u32;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_TEST32;
    CYCLES(2);
    NEXT();
}
void testr32e32_16(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa16(cpu, op));
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_TEST32;
    CYCLES(2);
    NEXT();
}
void testr32e32_32(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = readd(cpu->memory, eaa32(cpu, op));
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_TEST32;
    CYCLES(2);
    NEXT();
}
void test32_reg(struct CPU* cpu, struct Op* op) {
    cpu->dst.u32 = cpu->reg[op->r1].u32;
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_TEST32;
    CYCLES(1);
    NEXT();
}
void test32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_TEST32;
    CYCLES(2);
    NEXT();
}
void test32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->dst.u32 = readd(cpu->memory, eaa);
    cpu->src.u32 = op->data1;
    cpu->result.u32 = cpu->dst.u32 & cpu->src.u32;
    cpu->lazyFlags = FLAGS_TEST32;
    CYCLES(2);
    NEXT();
}

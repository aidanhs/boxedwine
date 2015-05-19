void OPCALL inc8_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf=getCF(cpu);
    cpu->dst.u8=*cpu->reg8[op->r1];
    cpu->result.u8=cpu->dst.u8 + 1;
    cpu->lazyFlags = FLAGS_INC8;
    *cpu->reg8[op->r1] = cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void OPCALL inc8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u8=readb(cpu->memory, eaa);
    cpu->result.u8=cpu->dst.u8 + 1;
    cpu->lazyFlags = FLAGS_INC8;
    writeb(cpu->memory, eaa, cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void OPCALL inc8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u8=readb(cpu->memory, eaa);
    cpu->result.u8=cpu->dst.u8 + 1;
    cpu->lazyFlags = FLAGS_INC8;
    writeb(cpu->memory, eaa, cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void OPCALL inc16_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf=getCF(cpu);
    cpu->dst.u16=cpu->reg[op->r1].u16;
    cpu->result.u16=cpu->dst.u16 + 1;
    cpu->lazyFlags = FLAGS_INC16;
    cpu->reg[op->r1].u16 = cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void OPCALL inc16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u16=readw(cpu->memory, eaa);
    cpu->result.u16=cpu->dst.u16 + 1;
    cpu->lazyFlags = FLAGS_INC16;
    writew(cpu->memory, eaa, cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void OPCALL inc16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u16=readw(cpu->memory, eaa);
    cpu->result.u16=cpu->dst.u16 + 1;
    cpu->lazyFlags = FLAGS_INC16;
    writew(cpu->memory, eaa, cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void OPCALL inc32_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf=getCF(cpu);
    cpu->dst.u32=cpu->reg[op->r1].u32;
    cpu->result.u32=cpu->dst.u32 + 1;
    cpu->lazyFlags = FLAGS_INC32;
    cpu->reg[op->r1].u32 = cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void OPCALL inc32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u32=readd(cpu->memory, eaa);
    cpu->result.u32=cpu->dst.u32 + 1;
    cpu->lazyFlags = FLAGS_INC32;
    writed(cpu->memory, eaa, cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void OPCALL inc32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u32=readd(cpu->memory, eaa);
    cpu->result.u32=cpu->dst.u32 + 1;
    cpu->lazyFlags = FLAGS_INC32;
    writed(cpu->memory, eaa, cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void OPCALL dec8_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf=getCF(cpu);
    cpu->dst.u8=*cpu->reg8[op->r1];
    cpu->result.u8=cpu->dst.u8 - 1;
    cpu->lazyFlags = FLAGS_DEC8;
    *cpu->reg8[op->r1] = cpu->result.u8;
    CYCLES(1);
    NEXT();
}
void OPCALL dec8_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u8=readb(cpu->memory, eaa);
    cpu->result.u8=cpu->dst.u8 - 1;
    cpu->lazyFlags = FLAGS_DEC8;
    writeb(cpu->memory, eaa, cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void OPCALL dec8_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u8=readb(cpu->memory, eaa);
    cpu->result.u8=cpu->dst.u8 - 1;
    cpu->lazyFlags = FLAGS_DEC8;
    writeb(cpu->memory, eaa, cpu->result.u8);
    CYCLES(3);
    NEXT();
}
void OPCALL dec16_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf=getCF(cpu);
    cpu->dst.u16=cpu->reg[op->r1].u16;
    cpu->result.u16=cpu->dst.u16 - 1;
    cpu->lazyFlags = FLAGS_DEC16;
    cpu->reg[op->r1].u16 = cpu->result.u16;
    CYCLES(1);
    NEXT();
}
void OPCALL dec16_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u16=readw(cpu->memory, eaa);
    cpu->result.u16=cpu->dst.u16 - 1;
    cpu->lazyFlags = FLAGS_DEC16;
    writew(cpu->memory, eaa, cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void OPCALL dec16_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u16=readw(cpu->memory, eaa);
    cpu->result.u16=cpu->dst.u16 - 1;
    cpu->lazyFlags = FLAGS_DEC16;
    writew(cpu->memory, eaa, cpu->result.u16);
    CYCLES(3);
    NEXT();
}
void OPCALL dec32_reg(struct CPU* cpu, struct Op* op) {
    cpu->oldcf=getCF(cpu);
    cpu->dst.u32=cpu->reg[op->r1].u32;
    cpu->result.u32=cpu->dst.u32 - 1;
    cpu->lazyFlags = FLAGS_DEC32;
    cpu->reg[op->r1].u32 = cpu->result.u32;
    CYCLES(1);
    NEXT();
}
void OPCALL dec32_mem16(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa16(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u32=readd(cpu->memory, eaa);
    cpu->result.u32=cpu->dst.u32 - 1;
    cpu->lazyFlags = FLAGS_DEC32;
    writed(cpu->memory, eaa, cpu->result.u32);
    CYCLES(3);
    NEXT();
}
void OPCALL dec32_mem32(struct CPU* cpu, struct Op* op) {
    U32 eaa = eaa32(cpu, op);
    cpu->oldcf=getCF(cpu);
    cpu->dst.u32=readd(cpu->memory, eaa);
    cpu->result.u32=cpu->dst.u32 - 1;
    cpu->lazyFlags = FLAGS_DEC32;
    writed(cpu->memory, eaa, cpu->result.u32);
    CYCLES(3);
    NEXT();
}

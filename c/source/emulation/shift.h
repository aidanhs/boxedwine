void rol8_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1=*cpu->reg8[op->r1];
    U8 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (8 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 7));
    *cpu->reg8[op->r1] = result;
    CYCLES(1);
    NEXT();
}
void rol8_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (8 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 7));
    writeb(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void rol8_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (8 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 7));
    writeb(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void rol8cl_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2&=7;
    var1 = *cpu->reg8[op->r1];
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (8 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 7));
    *cpu->reg8[op->r1] = result;
    }
    CYCLES(4);
    NEXT();
}
void rol8cl_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2&=7;
    var1 = readb(cpu->memory, eaa);
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (8 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 7));
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void rol8cl_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2&=7;
    var1 = readb(cpu->memory, eaa);
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (8 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 7));
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void rol16_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1=cpu->reg[op->r1].u16;
    U16 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (16 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 15));
    cpu->reg[op->r1].u16 = result;
    CYCLES(1);
    NEXT();
}
void rol16_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (16 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 15));
    writew(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void rol16_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (16 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 15));
    writew(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void rol16cl_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2&=15;
    var1 = cpu->reg[op->r1].u16;
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (16 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 15));
    cpu->reg[op->r1].u16 = result;
    }
    CYCLES(4);
    NEXT();
}
void rol16cl_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2&=15;
    var1 = readw(cpu->memory, eaa);
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (16 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 15));
    writew(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void rol16cl_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2&=15;
    var1 = readw(cpu->memory, eaa);
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (16 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 15));
    writew(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void rol32_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (32 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 31));
    cpu->reg[op->r1].u32 = result;
    CYCLES(1);
    NEXT();
}
void rol32_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (32 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 31));
    writed(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void rol32_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (32 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 31));
    writed(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void rol32cl_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (32 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 31));
    cpu->reg[op->r1].u32 = result;
    }
    CYCLES(4);
    NEXT();
}
void rol32cl_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (32 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 31));
    writed(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void rol32cl_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoCFOF(cpu);
    result = (var1 << var2) | (var1 >> (32 - var2));
    setCF(cpu, result & 1);
    setOF(cpu, (result & 1) ^ (result >> 31));
    writed(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void ror8_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1=*cpu->reg8[op->r1];
    U8 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (8 - var2));
    setCF(cpu, result & 0x80);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    *cpu->reg8[op->r1] = result;
    CYCLES(1);
    NEXT();
}
void ror8_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (8 - var2));
    setCF(cpu, result & 0x80);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    writeb(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void ror8_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (8 - var2));
    setCF(cpu, result & 0x80);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    writeb(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void ror8cl_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2&=7;
    var1 = *cpu->reg8[op->r1];
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (8 - var2));
    setCF(cpu, result & 0x80);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    *cpu->reg8[op->r1] = result;
    }
    CYCLES(4);
    NEXT();
}
void ror8cl_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2&=7;
    var1 = readb(cpu->memory, eaa);
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (8 - var2));
    setCF(cpu, result & 0x80);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void ror8cl_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2&=7;
    var1 = readb(cpu->memory, eaa);
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (8 - var2));
    setCF(cpu, result & 0x80);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void ror16_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1=cpu->reg[op->r1].u16;
    U16 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (16 - var2));
    setCF(cpu, result & 0x8000);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    cpu->reg[op->r1].u16 = result;
    CYCLES(1);
    NEXT();
}
void ror16_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (16 - var2));
    setCF(cpu, result & 0x8000);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    writew(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void ror16_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (16 - var2));
    setCF(cpu, result & 0x8000);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    writew(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void ror16cl_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2&=15;
    var1 = cpu->reg[op->r1].u16;
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (16 - var2));
    setCF(cpu, result & 0x8000);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    cpu->reg[op->r1].u16 = result;
    }
    CYCLES(4);
    NEXT();
}
void ror16cl_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2&=15;
    var1 = readw(cpu->memory, eaa);
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (16 - var2));
    setCF(cpu, result & 0x8000);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    writew(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void ror16cl_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2&=15;
    var1 = readw(cpu->memory, eaa);
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (16 - var2));
    setCF(cpu, result & 0x8000);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    writew(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void ror32_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (32 - var2));
    setCF(cpu, result & 0x80000000);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    cpu->reg[op->r1].u32 = result;
    CYCLES(1);
    NEXT();
}
void ror32_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (32 - var2));
    setCF(cpu, result & 0x80000000);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    writed(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void ror32_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (32 - var2));
    setCF(cpu, result & 0x80000000);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    writed(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void ror32cl_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (32 - var2));
    setCF(cpu, result & 0x80000000);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    cpu->reg[op->r1].u32 = result;
    }
    CYCLES(4);
    NEXT();
}
void ror32cl_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (32 - var2));
    setCF(cpu, result & 0x80000000);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    writed(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void ror32cl_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoCFOF(cpu);
    result = (var1 >> var2) | (var1 << (32 - var2));
    setCF(cpu, result & 0x80000000);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    writed(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void rcl8_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1=*cpu->reg8[op->r1];
    U8 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (9-var2));
    setCF(cpu, ((var1 >> (8-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 7));
    *cpu->reg8[op->r1] = result;
    CYCLES(8);
    NEXT();
}
void rcl8_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (9-var2));
    setCF(cpu, ((var1 >> (8-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 7));
    writeb(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcl8_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (9-var2));
    setCF(cpu, ((var1 >> (8-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 7));
    writeb(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcl8cl_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 9;
    var1 = *cpu->reg8[op->r1];
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (9-var2));
    setCF(cpu, ((var1 >> (8-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 7));
    *cpu->reg8[op->r1] = result;
    }
    CYCLES(7);
    NEXT();
}
void rcl8cl_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 9;
    var1 = readb(cpu->memory, eaa);
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (9-var2));
    setCF(cpu, ((var1 >> (8-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 7));
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcl8cl_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 9;
    var1 = readb(cpu->memory, eaa);
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (9-var2));
    setCF(cpu, ((var1 >> (8-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 7));
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcl16_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1=cpu->reg[op->r1].u16;
    U16 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (17-var2));
    setCF(cpu, ((var1 >> (16-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 15));
    cpu->reg[op->r1].u16 = result;
    CYCLES(8);
    NEXT();
}
void rcl16_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (17-var2));
    setCF(cpu, ((var1 >> (16-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 15));
    writew(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcl16_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (17-var2));
    setCF(cpu, ((var1 >> (16-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 15));
    writew(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcl16cl_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 17;
    var1 = cpu->reg[op->r1].u16;
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (17-var2));
    setCF(cpu, ((var1 >> (16-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 15));
    cpu->reg[op->r1].u16 = result;
    }
    CYCLES(7);
    NEXT();
}
void rcl16cl_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 17;
    var1 = readw(cpu->memory, eaa);
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (17-var2));
    setCF(cpu, ((var1 >> (16-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 15));
    writew(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcl16cl_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 17;
    var1 = readw(cpu->memory, eaa);
    fillFlagsNoOF(cpu);
    result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (17-var2));
    setCF(cpu, ((var1 >> (16-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 15));
    writew(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcl32_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = op->data1;
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 << var2) | (cpu->flags & CF);
    } else {
        result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (33-var2));
    }
    setCF(cpu, ((var1 >> (32-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 31));
    cpu->reg[op->r1].u32 = result;
    CYCLES(8);
    NEXT();
}
void rcl32_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 << var2) | (cpu->flags & CF);
    } else {
        result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (33-var2));
    }
    setCF(cpu, ((var1 >> (32-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 31));
    writed(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcl32_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 << var2) | (cpu->flags & CF);
    } else {
        result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (33-var2));
    }
    setCF(cpu, ((var1 >> (32-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 31));
    writed(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcl32cl_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 << var2) | (cpu->flags & CF);
    } else {
        result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (33-var2));
    }
    setCF(cpu, ((var1 >> (32-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 31));
    cpu->reg[op->r1].u32 = result;
    }
    CYCLES(7);
    NEXT();
}
void rcl32cl_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 << var2) | (cpu->flags & CF);
    } else {
        result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (33-var2));
    }
    setCF(cpu, ((var1 >> (32-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 31));
    writed(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcl32cl_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 << var2) | (cpu->flags & CF);
    } else {
        result = (var1 << var2) | ((cpu->flags & CF) << (var2-1)) | (var1 >> (33-var2));
    }
    setCF(cpu, ((var1 >> (32-var2)) & 1));
    setOF(cpu, (cpu->flags & CF) ^ (result >> 31));
    writed(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcr8_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1=*cpu->reg8[op->r1];
    U8 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (8-var2)) | (var1 << (9-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    *cpu->reg8[op->r1] = result;
    CYCLES(8);
    NEXT();
}
void rcr8_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (8-var2)) | (var1 << (9-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    writeb(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcr8_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (8-var2)) | (var1 << (9-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    writeb(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcr8cl_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 9;
    var1 = *cpu->reg8[op->r1];
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (8-var2)) | (var1 << (9-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    *cpu->reg8[op->r1] = result;
    }
    CYCLES(7);
    NEXT();
}
void rcr8cl_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 9;
    var1 = readb(cpu->memory, eaa);
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (8-var2)) | (var1 << (9-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcr8cl_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1;
    U8 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 9;
    var1 = readb(cpu->memory, eaa);
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (8-var2)) | (var1 << (9-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80);
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcr16_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1=cpu->reg[op->r1].u16;
    U16 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (16-var2)) | (var1 << (17-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    cpu->reg[op->r1].u16 = result;
    CYCLES(8);
    NEXT();
}
void rcr16_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (16-var2)) | (var1 << (17-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    writew(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcr16_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (16-var2)) | (var1 << (17-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    writew(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcr16cl_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 17;
    var1 = cpu->reg[op->r1].u16;
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (16-var2)) | (var1 << (17-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    cpu->reg[op->r1].u16 = result;
    }
    CYCLES(7);
    NEXT();
}
void rcr16cl_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 17;
    var1 = readw(cpu->memory, eaa);
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (16-var2)) | (var1 << (17-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    writew(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcr16cl_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1;
    U16 var2 = CL & 0x1f;
    if (var2) {
    var2=var2 % 17;
    var1 = readw(cpu->memory, eaa);
    fillFlagsNoOF(cpu);
    result = (var1 >> var2) | ((cpu->flags & CF) << (16-var2)) | (var1 << (17-var2));
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x8000);
    writew(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcr32_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = op->data1;
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 >> var2) | ((cpu->flags & CF) << 31);
    } else {
        result = (var1 >> var2) | ((cpu->flags & CF) << (32-var2)) | (var1 << (33-var2));
    }
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    cpu->reg[op->r1].u32 = result;
    CYCLES(8);
    NEXT();
}
void rcr32_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 >> var2) | ((cpu->flags & CF) << 31);
    } else {
        result = (var1 >> var2) | ((cpu->flags & CF) << (32-var2)) | (var1 << (33-var2));
    }
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    writed(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcr32_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 >> var2) | ((cpu->flags & CF) << 31);
    } else {
        result = (var1 >> var2) | ((cpu->flags & CF) << (32-var2)) | (var1 << (33-var2));
    }
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    writed(cpu->memory, eaa, result);
    CYCLES(10);
    NEXT();
}
void rcr32cl_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 >> var2) | ((cpu->flags & CF) << 31);
    } else {
        result = (var1 >> var2) | ((cpu->flags & CF) << (32-var2)) | (var1 << (33-var2));
    }
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    cpu->reg[op->r1].u32 = result;
    }
    CYCLES(7);
    NEXT();
}
void rcr32cl_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 >> var2) | ((cpu->flags & CF) << 31);
    } else {
        result = (var1 >> var2) | ((cpu->flags & CF) << (32-var2)) | (var1 << (33-var2));
    }
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    writed(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void rcr32cl_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    fillFlagsNoOF(cpu);
    if (var2==1) {
        result = (var1 >> var2) | ((cpu->flags & CF) << 31);
    } else {
        result = (var1 >> var2) | ((cpu->flags & CF) << (32-var2)) | (var1 << (33-var2));
    }
    setCF(cpu, (var1 >> (var2 - 1)) & 1);
    setOF(cpu, (result ^ (result<<1)) & 0x80000000);
    writed(cpu->memory, eaa, result);
    }
    CYCLES(9);
    NEXT();
}
void shl8_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1=*cpu->reg8[op->r1];
    U8 var2 = op->data1;
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    *cpu->reg8[op->r1] = result;
    CYCLES(1);
    NEXT();
}
void shl8_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shl8_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shl8cl_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1=*cpu->reg8[op->r1];
    U8 var2 = CL & 0x1f;
    if (var2) {
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    *cpu->reg8[op->r1] = result;
    }
    CYCLES(4);
    NEXT();
}
void shl8cl_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = CL & 0x1f;
    if (var2) {
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shl8cl_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = CL & 0x1f;
    if (var2) {
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shl16_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1=cpu->reg[op->r1].u16;
    U16 var2 = op->data1;
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    cpu->reg[op->r1].u16 = result;
    CYCLES(1);
    NEXT();
}
void shl16_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shl16_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shl16cl_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1=cpu->reg[op->r1].u16;
    U16 var2 = CL & 0x1f;
    if (var2) {
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    cpu->reg[op->r1].u16 = result;
    }
    CYCLES(4);
    NEXT();
}
void shl16cl_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = CL & 0x1f;
    if (var2) {
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shl16cl_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = CL & 0x1f;
    if (var2) {
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shl32_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = op->data1;
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    cpu->reg[op->r1].u32 = result;
    CYCLES(1);
    NEXT();
}
void shl32_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shl32_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shl32cl_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = CL & 0x1f;
    if (var2) {
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    cpu->reg[op->r1].u32 = result;
    }
    CYCLES(4);
    NEXT();
}
void shl32cl_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shl32cl_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    result = var1 << var2;
    cpu->lazyFlags = FLAGS_SHL32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shr8_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1=*cpu->reg8[op->r1];
    U8 var2 = op->data1;
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    *cpu->reg8[op->r1] = result;
    CYCLES(1);
    NEXT();
}
void shr8_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shr8_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shr8cl_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1=*cpu->reg8[op->r1];
    U8 var2 = CL & 0x1f;
    if (var2) {
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    *cpu->reg8[op->r1] = result;
    }
    CYCLES(4);
    NEXT();
}
void shr8cl_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = CL & 0x1f;
    if (var2) {
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shr8cl_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = CL & 0x1f;
    if (var2) {
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shr16_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1=cpu->reg[op->r1].u16;
    U16 var2 = op->data1;
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    cpu->reg[op->r1].u16 = result;
    CYCLES(1);
    NEXT();
}
void shr16_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shr16_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shr16cl_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1=cpu->reg[op->r1].u16;
    U16 var2 = CL & 0x1f;
    if (var2) {
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    cpu->reg[op->r1].u16 = result;
    }
    CYCLES(4);
    NEXT();
}
void shr16cl_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = CL & 0x1f;
    if (var2) {
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shr16cl_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = CL & 0x1f;
    if (var2) {
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shr32_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = op->data1;
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    cpu->reg[op->r1].u32 = result;
    CYCLES(1);
    NEXT();
}
void shr32_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shr32_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void shr32cl_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = CL & 0x1f;
    if (var2) {
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    cpu->reg[op->r1].u32 = result;
    }
    CYCLES(4);
    NEXT();
}
void shr32cl_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void shr32cl_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    result = var1 >> var2;
    cpu->lazyFlags = FLAGS_SHR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void sar8_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1=*cpu->reg8[op->r1];
    U8 var2 = op->data1;
    result = (S8)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    *cpu->reg8[op->r1] = result;
    CYCLES(1);
    NEXT();
}
void sar8_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    result = (S8)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void sar8_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = op->data1;
    result = (S8)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void sar8cl_reg(struct CPU* cpu, struct Op* op) {
    U8 result;
    U8 var1=*cpu->reg8[op->r1];
    U8 var2 = CL & 0x1f;
    if (var2) {
    result = (S8)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    *cpu->reg8[op->r1] = result;
    }
    CYCLES(4);
    NEXT();
}
void sar8cl_mem16(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa16(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = CL & 0x1f;
    if (var2) {
    result = (S8)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void sar8cl_mem32(struct CPU* cpu, struct Op* op) {
    U8 result;
    U32 eaa = eaa32(cpu, op);
    U8 var1=readb(cpu->memory, eaa);
    U8 var2 = CL & 0x1f;
    if (var2) {
    result = (S8)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR8;
    cpu->result.u8 = result;
    cpu->src.u8=var2;
    cpu->dst.u8 = var1;
    writeb(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void sar16_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1=cpu->reg[op->r1].u16;
    U16 var2 = op->data1;
    result = (S16)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    cpu->reg[op->r1].u16 = result;
    CYCLES(1);
    NEXT();
}
void sar16_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    result = (S16)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void sar16_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = op->data1;
    result = (S16)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void sar16cl_reg(struct CPU* cpu, struct Op* op) {
    U16 result;
    U16 var1=cpu->reg[op->r1].u16;
    U16 var2 = CL & 0x1f;
    if (var2) {
    result = (S16)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    cpu->reg[op->r1].u16 = result;
    }
    CYCLES(4);
    NEXT();
}
void sar16cl_mem16(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa16(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = CL & 0x1f;
    if (var2) {
    result = (S16)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void sar16cl_mem32(struct CPU* cpu, struct Op* op) {
    U16 result;
    U32 eaa = eaa32(cpu, op);
    U16 var1=readw(cpu->memory, eaa);
    U16 var2 = CL & 0x1f;
    if (var2) {
    result = (S16)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR16;
    cpu->result.u16 = result;
    cpu->src.u16=var2;
    cpu->dst.u16 = var1;
    writew(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void sar32_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = op->data1;
    result = (S32)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    cpu->reg[op->r1].u32 = result;
    CYCLES(1);
    NEXT();
}
void sar32_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    result = (S32)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void sar32_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = op->data1;
    result = (S32)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    CYCLES(3);
    NEXT();
}
void sar32cl_reg(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 var1=cpu->reg[op->r1].u32;
    U32 var2 = CL & 0x1f;
    if (var2) {
    result = (S32)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    cpu->reg[op->r1].u32 = result;
    }
    CYCLES(4);
    NEXT();
}
void sar32cl_mem16(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa16(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    result = (S32)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}
void sar32cl_mem32(struct CPU* cpu, struct Op* op) {
    U32 result;
    U32 eaa = eaa32(cpu, op);
    U32 var1=readd(cpu->memory, eaa);
    U32 var2 = CL & 0x1f;
    if (var2) {
    result = (S32)var1 >> var2;
    cpu->lazyFlags = FLAGS_SAR32;
    cpu->result.u32 = result;
    cpu->src.u32=var2;
    cpu->dst.u32 = var1;
    writed(cpu->memory, eaa, result);
    }
    CYCLES(4);
    NEXT();
}

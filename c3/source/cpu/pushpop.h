void OPCALL pushReg16(struct CPU* cpu, struct Op* op){
    push16(cpu, cpu->reg[op->r1].u16);
    CYCLES(1);
    NEXT();
}
void OPCALL popReg16(struct CPU * cpu, struct Op * op){
    cpu->reg[op->r1].u16 = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushReg32(struct CPU* cpu, struct Op* op){
    push32(cpu, cpu->reg[op->r1].u32);
    CYCLES(1);
    NEXT();
}
void OPCALL popReg32(struct CPU * cpu, struct Op * op){
    cpu->reg[op->r1].u32 = pop32(cpu);
    CYCLES(1);
    NEXT();
}

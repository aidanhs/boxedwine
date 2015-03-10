void pushAx(struct CPU* cpu, struct Op* op){
    push16(cpu, AX);
    CYCLES(1);
    NEXT();
}
void popAx(struct CPU * cpu, struct Op * op){
    AX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEax(struct CPU* cpu, struct Op* op){
    push32(cpu, EAX);
    CYCLES(1);
    NEXT();
}
void popEax(struct CPU * cpu, struct Op * op){
    EAX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushCx(struct CPU* cpu, struct Op* op){
    push16(cpu, CX);
    CYCLES(1);
    NEXT();
}
void popCx(struct CPU * cpu, struct Op * op){
    CX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEcx(struct CPU* cpu, struct Op* op){
    push32(cpu, ECX);
    CYCLES(1);
    NEXT();
}
void popEcx(struct CPU * cpu, struct Op * op){
    ECX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushDx(struct CPU* cpu, struct Op* op){
    push16(cpu, DX);
    CYCLES(1);
    NEXT();
}
void popDx(struct CPU * cpu, struct Op * op){
    DX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEdx(struct CPU* cpu, struct Op* op){
    push32(cpu, EDX);
    CYCLES(1);
    NEXT();
}
void popEdx(struct CPU * cpu, struct Op * op){
    EDX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushBx(struct CPU* cpu, struct Op* op){
    push16(cpu, BX);
    CYCLES(1);
    NEXT();
}
void popBx(struct CPU * cpu, struct Op * op){
    BX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEbx(struct CPU* cpu, struct Op* op){
    push32(cpu, EBX);
    CYCLES(1);
    NEXT();
}
void popEbx(struct CPU * cpu, struct Op * op){
    EBX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushSp(struct CPU* cpu, struct Op* op){
    push16(cpu, SP);
    CYCLES(1);
    NEXT();
}
void popSp(struct CPU * cpu, struct Op * op){
    SP = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEsp(struct CPU* cpu, struct Op* op){
    push32(cpu, ESP);
    CYCLES(1);
    NEXT();
}
void popEsp(struct CPU * cpu, struct Op * op){
    ESP = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushBp(struct CPU* cpu, struct Op* op){
    push16(cpu, BP);
    CYCLES(1);
    NEXT();
}
void popBp(struct CPU * cpu, struct Op * op){
    BP = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEbp(struct CPU* cpu, struct Op* op){
    push32(cpu, EBP);
    CYCLES(1);
    NEXT();
}
void popEbp(struct CPU * cpu, struct Op * op){
    EBP = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushSi(struct CPU* cpu, struct Op* op){
    push16(cpu, SI);
    CYCLES(1);
    NEXT();
}
void popSi(struct CPU * cpu, struct Op * op){
    SI = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEsi(struct CPU* cpu, struct Op* op){
    push32(cpu, ESI);
    CYCLES(1);
    NEXT();
}
void popEsi(struct CPU * cpu, struct Op * op){
    ESI = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushDi(struct CPU* cpu, struct Op* op){
    push16(cpu, DI);
    CYCLES(1);
    NEXT();
}
void popDi(struct CPU * cpu, struct Op * op){
    DI = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEdi(struct CPU* cpu, struct Op* op){
    push32(cpu, EDI);
    CYCLES(1);
    NEXT();
}
void popEdi(struct CPU * cpu, struct Op * op){
    EDI = pop32(cpu);
    CYCLES(1);
    NEXT();
}

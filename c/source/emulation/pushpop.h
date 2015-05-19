void OPCALL pushAx(struct CPU* cpu, struct Op* op){
    push16(cpu, AX);
    CYCLES(1);
    NEXT();
}
void OPCALL popAx(struct CPU * cpu, struct Op * op){
    AX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushEax(struct CPU* cpu, struct Op* op){
    push32(cpu, EAX);
    CYCLES(1);
    NEXT();
}
void OPCALL popEax(struct CPU * cpu, struct Op * op){
    EAX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushCx(struct CPU* cpu, struct Op* op){
    push16(cpu, CX);
    CYCLES(1);
    NEXT();
}
void OPCALL popCx(struct CPU * cpu, struct Op * op){
    CX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushEcx(struct CPU* cpu, struct Op* op){
    push32(cpu, ECX);
    CYCLES(1);
    NEXT();
}
void OPCALL popEcx(struct CPU * cpu, struct Op * op){
    ECX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushDx(struct CPU* cpu, struct Op* op){
    push16(cpu, DX);
    CYCLES(1);
    NEXT();
}
void OPCALL popDx(struct CPU * cpu, struct Op * op){
    DX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushEdx(struct CPU* cpu, struct Op* op){
    push32(cpu, EDX);
    CYCLES(1);
    NEXT();
}
void OPCALL popEdx(struct CPU * cpu, struct Op * op){
    EDX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushBx(struct CPU* cpu, struct Op* op){
    push16(cpu, BX);
    CYCLES(1);
    NEXT();
}
void OPCALL popBx(struct CPU * cpu, struct Op * op){
    BX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushEbx(struct CPU* cpu, struct Op* op){
    push32(cpu, EBX);
    CYCLES(1);
    NEXT();
}
void OPCALL popEbx(struct CPU * cpu, struct Op * op){
    EBX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushSp(struct CPU* cpu, struct Op* op){
    push16(cpu, SP);
    CYCLES(1);
    NEXT();
}
void OPCALL popSp(struct CPU * cpu, struct Op * op){
    SP = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushEsp(struct CPU* cpu, struct Op* op){
    push32(cpu, ESP);
    CYCLES(1);
    NEXT();
}
void OPCALL popEsp(struct CPU * cpu, struct Op * op){
    ESP = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushBp(struct CPU* cpu, struct Op* op){
    push16(cpu, BP);
    CYCLES(1);
    NEXT();
}
void OPCALL popBp(struct CPU * cpu, struct Op * op){
    BP = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushEbp(struct CPU* cpu, struct Op* op){
    push32(cpu, EBP);
    CYCLES(1);
    NEXT();
}
void OPCALL popEbp(struct CPU * cpu, struct Op * op){
    EBP = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushSi(struct CPU* cpu, struct Op* op){
    push16(cpu, SI);
    CYCLES(1);
    NEXT();
}
void OPCALL popSi(struct CPU * cpu, struct Op * op){
    SI = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushEsi(struct CPU* cpu, struct Op* op){
    push32(cpu, ESI);
    CYCLES(1);
    NEXT();
}
void OPCALL popEsi(struct CPU * cpu, struct Op * op){
    ESI = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushDi(struct CPU* cpu, struct Op* op){
    push16(cpu, DI);
    CYCLES(1);
    NEXT();
}
void OPCALL popDi(struct CPU * cpu, struct Op * op){
    DI = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void OPCALL pushEdi(struct CPU* cpu, struct Op* op){
    push32(cpu, EDI);
    CYCLES(1);
    NEXT();
}
void OPCALL popEdi(struct CPU * cpu, struct Op * op){
    EDI = pop32(cpu);
    CYCLES(1);
    NEXT();
}

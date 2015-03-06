void pushAx(CPU * cpu, Op * op){
    push16(cpu, AX);
    CYCLES(1);
    NEXT();
}
void popAx(CPU * cpu, Op * op){
    AX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEax(CPU * cpu, Op * op){
    push32(cpu, EAX);
    CYCLES(1);
    NEXT();
}
void popEax(CPU * cpu, Op * op){
    EAX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushCx(CPU * cpu, Op * op){
    push16(cpu, CX);
    CYCLES(1);
    NEXT();
}
void popCx(CPU * cpu, Op * op){
    CX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEcx(CPU * cpu, Op * op){
    push32(cpu, ECX);
    CYCLES(1);
    NEXT();
}
void popEcx(CPU * cpu, Op * op){
    ECX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushDx(CPU * cpu, Op * op){
    push16(cpu, DX);
    CYCLES(1);
    NEXT();
}
void popDx(CPU * cpu, Op * op){
    DX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEdx(CPU * cpu, Op * op){
    push32(cpu, EDX);
    CYCLES(1);
    NEXT();
}
void popEdx(CPU * cpu, Op * op){
    EDX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushBx(CPU * cpu, Op * op){
    push16(cpu, BX);
    CYCLES(1);
    NEXT();
}
void popBx(CPU * cpu, Op * op){
    BX = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEbx(CPU * cpu, Op * op){
    push32(cpu, EBX);
    CYCLES(1);
    NEXT();
}
void popEbx(CPU * cpu, Op * op){
    EBX = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushSp(CPU * cpu, Op * op){
    push16(cpu, SP);
    CYCLES(1);
    NEXT();
}
void popSp(CPU * cpu, Op * op){
    SP = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEsp(CPU * cpu, Op * op){
    push32(cpu, ESP);
    CYCLES(1);
    NEXT();
}
void popEsp(CPU * cpu, Op * op){
    ESP = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushBp(CPU * cpu, Op * op){
    push16(cpu, BP);
    CYCLES(1);
    NEXT();
}
void popBp(CPU * cpu, Op * op){
    BP = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEbp(CPU * cpu, Op * op){
    push32(cpu, EBP);
    CYCLES(1);
    NEXT();
}
void popEbp(CPU * cpu, Op * op){
    EBP = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushSi(CPU * cpu, Op * op){
    push16(cpu, SI);
    CYCLES(1);
    NEXT();
}
void popSi(CPU * cpu, Op * op){
    SI = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEsi(CPU * cpu, Op * op){
    push32(cpu, ESI);
    CYCLES(1);
    NEXT();
}
void popEsi(CPU * cpu, Op * op){
    ESI = pop32(cpu);
    CYCLES(1);
    NEXT();
}
void pushDi(CPU * cpu, Op * op){
    push16(cpu, DI);
    CYCLES(1);
    NEXT();
}
void popDi(CPU * cpu, Op * op){
    DI = pop16(cpu);
    CYCLES(1);
    NEXT();
}
void pushEdi(CPU * cpu, Op * op){
    push32(cpu, EDI);
    CYCLES(1);
    NEXT();
}
void popEdi(CPU * cpu, Op * op){
    EDI = pop32(cpu);
    CYCLES(1);
    NEXT();
}

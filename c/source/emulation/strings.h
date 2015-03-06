void movsb32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writeb(memory, dBase+EDI, readb(memory, sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
    NEXT();
}
void movsb16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writeb(memory, dBase+DI, readb(memory, sBase+SI));
        DI+=inc;
        SI+=inc;
    }
    CX=0;
    CYCLES(3+count);
    NEXT();
}
void movsb32(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
        writeb(memory, dBase+EDI, readb(memory, sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    CYCLES(4);
    NEXT();
}
void movsb16(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
        writeb(memory, dBase+DI, readb(memory, sBase+SI));
        DI+=inc;
        SI+=inc;
    CYCLES(4);
    NEXT();
}
void movsw32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writew(memory, dBase+EDI, readw(memory, sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
    NEXT();
}
void movsw16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writew(memory, dBase+DI, readw(memory, sBase+SI));
        DI+=inc;
        SI+=inc;
    }
    CX=0;
    CYCLES(3+count);
    NEXT();
}
void movsw32(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
        writew(memory, dBase+EDI, readw(memory, sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    CYCLES(4);
    NEXT();
}
void movsw16(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
        writew(memory, dBase+DI, readw(memory, sBase+SI));
        DI+=inc;
        SI+=inc;
    CYCLES(4);
    NEXT();
}
void movsd32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writed(memory, dBase+EDI, readd(memory, sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
    NEXT();
}
void movsd16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writed(memory, dBase+DI, readd(memory, sBase+SI));
        DI+=inc;
        SI+=inc;
    }
    CX=0;
    CYCLES(3+count);
    NEXT();
}
void movsd32(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
        writed(memory, dBase+EDI, readd(memory, sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    CYCLES(4);
    NEXT();
}
void movsd16(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
        writed(memory, dBase+DI, readd(memory, sBase+SI));
        DI+=inc;
        SI+=inc;
    CYCLES(4);
    NEXT();
}
void cmpsb32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U8 v1;
    U8 v2;
    U32 count = ECX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readb(memory, dBase+EDI);
            v2 = readb(memory, sBase+ESI);
            EDI+=inc;
            ESI+=inc;
            ECX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u8 = v1;
        cpu->src.u8 = v2;
        cpu->result.u8 = v1 - v2;
        cpu->inst = FLAGS_SUB8;
    }
    CYCLES(9+4*count);
    NEXT();
}
void cmpsb16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U8 v1;
    U8 v2;
    U32 count = CX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readb(memory, dBase+DI);
            v2 = readb(memory, sBase+SI);
            DI+=inc;
            SI+=inc;
            CX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u8 = v1;
        cpu->src.u8 = v2;
        cpu->result.u8 = v1 - v2;
        cpu->inst = FLAGS_SUB8;
    }
    CYCLES(9+4*count);
    NEXT();
}
void cmpsb32(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U8 v1;
    U8 v2;
            v1 = readb(memory, dBase+EDI);
            v2 = readb(memory, sBase+ESI);
            EDI+=inc;
            ESI+=inc;
        cpu->dst.u8 = v1;
        cpu->src.u8 = v2;
        cpu->result.u8 = v1 - v2;
        cpu->inst = FLAGS_SUB8;
    CYCLES(5);
    NEXT();
}
void cmpsb16(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U8 v1;
    U8 v2;
            v1 = readb(memory, dBase+DI);
            v2 = readb(memory, sBase+SI);
            DI+=inc;
            SI+=inc;
        cpu->dst.u8 = v1;
        cpu->src.u8 = v2;
        cpu->result.u8 = v1 - v2;
        cpu->inst = FLAGS_SUB8;
    CYCLES(5);
    NEXT();
}
void cmpsw32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U16 v1;
    U16 v2;
    U32 count = ECX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readw(memory, dBase+EDI);
            v2 = readw(memory, sBase+ESI);
            EDI+=inc;
            ESI+=inc;
            ECX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u16 = v1;
        cpu->src.u16 = v2;
        cpu->result.u16 = v1 - v2;
        cpu->inst = FLAGS_SUB16;
    }
    CYCLES(9+4*count);
    NEXT();
}
void cmpsw16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U16 v1;
    U16 v2;
    U32 count = CX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readw(memory, dBase+DI);
            v2 = readw(memory, sBase+SI);
            DI+=inc;
            SI+=inc;
            CX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u16 = v1;
        cpu->src.u16 = v2;
        cpu->result.u16 = v1 - v2;
        cpu->inst = FLAGS_SUB16;
    }
    CYCLES(9+4*count);
    NEXT();
}
void cmpsw32(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U16 v1;
    U16 v2;
            v1 = readw(memory, dBase+EDI);
            v2 = readw(memory, sBase+ESI);
            EDI+=inc;
            ESI+=inc;
        cpu->dst.u16 = v1;
        cpu->src.u16 = v2;
        cpu->result.u16 = v1 - v2;
        cpu->inst = FLAGS_SUB16;
    CYCLES(5);
    NEXT();
}
void cmpsw16(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U16 v1;
    U16 v2;
            v1 = readw(memory, dBase+DI);
            v2 = readw(memory, sBase+SI);
            DI+=inc;
            SI+=inc;
        cpu->dst.u16 = v1;
        cpu->src.u16 = v2;
        cpu->result.u16 = v1 - v2;
        cpu->inst = FLAGS_SUB16;
    CYCLES(5);
    NEXT();
}
void cmpsd32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 v1;
    U32 v2;
    U32 count = ECX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readd(memory, dBase+EDI);
            v2 = readd(memory, sBase+ESI);
            EDI+=inc;
            ESI+=inc;
            ECX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u32 = v1;
        cpu->src.u32 = v2;
        cpu->result.u32 = v1 - v2;
        cpu->inst = FLAGS_SUB32;
    }
    CYCLES(9+4*count);
    NEXT();
}
void cmpsd16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 v1;
    U32 v2;
    U32 count = CX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readd(memory, dBase+DI);
            v2 = readd(memory, sBase+SI);
            DI+=inc;
            SI+=inc;
            CX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u32 = v1;
        cpu->src.u32 = v2;
        cpu->result.u32 = v1 - v2;
        cpu->inst = FLAGS_SUB32;
    }
    CYCLES(9+4*count);
    NEXT();
}
void cmpsd32(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 v1;
    U32 v2;
            v1 = readd(memory, dBase+EDI);
            v2 = readd(memory, sBase+ESI);
            EDI+=inc;
            ESI+=inc;
        cpu->dst.u32 = v1;
        cpu->src.u32 = v2;
        cpu->result.u32 = v1 - v2;
        cpu->inst = FLAGS_SUB32;
    CYCLES(5);
    NEXT();
}
void cmpsd16(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 v1;
    U32 v2;
            v1 = readd(memory, dBase+DI);
            v2 = readd(memory, sBase+SI);
            DI+=inc;
            SI+=inc;
        cpu->dst.u32 = v1;
        cpu->src.u32 = v2;
        cpu->result.u32 = v1 - v2;
        cpu->inst = FLAGS_SUB32;
    CYCLES(5);
    NEXT();
}
void stosb32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writeb(memory, dBase+EDI, AL);
        EDI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
    NEXT();
}
void stosb16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writeb(memory, dBase+DI, AL);
        DI+=inc;
    }
    CX=0;
    CYCLES(3+count);
    NEXT();
}
void stosb32(CPU* cpu, Op* op) {
    writeb(cpu->memory, cpu->segAddress[ES]+EDI, AL);
    EDI+=cpu->df;
    CYCLES(3);
    NEXT();
}
void stosb16(CPU* cpu, Op* op) {
    writeb(cpu->memory, cpu->segAddress[ES]+DI, AL);
    DI+=cpu->df;
    CYCLES(3);
    NEXT();
}
void stosw32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writew(memory, dBase+EDI, AX);
        EDI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
    NEXT();
}
void stosw16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writew(memory, dBase+DI, AX);
        DI+=inc;
    }
    CX=0;
    CYCLES(3+count);
    NEXT();
}
void stosw32(CPU* cpu, Op* op) {
    writew(cpu->memory, cpu->segAddress[ES]+EDI, AX);
    EDI+=cpu->df << 1;
    CYCLES(3);
    NEXT();
}
void stosw16(CPU* cpu, Op* op) {
    writew(cpu->memory, cpu->segAddress[ES]+DI, AX);
    DI+=cpu->df << 1;
    CYCLES(3);
    NEXT();
}
void stosd32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writed(memory, dBase+EDI, EAX);
        EDI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
    NEXT();
}
void stosd16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writed(memory, dBase+DI, EAX);
        DI+=inc;
    }
    CX=0;
    CYCLES(3+count);
    NEXT();
}
void stosd32(CPU* cpu, Op* op) {
    writed(cpu->memory, cpu->segAddress[ES]+EDI, EAX);
    EDI+=cpu->df << 2;
    CYCLES(3);
    NEXT();
}
void stosd16(CPU* cpu, Op* op) {
    writed(cpu->memory, cpu->segAddress[ES]+DI, EAX);
    DI+=cpu->df << 2;
    CYCLES(3);
    NEXT();
}
void lodsb32_r(CPU* cpu, Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        AL = readb(memory, sBase+ESI);
        ESI+=inc;
    }
    ECX=0;
    CYCLES(2);
    NEXT();
}
void lodsb16_r(CPU* cpu, Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        AL = readb(memory, sBase+SI);
        SI+=inc;
    }
    CX=0;
    CYCLES(2);
    NEXT();
}
void lodsb32(CPU* cpu, Op* op) {
    AL = readb(cpu->memory, cpu->segAddress[op->base]+ESI);
    ESI+=cpu->df;
    CYCLES(2);
    NEXT();
}
void lodsb16(CPU* cpu, Op* op) {
    AL = readb(cpu->memory, cpu->segAddress[op->base]+SI);
    SI+=cpu->df;
    CYCLES(2);
    NEXT();
}
void lodsw32_r(CPU* cpu, Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        AX = readw(memory, sBase+ESI);
        ESI+=inc;
    }
    ECX=0;
    CYCLES(2);
    NEXT();
}
void lodsw16_r(CPU* cpu, Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        AX = readw(memory, sBase+SI);
        SI+=inc;
    }
    CX=0;
    CYCLES(2);
    NEXT();
}
void lodsw32(CPU* cpu, Op* op) {
    AX = readw(cpu->memory, cpu->segAddress[op->base]+ESI);
    ESI+=cpu->df << 1;
    CYCLES(2);
    NEXT();
}
void lodsw16(CPU* cpu, Op* op) {
    AX = readw(cpu->memory, cpu->segAddress[op->base]+SI);
    SI+=cpu->df << 1;
    CYCLES(2);
    NEXT();
}
void lodsd32_r(CPU* cpu, Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        EAX = readd(memory, sBase+ESI);
        ESI+=inc;
    }
    ECX=0;
    CYCLES(2);
    NEXT();
}
void lodsd16_r(CPU* cpu, Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        EAX = readd(memory, sBase+SI);
        SI+=inc;
    }
    CX=0;
    CYCLES(2);
    NEXT();
}
void lodsd32(CPU* cpu, Op* op) {
    EAX = readd(cpu->memory, cpu->segAddress[op->base]+ESI);
    ESI+=cpu->df << 2;
    CYCLES(2);
    NEXT();
}
void lodsd16(CPU* cpu, Op* op) {
    EAX = readd(cpu->memory, cpu->segAddress[op->base]+SI);
    SI+=cpu->df << 2;
    CYCLES(2);
    NEXT();
}
void scasb32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U8 v1;
    U32 count = ECX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readb(memory, dBase+EDI);
            EDI+=inc;
            ECX--;
            if ((AL==v1)!=rep_zero) break;
        }
        cpu->dst.u8 = AL;
        cpu->src.u8 = v1;
        cpu->result.u8 = AL - v1;
        cpu->inst = FLAGS_SUB8;
    }
    CYCLES(8+4*count);
    NEXT();
}
void scasb16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U8 v1;
    U32 count = CX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readb(memory, dBase+DI);
            DI+=inc;
            CX--;
            if ((AL==v1)!=rep_zero) break;
        }
        cpu->dst.u8 = AL;
        cpu->src.u8 = v1;
        cpu->result.u8 = AL - v1;
        cpu->inst = FLAGS_SUB8;
    }
    CYCLES(8+4*count);
    NEXT();
}
void scasb32(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U8 v1;
            v1 = readb(memory, dBase+EDI);
            EDI+=inc;
        cpu->dst.u8 = AL;
        cpu->src.u8 = v1;
        cpu->result.u8 = AL - v1;
        cpu->inst = FLAGS_SUB8;
    CYCLES(4);
    NEXT();
}
void scasb16(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df;
    Memory* memory = cpu->memory;
    U8 v1;
            v1 = readb(memory, dBase+DI);
            DI+=inc;
        cpu->dst.u8 = AL;
        cpu->src.u8 = v1;
        cpu->result.u8 = AL - v1;
        cpu->inst = FLAGS_SUB8;
    CYCLES(4);
    NEXT();
}
void scasw32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U16 v1;
    U32 count = ECX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readw(memory, dBase+EDI);
            EDI+=inc;
            ECX--;
            if ((AX==v1)!=rep_zero) break;
        }
        cpu->dst.u16 = AX;
        cpu->src.u16 = v1;
        cpu->result.u16 = AX - v1;
        cpu->inst = FLAGS_SUB16;
    }
    CYCLES(8+4*count);
    NEXT();
}
void scasw16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U16 v1;
    U32 count = CX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readw(memory, dBase+DI);
            DI+=inc;
            CX--;
            if ((AX==v1)!=rep_zero) break;
        }
        cpu->dst.u16 = AX;
        cpu->src.u16 = v1;
        cpu->result.u16 = AX - v1;
        cpu->inst = FLAGS_SUB16;
    }
    CYCLES(8+4*count);
    NEXT();
}
void scasw32(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U16 v1;
            v1 = readw(memory, dBase+EDI);
            EDI+=inc;
        cpu->dst.u16 = AX;
        cpu->src.u16 = v1;
        cpu->result.u16 = AX - v1;
        cpu->inst = FLAGS_SUB16;
    CYCLES(4);
    NEXT();
}
void scasw16(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 1;
    Memory* memory = cpu->memory;
    U16 v1;
            v1 = readw(memory, dBase+DI);
            DI+=inc;
        cpu->dst.u16 = AX;
        cpu->src.u16 = v1;
        cpu->result.u16 = AX - v1;
        cpu->inst = FLAGS_SUB16;
    CYCLES(4);
    NEXT();
}
void scasd32_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 v1;
    U32 count = ECX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readd(memory, dBase+EDI);
            EDI+=inc;
            ECX--;
            if ((EAX==v1)!=rep_zero) break;
        }
        cpu->dst.u32 = EAX;
        cpu->src.u32 = v1;
        cpu->result.u32 = EAX - v1;
        cpu->inst = FLAGS_SUB32;
    }
    CYCLES(8+4*count);
    NEXT();
}
void scasd16_r(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 v1;
    U32 count = CX;
    U32 i;
    U32 rep_zero = op->data1;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readd(memory, dBase+DI);
            DI+=inc;
            CX--;
            if ((EAX==v1)!=rep_zero) break;
        }
        cpu->dst.u32 = EAX;
        cpu->src.u32 = v1;
        cpu->result.u32 = EAX - v1;
        cpu->inst = FLAGS_SUB32;
    }
    CYCLES(8+4*count);
    NEXT();
}
void scasd32(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 v1;
            v1 = readd(memory, dBase+EDI);
            EDI+=inc;
        cpu->dst.u32 = EAX;
        cpu->src.u32 = v1;
        cpu->result.u32 = EAX - v1;
        cpu->inst = FLAGS_SUB32;
    CYCLES(4);
    NEXT();
}
void scasd16(CPU* cpu, Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 inc = cpu->df << 2;
    Memory* memory = cpu->memory;
    U32 v1;
            v1 = readd(memory, dBase+DI);
            DI+=inc;
        cpu->dst.u32 = EAX;
        cpu->src.u32 = v1;
        cpu->result.u32 = EAX - v1;
        cpu->inst = FLAGS_SUB32;
    CYCLES(4);
    NEXT();
}

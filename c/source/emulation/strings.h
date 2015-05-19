void OPCALL movsb32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
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
void OPCALL movsb16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
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
void OPCALL movsb32(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
        writeb(memory, dBase+EDI, readb(memory, sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    CYCLES(4);
    NEXT();
}
void OPCALL movsb16(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
        writeb(memory, dBase+DI, readb(memory, sBase+SI));
        DI+=inc;
        SI+=inc;
    CYCLES(4);
    NEXT();
}
void OPCALL movsw32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
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
void OPCALL movsw16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
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
void OPCALL movsw32(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
        writew(memory, dBase+EDI, readw(memory, sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    CYCLES(4);
    NEXT();
}
void OPCALL movsw16(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
        writew(memory, dBase+DI, readw(memory, sBase+SI));
        DI+=inc;
        SI+=inc;
    CYCLES(4);
    NEXT();
}
void OPCALL movsd32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
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
void OPCALL movsd16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
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
void OPCALL movsd32(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
        writed(memory, dBase+EDI, readd(memory, sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    CYCLES(4);
    NEXT();
}
void OPCALL movsd16(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
        writed(memory, dBase+DI, readd(memory, sBase+SI));
        DI+=inc;
        SI+=inc;
    CYCLES(4);
    NEXT();
}
void OPCALL cmpsb32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB8;
    }
    CYCLES(9+4*count);
    NEXT();
}
void OPCALL cmpsb16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB8;
    }
    CYCLES(9+4*count);
    NEXT();
}
void OPCALL cmpsb32(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
    U8 v1;
    U8 v2;
            v1 = readb(memory, dBase+EDI);
            v2 = readb(memory, sBase+ESI);
            EDI+=inc;
            ESI+=inc;
        cpu->dst.u8 = v1;
        cpu->src.u8 = v2;
        cpu->result.u8 = v1 - v2;
        cpu->lazyFlags = FLAGS_SUB8;
    CYCLES(5);
    NEXT();
}
void OPCALL cmpsb16(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
    U8 v1;
    U8 v2;
            v1 = readb(memory, dBase+DI);
            v2 = readb(memory, sBase+SI);
            DI+=inc;
            SI+=inc;
        cpu->dst.u8 = v1;
        cpu->src.u8 = v2;
        cpu->result.u8 = v1 - v2;
        cpu->lazyFlags = FLAGS_SUB8;
    CYCLES(5);
    NEXT();
}
void OPCALL cmpsw32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB16;
    }
    CYCLES(9+4*count);
    NEXT();
}
void OPCALL cmpsw16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB16;
    }
    CYCLES(9+4*count);
    NEXT();
}
void OPCALL cmpsw32(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
    U16 v1;
    U16 v2;
            v1 = readw(memory, dBase+EDI);
            v2 = readw(memory, sBase+ESI);
            EDI+=inc;
            ESI+=inc;
        cpu->dst.u16 = v1;
        cpu->src.u16 = v2;
        cpu->result.u16 = v1 - v2;
        cpu->lazyFlags = FLAGS_SUB16;
    CYCLES(5);
    NEXT();
}
void OPCALL cmpsw16(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
    U16 v1;
    U16 v2;
            v1 = readw(memory, dBase+DI);
            v2 = readw(memory, sBase+SI);
            DI+=inc;
            SI+=inc;
        cpu->dst.u16 = v1;
        cpu->src.u16 = v2;
        cpu->result.u16 = v1 - v2;
        cpu->lazyFlags = FLAGS_SUB16;
    CYCLES(5);
    NEXT();
}
void OPCALL cmpsd32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB32;
    }
    CYCLES(9+4*count);
    NEXT();
}
void OPCALL cmpsd16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB32;
    }
    CYCLES(9+4*count);
    NEXT();
}
void OPCALL cmpsd32(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
    U32 v1;
    U32 v2;
            v1 = readd(memory, dBase+EDI);
            v2 = readd(memory, sBase+ESI);
            EDI+=inc;
            ESI+=inc;
        cpu->dst.u32 = v1;
        cpu->src.u32 = v2;
        cpu->result.u32 = v1 - v2;
        cpu->lazyFlags = FLAGS_SUB32;
    CYCLES(5);
    NEXT();
}
void OPCALL cmpsd16(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
    U32 v1;
    U32 v2;
            v1 = readd(memory, dBase+DI);
            v2 = readd(memory, sBase+SI);
            DI+=inc;
            SI+=inc;
        cpu->dst.u32 = v1;
        cpu->src.u32 = v2;
        cpu->result.u32 = v1 - v2;
        cpu->lazyFlags = FLAGS_SUB32;
    CYCLES(5);
    NEXT();
}
void OPCALL stosb32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
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
void OPCALL stosb16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
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
void OPCALL stosb32(struct CPU* cpu, struct Op* op) {
    writeb(cpu->memory, cpu->segAddress[ES]+EDI, AL);
    EDI+=cpu->df;
    CYCLES(3);
    NEXT();
}
void OPCALL stosb16(struct CPU* cpu, struct Op* op) {
    writeb(cpu->memory, cpu->segAddress[ES]+DI, AL);
    DI+=cpu->df;
    CYCLES(3);
    NEXT();
}
void OPCALL stosw32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
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
void OPCALL stosw16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
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
void OPCALL stosw32(struct CPU* cpu, struct Op* op) {
    writew(cpu->memory, cpu->segAddress[ES]+EDI, AX);
    EDI+=cpu->df << 1;
    CYCLES(3);
    NEXT();
}
void OPCALL stosw16(struct CPU* cpu, struct Op* op) {
    writew(cpu->memory, cpu->segAddress[ES]+DI, AX);
    DI+=cpu->df << 1;
    CYCLES(3);
    NEXT();
}
void OPCALL stosd32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
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
void OPCALL stosd16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
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
void OPCALL stosd32(struct CPU* cpu, struct Op* op) {
    writed(cpu->memory, cpu->segAddress[ES]+EDI, EAX);
    EDI+=cpu->df << 2;
    CYCLES(3);
    NEXT();
}
void OPCALL stosd16(struct CPU* cpu, struct Op* op) {
    writed(cpu->memory, cpu->segAddress[ES]+DI, EAX);
    DI+=cpu->df << 2;
    CYCLES(3);
    NEXT();
}
void OPCALL lodsb32_r(struct CPU* cpu, struct Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
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
void OPCALL lodsb16_r(struct CPU* cpu, struct Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
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
void OPCALL lodsb32(struct CPU* cpu, struct Op* op) {
    AL = readb(cpu->memory, cpu->segAddress[op->base]+ESI);
    ESI+=cpu->df;
    CYCLES(2);
    NEXT();
}
void OPCALL lodsb16(struct CPU* cpu, struct Op* op) {
    AL = readb(cpu->memory, cpu->segAddress[op->base]+SI);
    SI+=cpu->df;
    CYCLES(2);
    NEXT();
}
void OPCALL lodsw32_r(struct CPU* cpu, struct Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
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
void OPCALL lodsw16_r(struct CPU* cpu, struct Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
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
void OPCALL lodsw32(struct CPU* cpu, struct Op* op) {
    AX = readw(cpu->memory, cpu->segAddress[op->base]+ESI);
    ESI+=cpu->df << 1;
    CYCLES(2);
    NEXT();
}
void OPCALL lodsw16(struct CPU* cpu, struct Op* op) {
    AX = readw(cpu->memory, cpu->segAddress[op->base]+SI);
    SI+=cpu->df << 1;
    CYCLES(2);
    NEXT();
}
void OPCALL lodsd32_r(struct CPU* cpu, struct Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
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
void OPCALL lodsd16_r(struct CPU* cpu, struct Op* op) {
    U32 sBase = cpu->segAddress[op->base];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
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
void OPCALL lodsd32(struct CPU* cpu, struct Op* op) {
    EAX = readd(cpu->memory, cpu->segAddress[op->base]+ESI);
    ESI+=cpu->df << 2;
    CYCLES(2);
    NEXT();
}
void OPCALL lodsd16(struct CPU* cpu, struct Op* op) {
    EAX = readd(cpu->memory, cpu->segAddress[op->base]+SI);
    SI+=cpu->df << 2;
    CYCLES(2);
    NEXT();
}
void OPCALL scasb32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB8;
    }
    CYCLES(8+4*count);
    NEXT();
}
void OPCALL scasb16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB8;
    }
    CYCLES(8+4*count);
    NEXT();
}
void OPCALL scasb32(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
    U8 v1;
            v1 = readb(memory, dBase+EDI);
            EDI+=inc;
        cpu->dst.u8 = AL;
        cpu->src.u8 = v1;
        cpu->result.u8 = AL - v1;
        cpu->lazyFlags = FLAGS_SUB8;
    CYCLES(4);
    NEXT();
}
void OPCALL scasb16(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    struct Memory* memory = cpu->memory;
    U8 v1;
            v1 = readb(memory, dBase+DI);
            DI+=inc;
        cpu->dst.u8 = AL;
        cpu->src.u8 = v1;
        cpu->result.u8 = AL - v1;
        cpu->lazyFlags = FLAGS_SUB8;
    CYCLES(4);
    NEXT();
}
void OPCALL scasw32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB16;
    }
    CYCLES(8+4*count);
    NEXT();
}
void OPCALL scasw16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB16;
    }
    CYCLES(8+4*count);
    NEXT();
}
void OPCALL scasw32(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
    U16 v1;
            v1 = readw(memory, dBase+EDI);
            EDI+=inc;
        cpu->dst.u16 = AX;
        cpu->src.u16 = v1;
        cpu->result.u16 = AX - v1;
        cpu->lazyFlags = FLAGS_SUB16;
    CYCLES(4);
    NEXT();
}
void OPCALL scasw16(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    struct Memory* memory = cpu->memory;
    U16 v1;
            v1 = readw(memory, dBase+DI);
            DI+=inc;
        cpu->dst.u16 = AX;
        cpu->src.u16 = v1;
        cpu->result.u16 = AX - v1;
        cpu->lazyFlags = FLAGS_SUB16;
    CYCLES(4);
    NEXT();
}
void OPCALL scasd32_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB32;
    }
    CYCLES(8+4*count);
    NEXT();
}
void OPCALL scasd16_r(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
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
        cpu->lazyFlags = FLAGS_SUB32;
    }
    CYCLES(8+4*count);
    NEXT();
}
void OPCALL scasd32(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
    U32 v1;
            v1 = readd(memory, dBase+EDI);
            EDI+=inc;
        cpu->dst.u32 = EAX;
        cpu->src.u32 = v1;
        cpu->result.u32 = EAX - v1;
        cpu->lazyFlags = FLAGS_SUB32;
    CYCLES(4);
    NEXT();
}
void OPCALL scasd16(struct CPU* cpu, struct Op* op) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    struct Memory* memory = cpu->memory;
    U32 v1;
            v1 = readd(memory, dBase+DI);
            DI+=inc;
        cpu->dst.u32 = EAX;
        cpu->src.u32 = v1;
        cpu->result.u32 = EAX - v1;
        cpu->lazyFlags = FLAGS_SUB32;
    CYCLES(4);
    NEXT();
}

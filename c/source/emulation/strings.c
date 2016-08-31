#include "cpu.h"
#include "decoder.h"
void movsb32_r(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writeb(MMU_PARAM_CPU dBase+EDI, readb(MMU_PARAM_CPU sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
}
void movsb16_r(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writeb(MMU_PARAM_CPU dBase+DI, readb(MMU_PARAM_CPU sBase+SI));
        DI+=inc;
        SI+=inc;
    }
    CX=0;
    CYCLES(3+count);
}
void movsb32(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df;
        writeb(MMU_PARAM_CPU dBase+EDI, readb(MMU_PARAM_CPU sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    CYCLES(4);
}
void movsb16(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df;
        writeb(MMU_PARAM_CPU dBase+DI, readb(MMU_PARAM_CPU sBase+SI));
        DI+=inc;
        SI+=inc;
    CYCLES(4);
}
void movsw32_r(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 1;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writew(MMU_PARAM_CPU dBase+EDI, readw(MMU_PARAM_CPU sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
}
void movsw16_r(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 1;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writew(MMU_PARAM_CPU dBase+DI, readw(MMU_PARAM_CPU sBase+SI));
        DI+=inc;
        SI+=inc;
    }
    CX=0;
    CYCLES(3+count);
}
void movsw32(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 1;
        writew(MMU_PARAM_CPU dBase+EDI, readw(MMU_PARAM_CPU sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    CYCLES(4);
}
void movsw16(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 1;
        writew(MMU_PARAM_CPU dBase+DI, readw(MMU_PARAM_CPU sBase+SI));
        DI+=inc;
        SI+=inc;
    CYCLES(4);
}
void movsd32_r(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 2;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writed(MMU_PARAM_CPU dBase+EDI, readd(MMU_PARAM_CPU sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
}
void movsd16_r(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 2;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writed(MMU_PARAM_CPU dBase+DI, readd(MMU_PARAM_CPU sBase+SI));
        DI+=inc;
        SI+=inc;
    }
    CX=0;
    CYCLES(3+count);
}
void movsd32(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 2;
        writed(MMU_PARAM_CPU dBase+EDI, readd(MMU_PARAM_CPU sBase+ESI));
        EDI+=inc;
        ESI+=inc;
    CYCLES(4);
}
void movsd16(struct CPU* cpu, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 2;
        writed(MMU_PARAM_CPU dBase+DI, readd(MMU_PARAM_CPU sBase+SI));
        DI+=inc;
        SI+=inc;
    CYCLES(4);
}
void cmpsb32_r(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df;
    U8 v1;
    U8 v2;
    U32 count = ECX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readb(MMU_PARAM_CPU dBase+EDI);
            v2 = readb(MMU_PARAM_CPU sBase+ESI);
            EDI+=inc;
            ESI+=inc;
            ECX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u8 = v2;
        cpu->src.u8 = v1;
        cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
        cpu->lazyFlags = FLAGS_SUB8;
    }
    CYCLES(9+4*count);
}
void cmpsb16_r(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df;
    U8 v1;
    U8 v2;
    U32 count = CX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readb(MMU_PARAM_CPU dBase+DI);
            v2 = readb(MMU_PARAM_CPU sBase+SI);
            DI+=inc;
            SI+=inc;
            CX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u8 = v2;
        cpu->src.u8 = v1;
        cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
        cpu->lazyFlags = FLAGS_SUB8;
    }
    CYCLES(9+4*count);
}
void cmpsb32(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df;
    U8 v1;
    U8 v2;
            v1 = readb(MMU_PARAM_CPU dBase+EDI);
            v2 = readb(MMU_PARAM_CPU sBase+ESI);
            EDI+=inc;
            ESI+=inc;
        cpu->dst.u8 = v2;
        cpu->src.u8 = v1;
        cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
        cpu->lazyFlags = FLAGS_SUB8;
    CYCLES(5);
}
void cmpsb16(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df;
    U8 v1;
    U8 v2;
            v1 = readb(MMU_PARAM_CPU dBase+DI);
            v2 = readb(MMU_PARAM_CPU sBase+SI);
            DI+=inc;
            SI+=inc;
        cpu->dst.u8 = v2;
        cpu->src.u8 = v1;
        cpu->result.u8 = cpu->dst.u8 - cpu->src.u8;
        cpu->lazyFlags = FLAGS_SUB8;
    CYCLES(5);
}
void cmpsw32_r(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 1;
    U16 v1;
    U16 v2;
    U32 count = ECX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readw(MMU_PARAM_CPU dBase+EDI);
            v2 = readw(MMU_PARAM_CPU sBase+ESI);
            EDI+=inc;
            ESI+=inc;
            ECX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u16 = v2;
        cpu->src.u16 = v1;
        cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
        cpu->lazyFlags = FLAGS_SUB16;
    }
    CYCLES(9+4*count);
}
void cmpsw16_r(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 1;
    U16 v1;
    U16 v2;
    U32 count = CX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readw(MMU_PARAM_CPU dBase+DI);
            v2 = readw(MMU_PARAM_CPU sBase+SI);
            DI+=inc;
            SI+=inc;
            CX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u16 = v2;
        cpu->src.u16 = v1;
        cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
        cpu->lazyFlags = FLAGS_SUB16;
    }
    CYCLES(9+4*count);
}
void cmpsw32(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 1;
    U16 v1;
    U16 v2;
            v1 = readw(MMU_PARAM_CPU dBase+EDI);
            v2 = readw(MMU_PARAM_CPU sBase+ESI);
            EDI+=inc;
            ESI+=inc;
        cpu->dst.u16 = v2;
        cpu->src.u16 = v1;
        cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
        cpu->lazyFlags = FLAGS_SUB16;
    CYCLES(5);
}
void cmpsw16(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 1;
    U16 v1;
    U16 v2;
            v1 = readw(MMU_PARAM_CPU dBase+DI);
            v2 = readw(MMU_PARAM_CPU sBase+SI);
            DI+=inc;
            SI+=inc;
        cpu->dst.u16 = v2;
        cpu->src.u16 = v1;
        cpu->result.u16 = cpu->dst.u16 - cpu->src.u16;
        cpu->lazyFlags = FLAGS_SUB16;
    CYCLES(5);
}
void cmpsd32_r(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 2;
    U32 v1;
    U32 v2;
    U32 count = ECX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readd(MMU_PARAM_CPU dBase+EDI);
            v2 = readd(MMU_PARAM_CPU sBase+ESI);
            EDI+=inc;
            ESI+=inc;
            ECX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u32 = v2;
        cpu->src.u32 = v1;
        cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
        cpu->lazyFlags = FLAGS_SUB32;
    }
    CYCLES(9+4*count);
}
void cmpsd16_r(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 2;
    U32 v1;
    U32 v2;
    U32 count = CX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readd(MMU_PARAM_CPU dBase+DI);
            v2 = readd(MMU_PARAM_CPU sBase+SI);
            DI+=inc;
            SI+=inc;
            CX--;
            if ((v1==v2)!=rep_zero) break;
        }
        cpu->dst.u32 = v2;
        cpu->src.u32 = v1;
        cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
        cpu->lazyFlags = FLAGS_SUB32;
    }
    CYCLES(9+4*count);
}
void cmpsd32(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 2;
    U32 v1;
    U32 v2;
            v1 = readd(MMU_PARAM_CPU dBase+EDI);
            v2 = readd(MMU_PARAM_CPU sBase+ESI);
            EDI+=inc;
            ESI+=inc;
        cpu->dst.u32 = v2;
        cpu->src.u32 = v1;
        cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
        cpu->lazyFlags = FLAGS_SUB32;
    CYCLES(5);
}
void cmpsd16(struct CPU* cpu, U32 rep_zero, U32 base) {
    U32 dBase = cpu->segAddress[ES];
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 2;
    U32 v1;
    U32 v2;
            v1 = readd(MMU_PARAM_CPU dBase+DI);
            v2 = readd(MMU_PARAM_CPU sBase+SI);
            DI+=inc;
            SI+=inc;
        cpu->dst.u32 = v2;
        cpu->src.u32 = v1;
        cpu->result.u32 = cpu->dst.u32 - cpu->src.u32;
        cpu->lazyFlags = FLAGS_SUB32;
    CYCLES(5);
}
void stosb32_r(struct CPU* cpu) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writeb(MMU_PARAM_CPU dBase+EDI, AL);
        EDI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
}
void stosb16_r(struct CPU* cpu) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writeb(MMU_PARAM_CPU dBase+DI, AL);
        DI+=inc;
    }
    CX=0;
    CYCLES(3+count);
}
void stosb32(struct CPU* cpu) {
    writeb(MMU_PARAM_CPU cpu->segAddress[ES]+EDI, AL);
    EDI+=cpu->df;
    CYCLES(3);
}
void stosb16(struct CPU* cpu) {
    writeb(MMU_PARAM_CPU cpu->segAddress[ES]+DI, AL);
    DI+=cpu->df;
    CYCLES(3);
}
void stosw32_r(struct CPU* cpu) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writew(MMU_PARAM_CPU dBase+EDI, AX);
        EDI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
}
void stosw16_r(struct CPU* cpu) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writew(MMU_PARAM_CPU dBase+DI, AX);
        DI+=inc;
    }
    CX=0;
    CYCLES(3+count);
}
void stosw32(struct CPU* cpu) {
    writew(MMU_PARAM_CPU cpu->segAddress[ES]+EDI, AX);
    EDI+=cpu->df << 1;
    CYCLES(3);
}
void stosw16(struct CPU* cpu) {
    writew(MMU_PARAM_CPU cpu->segAddress[ES]+DI, AX);
    DI+=cpu->df << 1;
    CYCLES(3);
}
void stosd32_r(struct CPU* cpu) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        writed(MMU_PARAM_CPU dBase+EDI, EAX);
        EDI+=inc;
    }
    ECX=0;
    CYCLES(3+count);
}
void stosd16_r(struct CPU* cpu) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        writed(MMU_PARAM_CPU dBase+DI, EAX);
        DI+=inc;
    }
    CX=0;
    CYCLES(3+count);
}
void stosd32(struct CPU* cpu) {
    writed(MMU_PARAM_CPU cpu->segAddress[ES]+EDI, EAX);
    EDI+=cpu->df << 2;
    CYCLES(3);
}
void stosd16(struct CPU* cpu) {
    writed(MMU_PARAM_CPU cpu->segAddress[ES]+DI, EAX);
    DI+=cpu->df << 2;
    CYCLES(3);
}
void lodsb32_r(struct CPU* cpu, U32 base) {
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        AL = readb(MMU_PARAM_CPU sBase+ESI);
        ESI+=inc;
    }
    ECX=0;
    CYCLES(2);
}
void lodsb16_r(struct CPU* cpu, U32 base) {
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        AL = readb(MMU_PARAM_CPU sBase+SI);
        SI+=inc;
    }
    CX=0;
    CYCLES(2);
}
void lodsb32(struct CPU* cpu, U32 base) {
    AL = readb(MMU_PARAM_CPU cpu->segAddress[base]+ESI);
    ESI+=cpu->df;
    CYCLES(2);
}
void lodsb16(struct CPU* cpu, U32 base) {
    AL = readb(MMU_PARAM_CPU cpu->segAddress[base]+SI);
    SI+=cpu->df;
    CYCLES(2);
}
void lodsw32_r(struct CPU* cpu, U32 base) {
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 1;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        AX = readw(MMU_PARAM_CPU sBase+ESI);
        ESI+=inc;
    }
    ECX=0;
    CYCLES(2);
}
void lodsw16_r(struct CPU* cpu, U32 base) {
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 1;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        AX = readw(MMU_PARAM_CPU sBase+SI);
        SI+=inc;
    }
    CX=0;
    CYCLES(2);
}
void lodsw32(struct CPU* cpu, U32 base) {
    AX = readw(MMU_PARAM_CPU cpu->segAddress[base]+ESI);
    ESI+=cpu->df << 1;
    CYCLES(2);
}
void lodsw16(struct CPU* cpu, U32 base) {
    AX = readw(MMU_PARAM_CPU cpu->segAddress[base]+SI);
    SI+=cpu->df << 1;
    CYCLES(2);
}
void lodsd32_r(struct CPU* cpu, U32 base) {
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 2;
    U32 count = ECX;
    U32 i;
    for (i=0;i<count;i++) {
        EAX = readd(MMU_PARAM_CPU sBase+ESI);
        ESI+=inc;
    }
    ECX=0;
    CYCLES(2);
}
void lodsd16_r(struct CPU* cpu, U32 base) {
    U32 sBase = cpu->segAddress[base];
    S32 inc = cpu->df << 2;
    U32 count = CX;
    U32 i;
    for (i=0;i<count;i++) {
        EAX = readd(MMU_PARAM_CPU sBase+SI);
        SI+=inc;
    }
    CX=0;
    CYCLES(2);
}
void lodsd32(struct CPU* cpu, U32 base) {
    EAX = readd(MMU_PARAM_CPU cpu->segAddress[base]+ESI);
    ESI+=cpu->df << 2;
    CYCLES(2);
}
void lodsd16(struct CPU* cpu, U32 base) {
    EAX = readd(MMU_PARAM_CPU cpu->segAddress[base]+SI);
    SI+=cpu->df << 2;
    CYCLES(2);
}
void scasb32_r(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    U8 v1;
    U32 count = ECX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readb(MMU_PARAM_CPU dBase+EDI);
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
}
void scasb16_r(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    U8 v1;
    U32 count = CX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readb(MMU_PARAM_CPU dBase+DI);
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
}
void scasb32(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    U8 v1;
            v1 = readb(MMU_PARAM_CPU dBase+EDI);
            EDI+=inc;
        cpu->dst.u8 = AL;
        cpu->src.u8 = v1;
        cpu->result.u8 = AL - v1;
        cpu->lazyFlags = FLAGS_SUB8;
    CYCLES(4);
}
void scasb16(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df;
    U8 v1;
            v1 = readb(MMU_PARAM_CPU dBase+DI);
            DI+=inc;
        cpu->dst.u8 = AL;
        cpu->src.u8 = v1;
        cpu->result.u8 = AL - v1;
        cpu->lazyFlags = FLAGS_SUB8;
    CYCLES(4);
}
void scasw32_r(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    U16 v1;
    U32 count = ECX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readw(MMU_PARAM_CPU dBase+EDI);
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
}
void scasw16_r(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    U16 v1;
    U32 count = CX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readw(MMU_PARAM_CPU dBase+DI);
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
}
void scasw32(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    U16 v1;
            v1 = readw(MMU_PARAM_CPU dBase+EDI);
            EDI+=inc;
        cpu->dst.u16 = AX;
        cpu->src.u16 = v1;
        cpu->result.u16 = AX - v1;
        cpu->lazyFlags = FLAGS_SUB16;
    CYCLES(4);
}
void scasw16(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 1;
    U16 v1;
            v1 = readw(MMU_PARAM_CPU dBase+DI);
            DI+=inc;
        cpu->dst.u16 = AX;
        cpu->src.u16 = v1;
        cpu->result.u16 = AX - v1;
        cpu->lazyFlags = FLAGS_SUB16;
    CYCLES(4);
}
void scasd32_r(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    U32 v1;
    U32 count = ECX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readd(MMU_PARAM_CPU dBase+EDI);
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
}
void scasd16_r(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    U32 v1;
    U32 count = CX;
    U32 i;
    if (count) {
        for (i=0;i<count;i++) {
            v1 = readd(MMU_PARAM_CPU dBase+DI);
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
}
void scasd32(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    U32 v1;
            v1 = readd(MMU_PARAM_CPU dBase+EDI);
            EDI+=inc;
        cpu->dst.u32 = EAX;
        cpu->src.u32 = v1;
        cpu->result.u32 = EAX - v1;
        cpu->lazyFlags = FLAGS_SUB32;
    CYCLES(4);
}
void scasd16(struct CPU* cpu, U32 rep_zero) {
    U32 dBase = cpu->segAddress[ES];
    S32 inc = cpu->df << 2;
    U32 v1;
            v1 = readd(MMU_PARAM_CPU dBase+DI);
            DI+=inc;
        cpu->dst.u32 = EAX;
        cpu->src.u32 = v1;
        cpu->result.u32 = EAX - v1;
        cpu->lazyFlags = FLAGS_SUB32;
    CYCLES(4);
}

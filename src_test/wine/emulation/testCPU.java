package wine.emulation;

import junit.framework.TestCase;

public class testCPU extends TestCase {
    static protected CPU cpu;
    static protected Memory memory;
    protected int cseip;

    protected static final int STACK_ADDRESS=0xE0001000;
    protected static final int HEAP_ADDRESS =0xF0000000;
    protected static final int CODE_ADDRESS =0xD0000000;

    protected static final int DEFAULT =0xDEADBEEF;
    
    protected void setUp() throws java.lang.Exception {
        if (memory==null) {
            memory = new Memory();
            memory.init(null);
            this.cpu = new CPU(memory);
            RAM.init(4096 * 5);
            memory.handlers[(STACK_ADDRESS>>>12)-1] = RAMHandler.create(RAM.allocPage(), PageHandler.READ|PageHandler.WRITE);
            memory.handlers[(HEAP_ADDRESS-1)>>>12] = RAMHandler.create(RAM.allocPage(), PageHandler.READ|PageHandler.WRITE);
            memory.handlers[HEAP_ADDRESS>>>12] = RAMHandler.create(RAM.allocPage(), PageHandler.READ|PageHandler.WRITE);
            memory.handlers[CODE_ADDRESS>>>12] = RAMHandler.create(RAM.allocPage(), PageHandler.READ|PageHandler.WRITE);
            cpu.cs.dword = CODE_ADDRESS;
            cpu.ds.dword = HEAP_ADDRESS;
            cpu.ss.dword = STACK_ADDRESS-4096;
        }
        super.setUp();
    }

    protected void tearDown() throws java.lang.Exception {
        super.tearDown();
    }

    protected void pushCode8(int value) {
        memory.writeb(cseip, value);
        cseip++;
    }

    protected void pushCode16(int value) {
        memory.writew(cseip, value);
        cseip+=2;
    }

    protected void pushCode32(int value) {
        memory.writed(cseip, value);
        cseip+=4;
    }

    protected void newInstruction(int instruction, int rm, int flags) {
        newInstruction(instruction, flags);
        pushCode8(rm);
    }
    protected void newInstruction(int instruction, int flags) {
        cseip=CODE_ADDRESS;
        cpu.lazyFlags = null;
        cpu.flags = flags;
        cpu.blocks.clear();
        cpu.eax.dword=0;
        cpu.ecx.dword=0;
        cpu.edx.dword=0;
        cpu.ebp.dword=0;
        cpu.esp.dword=4096;
        cpu.ebp.dword=0;
        cpu.esi.dword=0;
        cpu.edi.dword=0;
        cpu.eip=0;
        memory.zero(CODE_ADDRESS, 4096);
        memory.zero(STACK_ADDRESS-4096, 4096);
        memory.zero(HEAP_ADDRESS, 4096);
        pushCode8(instruction);
    }

    static protected class NullOp extends Op {
        public Block call(CPU cpu) {
            return null;
        }
    }
    static protected NullOp nullOp = new NullOp();

    protected void runCPU() {
        pushCode8(0x70); // jump causes the decoder to stop building the block
        pushCode8(0);
        Block block = cpu.getBlock(cpu.eip, cpu.cs.dword);
        if (block.op.next==null)
            return; // no ops generated
        block.op.next = nullOp;
        block.op.call(cpu);
    }

    protected void Eb(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int eb = 0; eb < 8; eb++) {
                int rm = eb | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                Reg reg = cpu.regs[Decoder.eb(rm)];
                if (reg.parent != null)
                    reg.parent.dword = DEFAULT;
                else
                    reg.dword = DEFAULT;
                reg.u8(d.left);
                runCPU();
                d.assertResult(cpu, instruction, reg.u8(), 0, reg.name8, null, reg, null, 0, 8);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writeb(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readb(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 8);
        }
    }

    protected void EbAlAx(int instruction, int which, Data[] data, boolean AX) {
        for (Data d : data) {
            for (int eb = 0; eb < 8; eb++) {
                if (eb==0 || eb==4)
                    continue;
                int rm = eb | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                Reg reg = cpu.regs[Decoder.eb(rm)];
                if (reg.parent != null)
                    reg.parent.dword = DEFAULT;
                else
                    reg.dword = DEFAULT;
                cpu.eax.dword = DEFAULT;
                if (AX)
                    cpu.eax.u16(d.left);
                else
                    cpu.eax.u8(d.left);
                reg.u8(d.right);
                runCPU();
                d.assertResult(cpu, instruction, cpu.eax.u16(), 0, reg.name16, null, cpu.eax, null, 0, 16);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            cpu.eax.dword = DEFAULT;
            if (AX)
                cpu.eax.u16(d.left);
            else
                cpu.eax.u8(d.left);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writeb(cpu.ds.dword + 200, d.right);
            runCPU();
            d.assertResult(cpu, instruction, cpu.eax.u16(), 0, cpu.eax.name16, null, cpu.eax, null, 0, 16);
        }
    }

    protected void EwAxDx(int instruction, int which, Data[] data, boolean useDX) {
        for (Data d : data) {
            for (int ew = 0; ew < 8; ew++) {
                if (ew==0)
                    continue;
                if (useDX & ew==2)
                    continue;
                int rm = ew | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                Reg reg = cpu.regs[Decoder.ew(rm)];
                cpu.eax.dword = DEFAULT;
                cpu.edx.dword = DEFAULT;
                reg.dword = DEFAULT;
                cpu.eax.u16(d.left);
                cpu.edx.u16(d.right);
                reg.u16(d.constant);
                runCPU();
                d.assertResult(cpu, instruction, cpu.eax.u16(), cpu.edx.u16(), cpu.eax.name16, cpu.edx.name16, cpu.eax, cpu.edx, 0, 16);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            cpu.eax.dword = DEFAULT;
            cpu.edx.dword = DEFAULT;
            cpu.eax.u16(d.left);
            cpu.edx.u16(d.right);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writew(cpu.ds.dword + 200, d.constant);
            runCPU();
            d.assertResult(cpu, instruction, cpu.eax.u16(), cpu.edx.u16(), cpu.eax.name16, cpu.edx.name16, cpu.eax, cpu.edx, 0, 16);
        }
    }

    protected void EdEaxEdx(int instruction, int which, Data[] data, boolean useEdx) {
        for (Data d : data) {
            for (int ed = 0; ed < 8; ed++) {
                if (ed==0)
                    continue;
                if (useEdx & ed==2)
                    continue;
                int rm = ed | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                Reg reg = cpu.regs[Decoder.ew(rm)];
                reg.dword = DEFAULT;
                cpu.eax.dword = d.left;
                cpu.edx.dword = d.right;
                reg.dword = d.constant;
                runCPU();
                d.assertResult(cpu, instruction, cpu.eax.dword, cpu.edx.dword, cpu.eax.name32, cpu.edx.name32, cpu.eax, cpu.edx, 0, 32);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            cpu.eax.dword=d.left;
            cpu.edx.dword=d.right;
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writed(cpu.ds.dword + 200, d.constant);
            runCPU();
            d.assertResult(cpu, instruction, cpu.eax.dword, cpu.edx.dword, cpu.eax.name32, cpu.edx.name32, cpu.eax, cpu.edx, 0, 32);
        }
    }

    protected void EbCl(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int eb = 0; eb < 8; eb++) {
                if (eb==1)
                    continue;
                int rm = eb | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                Reg reg = cpu.regs[Decoder.eb(rm)];
                if (reg.parent != null)
                    reg.parent.dword = DEFAULT;
                else
                    reg.dword = DEFAULT;
                cpu.ecx.dword = DEFAULT;
                reg.u8(d.left);
                cpu.ecx.u8(d.right);
                runCPU();
                d.assertResult(cpu, instruction, reg.u8(), 0, reg.name8, null, reg, null, 0, 8);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            cpu.ecx.dword = DEFAULT;
            cpu.ecx.u8(d.right);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writeb(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readb(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 8);
        }
    }

    protected void EbIb(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int eb = 0; eb < 8; eb++) {
                int rm = eb | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                pushCode8(d.right);
                Reg e = cpu.regs[Decoder.eb(rm)];
                if (e.parent != null)
                    e.parent.dword = DEFAULT;
                else
                    e.dword = DEFAULT;
                e.u8(d.left);
                runCPU();
                d.assertResult(cpu, instruction, e.u8(), 0, e.name8, null, e, null, 0, 8);

            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            pushCode8(d.right);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writeb(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readb(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 8);
        }
    }

    protected void EbIb(int instruction, Reg e, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, 0);
            pushCode8(d.right);
            if (e.parent != null)
                e.parent.dword = DEFAULT;
            else
                e.dword = DEFAULT;
            e.u8(d.left);
            runCPU();
            d.assertResult(cpu, instruction, e.u8(), 0, e.name8, null, e, null, 0, 8);
        }
    }

    protected void EbGb(int instruction, Data[] data) {
        for (Data d : data) {
            for (int eb = 0; eb < 8; eb++) {
                for (int gb = 0; gb < 8; gb++) {
                    if (eb == gb)
                        continue;
                    int rm = eb | (gb << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = cpu.regs[Decoder.eb(rm)];
                    Reg g = cpu.regs[Decoder.gb(rm)];
                    if (e.parent != null)
                        e.parent.dword = DEFAULT;
                    else
                        e.dword = DEFAULT;
                    if (g.parent != null)
                        g.parent.dword = DEFAULT;
                    else
                        g.dword = DEFAULT;
                    e.u8(d.left);
                    g.u8(d.right);
                    runCPU();
                    d.assertResult(cpu, instruction, e.u8(), g.u8(), e.name8, g.name8, e, g, 0, 8);
                }
            }

            for (int gb = 0; gb < 8; gb++) {
                int rm = (gb << 3);
                if (cpu.big)
                    rm += 5;
                else
                    rm += 6;
                newInstruction(instruction, rm, d.flags);
                if (cpu.big)
                    pushCode32(200);
                else
                    pushCode16(200);
                d.pushConstant();
                memory.writed(cpu.ds.dword + 200, DEFAULT);
                memory.writeb(cpu.ds.dword + 200, d.left);
                Reg g = cpu.regs[Decoder.gb(rm)];
                if (g.parent != null)
                    g.parent.dword = DEFAULT;
                else
                    g.dword = DEFAULT;
                g.u8(d.right);
                runCPU();
                int result = memory.readb(cpu.ds.dword + 200);
                d.assertResult(cpu, instruction, result, g.u8(), null, g.name8, g, null, cpu.ds.dword + 200, 8);
            }
        }
    }

    protected void GbEb(int instruction, Data[] data) {
        for (Data d : data) {
            for (int eb = 0; eb < 8; eb++) {
                for (int gb = 0; gb < 8; gb++) {
                    if (eb == gb)
                        continue;
                    int rm = eb | (gb << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = cpu.regs[Decoder.eb(rm)];
                    Reg g = cpu.regs[Decoder.gb(rm)];
                    if (e.parent != null)
                        e.parent.dword = DEFAULT;
                    else
                        e.dword = DEFAULT;
                    if (g.parent != null)
                        g.parent.dword = DEFAULT;
                    else
                        g.dword = DEFAULT;
                    e.u8(d.right);
                    g.u8(d.left);
                    runCPU();
                    d.assertResult(cpu, instruction, g.u8(), e.u8(), g.name8, e.name8, e, g, 0, 8);
                }
            }

            for (int gb = 0; gb < 8; gb++) {
                int rm = (gb << 3);
                if (cpu.big)
                    rm += 5;
                else
                    rm += 6;
                newInstruction(instruction, rm, d.flags);
                if (cpu.big)
                    pushCode32(200);
                else
                    pushCode16(200);
                d.pushConstant();
                memory.writed(cpu.ds.dword + 200, DEFAULT);
                memory.writeb(cpu.ds.dword + 200, d.right);
                Reg g = cpu.regs[Decoder.gb(rm)];
                if (g.parent != null)
                    g.parent.dword = DEFAULT;
                else
                    g.dword = DEFAULT;
                g.u8(d.left);
                runCPU();
                d.assertResult(cpu, instruction, g.u8(), memory.readb(cpu.ds.dword + 200), g.name8, null, g, null, cpu.ds.dword + 200, 8);
            }
        }
    }

    protected void Ew(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int eb = 0; eb < 8; eb++) {
                int rm = eb | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                Reg reg = cpu.regs[Decoder.ew(rm)];
                reg.dword = DEFAULT;
                reg.u16(d.left);
                runCPU();
                d.assertResult(cpu, instruction, reg.u16(), 0, reg.name16, null, reg, null, 0, 16);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writew(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readw(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 16);
        }
    }

    protected void EwCl(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int ew = 0; ew < 8; ew++) {
                if (ew==1)
                    continue;
                int rm = ew | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                Reg reg = cpu.regs[Decoder.ew(rm)];
                reg.dword = DEFAULT;
                cpu.ecx.dword = DEFAULT;
                reg.u16(d.left);
                cpu.ecx.u8(d.right);
                runCPU();
                d.assertResult(cpu, instruction, reg.u16(), 0, reg.name16, null, reg, null, 0, 16);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            cpu.ecx.dword = DEFAULT;
            cpu.ecx.u8(d.right);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writew(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readw(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 16);
        }
    }

    protected void EwIx(int instruction, int which, Data[] data) {
        for (Data d : data) {
            if ((byte)(d.right & 0xFF) != (short)d.right) {
                continue;
            }
            for (int ew = 0; ew < 8; ew++) {
                int rm = ew | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                pushCode8(d.right);
                Reg e = cpu.regs[Decoder.ew(rm)];
                e.dword = DEFAULT;
                e.u16(d.left);
                runCPU();
                d.assertResult(cpu, instruction, e.u16(), 0, e.name16, null, e, null, 0, 16);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            pushCode8(d.right);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writew(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readw(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 16);
        }
    }

    protected void EwIw(int instruction, Reg e, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, 0);
            pushCode16(d.right);
            e.dword = DEFAULT;
            e.u16(d.left);
            runCPU();
            d.assertResult(cpu, instruction, e.u16(), 0, e.name16, null, e, null, 0, 16);
        }
    }

    protected void EwIb(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int ew = 0; ew < 8; ew++) {
                int rm = ew | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                pushCode8(d.right);
                Reg e = cpu.regs[Decoder.ew(rm)];
                e.dword = DEFAULT;
                e.u16(d.left);
                runCPU();
                d.assertResult(cpu, instruction, e.u16(), 0, e.name16, null, e, null, 0, 16);

            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            pushCode8(d.right);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writew(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readw(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 16);
        }
    }

    protected void EwIw(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int ew = 0; ew < 8; ew++) {
                int rm = ew | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                pushCode16(d.right);
                Reg e = cpu.regs[Decoder.ew(rm)];
                e.dword = DEFAULT;
                e.u16(d.left);
                runCPU();
                d.assertResult(cpu, instruction, e.u16(), 0, e.name16, null, e, null, 0, 16);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            pushCode16(d.right);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writew(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readw(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 16);
        }
    }

    protected void EwGw(int instruction, Data[] data) {
        for (Data d : data) {
            for (int ew = 0; ew < 8; ew++) {
                for (int gw = 0; gw < 8; gw++) {
                    if (ew == gw)
                        continue;
                    int rm = ew | (gw << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = cpu.regs[Decoder.ew(rm)];
                    Reg g = cpu.regs[Decoder.gw(rm)];
                    e.dword = DEFAULT;
                    g.dword = DEFAULT;
                    e.u16(d.left);
                    g.u16(d.right);
                    runCPU();
                    d.assertResult(cpu, instruction, e.u16(), g.u16(), e.name16, g.name16, e, g, 0, 16);
                }
            }

            for (int gw = 0; gw < 8; gw++) {
                int rm = (gw << 3);
                if (cpu.big)
                    rm += 5;
                else
                    rm += 6;
                newInstruction(instruction, rm, d.flags);
                if (cpu.big)
                    pushCode32(200);
                else
                    pushCode16(200);
                d.pushConstant();
                memory.writed(cpu.ds.dword + 200, DEFAULT);
                memory.writew(cpu.ds.dword + 200, d.left);
                Reg g = cpu.regs[Decoder.gw(rm)];
                g.dword = DEFAULT;
                g.u16(d.right);
                runCPU();
                int result = memory.readw(cpu.ds.dword + 200);
                d.assertResult(cpu, instruction, result, g.u16(), null, g.name16, g, null, cpu.ds.dword + 200, 16);
            }
        }
    }

    protected void EwSw(int instruction, Data[] data) {
        for (Data d : data) {
            for (int ew = 0; ew < 8; ew++) {
                for (int gw = 0; gw < 6; gw++) {
                    int rm = ew | (gw << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = cpu.regs[Decoder.ew(rm)];
                    Reg g = cpu.regs[cpu.segValue[gw]];
                    e.dword = DEFAULT;
                    g.dword = DEFAULT;
                    e.u16(d.left);
                    g.u16(d.right);
                    runCPU();
                    d.assertResult(cpu, instruction, e.u16(), g.u16(), e.name16, g.name16, e, null, 0, 16);
                }
            }

            for (int gw = 0; gw < 6; gw++) {
                int rm = (gw << 3);
                if (cpu.big)
                    rm += 5;
                else
                    rm += 6;
                newInstruction(instruction, rm, d.flags);
                if (cpu.big)
                    pushCode32(200);
                else
                    pushCode16(200);
                d.pushConstant();
                memory.writed(cpu.ds.dword + 200, DEFAULT);
                memory.writew(cpu.ds.dword + 200, d.left);
                Reg g = cpu.regs[cpu.segValue[gw]];
                g.dword = DEFAULT;
                g.u16(d.right);
                runCPU();
                int result = memory.readw(cpu.ds.dword + 200);
                d.assertResult(cpu, instruction, result, g.u16(), null, g.name16, null, null, cpu.ds.dword + 200, 16);
            }
        }
    }

    protected void GwEw(int instruction, Data[] data) {
        for (Data d : data) {
            for (int ew = 0; ew < 8; ew++) {
                for (int gw = 0; gw < 8; gw++) {
                    if (ew == gw)
                        continue;
                    int rm = ew | (gw << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = cpu.regs[Decoder.ew(rm)];
                    Reg g = cpu.regs[Decoder.gw(rm)];
                    e.dword = DEFAULT;
                    g.dword = DEFAULT;
                    e.u16(d.right);
                    g.u16(d.left);
                    runCPU();
                    d.assertResult(cpu, instruction, g.u16(), e.u16(), g.name16, e.name16, e, g, 0, 16);
                }
            }

            for (int gw = 0; gw < 8; gw++) {
                int rm = (gw << 3);
                if (cpu.big)
                    rm += 5;
                else
                    rm += 6;
                newInstruction(instruction, rm, d.flags);
                if (cpu.big)
                    pushCode32(200);
                else
                    pushCode16(200);
                d.pushConstant();
                memory.writed(cpu.ds.dword + 200, DEFAULT);
                memory.writew(cpu.ds.dword + 200, d.right);
                Reg g = cpu.regs[Decoder.gw(rm)];
                g.dword = DEFAULT;
                g.u16(d.left);
                runCPU();
                int result = memory.readw(cpu.ds.dword + 200);
                d.assertResult(cpu, instruction, g.u16(), result, g.name16, null, g, null, cpu.ds.dword + 200, 16);
            }
        }
    }

    protected void Ed(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int ed = 0; ed < 8; ed++) {
                int rm = ed | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                Reg reg = cpu.regs[Decoder.ed(rm)];
                reg.dword = d.left;
                runCPU();
                d.assertResult(cpu, instruction, reg.dword, 0, reg.name32, null, reg, null, 0, 32);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readd(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 32);
        }
    }

    protected void EdCl(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int ed = 0; ed < 8; ed++) {
                if (ed==1)
                    continue;
                int rm = ed | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                Reg reg = cpu.regs[Decoder.ed(rm)];
                cpu.ecx.dword = DEFAULT;
                reg.dword = d.left;
                cpu.ecx.u8(d.right);
                runCPU();
                d.assertResult(cpu, instruction, reg.dword, 0, reg.name32, null, reg, null, 0, 32);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            cpu.ecx.dword = DEFAULT;
            cpu.ecx.u8(d.right);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readd(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 32);
        }
    }

    protected void EdIx(int instruction, int which, Data[] data) {
        for (Data d : data) {
            if ((byte)(d.right & 0xFF) != d.right) {
                continue;
            }
            for (int ed = 0; ed < 8; ed++) {
                int rm = ed | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                pushCode8(d.right);
                Reg e = cpu.regs[Decoder.ed(rm)];
                e.dword = d.left;
                runCPU();
                d.assertResult(cpu, instruction, e.dword, 0, e.name32, null, null, null, 0, 0);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            pushCode8(d.right);
            memory.writed(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readd(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, 0, 0);
        }
    }

    protected void EdIb(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int ew = 0; ew < 8; ew++) {
                int rm = ew | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                pushCode8(d.right);
                Reg e = cpu.regs[Decoder.ed(rm)];
                e.dword = d.left;
                runCPU();
                d.assertResult(cpu, instruction, e.dword, 0, e.name32, null, e, null, 0, 32);

            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            pushCode8(d.right);
            memory.writed(cpu.ds.dword + 200, DEFAULT);
            memory.writed(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readd(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, cpu.ds.dword + 200, 32);
        }
    }

    protected void EdId(int instruction, Reg e, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, 0);
            pushCode32(d.right);
            e.dword=d.left;
            runCPU();
            d.assertResult(cpu, instruction, e.dword, 0, e.name32, null, e, null, 0, 32);
        }
    }

    protected void EdId(int instruction, int which, Data[] data) {
        for (Data d : data) {
            for (int ed = 0; ed < 8; ed++) {
                int rm = ed | (which << 3) | 0xC0;
                newInstruction(instruction, rm, d.flags);
                pushCode32(d.right);
                Reg e = cpu.regs[Decoder.ed(rm)];
                e.dword = d.left;
                runCPU();
                d.assertResult(cpu, instruction, e.dword, 0, e.name32, null, null, null, 0, 0);
            }

            int rm = (which << 3);
            if (cpu.big)
                rm += 5;
            else
                rm += 6;
            newInstruction(instruction, rm, d.flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            pushCode32(d.right);
            memory.writed(cpu.ds.dword + 200, d.left);
            runCPU();
            int result = memory.readd(cpu.ds.dword + 200);
            d.assertResult(cpu, instruction, result, 0, null, null, null, null, 0, 0);
        }
    }

    protected void EdGd(int instruction, Data[] data) {
        for (Data d : data) {
            for (int ed = 0; ed < 8; ed++) {
                for (int gd = 0; gd < 8; gd++) {
                    if (ed == gd)
                        continue;
                    int rm = ed | (gd << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = cpu.regs[Decoder.ed(rm)];
                    Reg g = cpu.regs[Decoder.gd(rm)];
                    e.dword = d.left;
                    g.dword = d.right;
                    runCPU();
                    d.assertResult(cpu, instruction, e.dword, g.dword, e.name32, g.name32, null, null, 0, 0);
                }
            }

            for (int gd = 0; gd < 8; gd++) {
                int rm = (gd << 3);
                if (cpu.big)
                    rm += 5;
                else
                    rm += 6;
                newInstruction(instruction, rm, d.flags);
                if (cpu.big)
                    pushCode32(200);
                else
                    pushCode16(200);
                d.pushConstant();
                memory.writed(cpu.ds.dword + 200, d.left);
                Reg g = cpu.regs[Decoder.gw(rm)];
                g.dword = d.right;
                runCPU();
                int result = memory.readd(cpu.ds.dword + 200);
                d.assertResult(cpu, instruction, result, g.dword, null, g.name16, null, null, 0, 0);
            }
        }
    }

    protected void GdEd(int instruction, Data[] data) {
        for (Data d : data) {
            for (int ed = 0; ed < 8; ed++) {
                for (int gd = 0; gd < 8; gd++) {
                    if (ed == gd)
                        continue;
                    int rm = ed | (gd << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = cpu.regs[Decoder.ed(rm)];
                    Reg g = cpu.regs[Decoder.gd(rm)];
                    e.dword = d.right;
                    g.dword = d.left;
                    runCPU();
                    d.assertResult(cpu, instruction, g.dword, e.dword, g.name32, e.name32, null, null, 0, 0);
                }
            }

            for (int gd = 0; gd < 8; gd++) {
                int rm = (gd << 3);
                if (cpu.big)
                    rm += 5;
                else
                    rm += 6;
                newInstruction(instruction, rm, d.flags);
                if (cpu.big)
                    pushCode32(200);
                else
                    pushCode16(200);
                d.pushConstant();
                memory.writed(cpu.ds.dword + 200, d.right);
                Reg g = cpu.regs[Decoder.gd(rm)];
                g.dword = d.left;
                runCPU();
                int result = memory.readd(cpu.ds.dword + 200);
                d.assertResult(cpu, instruction, g.dword, result, g.name32, null, null, null, 0, 0);
            }
        }
    }

    protected void AlIb(int instruction, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            pushCode8(d.right);
            cpu.eax.dword = DEFAULT;
            cpu.eax.u8(d.left);
            runCPU();
            d.assertResult(cpu, instruction, cpu.eax.u8(), 0, cpu.eax.name8, null, cpu.eax, null, 0, 8);
        }
    }

    protected void AxIw(int instruction, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            pushCode16(d.right);
            cpu.eax.dword = DEFAULT;
            cpu.eax.u16(d.left);
            runCPU();
            d.assertResult(cpu, instruction, cpu.eax.u16(), 0, cpu.eax.name16, null, cpu.eax, null, 0, 16);
        }
    }

    protected void EaxId(int instruction, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            pushCode32(d.right);
            cpu.eax.dword = d.left;
            runCPU();
            d.assertResult(cpu, instruction, cpu.eax.dword, 0, cpu.eax.name32, null, null, null, 0, 0);
        }
    }

    protected void Ew(int instruction, Reg reg, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            reg.dword = DEFAULT;
            reg.u16(d.left);
            runCPU();
            d.assertResult(cpu, instruction, reg.u16(), 0, reg.name16, null, reg, null, 0, 16);
        }
    }

    protected void Eb(int instruction, Reg reg, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            reg.dword = DEFAULT;
            reg.u8(d.left);
            runCPU();
            d.assertResult(cpu, instruction, reg.u8(), 0, reg.name8, null, reg, null, 0, 8);
        }
    }

    protected void LeaGw() {
        for (int i=0;i<8;i++) {
            if (i==6) {
                continue;
            }
            newInstruction(0x8d, 0);
            int rm = i<<3 | 0x44;
            pushCode8(rm);
            pushCode8(8);
            Reg reg = cpu.regs[Decoder.gw(rm)];
            reg.dword = DEFAULT;
            cpu.esi.dword = DEFAULT;
            cpu.esi.u16(0xABCD);
            runCPU();
            assertTrue(reg.u16()==(0xABCD+8));
            assertTrue((reg.dword & 0xFFFF0000)==(DEFAULT & 0xFFFF0000));
        }
    }

    protected void LeaGd() {
        for (int i=0;i<8;i++) {
            if (i==3) {
                continue;
            }
            newInstruction(0x8d, 0);
            int rm = i<<3 | 0x43;
            pushCode8(rm);
            pushCode8(8);
            Reg reg = cpu.regs[Decoder.gd(rm)];
            cpu.ebx.dword=0xABCD1234;
            runCPU();
            assertTrue(reg.dword==(0xABCD1234+8));
        }
    }

    protected void Ed(int instruction, Reg reg, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            reg.dword = d.left;
            runCPU();
            d.assertResult(cpu, instruction, reg.dword, 0, reg.name32, null, null, null, 0, 0);
        }
    }

    protected void Reg16Reg16(int instruction, Data[] data, Reg r1, Reg r2) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            r1.dword = DEFAULT;
            r1.u16(d.left);
            r2.dword = DEFAULT;
            r2.u16(d.right);
            runCPU();
            d.assertResult(cpu, instruction, r1.u16(), r2.u16(), r1.name16, r2.name16, r1, r2, 0, 16);
        }
    }

    protected void Reg32Reg32(int instruction, Data[] data, Reg r1, Reg r2) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            r1.dword = d.left;
            r2.dword = d.right;
            runCPU();
            d.assertResult(cpu, instruction, r1.dword, r2.dword, r1.name32, r2.name32, null, null, 0, 0);
        }
    }

    protected void Push16(int instruction, Reg reg) {
        newInstruction(instruction, 0);
        reg.dword = 0xDDDD1234;
        cpu.esp.dword-=2;
        memory.writew(cpu.ss.dword+cpu.esp.dword, 0xAAAA);
        memory.writew(cpu.ss.dword+cpu.esp.dword-2, 0xCCCC);
        memory.writew(cpu.ss.dword+cpu.esp.dword-4, 0xBBBB);
        runCPU();
        assertTrue(cpu.esp.dword==4092);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword)==0x1234);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword+2)==0xAAAA);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword-2)==0xBBBB);
    }

    protected void Pushf(int instruction) {
        newInstruction(instruction, 0);
        cpu.flags = 0xDDDD1234;
        cpu.esp.dword-=2;
        memory.writew(cpu.ss.dword+cpu.esp.dword, 0xAAAA);
        memory.writew(cpu.ss.dword+cpu.esp.dword-2, 0xCCCC);
        memory.writew(cpu.ss.dword+cpu.esp.dword-4, 0xBBBB);
        runCPU();
        assertTrue(cpu.esp.dword==4092);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword)==0x1235); // bit 1 is always set
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword+2)==0xAAAA);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword-2)==0xBBBB);
    }

    protected void Push32(int instruction, Reg reg) {
        newInstruction(instruction, 0);
        reg.dword = 0x56781234;
        cpu.esp.dword-=4;
        memory.writed(cpu.ss.dword+cpu.esp.dword, 0xAAAAAAAA);
        memory.writed(cpu.ss.dword+cpu.esp.dword-4, 0xCCCCCCCC);
        memory.writed(cpu.ss.dword+cpu.esp.dword-8, 0xBBBBBBBB);
        runCPU();
        assertTrue(cpu.esp.dword==4088);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword)==0x56781234);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword+4)==0xAAAAAAAA);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword-4)==0xBBBBBBBB);
    }

    protected void Pushfd(int instruction) {
        newInstruction(instruction, 0);
        cpu.flags = 0x56781234;
        cpu.esp.dword-=4;
        memory.writed(cpu.ss.dword+cpu.esp.dword, 0xAAAAAAAA);
        memory.writed(cpu.ss.dword+cpu.esp.dword-4, 0xCCCCCCCC);
        memory.writed(cpu.ss.dword+cpu.esp.dword-8, 0xBBBBBBBB);
        runCPU();
        assertTrue(cpu.esp.dword==4088);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword)==(0x56781235 & 0xFCFFFF)); // bit 1 is always set
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword+4)==0xAAAAAAAA);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword-4)==0xBBBBBBBB);
    }

    protected void Pop16(int instruction, Reg reg) {
        newInstruction(instruction, 0);
        cpu.esp.dword-=2;
        reg.dword=0xDDDDDDDD;
        memory.writew(cpu.ss.dword+cpu.esp.dword, 0xAAAA);
        memory.writew(cpu.ss.dword+cpu.esp.dword-2, 0x1234);
        memory.writew(cpu.ss.dword+cpu.esp.dword-4, 0xBBBB);
        cpu.esp.dword-=2;
        runCPU();
        assertTrue(cpu.esp.dword==4094);
        assertTrue(reg.dword == 0xDDDD1234);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword)==0xAAAA);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword-4)==0xBBBB);
    }

    protected void Popf(int instruction) {
        newInstruction(instruction, 0);
        cpu.esp.dword-=2;
        cpu.flags=0xDDDDDDDD;
        memory.writew(cpu.ss.dword+cpu.esp.dword, 0xAAAA);
        memory.writew(cpu.ss.dword+cpu.esp.dword-2, 0x1234);
        memory.writew(cpu.ss.dword+cpu.esp.dword-4, 0xBBBB);
        cpu.esp.dword-=2;
        runCPU();
        assertTrue(cpu.esp.dword==4094);
        assertTrue(cpu.flags == 0xDDDD1234);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword)==0xAAAA);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword-4)==0xBBBB);
    }

    protected void Pop32(int instruction, Reg reg) {
        newInstruction(instruction, 0);
        cpu.esp.dword-=4;
        memory.writed(cpu.ss.dword+cpu.esp.dword, 0xAAAAAAAA);
        memory.writed(cpu.ss.dword+cpu.esp.dword-4, 0x56781234);
        memory.writed(cpu.ss.dword+cpu.esp.dword-8, 0xBBBBBBBB);
        cpu.esp.dword-=4;
        runCPU();
        assertTrue(cpu.esp.dword==4092);
        assertTrue(reg.dword == 0x56781234);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword)==0xAAAAAAAA);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword-8)==0xBBBBBBBB);
    }

    protected void Popfd(int instruction) {
        newInstruction(instruction, 0);
        cpu.esp.dword-=4;
        memory.writed(cpu.ss.dword+cpu.esp.dword, 0xAAAAAAAA);
        memory.writed(cpu.ss.dword+cpu.esp.dword-4, 0x56781234);
        memory.writed(cpu.ss.dword+cpu.esp.dword-8, 0xBBBBBBBB);
        cpu.esp.dword-=4;
        runCPU();
        assertTrue(cpu.esp.dword==4092);
        assertTrue(cpu.flags == 0x56781234);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword)==0xAAAAAAAA);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword-8)==0xBBBBBBBB);
    }

    protected void flags(int instruction, Data[] data, Reg reg) {
        for (Data d:data) {
            newInstruction(instruction, 0);
            cpu.flags = d.left;
            reg.dword = d.right;
            runCPU();
            assertTrue(reg.dword == d.resultRight);
            assertTrue(cpu.flags == d.result);
        }
    }

    protected void PopEw() {
        for (int i=0;i<8;i++) {
            if (i==4)
                continue;
            newInstruction(0x8f, 0);
            pushCode8(i|0xC0);
            Reg reg = cpu.regs[Decoder.ew(i|0xC0)];
            cpu.esp.dword-=2;
            reg.dword=0xDDDDDDDD;
            memory.writew(cpu.ss.dword+cpu.esp.dword, 0xAAAA);
            memory.writew(cpu.ss.dword+cpu.esp.dword-2, 0x1234);
            memory.writew(cpu.ss.dword+cpu.esp.dword-4, 0xBBBB);
            cpu.esp.dword-=2;
            runCPU();
            assertTrue(cpu.esp.dword==4094);
            assertTrue(reg.dword == 0xDDDD1234);
            assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword)==0xAAAA);
            assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword-4)==0xBBBB);
        }

        newInstruction(0x8f, 0);
        pushCode8(6);
        pushCode16(200);
        memory.writed(cpu.ds.dword + 200, DEFAULT);

        cpu.esp.dword-=2;
        memory.writew(cpu.ss.dword+cpu.esp.dword, 0xAAAA);
        memory.writew(cpu.ss.dword+cpu.esp.dword-2, 0x1234);
        memory.writew(cpu.ss.dword+cpu.esp.dword-4, 0xBBBB);
        cpu.esp.dword-=2;
        runCPU();

        assertTrue(cpu.esp.dword==4094);
        assertTrue(memory.readd(cpu.ds.dword + 200) == ((DEFAULT & 0xFFFF0000) | 0x1234));
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword)==0xAAAA);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword-4)==0xBBBB);
    }

    protected void PopEd() {
        for (int i=0;i<8;i++) {
            if (i==4)
                continue;
            newInstruction(0x8f, 0);
            pushCode8(i|0xC0);
            Reg reg = cpu.regs[Decoder.ew(i|0xC0)];
            cpu.esp.dword-=4;
            memory.writed(cpu.ss.dword+cpu.esp.dword, 0xAAAAAAAA);
            memory.writed(cpu.ss.dword+cpu.esp.dword-4, 0x56781234);
            memory.writed(cpu.ss.dword+cpu.esp.dword-8, 0xBBBBBBBB);
            cpu.esp.dword-=4;
            runCPU();
            assertTrue(cpu.esp.dword==4092);
            assertTrue(reg.dword == 0x56781234);
            assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword)==0xAAAAAAAA);
            assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword-8)==0xBBBBBBBB);
        }

        newInstruction(0x8f, 0);
        pushCode8(5);
        pushCode32(200);
        memory.writed(cpu.ds.dword + 200, DEFAULT);

        cpu.esp.dword-=4;
        memory.writed(cpu.ss.dword + cpu.esp.dword, 0xAAAAAAAA);
        memory.writed(cpu.ss.dword + cpu.esp.dword - 4, 0x56781234);
        memory.writed(cpu.ss.dword + cpu.esp.dword - 8, 0xBBBBBBBB);
        cpu.esp.dword-=4;
        runCPU();

        assertTrue(cpu.esp.dword==4092);
        assertTrue(memory.readd(cpu.ds.dword + 200) == 0x56781234);
        assertTrue(memory.readd(cpu.ss.dword + cpu.esp.dword)==0xAAAAAAAA);
        assertTrue(memory.readd(cpu.ss.dword + cpu.esp.dword - 8)==0xBBBBBBBB);
    }

    protected void Push16(int instruction) {
        newInstruction(instruction, 0);
        pushCode16(0x1234);
        cpu.esp.dword-=2;
        memory.writew(cpu.ss.dword+cpu.esp.dword, 0xAAAA);
        memory.writew(cpu.ss.dword+cpu.esp.dword-2, 0xCCCC);
        memory.writew(cpu.ss.dword+cpu.esp.dword-4, 0xBBBB);
        runCPU();
        assertTrue(cpu.esp.dword==4092);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword)==0x1234);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword+2)==0xAAAA);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword-2)==0xBBBB);
    }

    protected void Push32(int instruction) {
        newInstruction(instruction, 0);
        pushCode32(0x56781234);
        cpu.esp.dword-=4;
        memory.writed(cpu.ss.dword+cpu.esp.dword, 0xAAAAAAAA);
        memory.writed(cpu.ss.dword+cpu.esp.dword-4, 0xCCCCCCCC);
        memory.writed(cpu.ss.dword+cpu.esp.dword-8, 0xBBBBBBBB);
        runCPU();
        assertTrue(cpu.esp.dword==4088);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword)==0x56781234);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword+4)==0xAAAAAAAA);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword-4)==0xBBBBBBBB);
    }

    protected void Push16s8(int instruction) {
        newInstruction(instruction, 0);
        pushCode8(0xFC); // -4
        cpu.esp.dword-=2;
        memory.writew(cpu.ss.dword+cpu.esp.dword, 0xAAAA);
        memory.writew(cpu.ss.dword+cpu.esp.dword-2, 0xCCCC);
        memory.writew(cpu.ss.dword+cpu.esp.dword-4, 0xBBBB);
        runCPU();
        assertTrue(cpu.esp.dword==4092);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword)==0xFFFC);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword+2)==0xAAAA);
        assertTrue(memory.readw(cpu.ss.dword+cpu.esp.dword-2)==0xBBBB);
    }

    protected void Push32s8(int instruction) {
        newInstruction(instruction, 0);
        pushCode8(0xFC); // -4
        cpu.esp.dword-=4;
        memory.writed(cpu.ss.dword+cpu.esp.dword, 0xAAAAAAAA);
        memory.writed(cpu.ss.dword+cpu.esp.dword-4, 0xCCCCCCCC);
        memory.writed(cpu.ss.dword+cpu.esp.dword-8, 0xBBBBBBBB);
        runCPU();
        assertTrue(cpu.esp.dword==4088);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword)==0xFFFFFFFC);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword+4)==0xAAAAAAAA);
        assertTrue(memory.readd(cpu.ss.dword+cpu.esp.dword-4)==0xBBBBBBBB);
    }

    protected class Data {
        public Data(int left, int right, int result, int flags, boolean CF, boolean OF) {
            this.left = left;
            this.right = right;
            this.result = result;
            this.flags = flags;
            this.CF = CF;
            this.OF = OF;
            dontUseResultAndCheckSFZF = false;
        }

        public Data(int left, int right, int result, int flags, boolean CF, boolean OF, int constant, int rightResult) {
            this.left = left;
            this.right = right;
            this.result = result;
            this.flags = flags;
            this.CF = CF;
            this.OF = OF;
            this.resultRight = rightResult;
            this.useResultRight = true;
            this.constant = constant;
            dontUseResultAndCheckSFZF = false;
        }

        public Data(int left, int right, int result, int constant, int constantWidth, int flags, boolean CF, boolean OF) {
            this.left = left;
            this.right = right;
            this.result = result;
            this.flags = flags;
            this.CF = CF;
            this.OF = OF;
            this.constant = constant;
            this.constantWidth = constantWidth;
        }

        public Data(int left, int right, int resultLeft, int resultRight) {
            this.left = left;
            this.right = right;
            this.result = resultLeft;
            this.resultRight = resultRight;
            this.useResultRight = true;
        }

        public Data(int left, int right, boolean CF, boolean OF, boolean SF, boolean ZF) {
            this.left = left;
            this.right = right;
            this.result = 0;
            this.flags = 0;
            this.CF = CF;
            this.OF = OF;
            this.ZF = ZF;
            this.SF = SF;
            this.dontUseResultAndCheckSFZF = true;
        }
        public int left;
        public int right;
        public int result;
        public int resultRight;
        public int flags;
        public int constant;
        public boolean CF;
        public boolean OF;
        public boolean ZF;
        public boolean SF;
        public boolean dontUseResultAndCheckSFZF=false;
        public boolean useResultRight=false;
        public int constantWidth = 0;

        public void pushConstant() {
            if (constantWidth==8) {
                pushCode8(constant);
            } else if (constantWidth==16) {
                pushCode16(constant);
            } else if (constantWidth==32) {
                pushCode32(constant);
            }
        }

        protected StringBuilder buildErrorMsg(CPU cpu, int instruction, int resultLeft, int resultRight, String leftRegName, String rightRegName) {
            StringBuilder builder = new StringBuilder();
            builder.append("op ");
            builder.append(Integer.toHexString(instruction));
            builder.append(" failed: ");
            if (leftRegName!=null)
                builder.append(leftRegName);
            builder.append("(");
            builder.append(cpu.left);
            if (!dontUseResultAndCheckSFZF) {
                builder.append(",");
                builder.append(resultLeft);
            }
            builder.append("), ");
            if (rightRegName!=null)
                builder.append(rightRegName);
            builder.append("(");
            builder.append(cpu.right);
            if (useResultRight) {
                builder.append(",");
                builder.append(resultRight);
            }
            builder.append(")");
            if (dontUseResultAndCheckSFZF) {
                builder.append(" result(");
                builder.append(cpu.result);
                builder.append(")");
            }
            return builder;
        }
        public void assertResult(CPU cpu, int instruction, int resultLeft, int resultRight, String leftRegName, String rightRegName, Reg r1, Reg r2, int address, int bits) {
            if (useResultRight && this.resultRight!=resultRight) {
                StringBuilder builder = buildErrorMsg(cpu, instruction, resultLeft, resultRight, leftRegName, rightRegName);
                builder.append(" did not equal ");
                builder.append(this.resultRight);
                assertTrue(builder.toString(), false);
            }
            if (!dontUseResultAndCheckSFZF && this.result != resultLeft) {
                StringBuilder builder = buildErrorMsg(cpu, instruction, resultLeft, resultRight, leftRegName, rightRegName);
                builder.append(" did not equal ");
                builder.append(this.result);
                assertTrue(builder.toString(), false);
            }
            if (cpu.CF() != CF) {
                StringBuilder builder = buildErrorMsg(cpu, instruction, resultLeft, resultRight, leftRegName, rightRegName);
                builder.append(" expected CF value: ");
                builder.append(CF);
                assertTrue(builder.toString(), false);
            }
            if (cpu.OF() != OF) {
                StringBuilder builder = buildErrorMsg(cpu, instruction, resultLeft, resultRight, leftRegName, rightRegName);
                builder.append(" expected OF value: ");
                builder.append(OF);
                assertTrue(builder.toString(), false);
            }
            if (dontUseResultAndCheckSFZF) {
                if (cpu.SF() != SF) {
                    StringBuilder builder = buildErrorMsg(cpu, instruction, resultLeft, resultRight, leftRegName, rightRegName);
                    builder.append(" expected SF value: ");
                    builder.append(SF);
                    assertTrue(builder.toString(), false);
                }
                if (cpu.ZF() != ZF) {
                    StringBuilder builder = buildErrorMsg(cpu, instruction, resultLeft, resultRight, leftRegName, rightRegName);
                    builder.append(" expected ZF value: ");
                    builder.append(ZF);
                    assertTrue(builder.toString(), false);
                }
            }
            if (bits==8) {
                if (r1!=null) {
                    if (r1.parent!=null)
                        assertTrue((r1.parent.dword & 0xFFFF0000) == (DEFAULT & 0xFFFF0000));
                    else
                        assertTrue((r1.dword & 0xFFFF0000) == (DEFAULT & 0xFFFF0000));
                }
                if (r2!=null) {
                    if (r2.parent!=null)
                        assertTrue((r2.parent.dword & 0xFFFF0000) == (DEFAULT & 0xFFFF0000));
                    else
                        assertTrue((r2.dword & 0xFFFF0000) == (DEFAULT & 0xFFFF0000));
                }
                if (address!=0) {
                    assertTrue((memory.readd(address) & 0xFFFFFF00) == (DEFAULT & 0xFFFFFF00));
                }
            }
            if (bits==16) {
                if (r1!=null) {
                    assertTrue((r1.dword & 0xFFFF0000) == (DEFAULT & 0xFFFF0000));
                }
                if (r2!=null) {
                    assertTrue((r2.dword & 0xFFFF0000) == (DEFAULT & 0xFFFF0000));
                }
                if (address!=0) {
                    assertTrue((memory.readd(address) & 0xFFFF0000) == (DEFAULT & 0xFFFF0000));
                }
            }
        }
    }

    private Data[] addb = new Data[] {
            new Data(1, 2, 3, 0, false, false),
            new Data(0xFF, 1, 0, 0, true, false),
            new Data(1, 0xFF, 0, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xFF, 0xFF, 0xFE, 0, true, false),
            new Data(64, 64, 128, 0, false, true) // overflow indicates that the sign changed
    };

    private Data[] addw = new Data[] {
            new Data(1000, 2002, 3002, 0, false, false),
            new Data(0xFFFF, 1, 0, 0, true, false),
            new Data(1, 0xFFFF, 0, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xFFFF, 0xFFFF, 0xFFFE, 0, true, false),
            new Data(16384, 16384, 32768, 0, false, true) // overflow indicates that the sign changed
    };

    private Data[] addd = new Data[] {
            new Data(100000, 200200, 300200, 0, false, false),
            new Data(0xFFFFFFFF, 1, 0, 0, true, false),
            new Data(1, 0xFFFFFFFF, 0, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, true, false),
            new Data(0x40000000, 0x40000000, 0x80000000, 0, false, true) // overflow indicates that the sign changed
    };

    private Data[] orb = new Data[] {
            new Data(1, 2, 3, 0, false, false),
            new Data(0xFF, 0, 0xFF, 0, false, false),
            new Data(0, 0xFF, 0xFF, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xF0, 0x0F, 0xFF, 0, false, false)
    };

    private Data[] orw = new Data[] {
            new Data(1000, 2002, 2042, 0, false, false),
            new Data(0xFFFF, 0, 0xFFFF, 0, false, false),
            new Data(0, 0xFFFF, 0xFFFF, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xF00F, 0x0FF0, 0xFFFF, 0, false, false),
    };

    private Data[] ord = new Data[] {
            new Data(100000, 200200, 233128, 0, false, false),
            new Data(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, false, false),
            new Data(0, 0xFFFFFFFF, 0xFFFFFFFF, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xF0F0F0F0, 0x0F0F0F0F, 0xFFFFFFFF, 0, false, false)
    };

    private Data[] adcb = new Data[] {
            new Data(1, 2, 3, 0, false, false),
            new Data(0xFF, 1, 0, 0, true, false),
            new Data(1, 0xFF, 0, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xFF, 0xFF, 0xFE, 0, true, false),
            new Data(64, 64, 128, 0, false, true), // overflow indicates that the sign changed
            new Data(1, 2, 4, CPU.CF, false, false),
            new Data(0xFF, 1, 1, CPU.CF, true, false),
            new Data(1, 0xFF, 1, CPU.CF, true, false),
            new Data(0, 0, 1, CPU.CF, false, false),
            new Data(0xFF, 0xFF, 0xFF, CPU.CF, true, false),
            new Data(64, 64, 129, CPU.CF, false, true) // overflow indicates that the sign changed
    };

    private Data[] adcw = new Data[] {
            new Data(1000, 2002, 3002, 0, false, false),
            new Data(0xFFFF, 1, 0, 0, true, false),
            new Data(1, 0xFFFF, 0, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xFFFF, 0xFFFF, 0xFFFE, 0, true, false),
            new Data(16384, 16384, 32768, 0, false, true), // overflow indicates that the sign changed
            new Data(1000, 2002, 3003, CPU.CF, false, false),
            new Data(0xFFFF, 1, 1, CPU.CF, true, false),
            new Data(1, 0xFFFF, 1, CPU.CF, true, false),
            new Data(0, 0, 1, CPU.CF, false, false),
            new Data(0xFFFF, 0xFFFF, 0xFFFF, CPU.CF, true, false),
            new Data(16384, 16384, 32769, CPU.CF, false, true), // overflow indicates that the sign changed
    };

    private Data[] adcd = new Data[] {
            new Data(100000, 200200, 300200, 0, false, false),
            new Data(0xFFFFFFFF, 1, 0, 0, true, false),
            new Data(1, 0xFFFFFFFF, 0, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, true, false),
            new Data(0x40000000, 0x40000000, 0x80000000, 0, false, true), // overflow indicates that the sign changed
            new Data(100000, 200200, 300201, CPU.CF, false, false),
            new Data(0xFFFFFFFF, 1, 1, CPU.CF, true, false),
            new Data(1, 0xFFFFFFFF, 1, CPU.CF, true, false),
            new Data(0, 0, 1, CPU.CF, false, false),
            new Data(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, CPU.CF, true, false),
            new Data(0x40000000, 0x40000000, 0x80000001, CPU.CF, false, true) // overflow indicates that the sign changed
    };

    private Data[] sbbb = new Data[] {
            new Data(1, 2, 0xFF, 0, true, false),
            new Data(2, 1, 1, 0, false, false),
            new Data(0xFF, 0, 0xFF, 0, false, false),
            new Data(0, 0xFF, 1, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0x80, 0x10, 0x70, 0, false, true),
            new Data(1, 2, 0xFE, CPU.CF, true, false),
            new Data(2, 1, 0, CPU.CF, false, false),
            new Data(0xFF, 0, 0xFE, CPU.CF, false, false),
            new Data(0, 0xFF, 0, CPU.CF, true, false),
            new Data(0, 0, 0xFF, CPU.CF, true, false),
            new Data(0x80, 0x10, 0x6F, CPU.CF, false, true)
    };

    private Data[] sbbw = new Data[] {
            new Data(0xAAAA, 0xBBBB, 0xEEEF, 0, true, false),
            new Data(0xBBBB, 0xAAAA, 0x1111, 0, false, false),
            new Data(0xFFFF, 0, 0xFFFF, 0, false, false),
            new Data(0, 0xFFFF, 1, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0x8000, 0x1000, 0x7000, 0, false, true),
            new Data(0xAAAA, 0xBBBB, 0xEEEE, CPU.CF, true, false),
            new Data(0xBBBB, 0xAAAA, 0x1110, CPU.CF, false, false),
            new Data(0xFFFF, 0, 0xFFFE, CPU.CF, false, false),
            new Data(0, 0xFFFF, 0, CPU.CF, true, false),
            new Data(0, 0, 0xFFFF, CPU.CF, true, false),
            new Data(0x8000, 0x1000, 0x6FFF, CPU.CF, false, true),
    };

    private Data[] sbbd = new Data[] {
            new Data(0xAAAAAAAA, 0xBBBBBBBB, 0xEEEEEEEF, 0, true, false),
            new Data(0xBBBBBBBB, 0xAAAAAAAA, 0x11111111, 0, false, false),
            new Data(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, false, false),
            new Data(0, 0xFFFFFFFF, 1, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0x80000000, 0x10000000, 0x70000000, 0, false, true),
            new Data(0xAAAAAAAA, 0xBBBBBBBB, 0xEEEEEEEE, CPU.CF, true, false),
            new Data(0xBBBBBBBB, 0xAAAAAAAA, 0x11111110, CPU.CF, false, false),
            new Data(0xFFFFFFFF, 0, 0xFFFFFFFE, CPU.CF, false, false),
            new Data(0, 0xFFFFFFFF, 0, CPU.CF, true, false),
            new Data(0, 0, 0xFFFFFFFF, CPU.CF, true, false),
            new Data(0x80000000, 0x10000000, 0x6FFFFFFF, CPU.CF, false, true),
    };

    private Data[] andb = new Data[] {
            new Data(1, 2, 0, 0, false, false),
            new Data(0xFF, 1, 1, 0, false, false),
            new Data(1, 0xFF, 1, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xFF, 0xFF, 0xFF, 0, false, false),
            new Data(0xF0, 0x0F, 0, 0, false, false)
    };

    private Data[] andw = new Data[] {
            new Data(0x1000, 0x2000, 0, 0, false, false),
            new Data(0xFFFF, 0x100, 0x100, 0, false, false),
            new Data(0x100, 0xFFFF, 0x100, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xFFFF, 0xFFFF, 0xFFFF, 0, false, false),
            new Data(0xFF00, 0x00FF, 0, 0, false, false)
    };

    private Data[] andd = new Data[] {
            new Data(0x10000000, 0x20000000, 0, 0, false, false),
            new Data(0xFFFFFFFF, 0x1000000, 0x1000000, 0, false, false),
            new Data(0x1000000, 0xFFFFFFFF, 0x1000000, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, false, false),
            new Data(0xFF00FF00, 0x00FF00FF, 0, 0, false, false)
    };

    private Data[] subb = new Data[] {
            new Data(1, 2, 0xFF, 0, true, false),
            new Data(2, 1, 1, 0, false, false),
            new Data(0xFF, 0, 0xFF, 0, false, false),
            new Data(0, 0xFF, 1, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0x80, 0x10, 0x70, 0, false, true)
    };

    private Data[] subw = new Data[] {
            new Data(0xAAAA, 0xBBBB, 0xEEEF, 0, true, false),
            new Data(0xBBBB, 0xAAAA, 0x1111, 0, false, false),
            new Data(0xFFFF, 0, 0xFFFF, 0, false, false),
            new Data(0, 0xFFFF, 1, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0x8000, 0x1000, 0x7000, 0, false, true)
    };

    private Data[] subd = new Data[] {
            new Data(0xAAAAAAAA, 0xBBBBBBBB, 0xEEEEEEEF, 0, true, false),
            new Data(0xBBBBBBBB, 0xAAAAAAAA, 0x11111111, 0, false, false),
            new Data(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, false, false),
            new Data(0, 0xFFFFFFFF, 1, 0, true, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0x80000000, 0x10000000, 0x70000000, 0, false, true)
    };

    private Data[] xorb = new Data[] {
            new Data(1, 2, 3, 0, false, false),
            new Data(0xFF, 0, 0xFF, 0, false, false),
            new Data(0, 0xFF, 0xFF, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xF0, 0xFF, 0x0F, 0, false, false)
    };

    private Data[] xorw = new Data[] {
            new Data(0x1000, 0x2000, 0x3000, 0, false, false),
            new Data(0xFFFF, 0, 0xFFFF, 0, false, false),
            new Data(0, 0xFFFF, 0xFFFF, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xF00F, 0xFFFF, 0x0FF0, 0, false, false),
    };

    private Data[] xord = new Data[] {
            new Data(0x100000, 0x200000, 0x300000, 0, false, false),
            new Data(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, false, false),
            new Data(0, 0xFFFFFFFF, 0xFFFFFFFF, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0xFF00FF00, 0x00FFFFFF, 0xFFFF00FF, 0, false, false)
    };

    private Data[] cmpb = new Data[] {
            new Data(1, 2, true, false, true, false),
            new Data(2, 1, false, false, false, false),
            new Data(1, 1, false, false, false, true),
            new Data(0, 0, false, false, false, true),
            new Data(0xD0, 0xB0, false, false, false, false),
            new Data(0xB0, 0xD0, true, false, true, false),
            new Data(0xB0, 0xB0, false, false, false, true),
            new Data(0xB0, 1, false, false, true, false),
            new Data(0x90, 0x30, false, true, false, false),
            new Data(0x30, 0x90, true, true, true, false)
    };

    private Data[] cmpw = new Data[] {
            new Data(0xAAAA, 0xBBBB, true, false, true, false),
            new Data(0xBBBB, 0xAAAA, false, false, false, false),
            new Data(0xAAAA, 0xAAAA, false, false, false, true),
            new Data(0, 0, false, false, false, true),
            new Data(0xD000, 0xB000, false, false, false, false),
            new Data(0xB000, 0xD000, true, false, true, false),
            new Data(0xB000, 0xB000, false, false, false, true),
            new Data(0xB000, 1, false, false, true, false),
            new Data(0x9000, 0x3000, false, true, false, false),
            new Data(0x3000, 0x9000, true, true, true, false)
    };

    private Data[] cmpd = new Data[] {
            new Data(0xAAAA0000, 0xBBBB0000, true, false, true, false),
            new Data(0xBBBB0000, 0xAAAA0000, false, false, false, false),
            new Data(0xAAAA0000, 0xAAAA0000, false, false, false, true),
            new Data(0, 0, false, false, false, true),
            new Data(0xD0000000, 0xB0000000, false, false, false, false),
            new Data(0xB0000000, 0xD0000000, true, false, true, false),
            new Data(0xB0000000, 0xB0000000, false, false, false, true),
            new Data(0xB0000000, 1, false, false, true, false),
            new Data(0x90000000, 0x30000000, false, true, false, false),
            new Data(0x30000000, 0x90000000, true, true, true, false)
    };

    private Data[] testb = new Data[] {
            new Data(1, 2, false, false, false, true),
            new Data(2, 1, false, false, false, true),
            new Data(1, 1, false, false, false, false),
            new Data(0, 0, false, false, false, true),
            new Data(0xD0, 0xB0, false, false, true, false),
            new Data(0xB0, 0xD0, false, false, true, false),
            new Data(0xB0, 0xB0, false, false, true, false),
            new Data(0xB0, 1, false, false, false, true),
            new Data(0x90, 0x30, false, false, false, false),
            new Data(0x30, 0x90, false, false, false, false)
    };

    private Data[] testw = new Data[] {
            new Data(0xAAAA, 0xBBBB, false, false, true, false),
            new Data(0xBBBB, 0xAAAA, false, false, true, false),
            new Data(0xAAAA, 0xAAAA, false, false, true, false),
            new Data(0, 0, false, false, false, true),
            new Data(0xD000, 0xB000, false, false, true, false),
            new Data(0xB000, 0xD000, false, false, true, false),
            new Data(0xB000, 0xB000, false, false, true, false),
            new Data(0xB000, 1, false, false, false, true),
            new Data(0x9000, 0x3000, false, false, false, false),
            new Data(0x3000, 0x9000, false, false, false, false)
    };

    private Data[] testd = new Data[] {
            new Data(0xAAAA0000, 0xBBBB0000, false, false, true, false),
            new Data(0xBBBB0000, 0xAAAA0000, false, false, true, false),
            new Data(0xAAAA0000, 0xAAAA0000, false, false, true, false),
            new Data(0, 0, false, false, false, true),
            new Data(0xD0000000, 0xB0000000, false, false, true, false),
            new Data(0xB0000000, 0xD0000000, false, false, true, false),
            new Data(0xB0000000, 0xB0000000, false, false, true, false),
            new Data(0xB0000000, 1, false, false, false, true),
            new Data(0x90000000, 0x30000000, false, false, false, false),
            new Data(0x30000000, 0x90000000, false, false, false, false)
    };

    private Data[] incb = new Data[]{
            new Data(0, 0, 1, 0, false, false),
            new Data(0, 0, 1, CPU.CF, true, false), // it should keep the previous carry flag
            new Data(0x80, 0, 0x81, 0, false, false),
            new Data(0x7F, 0, 0x80, 0, false, true)
    };

    private Data[] incw = new Data[]{
            new Data(0, 0, 1, 0, false, false),
            new Data(0, 0, 1, CPU.CF, true, false), // it should keep the previous carry flag
            new Data(0x8000, 0, 0x8001, 0, false, false),
            new Data(0x7FFF, 0, 0x8000, 0, false, true),
            new Data(0xFFFF, 0, 0, 0, false, false) // carry flag is not set
    };

    private Data[] incd = new Data[]{
            new Data(0, 0, 1, 0, false, false),
            new Data(0, 0, 1, CPU.CF, true, false), // it should keep the previous carry flag
            new Data(0x80000000, 0, 0x80000001, 0, false, false),
            new Data(0x7FFFFFFF, 0, 0x80000000, 0, false, true),
            new Data(0xFFFFFFFF, 0, 0, 0, false, false) // carry flag is not set
    };

    private Data[] decb = new Data[]{
            new Data(2, 0, 1, 0, false, false),
            new Data(1, 0, 0, CPU.CF, true, false), // it should keep the previous carry flag
            new Data(0x80, 0, 0x7F, 0, false, true),
            new Data(0, 0, 0xFF, 0, false, false)
    };

    private Data[] decw = new Data[]{
            new Data(2, 0, 1, 0, false, false),
            new Data(1, 0, 0, CPU.CF, true, false), // it should keep the previous carry flag
            new Data(0x8000, 0, 0x7FFF, 0, false, true),
            new Data(0, 0, 0xFFFF, 0, false, false)
    };

    private Data[] decd = new Data[]{
            new Data(2, 0, 1, 0, false, false),
            new Data(1, 0, 0, CPU.CF, true, false), // it should keep the previous carry flag
            new Data(0x80000000, 0, 0x7FFFFFFF, 0, false, true),
            new Data(0, 0, 0xFFFFFFFF, 0, false, false)
    };

    private Data[] imulw = new Data[]{
            new Data(0, 2, 4, 2, 16, 0, false, false),
            new Data(0, 0xFFFE, 0xFFFC, 2, 16, 0, false, false), // -2 * 2 = -4
            new Data(0, 0xFFFE, 4, 0xFFFE, 16, CPU.CF|CPU.OF, false, false), // -2 * -2 = 4 (also, make sure it clears the flags)
            new Data(0, 300, 0x5F90, 300, 16, 0, true, true), // 300 x 300 = 0x15F90
            new Data(0, -300, 0xA070, 300, 16, 0, true, true),
    };

    private Data[] imuld = new Data[]{
            new Data(0, 2, 4, 2, 32, 0, false, false),
            new Data(0, 0xFFFFFFFE, 0xFFFFFFFC, 2, 32, 0, false, false), // -2 * 2 = -4
            new Data(0, 0xFFFFFFFE, 4, 0xFFFFFFFE, 32, CPU.CF|CPU.OF, false, false), // -2 * -2 = 4 (also, make sure it clears the flags)
            new Data(0, 300000, 0xF08EB000, 400000, 32, 0, true, true), // = 1BF08EB000
            new Data(0, -300000, 0x0F715000, 400000, 32, 0, true, true),
    };

    private Data[] imulw_s8 = new Data[]{
            new Data(0, 2, 4, 2, 8, 0, false, false),
            new Data(0, 0xFFFE, 0xFFFC, 2, 8, 0, false, false), // -2 * 2 = -4
            new Data(0, 0xFFFE, 4, 0xFE, 8, CPU.CF|CPU.OF, false, false), // -2 * -2 = 4 (also, make sure it clears the flags)
            new Data(0, 3000, 0xD048, 127, 8, 0, true, true), // 3000 x 127 = 0x5D048
            new Data(0, -3000, 0x2FB8, 127, 8, 0, true, true),
    };

    private Data[] imuld_s8 = new Data[]{
            new Data(0, 2, 4, 2, 8, 0, false, false),
            new Data(0, 0xFFFFFFFE, 0xFFFFFFFC, 2, 8, 0, false, false), // -2 * 2 = -4
            new Data(0, 0xFFFFFFFE, 4, 0xFE, 8, CPU.CF|CPU.OF, false, false), // -2 * -2 = 4 (also, make sure it clears the flags)
            new Data(0, 300000000, 0xDEEFDD00, 127, 8, 0, true, true), // = 8DEEFDD00
            new Data(0, -300000000, 0x21102300, 127, 8, 0, true, true),
    };

    private Data[] xchgb = new Data[]{
            new Data(1, 2, 2, 1),
            new Data(0, 0, 0, 0),
            new Data(0xFC, 0xAB, 0xAB, 0xFC)
    };

    private Data[] xchgw = new Data[]{
            new Data(1, 2, 2, 1),
            new Data(0, 0, 0, 0),
            new Data(0xFC15, 0xAB38, 0xAB38, 0xFC15)
    };

    private Data[] xchgd = new Data[]{
            new Data(1, 2, 2, 1),
            new Data(0, 0, 0, 0),
            new Data(0xFC150146, 0xAB38, 0xAB38, 0xFC150146)
    };

    private Data[] movb = new Data[] {
            new Data(0, 1, 1, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0, 0xFF, 0xFF, 0, false, false),
            new Data(0, 0x7F, 0x7F, 0, false, false)
    };

    private Data[] movw = new Data[] {
            new Data(0, 1, 1, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0, 0xFFFF, 0xFFFF, 0, false, false),
            new Data(0, 0x7FFF, 0x7FFF, 0, false, false)
    };

    private Data[] movd = new Data[] {
            new Data(0, 1, 1, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0, 0xFFFF, 0xFFFF, 0, false, false),
            new Data(0, 0x7FFF, 0x7FFF, 0, false, false),
            new Data(0, 0xFFFFFFFF, 0xFFFFFFFF, 0, false, false),
            new Data(0, 0x7FFFFFFF, 0x7FFFFFFF, 0, false, false),
            new Data(0, 0x08080808, 0x08080808, 0, false, false)
    };

    private Data[] cbw = new Data[] {
            new Data(0x1234, 0, 0x0034, 0, false, false),
            new Data(0x12FE, 0, 0xFFFE, 0, false, false)
    };

    private Data[] cwde = new Data[] {
            new Data(0x12345678, 0, 0x5678, 0, false, false),
            new Data(0x1234FFFE, 0, 0xFFFFFFFE, 0, false, false)
    };

    private Data[] cwd = new Data[] {
            new Data(0x1234, 0x1234, 0x1234, 0),
            new Data(0xFFFE, 0x1234, 0xFFFE, 0xFFFF),
    };

    private Data[] cdq = new Data[] {
            new Data(0x12345678, 0x12345678, 0x12345678, 0),
            new Data(0xFFFFFFFE, 0x12345678, 0xFFFFFFFE, 0xFFFFFFFF),
    };

    private Data[] sahf = new Data[] {
            new Data(0x12345600, 0x0000FF00, 0x123456D5, 0x0000FF00),
            new Data(0xFFFFFFFF, 0x00000000, 0xFFFFFF2A, 0x00000000),
    };

    private Data[] lahf = new Data[] {
            new Data(0x123456FF, 0x00000000, 0x123456FF, 0x0000D500),
            new Data(0xFFFFFF00, 0xFFFFFFFF, 0xFFFFFF00, 0xFFFF01FF),
    };

    private Data[] rolb = new Data[] {
            new Data(0x40, 1, 0x80, 0, false, true),
            new Data(0x01, 1, 0x02, 0, false, false),
            new Data(0x80, 1, 0x01, 0, true, true),
            new Data(0x30, 4, 0x03, 0, true, true),
            new Data(0x30, 12, 0x03, 0, true, true),
            new Data(0x01, 0, 0x01, 0, false, false),
            new Data(0x01, 8, 0x01, 0, true, true),
            new Data(0x80, 8, 0x80, 0, false, true),
            new Data(0x01, 9, 0x02, 0, false, false),
            new Data(0x01, 32, 0x01, 0, false, false),
    };

    private Data[] rolb_1 = new Data[] {
            new Data(0x40, 1, 0x80, 0, false, true),
            new Data(0x01, 1, 0x02, 0, false, false),
            new Data(0x80, 1, 0x01, 0, true, true),
    };

    private Data[] rorb = new Data[] {
            new Data(0x02, 1, 0x01, 0, false, false),
            new Data(0x80, 1, 0x40, 0, false, true),
            new Data(0x01, 1, 0x80, 0, true, true),
            new Data(0x03, 4, 0x30, 0, false, false),
            new Data(0x03, 12, 0x30, 0, false, false),
            new Data(0x01, 0, 0x01, 0, false, false),
            new Data(0x01, 8, 0x01, 0, false, false),
            new Data(0x80, 8, 0x80, 0, true, true),
            new Data(0x80, 9, 0x40, 0, false, true),
            new Data(0x80, 32, 0x80, 0, false, false),
    };

    private Data[] rorb_1 = new Data[] {
            new Data(0x02, 1, 0x01, 0, false, false),
            new Data(0x80, 1, 0x40, 0, false, true),
            new Data(0x01, 1, 0x80, 0, true, true),
    };

    private Data[] rclb = new Data[] {
            new Data(0x40, 1, 0x80, 0, false, true),
            new Data(0x01, 1, 0x02, 0, false, false),
            new Data(0x80, 1, 0x00, 0, true, true),
            new Data(0x30, 5, 0x03, 0, false, true),
            new Data(0x30, 14, 0x03, 0, false, true),
            new Data(0x01, 0, 0x01, 0, false, false),
            new Data(0x01, 9, 0x01, 0, false, false),
            new Data(0x80, 9, 0x80, 0, false, true),
            new Data(0x01, 10, 0x02, 0, false, false),
            new Data(0x01, 32, 0x01, 0, false, false),
            new Data(0x00, 1, 0x01, CPU.CF, false, false),
            new Data(0x80, 2, 0x03, CPU.CF, false, true),
    };

    private Data[] rclb_1 = new Data[] {
            new Data(0x40, 1, 0x80, 0, false, true),
            new Data(0x01, 1, 0x02, 0, false, false),
            new Data(0x80, 1, 0x00, 0, true, true),
            new Data(0x00, 1, 0x01, CPU.CF, false, false),
    };

    private Data[] rcrb = new Data[] {
            new Data(0x02, 1, 0x01, 0, false, false),
            new Data(0x80, 1, 0x40, 0, false, true),
            new Data(0x01, 1, 0x00, 0, true, false),
            new Data(0x03, 5, 0x30, 0, false, false),
            new Data(0x03, 14, 0x30, 0, false, false),
            new Data(0x01, 0, 0x01, 0, false, false),
            new Data(0x01, 9, 0x01, 0, false, false),
            new Data(0x80, 9, 0x80, 0, false, true),
            new Data(0x80, 10, 0x40, 0, false, true),
            new Data(0x80, 32, 0x80, 0, false, false),
            new Data(0x00, 1, 0x80, CPU.CF, false, true),
            new Data(0x01, 2, 0xC0, CPU.CF, false, false),
    };

    private Data[] rcrb_1 = new Data[] {
            new Data(0x02, 1, 0x01, 0, false, false),
            new Data(0x80, 1, 0x40, 0, false, true),
            new Data(0x01, 1, 0x00, 0, true, false),
            new Data(0x00, 1, 0x80, CPU.CF, false, true),
    };

    private Data[] shlb = new Data[] {
            new Data(0x40, 1, 0x80, 0, false, true),
            new Data(0x01, 1, 0x02, 0, false, false),
            new Data(0x80, 1, 0x00, 0, true, true),
            new Data(0x03, 4, 0x30, 0, false, false),
            new Data(0x03, 12, 0x00, 0, false, false),
            new Data(0x01, 0, 0x01, 0, false, false),
            new Data(0x01, 32, 0x01, 0, false, false),
    };

    private Data[] shlb_1 = new Data[] {
            new Data(0x40, 1, 0x80, 0, false, true),
            new Data(0x01, 1, 0x02, 0, false, false),
            new Data(0x80, 1, 0x00, 0, true, true),
    };

    private Data[] shrb = new Data[] {
            new Data(0x40, 1, 0x20, 0, false, false),
            new Data(0x02, 1, 0x01, 0, false, false),
            new Data(0x80, 1, 0x40, 0, false, false),
            new Data(0x01, 1, 0x00, 0, true, false),
            new Data(0x30, 4, 0x03, 0, false, false),
            new Data(0x03, 12, 0x00, 0, false, false),
            new Data(0x01, 0, 0x01, 0, false, false),
            new Data(0x01, 32, 0x01, 0, false, false),
    };

    private Data[] shrb_1 = new Data[] {
            new Data(0x40, 1, 0x20, 0, false, false),
            new Data(0x02, 1, 0x01, 0, false, false),
            new Data(0x80, 1, 0x40, 0, false, false),
            new Data(0x01, 1, 0x00, 0, true, false),
    };

    private Data[] sarb = new Data[] {
            new Data(0x40, 1, 0x20, 0, false, false),
            new Data(0x02, 1, 0x01, 0, false, false),
            new Data(0x80, 1, 0xC0, 0, false, false),
            new Data(0xC0, 7, 0xFF, 0, true, false),
            new Data(0x01, 1, 0x00, 0, true, false),
            new Data(0x30, 4, 0x03, 0, false, false),
            new Data(0x03, 12, 0x00, 0, false, false),
            new Data(0x01, 0, 0x01, 0, false, false),
            new Data(0x01, 32, 0x01, 0, false, false),
    };

    private Data[] sarb_1 = new Data[] {
            new Data(0x40, 1, 0x20, 0, false, false),
            new Data(0x02, 1, 0x01, 0, false, false),
            new Data(0x80, 1, 0xC0, 0, false, false),
    };

    private Data[] rolw = new Data[] {
            new Data(0x4000, 1, 0x8000, 0, false, true),
            new Data(0x0001, 1, 0x0002, 0, false, false),
            new Data(0x8000, 1, 0x0001, 0, true, true),
            new Data(0x3000, 8, 0x0030, 0, false, true),
            new Data(0x3000, 12, 0x0300, 0, false, true),
            new Data(0x0101, 0, 0x0101, 0, false, false),
            new Data(0x0101, 16, 0x0101, 0, true, true),
            new Data(0x8080, 16, 0x8080, 0, false, true),
            new Data(0x0101, 17, 0x0202, 0, false, false),
            new Data(0x0101, 32, 0x0101, 0, false, false),
    };

    private Data[] rolw_1 = new Data[] {
            new Data(0x4000, 1, 0x8000, 0, false, true),
            new Data(0x0001, 1, 0x0002, 0, false, false),
            new Data(0x8000, 1, 0x0001, 0, true, true),
    };

    private Data[] rorw = new Data[] {
            new Data(0x0002, 1, 0x0001, 0, false, false),
            new Data(0x8000, 1, 0x4000, 0, false, true),
            new Data(0x0001, 1, 0x8000, 0, true, true),
            new Data(0x0300, 8, 0x0003, 0, false, false),
            new Data(0x0300, 24, 0x003, 0, false, false),
            new Data(0x0101, 0, 0x0101, 0, false, false),
            new Data(0x0101, 16, 0x0101, 0, false, false),
            new Data(0x8000, 16, 0x8000, 0, true, true),
            new Data(0x8080, 17, 0x4040, 0, false, true),
            new Data(0x8080, 32, 0x8080, 0, false, false),
    };

    private Data[] rorw_1 = new Data[] {
            new Data(0x0002, 1, 0x0001, 0, false, false),
            new Data(0x8000, 1, 0x4000, 0, false, true),
            new Data(0x0001, 1, 0x8000, 0, true, true),
    };

    private Data[] rclw = new Data[] {
            new Data(0x4000, 1, 0x8000, 0, false, true),
            new Data(0x0101, 1, 0x0202, 0, false, false),
            new Data(0x8000, 1, 0x0000, 0, true, true),
            new Data(0x3000, 13, 0x0300, 0, false, true),
            new Data(0x3000, 30, 0x0300, 0, false, true),
            new Data(0x0101, 0, 0x0101, 0, false, false),
            new Data(0x0103, 17, 0x0103, 0, false, false),
            new Data(0x8070, 17, 0x8070, 0, false, true),
            new Data(0x0101, 18, 0x0202, 0, false, false),
            new Data(0x0102, 32, 0x0102, 0, false, false),
            new Data(0x0000, 1, 0x0001, CPU.CF, false, false),
            new Data(0x8000, 2, 0x0003, CPU.CF, false, true),
    };

    private Data[] rclw_1 = new Data[] {
            new Data(0x4000, 1, 0x8000, 0, false, true),
            new Data(0x0101, 1, 0x0202, 0, false, false),
            new Data(0x8000, 1, 0x0000, 0, true, true),
            new Data(0x0000, 1, 0x0001, CPU.CF, false, false),
    };

    private Data[] rcrw = new Data[] {
            new Data(0x0202, 1, 0x0101, 0, false, false),
            new Data(0x8080, 1, 0x4040, 0, false, true),
            new Data(0x0001, 1, 0x0000, 0, true, false),
            new Data(0x03, 5, 0x3000, 0, false, false),
            new Data(0x03, 22, 0x3000, 0, false, false),
            new Data(0x0100, 0, 0x0100, 0, false, false),
            new Data(0x0100, 17, 0x0100, 0, false, false),
            new Data(0x8000, 17, 0x8000, 0, false, true),
            new Data(0x8000, 18, 0x4000, 0, false, true),
            new Data(0x8070, 32, 0x8070, 0, false, false),
            new Data(0x0000, 1, 0x8000, CPU.CF, false, true),
            new Data(0x0001, 2, 0xC000, CPU.CF, false, false),
    };

    private Data[] rcrw_1 = new Data[] {
            new Data(0x0202, 1, 0x0101, 0, false, false),
            new Data(0x8080, 1, 0x4040, 0, false, true),
            new Data(0x0001, 1, 0x0000, 0, true, false),
            new Data(0x0000, 1, 0x8000, CPU.CF, false, true),
    };

    private Data[] shlw = new Data[] {
            new Data(0x4040, 1, 0x8080, 0, false, true),
            new Data(0x0101, 1, 0x0202, 0, false, false),
            new Data(0x8000, 1, 0x00, 0, true, true),
            new Data(0x0003, 8, 0x0300, 0, false, false),
            new Data(0x0003, 20, 0x00, 0, false, false),
            new Data(0x0102, 0, 0x0102, 0, false, false),
            new Data(0x0102, 32, 0x0102, 0, false, false),
    };

    private Data[] shlw_1 = new Data[] {
            new Data(0x4040, 1, 0x8080, 0, false, true),
            new Data(0x0101, 1, 0x0202, 0, false, false),
            new Data(0x8000, 1, 0x00, 0, true, true),
    };

    private Data[] shrw = new Data[] {
            new Data(0x4020, 1, 0x2010, 0, false, false),
            new Data(0x0802, 1, 0x0401, 0, false, false),
            new Data(0x8000, 1, 0x4000, 0, false, false),
            new Data(0x0001, 1, 0x0000, 0, true, false),
            new Data(0x3000, 12, 0x0003, 0, false, false),
            new Data(0x0300, 20, 0x0000, 0, false, false),
            new Data(0x0102, 0, 0x0102, 0, false, false),
            new Data(0x0102, 32, 0x0102, 0, false, false),
    };

    private Data[] shrw_1 = new Data[] {
            new Data(0x4020, 1, 0x2010, 0, false, false),
            new Data(0x0802, 1, 0x0401, 0, false, false),
            new Data(0x8000, 1, 0x4000, 0, false, false),
            new Data(0x0001, 1, 0x0000, 0, true, false),
    };

    private Data[] sarw = new Data[] {
            new Data(0x4020, 1, 0x2010, 0, false, false),
            new Data(0x0204, 1, 0x0102, 0, false, false),
            new Data(0x8000, 1, 0xC000, 0, false, false),
            new Data(0xC000, 15, 0xFFFF, 0, true, false),
            new Data(0x0001, 1, 0x0000, 0, true, false),
            new Data(0x3000, 12, 0x0003, 0, false, false),
            new Data(0x3000, 28, 0x0000, 0, false, false),
            new Data(0x0102, 0, 0x0102, 0, false, false),
            new Data(0x0102, 32, 0x0102, 0, false, false),
    };

    private Data[] sarw_1 = new Data[] {
            new Data(0x4020, 1, 0x2010, 0, false, false),
            new Data(0x0204, 1, 0x0102, 0, false, false),
            new Data(0x8000, 1, 0xC000, 0, false, false),
            new Data(0x0001, 1, 0x0000, 0, true, false),
    };

    private Data[] rold = new Data[] {
            new Data(0x40000000, 1, 0x80000000, 0, false, true),
            new Data(0x00000001, 1, 0x00000002, 0, false, false),
            new Data(0x80000000, 1, 0x00000001, 0, true, true),
            new Data(0x30000000, 24, 0x00300000, 0, false, false),
            new Data(0x01010101, 0, 0x01010101, 0, false, false),
            new Data(0x01010101, 32, 0x01010101, 0, false, false),
            new Data(0x80808080, 32, 0x80808080, 0, false, false),
            new Data(0x01010101, 33, 0x02020202, 0, false, false),
    };

    private Data[] rold_1 = new Data[] {
            new Data(0x40000000, 1, 0x80000000, 0, false, true),
            new Data(0x00000001, 1, 0x00000002, 0, false, false),
            new Data(0x80000000, 1, 0x00000001, 0, true, true),
    };

    private Data[] rord = new Data[] {
            new Data(0x00020000, 1, 0x00010000, 0, false, false),
            new Data(0x80000000, 1, 0x40000000, 0, false, false),
            new Data(0x00000001, 1, 0x80000000, 0, true, false),
            new Data(0x00000003, 8, 0x03000000, 0, false, false),
            new Data(0x03000000, 40, 0x00030000, 0, false, false),
            new Data(0x01020304, 0, 0x01020304, 0, false, false),
            new Data(0x01020304, 32, 0x01020304, 0, false, false),
            new Data(0x80000000, 32, 0x80000000, 0, false, false),
            new Data(0x80808080, 33, 0x40404040, 0, false, true),
    };

    private Data[] rord_1 = new Data[] {
            new Data(0x00020000, 1, 0x00010000, 0, false, false),
            new Data(0x80000000, 1, 0x40000000, 0, false, false),
            new Data(0x00000001, 1, 0x80000000, 0, true, false),
    };

    private Data[] rcld = new Data[] {
            new Data(0x40000000, 1, 0x80000000, 0, false, true),
            new Data(0x01010101, 1, 0x02020202, 0, false, false),
            new Data(0x80000000, 1, 0x00000000, 0, true, true),
            new Data(0x30000000, 29, 0x03000000, 0, false, false),
            new Data(0x30000000, 61, 0x03000000, 0, false, false),
            new Data(0x01020304, 0, 0x01020304, 0, false, false),
            new Data(0x01010101, 33, 0x02020202, 0, false, false),
            new Data(0x00000000, 1, 0x00000001, CPU.CF, false, false),
            new Data(0x80000000, 2, 0x00000003, CPU.CF, false, false),
    };

    private Data[] rcld_1 = new Data[] {
            new Data(0x40000000, 1, 0x80000000, 0, false, true),
            new Data(0x01010101, 1, 0x02020202, 0, false, false),
            new Data(0x80000000, 1, 0x00000000, 0, true, true),
            new Data(0x00000000, 1, 0x00000001, CPU.CF, false, false),
    };

    private Data[] rcrd = new Data[] {
            new Data(0x02020202, 1, 0x01010101, 0, false, false),
            new Data(0x80808080, 1, 0x40404040, 0, false, true),
            new Data(0x00000001, 1, 0x00000000, 0, true, false),
            new Data(0x00000003, 5, 0x30000000, 0, false, false),
            new Data(0x00000003, 37, 0x30000000, 0, false, false),
            new Data(0x01020304, 0, 0x01020304, 0, false, false),
            new Data(0x00000000, 1, 0x80000000, CPU.CF, false, true),
            new Data(0x00000001, 2, 0xC0000000, CPU.CF, false, false),
    };

    private Data[] rcrd_1 = new Data[] {
            new Data(0x02020202, 1, 0x01010101, 0, false, false),
            new Data(0x80808080, 1, 0x40404040, 0, false, true),
            new Data(0x00000001, 1, 0x00000000, 0, true, false),
            new Data(0x00000000, 1, 0x80000000, CPU.CF, false, true),
     };

    private Data[] shld = new Data[] {
            new Data(0x40404040, 1, 0x80808080, 0, false, true),
            new Data(0x01010101, 1, 0x02020202, 0, false, false),
            new Data(0x80000000, 1, 0x00000000, 0, true, true),
            new Data(0x00000003, 16, 0x00030000, 0, false, false),
            new Data(0x00030000, 20, 0x00000000, 0, false, false),
            new Data(0x01020304, 0, 0x01020304, 0, false, false),
            new Data(0x01020304, 32, 0x01020304, 0, false, false),
    };

    private Data[] shld_1 = new Data[] {
            new Data(0x40404040, 1, 0x80808080, 0, false, true),
            new Data(0x01010101, 1, 0x02020202, 0, false, false),
            new Data(0x80000000, 1, 0x00000000, 0, true, true),
    };

    private Data[] shrd = new Data[] {
            new Data(0x00804020, 1, 0x00402010, 0, false, false),
            new Data(0x80000000, 1, 0x40000000, 0, false, true),
            new Data(0x00000001, 1, 0x00000000, 0, true, false),
            new Data(0x30000000, 28, 0x00000003, 0, false, false),
            new Data(0x30000000, 30, 0x00000000, 0, true, false),
            new Data(0x01020304, 0, 0x01020304, 0, false, false),
            new Data(0x01020304, 32, 0x01020304, 0, false, false),
    };

    private Data[] shrd_1 = new Data[] {
            new Data(0x00804020, 1, 0x00402010, 0, false, false),
            new Data(0x80000000, 1, 0x40000000, 0, false, true),
            new Data(0x00000001, 1, 0x00000000, 0, true, false),
    };

    private Data[] sard = new Data[] {
            new Data(0x00804020, 1, 0x00402010, 0, false, false),
            new Data(0x80000000, 1, 0xC0000000, 0, false, false),
            new Data(0xC0000000, 31, 0xFFFFFFFF, 0, true, false),
            new Data(0x00000001, 1, 0x00000000, 0, true, false),
            new Data(0x3000, 12, 0x0003, 0, false, false),
            new Data(0x30000000, 28, 0x00000003, 0, false, false),
            new Data(0x30000000, 30, 0x00000000, 0, true, false),
            new Data(0x01020304, 0, 0x01020304, 0, false, false),
            new Data(0x01020304, 32, 0x01020304, 0, false, false),
    };

    private Data[] sard_1 = new Data[] {
            new Data(0x00804020, 1, 0x00402010, 0, false, false),
            new Data(0x80000000, 1, 0xC0000000, 0, false, false),
            new Data(0x00000001, 1, 0x00000000, 0, true, false),
    };

    private Data[] salc = new Data[] {
            new Data(10, 0, 0, 0, false, false),
            new Data(10, 0, 0xFF, CPU.CF, true, false)
    };

    private Data[] cmc = new Data[] {
            new Data(0, 0, 0, 0, true, false),
            new Data(0, 0, 0, CPU.CF, false, false)
    };

    private Data[] notb = new Data[] {
            new Data(0, 0, 0xFF, 0, false, false),
            new Data(0x0F, 0, 0xF0, 0, false, false),
            new Data(0xF0, 0, 0x0F, 0, false, false)
    };

    private Data[] notw = new Data[] {
            new Data(0, 0, 0xFFFF, 0, false, false),
            new Data(0xF0F, 0, 0xF0F0, 0, false, false),
            new Data(0xF0F0, 0, 0x0F0F, 0, false, false)
    };

    private Data[] notd = new Data[] {
            new Data(0, 0, 0xFFFFFFFF, 0, false, false),
            new Data(0x0F0F0F0F, 0, 0xF0F0F0F0, 0, false, false),
            new Data(0xF0F0F0F0, 0, 0x0F0F0F0F, 0, false, false)
    };

    private Data[] negb = new Data[] {
            new Data(0, 0, 0x0, 0, false, false),
            new Data(4, 0, ((byte)-4) & 0xFF, 0, true, false)
    };

    private Data[] negw = new Data[] {
            new Data(0, 0, 0x0, 0, false, false),
            new Data(2045, 0, ((short)-2045) & 0xFFFF, 0, true, false)
    };

    private Data[] negd = new Data[] {
            new Data(0, 0, 0x0, 0, false, false),
            new Data(20458512, 0, -20458512, 0, true, false)
    };

    private Data[] mulAl = new Data[] {
            new Data(2, 2, 4, 0, false, false),
            new Data(0, 0, 0, 0, false, false),
            new Data(0x20, 0x10, 0x200, 0, true, true),
    };

    private Data[] mulAx = new Data[] {
            new Data(2, 0, 4, 0, false, false, 2, 0),
            new Data(0, 0, 0, 0, false, false, 0, 0),
            new Data(0x2001, 0, 0x0010, 0, true, true, 0x10, 0x0002),
            new Data(0x2001, 0, 0x1000, 0, true, true, 0x1000, 0x0200),
    };

    private Data[] mulEax = new Data[] {
            new Data(2, 0, 4, 0, false, false, 2, 0),
            new Data(0, 0, 0, 0, false, false, 0, 0),
            new Data(0x20000001, 0, 0x00000010, 0, true, true, 0x10, 0x00000002),
            new Data(0x20000001, 0, 0x00010000, 0, true, true, 0x00010000, 0x00002000),
    };

    private Data[] imulAl = new Data[] {
            new Data(2, 2, 4, 0, false, false),
            new Data(0xFA, 2, 0xFFF4, 0, false, false), // -6 x 2 = -12
            new Data(0xFA, 0x9C, 600, 0, true, true), // -6 x -100 = 600
            new Data(0, 0xFF, 0, 0, false, false),
    };

    private Data[] imulAx = new Data[] {
            new Data(2, 0, 4, 0, false, false, 2, 0),
            new Data(0xFFFA, 0, 0xFFF4, 0, false, false, 2, 0xFFFF), // -6 x 2 = -12
            new Data(((short)-600) & 0xFFFF, 0, 0x5780, 0, true, true, 30000, 0xFEED), // -600 x 30000 = -18000000
            new Data(0xFFFA, 0, 600, 0, false, false, 0xFF9C, 0), // -6 x -100 = 600
    };

    private Data[] imulEax = new Data[] {
            new Data(2, 0, 4, 0, false, false, 2, 0),
            new Data(0xFFFFFFFA, 0, 0xFFFFFFF4, 0, false, false, 2, 0xFFFFFFFF), // -6 x 2 = -12
            new Data(-60000, 0, 0x1729f800, 0, true, true, 3000000, 0xFFFFFFD6), // -60000 x 3000000 = -180000000000
    };

    private Data[] divAl = new Data[] {
            new Data(10, 3, 0x0103, 0, false, false),
            new Data(1003, 200, 0x0305, 0, false, false),
    };

    private Data[] divAx = new Data[] {
            new Data(10, 0, 0x0003, 0, false, false, 3, 0x0001),
            new Data(0x8512, 0xCB, 4445, 0, false, false, 3000, 2874), // 13337874 / 3000 = 4445 r 2874
    };

    private Data[] divEax = new Data[] {
            new Data(10, 0, 0x0003, 0, false, false, 3, 0x0001),
            new Data(0x85121234, 0xCB, 0xB2D, 0, false, false, 0x12345678, 0x1227B71C), // 874110915124 / 305419896 = 2861 r 304592668
    };

    private Data[] idivAl = new Data[] {
            new Data(10, 3, 0x0103, 0, false, false),
            new Data(10, ((byte)-3) & 0xFF, 0x01FD, 0, false, false),
            new Data(((short)-1003) & 0xFFFF, ((byte)-100) & 0xFF, 0xFD0A, 0, false, false), // -3 rem, 10 quo
    };

    private Data[] idivAx = new Data[] {
            new Data(10, 0, 3, 0, false, false, 3, 1),
            new Data(0x8512, 0xCB, 4445, 0, false, false, 3000, 2874), // 13337874 / 3000 = 4445 r 2874
            new Data(0x7AEE, 0xFF34, ((short)-4445) & 0xFFFF, 0, false, false, 3000, ((short)-2874) & 0xFFFF), // -13337874 / 3000 = -4445 r -2874
    };

    private Data[] idivEax = new Data[] {
            new Data(10, 0, 3, 0, false, false, 3, 1),
            new Data(0x85121234, 0xCB, 0xB2D, 0, false, false, 0x12345678, 0x1227B71C), // 874110915124 / 305419896 = 2861 r 304592668
            new Data(0x7AEDEDCC, 0xFFFFFF34, 0xFFFFF4D3, 0, false, false, 0x12345678, 0xEDD848E4), // -874110915124 / 305419896 = -2861 r -304592668
    };

    private Data[] clc = new Data[] {
            new Data(0, 0, 0, 0, false, false),
            new Data(0, 0, 0, CPU.CF, false, false)
    };

    private Data[] stc = new Data[] {
            new Data(0, 0, 0, 0, true, false),
            new Data(0, 0, 0, CPU.CF, true, false)
    };

    public void testAdd0x000() {cpu.big = false;EbGb(0x00, addb);}
    public void testAdd0x200() {cpu.big = true;EbGb(0x00, addb);}
    public void testAdd0x001() {cpu.big = false;EwGw(0x01, addw);}
    public void testAdd0x201() {cpu.big = true;EdGd(0x01, addd);}
    public void testAdd0x002() {cpu.big = false;GbEb(0x02, addb);}
    public void testAdd0x202() {cpu.big = true;GbEb(0x02, addb);}
    public void testAdd0x003() {cpu.big = false;GwEw(0x03, addw);}
    public void testAdd0x203() {cpu.big = true;GdEd(0x03, addd);}
    public void testAdd0x004() {cpu.big = false;AlIb(0x04, addb);}
    public void testAdd0x204() {cpu.big = true;AlIb(0x04, addb);}
    public void testAdd0x005() {cpu.big = false;AxIw(0x05, addw);}
    public void testAdd0x205() {cpu.big = true;EaxId(0x05, addd);}

    public void testOr0x008() {cpu.big = false;EbGb(0x08, orb);}
    public void testOr0x208() {cpu.big = true;EbGb(0x08, orb);}
    public void testOr0x009() {cpu.big = false;EwGw(0x09, orw);}
    public void testOr0x209() {cpu.big = true;EdGd(0x09, ord);}
    public void testOr0x00a() {cpu.big = false;GbEb(0x0a, orb);}
    public void testOr0x20a() {cpu.big = true;GbEb(0x0a, orb);}
    public void testOr0x00b() {cpu.big = false;GwEw(0x0b, orw);}
    public void testOr0x20b() {cpu.big = true;GdEd(0x0b, ord);}
    public void testOr0x00c() {cpu.big = false;AlIb(0x0c, orb);}
    public void testOr0x20c() {cpu.big = true;AlIb(0x0c, orb);}
    public void testOr0x00d() {cpu.big = false;AxIw(0x0d, orw);}
    public void testOr0x20d() {cpu.big = true;EaxId(0x0d, ord);}

    public void testAdc0x010() {cpu.big = false;EbGb(0x10, adcb);}
    public void testAdc0x210() {cpu.big = true;EbGb(0x10, addb);}
    public void testAdc0x011() {cpu.big = false;EwGw(0x11, adcw);}
    public void testAdc0x211() {cpu.big = true;EdGd(0x11, adcd);}
    public void testAdc0x012() {cpu.big = false;GbEb(0x12, adcb);}
    public void testAdc0x212() {cpu.big = true;GbEb(0x12, adcb);}
    public void testAdc0x013() {cpu.big = false;GwEw(0x13, adcw);}
    public void testAdc0x213() {cpu.big = true;GdEd(0x13, adcd);}
    public void testAdc0x014() {cpu.big = false;AlIb(0x14, adcb);}
    public void testAdc0x214() {cpu.big = true;AlIb(0x14, adcb);}
    public void testAdc0x015() {cpu.big = false;AxIw(0x15, adcw);}
    public void testAdc0x215() {cpu.big = true;EaxId(0x15, adcd);}

    public void testSbb0x018() {cpu.big = false;EbGb(0x18, sbbb);}
    public void testSbb0x218() {cpu.big = true;EbGb(0x18, sbbb);}
    public void testSbb0x019() {cpu.big = false;EwGw(0x19, sbbw);}
    public void testSbb0x219() {cpu.big = true;EdGd(0x19, sbbd);}
    public void testSbb0x01a() {cpu.big = false;GbEb(0x1a, sbbb);}
    public void testSbb0x21a() {cpu.big = true;GbEb(0x1a, sbbb);}
    public void testSbb0x01b() {cpu.big = false;GwEw(0x1b, sbbw);}
    public void testSbb0x21b() {cpu.big = true;GdEd(0x1b, sbbd);}
    public void testSbb0x01c() {cpu.big = false;AlIb(0x1c, sbbb);}
    public void testSbb0x21c() {cpu.big = true;AlIb(0x1c, sbbb);}
    public void testSbb0x01d() {cpu.big = false;AxIw(0x1d, sbbw);}
    public void testSbb0x21d() {cpu.big = true;EaxId(0x1d, sbbd);}

    public void testAnd0x020() {cpu.big = false;EbGb(0x20, andb);}
    public void testAnd0x220() {cpu.big = true;EbGb(0x20, andb);}
    public void testAnd0x021() {cpu.big = false;EwGw(0x21, andw);}
    public void testAnd0x221() {cpu.big = true;EdGd(0x21, andd);}
    public void testAnd0x022() {cpu.big = false;GbEb(0x22, andb);}
    public void testAnd0x222() {cpu.big = true;GbEb(0x22, andb);}
    public void testAnd0x023() {cpu.big = false;GwEw(0x23, andw);}
    public void testAnd0x223() {cpu.big = true;GdEd(0x23, andd);}
    public void testAnd0x024() {cpu.big = false;AlIb(0x24, andb);}
    public void testAnd0x224() {cpu.big = true;AlIb(0x24, andb);}
    public void testAnd0x025() {cpu.big = false;AxIw(0x25, andw);}
    public void testAnd0x225() {cpu.big = true;EaxId(0x25, andd);}

    public void testSub0x028() {cpu.big = false;EbGb(0x28, subb);}
    public void testSub0x228() {cpu.big = true;EbGb(0x28, subb);}
    public void testSub0x029() {cpu.big = false;EwGw(0x29, subw);}
    public void testSub0x229() {cpu.big = true;EdGd(0x29, subd);}
    public void testSub0x02a() {cpu.big = false;GbEb(0x2a, subb);}
    public void testSub0x22a() {cpu.big = true;GbEb(0x2a, subb);}
    public void testSub0x02b() {cpu.big = false;GwEw(0x2b, subw);}
    public void testSub0x22b() {cpu.big = true;GdEd(0x2b, subd);}
    public void testSub0x02c() {cpu.big = false;AlIb(0x2c, subb);}
    public void testSub0x22c() {cpu.big = true;AlIb(0x2c, subb);}
    public void testSub0x02d() {cpu.big = false;AxIw(0x2d, subw);}
    public void testSub0x22d() {cpu.big = true;EaxId(0x2d, subd);}

    public void testXor0x030() {cpu.big = false;EbGb(0x30, xorb);}
    public void testXor0x230() {cpu.big = true;EbGb(0x30, xorb);}
    public void testXor0x031() {cpu.big = false;EwGw(0x31, xorw);}
    public void testXor0x231() {cpu.big = true;EdGd(0x31, xord);}
    public void testXor0x032() {cpu.big = false;GbEb(0x32, xorb);}
    public void testXor0x232() {cpu.big = true;GbEb(0x32, xorb);}
    public void testXor0x033() {cpu.big = false;GwEw(0x33, xorw);}
    public void testXor0x233() {cpu.big = true;GdEd(0x33, xord);}
    public void testXor0x034() {cpu.big = false;AlIb(0x34, xorb);}
    public void testXor0x234() {cpu.big = true;AlIb(0x34, xorb);}
    public void testXor0x035() {cpu.big = false;AxIw(0x35, xorw);}
    public void testXor0x235() {cpu.big = true;EaxId(0x35, xord);}

    public void testCmp0x038() {cpu.big = false;EbGb(0x38, cmpb);}
    public void testCmp0x238() {cpu.big = true;EbGb(0x38, cmpb);}
    public void testCmp0x039() {cpu.big = false;EwGw(0x39, cmpw);}
    public void testCmp0x239() {cpu.big = true;EdGd(0x39, cmpd);}
    public void testCmp0x03a() {cpu.big = false;GbEb(0x3a, cmpb);}
    public void testCmp0x23a() {cpu.big = true;GbEb(0x3a, cmpb);}
    public void testCmp0x03b() {cpu.big = false;GwEw(0x3b, cmpw);}
    public void testCmp0x23b() {cpu.big = true;GdEd(0x3b, cmpd);}
    public void testCmp0x03c() {cpu.big = false;AlIb(0x3c, cmpb);}
    public void testCmp0x23c() {cpu.big = true;AlIb(0x3c, cmpb);}
    public void testCmp0x03d() {cpu.big = false;AxIw(0x3d, cmpw);}
    public void testCmp0x23d() {cpu.big = true;EaxId(0x3d, cmpd);}

    public void testIncAx0x040() {cpu.big = false;Ew(0x40, cpu.eax, incw);}
    public void testIncEax0x240() {cpu.big = true;Ed(0x40, cpu.eax, incd);}
    public void testIncCx0x041() {cpu.big = false;Ew(0x41, cpu.ecx, incw);}
    public void testIncEcx0x241() {cpu.big = true;Ed(0x41, cpu.ecx, incd);}
    public void testIncDx0x042() {cpu.big = false;Ew(0x42, cpu.edx, incw);}
    public void testIncEdx0x242() {cpu.big = true;Ed(0x42, cpu.edx, incd);}
    public void testIncBx0x043() {cpu.big = false;Ew(0x43, cpu.ebx, incw);}
    public void testIncEbx0x243() {cpu.big = true;Ed(0x43, cpu.ebx, incd);}
    public void testIncSp0x044() {cpu.big = false;Ew(0x44, cpu.esp, incw);}
    public void testIncEsp0x244() {cpu.big = true;Ed(0x44, cpu.esp, incd);}
    public void testIncBp0x045() {cpu.big = false;Ew(0x45, cpu.ebp, incw);}
    public void testIncEbp0x245() {cpu.big = true;Ed(0x45, cpu.ebp, incd);}
    public void testIncSi0x046() {cpu.big = false;Ew(0x46, cpu.esi, incw);}
    public void testIncEsi0x246() {cpu.big = true;Ed(0x46, cpu.esi, incd);}
    public void testIncDi0x047() {cpu.big = false;Ew(0x47, cpu.edi, incw);}
    public void testIncEdi0x247() {cpu.big = true;Ed(0x47, cpu.edi, incd);}

    public void testDecAx0x048() {cpu.big = false;Ew(0x48, cpu.eax, decw);}
    public void testDecEax0x248() {cpu.big = true;Ed(0x48, cpu.eax, decd);}
    public void testDecCx0x049() {cpu.big = false;Ew(0x49, cpu.ecx, decw);}
    public void testDecEcx0x249() {cpu.big = true;Ed(0x49, cpu.ecx, decd);}
    public void testDecDx0x04a() {cpu.big = false;Ew(0x4a, cpu.edx, decw);}
    public void testDecEdx0x24a() {cpu.big = true;Ed(0x4a, cpu.edx, decd);}
    public void testDecBx0x04b() {cpu.big = false;Ew(0x4b, cpu.ebx, decw);}
    public void testDecEbx0x24b() {cpu.big = true;Ed(0x4b, cpu.ebx, decd);}
    public void testDecSp0x04c() {cpu.big = false;Ew(0x4c, cpu.esp, decw);}
    public void testDecEsp0x24c() {cpu.big = true;Ed(0x4c, cpu.esp, decd);}
    public void testDecBp0x04d() {cpu.big = false;Ew(0x4d, cpu.ebp, decw);}
    public void testDecEbp0x24d() {cpu.big = true;Ed(0x4d, cpu.ebp, decd);}
    public void testDecSi0x04e() {cpu.big = false;Ew(0x4e, cpu.esi, decw);}
    public void testDecEsi0x24e() {cpu.big = true;Ed(0x4e, cpu.esi, decd);}
    public void testDecDi0x04f() {cpu.big = false;Ew(0x4f, cpu.edi, decw);}
    public void testDecEdi0x24f() {cpu.big = true;Ed(0x4f, cpu.edi, decd);}

    public void testPushAx0x050() {cpu.big = false;Push16(0x50, cpu.eax);}
    public void testPushEax0x250() {cpu.big = true;Push32(0x50, cpu.eax);}
    public void testPushCx0x051() {cpu.big = false;Push16(0x51, cpu.ecx);}
    public void testPushEcx0x251() {cpu.big = true;Push32(0x51, cpu.ecx);}
    public void testPushDx0x052() {cpu.big = false;Push16(0x52, cpu.edx);}
    public void testPushEdx0x252() {cpu.big = true;Push32(0x52, cpu.edx);}
    public void testPushBx0x053() {cpu.big = false;Push16(0x53, cpu.ebx);}
    public void testPushEbx0x253() {cpu.big = true;Push32(0x53, cpu.ebx);}
    //public void testPushSp0x054() {cpu.big = false;Push16(0x54, cpu.esp);}
    //public void testPushEsp0x254() {cpu.big = true;Push32(0x54, cpu.esp);}
    public void testPushBp0x055() {cpu.big = false;Push16(0x55, cpu.ebp);}
    public void testPushEbp0x255() {cpu.big = true;Push32(0x55, cpu.ebp);}
    public void testPushSi0x056() {cpu.big = false;Push16(0x56, cpu.esi);}
    public void testPushEsi0x256() {cpu.big = true;Push32(0x56, cpu.esi);}
    public void testPushDi0x057() {cpu.big = false;Push16(0x57, cpu.edi);}
    public void testPushEdi0x257() {cpu.big = true;Push32(0x57, cpu.edi);}

    public void testPopAx0x058() {cpu.big = false;Pop16(0x58, cpu.eax);}
    public void testPopEax0x258() {cpu.big = true;Pop32(0x58, cpu.eax);}
    public void testPopCx0x059() {cpu.big = false;Pop16(0x59, cpu.ecx);}
    public void testPopEcx0x259() {cpu.big = true;Pop32(0x59, cpu.ecx);}
    public void testPopDx0x05a() {cpu.big = false;Pop16(0x5a, cpu.edx);}
    public void testPopEdx0x25a() {cpu.big = true;Pop32(0x5a, cpu.edx);}
    public void testPopBx0x05b() {cpu.big = false;Pop16(0x5b, cpu.ebx);}
    public void testPopEbx0x25b() {cpu.big = true;Pop32(0x5b, cpu.ebx);}
    //public void testPopSp0x05c() {cpu.big = false;Pop16(0x5c, cpu.esp);}
    //public void testPopEsp0x25c() {cpu.big = true;Pop32(0x5c, cpu.esp);}
    public void testPopBp0x05d() {cpu.big = false;Pop16(0x5d, cpu.ebp);}
    public void testPopEbp0x25d() {cpu.big = true;Pop32(0x5d, cpu.ebp);}
    public void testPopSi0x05e() {cpu.big = false;Pop16(0x5e, cpu.esi);}
    public void testPopEsi0x25e() {cpu.big = true;Pop32(0x5e, cpu.esi);}
    public void testPopDi0x05f() {cpu.big = false;Pop16(0x5f, cpu.edi);}
    public void testPopEdi0x25f() {cpu.big = true;Pop32(0x5f, cpu.edi);}

    public void testPush0x068() {cpu.big = false;Push16(0x68);}
    public void testPush0x268() {cpu.big = true;Push32(0x68);}

    public void testIMul0x069() {cpu.big = false;GwEw(0x69, imulw);}
    public void testIMul0x269() {cpu.big = true;GdEd(0x69, imuld);}

    public void testPush0x06a() {cpu.big = false;Push16s8(0x6a);}
    public void testPush0x26a() {cpu.big = true;Push32s8(0x6a);}

    public void testIMul0x06b() {cpu.big = false;GwEw(0x6b, imulw_s8);}
    public void testIMul0x26b() {cpu.big = true;GdEd(0x6b, imuld_s8);}

    public void testGrp10x080() {
        cpu.big = false;
        EbIb(0x80, 0, addb);
        EbIb(0x80, 1, orb);
        EbIb(0x80, 2, adcb);
        EbIb(0x80, 3, sbbb);
        EbIb(0x80, 4, andb);
        EbIb(0x80, 5, subb);
        EbIb(0x80, 6, xorb);
        EbIb(0x80, 7, cmpb);
    }

    public void testGrp10x280() {
        cpu.big = true;
        EbIb(0x80, 0, addb);
        EbIb(0x80, 1, orb);
        EbIb(0x80, 2, adcb);
        EbIb(0x80, 3, sbbb);
        EbIb(0x80, 4, andb);
        EbIb(0x80, 5, subb);
        EbIb(0x80, 6, xorb);
        EbIb(0x80, 7, cmpb);
    }

    public void testGrp10x081() {
        cpu.big = false;
        EwIw(0x81, 0, addw);
        EwIw(0x81, 1, orw);
        EwIw(0x81, 2, adcw);
        EwIw(0x81, 3, sbbw);
        EwIw(0x81, 4, andw);
        EwIw(0x81, 5, subw);
        EwIw(0x81, 6, xorw);
        EwIw(0x81, 7, cmpw);
    }

    public void testGrp10x281() {
        cpu.big = true;
        EdId(0x81, 0, addd);
        EdId(0x81, 1, ord);
        EdId(0x81, 2, adcd);
        EdId(0x81, 3, sbbd);
        EdId(0x81, 4, andd);
        EdId(0x81, 5, subd);
        EdId(0x81, 6, xord);
        EdId(0x81, 7, cmpd);
    }

    public void testGrp10x083() {
        cpu.big = false;
        EwIx(0x83, 0, addw);
        EwIx(0x83, 1, orw);
        EwIx(0x83, 2, adcw);
        EwIx(0x83, 3, sbbw);
        EwIx(0x83, 4, andw);
        EwIx(0x83, 5, subw);
        EwIx(0x83, 6, xorw);
        EwIx(0x83, 7, cmpw);
    }

    public void testGrp10x283() {
        cpu.big = true;
        EdIx(0x83, 0, addd);
        EdIx(0x83, 1, ord);
        EdIx(0x83, 2, adcd);
        EdIx(0x83, 3, sbbd);
        EdIx(0x83, 4, andd);
        EdIx(0x83, 5, subd);
        EdIx(0x83, 6, xord);
        EdIx(0x83, 7, cmpd);
    }

    public void testTest0x084() {cpu.big = false;EbGb(0x84, testb);}
    public void testTest0x284() {cpu.big = true;EbGb(0x84, testb);}
    public void testTest0x085() {cpu.big = false;EwGw(0x85, testw);}
    public void testTest0x285() {cpu.big = true;EdGd(0x85, testd);}

    public void testXchg0x086() {cpu.big = false;EbGb(0x86, xchgb);}
    public void testXchg0x286() {cpu.big = true;EbGb(0x86, xchgb);}
    public void testXchg0x087() {cpu.big = false;EwGw(0x87, xchgw);}
    public void testXchg0x287() {cpu.big = true;EdGd(0x87, xchgd);}

    public void testMovEbGb0x088() {cpu.big = false;EbGb(0x88, movb);}
    public void testMovEbGb0x288() {cpu.big = true;EbGb(0x88, movb);}
    public void testMovEwGw0x089() {cpu.big = false;EwGw(0x89, movw);}
    public void testMovEdGd0x289() {cpu.big = true;EdGd(0x89, movd);}

    public void testMovEbGb0x08a() {cpu.big = false;GbEb(0x8a, movb);}
    public void testMovEbGb0x28a() {cpu.big = true;GbEb(0x8a, movb);}
    public void testMovEwGw0x08b() {cpu.big = false;GwEw(0x8b, movw);}
    public void testMovEdGd0x28b() {cpu.big = true;GdEd(0x8b, movd);}

    public void testMovEwWw0x08c() {cpu.big = false;EwSw(0x8c, movw);}
    public void testMovEwWw0x28c() {cpu.big = true;EwSw(0x8c, movw);}

    public void testLeaGw0x08d() {cpu.big = false;LeaGw();}
    public void testLeaGd0x28d() {cpu.big = true;LeaGd();}

    // :TODO: 0x08e
    // :TODO: 0x28e

    public void testPopEw0x08f() {cpu.big = false;PopEw();}
    public void testPopEd0x28f() {cpu.big = true;PopEd();}

    public void testXchgCxAx0x091() {cpu.big = false;Reg16Reg16(0x91, xchgw, cpu.eax, cpu.ecx);}
    public void testXchgEcxEax0x291() {cpu.big = true;Reg32Reg32(0x91, xchgd, cpu.eax, cpu.ecx);}
    public void testXchgDxAx0x092() {cpu.big = false;Reg16Reg16(0x92, xchgw, cpu.eax, cpu.edx);}
    public void testXchgEdxEax0x292() {cpu.big = true;Reg32Reg32(0x92, xchgd, cpu.eax, cpu.edx);}
    public void testXchgBxAx0x093() {cpu.big = false;Reg16Reg16(0x93, xchgw, cpu.eax, cpu.ebx);}
    public void testXchgEbxEax0x293() {cpu.big = true;Reg32Reg32(0x93, xchgd, cpu.eax, cpu.ebx);}
    public void testXchgSpAx0x094() {cpu.big = false;Reg16Reg16(0x94, xchgw, cpu.eax, cpu.esp);}
    public void testXchgEspEax0x294() {cpu.big = true;Reg32Reg32(0x94, xchgd, cpu.eax, cpu.esp);}
    public void testXchgBpAx0x095() {cpu.big = false;Reg16Reg16(0x95, xchgw, cpu.eax, cpu.ebp);}
    public void testXchgEbpEax0x295() {cpu.big = true;Reg32Reg32(0x95, xchgd, cpu.eax, cpu.ebp);}
    public void testXchgSiAx0x096() {cpu.big = false;Reg16Reg16(0x96, xchgw, cpu.eax, cpu.esi);}
    public void testXchgEsiEax0x296() {cpu.big = true;Reg32Reg32(0x96, xchgd, cpu.eax, cpu.esi);}
    public void testXchgDiAx0x097() {cpu.big = false;Reg16Reg16(0x97, xchgw, cpu.eax, cpu.edi);}
    public void testXchgEdiEax0x297() {cpu.big = true;Reg32Reg32(0x97, xchgd, cpu.eax, cpu.edi);}

    public void testCbw0x098() {cpu.big = false;Ew(0x98, cpu.eax, cbw);}
    public void testCwde0x298() {cpu.big = true;Ed(0x298, cpu.eax, cwde);}
    public void testCwd0x099() {cpu.big = false;Reg16Reg16(0x99, cwd, cpu.eax, cpu.edx);}
    public void testCdq0x299() {cpu.big = true;Reg32Reg32(0x99, cdq, cpu.eax, cpu.edx);}

    public void testPushf0x09c() {cpu.big = false;Pushf(0x9c);}
    public void testPushf0x29c() {cpu.big = true;Pushfd(0x9c);}
    public void testPopf0x09d() {cpu.big = false;Popf(0x9d);}
    public void testPopf0x29d() {cpu.big = true;Popfd(0x9d);}

    public void testSahf0x09e() {cpu.big = false;flags(0x9e, sahf, cpu.eax);}
    public void testSahf0x29e() {cpu.big = true;flags(0x9e, sahf, cpu.eax);}
    public void testLahf0x09f() {cpu.big = false;flags(0x9f, lahf, cpu.eax);}
    public void testLahf0x29f() {cpu.big = true;flags(0x9f, lahf, cpu.eax);}

    // :TODO: 0xa0 - 0xaf

    public void testMovAlIb0x0b0() {cpu.big = false;EbIb(0xb0, cpu.eax, movb);}
    public void testMovAlIb0x2b0() {cpu.big = true;EbIb(0xb0, cpu.eax, movb);}
    public void testMovClIb0x0b1() {cpu.big = false;EbIb(0xb1, cpu.ecx, movb);}
    public void testMovClIb0x2b1() {cpu.big = true;EbIb(0xb1, cpu.ecx, movb);}
    public void testMovDlIb0x0b2() {cpu.big = false;EbIb(0xb2, cpu.edx, movb);}
    public void testMovDlIb0x2b2() {cpu.big = true;EbIb(0xb2, cpu.edx, movb);}
    public void testMovBlIb0x0b3() {cpu.big = false;EbIb(0xb3, cpu.ebx, movb);}
    public void testMovBlIb0x2b3() {cpu.big = true;EbIb(0xb3, cpu.ebx, movb);}
    public void testMovAhIb0x0b4() {cpu.big = false;EbIb(0xb4, cpu.ah, movb);}
    public void testMovAhIb0x2b4() {cpu.big = true;EbIb(0xb4, cpu.ah, movb);}
    public void testMovChIb0x0b5() {cpu.big = false;EbIb(0xb5, cpu.ch, movb);}
    public void testMovChIb0x2b5() {cpu.big = true;EbIb(0xb5, cpu.ch, movb);}
    public void testMovDhIb0x0b6() {cpu.big = false;EbIb(0xb6, cpu.dh, movb);}
    public void testMovDhIb0x2b6() {cpu.big = true;EbIb(0xb6, cpu.dh, movb);}
    public void testMovBhIb0x0b7() {cpu.big = false;EbIb(0xb7, cpu.bh, movb);}
    public void testMovBhIb0x2b7() {cpu.big = true;EbIb(0xb7, cpu.bh, movb);}
    public void testMovAxIw0x0b8() {cpu.big = false;EwIw(0xb8, cpu.eax, movw);}
    public void testMovEaxId0x2b8() {cpu.big = true;EdId(0xb8, cpu.eax, movd);}
    public void testMovCxIw0x0b9() {cpu.big = false;EwIw(0xb9, cpu.ecx, movw);}
    public void testMovEcxId0x2b9() {cpu.big = true;EdId(0xb9, cpu.ecx, movd);}
    public void testMovDxIw0x0ba() {cpu.big = false;EwIw(0xba, cpu.edx, movw);}
    public void testMovEdxId0x2ba() {cpu.big = true;EdId(0xba, cpu.edx, movd);}
    public void testMovBxIw0x0bb() {cpu.big = false;EwIw(0xbb, cpu.ebx, movw);}
    public void testMovEbxId0x2bb() {cpu.big = true;EdId(0xbb, cpu.ebx, movd);}
    public void testMovSpIw0x0bc() {cpu.big = false;EwIw(0xbc, cpu.esp, movw);}
    public void testMovEspId0x2bc() {cpu.big = true;EdId(0xbc, cpu.esp, movd);}
    public void testMovBpIw0x0bd() {cpu.big = false;EwIw(0xbd, cpu.ebp, movw);}
    public void testMovEbpId0x2bd() {cpu.big = true;EdId(0xbd, cpu.ebp, movd);}
    public void testMovSiIw0x0be() {cpu.big = false;EwIw(0xbe, cpu.esi, movw);}
    public void testMovEsiId0x2be() {cpu.big = true;EdId(0xbe, cpu.esi, movd);}
    public void testMovDiIw0x0bf() {cpu.big = false;EwIw(0xbf, cpu.edi, movw);}
    public void testMovEdiId0x2bf() {cpu.big = true;EdId(0xbf, cpu.edi, movd);}

    public void testGrp20x0c0() {
        cpu.big = false;
        EbIb(0xC0, 0, rolb);
        EbIb(0xC0, 1, rorb);
        EbIb(0xC0, 2, rclb);
        EbIb(0xC0, 3, rcrb);
        EbIb(0xC0, 4, shlb);
        EbIb(0xC0, 5, shrb);
        EbIb(0xC0, 6, shlb);
        EbIb(0xC0, 7, sarb);
    }

    public void testGrp20x2c0() {
        cpu.big = true;
        EbIb(0xC0, 0, rolb);
        EbIb(0xC0, 1, rorb);
        EbIb(0xC0, 2, rclb);
        EbIb(0xC0, 3, rcrb);
        EbIb(0xC0, 4, shlb);
        EbIb(0xC0, 5, shrb);
        EbIb(0xC0, 6, shlb);
        EbIb(0xC0, 7, sarb);
    }

    public void testGrp20x0c1() {
        cpu.big = false;
        EwIb(0xC1, 0, rolw);
        EwIb(0xC1, 1, rorw);
        EwIb(0xC1, 2, rclw);
        EwIb(0xC1, 3, rcrw);
        EwIb(0xC1, 4, shlw);
        EwIb(0xC1, 5, shrw);
        EwIb(0xC1, 6, shlw);
        EwIb(0xC1, 7, sarw);
    }

    public void testGrp20x2c1() {
        cpu.big = true;
        EdIb(0xC1, 0, rold);
        EdIb(0xC1, 1, rord);
        EdIb(0xC1, 2, rcld);
        EdIb(0xC1, 3, rcrd);
        EdIb(0xC1, 4, shld);
        EdIb(0xC1, 5, shrd);
        EdIb(0xC1, 6, shld);
        EdIb(0xC1, 7, sard);
    }

    public void testMovEbIb0x0c6() {cpu.big = false;EbIb(0xc6, 0, movb);}
    public void testMovEbIb0x2c6() {cpu.big = true;EbIb(0xc6, 0, movb);}
    public void testMovEwIw0x0c7() {cpu.big = false;EwIw(0xc7, 0, movw);}
    public void testMovEdId0x2c7() {cpu.big = true;EdId(0xc7, 0, movd);}

    public void testGrp20x0d0() {
        cpu.big = false;
        Eb(0xD0, 0, rolb_1);
        Eb(0xD0, 1, rorb_1);
        Eb(0xD0, 2, rclb_1);
        Eb(0xD0, 3, rcrb_1);
        Eb(0xD0, 4, shlb_1);
        Eb(0xD0, 5, shrb_1);
        Eb(0xD0, 6, shlb_1);
        Eb(0xD0, 7, sarb_1);
    }

    public void testGrp20x2d0() {
        cpu.big = true;
        Eb(0xD0, 0, rolb_1);
        Eb(0xD0, 1, rorb_1);
        Eb(0xD0, 2, rclb_1);
        Eb(0xD0, 3, rcrb_1);
        Eb(0xD0, 4, shlb_1);
        Eb(0xD0, 5, shrb_1);
        Eb(0xD0, 6, shlb_1);
        Eb(0xD0, 7, sarb_1);
    }

    public void testGrp20x0d1() {
        cpu.big = false;
        Ew(0xD1, 0, rolw_1);
        Ew(0xD1, 1, rorw_1);
        Ew(0xD1, 2, rclw_1);
        Ew(0xD1, 3, rcrw_1);
        Ew(0xD1, 4, shlw_1);
        Ew(0xD1, 5, shrw_1);
        Ew(0xD1, 6, shlw_1);
        Ew(0xD1, 7, sarw_1);
    }

    public void testGrp20x2d1() {
        cpu.big = true;
        Ed(0xD1, 0, rold_1);
        Ed(0xD1, 1, rord_1);
        Ed(0xD1, 2, rcld_1);
        Ed(0xD1, 3, rcrd_1);
        Ed(0xD1, 4, shld_1);
        Ed(0xD1, 5, shrd_1);
        Ed(0xD1, 6, shld_1);
        Ed(0xD1, 7, sard_1);
    }

    public void testGrp20x0d2() {
        cpu.big = false;
        EbCl(0xD2, 0, rolb);
        EbCl(0xD2, 1, rorb);
        EbCl(0xD2, 2, rclb);
        EbCl(0xD2, 3, rcrb);
        EbCl(0xD2, 4, shlb);
        EbCl(0xD2, 5, shrb);
        EbCl(0xD2, 6, shlb);
        EbCl(0xD2, 7, sarb);
    }

    public void testGrp20x2d2() {
        cpu.big = true;
        EbCl(0xD2, 0, rolb);
        EbCl(0xD2, 1, rorb);
        EbCl(0xD2, 2, rclb);
        EbCl(0xD2, 3, rcrb);
        EbCl(0xD2, 4, shlb);
        EbCl(0xD2, 5, shrb);
        EbCl(0xD2, 6, shlb);
        EbCl(0xD2, 7, sarb);
    }

    public void testGrp20x0d3() {
        cpu.big = false;
        EwCl(0xD3, 0, rolw);
        EwCl(0xD3, 1, rorw);
        EwCl(0xD3, 2, rclw);
        EwCl(0xD3, 3, rcrw);
        EwCl(0xD3, 4, shlw);
        EwCl(0xD3, 5, shrw);
        EwCl(0xD3, 6, shlw);
        EwCl(0xD3, 7, sarw);
    }

    public void testGrp20x2d3() {
        cpu.big = true;
        EdCl(0xD3, 0, rold);
        EdCl(0xD3, 1, rord);
        EdCl(0xD3, 2, rcld);
        EdCl(0xD3, 3, rcrd);
        EdCl(0xD3, 4, shld);
        EdCl(0xD3, 5, shrd);
        EdCl(0xD3, 6, shld);
        EdCl(0xD3, 7, sard);
    }

    public void testSalc0x0d6() {cpu.big=false; Eb(0xd6, cpu.eax, salc);}
    public void testSalc0x2d6() {cpu.big=true; Eb(0xd6, cpu.eax, salc);}

    public void testXlat0x0d7() {
        cpu.big=false;
        newInstruction(0xd7, 0);
        cpu.ebx.dword = DEFAULT;
        cpu.eax.dword = DEFAULT;
        memory.writed(cpu.ds.dword + 2000, DEFAULT);
        memory.writed(cpu.ds.dword + 2004, DEFAULT);
        memory.writed(cpu.ds.dword + 2008, DEFAULT);
        cpu.ebx.u16(2000);
        cpu.eax.u8(4);
        memory.writeb(cpu.ds.dword + 2004, 0x08);
        runCPU();
        int result = DEFAULT;
        result&=~0xFF;
        result+=8;
        assertTrue(cpu.eax.dword == result);
    }

    public void testXlat0x2d7() {
        cpu.big=true;
        newInstruction(0xd7, 0);
        cpu.eax.dword = DEFAULT;
        memory.writed(cpu.ds.dword + 2000, DEFAULT);
        memory.writed(cpu.ds.dword + 2004, DEFAULT);
        memory.writed(cpu.ds.dword + 2008, DEFAULT);
        cpu.ebx.dword=2000;
        cpu.eax.u8(4);
        memory.writeb(cpu.ds.dword + 2004, 0x08);
        runCPU();
        int result = DEFAULT;
        result&=~0xFF;
        result+=8;
        assertTrue(cpu.eax.dword == result);
    }

    public void testCmc0x0f5() {cpu.big=false;Eb(0xf5, cpu.eax, cmc);}
    public void testCmc0x2f5() {cpu.big=true;Eb(0xf5, cpu.eax, cmc);}

    public void testGrp30x0f6() {
        cpu.big = false;
        EbIb(0xf6, 0, testb);
        EbIb(0xf6, 1, testb);
        Eb(0xf6, 2, notb);
        Eb(0xf6, 3, negb);
        EbAlAx(0xf6, 4, mulAl, false);
        EbAlAx(0xf6, 5, imulAl, false);
        EbAlAx(0xf6, 6, divAl, true);
        EbAlAx(0xf6, 7, idivAl, true);
    }

    public void testGrp30x2f6() {
        cpu.big = true;
        EbIb(0xf6, 0, testb);
        EbIb(0xf6, 1, testb);
        Eb(0xf6, 2, notb);
        Eb(0xf6, 3, negb);
        EbAlAx(0xf6, 4, mulAl, false);
        EbAlAx(0xf6, 5, imulAl, false);
        EbAlAx(0xf6, 6, divAl, true);
        EbAlAx(0xf6, 7, idivAl, true);
    }

    public void testGrp30x0f7() {
        cpu.big = false;
        EwIw(0xf7, 0, testw);
        EwIw(0xf7, 1, testw);
        Ew(0xf7, 2, notw);
        Ew(0xf7, 3, negw);
        EwAxDx(0xf7, 4, mulAx, false);
        EwAxDx(0xf7, 5, imulAx, false);
        EwAxDx(0xf7, 6, divAx, true);
        EwAxDx(0xf7, 7, idivAx, true);
    }

    public void testGrp30x2f7() {
        cpu.big = true;
        EdId(0xf7, 0, testd);
        EdId(0xf7, 1, testd);
        Ed(0xf7, 2, notd);
        Ed(0xf7, 3, negd);
        EdEaxEdx(0xf7, 4, mulEax, false);
        EdEaxEdx(0xf7, 5, imulEax, false);
        EdEaxEdx(0xf7, 6, divEax, true);
        EdEaxEdx(0xf7, 7, idivEax, true);
    }

    public void testClc0x0f8() {cpu.big=false;Eb(0xf8, cpu.eax, clc);}
    public void testClc0x2f8() {cpu.big=true;Eb(0xf8, cpu.eax, clc);}
    public void testStc0x0f8() {cpu.big=false;Eb(0xf9, cpu.eax, stc);}
    public void testStc0x2f8() {cpu.big=true;Eb(0xf9, cpu.eax, stc);}
}

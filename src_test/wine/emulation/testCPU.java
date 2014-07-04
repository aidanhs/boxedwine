package wine.emulation;

import junit.framework.TestCase;

public class testCPU extends TestCase {
    static private CPU cpu;
    static private Memory memory;
    private int cseip;

    private static final int STACK_ADDRESS=0xE0001000;
    private static final int HEAP_ADDRESS =0xF0000000;
    private static final int CODE_ADDRESS =0xD0000000;

    protected void setUp() throws java.lang.Exception {
        if (memory==null) {
            memory = new Memory();
            memory.init();
            this.cpu = new CPU(memory);
            RAM.init(4096 * 4);
            memory.handlers[(STACK_ADDRESS>>>12)-1] = new RAMHandler(memory, RAM.allocPage(), false, false);
            memory.handlers[HEAP_ADDRESS>>>12] = new RAMHandler(memory, RAM.allocPage(), false, false);
            memory.handlers[CODE_ADDRESS>>>12] = new RAMHandler(memory, RAM.allocPage(), false, false);
            cpu.cs.dword = CODE_ADDRESS;
            cpu.ds.dword = HEAP_ADDRESS;
            cpu.ss.dword = STACK_ADDRESS-4096;
        }
        super.setUp();
    }

    protected void tearDown() throws java.lang.Exception {
        super.tearDown();
    }

    private void pushCode8(int value) {
        memory.writeb(cseip, value);
        cseip++;
    }

    private void pushCode16(int value) {
        memory.writew(cseip, value);
        cseip+=2;
    }

    private void pushCode32(int value) {
        memory.writed(cseip, value);
        cseip+=4;
    }

    private void newInstruction(int instruction, int rm, int flags) {
        newInstruction(instruction, flags);
        pushCode8(rm);
    }
    private void newInstruction(int instruction, int flags) {
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

    static private class NullOp extends Op {
        public Block call(CPU cpu) {
            return null;
        }
    }
    static private NullOp nullOp = new NullOp();

    private void runCPU() {
        pushCode8(0x70); // jump causes the decoder to stop building the block
        pushCode8(0);
        Block block = cpu.getBlock(cpu.eip, cpu.cs.dword);
        block.op.next = nullOp;
        block.op.call(cpu);
    }

    private void EbGb(int instruction, Data[] data) {
        for (Data d : data) {
            for (int eb = 0; eb < 8; eb++) {
                for (int gb = 0; gb < 8; gb++) {
                    if (eb == gb)
                        continue;
                    int rm = eb | (gb << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = Decoder.eb(cpu, rm);
                    Reg g = Decoder.gb(cpu, rm);
                    if (e.parent != null)
                        e.parent.dword = 0xABCDEF01;
                    else
                        e.dword = 0xABCDEF01;
                    if (g.parent != null)
                        g.parent.dword = 0xABCDEF01;
                    else
                        g.dword = 0xABCDEF01;
                    e.u8(d.left);
                    g.u8(d.right);
                    runCPU();
                    d.assertResult(cpu, instruction, e.u8(), e.name8, g.name8);
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
                memory.writed(cpu.ds.dword + 200, 0xABCDEF01);
                memory.writeb(cpu.ds.dword + 200, d.left);
                Reg g = Decoder.gb(cpu, rm);
                if (g.parent != null)
                    g.parent.dword = 0xABCDEF01;
                else
                    g.dword = 0xABCDEF01;
                g.u8(d.right);
                runCPU();
                int result = memory.readb(cpu.ds.dword + 200);
                d.assertResult(cpu, instruction, result, null, g.name8);
            }
        }
    }

    private void GbEb(int instruction, Data[] data) {
        for (Data d : data) {
            for (int eb = 0; eb < 8; eb++) {
                for (int gb = 0; gb < 8; gb++) {
                    if (eb == gb)
                        continue;
                    int rm = eb | (gb << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = Decoder.eb(cpu, rm);
                    Reg g = Decoder.gb(cpu, rm);
                    if (e.parent != null)
                        e.parent.dword = 0xABCDEF01;
                    else
                        e.dword = 0xABCDEF01;
                    if (g.parent != null)
                        g.parent.dword = 0xABCDEF01;
                    else
                        g.dword = 0xABCDEF01;
                    e.u8(d.right);
                    g.u8(d.left);
                    runCPU();
                    d.assertResult(cpu, instruction, g.u8(), g.name8, e.name8);
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
                memory.writed(cpu.ds.dword + 200, 0xABCDEF01);
                memory.writeb(cpu.ds.dword + 200, d.right);
                Reg g = Decoder.gb(cpu, rm);
                if (g.parent != null)
                    g.parent.dword = 0xABCDEF01;
                else
                    g.dword = 0xABCDEF01;
                g.u8(d.left);
                runCPU();
                d.assertResult(cpu, instruction, g.u8(), g.name8, null);
            }
        }
    }

    private void EwGw(int instruction, Data[] data) {
        for (Data d : data) {
            for (int ew = 0; ew < 8; ew++) {
                for (int gw = 0; gw < 8; gw++) {
                    if (ew == gw)
                        continue;
                    int rm = ew | (gw << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = Decoder.ew(cpu, rm);
                    Reg g = Decoder.gw(cpu, rm);
                    e.dword = 0xABCDEF01;
                    g.dword = 0xABCDEF01;
                    e.u16(d.left);
                    g.u16(d.right);
                    runCPU();
                    d.assertResult(cpu, instruction, e.u16(), e.name16, g.name16);
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
                memory.writed(cpu.ds.dword + 200, 0xABCDEF01);
                memory.writew(cpu.ds.dword + 200, d.left);
                Reg g = Decoder.gw(cpu, rm);
                g.dword = 0xABCDEF01;
                g.u16(d.right);
                runCPU();
                int result = memory.readw(cpu.ds.dword + 200);
                d.assertResult(cpu, instruction, result, null, g.name16);
            }
        }
    }

    private void GwEw(int instruction, Data[] data) {
        for (Data d : data) {
            for (int ew = 0; ew < 8; ew++) {
                for (int gw = 0; gw < 8; gw++) {
                    if (ew == gw)
                        continue;
                    int rm = ew | (gw << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = Decoder.ew(cpu, rm);
                    Reg g = Decoder.gw(cpu, rm);
                    e.dword = 0xABCDEF01;
                    g.dword = 0xABCDEF01;
                    e.u16(d.right);
                    g.u16(d.left);
                    runCPU();
                    d.assertResult(cpu, instruction, g.u16(), g.name16, e.name16);
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
                memory.writed(cpu.ds.dword + 200, 0xABCDEF01);
                memory.writew(cpu.ds.dword + 200, d.right);
                Reg g = Decoder.gw(cpu, rm);
                g.dword = 0xABCDEF01;
                g.u16(d.left);
                runCPU();
                d.assertResult(cpu, instruction, g.u16(), g.name16, null);
            }
        }
    }

    private void EdGd(int instruction, Data[] data) {
        for (Data d : data) {
            for (int ed = 0; ed < 8; ed++) {
                for (int gd = 0; gd < 8; gd++) {
                    if (ed == gd)
                        continue;
                    int rm = ed | (gd << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = Decoder.ed(cpu, rm);
                    Reg g = Decoder.gd(cpu, rm);
                    e.dword = d.left;
                    g.dword = d.right;
                    runCPU();
                    d.assertResult(cpu, instruction, e.dword, e.name32, g.name32);
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
                Reg g = Decoder.gw(cpu, rm);
                g.dword = d.right;
                runCPU();
                int result = memory.readd(cpu.ds.dword + 200);
                d.assertResult(cpu, instruction, result, null, g.name16);
            }
        }
    }

    private void GdEd(int instruction, Data[] data) {
        for (Data d : data) {
            for (int ed = 0; ed < 8; ed++) {
                for (int gd = 0; gd < 8; gd++) {
                    if (ed == gd)
                        continue;
                    int rm = ed | (gd << 3) | 0xC0;
                    newInstruction(instruction, rm, d.flags);
                    d.pushConstant();
                    Reg e = Decoder.ed(cpu, rm);
                    Reg g = Decoder.gd(cpu, rm);
                    e.dword = d.right;
                    g.dword = d.left;
                    runCPU();
                    d.assertResult(cpu, instruction, g.dword, g.name32, e.name32);
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
                Reg g = Decoder.gd(cpu, rm);
                g.dword = d.left;
                runCPU();
                d.assertResult(cpu, instruction, g.dword, g.name32, null);
            }
        }
    }

    private void AlIb(int instruction, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            pushCode8(d.right);
            cpu.eax.dword = 0xABCDEF01;
            cpu.eax.u8(d.left);
            runCPU();
            d.assertResult(cpu, instruction, cpu.eax.u8(), cpu.eax.name8, null);
        }
    }

    private void AxIw(int instruction, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            pushCode16(d.right);
            cpu.eax.dword = 0xABCDEF01;
            cpu.eax.u16(d.left);
            runCPU();
            d.assertResult(cpu, instruction, cpu.eax.u16(), cpu.eax.name16, null);
        }
    }

    private void EaxId(int instruction, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            pushCode32(d.right);
            cpu.eax.dword = d.left;
            runCPU();
            d.assertResult(cpu, instruction, cpu.eax.dword, cpu.eax.name32, null);
        }
    }

    private void Eb(int instruction, Reg reg, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            reg.dword = 0xABCDEF01;
            reg.u8(d.left);
            runCPU();
            d.assertResult(cpu, instruction, reg.u8(), reg.name8, null);
        }
    }

    private void Ew(int instruction, Reg reg, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            reg.dword = 0xABCDEF01;
            reg.u16(d.left);
            runCPU();
            d.assertResult(cpu, instruction, reg.u16(), reg.name16, null);
        }
    }

    private void Ed(int instruction, Reg reg, Data[] data) {
        for (Data d : data) {
            newInstruction(instruction, d.flags);
            reg.dword = d.left;
            runCPU();
            d.assertResult(cpu, instruction, reg.dword, reg.name32, null);
        }
    }

    private void Push16(int instruction, Reg reg) {
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

    private void Push32(int instruction, Reg reg) {
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

    private void Pop16(int instruction, Reg reg) {
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

    private void Pop32(int instruction, Reg reg) {
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

    private void Push16(int instruction) {
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

    private void Push32(int instruction) {
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

    private void Push16s8(int instruction) {
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

    private void Push32s8(int instruction) {
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

    private class Data {
        public Data(int left, int right, int result, int flags, boolean CF, boolean OF) {
            this.left = left;
            this.right = right;
            this.result = result;
            this.flags = flags;
            this.CF = CF;
            this.OF = OF;
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
            dontUseResultAndCheckSFZF = false;
        }

        public Data(int left, int right, boolean CF, boolean OF, boolean SF, boolean ZF) {
            this.left = left;
            this.right = right;
            this.result = 0;
            this.flags = flags;
            this.CF = CF;
            this.OF = OF;
            this.ZF = ZF;
            this.SF = SF;
            this.dontUseResultAndCheckSFZF = true;
        }
        public int left;
        public int right;
        public int result;
        public int flags;
        public int constant;
        public boolean CF;
        public boolean OF;
        public boolean ZF;
        public boolean SF;
        public boolean dontUseResultAndCheckSFZF;
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

        private StringBuilder buildErrorMsg(CPU cpu, int instruction, int result, String leftRegName, String rightRegName) {
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
                builder.append(result);
            }
            builder.append("), ");
            if (rightRegName!=null)
                builder.append(rightRegName);
            builder.append("(");
            builder.append(cpu.right);
            builder.append(")");
            if (dontUseResultAndCheckSFZF) {
                builder.append(" result(");
                builder.append(cpu.result);
                builder.append(")");
            }
            return builder;
        }
        public void assertResult(CPU cpu, int instruction, int result, String leftRegName, String rightRegName) {
            if (!dontUseResultAndCheckSFZF && this.result != result) {
                StringBuilder builder = buildErrorMsg(cpu, instruction, result, leftRegName, rightRegName);
                builder.append(" did not equal ");
                builder.append(this.result);
                assertTrue(builder.toString(), false);
            }
            if (cpu.CF() != CF) {
                StringBuilder builder = buildErrorMsg(cpu, instruction, result, leftRegName, rightRegName);
                builder.append(" expected CF value: ");
                builder.append(CF);
                assertTrue(builder.toString(), false);
            }
            if (cpu.OF() != OF) {
                StringBuilder builder = buildErrorMsg(cpu, instruction, result, leftRegName, rightRegName);
                builder.append(" expected OF value: ");
                builder.append(OF);
                assertTrue(builder.toString(), false);
            }
            if (dontUseResultAndCheckSFZF) {
                if (cpu.SF() != SF) {
                    StringBuilder builder = buildErrorMsg(cpu, instruction, result, leftRegName, rightRegName);
                    builder.append(" expected SF value: ");
                    builder.append(SF);
                    assertTrue(builder.toString(), false);
                }
                if (cpu.ZF() != ZF) {
                    StringBuilder builder = buildErrorMsg(cpu, instruction, result, leftRegName, rightRegName);
                    builder.append(" expected ZF value: ");
                    builder.append(ZF);
                    assertTrue(builder.toString(), false);
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
}

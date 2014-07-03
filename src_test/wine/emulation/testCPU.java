package wine.emulation;

import junit.framework.TestCase;

public class testCPU extends TestCase {
    private CPU cpu;
    private Memory memory;
    private int cseip;

    private static final int STACK_ADDRESS=0xE0001000;
    private static final int HEAP_ADDRESS =0xF0000000;
    private static final int CODE_ADDRESS =0xD0000000;

    protected void setUp() throws java.lang.Exception {
        memory = new Memory();
        memory.init();
        this.cpu = new CPU(memory);
        RAM.init(4096*4);
        memory.handlers[(STACK_ADDRESS>>>12)-1] = new RAMHandler(memory, RAM.allocPage(), false, false);
        memory.handlers[HEAP_ADDRESS>>>12] = new RAMHandler(memory, RAM.allocPage(), false, false);
        memory.handlers[CODE_ADDRESS>>>12] = new RAMHandler(memory, RAM.allocPage(), false, false);
        cpu.cs.dword = CODE_ADDRESS;
        cpu.ds.dword = HEAP_ADDRESS;
        cpu.ss.dword = STACK_ADDRESS;
        cseip=CODE_ADDRESS;
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
        cpu.esp.dword=0;
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

    private void EbGb(int instruction, int ebValue, int gbValue, int ebResult, int flags, boolean CF, boolean OF) {
        for (int eb=0;eb<8;eb++) {
            for (int gb=0;gb<8;gb++) {
                if (eb==gb)
                    continue;
                int rm = eb | (gb << 3) | 0xC0;
                newInstruction(instruction, rm, flags);
                Reg e = Decoder.eb(cpu, rm);
                Reg g = Decoder.gb(cpu, rm);
                if (e.parent!=null)
                    e.parent.dword=0xABCDEF01;
                else
                    e.dword=0xABCDEF01;
                if (g.parent!=null)
                    g.parent.dword=0xABCDEF01;
                else
                    g.dword=0xABCDEF01;
                e.u8(ebValue);
                g.u8(gbValue);
                runCPU();
                if (ebResult!=e.u8()) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+e.name8+"("+ebValue+","+e.u8()+"), "+g.name8+"("+g.u8()+") did not equal "+ebResult, false);
                }
                if (cpu.CF()!=CF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+e.name8+"("+ebValue+","+e.u8()+"), "+g.name8+"("+g.u8()+") expect CF value: "+CF, false);
                }
                if (cpu.OF()!=OF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+e.name8+"("+ebValue+","+e.u8()+"), "+g.name8+"("+g.u8()+") expect OF value: "+OF, false);
                }
            }
        }

        for (int gb=0;gb<8;gb++) {
            int rm = (gb << 3);
            if (cpu.big)
                rm+=5;
            else
                rm+=6;
            newInstruction(instruction, rm, flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword+200, 0xABCDEF01);
            memory.writeb(cpu.ds.dword+200, ebValue);
            Reg g = Decoder.gb(cpu, rm);
            if (g.parent!=null)
                g.parent.dword=0xABCDEF01;
            else
                g.dword=0xABCDEF01;
            g.u8(gbValue);
            runCPU();
            int result = memory.readb(cpu.ds.dword+200);
            if (ebResult!=result) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: ("+ebValue+","+result+"), "+g.name8+"("+g.u8()+") did not equal "+ebResult, false);
            }
            if (cpu.CF()!=CF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: ("+ebValue+","+result+"), "+g.name8+"("+g.u8()+") expect CF value: "+CF, false);
            }
            if (cpu.OF()!=OF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: ("+ebValue+","+result+"), "+g.name8+"("+g.u8()+") expect OF value: "+OF, false);
            }
        }
    }

    private void GbEb(int instruction, int gbValue, int ebValue, int gbResult, int flags, boolean CF, boolean OF) {
        for (int eb=0;eb<8;eb++) {
            for (int gb=0;gb<8;gb++) {
                if (eb==gb)
                    continue;
                int rm = eb | (gb << 3) | 0xC0;
                newInstruction(instruction, rm, flags);
                Reg e = Decoder.eb(cpu, rm);
                Reg g = Decoder.gb(cpu, rm);
                if (e.parent!=null)
                    e.parent.dword=0xABCDEF01;
                else
                    e.dword=0xABCDEF01;
                if (g.parent!=null)
                    g.parent.dword=0xABCDEF01;
                else
                    g.dword=0xABCDEF01;
                e.u8(ebValue);
                g.u8(gbValue);
                runCPU();
                if (gbResult!=g.u8()) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name8+"("+gbValue+","+g.u8()+"), "+e.name8+"("+e.u8()+") did not equal "+gbResult, false);
                }
                if (cpu.CF()!=CF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name8+"("+gbValue+","+g.u8()+"), "+e.name8+"("+e.u8()+") expect CF value: "+CF, false);
                }
                if (cpu.OF()!=OF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name8+"("+gbValue+","+g.u8()+"), "+e.name8+"("+e.u8()+") expect OF value: "+OF, false);
                }
            }
        }

        for (int gb=0;gb<8;gb++) {
            int rm = (gb << 3);
            if (cpu.big)
                rm+=5;
            else
                rm+=6;
            newInstruction(instruction, rm, flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword+200, 0xABCDEF01);
            memory.writeb(cpu.ds.dword+200, ebValue);
            Reg g = Decoder.gb(cpu, rm);
            if (g.parent!=null)
                g.parent.dword=0xABCDEF01;
            else
                g.dword=0xABCDEF01;
            g.u8(gbValue);
            runCPU();
            if (gbResult!=g.u8()) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name8+"("+g.u8()+", "+gbValue+"), ("+ebValue+") did not equal "+gbResult, false);
            }
            if (cpu.CF()!=CF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name8+"("+g.u8()+", "+gbValue+"), ("+ebValue+") expect CF value: "+CF, false);
            }
            if (cpu.OF()!=OF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name8+"("+g.u8()+", "+gbValue+"), ("+ebValue+") expect OF value: "+OF, false);
            }
        }
    }

    private void EwGw(int instruction, int ewValue, int gwValue, int ewResult, int flags, boolean CF, boolean OF) {
        for (int ew=0;ew<8;ew++) {
            for (int gw=0;gw<8;gw++) {
                if (ew==gw)
                    continue;
                int rm = ew | (gw << 3) | 0xC0;
                newInstruction(instruction, rm, flags);
                Reg e = Decoder.ew(cpu, rm);
                Reg g = Decoder.gw(cpu, rm);
                e.dword=0xABCDEF01;
                g.dword=0xABCDEF01;
                e.u16(ewValue);
                g.u16(gwValue);
                runCPU();
                if (ewResult!=Decoder.ew(cpu, rm).u16()) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+e.name16+"("+ewValue+","+e.u16()+"), "+g.name16+"("+g.u16()+") did not equal "+ewResult, false);
                }
                if (cpu.CF()!=CF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+e.name16+"("+ewValue+","+e.u16()+"), "+g.name16+"("+g.u16()+") expect CF value: "+CF, false);
                }
                if (cpu.OF()!=OF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+e.name16+"("+ewValue+","+e.u16()+"), "+g.name16+"("+g.u16()+") expect OF value: "+OF, false);
                }
            }
        }

        for (int gw=0;gw<8;gw++) {
            int rm = (gw << 3);
            if (cpu.big)
                rm+=5;
            else
                rm+=6;
            newInstruction(instruction, rm, flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword+200, 0xABCDEF01);
            memory.writew(cpu.ds.dword+200, ewValue);
            Reg g = Decoder.gw(cpu, rm);
            g.dword=0xABCDEF01;
            g.u16(gwValue);
            runCPU();
            int result = memory.readw(cpu.ds.dword+200);
            if (ewResult!=result) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: ("+ewValue+","+result+"), "+g.name16+"("+g.u16()+") did not equal "+ewResult, false);
            }
            if (cpu.CF()!=CF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: ("+ewValue+","+result+"), "+g.name16+"("+g.u16()+") expect CF value: "+CF, false);
            }
            if (cpu.OF()!=OF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: ("+ewValue+","+result+"), "+g.name16+"("+g.u16()+") expect OF value: "+OF, false);
            }
        }
    }

    private void GwEw(int instruction, int gwValue, int ewValue, int gwResult, int flags, boolean CF, boolean OF) {
        for (int ew=0;ew<8;ew++) {
            for (int gw=0;gw<8;gw++) {
                if (ew==gw)
                    continue;
                int rm = ew | (gw << 3) | 0xC0;
                newInstruction(instruction, rm, flags);
                Reg e = Decoder.ew(cpu, rm);
                Reg g = Decoder.gw(cpu, rm);
                e.dword=0xABCDEF01;
                g.dword=0xABCDEF01;
                e.u16(ewValue);
                g.u16(gwValue);
                runCPU();
                if (gwResult!=g.u16()) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name16+"("+gwValue+","+g.u16()+"), "+e.name16+"("+e.u16()+") did not equal "+gwResult, false);
                }
                if (cpu.CF()!=CF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name16+"("+gwValue+","+g.u16()+"), "+e.name16+"("+e.u16()+") expect CF value: "+CF, false);
                }
                if (cpu.OF()!=OF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name16+"("+gwValue+","+g.u16()+"), "+e.name16+"("+e.u16()+") expect OF value: "+OF, false);
                }
            }
        }

        for (int gw=0;gw<8;gw++) {
            int rm = (gw << 3);
            if (cpu.big)
                rm+=5;
            else
                rm+=6;
            newInstruction(instruction, rm, flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword+200, 0xABCDEF01);
            memory.writew(cpu.ds.dword+200, ewValue);
            Reg g = Decoder.gw(cpu, rm);
            g.dword=0xABCDEF01;
            g.u16(gwValue);
            runCPU();
            if (gwResult!=g.u16()) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name16+"("+g.u16()+", "+gwValue+"), ("+ewValue+") did not equal "+gwResult, false);
            }
            if (cpu.CF()!=CF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name16+"("+g.u16()+", "+gwValue+"), ("+ewValue+") expect CF value: "+CF, false);
            }
            if (cpu.OF()!=OF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name16+"("+g.u16()+", "+gwValue+"), ("+ewValue+") expect OF value: "+OF, false);
            }
        }
    }

    private void EdGd(int instruction, int edValue, int gdValue, int edResult, int flags, boolean CF, boolean OF) {
        for (int ed=0;ed<8;ed++) {
            for (int gd=0;gd<8;gd++) {
                if (ed==gd)
                    continue;
                int rm = ed | (gd << 3) | 0xC0;
                newInstruction(instruction, rm, flags);
                Reg e = Decoder.ed(cpu, rm);
                Reg g = Decoder.gd(cpu, rm);
                e.dword=edValue;
                g.dword=gdValue;
                runCPU();
                if (edResult!=Decoder.ed(cpu, rm).dword) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+e.name32+"("+edValue+","+e.dword+"), "+g.name32+"("+g.dword+") did not equal "+edResult, false);
                }
                if (cpu.CF()!=CF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+e.name32+"("+edValue+","+e.dword+"), "+g.name32+"("+g.dword+") expect CF value: "+CF, false);
                }
                if (cpu.OF()!=OF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+e.name32+"("+edValue+","+e.dword+"), "+g.name32+"("+g.dword+") expect OF value: "+OF, false);
                }
            }
        }

        for (int gd=0;gd<8;gd++) {
            int rm = (gd << 3);
            if (cpu.big)
                rm+=5;
            else
                rm+=6;
            newInstruction(instruction, rm, flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword+200, edValue);
            Reg g = Decoder.gw(cpu, rm);
            g.dword=gdValue;
            runCPU();
            int result = memory.readd(cpu.ds.dword+200);
            if (edResult!=result) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: ("+edValue+","+result+"), "+g.name32+"("+g.dword+") did not equal "+edResult, false);
            }
            if (cpu.CF()!=CF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: ("+edValue+","+result+"), "+g.name32+"("+g.dword+") expect CF value: "+CF, false);
            }
            if (cpu.OF()!=OF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: ("+edValue+","+result+"), "+g.name32+"("+g.dword+") expect OF value: "+OF, false);
            }
        }
    }

    private void GdEd(int instruction, int gdValue, int edValue, int gdResult, int flags, boolean CF, boolean OF) {
        for (int ed=0;ed<8;ed++) {
            for (int gd=0;gd<8;gd++) {
                if (ed==gd)
                    continue;
                int rm = ed | (gd << 3) | 0xC0;
                newInstruction(instruction, rm, flags);
                Reg e = Decoder.ed(cpu, rm);
                Reg g = Decoder.gd(cpu, rm);
                e.dword=edValue;
                g.dword=gdValue;
                runCPU();
                if (gdResult!=g.dword) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name32+"("+gdValue+","+g.dword+"), "+e.name32+"("+e.dword+") did not equal "+gdResult, false);
                }
                if (cpu.CF()!=CF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name32+"("+gdValue+","+g.dword+"), "+e.name32+"("+e.dword+") expect CF value: "+CF, false);
                }
                if (cpu.OF()!=OF) {
                    assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name32+"("+gdValue+","+g.dword+"), "+e.name32+"("+e.dword+") expect OF value: "+OF, false);
                }
            }
        }

        for (int gd=0;gd<8;gd++) {
            int rm = (gd << 3);
            if (cpu.big)
                rm+=5;
            else
                rm+=6;
            newInstruction(instruction, rm, flags);
            if (cpu.big)
                pushCode32(200);
            else
                pushCode16(200);
            memory.writed(cpu.ds.dword+200, edValue);
            Reg g = Decoder.gw(cpu, rm);
            g.dword=gdValue;
            runCPU();
            if (gdResult!=g.dword) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name32+"("+g.dword+", "+gdValue+"), ("+edValue+") did not equal "+gdResult, false);
            }
            if (cpu.CF()!=CF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name32+"("+g.dword+", "+gdValue+"), ("+edValue+") expect CF value: "+CF, false);
            }
            if (cpu.OF()!=OF) {
                assertTrue("op "+Integer.toHexString(instruction)+" failed: "+g.name32+"("+g.dword+", "+gdValue+"), ("+edValue+") expect OF value: "+OF, false);
            }
        }
    }

    private void AlIb(int instruction, int alValue, int value, int result, int flags, boolean CF, boolean OF) {
        newInstruction(instruction, flags);
        pushCode8(value);
        cpu.eax.dword=0xABCDEF01;
        cpu.eax.u8(alValue);
        runCPU();
        if (result!=cpu.eax.u8()) {
            assertTrue("op "+Integer.toHexString(instruction)+" failed: al("+cpu.eax.u8()+", "+alValue+"), ("+value+") did not equal "+result, false);
        }
        if (cpu.CF()!=CF) {
            assertTrue("op "+Integer.toHexString(instruction)+" failed: al("+cpu.eax.u8()+", "+alValue+"), ("+value+") expect CF value: "+CF, false);
        }
        if (cpu.OF()!=OF) {
            assertTrue("op "+Integer.toHexString(instruction)+" failed: al("+cpu.eax.u8()+", "+alValue+"), ("+value+") expect OF value: "+OF, false);
        }
    }

    private void AxIw(int instruction, int axValue, int value, int result, int flags, boolean CF, boolean OF) {
        newInstruction(instruction, flags);
        pushCode16(value);
        cpu.eax.dword=0xABCDEF01;
        cpu.eax.u16(axValue);
        runCPU();
        if (result!=cpu.eax.u16()) {
            assertTrue("op "+Integer.toHexString(instruction)+" failed: ax("+cpu.eax.u16()+", "+axValue+"), ("+value+") did not equal "+result, false);
        }
        if (cpu.CF()!=CF) {
            assertTrue("op "+Integer.toHexString(instruction)+" failed: ax("+cpu.eax.u16()+", "+axValue+"), ("+value+") expect CF value: "+CF, false);
        }
        if (cpu.OF()!=OF) {
            assertTrue("op "+Integer.toHexString(instruction)+" failed: ax("+cpu.eax.u16()+", "+axValue+"), ("+value+") expect OF value: "+OF, false);
        }
    }

    private void EaxId(int instruction, int eaxValue, int value, int result, int flags, boolean CF, boolean OF) {
        newInstruction(instruction, flags);
        pushCode32(value);
        cpu.eax.dword=eaxValue;
        runCPU();
        if (result!=cpu.eax.dword) {
            assertTrue("op "+Integer.toHexString(instruction)+" failed: eax("+cpu.eax.dword+", "+eaxValue+"), ("+value+") did not equal "+result, false);
        }
        if (cpu.CF()!=CF) {
            assertTrue("op "+Integer.toHexString(instruction)+" failed: eax("+cpu.eax.dword+", "+eaxValue+"), ("+value+") expect CF value: "+CF, false);
        }
        if (cpu.OF()!=OF) {
            assertTrue("op "+Integer.toHexString(instruction)+" failed: eax("+cpu.eax.dword+", "+eaxValue+"), ("+value+") expect OF value: "+OF, false);
        }
    }

    public void testAdd0x000() {
        cpu.big = false;
        EbGb(0x00, 1, 2, 3, 0, false, false);
        EbGb(0x00, 0xFF, 1, 0, 0, true, false);
        EbGb(0x00, 1, 0xFF, 0, 0, true, false);
        EbGb(0x00, 0, 0, 0, 0, false, false);
        EbGb(0x00, 0xFF, 0xFF, 0xFE, 0, true, false);
        EbGb(0x00, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x200() {
        cpu.big = true;
        EbGb(0x00, 1, 2, 3, 0, false, false);
        EbGb(0x00, 0xFF, 1, 0, 0, true, false);
        EbGb(0x00, 1, 0xFF, 0, 0, true, false);
        EbGb(0x00, 0, 0, 0, 0, false, false);
        EbGb(0x00, 0xFF, 0xFF, 0xFE, 0, true, false);
        EbGb(0x00, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x001() {
        cpu.big = false;
        EwGw(0x01, 1000, 2002, 3002, 0, false, false);
        EwGw(0x01, 0xFFFF, 1, 0, 0, true, false);
        EwGw(0x01, 1, 0xFFFF, 0, 0, true, false);
        EwGw(0x01, 0, 0, 0, 0, false, false);
        EwGw(0x01, 0xFFFF, 0xFFFF, 0xFFFE, 0, true, false);
        EwGw(0x01, 16384, 16384, 32768, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x201() {
        cpu.big = true;
        EdGd(0x01, 100000, 200200, 300200, 0, false, false);
        EdGd(0x01, 0xFFFFFFFF, 1, 0, 0, true, false);
        EdGd(0x01, 1, 0xFFFFFFFF, 0, 0, true, false);
        EdGd(0x01, 0, 0, 0, 0, false, false);
        EdGd(0x01, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, true, false);
        EdGd(0x01, 0x40000000, 0x40000000, 0x80000000, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x002() {
        cpu.big = false;
        GbEb(0x02, 1, 2, 3, 0, false, false);
        GbEb(0x02, 0xFF, 1, 0, 0, true, false);
        GbEb(0x02, 1, 0xFF, 0, 0, true, false);
        GbEb(0x02, 0, 0, 0, 0, false, false);
        GbEb(0x02, 0xFF, 0xFF, 0xFE, 0, true, false);
        GbEb(0x02, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x202() {
        cpu.big = true;
        GbEb(0x02, 1, 2, 3, 0, false, false);
        GbEb(0x02, 0xFF, 1, 0, 0, true, false);
        GbEb(0x02, 1, 0xFF, 0, 0, true, false);
        GbEb(0x02, 0, 0, 0, 0, false, false);
        GbEb(0x02, 0xFF, 0xFF, 0xFE, 0, true, false);
        GbEb(0x02, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x003() {
        cpu.big = false;
        GwEw(0x03, 1000, 2002, 3002, 0, false, false);
        GwEw(0x03, 0xFFFF, 1, 0, 0, true, false);
        GwEw(0x03, 1, 0xFFFF, 0, 0, true, false);
        GwEw(0x03, 0, 0, 0, 0, false, false);
        GwEw(0x03, 0xFFFF, 0xFFFF, 0xFFFE, 0, true, false);
        GwEw(0x03, 16384, 16384, 32768, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x203() {
        cpu.big = true;
        GdEd(0x03, 100000, 200200, 300200, 0, false, false);
        GdEd(0x03, 0xFFFFFFFF, 1, 0, 0, true, false);
        GdEd(0x03, 1, 0xFFFFFFFF, 0, 0, true, false);
        GdEd(0x03, 0, 0, 0, 0, false, false);
        GdEd(0x03, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, true, false);
        GdEd(0x03, 0x40000000, 0x40000000, 0x80000000, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x004() {
        cpu.big = false;
        AlIb(0x04, 1, 2, 3, 0, false, false);
        AlIb(0x04, 0xFF, 1, 0, 0, true, false);
        AlIb(0x04, 1, 0xFF, 0, 0, true, false);
        AlIb(0x04, 0, 0, 0, 0, false, false);
        AlIb(0x04, 0xFF, 0xFF, 0xFE, 0, true, false);
        AlIb(0x04, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x204() {
        cpu.big = true;
        AlIb(0x04, 1, 2, 3, 0, false, false);
        AlIb(0x04, 0xFF, 1, 0, 0, true, false);
        AlIb(0x04, 1, 0xFF, 0, 0, true, false);
        AlIb(0x04, 0, 0, 0, 0, false, false);
        AlIb(0x04, 0xFF, 0xFF, 0xFE, 0, true, false);
        AlIb(0x04, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x005() {
        cpu.big = false;
        AxIw(0x05, 1000, 2002, 3002, 0, false, false);
        AxIw(0x05, 0xFFFF, 1, 0, 0, true, false);
        AxIw(0x05, 1, 0xFFFF, 0, 0, true, false);
        AxIw(0x05, 0, 0, 0, 0, false, false);
        AxIw(0x05, 0xFFFF, 0xFFFF, 0xFFFE, 0, true, false);
        AxIw(0x05, 16384, 16384, 32768, 0, false, true); // overflow indicates that the sign changed
    }

    public void testAdd0x205() {
        cpu.big = true;
        EaxId(0x05, 100000, 200200, 300200, 0, false, false);
        EaxId(0x05, 0xFFFFFFFF, 1, 0, 0, true, false);
        EaxId(0x05, 1, 0xFFFFFFFF, 0, 0, true, false);
        EaxId(0x05, 0, 0, 0, 0, false, false);
        EaxId(0x05, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, true, false);
        EaxId(0x05, 0x40000000, 0x40000000, 0x80000000, 0, false, true); // overflow indicates that the sign changed
    }

    public void testOr0x008() {
        cpu.big = false;
        EbGb(0x08, 1, 2, 3, 0, false, false);
        EbGb(0x08, 0xFF, 0, 0xFF, 0, false, false);
        EbGb(0x08, 0, 0xFF, 0xFF, 0, false, false);
        EbGb(0x08, 0, 0, 0, 0, false, false);
        EbGb(0x08, 0xF0, 0x0F, 0xFF, 0, false, false);
    }

    public void testOr0x208() {
        cpu.big = true;
        EbGb(0x08, 1, 2, 3, 0, false, false);
        EbGb(0x08, 0xFF, 0, 0xFF, 0, false, false);
        EbGb(0x08, 0, 0xFF, 0xFF, 0, false, false);
        EbGb(0x08, 0, 0, 0, 0, false, false);
        EbGb(0x08, 0xF0, 0x0F, 0xFF, 0, false, false);
    }

    public void testOr0x009() {
        cpu.big = false;
        EwGw(0x09, 1000, 2002, 2042, 0, false, false); // 11 1110 1000 | 111 1101 0010
        EwGw(0x09, 0xFFFF, 0, 0xFFFF, 0, false, false);
        EwGw(0x09, 0, 0xFFFF, 0xFFFF, 0, false, false);
        EwGw(0x09, 0, 0, 0, 0, false, false);
        EwGw(0x09, 0xF00F, 0x0FF0, 0xFFFF, 0, false, false);
    }

    public void testOr0x209() {
        cpu.big = true;
        EdGd(0x09, 100000, 200200, 233128, 0, false, false);
        EdGd(0x09, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, false, false);
        EdGd(0x09, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, false, false);
        EdGd(0x09, 0, 0, 0, 0, false, false);
        EdGd(0x09, 0xF0F0F0F0, 0x0F0F0F0F, 0xFFFFFFFF, 0, false, false);
    }

    public void testOr0x00a() {
        cpu.big = false;
        GbEb(0x0a, 1, 2, 3, 0, false, false);
        GbEb(0x0a, 0xFF, 0, 0xFF, 0, false, false);
        GbEb(0x0a, 0, 0xFF, 0xFF, 0, false, false);
        GbEb(0x0a, 0, 0, 0, 0, false, false);
        GbEb(0x0a, 0xF0, 0x0F, 0xFF, 0, false, false);
    }

    public void testOr0x20a() {
        cpu.big = true;
        GbEb(0x0a, 1, 2, 3, 0, false, false);
        GbEb(0x0a, 0xFF, 0, 0xFF, 0, false, false);
        GbEb(0x0a, 0, 0xFF, 0xFF, 0, false, false);
        GbEb(0x0a, 0, 0, 0, 0, false, false);
        GbEb(0x0a, 0x0C, 0xC0, 0xCC, 0, false, false);
    }

    public void testOr0x00b() {
        cpu.big = false;
        GwEw(0x0b, 1000, 2002, 2042, 0, false, false);
        GwEw(0x0b, 0xFFFF, 0, 0xFFFF, 0, false, false);
        GwEw(0x0b, 0, 0xFFFF, 0xFFFF, 0, false, false);
        GwEw(0x0b, 0, 0, 0, 0, false, false);
        GwEw(0x0b, 0xAB00, 0x00CD, 0xABCD, 0, false, false);
    }

    public void testOr0x20b() {
        cpu.big = true;
        GdEd(0x0b, 100000, 200200, 233128, 0, false, false);
        GdEd(0x0b, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, false, false);
        GdEd(0x0b, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, false, false);
        GdEd(0x0b, 0, 0, 0, 0, false, false);
        GdEd(0x0b, 0x10305070, 0x02040608, 0x12345678, 0, false, false);
    }

    public void testOr0x00c() {
        cpu.big = false;
        AlIb(0x0c, 1, 2, 3, 0, false, false);
        AlIb(0x0c, 0xFF, 0, 0xFF, 0, false, false);
        AlIb(0x0c, 0, 0xFF, 0xFF, 0, false, false);
        AlIb(0x0c, 0, 0, 0, 0, false, false);
        AlIb(0x0c, 0xF0, 0x0F, 0xFF, 0, false, false);
    }

    public void testOr0x20c() {
        cpu.big = true;
        AlIb(0x0c, 1, 2, 3, 0, false, false);
        AlIb(0x0c, 0xFF, 0, 0xFF, 0, false, false);
        AlIb(0x0c, 0, 0xFF, 0xFF, 0, false, false);
        AlIb(0x0c, 0, 0, 0, 0, false, false);
        AlIb(0x0c, 0x0F, 0xF0, 0xFF, 0, false, false);
    }

    public void testOr0x00d() {
        cpu.big = false;
        AxIw(0x0d, 1000, 2002, 2042, 0, false, false);
        AxIw(0x0d, 0xFFFF, 0, 0xFFFF, 0, false, false);
        AxIw(0x0d, 0, 0xFFFF, 0xFFFF, 0, false, false);
        AxIw(0x0d, 0, 0, 0, 0, false, false);
        AxIw(0x0d, 0x0FF0, 0xF00F, 0xFFFF, 0, false, false);
    }

    public void testOr0x20d() {
        cpu.big = true;
        EaxId(0x0d, 100000, 200200, 233128, 0, false, false);
        EaxId(0x0d, 0xFFFFFFFF, 0, 0xFFFFFFFF, 0, false, false);
        EaxId(0x0d, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0, false, false);
        EaxId(0x0d, 0, 0, 0, 0, false, false);
        EaxId(0x0d, 0xFFFF0000, 0x0000FFFF, 0xFFFFFFFF, 0, false, false);
    }

    public void testAdc0x010() {
        cpu.big = false;
        EbGb(0x10, 1, 2, 3, 0, false, false);
        EbGb(0x10, 0xFF, 1, 0, 0, true, false);
        EbGb(0x10, 1, 0xFF, 0, 0, true, false);
        EbGb(0x10, 0, 0, 0, 0, false, false);
        EbGb(0x10, 0xFF, 0xFF, 0xFE, 0, true, false);
        EbGb(0x10, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
        EbGb(0x10, 1, 2, 4, CPU.CF, false, false);
        EbGb(0x10, 0xFF, 1, 1, CPU.CF, true, false);
        EbGb(0x10, 1, 0xFF, 1, CPU.CF, true, false);
        EbGb(0x10, 0, 0, 1, CPU.CF, false, false);
        EbGb(0x10, 0xFF, 0xFF, 0xFF, CPU.CF, true, false);
        EbGb(0x10, 64, 64, 129, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x210() {
        cpu.big = true;
        EbGb(0x10, 1, 2, 3, 0, false, false);
        EbGb(0x10, 0xFF, 1, 0, 0, true, false);
        EbGb(0x10, 1, 0xFF, 0, 0, true, false);
        EbGb(0x10, 0, 0, 0, 0, false, false);
        EbGb(0x10, 0xFF, 0xFF, 0xFE, 0, true, false);
        EbGb(0x10, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
        EbGb(0x10, 1, 2, 4, CPU.CF, false, false);
        EbGb(0x10, 0xFF, 1, 1, CPU.CF, true, false);
        EbGb(0x10, 1, 0xFF, 1, CPU.CF, true, false);
        EbGb(0x10, 0, 0, 1, CPU.CF, false, false);
        EbGb(0x10, 0xFF, 0xFF, 0xFF, CPU.CF, true, false);
        EbGb(0x10, 64, 64, 129, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x011() {
        cpu.big = false;
        EwGw(0x11, 1000, 2002, 3002, 0, false, false);
        EwGw(0x11, 0xFFFF, 1, 0, 0, true, false);
        EwGw(0x11, 1, 0xFFFF, 0, 0, true, false);
        EwGw(0x11, 0, 0, 0, 0, false, false);
        EwGw(0x11, 0xFFFF, 0xFFFF, 0xFFFE, 0, true, false);
        EwGw(0x11, 16384, 16384, 32768, 0, false, true); // overflow indicates that the sign changed
        EwGw(0x11, 1000, 2002, 3003, CPU.CF, false, false);
        EwGw(0x11, 0xFFFF, 1, 1, CPU.CF, true, false);
        EwGw(0x11, 1, 0xFFFF, 1, CPU.CF, true, false);
        EwGw(0x11, 0, 0, 1, CPU.CF, false, false);
        EwGw(0x11, 0xFFFF, 0xFFFF, 0xFFFF, CPU.CF, true, false);
        EwGw(0x11, 16384, 16384, 32769, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x211() {
        cpu.big = true;
        EdGd(0x11, 100000, 200200, 300200, 0, false, false);
        EdGd(0x11, 0xFFFFFFFF, 1, 0, 0, true, false);
        EdGd(0x11, 1, 0xFFFFFFFF, 0, 0, true, false);
        EdGd(0x11, 0, 0, 0, 0, false, false);
        EdGd(0x11, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, true, false);
        EdGd(0x11, 0x40000000, 0x40000000, 0x80000000, 0, false, true); // overflow indicates that the sign changed
        EdGd(0x11, 100000, 200200, 300201, CPU.CF, false, false);
        EdGd(0x11, 0xFFFFFFFF, 1, 1, CPU.CF, true, false);
        EdGd(0x11, 1, 0xFFFFFFFF, 1, CPU.CF, true, false);
        EdGd(0x11, 0, 0, 1, CPU.CF, false, false);
        EdGd(0x11, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, CPU.CF, true, false);
        EdGd(0x11, 0x40000000, 0x40000000, 0x80000001, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x012() {
        cpu.big = false;
        GbEb(0x12, 1, 2, 3, 0, false, false);
        GbEb(0x12, 0xFF, 1, 0, 0, true, false);
        GbEb(0x12, 1, 0xFF, 0, 0, true, false);
        GbEb(0x12, 0, 0, 0, 0, false, false);
        GbEb(0x12, 0xFF, 0xFF, 0xFE, 0, true, false);
        GbEb(0x12, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
        GbEb(0x12, 1, 2, 4, CPU.CF, false, false);
        GbEb(0x12, 0xFF, 1, 1, CPU.CF, true, false);
        GbEb(0x12, 1, 0xFF, 1, CPU.CF, true, false);
        GbEb(0x12, 0, 0, 1, CPU.CF, false, false);
        GbEb(0x12, 0xFF, 0xFF, 0xFF, CPU.CF, true, false);
        GbEb(0x12, 64, 64, 129, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x212() {
        cpu.big = true;
        GbEb(0x12, 1, 2, 3, 0, false, false);
        GbEb(0x12, 0xFF, 1, 0, 0, true, false);
        GbEb(0x12, 1, 0xFF, 0, 0, true, false);
        GbEb(0x12, 0, 0, 0, 0, false, false);
        GbEb(0x12, 0xFF, 0xFF, 0xFE, 0, true, false);
        GbEb(0x12, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
        GbEb(0x12, 1, 2, 4, CPU.CF, false, false);
        GbEb(0x12, 0xFF, 1, 1, CPU.CF, true, false);
        GbEb(0x12, 1, 0xFF, 1, CPU.CF, true, false);
        GbEb(0x12, 0, 0, 1, CPU.CF, false, false);
        GbEb(0x12, 0xFF, 0xFF, 0xFF, CPU.CF, true, false);
        GbEb(0x12, 64, 64, 129, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x013() {
        cpu.big = false;
        GwEw(0x13, 1000, 2002, 3002, 0, false, false);
        GwEw(0x13, 0xFFFF, 1, 0, 0, true, false);
        GwEw(0x13, 1, 0xFFFF, 0, 0, true, false);
        GwEw(0x13, 0, 0, 0, 0, false, false);
        GwEw(0x13, 0xFFFF, 0xFFFF, 0xFFFE, 0, true, false);
        GwEw(0x13, 16384, 16384, 32768, 0, false, true); // overflow indicates that the sign changed
        GwEw(0x13, 1000, 2002, 3003, CPU.CF, false, false);
        GwEw(0x13, 0xFFFF, 1, 1, CPU.CF, true, false);
        GwEw(0x13, 1, 0xFFFF, 1, CPU.CF, true, false);
        GwEw(0x13, 0, 0, 1, CPU.CF, false, false);
        GwEw(0x13, 0xFFFF, 0xFFFF, 0xFFFF, CPU.CF, true, false);
        GwEw(0x13, 16384, 16384, 32769, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x213() {
        cpu.big = true;
        GdEd(0x13, 100000, 200200, 300200, 0, false, false);
        GdEd(0x13, 0xFFFFFFFF, 1, 0, 0, true, false);
        GdEd(0x13, 1, 0xFFFFFFFF, 0, 0, true, false);
        GdEd(0x13, 0, 0, 0, 0, false, false);
        GdEd(0x13, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, true, false);
        GdEd(0x13, 0x40000000, 0x40000000, 0x80000000, 0, false, true); // overflow indicates that the sign changed
        GdEd(0x13, 100000, 200200, 300201, CPU.CF, false, false);
        GdEd(0x13, 0xFFFFFFFF, 1, 1, CPU.CF, true, false);
        GdEd(0x13, 1, 0xFFFFFFFF, 1, CPU.CF, true, false);
        GdEd(0x13, 0, 0, 1, CPU.CF, false, false);
        GdEd(0x13, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, CPU.CF, true, false);
        GdEd(0x13, 0x40000000, 0x40000000, 0x80000001, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x014() {
        cpu.big = false;
        AlIb(0x14, 1, 2, 3, 0, false, false);
        AlIb(0x14, 0xFF, 1, 0, 0, true, false);
        AlIb(0x14, 1, 0xFF, 0, 0, true, false);
        AlIb(0x14, 0, 0, 0, 0, false, false);
        AlIb(0x14, 0xFF, 0xFF, 0xFE, 0, true, false);
        AlIb(0x14, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
        AlIb(0x14, 1, 2, 4, CPU.CF, false, false);
        AlIb(0x14, 0xFF, 1, 1, CPU.CF, true, false);
        AlIb(0x14, 1, 0xFF, 1, CPU.CF, true, false);
        AlIb(0x14, 0, 0, 1, CPU.CF, false, false);
        AlIb(0x14, 0xFF, 0xFF, 0xFF, CPU.CF, true, false);
        AlIb(0x14, 64, 64, 129, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x214() {
        cpu.big = true;
        AlIb(0x14, 1, 2, 3, 0, false, false);
        AlIb(0x14, 0xFF, 1, 0, 0, true, false);
        AlIb(0x14, 1, 0xFF, 0, 0, true, false);
        AlIb(0x14, 0, 0, 0, 0, false, false);
        AlIb(0x14, 0xFF, 0xFF, 0xFE, 0, true, false);
        AlIb(0x14, 64, 64, 128, 0, false, true); // overflow indicates that the sign changed
        AlIb(0x14, 1, 2, 4, CPU.CF, false, false);
        AlIb(0x14, 0xFF, 1, 1, CPU.CF, true, false);
        AlIb(0x14, 1, 0xFF, 1, CPU.CF, true, false);
        AlIb(0x14, 0, 0, 1, CPU.CF, false, false);
        AlIb(0x14, 0xFF, 0xFF, 0xFF, CPU.CF, true, false);
        AlIb(0x14, 64, 64, 129, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x015() {
        cpu.big = false;
        AxIw(0x15, 1000, 2002, 3002, 0, false, false);
        AxIw(0x15, 0xFFFF, 1, 0, 0, true, false);
        AxIw(0x15, 1, 0xFFFF, 0, 0, true, false);
        AxIw(0x15, 0, 0, 0, 0, false, false);
        AxIw(0x15, 0xFFFF, 0xFFFF, 0xFFFE, 0, true, false);
        AxIw(0x15, 16384, 16384, 32768, 0, false, true); // overflow indicates that the sign changed
        AxIw(0x15, 1000, 2002, 3003, CPU.CF, false, false);
        AxIw(0x15, 0xFFFF, 1, 1, CPU.CF, true, false);
        AxIw(0x15, 1, 0xFFFF, 1, CPU.CF, true, false);
        AxIw(0x15, 0, 0, 1, CPU.CF, false, false);
        AxIw(0x15, 0xFFFF, 0xFFFF, 0xFFFF, CPU.CF, true, false);
        AxIw(0x15, 16384, 16384, 32769, CPU.CF, false, true); // overflow indicates that the sign changed
    }

    public void testAdc0x215() {
        cpu.big = true;
        EaxId(0x15, 100000, 200200, 300200, 0, false, false);
        EaxId(0x15, 0xFFFFFFFF, 1, 0, 0, true, false);
        EaxId(0x15, 1, 0xFFFFFFFF, 0, 0, true, false);
        EaxId(0x15, 0, 0, 0, 0, false, false);
        EaxId(0x15, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, true, false);
        EaxId(0x15, 0x40000000, 0x40000000, 0x80000000, 0, false, true); // overflow indicates that the sign changed
        EaxId(0x15, 100000, 200200, 300201, CPU.CF, false, false);
        EaxId(0x15, 0xFFFFFFFF, 1, 1, CPU.CF, true, false);
        EaxId(0x15, 1, 0xFFFFFFFF, 1, CPU.CF, true, false);
        EaxId(0x15, 0, 0, 1, CPU.CF, false, false);
        EaxId(0x15, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, CPU.CF, true, false);
        EaxId(0x15, 0x40000000, 0x40000000, 0x80000001, CPU.CF, false, true); // overflow indicates that the sign changed
    }
}

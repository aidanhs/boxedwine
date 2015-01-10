package wine.emulation;

import wine.system.Callback;
import wine.util.Log;

class Ops {
    final static class Instruction_Reg8 extends Op {
        final private Instruction instruction;
        final private int dest;

        public Instruction_Reg8(Instruction instruction, int dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u8(instruction.run(cpu, cpu.regs[dest].u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString8(dest);
        }
    }

    final static class Instruction_Reg8_Custom extends Op {
        final private Instruction instruction;
        final private int dest;

        public Instruction_Reg8_Custom(Instruction instruction, int dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            instruction.runCustom(cpu, cpu.regs[dest].u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString8(dest);
        }
    }

    final static class Instruction_Reg16 extends Op {
        final private Instruction instruction;
        final private int dest;

        public Instruction_Reg16(Instruction instruction, int dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(instruction.run(cpu, cpu.regs[dest].u16()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest);
        }
    }

    final static class Instruction_Reg16_Custom extends Op {
        final private Instruction instruction;
        final private int dest;

        public Instruction_Reg16_Custom(Instruction instruction, int dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            instruction.runCustom(cpu, cpu.regs[dest].u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest);
        }
    }

    final static class Instruction_Reg32 extends Op {
        final private Instruction instruction;
        final private int dest;

        public Instruction_Reg32(Instruction instruction, int dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=instruction.run(cpu, cpu.regs[dest].dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest);
        }
    }

    final static class Instruction_Reg32_Custom extends Op {
        final private Instruction instruction;
        final private int dest;

        public Instruction_Reg32_Custom(Instruction instruction, int dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            instruction.runCustom(cpu, cpu.regs[dest].dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest);
        }
    }

    final static class Instruction_Reg8_value extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg8_value(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u8(instruction.run(cpu, cpu.regs[dest].u8(), src));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString8(dest)+", "+src;
        }
    }

    final static class Instruction_Reg16_value extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg16_value(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(instruction.run(cpu, cpu.regs[dest].u16(), src));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest)+", "+src;
        }
    }

    final static class Instruction_Reg32_value extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg32_value(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=instruction.run(cpu, cpu.regs[dest].dword, src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest)+", "+src;
        }
    }

    final static class Instruction_Reg8_Reg8 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg8_Reg8(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u8(instruction.run(cpu, cpu.regs[dest].u8(), cpu.regs[src].u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString8(dest)+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Reg16_Reg8 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg16_Reg8(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(instruction.run(cpu, cpu.regs[dest].u16(), cpu.regs[src].u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest)+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Reg16_Reg16 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg16_Reg16(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(instruction.run(cpu, cpu.regs[dest].u16(), cpu.regs[src].u16()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest)+", "+CPU.regToString16(src);
        }
    }

    final static class Instruction_Reg32_Reg8 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg32_Reg8(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=instruction.run(cpu, cpu.regs[dest].dword, cpu.regs[src].u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest)+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Reg32_Reg32 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg32_Reg32(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=instruction.run(cpu, cpu.regs[dest].dword, cpu.regs[src].dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest)+", "+CPU.regToString(src);
        }
    }

    final static class Instruction_Reg16_Reg16_Value extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;
        final private int value;

        public Instruction_Reg16_Reg16_Value(Instruction instruction, int dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(instruction.run(cpu, cpu.regs[src].u16(), value));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest)+", "+CPU.regToString16(src)+", "+value;
        }
    }

    final static class Instruction_Reg32_Reg32_Value extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;
        final private int value;

        public Instruction_Reg32_Reg32_Value(Instruction instruction, int dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=instruction.run(cpu, cpu.regs[src].dword, value);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest)+", "+CPU.regToString(src)+", "+value;
        }
    }

    final static class Instruction_Reg16_Reg16_Value_3 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;
        final private int value;

        public Instruction_Reg16_Reg16_Value_3(Instruction instruction, int dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(instruction.run(cpu, cpu.regs[dest].u16(), cpu.regs[src].u16(), value));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest)+", "+CPU.regToString16(src)+", "+value;
        }
    }

    final static class Instruction_Reg32_Reg32_Reg8 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;
        final private int value;

        public Instruction_Reg32_Reg32_Reg8(Instruction instruction, int dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=instruction.run(cpu, cpu.regs[dest].dword, cpu.regs[src].dword, cpu.regs[value].u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest)+", "+CPU.regToString(src)+", "+CPU.regToString8(value);
        }
    }

    final static class Instruction_Reg16_Reg16_Reg8 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;
        final private int value;

        public Instruction_Reg16_Reg16_Reg8(Instruction instruction, int dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(instruction.run(cpu, cpu.regs[dest].u16(), cpu.regs[src].u16(), cpu.regs[value].u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest)+", "+CPU.regToString16(src)+", "+CPU.regToString8(value);
        }
    }

    final static class Instruction_Reg32_Reg32_Value_3 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;
        final private int value;

        public Instruction_Reg32_Reg32_Value_3(Instruction instruction, int dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=instruction.run(cpu, cpu.regs[dest].dword, cpu.regs[src].dword, value);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest)+", "+CPU.regToString(src)+", "+value;
        }
    }

    final static class Instruction_Mem8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;

        public Instruction_Mem8(Instruction instruction, EaaBase dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writeb(address, instruction.run(cpu, memory.readb(address)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString8();
        }
    }

    final static class Instruction_Mem8_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;

        public Instruction_Mem8_Lock(Instruction instruction, EaaBase dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writeb(address, instruction.run(cpu, memory.readb(address)));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString8();
        }
    }

    final static class Instruction_Mem8_Custom extends Op {
        final private Instruction instruction;
        final private EaaBase dest;

        public Instruction_Mem8_Custom(Instruction instruction, EaaBase dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            instruction.runCustom(cpu, cpu.memory.readb(dest.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString8();
        }
    }

    final static class Instruction_Mem8_value_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem8_value_Lock(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writeb(address, instruction.run(cpu, memory.readb(address), src));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString8()+", "+src;
        }
    }

    final static class Instruction_Mem8_value extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem8_value(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writeb(address, instruction.run(cpu, memory.readb(address), src));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString8()+", "+src;
        }
    }

    final static class Instruction_Mem16 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;

        public Instruction_Mem16(Instruction instruction, EaaBase dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writew(address, instruction.run(cpu, memory.readw(address)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16();
        }
    }

    final static class Instruction_Mem16_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;

        public Instruction_Mem16_Lock(Instruction instruction, EaaBase dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writew(address, instruction.run(cpu, memory.readw(address)));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString16();
        }
    }

    final static class Instruction_Mem16_Custom extends Op {
        final private Instruction instruction;
        final private EaaBase dest;

        public Instruction_Mem16_Custom(Instruction instruction, EaaBase dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            instruction.runCustom(cpu, cpu.memory.readw(dest.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16();
        }
    }

    final static class Instruction_Mem16_value extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem16_value(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writew(address, instruction.run(cpu, memory.readw(address), src));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+src;
        }
    }

    final static class Instruction_Mem16_value_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem16_value_Lock(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writew(address, instruction.run(cpu, memory.readw(address), src));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString16()+", "+src;
        }
    }

    final static class Instruction_Mem32 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;

        public Instruction_Mem32(Instruction instruction, EaaBase dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writed(address, instruction.run(cpu, memory.readd(address)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32();
        }
    }

    final static class Instruction_Mem32_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;

        public Instruction_Mem32_Lock(Instruction instruction, EaaBase dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writed(address, instruction.run(cpu, memory.readd(address)));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString32();
        }
    }

    final static class Instruction_Mem32_Custom extends Op {
        final private Instruction instruction;
        final private EaaBase dest;

        public Instruction_Mem32_Custom(Instruction instruction, EaaBase dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            instruction.runCustom(cpu, cpu.memory.readd(dest.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32();
        }
    }

    final static class Instruction_Mem32_value extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem32_value(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writed(address, instruction.run(cpu, memory.readd(address), src));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+src;
        }
    }

    final static class Instruction_Mem32_value_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem32_value_Lock(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writed(address, instruction.run(cpu, memory.readd(address), src));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString32()+", "+src;
        }
    }

    final static class Instruction_Mem8_Reg8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem8_Reg8(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writeb(address, instruction.run(cpu, memory.readb(address), cpu.regs[src].u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString8()+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Mem8_Reg8_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem8_Reg8_Lock(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writeb(address, instruction.run(cpu, memory.readb(address), cpu.regs[src].u8()));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString8()+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Mem16_Reg8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem16_Reg8(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writew(address, instruction.run(cpu, memory.readw(address), cpu.regs[src].u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Mem16_Reg8_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem16_Reg8_Lock(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writew(address, instruction.run(cpu, memory.readw(address), cpu.regs[src].u8()));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString16()+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Mem16_Reg16 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem16_Reg16(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writew(address, instruction.run(cpu, memory.readw(address), cpu.regs[src].u16()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+CPU.regToString16(src);
        }
    }

    final static class Instruction_Mem16_Reg16_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem16_Reg16_Lock(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writew(address, instruction.run(cpu, memory.readw(address), cpu.regs[src].u16()));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString16()+", "+CPU.regToString16(src);
        }
    }

    final static class Instruction_Mem32_Reg8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem32_Reg8(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writed(address, instruction.run(cpu, memory.readd(address), cpu.regs[src].u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Mem32_Reg8_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem32_Reg8_Lock(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writed(address, instruction.run(cpu, memory.readd(address), cpu.regs[src].u8()));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString32()+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Mem32_Reg32 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem32_Reg32(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writed(address, instruction.run(cpu, memory.readd(address), cpu.regs[src].dword));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+CPU.regToString(src);
        }
    }

    final static class Instruction_Mem32_Reg32_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem32_Reg32_Lock(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writed(address, instruction.run(cpu, memory.readd(address), cpu.regs[src].dword));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString32()+", "+CPU.regToString(src);
        }
    }

    final static class Instruction_Mem16_Reg16_Reg8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;
        final private int value;

        public Instruction_Mem16_Reg16_Reg8(Instruction instruction, EaaBase dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writew(eaa, instruction.run(cpu, memory.readw(eaa), cpu.regs[src].u16(), cpu.regs[value].u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+CPU.regToString16(src)+", "+CPU.regToString8(value);
        }
    }

    final static class Instruction_Mem16_Reg16_Reg8_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;
        final private int value;

        public Instruction_Mem16_Reg16_Reg8_Lock(Instruction instruction, EaaBase dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writew(eaa, instruction.run(cpu, memory.readw(eaa), cpu.regs[src].u16(), cpu.regs[value].u8()));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString16()+", "+CPU.regToString16(src)+", "+CPU.regToString8(value);
        }
    }

    final static class Instruction_Mem32_Reg32_Reg8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;
        final private int value;

        public Instruction_Mem32_Reg32_Reg8(Instruction instruction, EaaBase dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writed(eaa, instruction.run(cpu, memory.readd(eaa), cpu.regs[src].dword, cpu.regs[value].u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+CPU.regToString(src)+", "+CPU.regToString8(value);
        }
    }

    final static class Instruction_Mem32_Reg32_Reg8_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;
        final private int value;

        public Instruction_Mem32_Reg32_Reg8_Lock(Instruction instruction, EaaBase dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writed(eaa, instruction.run(cpu, memory.readd(eaa), cpu.regs[src].dword, cpu.regs[value].u8()));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString32()+", "+CPU.regToString(src)+", "+CPU.regToString8(value);
        }
    }

    final static class Instruction_Mem16_Reg16_Value_3 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;
        final private int value;

        public Instruction_Mem16_Reg16_Value_3(Instruction instruction, EaaBase dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writew(eaa, instruction.run(cpu, memory.readw(eaa), cpu.regs[src].u16(), value));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+CPU.regToString16(src)+", "+value;
        }
    }

    final static class Instruction_Mem16_Reg16_Value_3_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;
        final private int value;

        public Instruction_Mem16_Reg16_Value_3_Lock(Instruction instruction, EaaBase dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writew(eaa, instruction.run(cpu, memory.readw(eaa), cpu.regs[src].u16(), value));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString16()+", "+CPU.regToString16(src)+", "+value;
        }
    }

    final static class Instruction_Mem32_Reg32_Value_3 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;
        final private int value;

        public Instruction_Mem32_Reg32_Value_3(Instruction instruction, EaaBase dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.writed(eaa, instruction.run(cpu, memory.readd(eaa), cpu.regs[src].dword, value));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+CPU.regToString(src)+", "+value;
        }
    }

    final static class Instruction_Mem32_Reg32_Value_3_Lock extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;
        final private int value;

        public Instruction_Mem32_Reg32_Value_3_Lock(Instruction instruction, EaaBase dest, int src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            memory.writed(eaa, instruction.run(cpu, memory.readd(eaa), cpu.regs[src].dword, value));
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock "+instruction.toString()+" "+dest.toString32()+", "+CPU.regToString(src)+", "+value;
        }
    }

    final static class Instruction_Reg8_Mem8 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private EaaBase src;

        public Instruction_Reg8_Mem8(Instruction instruction, int dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u8(instruction.run(cpu, cpu.regs[dest].u8(), cpu.memory.readb(src.call(cpu))));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString8(dest)+", "+src.toString8();
        }
    }

    final static class Instruction_Reg16_Mem16 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private EaaBase src;

        public Instruction_Reg16_Mem16(Instruction instruction, int dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(instruction.run(cpu, cpu.regs[dest].u16(), cpu.memory.readw(src.call(cpu))));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest)+", "+src.toString16();
        }
    }

    final static class Instruction_Reg32_Mem32 extends Op {
        final private Instruction instruction;
        final private int dest;
        final private EaaBase src;

        public Instruction_Reg32_Mem32(Instruction instruction, int dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=instruction.run(cpu, cpu.regs[dest].dword, cpu.memory.readd(src.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest)+", "+src.toString32();
        }
    }

    final static class Instruction_Reg16_Mem16_Value extends Op {
        final private Instruction instruction;
        final private int dest;
        final private EaaBase src;
        final private int value;

        public Instruction_Reg16_Mem16_Value(Instruction instruction, int dest, EaaBase src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(instruction.run(cpu, cpu.memory.readw(src.call(cpu)), value));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest)+", "+src.toString16()+", "+value;
        }
    }

    final static class Instruction_Reg32_Mem32_Value extends Op {
        final private Instruction instruction;
        final private int dest;
        final private EaaBase src;
        final private int value;

        public Instruction_Reg32_Mem32_Value(Instruction instruction, int dest, EaaBase src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=instruction.run(cpu, cpu.memory.readd(src.call(cpu)), value);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest)+", "+src.toString32()+", "+value;
        }
    }

    final static class Instruction_Reg8_noresult extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg8_noresult(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.regs[dest].u8(), src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString8(dest)+", "+src;
        }
    }

    final static class Instruction_Reg16_noresult extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg16_noresult(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.regs[dest].u16(), src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest)+", "+src;
        }
    }

    final static class Instruction_Reg32_noresult extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg32_noresult(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.regs[dest].dword, src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest)+", "+src;
        }
    }

    final static class Instruction_Reg8_Reg8_noresult extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg8_Reg8_noresult(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.regs[dest].u8(), cpu.regs[src].u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString8(dest)+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Reg16_Reg16_noresult extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg16_Reg16_noresult(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.regs[dest].u16(), cpu.regs[src].u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString16(dest)+", "+CPU.regToString16(src);
        }
    }

    final static class Instruction_Reg32_Reg32_noresult extends Op {
        final private Instruction instruction;
        final private int dest;
        final private int src;

        public Instruction_Reg32_Reg32_noresult(Instruction instruction, int dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.regs[dest].dword, cpu.regs[src].dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+CPU.regToString(dest)+", "+CPU.regToString(src);
        }
    }

    final static class Instruction_Mem8_noresult extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem8_noresult(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.memory.readb(dest.call(cpu)), src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.noResultString()+" "+dest.toString8()+", "+src;
        }
    }

    final static class Instruction_Mem16_noresult extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem16_noresult(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.memory.readw(dest.call(cpu)), src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.noResultString()+" "+dest.toString16()+", "+src;
        }
    }

    final static class Instruction_Mem32_noresult extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem32_noresult(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.memory.readd(dest.call(cpu)), src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.noResultString()+" "+dest.toString32()+", "+src;
        }
    }

    final static class Instruction_Mem8_Reg8_noresult extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem8_Reg8_noresult(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.memory.readb(dest.call(cpu)), cpu.regs[src].u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.noResultString()+" "+dest.toString8()+", "+CPU.regToString8(src);
        }
    }

    final static class Instruction_Mem16_Reg16_noresult extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem16_Reg16_noresult(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.memory.readw(dest.call(cpu)), cpu.regs[src].u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.noResultString()+" "+dest.toString16()+", "+CPU.regToString16(src);
        }
    }

    final static class Instruction_Mem32_Reg32_noresult extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private int src;

        public Instruction_Mem32_Reg32_noresult(Instruction instruction, EaaBase dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.memory.readd(dest.call(cpu)), cpu.regs[src].dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.noResultString()+" "+dest.toString32()+", "+CPU.regToString(src);
        }
    }

    final static class Instruction_Reg8_Mem8_noresult extends Op {
        final private Instruction instruction;
        final private int dest;
        final private EaaBase src;

        public Instruction_Reg8_Mem8_noresult(Instruction instruction, int dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.regs[dest].u8(), cpu.memory.readb(src.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.noResultString()+" "+CPU.regToString8(dest)+", "+src.toString8();
        }
    }

    final static class Instruction_Reg16_Mem16_noresult extends Op {
        final private Instruction instruction;
        final private int dest;
        final private EaaBase src;

        public Instruction_Reg16_Mem16_noresult(Instruction instruction, int dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.regs[dest].u16(), cpu.memory.readw(src.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.noResultString()+" "+CPU.regToString16(dest)+", "+src.toString16();
        }
    }

    final static class Instruction_Reg32_Mem32_noresult extends Op {
        final private Instruction instruction;
        final private int dest;
        final private EaaBase src;

        public Instruction_Reg32_Mem32_noresult(Instruction instruction, int dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.regs[dest].dword, cpu.memory.readd(src.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.noResultString()+" "+CPU.regToString(dest)+", "+src.toString32();
        }
    }

    final static class PushReg16 extends Op {
        final private int src;

        public PushReg16(int src) {
            this.src = src;
        }

        public Block call(CPU cpu) {
            cpu.push16(cpu.regs[src].u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+CPU.regToString16(src);
        }
    }

    final static class PushReg32 extends Op {
        final private int src;

        public PushReg32(int src) {
            this.src = src;
        }

        public Block call(CPU cpu) {
            cpu.push32(cpu.regs[src].dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+CPU.regToString(src);
        }
    }

    final static class PopReg16 extends Op {
        final private int dest;

        public PopReg16(int dest) {
            this.dest = dest;
        }

        public Block call(CPU cpu) {
            cpu.regs[dest].u16(cpu.pop16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pop "+CPU.regToString16(dest);
        }
    }

    final static class PopReg32 extends Op {
        final private int dest;

        public PopReg32(int dest) {
            this.dest = dest;
        }

        public Block call(CPU cpu) {
            cpu.regs[dest].dword=cpu.pop32();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pop "+CPU.regToString(dest);
        }
    }

    final static class PopMem16 extends Op {
        final private EaaBase dest;

        public PopMem16(EaaBase dest) {
            this.dest = dest;
        }

        public Block call(CPU cpu) {
            cpu.memory.writew(dest.call(cpu), cpu.pop16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pop "+dest.toString16();
        }
    }

    final static class PopMem32 extends Op {
        final private EaaBase dest;

        public PopMem32(EaaBase dest) {
            this.dest = dest;
        }

        public Block call(CPU cpu) {
            cpu.memory.writed(dest.call(cpu), cpu.pop32());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pop "+dest.toString32();
        }
    }

    final static class PushValue16 extends Op {
        final private int src;

        public PushValue16(int src) {
            this.src = src;
        }

        public Block call(CPU cpu) {
            cpu.push16(src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+src;
        }
    }

    final static class PushValue32 extends Op {
        final private int src;

        public PushValue32(int src) {
            this.src = src;
        }

        public Block call(CPU cpu) {
            cpu.push32(src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+src;
        }
    }

    static public class IRet extends JumpOp {
        public Block call(CPU cpu) {
            Log.panic("iret not implemented yet");
            cpu.iret(true, cpu.eip + block.eip_count);
            return null;
        }
    }

    abstract static class JumpOp extends Op {
        public Block block;
        final public int offset;

        public JumpOp() {
            offset = 0;
        }

        public JumpOp(int offset) {
            this.offset = offset;
        }

        public Block jump(CPU cpu, boolean condition) {
            if (condition) {
                cpu.eip = cpu.eip + offset + block.eip_count;
                if (!cpu.big)
                    cpu.eip &= 0xFFFF;
                if (block.link1 == null)
                    block.link1 = cpu.getBlock(cpu.eip, cpu.cs.dword);
                return block.link1;
            }
            cpu.eip = cpu.eip + block.eip_count;
            if (!cpu.big)
                cpu.eip &= 0xFFFF;
            if (block.link2 == null)
                block.link2 = cpu.getBlock(cpu.eip, cpu.cs.dword);
            return block.link2;
        }
    }
    final static class Jump extends JumpOp {
        final private Condition cond;

        public Jump(Condition cond, int offset) {
            super(offset);
            this.cond = cond;
        }

        public Block call(CPU cpu) {
            return jump(cpu, cond.test(cpu));
        }
        public String toString() {
            return "j"+cond.toString()+" "+Integer.toHexString(offset+block.eip_count);
        }
    }

    final static class LeaGw extends Op {
        final private EaaBase eaa;
        final private int dest;

        public LeaGw(EaaBase eaa, int dest) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lea "+CPU.regToString16(dest)+", "+eaa.toString();
        }
    }

    final static class LeaGd extends Op {
        final private EaaBase eaa;
        final private int dest;

        public LeaGd(EaaBase eaa, int dest) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=eaa.call(cpu);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lea "+CPU.regToString(dest)+", "+eaa.toString();
        }
    }

    final static class PushA16 extends Op {
        public Block call(CPU cpu) {
            int sp = cpu.esp.u16();
            cpu.push16(cpu.eax.u16());
            cpu.push16(cpu.ecx.u16());
            cpu.push16(cpu.edx.u16());
            cpu.push16(cpu.ebx.u16());
            cpu.push16(sp);
            cpu.push16(cpu.ebp.u16());
            cpu.push16(cpu.esi.u16());
            cpu.push16(cpu.edi.u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pusha";
        }
    }

    final static class PushA32 extends Op {
        public Block call(CPU cpu) {
            int sp = cpu.esp.dword;
            cpu.push32(cpu.eax.dword);
            cpu.push32(cpu.ecx.dword);
            cpu.push32(cpu.edx.dword);
            cpu.push32(cpu.ebx.dword);
            cpu.push32(sp);
            cpu.push32(cpu.ebp.dword);
            cpu.push32(cpu.esi.dword);
            cpu.push32(cpu.edi.dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pusha";
        }
    }

    final static class PopA16 extends Op {
        public Block call(CPU cpu) {
            cpu.eax.u16(cpu.pop16());
            cpu.ecx.u16(cpu.pop16());
            cpu.edx.u16(cpu.pop16());
            cpu.ebx.u16(cpu.pop16());
            cpu.pop16();
            cpu.ebp.u16(cpu.pop16());
            cpu.esi.u16(cpu.pop16());
            cpu.edi.u16(cpu.pop16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "popa";
        }
    }

    final static class PopA32 extends Op {
        public Block call(CPU cpu) {
            cpu.eax.dword=cpu.pop32();
            cpu.ecx.dword=cpu.pop32();
            cpu.edx.dword=cpu.pop32();
            cpu.ebx.dword=cpu.pop32();
            cpu.pop32();
            cpu.ebp.dword=cpu.pop32();
            cpu.esi.dword=cpu.pop32();
            cpu.edi.dword=cpu.pop32();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "popa";
        }
    }

    final static class Exchange_Reg8_Reg8 extends Op {
        final private int reg1;
        final private int reg2;

        public Exchange_Reg8_Reg8(int reg1, int reg2) {
            this.reg1 = reg1;
            this.reg2 = reg2;
        }
        public Block call(CPU cpu) {
            int tmp = cpu.regs[reg1].u8();
            cpu.regs[reg1].u8(cpu.regs[reg2].u8());
            cpu.regs[reg2].u8(tmp);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xchg "+CPU.regToString8(reg1)+", "+CPU.regToString8(reg2);
        }
    }

    final static class Exchange_Mem8_Reg8_Lock extends Op {
        final private EaaBase eaa;
        final private int reg;

        public Exchange_Mem8_Reg8_Lock(EaaBase eaa, int reg) {
            this.eaa = eaa;
            this.reg = reg;
        }
        public Block call(CPU cpu) {
            int addresss = eaa.call(cpu);
            int tmp = cpu.regs[reg].u8();
            Memory memory = cpu.memory;
            memory.lock();
            cpu.regs[reg].u8(memory.readb(addresss));
            memory.writeb(addresss, tmp);
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock xchg "+eaa.toString8()+", "+CPU.regToString8(reg);
        }
    }

    final static class Exchange_Reg16_Reg16 extends Op {
        final private int reg1;
        final private int reg2;

        public Exchange_Reg16_Reg16(int reg1, int reg2) {
            this.reg1 = reg1;
            this.reg2 = reg2;
        }
        public Block call(CPU cpu) {
            int tmp = cpu.regs[reg1].u16();
            cpu.regs[reg1].u16(cpu.regs[reg2].u16());
            cpu.regs[reg2].u16(tmp);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xchg "+CPU.regToString16(reg1)+", "+CPU.regToString16(reg2);
        }
    }

    final static class Exchange_Mem16_Reg16_Lock extends Op {
        final private EaaBase eaa;
        final private int reg;

        public Exchange_Mem16_Reg16_Lock(EaaBase eaa, int reg) {
            this.eaa = eaa;
            this.reg = reg;
        }
        public Block call(CPU cpu) {
            int addresss = eaa.call(cpu);
            int tmp = cpu.regs[reg].u16();
            Memory memory = cpu.memory;
            memory.lock();
            cpu.regs[reg].u16(memory.readw(addresss));
            memory.writew(addresss, tmp);
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock xchg "+eaa.toString16()+", "+CPU.regToString16(reg);
        }
    }

    final static class Exchange_Reg32_Reg32 extends Op {
        final private int reg1;
        final private int reg2;

        public Exchange_Reg32_Reg32(int reg1, int reg2) {
            this.reg1 = reg1;
            this.reg2 = reg2;
        }
        public Block call(CPU cpu) {
            int tmp = cpu.regs[reg1].dword;
            cpu.regs[reg1].dword=cpu.regs[reg2].dword;
            cpu.regs[reg2].dword=tmp;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xchg "+CPU.regToString(reg1)+", "+CPU.regToString(reg2);
        }
    }

    final static class Exchange_Mem32_Reg32_Lock extends Op {
        final private EaaBase eaa;
        final private int reg;

        public Exchange_Mem32_Reg32_Lock(EaaBase eaa, int reg) {
            this.eaa = eaa;
            this.reg = reg;
        }
        public Block call(CPU cpu) {
            int addresss = eaa.call(cpu);
            int tmp = cpu.regs[reg].dword;
            Memory memory = cpu.memory;
            memory.lock();
            cpu.regs[reg].dword=memory.readd(addresss);
            memory.writed(addresss, tmp);
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock xchg "+eaa.toString32()+", "+CPU.regToString(reg);
        }
    }

    final static class Mov_Reg8 extends Op {
        final private int dest;
        final private int value;

        public Mov_Reg8(int dest, int value) {
            this.dest = dest;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u8(value);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString8(dest)+", "+value;
        }
    }

    final static class Mov_Reg8_Reg8 extends Op {
        final private int dest;
        final private int src;

        public Mov_Reg8_Reg8(int dest, int src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u8(cpu.regs[src].u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString8(dest)+", "+CPU.regToString8(src);
        }
    }

    final static class Mov_Mem8 extends Op {
        final private EaaBase eaa;
        final private int src;

        public Mov_Mem8(EaaBase eaa, int src) {
            this.eaa = eaa;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.memory.writeb(eaa.call(cpu), src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString8()+", "+src;
        }
    }

    final static class Mov_Mem8_Reg8 extends Op {
        final private EaaBase eaa;
        final private int src;

        public Mov_Mem8_Reg8(EaaBase eaa, int src) {
            this.eaa = eaa;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.memory.writeb(eaa.call(cpu), cpu.regs[src].u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString8()+", "+CPU.regToString8(src);
        }
    }

    final static class Mov_Reg8_Mem8 extends Op {
        final private EaaBase eaa;
        final private int dest;

        public Mov_Reg8_Mem8(int dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u8(cpu.memory.readb(eaa.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString8(dest)+", "+eaa.toString8();
        }
    }

    final static class Mov_Reg16 extends Op {
        final private int dest;
        final private int value;

        public Mov_Reg16(int dest, int value) {
            this.dest = dest;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(value);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString16(dest)+", "+value;
        }
    }

    final static class Mov_Reg16_Reg8 extends Op {
        final private int dest;
        final private int src;

        public Mov_Reg16_Reg8(int dest, int src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(cpu.regs[src].u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(dest)+", "+CPU.regToString8(src);
        }
    }

    final static class Mov_Reg16_Reg8_sx extends Op {
        final private int dest;
        final private int src;

        public Mov_Reg16_Reg8_sx(int dest, int src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16((byte)cpu.regs[src].u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+CPU.regToString16(dest)+", "+CPU.regToString8(src);
        }
    }

    final static class Mov_Reg16_Reg16 extends Op {
        final private int dest;
        final private int src;

        public Mov_Reg16_Reg16(int dest, int src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(cpu.regs[src].u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString16(dest)+", "+CPU.regToString16(src);
        }
    }

    final static class Mov_Mem16 extends Op {
        final private EaaBase eaa;
        final private int src;

        public Mov_Mem16(EaaBase eaa, int src) {
            this.eaa = eaa;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.memory.writew(eaa.call(cpu), src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString16()+", "+src;
        }
    }

    final static class Mov_Mem16_Reg16 extends Op {
        final private EaaBase eaa;
        final private int src;

        public Mov_Mem16_Reg16(EaaBase eaa, int src) {
            this.eaa = eaa;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.memory.writew(eaa.call(cpu), cpu.regs[src].u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString16()+", "+CPU.regToString16(src);
        }
    }

    final static class Mov_Reg16_Mem8 extends Op {
        final private EaaBase eaa;
        final private int dest;

        public Mov_Reg16_Mem8(int dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(cpu.memory.readb(eaa.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(dest)+", "+eaa.toString8();
        }
    }

    final static class Mov_Reg16_Mem8_sx extends Op {
        final private EaaBase eaa;
        final private int dest;

        public Mov_Reg16_Mem8_sx(int dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16((byte)cpu.memory.readb(eaa.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+CPU.regToString16(dest)+", "+eaa.toString8();
        }
    }

    final static class Mov_Reg16_Mem16 extends Op {
        final private EaaBase eaa;
        final private int dest;

        public Mov_Reg16_Mem16(int dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(cpu.memory.readw(eaa.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString16(dest)+", "+eaa.toString16();
        }
    }

    final static class Mov_Reg32 extends Op {
        final private int dest;
        final private int value;

        public Mov_Reg32(int dest, int value) {
            this.dest = dest;
            this.value = value;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=value;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(dest)+", "+value;
        }
    }

    final static class Mov_Reg32_Reg8 extends Op {
        final private int dest;
        final private int src;

        public Mov_Reg32_Reg8(int dest, int src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=cpu.regs[src].u8();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(dest)+", "+CPU.regToString8(src);
        }
    }

    final static class Mov_Reg32_Reg8_sx extends Op {
        final private int dest;
        final private int src;

        public Mov_Reg32_Reg8_sx(int dest, int src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=(byte)cpu.regs[src].u8();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+CPU.regToString(dest)+", "+CPU.regToString8(src);
        }
    }

    final static class Mov_Reg32_Reg16 extends Op {
        final private int dest;
        final private int src;

        public Mov_Reg32_Reg16(int dest, int src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=cpu.regs[src].u16();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(dest)+", "+CPU.regToString16(src);
        }
    }

    final static class Mov_Reg32_Reg16_sx extends Op {
        final private int dest;
        final private int src;

        public Mov_Reg32_Reg16_sx(int dest, int src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=(short)cpu.regs[src].u16();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+CPU.regToString(dest)+", "+CPU.regToString16(src);
        }
    }

    final static class Mov_Reg32_Reg32 extends Op {
        final private int dest;
        final private int src;

        public Mov_Reg32_Reg32(int dest, int src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=cpu.regs[src].dword;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(dest)+", "+CPU.regToString(src);
        }
    }

    final static class Mov_Mem32 extends Op {
        final private EaaBase eaa;
        final private int src;

        public Mov_Mem32(EaaBase eaa, int src) {
            this.eaa = eaa;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.memory.writed(eaa.call(cpu), src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString32()+", "+src;
        }
    }

    final static class Mov_Mem32_Reg32 extends Op {
        final private EaaBase eaa;
        final private int src;

        public Mov_Mem32_Reg32(EaaBase eaa, int src) {
            this.eaa = eaa;
            this.src = src;
        }
        public Block call(CPU cpu) {
            cpu.memory.writed(eaa.call(cpu), cpu.regs[src].dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString32()+", "+CPU.regToString(src);
        }
    }

    final static class Mov_Reg32_Mem8 extends Op {
        final private EaaBase eaa;
        final private int dest;

        public Mov_Reg32_Mem8(int dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=cpu.memory.readb(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(dest)+", "+eaa.toString8();
        }
    }

    final static class Mov_Reg32_Mem8_sx extends Op {
        final private EaaBase eaa;
        final private int dest;

        public Mov_Reg32_Mem8_sx(int dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=(byte)cpu.memory.readb(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+CPU.regToString(dest)+", "+eaa.toString8();
        }
    }

    final static class Mov_Reg32_Mem16 extends Op {
        final private EaaBase eaa;
        final private int dest;

        public Mov_Reg32_Mem16(int dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=cpu.memory.readw(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(dest)+", "+eaa.toString16();
        }
    }

    final static class Mov_Reg32_Mem16_sx extends Op {
        final private EaaBase eaa;
        final private int dest;

        public Mov_Reg32_Mem16_sx(int dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=(short)cpu.memory.readw(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+CPU.regToString(dest)+", "+eaa.toString16();
        }
    }

    final static class Mov_Reg32_Mem32 extends Op {
        final private EaaBase eaa;
        final private int dest;

        public Mov_Reg32_Mem32(int dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=cpu.memory.readd(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(dest)+", "+eaa.toString32();
        }
    }

    final static class Cbw extends Op {
        public Block call(CPU cpu) {
            cpu.eax.u16(((byte)cpu.eax.u8()) & 0xFFFF);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "Cbw";
        }
    }

    final static class Cwde extends Op {
        public Block call(CPU cpu) {
            cpu.eax.dword=(short)cpu.eax.u16();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "Cwde";
        }
    }

    final static class Cwd extends Op {
        public Block call(CPU cpu) {
            if ((cpu.eax.u16() & 0x8000)!=0) cpu.edx.u16(0xffff);else cpu.edx.u16(0);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "Cwd";
        }
    }

    final static class Cdq extends Op {
        public Block call(CPU cpu) {
            if (cpu.eax.dword<0) cpu.edx.dword=0xffffffff; else cpu.edx.dword=0;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "Cdq";
        }
    }

    final static class PushF16 extends Op {
        public Block call(CPU cpu) {
            cpu.fillFlags();
            cpu.push16(cpu.flags|1);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pushf";
        }
    }

    final static class PushF32 extends Op {
        public Block call(CPU cpu) {
            cpu.fillFlags();
            cpu.push32((cpu.flags|1) & 0xFCFFFF);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pushf";
        }
    }

    final static class PopF16 extends Op {
        public Block call(CPU cpu) {
            cpu.lazyFlags = null;
            cpu.flags=(cpu.flags & 0xFFFF0000) | cpu.pop16();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "popf";
        }
    }

    final static class PopF32 extends Op {
        public Block call(CPU cpu) {
            cpu.lazyFlags = null;
            cpu.flags=(cpu.flags & CPU.VM) | (cpu.pop32() & ~CPU.VM);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "popf";
        }
    }

    final static class Sahf extends Op {
        public Block call(CPU cpu) {
            int mask = CPU.SF|CPU.ZF|CPU.AF|CPU.PF|CPU.CF;
            cpu.flags=(cpu.flags & (0xFFFFFF00 | (~mask))) | (cpu.ah.u8() & mask);
            cpu.lazyFlags = null;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "sahf";
        }
    }

    final static class Lahf extends Op {
        public Block call(CPU cpu) {
            cpu.fillFlags();
            int mask = CPU.SF|CPU.ZF|CPU.AF|CPU.PF|CPU.CF;
            cpu.ah.u8((cpu.flags & mask)|1);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lahf";
        }
    }

    final static class Mov_Reg8_0b extends Op {
        final private int address;
        final private int dest;
        final private int base;

        public Mov_Reg8_0b(int dest, int base, int address) {
            this.address = address;
            this.dest = dest;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u8(cpu.memory.readb(cpu.regs[base].dword+address));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString8(dest)+", "+CPU.regToString(base)+":"+address;
        }
    }

    final static class Mov_Reg16_0w extends Op {
        final private int address;
        final private int dest;
        final private int base;

        public Mov_Reg16_0w(int dest, int base, int address) {
            this.address = address;
            this.dest = dest;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].u16(cpu.memory.readw(cpu.regs[base].dword + address));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString16(dest)+", "+CPU.regToString(base)+":"+address;
        }
    }

    final static class Mov_Reg32_0d extends Op {
        final private int address;
        final private int dest;
        final private int base;

        public Mov_Reg32_0d(int dest, int base, int address) {
            this.address = address;
            this.dest = dest;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.regs[dest].dword=cpu.memory.readd(cpu.regs[base].dword+address);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(dest)+", "+CPU.regToString(base)+":"+address;
        }
    }

    final static class Mov_0b_Reg8 extends Op {
        final private int address;
        final private int src;
        final private int base;

        public Mov_0b_Reg8(int base, int address, int src) {
            this.address = address;
            this.src = src;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writeb(cpu.regs[base].dword + this.address, cpu.regs[src].u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(base)+":"+address+", "+CPU.regToString8(src);
        }
    }

    final static class Mov_0w_Reg16 extends Op {
        final private int address;
        final private int src;
        final private int base;

        public Mov_0w_Reg16(int base, int address, int src) {
            this.address = address;
            this.src = src;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writew(cpu.regs[base].dword + this.address, cpu.regs[src].u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(base)+":"+address+", "+CPU.regToString16(src);
        }
    }

    final static class Mov_0d_Reg32 extends Op {
        final private int address;
        final private int src;
        final private int base;

        public Mov_0d_Reg32(int base, int address, int src) {
            this.address = address;
            this.src = src;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int address = cpu.regs[base].dword + this.address;
            cpu.memory.writed(address, cpu.regs[src].dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(base)+":"+address+", "+CPU.regToString(src);
        }
    }

    final static class Movsb32 extends Op {
        final private int add_index;
        final private int base;

        public Movsb32(int add_index, int base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writeb(cpu.es.dword + cpu.edi.dword, cpu.memory.readb(cpu.regs[base].dword + cpu.esi.dword));
            cpu.edi.dword+=add_index;
            cpu.esi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsb";
        }
    }

    final static class Movsb32_r extends Op {
        final private int add_index;
        final private int base;

        public Movsb32_r(int add_index, int base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.memory.writeb(cpu.es.dword + cpu.edi.dword, cpu.memory.readb(cpu.regs[base].dword + cpu.esi.dword));
                cpu.edi.dword += add_index;
                cpu.esi.dword += add_index;
                cpu.ecx.dword--;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep movsb";
        }
    }

    final static class Movsb16 extends Op {
        final private int add_index;
        final private int base;

        public Movsb16(int add_index, int base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writeb(cpu.es.dword + cpu.edi.u16(), cpu.memory.readb(cpu.regs[base].dword + cpu.esi.u16()));
            cpu.edi.u16(cpu.edi.u16()+add_index);
            cpu.esi.u16(cpu.esi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsb";
        }
    }

    final static class Movsb16_r extends Op {
        final private int add_index;
        final private int base;

        public Movsb16_r(int add_index, int base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.memory.writeb(cpu.es.dword + cpu.edi.dword, cpu.memory.readb(cpu.regs[base].dword + cpu.esi.dword));
                cpu.edi.u16(cpu.edi.u16()+add_index);
                cpu.esi.u16(cpu.esi.u16()+add_index);
                cpu.ecx.u16(cpu.ecx.u16()-1);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep movsb";
        }
    }

    final static class Movsw32 extends Op {
        final private int add_index;
        final private int base;

        public Movsw32(int add_index, int base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writew(cpu.es.dword + cpu.edi.dword, cpu.memory.readw(cpu.regs[base].dword + cpu.esi.dword));
            cpu.edi.dword+=add_index;
            cpu.esi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsw";
        }
    }

    final static class Movsw32_r extends Op {
        final private int add_index;
        final private int base;

        public Movsw32_r(int add_index, int base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.memory.writew(cpu.es.dword + cpu.edi.dword, cpu.memory.readw(cpu.regs[base].dword + cpu.esi.dword));
                cpu.edi.dword += add_index;
                cpu.esi.dword += add_index;
                cpu.ecx.dword--;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep movsw";
        }
    }

    final static class Movsw16 extends Op {
        final private int add_index;
        final private int base;

        public Movsw16(int add_index, int base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writew(cpu.es.dword + cpu.edi.u16(), cpu.memory.readw(cpu.regs[base].dword + cpu.esi.u16()));
            cpu.edi.u16(cpu.edi.u16()+add_index);
            cpu.esi.u16(cpu.esi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsw";
        }
    }

    final static class Movsw16_r extends Op {
        final private int add_index;
        final private int base;

        public Movsw16_r(int add_index, int base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.memory.writew(cpu.es.dword + cpu.edi.dword, cpu.memory.readw(cpu.regs[base].dword + cpu.esi.dword));
                cpu.edi.u16(cpu.edi.u16()+add_index);
                cpu.esi.u16(cpu.esi.u16()+add_index);
                cpu.ecx.u16(cpu.ecx.u16()-1);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep movsw";
        }
    }

    final static class Movsd32 extends Op {
        final private int add_index;
        final private int base;

        public Movsd32(int add_index, int base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writed(cpu.es.dword + cpu.edi.dword, cpu.memory.readd(cpu.regs[base].dword + cpu.esi.dword));
            cpu.edi.dword+=add_index;
            cpu.esi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsd";
        }
    }

    final static class Movsd32_r extends Op {
        final private int add_index;
        final private int base;

        public Movsd32_r(int add_index, int base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.memory.writed(cpu.es.dword + cpu.edi.dword, cpu.memory.readd(cpu.regs[base].dword + cpu.esi.dword));
                cpu.edi.dword += add_index;
                cpu.esi.dword += add_index;
                cpu.ecx.dword--;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep movsd";
        }
    }

    final static class Movsd16 extends Op {
        final private int add_index;
        final private int base;

        public Movsd16(int add_index, int base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writed(cpu.es.dword + cpu.edi.u16(), cpu.memory.readd(cpu.regs[base].dword + cpu.esi.u16()));
            cpu.edi.u16(cpu.edi.u16()+add_index);
            cpu.esi.u16(cpu.esi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsd";
        }
    }

    final static class Movsd16_r extends Op {
        final private int add_index;
        final private int base;

        public Movsd16_r(int add_index, int base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.memory.writed(cpu.es.dword + cpu.edi.dword, cpu.memory.readd(cpu.regs[base].dword + cpu.esi.dword));
                cpu.edi.u16(cpu.edi.u16()+add_index);
                cpu.esi.u16(cpu.esi.u16()+add_index);
                cpu.ecx.u16(cpu.ecx.u16()-1);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep movsd";
        }
    }

    final static class Cmpsb32 extends Op {
        final private int add_index;
        final private int base;

        public Cmpsb32(int add_index, int base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readb(cpu.regs[base].dword + cpu.esi.dword);
            int v2 = cpu.memory.readb(cpu.es.dword + cpu.edi.dword);
            cpu.edi.dword+=add_index;
            cpu.esi.dword+=add_index;
            Instructions.subb.run(cpu, v1, v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpsb";
        }
    }

    final static class Cmpsb32_r extends Op {
        final private int add_index;
        final private int base;
        final private boolean repeat_zero;

        public Cmpsb32_r(int add_index, int base, boolean repeat_zero) {
            this.add_index = add_index;
            this.base = base;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            if (count>0) {
                int v1=0;
                int v2=0;

                for (int i = 0; i < count; i++) {
                    v1 = cpu.memory.readb(cpu.regs[base].dword + cpu.esi.dword);
                    v2 = cpu.memory.readb(cpu.es.dword + cpu.edi.dword);
                    cpu.edi.dword += add_index;
                    cpu.esi.dword += add_index;
                    cpu.ecx.dword--;
                    if ((v1==v2)!=repeat_zero)
                        break;
                }
                Instructions.subb.run(cpu, v1, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz cmpsb";
            return "repnz cmpsb";
        }
    }

    final static class Cmpsb16 extends Op {
        final private int add_index;
        final private int base;

        public Cmpsb16(int add_index, int base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readb(cpu.regs[base].dword + cpu.esi.u16());
            int v2 = cpu.memory.readb(cpu.es.dword + cpu.edi.u16());
            cpu.edi.u16(cpu.edi.u16()+add_index);
            cpu.esi.u16(cpu.esi.u16()+add_index);
            Instructions.subb.run(cpu, v1, v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpsb";
        }
    }

    final static class Cmpsb16_r extends Op {
        final private int add_index;
        final private int base;
        final private boolean repeat_zero;

        public Cmpsb16_r(int add_index, int base, boolean repeat_zero) {
            this.add_index = add_index;
            this.base = base;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            if (count>0) {
                int v1=0;
                int v2=0;

                for (int i = 0; i < count; i++) {
                    v1 = cpu.memory.readb(cpu.regs[base].dword + cpu.esi.u16());
                    v2 = cpu.memory.readb(cpu.es.dword + cpu.edi.u16());

                    cpu.edi.u16(cpu.edi.u16() + add_index);
                    cpu.esi.u16(cpu.esi.u16() + add_index);
                    cpu.ecx.u16(cpu.ecx.u16() - 1);
                    if ((v1 == v2) != repeat_zero)
                        break;
                }
                Instructions.subb.run(cpu, v1, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz cmpsb";
            return "repnz cmpsb";
        }
    }

    final static class Cmpsw32 extends Op {
        final private int add_index;
        final private int base;

        public Cmpsw32(int add_index, int base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readw(cpu.regs[base].dword + cpu.esi.dword);
            int v2 = cpu.memory.readw(cpu.es.dword + cpu.edi.dword);
            cpu.edi.dword+=add_index;
            cpu.esi.dword+=add_index;
            Instructions.subw.run(cpu, v1, v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpsw";
        }
    }

    final static class Cmpsw32_r extends Op {
        final private int add_index;
        final private int base;
        final private boolean repeat_zero;

        public Cmpsw32_r(int add_index, int base, boolean repeat_zero) {
            this.add_index = add_index << 1;
            this.base = base;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            if (count>0) {
                int v1=0;
                int v2=0;

                for (int i = 0; i < count; i++) {
                    v1 = cpu.memory.readw(cpu.regs[base].dword + cpu.esi.dword);
                    v2 = cpu.memory.readw(cpu.es.dword + cpu.edi.dword);
                    cpu.edi.dword += add_index;
                    cpu.esi.dword += add_index;
                    cpu.ecx.dword--;
                    if ((v1==v2)!=repeat_zero)
                        break;
                }
                Instructions.subw.run(cpu, v1, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz cmpsw";
            return "repnz cmpsw";
        }
    }

    final static class Cmpsw16 extends Op {
        final private int add_index;
        final private int base;

        public Cmpsw16(int add_index, int base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readw(cpu.regs[base].dword + cpu.esi.u16());
            int v2 = cpu.memory.readw(cpu.es.dword + cpu.edi.u16());
            cpu.edi.u16(cpu.edi.u16()+add_index);
            cpu.esi.u16(cpu.esi.u16()+add_index);
            Instructions.subw.run(cpu, v1, v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpsw";
        }
    }

    final static class Cmpsw16_r extends Op {
        final private int add_index;
        final private int base;
        final private boolean repeat_zero;

        public Cmpsw16_r(int add_index, int base, boolean repeat_zero) {
            this.add_index = add_index << 1;
            this.base = base;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            if (count>0) {
                int v1=0;
                int v2=0;

                for (int i = 0; i < count; i++) {
                    v1 = cpu.memory.readw(cpu.regs[base].dword + cpu.esi.u16());
                    v2 = cpu.memory.readw(cpu.es.dword + cpu.edi.u16());

                    cpu.edi.u16(cpu.edi.u16() + add_index);
                    cpu.esi.u16(cpu.esi.u16() + add_index);
                    cpu.ecx.u16(cpu.ecx.u16() - 1);
                    if ((v1 == v2) != repeat_zero)
                        break;
                }
                Instructions.subw.run(cpu, v1, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz cmpsw";
            return "repnz cmpsw";
        }
    }

    final static class Cmpsd32 extends Op {
        final private int add_index;
        final private int base;

        public Cmpsd32(int add_index, int base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readd(cpu.regs[base].dword + cpu.esi.dword);
            int v2 = cpu.memory.readd(cpu.es.dword + cpu.edi.dword);
            cpu.edi.dword+=add_index;
            cpu.esi.dword+=add_index;
            Instructions.subd.run(cpu, v1, v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpsd";
        }
    }

    final static class Cmpsd32_r extends Op {
        final private int add_index;
        final private int base;
        final private boolean repeat_zero;

        public Cmpsd32_r(int add_index, int base, boolean repeat_zero) {
            this.add_index = add_index << 2;
            this.base = base;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            if (count>0) {
                int v1=0;
                int v2=0;

                for (int i = 0; i < count; i++) {
                    v1 = cpu.memory.readd(cpu.regs[base].dword + cpu.esi.dword);
                    v2 = cpu.memory.readd(cpu.es.dword + cpu.edi.dword);
                    cpu.edi.dword += add_index;
                    cpu.esi.dword += add_index;
                    cpu.ecx.dword--;
                    if ((v1==v2)!=repeat_zero)
                        break;
                }
                Instructions.subd.run(cpu, v1, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz cmpsd";
            return "repnz cmpsd";
        }
    }

    final static class Cmpsd16 extends Op {
        final private int add_index;
        final private int base;

        public Cmpsd16(int add_index, int base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readd(cpu.regs[base].dword + cpu.esi.u16());
            int v2 = cpu.memory.readd(cpu.es.dword + cpu.edi.u16());
            cpu.edi.u16(cpu.edi.u16()+add_index);
            cpu.esi.u16(cpu.esi.u16()+add_index);
            Instructions.subd.run(cpu, v1, v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpsd";
        }
    }

    final static class Cmpsd16_r extends Op {
        final private int add_index;
        final private int base;
        final private boolean repeat_zero;

        public Cmpsd16_r(int add_index, int base, boolean repeat_zero) {
            this.add_index = add_index << 2;
            this.base = base;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            if (count>0) {
                int v1=0;
                int v2=0;

                for (int i = 0; i < count; i++) {
                    v1 = cpu.memory.readd(cpu.regs[base].dword + cpu.esi.u16());
                    v2 = cpu.memory.readd(cpu.es.dword + cpu.edi.u16());

                    cpu.edi.u16(cpu.edi.u16() + add_index);
                    cpu.esi.u16(cpu.esi.u16() + add_index);
                    cpu.ecx.u16(cpu.ecx.u16() - 1);
                    if ((v1 == v2) != repeat_zero)
                        break;
                }
                Instructions.subd.run(cpu, v1, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz cmpsd";
            return "repnz cmpsd";
        }
    }

    final static class Stosb32 extends Op {
        final private int add_index;

        public Stosb32(int add_index) {
            this.add_index = add_index;
        }
        public Block call(CPU cpu) {
            cpu.memory.writeb(cpu.es.dword + cpu.edi.dword, cpu.eax.dword);
            cpu.edi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "stosb";
        }
    }

    final static class Stosb32_r extends Op {
        final private int add_index;

        public Stosb32_r(int add_index) {
            this.add_index = add_index;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.memory.writeb(cpu.es.dword + cpu.edi.dword, cpu.eax.dword);
                cpu.edi.dword += add_index;
                cpu.ecx.dword--;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep stosb";
        }
    }

    final static class Stosb16 extends Op {
        final private int add_index;

        public Stosb16(int add_index) {
            this.add_index = add_index;
        }
        public Block call(CPU cpu) {
            cpu.memory.writeb(cpu.es.dword + cpu.edi.u16(), cpu.eax.dword);
            cpu.edi.u16(cpu.edi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "stosb";
        }
    }

    final static class Stosb16_r extends Op {
        final private int add_index;

        public Stosb16_r(int add_index) {
            this.add_index = add_index;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.memory.writeb(cpu.es.dword + cpu.edi.dword, cpu.eax.dword);
                cpu.edi.u16(cpu.edi.u16()+add_index);
                cpu.ecx.u16(cpu.ecx.u16()-1);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep stosb";
        }
    }

    final static class Stosw32 extends Op {
        final private int add_index;

        public Stosw32(int add_index) {
            this.add_index = add_index << 1;
        }
        public Block call(CPU cpu) {
            cpu.memory.writew(cpu.es.dword + cpu.edi.dword, cpu.eax.dword);
            cpu.edi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "stosw";
        }
    }

    final static class Stosw32_r extends Op {
        final private int add_index;

        public Stosw32_r(int add_index) {
            this.add_index = add_index << 1;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.memory.writew(cpu.es.dword + cpu.edi.dword, cpu.eax.dword);
                cpu.edi.dword += add_index;
                cpu.ecx.dword--;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep stosw";
        }
    }

    final static class Stosw16 extends Op {
        final private int add_index;

        public Stosw16(int add_index) {
            this.add_index = add_index << 1;
        }
        public Block call(CPU cpu) {
            cpu.memory.writew(cpu.es.dword + cpu.edi.u16(), cpu.eax.dword);
            cpu.edi.u16(cpu.edi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "stosw";
        }
    }

    final static class Stosw16_r extends Op {
        final private int add_index;

        public Stosw16_r(int add_index) {
            this.add_index = add_index << 1;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.memory.writew(cpu.es.dword + cpu.edi.dword, cpu.eax.dword);
                cpu.edi.u16(cpu.edi.u16()+add_index);
                cpu.ecx.u16(cpu.ecx.u16()-1);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep stosw";
        }
    }

    final static class Stosd32 extends Op {
        final private int add_index;

        public Stosd32(int add_index) {
            this.add_index = add_index << 2;
        }
        public Block call(CPU cpu) {
            cpu.memory.writed(cpu.es.dword + cpu.edi.dword, cpu.eax.dword);
            cpu.edi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "stosd";
        }
    }

    final static class Stosd32_r extends Op {
        final private int add_index;

        public Stosd32_r(int add_index) {
            this.add_index = add_index << 2;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.memory.writed(cpu.es.dword + cpu.edi.dword, cpu.eax.dword);
                cpu.edi.dword += add_index;
                cpu.ecx.dword--;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep stosd";
        }
    }

    final static class Stosd16 extends Op {
        final private int add_index;

        public Stosd16(int add_index) {
            this.add_index = add_index << 2;
        }
        public Block call(CPU cpu) {
            cpu.memory.writed(cpu.es.dword + cpu.edi.u16(), cpu.eax.dword);
            cpu.edi.u16(cpu.edi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "stosd";
        }
    }

    final static class Stosd16_r extends Op {
        final private int add_index;

        public Stosd16_r(int add_index) {
            this.add_index = add_index << 2;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.memory.writed(cpu.es.dword + cpu.edi.dword, cpu.eax.dword);
                cpu.edi.u16(cpu.edi.u16()+add_index);
                cpu.ecx.u16(cpu.ecx.u16()-1);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep stosd";
        }
    }

    final static class Lodsb32 extends Op {
        final private int add_index;
        final private int base;

        public Lodsb32(int add_index, int base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u8(cpu.memory.readb(cpu.regs[base].dword + cpu.esi.dword));
            cpu.esi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsb";
        }
    }

    final static class Lodsb32_r extends Op {
        final private int add_index;
        final private int base;

        public Lodsb32_r(int add_index, int base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.eax.u8(cpu.memory.readb(cpu.regs[base].dword + cpu.esi.dword));
                cpu.esi.dword += add_index;
                cpu.ecx.dword--;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep lodsb";
        }
    }

    final static class Lodsb16 extends Op {
        final private int add_index;
        final private int base;

        public Lodsb16(int add_index, int base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u8(cpu.memory.readb(cpu.regs[base].dword + cpu.esi.u16()));
            cpu.esi.u16(cpu.esi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsb";
        }
    }

    final static class Lodsb16_r extends Op {
        final private int add_index;
        final private int base;

        public Lodsb16_r(int add_index, int base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.eax.u8(cpu.memory.readb(cpu.regs[base].dword + cpu.esi.u16()));
                cpu.esi.u16(cpu.esi.u16()+add_index);
                cpu.ecx.u16(cpu.ecx.u16()-1);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep lodsb";
        }
    }

    final static class Lodsw32 extends Op {
        final private int add_index;
        final private int base;

        public Lodsw32(int add_index, int base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u16(cpu.memory.readw(cpu.regs[base].dword + cpu.esi.dword));
            cpu.esi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsw";
        }
    }

    final static class Lodsw32_r extends Op {
        final private int add_index;
        final private int base;

        public Lodsw32_r(int add_index, int base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.eax.u16(cpu.memory.readw(cpu.regs[base].dword + cpu.esi.dword));
                cpu.esi.dword += add_index;
                cpu.ecx.dword--;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep lodsw";
        }
    }

    final static class Lodsw16 extends Op {
        final private int add_index;
        final private int base;

        public Lodsw16(int add_index, int base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u16(cpu.memory.readw(cpu.regs[base].dword + cpu.esi.u16()));
            cpu.esi.u16(cpu.esi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsw";
        }
    }

    final static class Lodsw16_r extends Op {
        final private int add_index;
        final private int base;

        public Lodsw16_r(int add_index, int base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.eax.u16(cpu.memory.readw(cpu.regs[base].dword + cpu.esi.u16()));
                cpu.esi.u16(cpu.esi.u16()+add_index);
                cpu.ecx.u16(cpu.ecx.u16()-1);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep lodsw";
        }
    }

    final static class Lodsd32 extends Op {
        final private int add_index;
        final private int base;

        public Lodsd32(int add_index, int base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.dword=cpu.memory.readd(cpu.regs[base].dword + cpu.esi.dword);
            cpu.esi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsd";
        }
    }

    final static class Lodsd32_r extends Op {
        final private int add_index;
        final private int base;

        public Lodsd32_r(int add_index, int base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.eax.dword=cpu.memory.readd(cpu.regs[base].dword + cpu.esi.dword);
                cpu.esi.dword += add_index;
                cpu.ecx.dword--;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep lodsd";
        }
    }

    final static class Lodsd16 extends Op {
        final private int add_index;
        final private int base;

        public Lodsd16(int add_index, int base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.dword=cpu.memory.readd(cpu.regs[base].dword + cpu.esi.u16());
            cpu.esi.u16(cpu.esi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsd";
        }
    }

    final static class Lodsd16_r extends Op {
        final private int add_index;
        final private int base;

        public Lodsd16_r(int add_index, int base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.eax.dword=cpu.memory.readd(cpu.regs[base].dword + cpu.esi.u16());
                cpu.esi.u16(cpu.esi.u16()+add_index);
                cpu.ecx.u16(cpu.ecx.u16()-1);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "rep lodsd";
        }
    }

    final static class Scasb32 extends Op {
        final private int add_index;

        public Scasb32(int add_index) {
            this.add_index = add_index;
        }
        public Block call(CPU cpu) {
            int v2 = cpu.memory.readb(cpu.es.dword + cpu.edi.dword);
            cpu.edi.dword+=add_index;
            Instructions.subb.run(cpu, cpu.eax.u8(), v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "scasb";
        }
    }

    final static class Scasb32_r extends Op {
        final private int add_index;
        final private boolean repeat_zero;

        public Scasb32_r(int add_index, boolean repeat_zero) {
            this.add_index = add_index;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            if (count>0) {
                int v2=0;
                int eax=cpu.eax.u8();

                for (int i = 0; i < count; i++) {
                    v2 = cpu.memory.readb(cpu.es.dword + cpu.edi.dword);
                    cpu.edi.dword += add_index;
                    cpu.ecx.dword--;
                    if ((eax==v2)!=repeat_zero)
                        break;
                }
                Instructions.subb.run(cpu, eax, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz scasb";
            return "repnz scasb";
        }
    }

    final static class Scasb16 extends Op {
        final private int add_index;

        public Scasb16(int add_index) {
            this.add_index = add_index;
        }
        public Block call(CPU cpu) {
            int v2 = cpu.memory.readb(cpu.es.dword + cpu.edi.u16());
            cpu.edi.u16(cpu.edi.u16()+add_index);
            Instructions.subb.run(cpu, cpu.eax.u8(), v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "scasb";
        }
    }

    final static class Scasb16_r extends Op {
        final private int add_index;
        final private boolean repeat_zero;

        public Scasb16_r(int add_index, boolean repeat_zero) {
            this.add_index = add_index;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            if (count>0) {
                int v2=0;
                int eax=cpu.eax.u8();

                for (int i = 0; i < count; i++) {
                    v2 = cpu.memory.readb(cpu.es.dword + cpu.edi.u16());
                    cpu.edi.u16(cpu.edi.u16() + add_index);
                    cpu.ecx.u16(cpu.ecx.u16() - 1);
                    if ((eax == v2) != repeat_zero)
                        break;
                }
                Instructions.subb.run(cpu, eax, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz scasb";
            return "repnz scasb";
        }
    }

    final static class Scasw32 extends Op {
        final private int add_index;

        public Scasw32(int add_index) {
            this.add_index = add_index << 1;
        }
        public Block call(CPU cpu) {
            int v2 = cpu.memory.readw(cpu.es.dword + cpu.edi.dword);
            cpu.edi.dword+=add_index;
            Instructions.subw.run(cpu, cpu.eax.u16(), v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "scasw";
        }
    }

    final static class Scasw32_r extends Op {
        final private int add_index;
        final private boolean repeat_zero;

        public Scasw32_r(int add_index, boolean repeat_zero) {
            this.add_index = add_index << 1;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            if (count>0) {
                int v2=0;
                int eax = cpu.eax.u16();

                for (int i = 0; i < count; i++) {
                    v2 = cpu.memory.readw(cpu.es.dword + cpu.edi.dword);
                    cpu.edi.dword += add_index;
                    cpu.ecx.dword--;
                    if ((eax==v2)!=repeat_zero)
                        break;
                }
                Instructions.subw.run(cpu, eax, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz scasw";
            return "repnz scasw";
        }
    }

    final static class Scasw16 extends Op {
        final private int add_index;

        public Scasw16(int add_index) {
            this.add_index = add_index << 1;
        }
        public Block call(CPU cpu) {
            int v2 = cpu.memory.readw(cpu.es.dword + cpu.edi.u16());
            cpu.edi.u16(cpu.edi.u16()+add_index);
            Instructions.subw.run(cpu, cpu.eax.u16(), v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "scasw";
        }
    }

    final static class Scasw16_r extends Op {
        final private int add_index;
        final private boolean repeat_zero;

        public Scasw16_r(int add_index, boolean repeat_zero) {
            this.add_index = add_index << 1;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            if (count>0) {
                int v2=0;
                int eax=cpu.eax.u16();

                for (int i = 0; i < count; i++) {
                    v2 = cpu.memory.readw(cpu.es.dword + cpu.edi.u16());
                    cpu.edi.u16(cpu.edi.u16() + add_index);
                    cpu.ecx.u16(cpu.ecx.u16() - 1);
                    if ((eax == v2) != repeat_zero)
                        break;
                }
                Instructions.subw.run(cpu, eax, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz scasw";
            return "repnz scasw";
        }
    }

    final static class Scasd32 extends Op {
        final private int add_index;

        public Scasd32(int add_index) {
            this.add_index = add_index << 2;
        }
        public Block call(CPU cpu) {
            int v2 = cpu.memory.readd(cpu.es.dword + cpu.edi.dword);
            cpu.edi.dword+=add_index;
            Instructions.subd.run(cpu, cpu.eax.dword, v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "scasd";
        }
    }

    final static class Scasd32_r extends Op {
        final private int add_index;
        final private boolean repeat_zero;

        public Scasd32_r(int add_index, boolean repeat_zero) {
            this.add_index = add_index << 2;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            if (count>0) {
                int v2=0;
                int eax=cpu.eax.dword;

                for (int i = 0; i < count; i++) {
                    v2 = cpu.memory.readd(cpu.es.dword + cpu.edi.dword);
                    cpu.edi.dword += add_index;
                    cpu.ecx.dword--;
                    if ((eax==v2)!=repeat_zero)
                        break;
                }
                Instructions.subd.run(cpu, eax, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz scasd";
            return "repnz scasd";
        }
    }

    final static class Scasd16 extends Op {
        final private int add_index;

        public Scasd16(int add_index) {
            this.add_index = add_index << 2;
        }
        public Block call(CPU cpu) {
            int v2 = cpu.memory.readd(cpu.es.dword + cpu.edi.u16());
            cpu.edi.u16(cpu.edi.u16()+add_index);
            Instructions.subd.run(cpu, cpu.eax.dword, v2);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "scasd";
        }
    }

    final static class Scasd16_r extends Op {
        final private int add_index;
        final private boolean repeat_zero;

        public Scasd16_r(int add_index, boolean repeat_zero) {
            this.add_index = add_index << 2;
            this.repeat_zero = repeat_zero;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            if (count>0) {
                int v2=0;
                int eax=cpu.eax.dword;

                for (int i = 0; i < count; i++) {
                    v2 = cpu.memory.readd(cpu.es.dword + cpu.edi.u16());
                    cpu.edi.u16(cpu.edi.u16() + add_index);
                    cpu.ecx.u16(cpu.ecx.u16() - 1);
                    if ((eax == v2) != repeat_zero)
                        break;
                }
                Instructions.subd.run(cpu, eax, v2);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            if (repeat_zero)
                return "repz scasd";
            return "repnz scasd";
        }
    }

    static public final class RetIw16 extends Op {
        final private int offset;
        public RetIw16(int offset) {
            this.offset = offset;
        }
        public Block call(CPU cpu) {
            cpu.eip = cpu.pop16();
            cpu.esp.u16(cpu.esp.u16()+offset);
            cpu.callIndex--;
            return cpu.getBlock(cpu.eip, cpu.cs.dword);
        }
        public String toString() {
            return "ret "+offset;
        }
    }

    static public final class Ret16 extends Op {
        public Block call(CPU cpu) {
            cpu.eip = cpu.pop16();
            cpu.callIndex--;
            return cpu.getBlock(cpu.eip, cpu.cs.dword);
        }
        public String toString() {
            return "ret";
        }
    }

    static public final class RetIw32 extends Op {
        final private int offset;
        public RetIw32(int offset) {
            this.offset = offset;
        }
        public Block call(CPU cpu) {
            cpu.eip = cpu.pop32();
            cpu.esp.dword+=offset;
            cpu.callIndex--;
            return cpu.getBlock(cpu.eip, cpu.cs.dword);
        }
        public String toString() {
            return "ret "+offset;
        }
    }

    static public final class Ret32 extends Op {
        public Block call(CPU cpu) {
            cpu.eip = cpu.pop32();
            cpu.callIndex--;
            return cpu.getBlock(cpu.eip, cpu.cs.dword);
        }
        public String toString() {
            return "ret";
        }
    }

    static public final class Salc extends Op {
        public Block call(CPU cpu) {
            cpu.eax.u8(cpu.CF() ? 0xFF : 0);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "salc";
        }
    }

    static public final class Xlat32 extends Op {
        final private int base;
        public Xlat32(int base) {
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u8(cpu.memory.readb(cpu.regs[base].dword + cpu.ebx.dword + cpu.eax.u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return CPU.regToString(base)+":xlat";
        }
    }

    static public final class Xlat16 extends Op {
        final private int base;
        public Xlat16(int base) {
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u8(cpu.memory.readb(cpu.regs[base].dword + ((cpu.ebx.u16() + cpu.eax.u8()) & 0xFFFF)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return CPU.regToString(base)+":xlat";
        }
    }

    static public final class Loopnz32 extends JumpOp {
        public Loopnz32(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            cpu.ecx.dword--;
            return jump(cpu, cpu.ecx.dword!=0 && !cpu.ZF());
        }
        public String toString() {
            return "loopnz "+Integer.toHexString(offset);
        }
    }

    static public final class Loopnz16 extends JumpOp {
        public Loopnz16(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            int v = cpu.ecx.u16()-1;
            cpu.ecx.u16(v);
            return jump(cpu, v!=0 && !cpu.ZF());
        }
        public String toString() {
            return "loopnz "+Integer.toHexString(offset);
        }
    }

    static public final class Loopz32 extends JumpOp {
        public Loopz32(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            cpu.ecx.dword--;
            return jump(cpu, cpu.ecx.dword!=0 && cpu.ZF());
        }
        public String toString() {
            return "loopz "+Integer.toHexString(offset);
        }
    }

    static public final class Loopz16 extends JumpOp {
        public Loopz16(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            int v = cpu.ecx.u16()-1;
            cpu.ecx.u16(v);
            return jump(cpu, v!=0 && cpu.ZF());
        }
        public String toString() {
            return "loopz "+Integer.toHexString(offset);
        }
    }

    static public final class Loop32 extends JumpOp {
        public Loop32(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            cpu.ecx.dword--;
            return jump(cpu, cpu.ecx.dword!=0);
        }
        public String toString() {
            return "loop "+Integer.toHexString(offset);
        }
    }

    static public final class Loop16 extends JumpOp {
        public Loop16(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            int v = cpu.ecx.u16()-1;
            cpu.ecx.u16(v);
            return jump(cpu, v!=0);
        }
        public String toString() {
            return "loop "+Integer.toHexString(offset);
        }
    }

    static public final class Jecxz extends JumpOp {
        public Jecxz(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            return jump(cpu, cpu.ecx.dword==0);
        }
        public String toString() {
            return "jecxz "+Integer.toHexString(offset);
        }
    }

    static public final class Jcxz extends JumpOp {
        public Jcxz(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            return jump(cpu, cpu.ecx.u16()==0);
        }
        public String toString() {
            return "jcxz "+Integer.toHexString(offset);
        }
    }

    static public final class CallJw16 extends JumpOp {
        public CallJw16(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            cpu.push16(cpu.eip+block.eip_count);
            cpu.eip = (cpu.eip + block.eip_count + offset) & 0xFFFF;
            if (block.link1==null)
                block.link1 = cpu.getBlock(cpu.eip, cpu.cs.dword);
            cpu.callIndex++;
            return block.link1;
        }
        public String toString() {
            return "call "+Integer.toHexString(offset+block.eip_count);
        }
    }

    static public final class CallJd32 extends JumpOp {
        public CallJd32(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            cpu.push32(cpu.eip+block.eip_count);
            cpu.eip+=block.eip_count + offset;
            if (block.link1==null)
                block.link1 = cpu.getBlock(cpu.eip, cpu.cs.dword);
            cpu.callIndex++;
            return block.link1;
        }
        public String toString() {
            return "call "+Integer.toHexString(offset+block.eip_count);
        }
    }

    static public final class Jmp16 extends JumpOp {
        public Jmp16(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            cpu.eip = (cpu.eip + block.eip_count + offset) & 0xFFFF;
            if (block.link1==null)
                block.link1 = cpu.getBlock(cpu.eip, cpu.cs.dword);
            return block.link1;
        }
        public String toString() {
            return "jmp 0x"+Integer.toHexString(offset+block.eip_count);
        }
    }

    static public final class Jmp32 extends JumpOp {
        public Jmp32(int offset) {
            super(offset);
        }
        public Block call(CPU cpu) {
            cpu.eip = cpu.eip + block.eip_count + offset;
            if (block.link1==null)
                block.link1 = cpu.getBlock(cpu.eip, cpu.cs.dword);
            return block.link1;
        }
        public String toString() {
            return "jmp 0x"+Integer.toHexString(offset+block.eip_count);
        }
    }

    static public final class Cmc extends Op {
        public Block call(CPU cpu) {
            cpu.fillFlags();
            if (cpu.CF())
                cpu.flags &= ~CPU.CF;
            else
                cpu.flags |= CPU.CF;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmc";
        }
    }

    static public final class Clc extends Op {
        public Block call(CPU cpu) {
            cpu.fillFlags();
            cpu.flags &= ~CPU.CF;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "clc";
        }
    }

    static public final class Stc extends Op {
        public Block call(CPU cpu) {
            cpu.fillFlags();
            cpu.flags |= CPU.CF;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "stc";
        }
    }

    static public final class Cld extends Op {
        public Block call(CPU cpu) {
            cpu.setFlag(false, CPU.DF);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cld";
        }
    }

    static public final class Std extends Op {
        public Block call(CPU cpu) {
            cpu.setFlag(true, CPU.DF);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "std";
        }
    }

    static public final class CallbackOp extends Op {
        final private Callback callback;

        public CallbackOp(Callback callback) {
            this.callback = callback;
        }

        public Block call(CPU cpu) {
            callback.call(cpu);
            cpu.callIndex--;
            return null;
        }

        public String toString() {
            return callback.toString();
        }
    }

    static public final class CallEv_reg extends JumpOp {
        private final int reg;

        public CallEv_reg(int reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            int old = cpu.eip + block.eip_count;
            cpu.push16(old);
            cpu.eip = cpu.regs[reg].u16();
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "call "+CPU.regToString16(reg);
        }
    }

    static public final class CallEv_mem extends JumpOp {
        EaaBase eaa;
        public CallEv_mem(EaaBase eaa) {
            this.eaa = eaa;
        }
        public Block call(CPU cpu) {
            int old = cpu.eip + block.eip_count;
            cpu.eip = cpu.memory.readw(eaa.call(cpu));
            cpu.push16(old);
            cpu.callIndex++;
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "call "+eaa.toString16();
        }
    }

    static public final class JmpEv_reg extends JumpOp {
        private final int reg;

        public JmpEv_reg(int reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            cpu.eip = cpu.regs[reg].u16();
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "jmp "+CPU.regToString16(reg);
        }
    }

    static public final class JmpEv_mem extends JumpOp {
        EaaBase eaa;
        public JmpEv_mem(EaaBase eaa) {
            this.eaa = eaa;
        }
        public Block call(CPU cpu) {
            cpu.eip = cpu.memory.readw(eaa.call(cpu));
            if (!cpu.big)
                cpu.eip &= 0xFFFF;
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "jmp "+eaa.toString16();
        }
    }

    static public final class PushEv_reg extends JumpOp {
        private final int reg;

        public PushEv_reg(int reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            cpu.push16(cpu.regs[reg].u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+CPU.regToString16(reg);
        }
    }

    static public final class PushEv_mem extends JumpOp {
        EaaBase eaa;
        public PushEv_mem(EaaBase eaa) {
            this.eaa = eaa;
        }
        public Block call(CPU cpu) {
            cpu.push16(cpu.memory.readw(eaa.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+eaa.toString16();
        }
    }

    static public final class CallNearEd_reg extends JumpOp {
        private final int reg;

        public CallNearEd_reg(int reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            int old = cpu.eip + block.eip_count;
            cpu.push32(old);
            cpu.callIndex++;
            cpu.eip = cpu.regs[reg].dword;
            if (!cpu.big)
                cpu.eip &= 0xFFFF;
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "call "+CPU.regToString(reg);
        }
    }

    static public final class CallNearEd_mem extends JumpOp {
        EaaBase eaa;
        public CallNearEd_mem(EaaBase eaa) {
            this.eaa = eaa;
        }
        public Block call(CPU cpu) {
            int old = cpu.eip + block.eip_count;
            cpu.eip = cpu.memory.readd(eaa.call(cpu));
            cpu.push32(old);
            cpu.callIndex++;
            if (!cpu.big)
                cpu.eip &= 0xFFFF;
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "call "+eaa.toString32();
        }
    }

    static public final class JmpNearEd_reg extends JumpOp {
        private final int reg;

        public JmpNearEd_reg(int reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            cpu.eip = cpu.regs[reg].dword;
            if (!cpu.big)
                cpu.eip &= 0xFFFF;
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "jmp "+CPU.regToString(reg);
        }
    }

    static public final class JmpNearEd_mem extends JumpOp {
        EaaBase eaa;
        public JmpNearEd_mem(EaaBase eaa) {
            this.eaa = eaa;
        }
        public Block call(CPU cpu) {
            cpu.eip = cpu.memory.readd(eaa.call(cpu));
            if (!cpu.big)
                cpu.eip &= 0xFFFF;
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "jmp "+eaa.toString32();
        }
    }

    static public final class PushEd_reg extends Op {
        private final int reg;

        public PushEd_reg(int reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            cpu.push32(cpu.regs[reg].dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+CPU.regToString(reg);
        }
    }

    static public final class PushEd_mem extends Op {
        EaaBase eaa;
        public PushEd_mem(EaaBase eaa) {
            this.eaa = eaa;
        }
        public Block call(CPU cpu) {
            cpu.push32(cpu.memory.readd(eaa.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+eaa.toString32();
        }
    }

    static public final class Nop extends Op {
        public Block call(CPU cpu) {
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "nop";
        }
    }

    static public final class Leave16 extends Op {
        public Block call(CPU cpu) {
            if (cpu.big) {
                cpu.esp.dword = cpu.ebp.dword;
            } else {
                cpu.esp.u16(cpu.ebp.u16());
            }
            cpu.ebp.u16(cpu.pop16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "leave";
        }
    }

    static public final class Leave32 extends Op {
        public Block call(CPU cpu) {
            if (cpu.big) {
                cpu.esp.dword = cpu.ebp.dword;
            } else {
                cpu.esp.u16(cpu.ebp.u16());
            }
            cpu.ebp.dword = cpu.pop32();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "leave";
        }
    }

    static public final class SetFlag_Reg8 extends Op {
        final private int reg;
        final private Condition condition;

        public SetFlag_Reg8(int reg, Condition condition) {
            this.reg = reg;
            this.condition = condition;
        }
        public Block call(CPU cpu) {
            if (condition.test(cpu))
                cpu.regs[reg].u8(1);
            else
                cpu.regs[reg].u8(0);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "set"+condition.toString()+" "+CPU.regToString8(reg);
        }
    }

    static public final class SetFlag_Mem8 extends Op {
        final private EaaBase eaa;
        final private Condition condition;

        public SetFlag_Mem8(EaaBase eaa, Condition condition) {
            this.eaa = eaa;
            this.condition = condition;
        }
        public Block call(CPU cpu) {
            if (condition.test(cpu))
                cpu.memory.writeb(eaa.call(cpu), 1);
            else
                cpu.memory.writeb(eaa.call(cpu), 0);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "set"+condition.toString()+" "+eaa.toString8();
        }
    }

    static public final class BtEwGw_reg extends Op {
        final private int ew;
        final private int gw;

        public BtEwGw_reg(int ew, int gw) {
            this.ew = ew;
            this.gw = gw;
        }
        public Block call(CPU cpu) {
            cpu.fillFlagsNoCF();
            int mask=1 << (cpu.regs[gw].u16() & 15);
            if ((cpu.regs[ew].u16() & mask)!=0) {
                cpu.flags|=CPU.CF;
            } else {
                cpu.flags&=~CPU.CF;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bt "+CPU.regToString16(ew)+", "+CPU.regToString16(gw);
        }
    }

    static public final class BtEwGw_mem extends Op {
        final private EaaBase eaa;
        final private int gw;

        public BtEwGw_mem(EaaBase eaa, int gw) {
            this.eaa = eaa;
            this.gw = gw;
        }
        public Block call(CPU cpu) {
            cpu.fillFlagsNoCF();
            int mask=1 << (cpu.regs[gw].u16() & 15);
            int address = eaa.call(cpu);

            address+=(((short)cpu.regs[gw].u16())>>4)*2;
            if ((cpu.memory.readw(address) & mask)!=0) {
                cpu.flags|=CPU.CF;
            } else {
                cpu.flags&=~CPU.CF;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bt "+eaa.toString16()+", "+CPU.regToString16(gw);
        }
    }

    static public final class BtEdGd_reg extends Op {
        final private int ed;
        final private int gd;

        public BtEdGd_reg(int ed, int gd) {
            this.ed = ed;
            this.gd = gd;
        }
        public Block call(CPU cpu) {
            cpu.fillFlagsNoCF();
            int mask=1 << (cpu.regs[gd].dword & 31);
            if ((cpu.regs[ed].dword & mask)!=0) {
                cpu.flags|=CPU.CF;
            } else {
                cpu.flags&=~CPU.CF;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bt "+CPU.regToString(ed)+", "+CPU.regToString(gd);
        }
    }

    static public final class BtEdGd_mem extends Op {
        final private EaaBase eaa;
        final private int gd;

        public BtEdGd_mem(EaaBase eaa, int gd) {
            this.eaa = eaa;
            this.gd = gd;
        }
        public Block call(CPU cpu) {
            cpu.fillFlagsNoCF();
            int mask=1 << (cpu.regs[gd].dword & 31);
            int address = eaa.call(cpu);

            address+=(cpu.regs[gd].dword>>5)*4; // intentional signed shift
            if ((cpu.memory.readd(address) & mask)!=0) {
                cpu.flags|=CPU.CF;
            } else {
                cpu.flags&=~CPU.CF;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bt "+eaa.toString32()+", "+CPU.regToString(gd);
        }
    }

    static public final class Mov_Reg16_Seg extends Op {
        final private int segValue;
        final private int segPhys;
        final private int reg;

        public Mov_Reg16_Seg(int segPhys, int segValue, int reg) {
            this.segPhys = segPhys;
            this.segValue = segValue;
            this.reg = reg;
        }
        public Block call(CPU cpu) {
            cpu.regs[segValue].dword = cpu.regs[reg].u16();
            cpu.regs[segPhys].dword = cpu.ldt[cpu.regs[reg].u16() >> 3];
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(segPhys)+", "+CPU.regToString16(reg);
        }
    }

    static public final class Mov_Mem16_Seg extends Op {
        final private int segValue;
        final private int segPhys;
        final private EaaBase eaa;

        public Mov_Mem16_Seg(int segPhys, int segValue, EaaBase eaa) {
            this.segPhys = segPhys;
            this.segValue = segValue;
            this.eaa = eaa;
        }
        public Block call(CPU cpu) {
            int sel = cpu.memory.readw(eaa.call(cpu));
            cpu.regs[segValue].dword = sel;
            cpu.regs[segPhys].dword = cpu.ldt[sel];
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+CPU.regToString(segPhys)+", "+eaa.toString16();
        }
    }

    final static class Xadd_Reg8_Reg8 extends Op {
        final private int gb;
        final private int eb;

        public Xadd_Reg8_Reg8(int gb, int eb) {
            this.gb = gb;
            this.eb = eb;
        }
        public Block call(CPU cpu) {
            int result=Instructions.addb.run(cpu, cpu.regs[gb].u8(), cpu.regs[eb].u8());
            cpu.regs[gb].u8(cpu.regs[eb].u8());
            cpu.regs[eb].u8(result);

            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+CPU.regToString8(gb)+", "+CPU.regToString8(eb);
        }
    }

    final static class Xadd_Reg8_Mem8 extends Op {
        final private int gb;
        final private EaaBase eb;

        public Xadd_Reg8_Mem8(int gb, EaaBase eb) {
            this.gb = gb;
            this.eb = eb;
        }
        public Block call(CPU cpu) {
            int eaa=eb.call(cpu);
            Memory memory = cpu.memory;
            int val = memory.readb(eaa);
            int result = Instructions.addb.run(cpu, cpu.regs[gb].u8(), val);
            memory.writeb(eaa,result);
            cpu.regs[gb].u8(val);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+CPU.regToString8(gb)+", "+eb.toString8();
        }
    }

    final static class Xadd_Reg8_Mem8_Lock extends Op {
        final private int gb;
        final private EaaBase eb;

        public Xadd_Reg8_Mem8_Lock(int gb, EaaBase eb) {
            this.gb = gb;
            this.eb = eb;
        }
        public Block call(CPU cpu) {
            int eaa=eb.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            int val = memory.readb(eaa);
            int result = Instructions.addb.run(cpu, cpu.regs[gb].u8(), val);
            memory.writeb(eaa,result);
            memory.unlock();
            cpu.regs[gb].u8(val);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock xadd "+CPU.regToString8(gb)+", "+eb.toString8();
        }
    }

    final static class Xadd_Reg16_Reg16 extends Op {
        final private int gw;
        final private int ew;

        public Xadd_Reg16_Reg16(int gw, int ew) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            int result=Instructions.addw.run(cpu, cpu.regs[gw].u16(), cpu.regs[ew].u16());
            cpu.regs[gw].u16(cpu.regs[ew].u16());
            cpu.regs[ew].u16(result);

            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+CPU.regToString16(gw)+", "+CPU.regToString16(ew);
        }
    }

    final static class Xadd_Reg16_Mem16 extends Op {
        final private int gw;
        final private EaaBase ew;

        public Xadd_Reg16_Mem16(int gw, EaaBase ew) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            int eaa=ew.call(cpu);
            Memory memory = cpu.memory;
            int val = memory.readw(eaa);
            int result = Instructions.addw.run(cpu, cpu.regs[gw].u16(), val);
            memory.writew(eaa,result);
            cpu.regs[gw].u16(val);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+CPU.regToString16(gw)+", "+ew.toString16();
        }
    }

    final static class Xadd_Reg16_Mem16_Lock extends Op {
        final private int gw;
        final private EaaBase ew;

        public Xadd_Reg16_Mem16_Lock(int gw, EaaBase ew) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            int eaa=ew.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            int val = memory.readw(eaa);
            int result = Instructions.addw.run(cpu, cpu.regs[gw].u16(), val);
            memory.writew(eaa,result);
            memory.unlock();
            cpu.regs[gw].u16(val);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock xadd "+CPU.regToString16(gw)+", "+ew.toString16();
        }
    }

    final static class Xadd_Reg32_Reg32 extends Op {
        final private int gd;
        final private int ed;

        public Xadd_Reg32_Reg32(int gd, int ed) {
            this.gd = gd;
            this.ed = ed;
        }
        public Block call(CPU cpu) {
            int result=Instructions.addd.run(cpu, cpu.regs[gd].dword, cpu.regs[ed].dword);
            cpu.regs[gd].dword=cpu.regs[ed].dword;
            cpu.regs[ed].dword=result;

            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+CPU.regToString(gd)+", "+CPU.regToString(ed);
        }
    }

    final static class Xadd_Reg32_Mem32 extends Op {
        final private int gd;
        final private EaaBase ed;

        public Xadd_Reg32_Mem32(int gd, EaaBase ed) {
            this.gd = gd;
            this.ed = ed;
        }
        public Block call(CPU cpu) {
            int eaa=ed.call(cpu);
            Memory memory = cpu.memory;
            int val = memory.readd(eaa);
            int result = Instructions.addd.run(cpu, cpu.regs[gd].dword, val);
            memory.writed(eaa,result);
            cpu.regs[gd].dword=val;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+CPU.regToString(gd)+", "+ed.toString32();
        }
    }

    final static class Xadd_Reg32_Mem32_Lock extends Op {
        final private int gd;
        final private EaaBase ed;

        public Xadd_Reg32_Mem32_Lock(int gd, EaaBase ed) {
            this.gd = gd;
            this.ed = ed;
        }
        public Block call(CPU cpu) {
            int eaa=ed.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            int val = memory.readd(eaa);
            int result = Instructions.addd.run(cpu, cpu.regs[gd].dword, val);
            memory.writed(eaa,result);
            memory.unlock();
            cpu.regs[gd].dword=val;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock xadd "+CPU.regToString(gd)+", "+ed.toString32();
        }
    }

    final static class CPUID extends Op {
        public Block call(CPU cpu) {
            switch (cpu.eax.dword) {
                case 0:	/* Vendor ID String and maximum level? */
                    cpu.eax.dword=1;  /* Maximum level */
                    cpu.ebx.dword='G' | ('e' << 8) | ('n' << 16) | ('u'<< 24);
                    cpu.edx.dword='i' | ('n' << 8) | ('e' << 16) | ('I'<< 24);
                    cpu.ecx.dword='n' | ('t' << 8) | ('e' << 16) | ('l'<< 24);
                    break;
                case 1:	/* get processor type/family/model/stepping and feature flags */
                    cpu.eax.dword=0x611;		/* intel pentium pro */
                    cpu.ebx.dword=0;			/* Not Supported */
                    cpu.ecx.dword=0;			/* No features */
                    cpu.edx.dword=0x00000011;	/* FPU+TimeStamp/RDTSC */
                    cpu.edx.dword|= (1<<5);     /* MSR */
                    cpu.edx.dword|= (1<<15);    /* support CMOV instructions */
                    cpu.edx.dword|= (1<<13);    /* PTE Global Flag */
                    cpu.edx.dword|= (1<<8);     /* CMPXCHG8B instruction */
                    break;
                case 0x80000000:
                    cpu.eax.dword = 0;
                    break;
                default:
                    Log.warn("Unhandled CPUID Function " + Integer.toString(cpu.eax.dword, 16));
                    cpu.eax.dword=0;
                    cpu.ebx.dword=0;
                    cpu.ecx.dword=0;
                    cpu.edx.dword=0;
                    break;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "CPUID";
        }
    }

    final static class CmpxchgEdGd_reg extends Op {
        final private int gd;
        final private int ed;

        public CmpxchgEdGd_reg(int ed, int gd) {
            this.gd = gd;
            this.ed = ed;
        }
        public Block call(CPU cpu) {
            Instructions.subd.run(cpu, cpu.regs[ed].dword, cpu.eax.dword);
            if (cpu.eax.dword == cpu.regs[ed].dword) {
                cpu.regs[ed].dword=cpu.regs[gd].dword;
            } else {
                cpu.eax.dword=cpu.regs[ed].dword;
            }

            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpxchg "+CPU.regToString(ed)+", "+CPU.regToString(gd);
        }
    }

    final static class CmpxchgEdGd_mem extends Op {
        final private int gd;
        final private EaaBase ed;

        public CmpxchgEdGd_mem(EaaBase ed, int gd) {
            this.gd = gd;
            this.ed = ed;
        }
        public Block call(CPU cpu) {
            int eaa=ed.call(cpu);
            Memory memory = cpu.memory;
            int val = memory.readd(eaa);
            Instructions.subd.run(cpu, val, cpu.eax.dword);
            if (cpu.eax.dword == val) {
                memory.writed(eaa,cpu.regs[gd].dword);
            } else {
                memory.writed(eaa,val);	// cmpxchg always issues a write
                cpu.eax.dword=val;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpxchg "+ed.toString32()+", "+CPU.regToString(gd);
        }
    }

    final static class CmpxchgEdGd_mem_Lock extends Op {
        final private int gd;
        final private EaaBase ed;

        public CmpxchgEdGd_mem_Lock(EaaBase ed, int gd) {
            this.gd = gd;
            this.ed = ed;
        }
        public Block call(CPU cpu) {
            int eaa=ed.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            int val = memory.readd(eaa);
            Instructions.subd.run(cpu, val, cpu.eax.dword);
            if (cpu.eax.dword == val) {
                memory.writed(eaa,cpu.regs[gd].dword);
            } else {
                memory.writed(eaa,val);	// cmpxchg always issues a write
                cpu.eax.dword=val;
            }
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock cmpxchg "+ed.toString32()+", "+CPU.regToString(gd);
        }
    }

    final static class CmpxchgEwGw_reg extends Op {
        final private int gw;
        final private int ew;

        public CmpxchgEwGw_reg(int ew, int gw) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            Instructions.subw.run(cpu, cpu.regs[ew].u16(), cpu.eax.u16());
            if (cpu.eax.u16() == cpu.regs[ew].u16()) {
                cpu.regs[ew].u16(cpu.regs[gw].u16());
            } else {
                cpu.eax.u16(cpu.regs[ew].u16());
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpxchg "+CPU.regToString16(ew)+", "+CPU.regToString16(gw);
        }
    }

    final static class CmpxchgEwGw_mem extends Op {
        final private int gw;
        final private EaaBase ew;

        public CmpxchgEwGw_mem(EaaBase ew, int gw) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            int eaa=ew.call(cpu);
            Memory memory = cpu.memory;
            int val = memory.readw(eaa);
            Instructions.subw.run(cpu, val, cpu.eax.u16());
            if (cpu.eax.u16() == val) {
                memory.writew(eaa,cpu.regs[gw].u16());
            } else {
                memory.writew(eaa,val);	// cmpxchg always issues a write
                cpu.eax.u16(val);
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpxchg "+ew.toString16()+", "+CPU.regToString16(gw);
        }
    }

    final static class CmpxchgEwGw_mem_Lock extends Op {
        final private int gw;
        final private EaaBase ew;

        public CmpxchgEwGw_mem_Lock(EaaBase ew, int gw) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            int eaa=ew.call(cpu);
            Memory memory = cpu.memory;
            memory.lock();
            int val = memory.readw(eaa);
            Instructions.subw.run(cpu, val, cpu.eax.u16());
            if (cpu.eax.u16() == val) {
                memory.writew(eaa,cpu.regs[gw].u16());
            } else {
                memory.writew(eaa,val);	// cmpxchg always issues a write
                cpu.eax.u16(val);
            }
            memory.unlock();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lock cmpxchg "+ew.toString16()+", "+CPU.regToString16(gw);
        }
    }

    static public class RegGetter extends wine.system.kernel.Syscall.SyscallGetter {
        CPU cpu;
        int pos=-1;
        public RegGetter(CPU cpu) {
            this.cpu = cpu;
        }

        public int next() {
            pos++;
            switch (pos) {
                case 0:return cpu.ebx.dword;
                case 1:return cpu.ecx.dword;
                case 2:return cpu.edx.dword;
                case 3:return cpu.esi.dword;
                case 4:return cpu.edi.dword;
                case 5:return cpu.ebp.dword;
                default:
                    Log.panic("Oops, something went wrong with int 80 sys call");
            }
            return 0;
        }
    }

    static public final class Syscall extends Op {
        public Block call(CPU cpu) {
            cpu.eax.dword = wine.system.kernel.Syscall.syscall(cpu.thread, cpu.eax.dword, new RegGetter(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "syscall";
        }
    }

    static public final class ConditionalMov_Reg16 extends Op {
        final int ew;
        final int gw;
        final Condition cond;

        public ConditionalMov_Reg16(Condition cond, int ew, int gw) {
            this.ew = ew;
            this.gw = gw;
            this.cond = cond;
        }

        public Block call(CPU cpu) {
            if (cond.test(cpu))
                cpu.regs[gw].u16(cpu.regs[ew].u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmov "+cond.toString()+" "+CPU.regToString16(gw)+", "+CPU.regToString16(ew);
        }
    }

    static public final class ConditionalMov_Mem16 extends Op {
        final EaaBase eaa;
        final int gw;
        final Condition cond;

        public ConditionalMov_Mem16(Condition cond, EaaBase eaa, int gw) {
            this.eaa = eaa;
            this.gw = gw;
            this.cond = cond;
        }

        public Block call(CPU cpu) {
            int tmp = cpu.memory.readw(eaa.call(cpu));
            if (cond.test(cpu))
                cpu.regs[gw].u16(tmp);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmov "+cond.toString()+" "+CPU.regToString16(gw)+", "+eaa.toString16();
        }
    }

    static public final class ConditionalMov_Reg32 extends Op {
        final int ed;
        final int gd;
        final Condition cond;

        public ConditionalMov_Reg32(Condition cond, int ed, int gd) {
            this.ed = ed;
            this.gd = gd;
            this.cond = cond;
        }

        public Block call(CPU cpu) {
            if (cond.test(cpu))
                cpu.regs[gd].dword = cpu.regs[ed].dword;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmov "+cond.toString()+" "+CPU.regToString(gd)+", "+CPU.regToString(ed);
        }
    }

    static public final class ConditionalMov_Mem32 extends Op {
        final EaaBase eaa;
        final int gd;
        final Condition cond;

        public ConditionalMov_Mem32(Condition cond, EaaBase eaa, int gd) {
            this.eaa = eaa;
            this.gd = gd;
            this.cond = cond;
        }

        public Block call(CPU cpu) {
            int tmp = cpu.memory.readd(eaa.call(cpu));
            if (cond.test(cpu))
                cpu.regs[gd].dword = tmp;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmov "+cond.toString()+" "+CPU.regToString(gd)+", "+eaa.toString32();
        }
    }

    static public final class BtcEdGd_reg extends Op {
        final int ed;
        final int gd;

        public BtcEdGd_reg(int ed, int gd) {
            this.ed = ed;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            cpu.fillFlags();
            int mask=1 << (cpu.regs[gd].dword & 31);
            cpu.setFlag((cpu.regs[ed].dword & mask)!=0, CPU.CF);
            cpu.regs[ed].dword^=mask;
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "btc";
        }
    }

    static public final class BtcEdGd_mem extends Op {
        final EaaBase eaa;
        final int gd;

        public BtcEdGd_mem(EaaBase eaa, int gd) {
            this.eaa = eaa;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            cpu.fillFlags();
            int mask=1 << (cpu.regs[gd].dword & 31);
            int address=eaa.call(cpu);
            address+=(cpu.regs[gd].dword>>5)*4; // intentional signed shift
            Memory memory = cpu.memory;
            int old=memory.readd(address);
            cpu.setFlag((old & mask)!=0, CPU.CF);
            memory.writed(address,old ^ mask);
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "btc";
        }
    }

    static public final class BtcEdGd_mem_Lock extends Op {
        final EaaBase eaa;
        final int gd;

        public BtcEdGd_mem_Lock(EaaBase eaa, int gd) {
            this.eaa = eaa;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            cpu.fillFlags();
            int mask=1 << (cpu.regs[gd].dword & 31);
            int address=eaa.call(cpu);
            address+=(cpu.regs[gd].dword>>5)*4; // intentional signed shift
            Memory memory = cpu.memory;
            memory.lock();
            int old=memory.readd(address);
            cpu.setFlag((old & mask)!=0, CPU.CF);
            memory.writed(address,old ^ mask);
            memory.unlock();
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "lock btc";
        }
    }

    static public final class BsfGdEd_reg extends Op {
        final int ed;
        final int gd;

        public BsfGdEd_reg(int ed, int gd) {
            this.ed = ed;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            cpu.fillFlags();
            int value=cpu.regs[ed].dword;
            if (value==0) {
                cpu.setFlag(true, CPU.ZF);
            } else {
                int result = 0;
                while ((value & 0x01)==0) { result++; value>>>=1; }
                cpu.setFlag(false, CPU.ZF);
                cpu.regs[gd].dword=result;
            }
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "bsf";
        }
    }

    static public final class BsfGdEd_mem extends Op {
        final EaaBase eaa;
        final int gd;

        public BsfGdEd_mem(EaaBase eaa, int gd) {
            this.eaa = eaa;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            int address=eaa.call(cpu);
            int value=cpu.memory.readd(address);
            if (value==0) {
                cpu.setFlag(true, CPU.ZF);
            } else {
                int result = 0;
                while ((value & 0x01)==0) { result++; value>>>=1; }
                cpu.setFlag(false, CPU.ZF);
                cpu.regs[gd].dword=result;
            }
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "bsf";
        }
    }

    static public final class BsrGdEd_reg extends Op {
        final int ed;
        final int gd;

        public BsrGdEd_reg(int ed, int gd) {
            this.ed = ed;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            cpu.fillFlags();
            int value=cpu.regs[ed].dword;
            if (value==0) {
                cpu.setFlag(true, CPU.ZF);
            } else {
                int result = 31;	// Operandsize-1
                while ((value & 0x80000000)==0) { result--; value<<=1; }
                cpu.setFlag(false, CPU.ZF);
                cpu.regs[gd].dword=result;
            }
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "bsr";
        }
    }

    static public final class BsrGdEd_mem extends Op {
        final EaaBase eaa;
        final int gd;

        public BsrGdEd_mem(EaaBase eaa, int gd) {
            this.eaa = eaa;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            int address=eaa.call(cpu);
            int value=cpu.memory.readd(address);
            if (value==0) {
                cpu.setFlag(true, CPU.ZF);
            } else {
                int result = 31;	// Operandsize-1
                while ((value & 0x80000000)==0) { result--; value<<=1; }
                cpu.setFlag(false, CPU.ZF);
                cpu.regs[gd].dword=result;
            }
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "bsr";
        }
    }

    final static class PopSeg32 extends Op {
        final private int segValue;
        final private int segPhys;
        final private String name;

        public PopSeg32(int segValue, int segPhys, String name) {
            this.segValue = segValue;
            this.segPhys = segPhys;
            this.name = name;
        }

        public Block call(CPU cpu) {
            cpu.regs[segValue].dword = cpu.pop32();
            cpu.regs[segPhys].dword = cpu.ldt[cpu.regs[segValue].dword >> 3];
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pop "+name;
        }
    }

    final static class Rdtsc extends Op {
        public Block call(CPU cpu) {
            long tsc = System.nanoTime();
            cpu.eax.dword = (int)tsc;
            cpu.edx.dword = (int)(tsc >>> 32);
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "RDTSCc";
        }
    }

    final static public class BtEdIb_reg extends Op {
        int ed;
        int mask;
        public BtEdIb_reg(int ed, int mask) {
            this.ed = ed;
            this.mask = mask;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            cpu.setFlag((cpu.regs[ed].dword & mask)!=0, CPU.CF);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bt "+CPU.regToString(ed)+", "+Integer.toHexString(mask);
        }
    }

    final static public class BtsEdIb_reg extends Op {
        int ed;
        int mask;
        public BtsEdIb_reg(int ed, int mask) {
            this.ed = ed;
            this.mask = mask;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            cpu.setFlag((cpu.regs[ed].dword & mask)!=0, CPU.CF);
            cpu.regs[ed].dword|=mask;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bts "+CPU.regToString(ed)+", "+Integer.toHexString(mask);
        }
    }
    final static public class BtrEdIb_reg extends Op {
        int ed;
        int mask;
        public BtrEdIb_reg(int ed, int mask) {
            this.ed = ed;
            this.mask = mask;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            cpu.setFlag((cpu.regs[ed].dword & mask)!=0, CPU.CF);
            cpu.regs[ed].dword&=~mask;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "btr "+CPU.regToString(ed)+", "+Integer.toHexString(mask);
        }
    }
    final static public class BtcEdIb_reg extends Op {
        int ed;
        int mask;
        public BtcEdIb_reg(int ed, int mask) {
            this.ed = ed;
            this.mask = mask;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            boolean cf = (cpu.regs[ed].dword & mask)!=0;
            cpu.setFlag(cf, CPU.CF);
            if (cf) cpu.regs[ed].dword&=~mask;
            else cpu.regs[ed].dword|=mask;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "btc "+CPU.regToString(ed)+", "+Integer.toHexString(mask);
        }
    }
    final static public class BtEdIb_mem extends Op {
        EaaBase eaa;
        int mask;
        public BtEdIb_mem(EaaBase eaa, int mask) {
            this.eaa = eaa;
            this.mask = mask;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            int address=eaa.call(cpu);
            int old=cpu.memory.readd(address);
            cpu.setFlag((old & mask)!=0, CPU.CF);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bt "+eaa.toString32()+", "+Integer.toHexString(mask);
        }
    }
    final static public class BtsEdIb_mem extends Op {
        EaaBase eaa;
        int mask;
        public BtsEdIb_mem(EaaBase eaa, int mask) {
            this.eaa = eaa;
            this.mask = mask;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            int address=eaa.call(cpu);
            int old=cpu.memory.readd(address);
            cpu.setFlag((old & mask)!=0, CPU.CF);
            cpu.memory.writed(address,old|mask);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bts "+eaa.toString32()+", "+Integer.toHexString(mask);
        }
    }
    final static public class BtrEdIb_mem extends Op {
        EaaBase eaa;
        int mask;
        public BtrEdIb_mem(EaaBase eaa, int mask) {
            this.eaa = eaa;
            this.mask = mask;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            int address=eaa.call(cpu);
            int old=cpu.memory.readd(address);
            cpu.setFlag((old & mask)!=0, CPU.CF);
            cpu.memory.writed(address,old & ~mask);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "btr "+eaa.toString32()+", "+Integer.toHexString(mask);
        }
    }
    final static public class BtcEdIb_mem extends Op {
        EaaBase eaa;
        int mask;
        public BtcEdIb_mem(EaaBase eaa, int mask) {
            this.eaa = eaa;
            this.mask = mask;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            int address=eaa.call(cpu);
            int old=cpu.memory.readd(address);
            boolean cf = (old & mask)!=0;
            cpu.setFlag(cf, CPU.CF);
            if (cf) old&=~mask;
            else old|=mask;
            cpu.memory.writed(address,old);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "btc "+eaa.toString32()+", "+Integer.toHexString(mask);
        }
    }

    final static public class BtsEdGd_reg extends Op {
        int gd;
        int ed;

        public BtsEdGd_reg(int ed, int gd) {
            this.ed = ed;
            this.gd = gd;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            int mask=1 << (cpu.regs[gd].dword & 31);
            cpu.setFlag((cpu.regs[ed].dword & mask)!=0, CPU.CF);
            cpu.regs[ed].dword|=mask;
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "bts " + CPU.regToString(ed) + ", " + CPU.regToString(gd);
        }
    }
    final static public class BtsEdGd_mem extends Op {
        int gd;
        EaaBase eaa;
        public BtsEdGd_mem(EaaBase eaa, int gd) {
            this.gd = gd;
            this.eaa = eaa;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            int mask=1 << (cpu.regs[gd].dword & 31);
            int address=eaa.call(cpu);
            address+=(cpu.regs[gd].dword>>5)*4; // intentional signed shift
            int old=cpu.memory.readd(address);
            cpu.setFlag((old & mask)!=0, CPU.CF);
            cpu.memory.writed(address,old | mask);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bts " + eaa.toString32() + ", " + CPU.regToString(gd);
        }
    }

    final static public class BtsEwGw_reg extends Op {
        int gw;
        int ew;
        public BtsEwGw_reg(int ew, int gw) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            int mask=1 << (cpu.regs[gw].u16() & 15);
            cpu.setFlag((cpu.regs[ew].u16() & mask)!=0, CPU.CF);
            cpu.regs[ew].u16(cpu.regs[ew].u16() | mask);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bts " + CPU.regToString16(ew) + ", " + CPU.regToString16(gw);
        }
    }
    final static public class BtsEwGw_mem extends Op {
        int gw;
        EaaBase eaa;
        public BtsEwGw_mem(EaaBase eaa, int gw) {
            this.eaa = eaa;
            this.gw = gw;
        }
        public Block call(CPU cpu) {
            cpu.fillFlags();
            int mask=1 << (cpu.regs[gw].u16() & 15);
            int address=eaa.call(cpu);
            address+=(((/*Bit16s*/short)cpu.regs[gw].u16())>>4)*2;
            int old=cpu.memory.readw(address);
            cpu.setFlag((old & mask)!=0, CPU.CF);
            cpu.memory.writew(address,old | mask);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bts " + eaa.toString16() + ", " + CPU.regToString16(gw);
        }
    }

    final static public class Bswapd extends Op {
        int reg;

        public Bswapd(int reg) {
            this.reg = reg;
        }
        public Block call(CPU cpu) {
            int i = cpu.regs[reg].dword;
            cpu.regs[reg].dword=(i>>>24)|((i>>8)&0xFF00)|((i<<8)&0xFF0000)|(i<<24);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bswap " + CPU.regToString(reg);
        }
    }
}

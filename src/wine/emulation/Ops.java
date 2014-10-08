package wine.emulation;

import wine.system.Callback;
import wine.util.Log;

class Ops {
    final static class Instruction_Reg8 extends Op {
        final private Instruction instruction;
        final private Reg dest;

        public Instruction_Reg8(Instruction instruction, Reg dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.u8(instruction.run(cpu, dest.u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name8;
        }
    }

    final static class Instruction_Reg8_Custom extends Op {
        final private Instruction instruction;
        final private Reg dest;

        public Instruction_Reg8_Custom(Instruction instruction, Reg dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            instruction.runCustom(cpu, dest.u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name8;
        }
    }

    final static class Instruction_Reg16 extends Op {
        final private Instruction instruction;
        final private Reg dest;

        public Instruction_Reg16(Instruction instruction, Reg dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.u16(instruction.run(cpu, dest.u16()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16;
        }
    }

    final static class Instruction_Reg16_Custom extends Op {
        final private Instruction instruction;
        final private Reg dest;

        public Instruction_Reg16_Custom(Instruction instruction, Reg dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            instruction.runCustom(cpu, dest.u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16;
        }
    }

    final static class Instruction_Reg32 extends Op {
        final private Instruction instruction;
        final private Reg dest;

        public Instruction_Reg32(Instruction instruction, Reg dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.dword=instruction.run(cpu, dest.dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32;
        }
    }

    final static class Instruction_Reg32_Custom extends Op {
        final private Instruction instruction;
        final private Reg dest;

        public Instruction_Reg32_Custom(Instruction instruction, Reg dest) {
            this.instruction = instruction;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            instruction.runCustom(cpu, dest.dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32;
        }
    }

    final static class Instruction_Reg8_value extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private int src;

        public Instruction_Reg8_value(Instruction instruction, Reg dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u8(instruction.run(cpu, dest.u8(), src));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name8+", "+src;
        }
    }

    final static class Instruction_Reg16_value extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private int src;

        public Instruction_Reg16_value(Instruction instruction, Reg dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u16(instruction.run(cpu, dest.u16(), src));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src;
        }
    }

    final static class Instruction_Reg32_value extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private int src;

        public Instruction_Reg32_value(Instruction instruction, Reg dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.dword=instruction.run(cpu, dest.dword, src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src;
        }
    }

    final static class Instruction_Reg8_Reg8 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;

        public Instruction_Reg8_Reg8(Instruction instruction, Reg dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u8(instruction.run(cpu, dest.u8(), src.u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name8+", "+src.name8;
        }
    }

    final static class Instruction_Reg16_Reg8 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;

        public Instruction_Reg16_Reg8(Instruction instruction, Reg dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u16(instruction.run(cpu, dest.u16(), src.u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src.name8;
        }
    }

    final static class Instruction_Reg16_Reg16 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;

        public Instruction_Reg16_Reg16(Instruction instruction, Reg dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u16(instruction.run(cpu, dest.u16(), src.u16()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src.name16;
        }
    }

    final static class Instruction_Reg32_Reg8 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;

        public Instruction_Reg32_Reg8(Instruction instruction, Reg dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.dword=instruction.run(cpu, dest.dword, src.u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src.name8;
        }
    }

    final static class Instruction_Reg32_Reg32 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;

        public Instruction_Reg32_Reg32(Instruction instruction, Reg dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.dword=instruction.run(cpu, dest.dword, src.dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src.name32;
        }
    }

    final static class Instruction_Reg16_Reg16_Value extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;
        final private int value;

        public Instruction_Reg16_Reg16_Value(Instruction instruction, Reg dest, Reg src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.u16(instruction.run(cpu, src.u16(), value));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src.name16+", "+value;
        }
    }

    final static class Instruction_Reg32_Reg32_Value extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;
        final private int value;

        public Instruction_Reg32_Reg32_Value(Instruction instruction, Reg dest, Reg src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.dword=instruction.run(cpu, src.dword, value);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src.name32+", "+value;
        }
    }

    final static class Instruction_Reg16_Reg16_Value_3 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;
        final private int value;

        public Instruction_Reg16_Reg16_Value_3(Instruction instruction, Reg dest, Reg src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.u16(instruction.run(cpu, dest.u16(), src.u16(), value));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src.name16+", "+value;
        }
    }

    final static class Instruction_Reg32_Reg32_Reg8 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;
        final private Reg value;

        public Instruction_Reg32_Reg32_Reg8(Instruction instruction, Reg dest, Reg src, Reg value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.dword=instruction.run(cpu, dest.dword, src.dword, value.u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src.name32+", "+value.name8;
        }
    }

    final static class Instruction_Reg16_Reg16_Reg8 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;
        final private Reg value;

        public Instruction_Reg16_Reg16_Reg8(Instruction instruction, Reg dest, Reg src, Reg value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.u16(instruction.run(cpu, dest.u16(), src.u16(), value.u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src.name16+", "+value.name8;
        }
    }

    final static class Instruction_Reg32_Reg32_Value_3 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;
        final private int value;

        public Instruction_Reg32_Reg32_Value_3(Instruction instruction, Reg dest, Reg src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.dword=instruction.run(cpu, dest.dword, src.dword, value);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src.name32+", "+value;
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
            memory.lock(address, 1, lock);
            memory.writeb(address, instruction.run(cpu, memory.readb(address)));
            memory.unlock(address, 1, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString8();
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
            memory.lock(address, 1, lock);
            memory.writeb(address, instruction.run(cpu, memory.readb(address), src));
            memory.unlock(address, 1, lock);
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
            memory.lock(address, 2, lock);
            memory.writew(address, instruction.run(cpu, memory.readw(address)));
            memory.unlock(address, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16();
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
            memory.lock(address, 2, lock);
            memory.writew(address, instruction.run(cpu, memory.readw(address), src));
            memory.unlock(address, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+src;
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
            memory.lock(address, 4, lock);
            memory.writed(address, instruction.run(cpu, memory.readd(address)));
            memory.unlock(address, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32();
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
            memory.lock(address, 4, lock);
            memory.writed(address, instruction.run(cpu, memory.readd(address), src));
            memory.unlock(address, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+src;
        }
    }

    final static class Instruction_Mem8_Reg8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;

        public Instruction_Mem8_Reg8(Instruction instruction, EaaBase dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 1, lock);
            memory.writeb(address, instruction.run(cpu, memory.readb(address), src.u8()));
            memory.unlock(address, 1, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString8()+", "+src.name8;
        }
    }

    final static class Instruction_Mem16_Reg8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;

        public Instruction_Mem16_Reg8(Instruction instruction, EaaBase dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 2, lock);
            memory.writew(address, instruction.run(cpu, memory.readw(address), src.u8()));
            memory.unlock(address, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+src.name8;
        }
    }

    final static class Instruction_Mem16_Reg16 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;

        public Instruction_Mem16_Reg16(Instruction instruction, EaaBase dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 2, lock);
            memory.writew(address, instruction.run(cpu, memory.readw(address), src.u16()));
            memory.unlock(address, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+src.name16;
        }
    }

    final static class Instruction_Mem32_Reg8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;

        public Instruction_Mem32_Reg8(Instruction instruction, EaaBase dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 4, lock);
            memory.writed(address, instruction.run(cpu, memory.readd(address), src.u8()));
            memory.unlock(address, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+src.name8;
        }
    }

    final static class Instruction_Mem32_Reg32 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;

        public Instruction_Mem32_Reg32(Instruction instruction, EaaBase dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 4, lock);
            memory.writed(address, instruction.run(cpu, memory.readd(address), src.dword));
            memory.unlock(address, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+src.name32;
        }
    }

    final static class Instruction_Mem16_Reg16_Reg8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;
        final private Reg value;

        public Instruction_Mem16_Reg16_Reg8(Instruction instruction, EaaBase dest, Reg src, Reg value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(eaa, 2, lock);
            memory.writew(eaa, instruction.run(cpu, memory.readw(eaa), src.u16(), value.u8()));
            memory.unlock(eaa, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+src.name16+", "+value.name8;
        }
    }

    final static class Instruction_Mem32_Reg32_Reg8 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;
        final private Reg value;

        public Instruction_Mem32_Reg32_Reg8(Instruction instruction, EaaBase dest, Reg src, Reg value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(eaa, 4, lock);
            memory.writed(eaa, instruction.run(cpu, memory.readd(eaa), src.dword, value.u8()));
            memory.unlock(eaa, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+src.name32+", "+value.name8;
        }
    }

    final static class Instruction_Mem16_Reg16_Value_3 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;
        final private int value;

        public Instruction_Mem16_Reg16_Value_3(Instruction instruction, EaaBase dest, Reg src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(eaa, 2, lock);
            memory.writew(eaa, instruction.run(cpu, memory.readw(eaa), src.u16(), value));
            memory.unlock(eaa, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+src.name16+", "+value;
        }
    }

    final static class Instruction_Mem32_Reg32_Value_3 extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;
        final private int value;

        public Instruction_Mem32_Reg32_Value_3(Instruction instruction, EaaBase dest, Reg src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            int eaa = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(eaa, 4, lock);
            memory.writed(eaa, instruction.run(cpu, memory.readd(eaa), src.dword, value));
            memory.unlock(eaa, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+src.name32+", "+value;
        }
    }

    final static class Instruction_Reg8_Mem8 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private EaaBase src;

        public Instruction_Reg8_Mem8(Instruction instruction, Reg dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u8(instruction.run(cpu, dest.u8(), cpu.memory.readb(src.call(cpu))));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name8+", "+src.toString8();
        }
    }

    final static class Instruction_Reg16_Mem16 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private EaaBase src;

        public Instruction_Reg16_Mem16(Instruction instruction, Reg dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u16(instruction.run(cpu, dest.u16(), cpu.memory.readw(src.call(cpu))));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src.toString16();
        }
    }

    final static class Instruction_Reg32_Mem32 extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private EaaBase src;

        public Instruction_Reg32_Mem32(Instruction instruction, Reg dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.dword=instruction.run(cpu, dest.dword, cpu.memory.readd(src.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src.toString32();
        }
    }

    final static class Instruction_Reg16_Mem16_Value extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private EaaBase src;
        final private int value;

        public Instruction_Reg16_Mem16_Value(Instruction instruction, Reg dest, EaaBase src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.u16(instruction.run(cpu, cpu.memory.readw(src.call(cpu)), value));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src.toString16()+", "+value;
        }
    }

    final static class Instruction_Reg32_Mem32_Value extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private EaaBase src;
        final private int value;

        public Instruction_Reg32_Mem32_Value(Instruction instruction, Reg dest, EaaBase src, int value) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.dword=instruction.run(cpu, cpu.memory.readd(src.call(cpu)), value);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src.toString32()+", "+value;
        }
    }

    final static class Instruction_Reg8_noresult extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private int src;

        public Instruction_Reg8_noresult(Instruction instruction, Reg dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, dest.u8(), src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name8+", "+src;
        }
    }

    final static class Instruction_Reg16_noresult extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private int src;

        public Instruction_Reg16_noresult(Instruction instruction, Reg dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, dest.u16(), src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src;
        }
    }

    final static class Instruction_Reg32_noresult extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private int src;

        public Instruction_Reg32_noresult(Instruction instruction, Reg dest, int src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, dest.dword, src);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src;
        }
    }

    final static class Instruction_Reg8_Reg8_noresult extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;

        public Instruction_Reg8_Reg8_noresult(Instruction instruction, Reg dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, dest.u8(), src.u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name8+", "+src.name8;
        }
    }

    final static class Instruction_Reg16_Reg16_noresult extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;

        public Instruction_Reg16_Reg16_noresult(Instruction instruction, Reg dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, dest.u16(), src.u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src.name16;
        }
    }

    final static class Instruction_Reg32_Reg32_noresult extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private Reg src;

        public Instruction_Reg32_Reg32_noresult(Instruction instruction, Reg dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, dest.dword, src.dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src.name32;
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
            return instruction.toString()+" "+dest.toString8()+", "+src;
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
            return instruction.toString()+" "+dest.toString16()+", "+src;
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
            return instruction.toString()+" "+dest.toString32()+", "+src;
        }
    }

    final static class Instruction_Mem8_Reg8_noresult extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;

        public Instruction_Mem8_Reg8_noresult(Instruction instruction, EaaBase dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.memory.readb(dest.call(cpu)), src.u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString8()+", "+src.name8;
        }
    }

    final static class Instruction_Mem16_Reg16_noresult extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;

        public Instruction_Mem16_Reg16_noresult(Instruction instruction, EaaBase dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.memory.readw(dest.call(cpu)), src.u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString16()+", "+src.name16;
        }
    }

    final static class Instruction_Mem32_Reg32_noresult extends Op {
        final private Instruction instruction;
        final private EaaBase dest;
        final private Reg src;

        public Instruction_Mem32_Reg32_noresult(Instruction instruction, EaaBase dest, Reg src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, cpu.memory.readd(dest.call(cpu)), src.dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.toString32()+", "+src.name32;
        }
    }

    final static class Instruction_Reg8_Mem8_noresult extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private EaaBase src;

        public Instruction_Reg8_Mem8_noresult(Instruction instruction, Reg dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, dest.u8(), cpu.memory.readb(src.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name8+", "+src.toString8();
        }
    }

    final static class Instruction_Reg16_Mem16_noresult extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private EaaBase src;

        public Instruction_Reg16_Mem16_noresult(Instruction instruction, Reg dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, dest.u16(), cpu.memory.readw(src.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name16+", "+src.toString16();
        }
    }

    final static class Instruction_Reg32_Mem32_noresult extends Op {
        final private Instruction instruction;
        final private Reg dest;
        final private EaaBase src;

        public Instruction_Reg32_Mem32_noresult(Instruction instruction, Reg dest, EaaBase src) {
            this.instruction = instruction;
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            instruction.run(cpu, dest.dword, cpu.memory.readd(src.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return instruction.toString()+" "+dest.name32+", "+src.toString32();
        }
    }

    final static class PushReg16 extends Op {
        final private Reg src;

        public PushReg16(Reg src) {
            this.src = src;
        }

        public Block call(CPU cpu) {
            cpu.push16(src.u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+src.name16;
        }
    }

    final static class PushReg32 extends Op {
        final private Reg src;

        public PushReg32(Reg src) {
            this.src = src;
        }

        public Block call(CPU cpu) {
            cpu.push32(src.dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+src.name32;
        }
    }

    final static class PopReg16 extends Op {
        final private Reg dest;

        public PopReg16(Reg dest) {
            this.dest = dest;
        }

        public Block call(CPU cpu) {
            dest.u16(cpu.pop16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pop "+dest.name16;
        }
    }

    final static class PopReg32 extends Op {
        final private Reg dest;

        public PopReg32(Reg dest) {
            this.dest = dest;
        }

        public Block call(CPU cpu) {
            dest.dword=cpu.pop32();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pop "+dest.name32;
        }
    }

    final static class PopMem16 extends Op {
        final private EaaBase dest;

        public PopMem16(EaaBase dest) {
            this.dest = dest;
        }

        public Block call(CPU cpu) {
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 2, lock);
            memory.writew(address, cpu.pop16());
            memory.unlock(address, 2, lock);
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
            int address = dest.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 4, lock);
            memory.writed(address, cpu.pop32());
            memory.unlock(address, 4, lock);
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
            return "j"+cond.toString()+" "+Integer.toHexString(offset+block.eip+block.eip_count);
        }
    }

    final static class LeaGw extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public LeaGw(EaaBase eaa, Reg dest) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.u16(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lea "+dest.name16+", "+eaa.toString();
        }
    }

    final static class LeaGd extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public LeaGd(EaaBase eaa, Reg dest) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.dword=eaa.call(cpu);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lea "+dest.name32+", "+eaa.toString();
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
        final private Reg reg1;
        final private Reg reg2;

        public Exchange_Reg8_Reg8(Reg reg1, Reg reg2) {
            this.reg1 = reg1;
            this.reg2 = reg2;
        }
        public Block call(CPU cpu) {
            int tmp = reg1.u8();
            reg1.u8(reg2.u8());
            reg2.u8(tmp);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xchg "+reg1.name8+", "+reg2.name8;
        }
    }

    final static class Exchange_Mem8_Reg8 extends Op {
        final private EaaBase eaa;
        final private Reg reg;

        public Exchange_Mem8_Reg8(EaaBase eaa, Reg reg) {
            this.eaa = eaa;
            this.reg = reg;
        }
        public Block call(CPU cpu) {
            int addresss = eaa.call(cpu);
            int tmp = reg.u8();
            Memory memory = cpu.memory;
            memory.lock(addresss, 1, lock);
            reg.u8(memory.readb(addresss));
            memory.writeb(addresss, tmp);
            memory.unlock(addresss, 1, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xchg "+eaa.toString8()+", "+reg.name8;
        }
    }

    final static class Exchange_Reg16_Reg16 extends Op {
        final private Reg reg1;
        final private Reg reg2;

        public Exchange_Reg16_Reg16(Reg reg1, Reg reg2) {
            this.reg1 = reg1;
            this.reg2 = reg2;
        }
        public Block call(CPU cpu) {
            int tmp = reg1.u16();
            reg1.u16(reg2.u16());
            reg2.u16(tmp);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xchg "+reg1.name16+", "+reg2.name16;
        }
    }

    final static class Exchange_Mem16_Reg16 extends Op {
        final private EaaBase eaa;
        final private Reg reg;

        public Exchange_Mem16_Reg16(EaaBase eaa, Reg reg) {
            this.eaa = eaa;
            this.reg = reg;
        }
        public Block call(CPU cpu) {
            int addresss = eaa.call(cpu);
            int tmp = reg.u16();
            Memory memory = cpu.memory;
            memory.lock(addresss, 2, lock);
            reg.u16(memory.readw(addresss));
            memory.writew(addresss, tmp);
            memory.unlock(addresss, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xchg "+eaa.toString16()+", "+reg.name16;
        }
    }

    final static class Exchange_Reg32_Reg32 extends Op {
        final private Reg reg1;
        final private Reg reg2;

        public Exchange_Reg32_Reg32(Reg reg1, Reg reg2) {
            this.reg1 = reg1;
            this.reg2 = reg2;
        }
        public Block call(CPU cpu) {
            int tmp = reg1.dword;
            reg1.dword=reg2.dword;
            reg2.dword=tmp;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xchg "+reg1.name32+", "+reg2.name32;
        }
    }

    final static class Exchange_Mem32_Reg32 extends Op {
        final private EaaBase eaa;
        final private Reg reg;

        public Exchange_Mem32_Reg32(EaaBase eaa, Reg reg) {
            this.eaa = eaa;
            this.reg = reg;
        }
        public Block call(CPU cpu) {
            int addresss = eaa.call(cpu);
            int tmp = reg.dword;
            Memory memory = cpu.memory;
            memory.lock(addresss, 4, lock);
            reg.dword=memory.readd(addresss);
            memory.writed(addresss, tmp);
            memory.unlock(addresss, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xchg "+eaa.toString32()+", "+reg.name32;
        }
    }

    final static class Mov_Reg8 extends Op {
        final private Reg dest;
        final private int value;

        public Mov_Reg8(Reg dest, int value) {
            this.dest = dest;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.u8(value);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name8+", "+value;
        }
    }

    final static class Mov_Reg8_Reg8 extends Op {
        final private Reg dest;
        final private Reg src;

        public Mov_Reg8_Reg8(Reg dest, Reg src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u8(src.u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name8+", "+src.name8;
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
            int address = eaa.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 1, lock);
            memory.writeb(address, src);
            memory.unlock(address, 1, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString8()+", "+src;
        }
    }

    final static class Mov_Mem8_Reg8 extends Op {
        final private EaaBase eaa;
        final private Reg src;

        public Mov_Mem8_Reg8(EaaBase eaa, Reg src) {
            this.eaa = eaa;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = eaa.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 1, lock);
            memory.writeb(address, src.u8());
            memory.unlock(address, 1, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString8()+", "+src.name8;
        }
    }

    final static class Mov_Reg8_Mem8 extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public Mov_Reg8_Mem8(Reg dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.u8(cpu.memory.readb(eaa.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name8+", "+eaa.toString8();
        }
    }

    final static class Mov_Reg16 extends Op {
        final private Reg dest;
        final private int value;

        public Mov_Reg16(Reg dest, int value) {
            this.dest = dest;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.u16(value);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name16+", "+value;
        }
    }

    final static class Mov_Reg16_Reg8 extends Op {
        final private Reg dest;
        final private Reg src;

        public Mov_Reg16_Reg8(Reg dest, Reg src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u16(src.u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name32+", "+src.name8;
        }
    }

    final static class Mov_Reg16_Reg8_sx extends Op {
        final private Reg dest;
        final private Reg src;

        public Mov_Reg16_Reg8_sx(Reg dest, Reg src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u16((byte)src.u8());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+dest.name16+", "+src.name8;
        }
    }

    final static class Mov_Reg16_Reg16 extends Op {
        final private Reg dest;
        final private Reg src;

        public Mov_Reg16_Reg16(Reg dest, Reg src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.u16(src.u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name16+", "+src.name16;
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
            int address = eaa.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 2, lock);
            memory.writew(address, src);
            memory.unlock(address, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString16()+", "+src;
        }
    }

    final static class Mov_Mem16_Reg16 extends Op {
        final private EaaBase eaa;
        final private Reg src;

        public Mov_Mem16_Reg16(EaaBase eaa, Reg src) {
            this.eaa = eaa;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = eaa.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 2, lock);
            memory.writew(address, src.u16());
            memory.unlock(address, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString16()+", "+src.name16;
        }
    }

    final static class Mov_Reg16_Mem8 extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public Mov_Reg16_Mem8(Reg dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.u16(cpu.memory.readb(eaa.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name32+", "+eaa.toString8();
        }
    }

    final static class Mov_Reg16_Mem8_sx extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public Mov_Reg16_Mem8_sx(Reg dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.u16((byte)cpu.memory.readb(eaa.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+dest.name16+", "+eaa.toString8();
        }
    }

    final static class Mov_Reg16_Mem16 extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public Mov_Reg16_Mem16(Reg dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.u16(cpu.memory.readw(eaa.call(cpu)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name16+", "+eaa.toString16();
        }
    }

    final static class Mov_Reg32 extends Op {
        final private Reg dest;
        final private int value;

        public Mov_Reg32(Reg dest, int value) {
            this.dest = dest;
            this.value = value;
        }
        public Block call(CPU cpu) {
            dest.dword=value;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name32+", "+value;
        }
    }

    final static class Mov_Reg32_Reg8 extends Op {
        final private Reg dest;
        final private Reg src;

        public Mov_Reg32_Reg8(Reg dest, Reg src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.dword=src.u8();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name32+", "+src.name8;
        }
    }

    final static class Mov_Reg32_Reg8_sx extends Op {
        final private Reg dest;
        final private Reg src;

        public Mov_Reg32_Reg8_sx(Reg dest, Reg src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.dword=(byte)src.u8();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+dest.name32+", "+src.name8;
        }
    }

    final static class Mov_Reg32_Reg16 extends Op {
        final private Reg dest;
        final private Reg src;

        public Mov_Reg32_Reg16(Reg dest, Reg src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.dword=src.u16();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name32+", "+src.name16;
        }
    }

    final static class Mov_Reg32_Reg16_sx extends Op {
        final private Reg dest;
        final private Reg src;

        public Mov_Reg32_Reg16_sx(Reg dest, Reg src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.dword=(short)src.u16();
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+dest.name32+", "+src.name16;
        }
    }

    final static class Mov_Reg32_Reg32 extends Op {
        final private Reg dest;
        final private Reg src;

        public Mov_Reg32_Reg32(Reg dest, Reg src) {
            this.dest = dest;
            this.src = src;
        }
        public Block call(CPU cpu) {
            dest.dword=src.dword;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name32+", "+src.name32;
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
            int address = eaa.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 4, lock);
            memory.writed(address, src);
            memory.unlock(address, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString32()+", "+src;
        }
    }

    final static class Mov_Mem32_Reg32 extends Op {
        final private EaaBase eaa;
        final private Reg src;

        public Mov_Mem32_Reg32(EaaBase eaa, Reg src) {
            this.eaa = eaa;
            this.src = src;
        }
        public Block call(CPU cpu) {
            int address = eaa.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 4, lock);
            memory.writed(address, src.dword);
            memory.unlock(address, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+eaa.toString32()+", "+src.name32;
        }
    }

    final static class Mov_Reg32_Mem8 extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public Mov_Reg32_Mem8(Reg dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.dword=cpu.memory.readb(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name32+", "+eaa.toString8();
        }
    }

    final static class Mov_Reg32_Mem8_sx extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public Mov_Reg32_Mem8_sx(Reg dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.dword=(byte)cpu.memory.readb(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+dest.name32+", "+eaa.toString8();
        }
    }

    final static class Mov_Reg32_Mem16 extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public Mov_Reg32_Mem16(Reg dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.dword=cpu.memory.readw(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name32+", "+eaa.toString16();
        }
    }

    final static class Mov_Reg32_Mem16_sx extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public Mov_Reg32_Mem16_sx(Reg dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.dword=(short)cpu.memory.readw(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "movsx "+dest.name32+", "+eaa.toString16();
        }
    }

    final static class Mov_Reg32_Mem32 extends Op {
        final private EaaBase eaa;
        final private Reg dest;

        public Mov_Reg32_Mem32(Reg dest, EaaBase eaa) {
            this.eaa = eaa;
            this.dest = dest;
        }
        public Block call(CPU cpu) {
            dest.dword=cpu.memory.readd(eaa.call(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name32+", "+eaa.toString32();
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
        final private Reg dest;
        final private Reg base;

        public Mov_Reg8_0b(Reg dest, Reg base, int address) {
            this.address = address;
            this.dest = dest;
            this.base = base;
        }
        public Block call(CPU cpu) {
            dest.u8(cpu.memory.readb(base.dword+address));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name8+", "+base.toString()+":"+address;
        }
    }

    final static class Mov_Reg16_0w extends Op {
        final private int address;
        final private Reg dest;
        final private Reg base;

        public Mov_Reg16_0w(Reg dest, Reg base, int address) {
            this.address = address;
            this.dest = dest;
            this.base = base;
        }
        public Block call(CPU cpu) {
            dest.u16(cpu.memory.readw(base.dword + address));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name16+", "+base.toString()+":"+address;
        }
    }

    final static class Mov_Reg32_0d extends Op {
        final private int address;
        final private Reg dest;
        final private Reg base;

        public Mov_Reg32_0d(Reg dest, Reg base, int address) {
            this.address = address;
            this.dest = dest;
            this.base = base;
        }
        public Block call(CPU cpu) {
            dest.dword=cpu.memory.readd(base.dword+address);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+dest.name32+", "+base.toString()+":"+address;
        }
    }

    final static class Mov_0b_Reg8 extends Op {
        final private int address;
        final private Reg src;
        final private Reg base;

        public Mov_0b_Reg8(Reg base, int address, Reg src) {
            this.address = address;
            this.src = src;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int address = base.dword + this.address;
            Memory memory = cpu.memory;
            memory.lock(address, 1, lock);
            memory.writeb(address, src.u8());
            memory.unlock(address, 1, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+base.toString()+":"+address+", "+src.name8;
        }
    }

    final static class Mov_0w_Reg16 extends Op {
        final private int address;
        final private Reg src;
        final private Reg base;

        public Mov_0w_Reg16(Reg base, int address, Reg src) {
            this.address = address;
            this.src = src;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int address = base.dword + this.address;
            Memory memory = cpu.memory;
            memory.lock(address, 2, lock);
            memory.writew(address, src.u16());
            memory.unlock(address, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+base.toString()+":"+address+", "+src.name16;
        }
    }

    final static class Mov_0d_Reg32 extends Op {
        final private int address;
        final private Reg src;
        final private Reg base;

        public Mov_0d_Reg32(Reg base, int address, Reg src) {
            this.address = address;
            this.src = src;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int address = base.dword + this.address;
            Memory memory = cpu.memory;
            memory.lock(address, 4, lock);
            memory.writed(address, src.dword);
            memory.unlock(address, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+base.toString()+":"+address+", "+src.name32;
        }
    }

    final static class Movsb32 extends Op {
        final private int add_index;
        final private Reg base;

        public Movsb32(int add_index, Reg base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writeb(cpu.es.dword + cpu.edi.dword, cpu.memory.readb(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Movsb32_r(int add_index, Reg base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.memory.writeb(cpu.es.dword + cpu.edi.dword, cpu.memory.readb(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Movsb16(int add_index, Reg base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writeb(cpu.es.dword + cpu.edi.u16(), cpu.memory.readb(base.dword + cpu.esi.u16()));
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
        final private Reg base;

        public Movsb16_r(int add_index, Reg base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.memory.writeb(cpu.es.dword + cpu.edi.dword, cpu.memory.readb(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Movsw32(int add_index, Reg base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writew(cpu.es.dword + cpu.edi.dword, cpu.memory.readw(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Movsw32_r(int add_index, Reg base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.memory.writew(cpu.es.dword + cpu.edi.dword, cpu.memory.readw(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Movsw16(int add_index, Reg base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writew(cpu.es.dword + cpu.edi.u16(), cpu.memory.readw(base.dword + cpu.esi.u16()));
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
        final private Reg base;

        public Movsw16_r(int add_index, Reg base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.memory.writew(cpu.es.dword + cpu.edi.dword, cpu.memory.readw(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Movsd32(int add_index, Reg base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writed(cpu.es.dword + cpu.edi.dword, cpu.memory.readd(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Movsd32_r(int add_index, Reg base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.memory.writed(cpu.es.dword + cpu.edi.dword, cpu.memory.readd(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Movsd16(int add_index, Reg base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.memory.writed(cpu.es.dword + cpu.edi.u16(), cpu.memory.readd(base.dword + cpu.esi.u16()));
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
        final private Reg base;

        public Movsd16_r(int add_index, Reg base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.memory.writed(cpu.es.dword + cpu.edi.dword, cpu.memory.readd(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Cmpsb32(int add_index, Reg base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readb(base.dword + cpu.esi.dword);
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
        final private Reg base;
        final private boolean repeat_zero;

        public Cmpsb32_r(int add_index, Reg base, boolean repeat_zero) {
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
                    v1 = cpu.memory.readb(base.dword + cpu.esi.dword);
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
        final private Reg base;

        public Cmpsb16(int add_index, Reg base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readb(base.dword + cpu.esi.u16());
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
        final private Reg base;
        final private boolean repeat_zero;

        public Cmpsb16_r(int add_index, Reg base, boolean repeat_zero) {
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
                    v1 = cpu.memory.readb(base.dword + cpu.esi.u16());
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
        final private Reg base;

        public Cmpsw32(int add_index, Reg base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readw(base.dword + cpu.esi.dword);
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
        final private Reg base;
        final private boolean repeat_zero;

        public Cmpsw32_r(int add_index, Reg base, boolean repeat_zero) {
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
                    v1 = cpu.memory.readw(base.dword + cpu.esi.dword);
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
        final private Reg base;

        public Cmpsw16(int add_index, Reg base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readw(base.dword + cpu.esi.u16());
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
        final private Reg base;
        final private boolean repeat_zero;

        public Cmpsw16_r(int add_index, Reg base, boolean repeat_zero) {
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
                    v1 = cpu.memory.readw(base.dword + cpu.esi.u16());
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
        final private Reg base;

        public Cmpsd32(int add_index, Reg base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readd(base.dword + cpu.esi.dword);
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
        final private Reg base;
        final private boolean repeat_zero;

        public Cmpsd32_r(int add_index, Reg base, boolean repeat_zero) {
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
                    v1 = cpu.memory.readd(base.dword + cpu.esi.dword);
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
        final private Reg base;

        public Cmpsd16(int add_index, Reg base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int v1 = cpu.memory.readd(base.dword + cpu.esi.u16());
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
        final private Reg base;
        final private boolean repeat_zero;

        public Cmpsd16_r(int add_index, Reg base, boolean repeat_zero) {
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
                    v1 = cpu.memory.readd(base.dword + cpu.esi.u16());
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
        final private Reg base;

        public Lodsb32(int add_index, Reg base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u8(cpu.memory.readb(base.dword + cpu.esi.dword));
            cpu.esi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsb";
        }
    }

    final static class Lodsb32_r extends Op {
        final private int add_index;
        final private Reg base;

        public Lodsb32_r(int add_index, Reg base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.eax.u8(cpu.memory.readb(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Lodsb16(int add_index, Reg base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u8(cpu.memory.readb(base.dword + cpu.esi.u16()));
            cpu.esi.u16(cpu.esi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsb";
        }
    }

    final static class Lodsb16_r extends Op {
        final private int add_index;
        final private Reg base;

        public Lodsb16_r(int add_index, Reg base) {
            this.add_index = add_index;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.eax.u8(cpu.memory.readb(base.dword + cpu.esi.u16()));
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
        final private Reg base;

        public Lodsw32(int add_index, Reg base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u16(cpu.memory.readw(base.dword + cpu.esi.dword));
            cpu.esi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsw";
        }
    }

    final static class Lodsw32_r extends Op {
        final private int add_index;
        final private Reg base;

        public Lodsw32_r(int add_index, Reg base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.eax.u16(cpu.memory.readw(base.dword + cpu.esi.dword));
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
        final private Reg base;

        public Lodsw16(int add_index, Reg base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u16(cpu.memory.readw(base.dword + cpu.esi.u16()));
            cpu.esi.u16(cpu.esi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsw";
        }
    }

    final static class Lodsw16_r extends Op {
        final private int add_index;
        final private Reg base;

        public Lodsw16_r(int add_index, Reg base) {
            this.add_index = add_index << 1;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.eax.u16(cpu.memory.readw(base.dword + cpu.esi.u16()));
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
        final private Reg base;

        public Lodsd32(int add_index, Reg base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.dword=cpu.memory.readd(base.dword + cpu.esi.dword);
            cpu.esi.dword+=add_index;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsd";
        }
    }

    final static class Lodsd32_r extends Op {
        final private int add_index;
        final private Reg base;

        public Lodsd32_r(int add_index, Reg base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            long count = cpu.ecx.dword & 0xFFFFFFFFl;
            for (int i=0;i<count;i++) {
                cpu.eax.dword=cpu.memory.readd(base.dword + cpu.esi.dword);
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
        final private Reg base;

        public Lodsd16(int add_index, Reg base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.dword=cpu.memory.readd(base.dword + cpu.esi.u16());
            cpu.esi.u16(cpu.esi.u16()+add_index);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "lodsd";
        }
    }

    final static class Lodsd16_r extends Op {
        final private int add_index;
        final private Reg base;

        public Lodsd16_r(int add_index, Reg base) {
            this.add_index = add_index << 2;
            this.base = base;
        }
        public Block call(CPU cpu) {
            int count = cpu.ecx.u16();
            for (int i=0;i<count;i++) {
                cpu.eax.dword=cpu.memory.readd(base.dword + cpu.esi.u16());
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
        final private Reg base;
        public Xlat32(Reg base) {
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u8(cpu.memory.readb(base.dword + cpu.ebx.dword + cpu.eax.u8()));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return base.toString()+":xlat";
        }
    }

    static public final class Xlat16 extends Op {
        final private Reg base;
        public Xlat16(Reg base) {
            this.base = base;
        }
        public Block call(CPU cpu) {
            cpu.eax.u8(cpu.memory.readb(base.dword + ((cpu.ebx.u16() + cpu.eax.u8()) & 0xFFFF)));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return base.toString()+":xlat";
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
            return "call "+Integer.toHexString(offset+block.eip+block.eip_count);
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
            return "call "+Integer.toHexString(offset+block.eip+block.eip_count);
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
            return "jmp 0x"+Integer.toHexString(offset+block.eip+block.eip_count);
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
            return "jmp 0x"+Integer.toHexString(offset+block.eip+block.eip_count);
        }
    }

    static public final class Cmc extends Op {
        public Block call(CPU cpu) {
            cpu.fillFlags();
            if (cpu.CF())
                cpu.flags |= CPU.CF;
            else
                cpu.flags &= ~CPU.CF;
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
        private final Reg reg;

        public CallEv_reg(Reg reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            int old = cpu.eip + block.eip_count;
            cpu.push16(old);
            cpu.eip = reg.u16();
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "call "+reg.name16;
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
        private final Reg reg;

        public JmpEv_reg(Reg reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            cpu.eip = reg.u16();
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "jmp "+reg.name16;
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
        private final Reg reg;

        public PushEv_reg(Reg reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            cpu.push16(reg.u16());
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+reg.name16;
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
        private final Reg reg;

        public CallNearEd_reg(Reg reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            int old = cpu.eip + block.eip_count;
            cpu.push32(old);
            cpu.callIndex++;
            cpu.eip = reg.dword;
            if (!cpu.big)
                cpu.eip &= 0xFFFF;
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "call "+reg.name32;
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
        private final Reg reg;

        public JmpNearEd_reg(Reg reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            cpu.eip = reg.dword;
            if (!cpu.big)
                cpu.eip &= 0xFFFF;
            return null; // don't link this call with a super.jump since eip can be variable
        }
        public String toString() {
            return "jmp "+reg.name32;
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
        private final Reg reg;

        public PushEd_reg(Reg reg) {
            this.reg = reg;
        }

        public Block call(CPU cpu) {
            cpu.push32(reg.dword);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "push "+reg.name32;
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
        final private Reg reg;
        final private Condition condition;

        public SetFlag_Reg8(Reg reg, Condition condition) {
            this.reg = reg;
            this.condition = condition;
        }
        public Block call(CPU cpu) {
            if (condition.test(cpu))
                reg.u8(1);
            else
                reg.u8(0);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "set"+condition.toString()+" "+reg.name8;
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
            int address = eaa.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(address, 1, lock);
            if (condition.test(cpu))
                cpu.memory.writeb(address, 1);
            else
                cpu.memory.writeb(address, 0);
            memory.unlock(address, 1, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "set"+condition.toString()+" "+eaa.toString8();
        }
    }

    static public final class BtEwGw_reg extends Op {
        final private Reg ew;
        final private Reg gw;

        public BtEwGw_reg(Reg ew, Reg gw) {
            this.ew = ew;
            this.gw = gw;
        }
        public Block call(CPU cpu) {
            cpu.fillFlagsNoCF();
            int mask=1 << (gw.u16() & 15);
            if ((ew.u16() & mask)!=0) {
                cpu.flags|=CPU.CF;
            } else {
                cpu.flags&=~CPU.CF;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bt "+ew.name16+", "+gw.name16;
        }
    }

    static public final class BtEwGw_mem extends Op {
        final private EaaBase eaa;
        final private Reg gw;

        public BtEwGw_mem(EaaBase eaa, Reg gw) {
            this.eaa = eaa;
            this.gw = gw;
        }
        public Block call(CPU cpu) {
            cpu.fillFlagsNoCF();
            int mask=1 << (gw.u16() & 15);
            int address = eaa.call(cpu);

            address+=(((short)gw.u16())>>4)*2;
            if ((cpu.memory.readw(address) & mask)!=0) {
                cpu.flags|=CPU.CF;
            } else {
                cpu.flags&=~CPU.CF;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bt "+eaa.toString16()+", "+gw.name16;
        }
    }

    static public final class BtEdGd_reg extends Op {
        final private Reg ed;
        final private Reg gd;

        public BtEdGd_reg(Reg ed, Reg gd) {
            this.ed = ed;
            this.gd = gd;
        }
        public Block call(CPU cpu) {
            cpu.fillFlagsNoCF();
            int mask=1 << (gd.dword & 31);
            if ((ed.dword & mask)!=0) {
                cpu.flags|=CPU.CF;
            } else {
                cpu.flags&=~CPU.CF;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bt "+ed.name32+", "+gd.name32;
        }
    }

    static public final class BtEdGd_mem extends Op {
        final private EaaBase eaa;
        final private Reg gd;

        public BtEdGd_mem(EaaBase eaa, Reg gd) {
            this.eaa = eaa;
            this.gd = gd;
        }
        public Block call(CPU cpu) {
            cpu.fillFlagsNoCF();
            int mask=1 << (gd.dword & 31);
            int address = eaa.call(cpu);

            address+=(gd.dword>>5)*4; // intentional signed shift
            if ((cpu.memory.readd(address) & mask)!=0) {
                cpu.flags|=CPU.CF;
            } else {
                cpu.flags&=~CPU.CF;
            }
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "bt "+eaa.toString32()+", "+gd.name32;
        }
    }

    static public final class Mov_Reg16_Seg extends Op {
        final private Reg segValue;
        final private Reg segPhys;
        final private Reg reg;

        public Mov_Reg16_Seg(Reg segPhys, Reg segValue, Reg reg) {
            this.segPhys = segPhys;
            this.segValue = segValue;
            this.reg = reg;
        }
        public Block call(CPU cpu) {
            segValue.dword = reg.u16();
            segPhys.dword = cpu.ldt[reg.u16() >> 3];
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+segPhys.name32+", "+reg.name16;
        }
    }

    static public final class Mov_Mem16_Seg extends Op {
        final private Reg segValue;
        final private Reg segPhys;
        final private EaaBase eaa;

        public Mov_Mem16_Seg(Reg segPhys, Reg segValue, EaaBase eaa) {
            this.segPhys = segPhys;
            this.segValue = segValue;
            this.eaa = eaa;
        }
        public Block call(CPU cpu) {
            int sel = cpu.memory.readw(eaa.call(cpu));
            segValue.dword = sel;
            segPhys.dword = cpu.ldt[sel];
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "mov "+segPhys.name32+", "+eaa.toString16();
        }
    }

    final static class Xadd_Reg8_Reg8 extends Op {
        final private Reg gb;
        final private Reg eb;

        public Xadd_Reg8_Reg8(Reg gb, Reg eb) {
            this.gb = gb;
            this.eb = eb;
        }
        public Block call(CPU cpu) {
            int result=Instructions.addb.run(cpu, gb.u8(), eb.u8());
            gb.u8(eb.u8());
            eb.u8(result);

            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+gb.name8+", "+eb.name8;
        }
    }

    final static class Xadd_Reg8_Mem8 extends Op {
        final private Reg gb;
        final private EaaBase eb;

        public Xadd_Reg8_Mem8(Reg gb, EaaBase eb) {
            this.gb = gb;
            this.eb = eb;
        }
        public Block call(CPU cpu) {
            int eaa=eb.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(eaa, 1, lock);
            int val = memory.readb(eaa);
            int result = Instructions.addb.run(cpu, gb.u8(), val);
            memory.writeb(eaa,result);
            memory.unlock(eaa, 1, lock);
            gb.u8(val);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+gb.name8+", "+eb.toString8();
        }
    }

    final static class Xadd_Reg16_Reg16 extends Op {
        final private Reg gw;
        final private Reg ew;

        public Xadd_Reg16_Reg16(Reg gw, Reg ew) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            int result=Instructions.addw.run(cpu, gw.u16(), ew.u16());
            gw.u16(ew.u16());
            ew.u16(result);

            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+gw.name16+", "+ew.name16;
        }
    }

    final static class Xadd_Reg16_Mem16 extends Op {
        final private Reg gw;
        final private EaaBase ew;

        public Xadd_Reg16_Mem16(Reg gw, EaaBase ew) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            int eaa=ew.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(eaa, 2, lock);
            int val = memory.readw(eaa);
            int result = Instructions.addw.run(cpu, gw.u16(), val);
            memory.writew(eaa,result);
            memory.unlock(eaa, 2, lock);
            gw.u16(val);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+gw.name16+", "+ew.toString16();
        }
    }

    final static class Xadd_Reg32_Reg32 extends Op {
        final private Reg gd;
        final private Reg ed;

        public Xadd_Reg32_Reg32(Reg gd, Reg ed) {
            this.gd = gd;
            this.ed = ed;
        }
        public Block call(CPU cpu) {
            int result=Instructions.addd.run(cpu, gd.dword, ed.dword);
            gd.dword=ed.dword;
            ed.dword=result;

            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+gd.name32+", "+ed.name32;
        }
    }

    final static class Xadd_Reg32_Mem32 extends Op {
        final private Reg gd;
        final private EaaBase ed;

        public Xadd_Reg32_Mem32(Reg gd, EaaBase ed) {
            this.gd = gd;
            this.ed = ed;
        }
        public Block call(CPU cpu) {
            int eaa=ed.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(eaa, 4, lock);
            int val = memory.readd(eaa);
            int result = Instructions.addd.run(cpu, gd.dword, val);
            memory.writed(eaa,result);
            memory.unlock(eaa, 4, lock);
            gd.dword=val;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "xadd "+gd.name32+", "+ed.toString32();
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
        final private Reg gd;
        final private Reg ed;

        public CmpxchgEdGd_reg(Reg ed, Reg gd) {
            this.gd = gd;
            this.ed = ed;
        }
        public Block call(CPU cpu) {
            Instructions.subd.run(cpu, ed.dword, cpu.eax.dword);
            cpu.fillFlags();
            if (cpu.eax.dword == ed.dword) {
                ed.dword=gd.dword;
                cpu.flags|=CPU.ZF;
            } else {
                cpu.eax.dword=ed.dword;
                cpu.flags&=~CPU.ZF;
            }

            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpxchg "+ed.name32+", "+gd.name32;
        }
    }

    final static class CmpxchgEdGd_mem extends Op {
        final private Reg gd;
        final private EaaBase ed;

        public CmpxchgEdGd_mem(EaaBase ed, Reg gd) {
            this.gd = gd;
            this.ed = ed;
        }
        public Block call(CPU cpu) {
            int eaa=ed.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(eaa, 4, lock);
            int val = memory.readd(eaa);
            Instructions.subd.run(cpu, val, cpu.eax.dword);
            cpu.fillFlags();
            if (cpu.eax.dword == val) {
                memory.writed(eaa,gd.dword);
                cpu.flags|=CPU.ZF;
            } else {
                memory.writed(eaa,val);	// cmpxchg always issues a write
                cpu.eax.dword=val;
                cpu.flags&=~CPU.ZF;
            }
            memory.unlock(eaa, 4, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpxchg "+ed.toString32()+", "+gd.name32;
        }
    }

    final static class CmpxchgEwGw_reg extends Op {
        final private Reg gw;
        final private Reg ew;

        public CmpxchgEwGw_reg(Reg ew, Reg gw) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            Instructions.subw.run(cpu, ew.u16(), cpu.eax.u16());
            cpu.fillFlags();
            if (cpu.eax.u16() == ew.u16()) {
                ew.u16(gw.u16());
                cpu.flags|=CPU.ZF;
            } else {
                cpu.eax.u16(ew.u16());
                cpu.flags&=~CPU.ZF;
            }

            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpxchg "+ew.name16+", "+gw.name16;
        }
    }

    final static class CmpxchgEwGw_mem extends Op {
        final private Reg gw;
        final private EaaBase ew;

        public CmpxchgEwGw_mem(EaaBase ew, Reg gw) {
            this.gw = gw;
            this.ew = ew;
        }
        public Block call(CPU cpu) {
            int eaa=ew.call(cpu);
            Memory memory = cpu.memory;
            memory.lock(eaa, 2, lock);
            int val = memory.readw(eaa);
            Instructions.subw.run(cpu, val, cpu.eax.u16());
            cpu.fillFlags();
            if (cpu.eax.u16() == val) {
                memory.writew(eaa,gw.u16());
                cpu.flags|=CPU.ZF;
            } else {
                memory.writew(eaa,val);	// cmpxchg always issues a write
                cpu.eax.u16(val);
                cpu.flags&=~CPU.ZF;
            }
            memory.unlock(eaa, 2, lock);
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmpxchg "+ew.toString16()+", "+gw.name16;
        }
    }

    static public class RegGetter implements wine.builtin.libc.Syscall.SyscallGetter {
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
            cpu.eax.dword = wine.builtin.libc.Syscall.syscall(cpu.eax.dword, new RegGetter(cpu));
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "syscall";
        }
    }

    static public final class ConditionalMov_Reg extends Op {
        final Reg ed;
        final Reg gd;
        final Condition cond;

        public ConditionalMov_Reg(Condition cond, Reg ed, Reg gd) {
            this.ed = ed;
            this.gd = gd;
            this.cond = cond;
        }

        public Block call(CPU cpu) {
            if (cond.test(cpu))
                gd.dword = ed.dword;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmov";
        }
    }

    static public final class ConditionalMov_Mem extends Op {
        final EaaBase eaa;
        final Reg gd;
        final Condition cond;

        public ConditionalMov_Mem(Condition cond, EaaBase eaa, Reg gd) {
            this.eaa = eaa;
            this.gd = gd;
            this.cond = cond;
        }

        public Block call(CPU cpu) {
            int tmp = cpu.memory.readd(eaa.call(cpu));
            if (cond.test(cpu))
                gd.dword = tmp;
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "cmov";
        }
    }

    static public final class BtcEdGd_reg extends Op {
        final Reg ed;
        final Reg gd;

        public BtcEdGd_reg(Reg ed, Reg gd) {
            this.ed = ed;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            cpu.fillFlags();
            int mask=1 << (gd.dword & 31);
            cpu.setFlag((ed.dword & mask)!=0, CPU.CF);
            ed.dword^=mask;
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "btc";
        }
    }

    static public final class BtcEdGd_mem extends Op {
        final EaaBase eaa;
        final Reg gd;

        public BtcEdGd_mem(EaaBase eaa, Reg gd) {
            this.eaa = eaa;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            cpu.fillFlags();
            int mask=1 << (gd.dword & 31);
            int address=eaa.call(cpu);
            address+=(gd.dword>>5)*4; // intentional signed shift
            int old=cpu.memory.readd(address);
            cpu.setFlag((old & mask)!=0, CPU.CF);
            cpu.memory.writed(address,old ^ mask);
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "btc";
        }
    }

    static public final class BsfGdEd_reg extends Op {
        final Reg ed;
        final Reg gd;

        public BsfGdEd_reg(Reg ed, Reg gd) {
            this.ed = ed;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            cpu.fillFlags();
            int value=ed.dword;
            if (value==0) {
                cpu.setFlag(true, CPU.ZF);
            } else {
                int result = 0;
                while ((value & 0x01)==0) { result++; value>>>=1; }
                cpu.setFlag(false, CPU.ZF);
                gd.dword=result;
            }
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "bsf";
        }
    }

    static public final class BsfGdEd_mem extends Op {
        final EaaBase eaa;
        final Reg gd;

        public BsfGdEd_mem(EaaBase eaa, Reg gd) {
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
                gd.dword=result;
            }
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "bsf";
        }
    }

    static public final class BsrGdEd_reg extends Op {
        final Reg ed;
        final Reg gd;

        public BsrGdEd_reg(Reg ed, Reg gd) {
            this.ed = ed;
            this.gd = gd;
        }

        public Block call(CPU cpu) {
            cpu.fillFlags();
            int value=ed.dword;
            if (value==0) {
                cpu.setFlag(true, CPU.ZF);
            } else {
                int result = 31;	// Operandsize-1
                while ((value & 0x80000000)==0) { result--; value<<=1; }
                cpu.setFlag(false, CPU.ZF);
                gd.dword=result;
            }
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "bsr";
        }
    }

    static public final class BsrGdEd_mem extends Op {
        final EaaBase eaa;
        final Reg gd;

        public BsrGdEd_mem(EaaBase eaa, Reg gd) {
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
                gd.dword=result;
            }
            return next.callAndLog(cpu);
        }

        public String toString() {
            return "bsr";
        }
    }

    final static class PopSeg32 extends Op {
        final private Reg segValue;
        final private Reg segPhys;
        final private String name;

        public PopSeg32(Reg segValue, Reg segPhys, String name) {
            this.segValue = segValue;
            this.segPhys = segPhys;
            this.name = name;
        }

        public Block call(CPU cpu) {
            segValue.dword = cpu.pop32();
            segPhys.dword = cpu.ldt[segValue.dword >> 3];
            return next.callAndLog(cpu);
        }
        public String toString() {
            return "pop "+name;
        }
    }
}

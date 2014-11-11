package wine.builtin.libc;

import wine.emulation.CPU;
import wine.emulation.Memory;
import wine.system.WineThread;
import wine.util.Log;

public class Setjmp {
//    double 	__align [21]
//    int * 	__sp
//    long int 	__gregs [20]
//    int 	__mask_was_saved
//    __sigset_t 	__saved_mask
    static public void __longjmp_chk(int env, int val) {
        Log.panic("__longjmp_chk not implemented");
    }

    static public void longjmp(int env, int val) {
        Log.panic("longjmp not implemented");
    }

    // void siglongjmp(sigjmp_buf env, int val)
    static public void siglongjmp(int env, int val) {
        Log.panic("siglongjmp not implemented");
    }

    // int _setjmp(jmp_buf env)
    static public int _setjmp(int env) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        CPU cpu = thread.cpu;
        memory.writed(env, cpu.ebx.dword);env+=4;
        memory.writed(env, cpu.esi.dword);env+=4;
        memory.writed(env, cpu.edi.dword);env+=4;
        memory.writed(env, cpu.ebp.dword);env+=4;
        memory.writed(env, cpu.esp.dword);env+=4;
        memory.writed(env, cpu.eip);env+=4; // eip was popped before we got here
        memory.writed(env, 0);env+=4; // __mask_was_saved
        return 0;
    }

    // int __sigsetjmp(sigjmp_buf env, int savemask)
    static public int __sigsetjmp(int env, int savemask) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        CPU cpu = thread.cpu;
        memory.writed(env, cpu.ebx.dword);env+=4;
        memory.writed(env, cpu.esi.dword);env+=4;
        memory.writed(env, cpu.edi.dword);env+=4;
        memory.writed(env, cpu.ebp.dword);env+=4;
        memory.writed(env, cpu.esp.dword);env+=4;
        memory.writed(env, cpu.eip);env+=4; // eip was popped before we got here
        memory.writed(env, 1);env+=4; // __mask_was_saved
        memory.writed(env, savemask);
        return 0;
    }
}

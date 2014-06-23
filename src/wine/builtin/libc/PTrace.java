package wine.builtin.libc;

public class PTrace {
    // long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
    static public int ptrace(int request, int pid, int addr, int data) {
        return -1;
    }
}

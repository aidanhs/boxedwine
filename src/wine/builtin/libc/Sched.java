package wine.builtin.libc;

public class Sched {
    // int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask)
    static public int sched_getaffinity(int pid, int cpusetsize, int mask) {
        return -1;
    }
    // int sched_setaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask)
    static public int sched_setaffinity(int pid, int cpusetsize, int mask) {
        return 0;
    }
    // int sched_yield(void)
    static public int sched_yield() {
        return 0;
    }
}

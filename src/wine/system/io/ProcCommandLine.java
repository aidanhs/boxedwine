package wine.system.io;

import wine.emulation.Memory;
import wine.system.WineSystem;
import wine.system.WineThread;
import wine.system.kernel.*;
import wine.system.kernel.Process;
import wine.util.Log;

public class ProcCommandLine implements FSNodeAccess {
    private int pos = 0;
    private byte[] buffer;
    private int processId;

    public ProcCommandLine() {
    }

    public ProcCommandLine(int id) {
        this.processId = id;
    }

    public boolean open(String mode) {
        if (processId==0) {
            this.buffer = new byte[2]; // no kernel command line arguments
        } else {
            Process p = WineSystem.processes.get(processId);
            if (p == null)
                return false;
            StringBuilder buffer = new StringBuilder();
            int i=1;
            if (p.args.size()>0 && p.args.get(0).endsWith("ld-linux.so.2"))
                i++;
            for (;i<p.args.size();i++) {
                buffer.append(p.args.get(i));
                buffer.append((char)0);
            }
            buffer.append((char)0);
            this.buffer = buffer.toString().getBytes();
        }
        return true;
    }

    public boolean setLength(long length) {
        return false;
    }

    public long getFilePointer() {
        return 0;
    }

    public boolean seek(long pos) {
        return false;
    }

    public int read(byte[] b) {
        int len = b.length;
        if (pos+len>buffer.length)
            len = buffer.length-pos;
        System.arraycopy(buffer, pos, b, 0, len);
        pos+=len;
        return len;
    }

    public boolean write(byte[] b) {
        return true;
    }

    public void close() {
    }

    public int ioctl(WineThread thread, int request, Syscall.SyscallGetter getter) {
        return -Errno.ENODEV;
    }

    public int map(Memory memory, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
        Log.panic("Mapping /proc/cmdline not supported");
        return -1;
    }

    public boolean isReadReady() {
        return true;
    }

    public boolean isWriteReady() {
        return true;
    }

    public void setAsync(Process process, boolean remove) {
        Log.panic("async io not implemented for /proc/cmdline");
    }

    public boolean isAsync(Process process) {
        return false;
    }
}

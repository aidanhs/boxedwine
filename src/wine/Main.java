package wine;

import wine.emulation.RAM;
import wine.gui.Screen;
import wine.system.kernel.Process;
import wine.system.WineSystem;
import wine.system.io.*;

import java.io.File;
import java.util.Vector;

public class Main {
    static final public DevPS2Mouse mouse = new DevPS2Mouse();

    static public void main(String[] args) {
        int m = 384;
        int i;
        int cx = 1024;
        int cy = 768;
        int bpp = 32;

        if (args.length==0)
            args = new String[] {"/bin/sh", "/init.sh"};

        Vector<String> env = new Vector<String>();
        Vector<String> programArgs = new Vector<String>();
        String root = System.getProperty("user.dir")+ File.separator+"root";

        env.add("HOME=/home/username");
        env.add("LOGNAME=username");
        env.add("USERNAME=username");
        env.add("USER=username");
        env.add("DISPLAY=:0");
        env.add("LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib");
//        env.add("LD_DEBUG=all");
//        env.add("LD_BIND_NOW=1");

        for (i=0;i<args.length;i++) {
            // :TODO: command line for resolution
            if (args[i].equals("-m")) {
                if (i+1<args.length) {
                    i++;
                    m = Integer.parseInt(args[i]);
                }
            } else if (args[i].contains("=")) {
                if (args[i].startsWith("PATH=")) {
                    WineSystem.path = args[i].substring(5).split(":");
                }
                env.add(args[i]);
            } else if (args[i].equals("-root")) {
                if (i+1<args.length) {
                    i++;
                    root = args[i];
                }
            }
            else {
                break;
            }
        }

        System.out.println("root path="+root);

        FileSystem.root = root;
        VirtualFSNode.addVirtualFile("/dev/null", new DevNull(), KernelStat._S_IREAD|KernelStat._S_IWRITE|KernelStat._S_IFCHR);
        VirtualFSNode.addVirtualFile("/dev/zero", new DevZero(), KernelStat._S_IREAD|KernelStat._S_IFCHR);
        VirtualFSNode.addVirtualFile("/dev/urandom", new DevUrandom(), KernelStat._S_IREAD|KernelStat._S_IFCHR);
        VirtualFSNode.addVirtualFile("/proc/meminfo", new ProcMeminfo(), KernelStat._S_IREAD);
        VirtualFSNode.addVirtualFile("/proc/cmdline", new ProcCommandLine(), KernelStat._S_IREAD);
        VirtualFSNode.addVirtualFile("/dev/tty0", new DevTTY(0), KernelStat._S_IREAD|KernelStat._S_IWRITE|KernelStat._S_IFCHR);
        VirtualFSNode.addVirtualFile("/dev/tty2", new DevTTY(2), KernelStat._S_IREAD|KernelStat._S_IWRITE|KernelStat._S_IFCHR);

        VirtualFSNode.addVirtualFile("/dev/psaux", mouse, KernelStat._S_IWRITE|KernelStat._S_IREAD|KernelStat._S_IFCHR);

        programArgs.add("/lib/ld-linux.so.2");
        WineSystem.path = null;

        Screen.create(cx, cy, bpp);
        VirtualFSNode.addVirtualFile("/dev/fb0", new DevFB(cx, cy, bpp, cx*4, Process.ADDRESS_PROCESS_FRAME_BUFFER << 12, cx*4*cy*2), KernelStat._S_IREAD|KernelStat._S_IWRITE|KernelStat._S_IFCHR);

        if (WineSystem.path==null) {
            WineSystem.path = new String[] {"/bin","/usr/bin","/usr/local/bin"};
            env.add("PATH=/bin:/usr/bin:/usr/local/bin");
        }
        String program = args[i++];
        if (!program.startsWith("/")) {
            for (String p : WineSystem.path) {
                if (FSNode.getNode(p+"/"+program, true)!=null) {
                    program = p+"/"+program;
                    break;
                }
            }
        }

        programArgs.add(program);

        for (;i<args.length;i++) {
            programArgs.add(args[i]);
        }

        RAM.init(m * 1024 * 1024);
        if (wine.system.kernel.Process.create("/home/username", programArgs, env)==null) {
            System.out.println("Failed to start wine");
            System.exit(-1);
        }
    }
}

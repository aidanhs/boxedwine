package wine;

import wine.emulation.RAM;
import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.io.*;
import wine.util.Path;

import java.io.File;
import java.util.Vector;

public class Main {
    static public void main(String[] args) {
        FileSystem.links.add(new Path("/home/username/.wine/drive_c", "/home/username/.wine/dosdevices/c:"));
        FileSystem.links.add(new Path("/", "/home/username/.wine/dosdevices/z:"));
        FileSystem.paths.add(new Path(System.getProperty("user.dir")+ File.separator+"root", ""));
        VirtualFSNode.addVirtualFile("/dev/null", new DevNull());
        VirtualFSNode.addVirtualFile("/dev/zero", new DevZero());
        VirtualFSNode.addVirtualFile("/dev/urandom", new DevUrandom());
        VirtualFSNode.addVirtualFile("/proc/meminfo", new ProcMeminfo());


        int m = 32;
        Vector<String> env = new Vector<String>();
        Vector<String> programArgs = new Vector<String>();
        int i;

        env.add("HOME=/home/username");
        env.add("LOGNAME=username");
        env.add("USERNAME=username");
        env.add("USER=username");
        env.add("DISPLAY=:0");

        programArgs.add("/lib/ld-linux.so.2");
        String[] path = null;

        for (i=0;i<args.length;i++) {
            if (args[i].equals("-m")) {
                if (i+1<args.length) {
                    i++;
                    m = Integer.parseInt(args[i]);
                }
            } else if (args[i].contains("=")) {
                if (args[i].startsWith("PATH=")) {
                    path = args[i].substring(5).split(":");
                }
                env.add(args[i]);
            } else {
                break;
            }
        }
        if (path==null) {
            path = new String[] {"/usr/bin"};
            env.add("PATH=/usr/bin");
        }
        String program = args[i++];
        if (!program.startsWith("/")) {
            for (String p : path) {
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
        if (WineProcess.create("/home/username", programArgs, env)==null) {
            System.out.println("Failed to start wine");
            System.exit(-1);
        }
    }
}

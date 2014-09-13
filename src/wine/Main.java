package wine;

import wine.emulation.RAM;
import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.io.*;
import wine.util.Path;

import java.io.File;

public class Main {
    static public void main(String[] args) {
        FileSystem.links.add(new Path("/home/boxedwine/.wine/drive_c", "/home/boxedwine/.wine/dosdevices/c:"));
        FileSystem.links.add(new Path("/", "/home/boxedwine/.wine/dosdevices/z:"));
        FileSystem.paths.add(new Path(System.getProperty("user.dir")+ File.separator+"root", ""));
        VirtualFSNode.addVirtualFile("/dev/null", new DevNull());
        VirtualFSNode.addVirtualFile("/dev/zero", new DevZero());
        VirtualFSNode.addVirtualFile("/dev/urandom", new DevUrandom());
        VirtualFSNode.addVirtualFile("/proc/meminfo", new ProcMeminfo());
        WineSystem.libDirs.add("/usr/lib/i386-linux-gnu/wine-unstable");
        WineSystem.libDirs.add("/usr/lib/i386-linux-gnu/wine-unstable/wine");
        WineSystem.libDirs.add("/usr/lib/i386-linux-gnu");

        RAM.init(170 * 1024 * 1024);
        // not necessary to create this up front, but it allows debug msg's to go to stdout
        //createWineServer();
        //WineProcess.create(WineSystem.homeDirectory, new String[] {"/usr/lib/i386-linux-gnu/libz.so.1"}, new String[0]);
        String[] t = new String[args.length+1];
        System.arraycopy(args, 0, t, 1, args.length);
        args = t;
        args[0]="/usr/lib/i386-linux-gnu/wine-unstable/bin/wine";
        if (WineProcess.create(WineSystem.homeDirectory, args, new String[] {"WINELOADERNOEXEC=1","WINEARCH=win32","WINEDEBUG=+x11drv"})==null) {
            System.out.println("Failed to start wine");
            System.exit(-1);
        }
    }

    static private void createWineServer() {
        final WineProcess process = WineProcess.create(WineSystem.homeDirectory,new String[] {"/usr/lib/i386-linux-gnu/wine-unstable/bin/wineserver", "-f"}, new String[0]);
        if (process==null) {
            System.out.println("Failed to start wineserver");
            System.exit(-1);
        }
        for (int i=0;i<1;i++) {
            synchronized (process) {
                if (process.state == WineProcess.STATE_STARTED)
                    break;
                try {process.wait(3000);} catch (Exception e) {}
            }
        }
        if (process.state != WineProcess.STATE_STARTED) {
            System.out.println("Failed to start wineserver");
            System.exit(-1);
        }
        boolean socketCreated = false;
        long startTime = System.nanoTime();
        while (true) {
            synchronized (FileSystem.openNodes) {
                for (FSNode node : FileSystem.openNodes.values()) {
                    if (node.localPath.endsWith("socket")) {
                        socketCreated = true;
                        break;
                    }
                }
                if (socketCreated)
                    break;
                long elapsed = (System.nanoTime()-startTime)/1000000;
                if (elapsed>30000)
                    break;
                try {FileSystem.openNodes.wait(30000-elapsed);} catch (Exception e){}
            }
        }
        if (!socketCreated) {
            System.out.println("wineserver failed to create master socket within 10 seconds");
            System.exit(-1);
        }
    }
}

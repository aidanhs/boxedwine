package wine;

import wine.emulation.RAM;
import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.io.*;
import wine.util.Path;

public class Main {

    static public void main(String[] args) {
        FileSystem.links.add(new Path("/home/boxedwine/.wine/drive_c", "/home/boxedwine/.wine/dosdevices/c:"));
        FileSystem.links.add(new Path("/", "/home/boxedwine/.wine/dosdevices/z:"));
        FileSystem.paths.add(new Path("c:\\wine\\root", ""));
        VirtualFSNode.addVirtualFile("/dev/null", new DevNull());
        VirtualFSNode.addVirtualFile("/dev/zero", new DevZero());
        WineSystem.libDirs.add("/usr/lib/i386-linux-gnu/wine-unstable");
        WineSystem.libDirs.add("/usr/lib/i386-linux-gnu/wine-unstable/wine");
        WineSystem.libDirs.add("/usr/lib/i386-linux-gnu");

        RAM.init(192 * 1024 * 1024);
        // not necessary to create this up front, but it allows debug msg's to go to stdout
        //createWineServer();
        //WineProcess.create(WineSystem.homeDirectory, new String[] {"/usr/lib/i386-linux-gnu/libz.so.1"}, new String[0]);
        if (WineProcess.create(WineSystem.homeDirectory, new String[] {"/usr/lib/i386-linux-gnu/wine-unstable/bin/wine", "unzip.exe"}, new String[] {"WINELOADERNOEXEC=1"})==null) {
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

package wine;

import wine.emulation.RAM;
import wine.system.WineProcess;
import wine.system.io.*;
import wine.util.Path;

public class Main {

    static public void main(String[] args) {
        // the first entry should be the default process current directory
        FileSystem.paths.add(new Path("c:\\wine\\.wine\\drive_c", "/home/boxedwine/.wine/dosdevices/c:"));
        FileSystem.paths.add(new Path("c:\\wine", "/home/boxedwine"));
        FileSystem.paths.add(new Path("c:\\wine\\lib", "/lib"));
        FileSystem.paths.add(new Path("c:\\wine\\bin", "/bin"));
        FileSystem.paths.add(new Path("c:\\wine\\tmp", "/tmp"));
        FileSystem.paths.add(new Path("c:\\wine\\dev", "/dev"));
        VirtualFSNode.addVirtualFile("/dev/null", new DevNull());
        VirtualFSNode.addVirtualFile("/dev/zero", new DevZero());

        RAM.init(128 * 1024 * 1024);
        createWineServer();
        if (WineProcess.create(new String[] {"wine", "unzip.exe"}, new String[] {"WINELOADERNOEXEC=1", "WINEDLLPATH=/lib", "WINEDEBUG=+all"})==null) {
            System.out.println("Failed to start wine");
            System.exit(-1);
        }
    }

    static private void createWineServer() {
        final WineProcess process = WineProcess.create(new String[] {"wineserver", "-f"}, new String[0]);
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
        try {Thread.sleep(2000);} catch (Exception e){}
    }
}

package wine;

import wine.emulation.RAM;
import wine.system.WineProcess;
import wine.system.WineSystem;
import wine.system.io.DevNull;
import wine.system.io.DevZero;
import wine.system.io.FileSystem;
import wine.system.io.VirtualFSNode;
import wine.util.Path;

public class Main {

    static public void main(String[] args) {
        FileSystem.paths.add(new Path("c:\\wine\\.wine\\drive_c", "/home/boxedwine/.wine/dosdevices/c:"));
        FileSystem.paths.add(new Path("c:\\wine", "/home/boxedwine/.wine/dosdevices/z:"));
        FileSystem.paths.add(new Path("c:\\wine", "/home/boxedwine"));
        FileSystem.paths.add(new Path("c:\\wine\\lib", "/lib"));
        FileSystem.paths.add(new Path("c:\\wine\\bin", "/bin"));
        FileSystem.paths.add(new Path("c:\\wine\\tmp", "/tmp"));
        FileSystem.paths.add(new Path("c:\\wine\\dev", "/dev"));
        VirtualFSNode.addVirtualFile("/dev/null", new DevNull());
        VirtualFSNode.addVirtualFile("/dev/zero", new DevZero());

        RAM.init(128 * 1024 * 1024);
        if (WineProcess.create(WineSystem.homeDirectory, new String[] {"wine", "unzip.exe"}, new String[] {"WINELOADERNOEXEC=1", "WINEDLLPATH=/lib", "WINEDEBUG=+all"})==null) {
            System.out.println("Failed to start wine");
            System.exit(-1);
        }
    }
}

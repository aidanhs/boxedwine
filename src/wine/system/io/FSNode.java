package wine.system.io;

import wine.emulation.Memory;
import wine.emulation.PageHandler;
import wine.system.kernel.*;
import wine.system.WineThread;
import wine.util.Path;

import java.io.*;
import java.util.Vector;

abstract public class FSNode {
    static private int nextId=10;

    final public String localPath;
    final public String nativePath;
    final public Vector locks = new Vector(); // there can only be one write lock per byte and only one read lock per byte per process
    final public int id;
    public String localLinkPath;
    public String nativeLinkPath;

    static public String readLink(File file) {
        try {
            FileInputStream is = new FileInputStream(file);
            byte[] b = new byte[1024];
            int read = is.read(b);
            is.close();
            return new String(b, 0, read);
        } catch (Exception e) {

        }
        return null;
    }

    static private String localToNative(String local) {
        if (local.contains(":"))
            local=local.replace(":", "(_colon_)");
        local = FileSystem.root + local;
        if (File.separator.equals("\\"))
            local=local.replace('/', '\\');
        return local;
    }

    static private String nativeToLocalName(String nativePath) {
        nativePath=nativePath.replace("(_colon_)", ":");
        if (nativePath.endsWith(".link"))
            nativePath = nativePath.substring(0, nativePath.length()-5);
        return nativePath;
    }

    static public FSNode getNode(String localPath, boolean existing) {
        if (!localPath.startsWith("/")) {
            WineThread thread = WineThread.getCurrent();
            if (thread.currentDirectory!=null) {
                localPath = thread.currentDirectory + "/" + localPath;
            } else {
                localPath = thread.process.currentDirectory + "/" + localPath;
            }
        }
        String localLinkPath = null;
        String nativeLinkPath = null;
        //System.out.println(localPath);
        while (true) {
            String nativePath = localToNative(localPath);
            File nativeFile = new File(nativePath+".link");
            if (!nativeFile.exists()) {
                // remove the first /
                String[] parts = localPath.substring(1).split("/");
                StringBuilder path = new StringBuilder("/");
                StringBuilder parentPath = new StringBuilder("/");
                boolean foundLink = false;

                // -1 because the last part will be handled below
                for (int i=0;i<parts.length-1;i++) {
                    String part = parts[i];
                    path.append(part);
                    path.append("/");
                    File dir = new File(localToNative(path.toString()));
                    if (!dir.exists()) {
                        File link = new File(dir.getAbsolutePath()+".link");
                        if (link.exists()) {
                            String newPath = readLink(link);
                            path = new StringBuilder(newPath);
                            i++;
                            if (!newPath.endsWith("/"))
                                path.append("/");
                            for (;i<parts.length;i++) {
                                path.append(parts[i]);
                                if (i!=parts.length-1)
                                    path.append("/");
                            }
                            localPath = path.toString();
                            if (!localPath.startsWith("/")) {
                                localPath=parentPath+localPath;
                            }
                            foundLink = true;
                            break;
                        }
                    }
                    parentPath.append(part);
                    parentPath.append("/");
                }
                if (foundLink)
                    continue;
                nativeFile = new File(nativePath);
            } else {
                if (localLinkPath==null) {
                    localLinkPath = localPath;
                    nativeLinkPath = nativeFile.getAbsolutePath();
                }
                localPath = readLink(nativeFile);
                if (!localPath.startsWith("/")) {
                    localPath = localLinkPath.substring(0, localLinkPath.lastIndexOf("/")+1)+localPath;
                }
                continue;
            }
            // remove the ../../ etc
            try {
                nativePath = nativeFile.getCanonicalPath();
                localPath = nativePath.substring(FileSystem.root.length());
                if (localPath.length()==0)
                    localPath="/";
                if (File.separator.equals("\\"))
                    localPath=localPath.replace('\\', '/');
                localPath=nativeToLocalName(localPath); // unescape colons
                nativeFile = new File(nativePath);
            } catch (IOException e) {
            }
            if (localLinkPath!=null) {
                System.out.println(localLinkPath);
                System.out.println("    -> "+nativePath);
            }
            String key;
            if (localLinkPath==null)
                key = localPath;
            else
                key = localLinkPath;
            FSNode node = FileSystem.openNodes.get(key);
            if (node == null && (nativeFile.exists() || !existing)) {
                node = new FSNodeFile(nativeFile, localPath, nativePath, localLinkPath, nativeLinkPath);
                synchronized (FileSystem.openNodes) {
                    FileSystem.openNodes.put(key, node);
                    FileSystem.openNodes.notifyAll();
                }
            }
            return node;
        }
    }

    static public void remove(FSNode node) {
        synchronized (FileSystem.openNodes) {
            FileSystem.openNodes.remove(node.localPath);
            FileSystem.openNodes.notifyAll();
        }
    }

    static public void addNode(FSNode node) {
        synchronized (FileSystem.openNodes) {
            FileSystem.openNodes.put(node.localPath, node);
            FileSystem.openNodes.notifyAll();
        }
    }

    protected FSNode(String localPath, String nativePath, String localLinkPath, String nativeLinkPath) {
        this.localPath = localPath;
        this.nativePath = nativePath;
        this.id=nextId++;
        this.localLinkPath = localLinkPath;
        this.nativeLinkPath = nativeLinkPath;
    }

    abstract public boolean isDirectory();
    abstract public FSNode[] list();
    abstract public boolean exists();
    abstract public boolean delete();
    abstract public boolean renameTo(FSNode node);
    abstract public long lastModified();
    abstract public long length();
    abstract public FSNodeAccess open(String mode);
    abstract public boolean mkdir();
    abstract public InputStream getInputStream() throws IOException;
    abstract public boolean setLastModifiedTime(long time);
    abstract public String name();
    abstract public boolean canRead();
    abstract public boolean canWrite();
    abstract public int getType();
    abstract public int getMode();

    private static class FSNodeFile extends FSNode {
        private FSNodeFile(File file, String localPath, String nativePath, String localLinkPath, String nativeLinkPath) {
            super(localPath, nativePath, localLinkPath, nativeLinkPath);
            this.file = file;
        }

        public boolean isDirectory() {
            return file.isDirectory();
        }

        public int getType() {
            if (isDirectory())
                return 4; // DT_DIR;
            return 8; // DT_REG
        }

        public int getMode() {
            int result = 0;
            if (isDirectory()) {
                result |= KernelStat._S_IFDIR;
            } else {
                result |= KernelStat._S_IFREG;
            }
            result |= KernelStat._S_IREAD;
            result |= KernelStat._S_IWRITE;
            result |= KernelStat._S_IEXEC;
            return result;
        }

        public FSNode[] list() {
            // :TODO: what about virtual files and sockets
            String[] s = file.list();
            for (int i=0;i<s.length;i++) {
                s[i] = nativeToLocalName(s[i]);
            }
            FSNode[] result = new FSNode[s.length];
            for (int i=0;i<s.length;i++) {
                result[i] = getNode(localPath+"/"+s[i], true);
            }
            return result;
        }

        public boolean exists() {
            return file.exists();
        }

        public boolean delete() {
            if (nativeLinkPath==null) {
                return file.delete();
            }
            return new File(nativeLinkPath).delete();
        }

        public boolean renameTo(FSNode node) {
            if (node instanceof FSNodeFile) {
                if (nativeLinkPath==null)
                    return file.renameTo(((FSNodeFile) node).file);
                String path = node.nativePath;
                if (path.endsWith(File.separator))
                    path = path.substring(0, path.length()-1);
                path=path+".link";
                return new File(nativeLinkPath).renameTo(new File(path));
            }
            return false;
        }

        public long lastModified() {
            return file.lastModified();
        }

        public long length() {
            return file.length();
        }

        public InputStream getInputStream() throws IOException {
            return new FileInputStream(file);
        }

        private class Access implements FSNodeAccess {
            RandomAccessFile io;

            public Access(RandomAccessFile io) {
                this.io = io;
            }

            public boolean open(String mode) {
                return true;
            }

            public boolean isReadReady() {
                return true;
            }

            public boolean isWriteReady() {
                return true;
            }

            public boolean setLength(long length) {
                try {
                    io.setLength(length);
                } catch (IOException e) {
                    return false;
                }
                return true;
            }

            public long getFilePointer() {
                try {
                    return io.getFilePointer();
                } catch (IOException e) {
                    return -1;
                }
            }

            public boolean seek(long pos) {
                try {
                    io.seek(pos);
                    return true;
                } catch (IOException e) {
                    return false;
                }
            }

            public int read(byte[] b) {
                try {
                    return io.read(b);
                } catch (IOException e) {
                    return -1;
                }
            }

            public boolean write(byte[] b) {
                try {
                    io.write(b);
                    return true;
                } catch (IOException e) {
                    return false;
                }
            }

            public void close() {
                if (io != null) {
                    try {
                        io.close();
                    } catch (IOException e) {
                    }
                    io = null;
                }
            }

            public int ioctl(WineThread thread, int request, Syscall.SyscallGetter getter) {
                return -Errno.ENODEV;
            }

            public int map(Memory memory, FileDescriptor fd, long off, int address, int len, boolean fixed, boolean read, boolean exec, boolean write, boolean shared) {
                PageHandler[] handlers = memory.handlers;
                int pageStart = address>>>12;
                int pageCount = (int)((len+0xFFF)>>>12);

                for (int i = 0; i < pageCount; i++) {
                    handlers[pageStart + i].close();
                    if ((read | exec) && write)
                        handlers[i + pageStart] = new MMapHandler(fd, fd.getFile(), address + i * 4096, off + i * 4096, shared);
                    else if (read | exec)
                        handlers[i + pageStart] = new MMapHandlerRO(fd, fd.getFile(), address + i * 4096, off + i * 4096, shared);
                    else if (write)
                        handlers[i + pageStart] = new MMapHandlerWO(fd, fd.getFile(), address + i * 4096, off + i * 4096, shared);
                    else
                        handlers[i + pageStart] = new MMapHandlerNone(fd, fd.getFile(), address + i * 4096, off + i * 4096, shared);
                }
                return address;
            }
        }

        public FSNodeAccess open(String mode) {
            try {
                RandomAccessFile io = new RandomAccessFile(file, mode);
                return new Access(io);
            } catch (Exception e) {
                return null;
            }
        }

        public boolean mkdir() {
            return file.mkdir();
        }

        public boolean setLastModifiedTime(long time) {
            return file.setLastModified(time);
        }

        public String name() {
            if (localLinkPath==null)
                return file.getName();
            return localLinkPath.substring(localLinkPath.lastIndexOf("/")+1);
        }

        public boolean canRead() {
            return file.canRead();
        }

        public boolean canWrite() {
            return file.canWrite();
        }


        final private File file;
    }
}

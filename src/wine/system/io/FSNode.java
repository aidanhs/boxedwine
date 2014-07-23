package wine.system.io;

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

    static public FSNode getNode(String localPath, boolean existing) {
        if (!localPath.startsWith("/")) {
            localPath = WineThread.getCurrent().process.currentDirectory+"/"+localPath;
        }
        for (int i=0;i<FileSystem.links.size();i++) {
            Path path = FileSystem.links.elementAt(i);
            if (localPath.startsWith(path.localPath)) {
                localPath = path.nativePath+localPath.substring(path.localPath.length());
                break;
            }
        }
        for (int i=0;i<FileSystem.paths.size();i++) {
            Path path = FileSystem.paths.elementAt(i);
            if (path.localPath.length()==0 || localPath.startsWith(path.localPath)) {
                String nativePath = path.nativePath + localPath.substring(path.localPath.length());
                if (File.separator.equals("\\"))
                    nativePath=nativePath.replace('/', '\\');
                File nativeFile = new File(nativePath);
                // remove the ../../ etc
                try {
                    nativePath = nativeFile.getCanonicalPath();
                    localPath = path.localPath+nativePath.substring(path.nativePath.length());
                    if (File.separator.equals("\\"))
                        localPath=localPath.replace('\\', '/');
                    nativeFile = new File(nativePath);
                } catch (IOException e) {
                }
                FSNode node = FileSystem.openNodes.get(localPath);
                if (node == null && (nativeFile.exists() || !existing)) {
                    node = new FSNodeFile(nativeFile, localPath, nativePath);
                    synchronized (FileSystem.openNodes) {
                        FileSystem.openNodes.put(localPath, node);
                        FileSystem.openNodes.notifyAll();
                    }
                }
                return node;
            }
        }
        return null;
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

    protected FSNode(String localPath, String nativePath) {
        this.localPath = localPath;
        this.nativePath = nativePath;
        this.id=nextId++;
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

    private static class FSNodeFile extends FSNode {
        private FSNodeFile(File file, String localPath, String nativePath) {
            super(localPath, nativePath);
            this.file = file;
        }

        public boolean isDirectory() {
            return file.isDirectory();
        }

        public FSNode[] list() {
            // :TODO: what about virtual files and sockets
            String[] s = file.list();
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
            return file.delete();
        }

        public boolean renameTo(FSNode node) {
            if (node instanceof FSNodeFile)
                return file.renameTo(((FSNodeFile)node).file);
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
            return file.getName();
        }

        final private File file;
    }
}

package wine.system.io;

public class FileDescriptorMsg {
    public KernelObject object;
    public int accessFlags;

    public FileDescriptorMsg(FileDescriptor fd) {
        object = fd.object;
        accessFlags = fd.accessFlags;
        object.incrementRefCount();
    }

    public FileDescriptor createNewFileDescriptor(wine.system.kernel.Process process) {
        FileDescriptor result = object.createNewFileDescriptor(process);
        result.accessFlags = accessFlags;
        return result;
    }
}

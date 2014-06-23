package wine.system.io;

import wine.system.WineProcess;

public class FileDescriptorMsg {
    public KernelObject object;
    public int accessFlags;

    public FileDescriptorMsg(FileDescriptor fd) {
        object = fd.object;
        accessFlags = fd.accessFlags;
        object.incrementRefCount();
    }

    public FileDescriptor createNewFileDescriptor(WineProcess process) {
        FileDescriptor result = object.createNewFileDescriptor(process);
        result.accessFlags = accessFlags;
        return result;
    }
}

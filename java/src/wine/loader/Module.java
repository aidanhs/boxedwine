package wine.loader;

import wine.loader.elf.ElfSymbol;
import wine.system.kernel.Process;

abstract public class Module {
    final public String name;
    final public Process process;
    final public Integer id;

    public Module(String name, Process process, int id) {
        this.id = new Integer(id);
        this.name = name;
        this.process = process;
    }

    abstract public ElfSymbol getSymbol(String name);
    abstract public Module fork(wine.system.kernel.Process process);
    abstract public void unload();
}

package wine.loader;

import wine.loader.elf.ElfSymbol;
import wine.system.WineProcess;

abstract public class Module {
    final public String name;
    final public WineProcess process;
    final public Integer id;

    public Module(String name, WineProcess process, int id) {
        this.id = new Integer(id);
        this.name = name;
        this.process = process;
    }

    abstract public ElfSymbol getSymbol(String name);
    abstract public Module fork(WineProcess process);
}

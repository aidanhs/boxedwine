package wine.system;

import wine.emulation.CPU;

public interface Callback {
    public void call(CPU cpu);
}

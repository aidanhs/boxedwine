package wine.loader;

import wine.emulation.CPU;
import wine.emulation.RestartThreadException;
import wine.loader.elf.ElfSymbol;
import wine.system.Callback;
import wine.system.ExitThreadException;
import wine.system.kernel.Process;
import wine.system.WineThread;
import wine.util.Log;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Hashtable;

public class BuiltinModule extends Module {
    private Hashtable<String, Callback> functions = new Hashtable<String, Callback>();
    private Hashtable<String, Integer> registeredCallbacks = new Hashtable<String, Integer>();
    protected Hashtable<String, ElfSymbol> symbols = new Hashtable<String, ElfSymbol>();
    private Hashtable<String, Integer> data = new Hashtable<String, Integer>();
    static public int log = 0;

    public BuiltinModule(String name, Process process, int id) {
        super(name, process, id);
    }

    public Module fork(wine.system.kernel.Process process) {
        BuiltinModule lib = new BuiltinModule(name, process, id);
        // shallow copies of Callback and ElfSymbol are ok since they don't change after they are created
        lib.functions = (Hashtable<String, Callback>)functions.clone();
        lib.registeredCallbacks = (Hashtable<String, Integer>)registeredCallbacks.clone();
        lib.symbols = (Hashtable<String, ElfSymbol>)symbols.clone();
        lib.data = (Hashtable<String, Integer>)data.clone();
        return lib;
    }

    public ElfSymbol getSymbol(String name) {
        ElfSymbol symbol = symbols.get(name);
        if (symbol!=null) {
            return symbol;
        }
        Integer address = data.get(name);
        if (address == null)
            address = registeredCallbacks.get(name);
        if (address == null) {
            Callback handler = functions.get(name);
            if (handler == null) {
                return null;
            }
            address =  new Integer(process.loader.registerFunction(handler));
            registeredCallbacks.put(name, address);
        }
        symbol = new ElfSymbol();
        symbol.st_value = address.intValue() & 0xFFFFFFFFl;
        symbol.name = name;
        return symbol;
    }

    static public class HandlerBase implements Callback {
        Method method;
        Object[] args;
        String name;
        boolean pop;
        Class[] params;
        public HandlerBase(String name, Method method, boolean pop) {
            this.method = method;
            params = method.getParameterTypes();
            args = new Object[params.length];
            this.name = name;
            this.pop = pop;
        }

        public String toString() {
            return name;
        }

        public void call(CPU cpu) {
            boolean shouldLog = log == WineThread.getCurrent().id;
            if (name.contains("printf") || name.contains("put") || name.contains("fwrite")) {
                shouldLog = false;
            }
            StringBuilder builder = null;
            cpu.eip = cpu.pop32();
            if (shouldLog) {
                builder = new StringBuilder();
                builder.append(Integer.toHexString(cpu.eip));
                builder.append(" ");
                builder.append(name);
                builder.append(" ");
            }
            int index = 0;
            for (int i=0;i<args.length;i++) {
                if (shouldLog) {
                    if (i > 0)
                        builder.append(", ");
                }
                if (params[i].getName().equals("int") || params[i].getName().equals(Integer.class.getName())) {
                    int v;
                    if (pop)
                        v = cpu.pop32();
                    else
                        v = cpu.peek32(index);
                    args[i] = new Integer(v);
                    if (shouldLog) {
                        builder.append(Integer.toHexString(v));
                    }
                    index++;
                } else if (params[i].getName().equals("float") || params[i].getName().equals(Float.class.getName())) {
                    int v;
                    if (pop)
                        v = cpu.pop32();
                    else
                        v = cpu.peek32(index);
                    args[i] = Float.intBitsToFloat(v);
                    if (shouldLog) {
                        builder.append(args[i]);
                    }
                    index++;
                } else if (params[i].getName().equals("double") || params[i].getName().equals(Double.class.getName())) {
                    int v1;
                    int v2;
                    if (pop) {
                        v1 = cpu.pop32();
                        v2 = cpu.pop32();
                    } else {
                        v1 = cpu.peek32(index);
                        v2 = cpu.peek32(index+1);
                    }
                    args[i] = new Double(Double.longBitsToDouble(v1 | ((long)v2) << 32));
                    index+=2;
                    if (shouldLog)
                        builder.append(args[i]);
                } else if (params[i].getName().equals("long") || params[i].getName().equals(Long.class.getName())) {
                    int v1;
                    int v2;
                    if (pop) {
                        v1 = cpu.pop32();
                        v2 = cpu.pop32();
                    } else {
                        v1 = cpu.peek32(index);
                        v2 = cpu.peek32(index+1);
                    }
                    args[i] = new Long(v1 | ((long)v2) << 32);
                    index+=2;
                    if (shouldLog)
                        builder.append(args[i]);
                }
            }
            try {
                Object result = method.invoke(null, args);

                if (result != null) {
                    if (result instanceof Integer) {
                        cpu.eax.dword = ((Integer) result).intValue();
                        if (shouldLog) {
                            builder.append(" result=");
                            builder.append(Integer.toHexString(cpu.eax.dword));
                        }
                    } else if (result instanceof Long) {
                        long r = ((Long) result).longValue();
                        cpu.eax.dword = (int)r;
                        cpu.edx.dword = (int)(r >>> 32);
                        if (shouldLog) {
                            builder.append(" result=");
                            builder.append(Long.toHexString(r));
                        }
                    } else if (result instanceof Double) {
                        cpu.fpu.push(((Double)result).doubleValue());
                        if (shouldLog) {
                            builder.append(" result=");
                            builder.append(result);
                        }
                    } else if (result instanceof Float) {
                        cpu.fpu.push(((Float)result).floatValue());
                        if (shouldLog) {
                            builder.append(" result=");
                            builder.append(result);
                        }
                    }
                }
            } catch (ExitThreadException e) {
                throw e;
            } catch (RestartThreadException e) {
                throw e;
            } catch (Exception e) {
                if (e instanceof InvocationTargetException) {
                    if (((InvocationTargetException)e).getTargetException() instanceof ExitThreadException)
                        throw new ExitThreadException();
                    if (((InvocationTargetException)e).getTargetException() instanceof RestartThreadException)
                        throw new RestartThreadException();
                }
                e.printStackTrace();
                Log.panic(toString() + " failed to execute: " + e.getMessage());
            }
            if (shouldLog) {
                builder.append('\n');
                WineThread.getCurrent().out(builder.toString());
            }
        }
    }

    protected void addData(String name, int value) {
        data.put(name, value);
    }

    protected void add(Class c, String methodName) {
        Method[] methods = c.getMethods();
        for (Method method: methods) {
            if (method.getName().equals(methodName)) {
                add(new HandlerBase(methodName, method, true));
                return;
            }
        }
        Log.panic("Failed to find "+methodName);
    }

    protected void add_cdecl(Class c, String methodName) {
        Method[] methods = c.getMethods();
        for (Method method: methods) {
            if (method.getName().equals(methodName)) {
                add(new HandlerBase(methodName, method, false));
                return;
            }
        }
        Log.panic("Failed to find "+methodName);
    }

    protected void add(Callback handler) {
        if (handler.toString().toLowerCase().startsWith(name.toLowerCase()))
            functions.put(handler.toString().substring(name.length() + 1), handler);
        else
            functions.put(handler.toString(), handler);
    }

    public void unload() {
        for (Integer func : registeredCallbacks.values()) {
            process.loader.unregisterFunction(func);
        }
    }
}

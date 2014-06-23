package wine.loader;

import wine.emulation.CPU;
import wine.loader.elf.ElfSymbol;
import wine.system.ExitThreadException;
import wine.system.WineThread;
import wine.util.Log;
import wine.system.Callback;
import wine.system.WineProcess;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Hashtable;

public class BuiltinModule extends Module {
    static public final String log = "wine";

    private Hashtable functions = new Hashtable();
    private Hashtable registeredCallbacks = new Hashtable();
    private Hashtable ordinalToName = new Hashtable();
    private Hashtable<String, ElfSymbol> symbols = new Hashtable<String, ElfSymbol>();
    private Hashtable<String, Integer> data = new Hashtable<String, Integer>();

    public BuiltinModule(String name, WineProcess process, int id) {
        super(name, process, id);
    }

    public ElfSymbol getSymbol(String name) {
        ElfSymbol symbol = symbols.get(name);
        if (symbol!=null) {
            return symbol;
        }
        Integer address = data.get(name);
        if (address == null)
            address = (Integer)registeredCallbacks.get(name);
        if (address == null) {
            Callback handler = (Callback)functions.get(name);
            if (handler == null) {
                return null;
            }
            address =  new Integer(process.loader.registerFunction(handler));
            registeredCallbacks.put(name, address);
        }
        symbol = new ElfSymbol();
        symbol.st_value = address.intValue() & 0xFFFFFFFFl;
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
            boolean shouldLog = true;
            StringBuilder builder = null;
            if (shouldLog) {
                builder = new StringBuilder();
                builder.append(name);
                builder.append(" ");
            }
            cpu.eip = cpu.pop32();
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
                        //cpu.fpu.push(((Double) result).doubleValue());
                    }
                }
            } catch (ExitThreadException e) {
                throw e;
            } catch (Exception e) {
                if (e instanceof InvocationTargetException) {
                    if (((InvocationTargetException)e).getTargetException() instanceof ExitThreadException)
                        throw new ExitThreadException();
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
        data.put(name, new Integer(value));
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

    public int getProcAddress(final String functionName) {
        Integer result = (Integer)registeredCallbacks.get(functionName);
        if (result != null)
            return result;

        Callback handler = (Callback)functions.get(functionName);
        if (handler == null) {
            Log.panic("Unknown " + name + " function: " + functionName);
            return 0;
        }
        int address =  process.loader.registerFunction(handler);
        registeredCallbacks.put(functionName, address);
        return address;
    }
}

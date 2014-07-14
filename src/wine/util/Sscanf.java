package wine.util;

import wine.builtin.libc.CType;
import wine.emulation.Memory;

public class Sscanf {
    private static int readNumber(Memory memory, int valuePos, String value, Sprintf.SprintfGetter getter, boolean isShort, boolean isLongLong, String width, boolean signed, int base, boolean ignored) {
        int originalValuePos = valuePos;

        StringBuilder result = new StringBuilder();
        boolean negative = false;
        if (value.charAt(valuePos)=='-') {
            negative=true;
            valuePos++;
        } else if (value.charAt(valuePos)=='+') {
            valuePos++;
        }
        if (base == 0) {
            if (value.charAt(valuePos)=='0') {
                if (value.length()>valuePos+1 && (value.charAt(valuePos+1)=='x' || value.charAt(valuePos+1)=='X')) {
                    valuePos+=2;
                    base = 16;
                } else {
                    base = 8;
                    valuePos++;
                }
            }
        } else if (base==16) {
            if (value.length()>valuePos+1 && value.charAt(valuePos+1)=='0' && (value.charAt(valuePos+1)=='x' || value.charAt(valuePos+1)=='X')) {
                valuePos+=2;
            }
        }
        int w = Integer.MAX_VALUE;
        if (width.length()>0)
            w = Integer.parseInt(width);
        for (int i=0;i<w && valuePos<value.length();i++) {
            int c = value.charAt(valuePos);
            if (base == 8) {
                if (c>='0' && c<='7') {
                    result.append(c);
                    valuePos++;
                } else {
                    break;
                }
            } else if (base == 10) {
                if (c>='0' && c<='9') {
                    result.append(c);
                    valuePos++;
                } else {
                    break;
                }
            } else if (base == 16) {
                if ((c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F')) {
                    result.append(c);
                    valuePos++;
                } else {
                    break;
                }
            } else {
                Log.panic("sscanf: unsupported base "+base);
            }
        }
        if (result.length()==0)
            return originalValuePos;
        if (!ignored) {
            try {
                int address = getter.getNextInt();
                long number = Long.parseLong(result.toString(), base);
                if (negative)
                    number = -number;
                if (isShort) {
                    memory.writew(address, (int) number);
                } else if (isLongLong) {
                    memory.writeq(address, number);
                } else {
                    memory.writed(address, (int)number);
                }
            } catch (NumberFormatException e) {
                // :TODO: handle unsigned longs bigger than Long.MAX_VALUE
                e.printStackTrace();
                Log.panic("Unhandled number format in sscanf: "+result.toString()+" base "+base);
                return originalValuePos;
            }
        }
        return valuePos;
    }

    public static int sscanf(Memory memory, String value, String format, Sprintf.SprintfGetter getter) {
        int formatPos = 0;
        int valuePos = 0;
        int conversions = 0;
        int assigned = 0;

        while (formatPos<format.length()) {
            char c = format.charAt(formatPos++);
            boolean ignored = false;
            boolean isLong = false;
            boolean isLongLong = false;
            boolean isShort = false;
            StringBuilder width = new StringBuilder();

            if (c == 0)
                break;
            if (CType.isspace(c) != 0) {
                while (valuePos < value.length() && CType.isspace(value.charAt(valuePos)) != 0) {
                    valuePos++;
                }
            }
            if (c != '%') {
                if (valuePos >= value.length())
                    return (conversions != 0 ? assigned : -1);
                if (value.charAt(valuePos)!=format.charAt(formatPos))
                    return assigned;
                valuePos++;
            } else {
                boolean done = false;
                while (!done) {
                    c = format.charAt(formatPos++);
                    switch (c) {
                        case '%':
                            if (valuePos >= value.length())
                                return (conversions != 0 ? assigned : -1);
                            if (value.charAt(valuePos)!=format.charAt(formatPos))
                                return assigned;
                            valuePos++;
                            break;
                        case '*':
                            ignored = true;
                            break;
                        case 'h':
                            isShort = true;
                            break;
                        case 'l':
                            if (isLong) {
                                isLong = false;
                                isLongLong = true;
                            } else {
                                isLong = true;
                            }
                            break;
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            width.append(c);
                            break;
                        case 'i': {
                            int pos = readNumber(memory, valuePos, value, getter, isShort, isLongLong, width.toString(), true, 0, ignored);
                            if (pos == valuePos)
                                return (conversions != 0 ? assigned : -1);
                            if (!ignored)
                                assigned++;
                            conversions++;
                            done = true;
                            break;
                        }
                        case 'd': {
                            int pos = readNumber(memory, valuePos, value, getter, isShort, isLongLong, width.toString(), true, 10, ignored);
                            if (pos == valuePos)
                                return (conversions != 0 ? assigned : -1);
                            if (!ignored)
                                assigned++;
                            conversions++;
                            done = true;
                            break;
                        }
                        case 'u': {
                            int pos = readNumber(memory, valuePos, value, getter, isShort, isLongLong, width.toString(), false, 10, ignored);
                            if (pos == valuePos)
                                return (conversions != 0 ? assigned : -1);
                            if (!ignored)
                                assigned++;
                            conversions++;
                            done = true;
                            break;
                        }
                        case 'o': {
                            int pos = readNumber(memory, valuePos, value, getter, isShort, isLongLong, width.toString(), false, 8, ignored);
                            if (pos == valuePos)
                                return (conversions != 0 ? assigned : -1);
                            if (!ignored)
                                assigned++;
                            conversions++;
                            done = true;
                            break;
                        }
                        case 'x': {
                            int pos = readNumber(memory, valuePos, value, getter, isShort, isLongLong, width.toString(), false, 16, ignored);
                            if (pos == valuePos)
                                return (conversions != 0 ? assigned : -1);
                            if (!ignored)
                                assigned++;
                            conversions++;
                            done = true;
                            break;
                        }
                        case 's': {
                            if (isLong)
                                Log.panic("long string not supported in ssprintf");
                            if (isLong)
                                Log.panic("long long string not supported in ssprintf");
                            if (isLong)
                                Log.panic("short string not supported in ssprintf");
                            int w=Integer.MAX_VALUE;
                            if (width.length()>0)
                                w = Integer.parseInt(width.toString());
                            if (w<1)
                                w = Integer.MAX_VALUE;
                            int address = 0;
                            if (!ignored)
                                address = getter.getNextInt();
                            for (int i=0;i<w && valuePos<value.length();i++) {
                                int ch = value.charAt(valuePos);
                                if (CType.isspace(ch)!=0)
                                    break;
                                if (!ignored)
                                    memory.writeb(address++, ch);
                                valuePos++;
                            }
                            memory.writeb(address, 0);
                            if (!ignored)
                                assigned++;
                            conversions++;
                            done = true;
                            break;
                        }
                        case 'c': {
                            if (isLong)
                                Log.panic("long string not supported in ssprintf");
                            if (isLong)
                                Log.panic("long long string not supported in ssprintf");
                            if (isLong)
                                Log.panic("short string not supported in ssprintf");
                            int w=1;
                            if (width.length()>0)
                                w = Integer.parseInt(width.toString());
                            if (w<1)
                                w = 1;
                            int address = 0;
                            if (!ignored)
                                address = getter.getNextInt();
                            for (int i=0;i<w && valuePos<value.length();i++) {
                                int ch = value.charAt(valuePos++);
                                if (!ignored) {
                                    memory.writeb(address++, ch);
                                }
                            }
                            if (!ignored)
                                assigned++;
                            conversions++;
                            done = true;
                            break;
                        }
                        default:
                            Log.panic("Unhandled "+c+" in ssprintf");
                            done = true;
                            break;
                    }
                }
            }
        }
        return assigned;
    }
}

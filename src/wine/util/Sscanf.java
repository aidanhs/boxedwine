package wine.util;

import wine.builtin.libc.CType;
import wine.emulation.Memory;

public class Sscanf {
    public static int sscanf(Memory memory, String value, String format, Sprintf.SprintfGetter getter) {
        int formatPos = 0;
        int valuePos = 0;
        int nconversions = 0;
        int nassigned = 0;

        while (formatPos<format.length()) {
            char c = format.charAt(formatPos++);
            if (c == 0)
                break;
            if (CType.isspace(c) != 0) {
                while (valuePos < value.length() && CType.isspace(value.charAt(valuePos)) != 0) {
                    valuePos++;
                }
            }
            if (c != '%') {
                if (valuePos >= value.length())
                    return (nconversions != 0 ? nassigned : -1);
                if (value.charAt(valuePos)!=format.charAt(formatPos))
                    return nassigned;
                valuePos++;
            } else {
                while (true) {
                    c = format.charAt(formatPos++);
                    switch (c) {
                        case '%':
                            if (valuePos >= value.length())
                                return (nconversions != 0 ? nassigned : -1);
                            if (value.charAt(valuePos)!=format.charAt(formatPos))
                                return nassigned;
                            valuePos++;
                            break;
                        case '*':
                    }
                }
            }
        }
        return nassigned;
    }
}

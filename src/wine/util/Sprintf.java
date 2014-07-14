package wine.util;

public class Sprintf {
    public static interface SprintfGetter {
        public int getNextInt();
        public short getNextShort();
        public long getNextLong();
        public char getNextChar();
        public String getNextString();
        public double getNextDouble();
    }
    public static String sprintf(String format, SprintfGetter getter) {
        String originalFormat=format;
        int pos = format.indexOf('%');
        if (pos>=0) {
            StringBuffer buffer = new StringBuffer();
            while (pos>=0) {
                buffer.append(format.substring(0, pos));
                if (pos+1<format.length()) {
                    char c = format.charAt(++pos);
                    if (c == '%') {
                        buffer.append("%");
                        format = format.substring(2);
                    } else {
                        boolean leftJustify = false;
                        boolean showPlus = false;
                        boolean spaceSign = false;
                        boolean prefix = false;
                        boolean leftPadZero = false;
                        int width = 0;
                        int precision = -1;
                        boolean longValue = false;
                        boolean shortValue = false;

                        // flags
                        while (true) {
                            if (c=='-') {
                                leftJustify = true;
                            } else if (c=='+') {
                                showPlus = true;
                            } else if (c==' ') {
                                spaceSign = true;
                            } else if (c=='#') {
                                prefix = true;
                            } else if (c=='0') {
                                leftPadZero = true;
                            } else {
                                break;
                            }
                            if (pos+1<format.length()) {
                                c = format.charAt(++pos);
                            } else {
                                return buffer.toString();
                            }
                        }

                        // width
                        String w = "";
                        while (true) {
                            if (c>='0' && c<='9') {
                                w+=c;
                            } else {
                                break;
                            }
                            if (pos+1<format.length()) {
                                c = format.charAt(++pos);
                            } else {
                                return buffer.toString();
                            }
                        }
                        if (w.length()>0) {
                            width = Integer.parseInt(w);
                        }
                        if (c=='*') {
                            if (pos+1<format.length()) {
                                c = format.charAt(++pos);
                            }
                            width = getter.getNextInt();
                        }
                        // precision
                        if (c=='.') {
                            if (pos+1<format.length()) {
                                c = format.charAt(++pos);
                            } else {
                                return buffer.toString();
                            }

                            if (c=='*') {
                                precision = getter.getNextInt();
                                if (pos + 1 < format.length()) {
                                    c = format.charAt(++pos);
                                } else {
                                    return buffer.toString();
                                }
                            } else {
                                String p = "";
                                while (true) {
                                    if (c >= '0' && c <= '9') {
                                        p += c;
                                    } else {
                                        break;
                                    }
                                    if (pos + 1 < format.length()) {
                                        c = format.charAt(++pos);
                                    } else {
                                        return buffer.toString();
                                    }
                                }
                                if (p.length() > 0) {
                                    precision = Integer.parseInt(p);
                                }
                            }
                        }

                        // length
                        if (c=='h') {
                            shortValue = true;
                            if (pos+1<format.length()) {
                                c = format.charAt(++pos);
                            } else {
                                return buffer.toString();
                            }
                        } else if (c=='l') {
                            //longValue = true;
                            if (pos+1<format.length()) {
                                c = format.charAt(++pos);
                            } else {
                                return buffer.toString();
                            }
                        } else if (c=='L') {
                            //longValue = true;
                            if (pos+1<format.length()) {
                                c = format.charAt(++pos);
                            } else {
                                return buffer.toString();
                            }
                        }

                        String value = "";
                        String strPrfix = "";
                        boolean negnumber = false;
                        if (c == 'c') {
                            value = String.valueOf(getter.getNextChar());
                        } else if (c == 's') {
                            value = getter.getNextString();
                        } else if (c == 'x' || c=='X') {
                            if (shortValue) {
                                value = Integer.toHexString(getter.getNextShort());
                            } else if (longValue) {
                                value = Long.toHexString(getter.getNextLong());
                            } else {
                                value = Integer.toHexString(getter.getNextInt());
                            }
                            if (c=='x')
                                value=value.toLowerCase();
                            else
                                value=value.toUpperCase();
                            if (precision==0 && value.equals("0")) {
                                format = format.substring(pos);
                                continue;
                            }
                            if (precision>0) {
                                while (value.length()<precision) {
                                    value = "0"+value;
                                }
                            }
                            if (prefix) {
                                strPrfix += "0"+c+value;
                            }
                        } else if (c == 'u') {
                            if (shortValue) {
                                value = Integer.toString(getter.getNextShort() & 0xFFFF);
                            } else if (longValue) {
                                Log.warn("sprintf: unsigned long not full supported");
                                value = Long.toString(getter.getNextLong());
                            } else {
                                value = Long.toString(getter.getNextInt() & 0xFFFFFFFFl);
                            }
                            negnumber = value.startsWith("-");
                            if (negnumber)
                                value = value.substring(1);
                            if (precision==0 && value.equals("0")) {
                                format = format.substring(pos);
                                continue;
                            }
                            if (precision>0) {
                                while (value.length()<precision) {
                                    value = "0"+value;
                                }
                            }
                        } else if (c == 'd' || c == 'i') {
                            if (shortValue) {
                                value = Integer.toString(getter.getNextShort());
                            } else if (longValue) {
                                value = Long.toString(getter.getNextLong());
                            } else {
                                value = Integer.toString(getter.getNextInt());
                            }
                            negnumber = value.startsWith("-");
                            if (negnumber)
                                value = value.substring(1);
                            if (precision==0 && value.equals("0")) {
                                format = format.substring(pos);
                                continue;
                            }
                            if (precision>0) {
                                while (value.length()<precision) {
                                    value = "0"+value;
                                }
                            }
                        } else if (c == 'f') {
                            value = Double.toString(getter.getNextDouble());
                            negnumber = value.startsWith("-");
                            if (negnumber)
                                value = value.substring(1);
                            int dec = value.indexOf('.');
                            if (dec>=0) {
                                if (precision==0) {
                                    value = value.substring(0, dec);
                                } else if (value.length()>dec+1+precision) {
                                    value = value.substring(0, dec+1+precision);
                                }
                            }
                        } else if (c == 'p') {
                            value = Integer.toHexString(getter.getNextInt());
                            value="0x"+value.toLowerCase();
                        } else if (c == 'o') {
                            value=Integer.toOctalString(getter.getNextInt());
                        } else {
                            Log.panic("Unknown type in sprintf: "+c+" in ("+originalFormat+")");
                        }

                        if (negnumber) {
                            strPrfix = "-";
                        } else {
                            if (showPlus) {
                                strPrfix = "+"+strPrfix;
                            } else if (spaceSign) {
                                strPrfix = " "+strPrfix;
                            }
                        }
                        while (width>strPrfix.length()+value.length()) {
                            if (leftPadZero) {
                                strPrfix+="0";
                            } else if (leftJustify) {
                                value=value+" ";
                            } else {
                                strPrfix=" "+strPrfix;
                            }
                        }
                        buffer.append(strPrfix);
                        buffer.append(value);
                        format = format.substring(++pos);
                    }
                }
                pos = format.indexOf('%');
            }
            buffer.append(format);
            return buffer.toString();
        } else {
            return format;
        }
    }
}

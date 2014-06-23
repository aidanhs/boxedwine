package wine.builtin.libc;

public class In {
    // unsigned short ntohs(unsigned short s)
    static public int ntohs(int s) {
        return ((s & 0xFF) << 8) | ((s >> 8) & 0xFF);
    }
}

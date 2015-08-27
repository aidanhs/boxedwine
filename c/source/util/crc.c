unsigned int crc32b(unsigned char *message, int len) {
    unsigned int h = 0, g;
    int i;

    for (i=0;i<len;i++) {
        h = (h << 4) + message[i];
        g = h & 0xf0000000;
        if (g)
            h ^= g >> 24;
        h &= ~g;
    }
    return h;
}
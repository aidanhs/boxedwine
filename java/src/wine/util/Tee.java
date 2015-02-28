package wine.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class Tee extends InputStream {
    private InputStream is;
    private OutputStream os;

    public Tee(InputStream is, OutputStream os) {
        this.is = is;
        this.os = os;
    }
    public int read() throws IOException {
        int r = is.read();
        os.write(r);
        return r;
    }
}

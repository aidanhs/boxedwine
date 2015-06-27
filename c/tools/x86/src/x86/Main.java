package x86;

import java.io.FileOutputStream;
import java.util.Vector;

public class Main {
    static public void main(String[] args) {
        Vector<Base> ops = new Vector<Base>();
        ops.add(new Arith());
        ops.add(new IncDec());
        ops.add(new PushPop());
        ops.add(new Strings());
        ops.add(new Shift());
        ops.add(new Conditions());
        ops.add(new SetCC());
        ops.add(new Decoder());

        try {
            FileOutputStream fos = new FileOutputStream("decode.h");
            for (Base b : ops) {
                b.decode(fos);
                b.generate();
            }
            fos.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

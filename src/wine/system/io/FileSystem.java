package wine.system.io;

import wine.util.Path;

import java.io.*;
import java.nio.charset.Charset;
import java.util.Hashtable;
import java.util.Vector;

public class FileSystem {
    final static public Vector<Path> paths = new Vector<Path>();
    final static public Vector<Path> links = new Vector<Path>();
    final public static Hashtable<String, FSNode> openNodes = new Hashtable<String, FSNode>();

    static public void readLinks(String path) {
        Vector<String> lines = new Vector<String>();

        InputStream fis = null;
        BufferedReader br = null;

        try {
            String line;
            fis = new FileInputStream(path);
            br = new BufferedReader(new InputStreamReader(fis, Charset.forName("UTF-8")));
            while ((line = br.readLine()) != null) {
                lines.add(line);
            }
        } catch (IOException e) {
        } finally {
            if (br!=null) {
                try {
                    br.close();
                } catch (IOException e) {
                }
            }
        }
        for (String line : lines) {
            String[] args = line.split("\\|");
            if (args.length==2) {
                FileSystem.links.add(new Path(args[1], args[0]));
            } else {
                System.out.println("line is links file is not formatted properly: "+line);
            }
        }
    }
}

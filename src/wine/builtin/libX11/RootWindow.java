package wine.builtin.libX11;

import wine.system.WineSystem;

import javax.swing.*;
import java.awt.*;

public class RootWindow extends Window {
    public JFrame frame;
    public RootWindow() {
        super(null, 0, null);
        resize(WineSystem.SCREEN_WIDTH, WineSystem.SCREEN_HEIGHT);
        setProperty(Display.setAtom("_NET_WORKAREA", false), new Property(XAtom.XA_CARDINAL, 32, 0, 0, WineSystem.SCREEN_WIDTH, WineSystem.SCREEN_HEIGHT));

        frame = new JFrame();
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        Dimension d = new Dimension(width,height);
        Container c = frame.getContentPane();
        c.setBackground(Color.black);
        c.setPreferredSize(d);
        frame.setLayout(null);
        frame.getContentPane().add(panel);
        frame.pack();
        frame.setResizable(false);
        frame.setVisible(true);
    }
}

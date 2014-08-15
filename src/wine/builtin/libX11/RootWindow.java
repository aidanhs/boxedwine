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

        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                frame = new JFrame("Wine");
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

                Container pane = frame.getContentPane();
                pane.setLayout(null);
                panel.setBackground(Color.BLACK);
                pane.add(panel);
                panel.setBounds(0, 0, width, height);

                Insets insets = frame.getInsets();
                frame.setSize(width + insets.left + insets.right,
                        height + insets.top + insets.bottom);
                frame.setResizable(false);
                frame.setVisible(true);
            }
        });
    }
}

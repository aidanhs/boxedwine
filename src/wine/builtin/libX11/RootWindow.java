package wine.builtin.libX11;

import wine.builtin.libX11.events.XKeyPressedEvent;
import wine.builtin.libX11.events.XKeyReleasedEvent;
import wine.system.WineSystem;
import wine.util.Log;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class RootWindow extends Window {
    public JFrame frame;
    public Window keyboardFocus;
    public Window capture;
    public int focusRevertTo;

    public RootWindow() {
        super(null, 0, null);
        resize(WineSystem.SCREEN_WIDTH, WineSystem.SCREEN_HEIGHT);
        setProperty(Display.setAtom("_NET_WORKAREA", false), new Property(XAtom.XA_CARDINAL, 32, 0, 0, WineSystem.SCREEN_WIDTH, WineSystem.SCREEN_HEIGHT));

        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                frame = new JFrame("Wine");
                frame.addKeyListener(new KeyListener() {
                    public void keyTyped(KeyEvent e) {

                    }

                    public void keyPressed(KeyEvent e) {
                        if (keyboardFocus != null && (keyboardFocus.eventMask & Window.KeyPressMask) != 0) {
                            XKeyPressedEvent event = new XKeyPressedEvent(keyboardFocus.display, keyboardFocus.id);
                            event.root = id;
                            event.subwindow = 0;
                            event.time = (int) System.currentTimeMillis();
                            event.x = 0;
                            event.y = 0;
                            event.x_root = 0;
                            event.y_root = 0;
                            event.state = 0;
                            event.keycode = KeySym.javaKeyCodeToKeySym.get(e.getKeyCode()).keyCode;
                            event.same_screen = 1;
                            keyboardFocus.process.x11.addEvent(keyboardFocus.process, event);
                        }
                    }

                    public void keyReleased(KeyEvent e) {
                        if (keyboardFocus != null && (keyboardFocus.eventMask & Window.KeyPressMask) != 0) {
                            XKeyReleasedEvent event = new XKeyReleasedEvent(keyboardFocus.display, keyboardFocus.id);
                            event.root = id;
                            event.subwindow = 0;
                            event.time = (int) System.currentTimeMillis();
                            event.x = 0;
                            event.y = 0;
                            event.x_root = 0;
                            event.y_root = 0;
                            event.state = 0;
                            event.keycode = KeySym.javaKeyCodeToKeySym.get(e.getKeyCode()).keyCode;
                            event.same_screen = 1;
                            keyboardFocus.process.x11.addEvent(keyboardFocus.process, event);
                        }
                    }
                });
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

    public void unmapped(Window window) {
        if (window == keyboardFocus) {
            window.sendFocusOutEvent();
            keyboardFocus = null;
            if (focusRevertTo!=Window.RevertToNone) {
                Log.panic("Unmap window changed focus, not fully implemented");
            }
        }
    }
}

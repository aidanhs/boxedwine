package wine.gui;

import wine.Main;
import wine.emulation.Memory;
import wine.emulation.PageHandler;
import wine.system.kernel.Process;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferInt;
import java.util.Arrays;

public class Screen {
    static JFrame frame;
    static BufferedImage buffer;
    static int[] int_rawImageData;
    static JPanel panel;
    static public int width;
    static public int height;
    static public int depth;
    static private FBPageHandler handler;

    static public class FBPageHandler extends PageHandler {
        private int offset;
        public FBPageHandler(int offset) {
            this.offset = offset;
        }
        public void close() {

        }

        public int readd(Memory memory, int address) {
            address-=offset;
            int rem = (address & 0x3);
            if (rem == 0) {
                return int_rawImageData[address >>> 2];
            }
            int off = rem << 3;
            int[] local = int_rawImageData;
            int index = (address >>> 2);
            return local[index] >>> off | local[index + 1] << (32 - off);
        }

        public int readw(Memory memory, int address) {
            address-=offset;
            int rem = address & 0x3;
            int[] local = int_rawImageData;
            int index = (address >>> 2);
            int val = local[index] >>> (rem << 3);
            if (rem == 3) {
                val |= local[index + 1] << 8;
            }
            return val & 0xFFFF;
        }

        public int readb(int address) {
            address-=offset;
            return ((int_rawImageData[(address >>> 2)] >>> ((address & 0x3) << 3)) & 0xFF);
        }

        public void writed(Memory memory, int address, int value) {
            address-=offset;
            int rem = (address & 0x3);
            if (rem == 0) {
                int_rawImageData[address >>> 2] = value;
            } else {
                int index = (address >>> 2);
                int[] local = int_rawImageData;
                int off = rem << 3;
                int mask = -1 << off;
                local[index] = (local[index] & ~mask) | (value << off);
                index++;
                local[index] = (local[index] & mask) | (value >>> (32 - off));
            }
        }

        public void writew(Memory memory, int address, int value) {
            address-=offset;
            int rem = (address & 0x3);
            int[] local = int_rawImageData;
            int index = (address >>> 2);
            value &= 0xFFFF;
            if (rem == 3) {
                local[index] = (local[index] & 0xFFFFFF | value << 24);
                index++;
                local[index] = (local[index] & 0xFFFFFF00 | value >>> 8);
            } else {
                int off = rem << 3;
                int mask = ~(0xFFFF << off);
                local[index] = (local[index] & mask | value << off);
            }
        }

        public void writeb(int address, int value) {
            address-=offset;
            int off = (address & 0x3) << 3;
            int[] local = int_rawImageData;
            int mask = ~(0xFF << off);
            int index = (address >>> 2);
            int val = local[index] & mask | (value & 0xFF) << off;
            local[index] = val;
        }

        public PageHandler fork(Process process) {
            return this;
        }
    }

    static public void initProcess(Process process) {
        int pageCount = (width*height*4+0xFFF) >>> 12;
        for (int i=0;i<pageCount;i++)
            process.memory.handlers[Process.ADDRESS_PROCESS_FRAME_BUFFER+i] = handler;
    }

    static public void create(int cx, int cy, int bpp) {
        width = cx;
        height = cy;
        depth = bpp;

        handler = new FBPageHandler(wine.system.kernel.Process.ADDRESS_PROCESS_FRAME_BUFFER<<12);

        buffer = new BufferedImage(cx, cy*2, BufferedImage.TYPE_INT_RGB);
        DataBufferInt buf = (DataBufferInt) buffer.getRaster().getDataBuffer();
        int_rawImageData = buf.getData();
        Arrays.fill(int_rawImageData, 0x00FFFFFF);

        frame = new JFrame("BoxedWine");
        frame.setFocusTraversalKeysEnabled(false); // don't eat tab keys
        frame.addKeyListener(new KeyListener() {
            public void keyTyped(KeyEvent e) {

            }

            public void keyPressed(KeyEvent e) {

            }

            public void keyReleased(KeyEvent e) {

            }
        });

        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        panel = new JPanel() {
            public void paint(Graphics g) {
                g.drawImage(buffer, 0, 0, width, height, 0, 0, width, height, null);
            }
        };

        panel.addMouseListener(new MouseListener() {
            public void mouseClicked(MouseEvent e) {

            }

            public void mousePressed(MouseEvent e) {
                Main.mouse.mouseDown(e.getButton());
            }

            public void mouseReleased(MouseEvent e) {
                Main.mouse.mouseUp(e.getButton());
            }

            public void mouseEntered(MouseEvent e) {

            }

            public void mouseExited(MouseEvent e) {

            }
        });

        panel.addMouseMotionListener(new MouseMotionListener() {
            public void mouseDragged(MouseEvent e) {
                Main.mouse.event(e.getX(), e.getY());
            }

            public void mouseMoved(MouseEvent e) {
                Main.mouse.event(e.getX(), e.getY());
            }
        });

        final Container pane = frame.getContentPane();
        pane.setLayout(null);
        panel.setBackground(Color.BLACK);
        pane.add(panel);
        panel.setBounds(0, 0, cx, cy);

        Insets insets = frame.getInsets();
        frame.setSize(cx + insets.left + insets.right, cy + insets.top + insets.bottom);
        frame.setResizable(false);
        frame.setVisible(true);

        Thread thread = new Thread(new Runnable() {
            public void run() {
                while (true) {
                    panel.repaint();
                    try {Thread.sleep(10);} catch (Exception e) {}
                }
            }
        });
        thread.start();
    }
}

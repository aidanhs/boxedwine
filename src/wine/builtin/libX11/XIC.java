package wine.builtin.libX11;

public class XIC {
    public XIC(int id) {
        this.id = id;
    }
    public int id;
    public int inputStyle;
    public Window clientWindow;
    public Window focusWindow;
    public IM.IMCallback destroyCallback;
}

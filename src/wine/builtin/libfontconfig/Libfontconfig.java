package wine.builtin.libfontconfig;

import wine.loader.BuiltinModule;
import wine.system.WineProcess;
import wine.util.Log;

public class Libfontconfig extends BuiltinModule {
    public Libfontconfig(String name, WineProcess process, int id) {
        super(name, process, id);

        add_cdecl(Libfontconfig.class, "FcConfigSubstitute");
        add_cdecl(Libfontconfig.class, "FcFontList");
        add_cdecl(Libfontconfig.class, "FcFontSetDestroy");
        add_cdecl(Libfontconfig.class, "FcInit");
        add_cdecl(Libfontconfig.class, "FcObjectSetAdd");
        add_cdecl(Libfontconfig.class, "FcObjectSetCreate");
        add_cdecl(Libfontconfig.class, "FcObjectSetDestroy");
        add_cdecl(Libfontconfig.class, "FcPatternCreate");
        add_cdecl(Libfontconfig.class, "FcPatternDestroy");
        add_cdecl(Libfontconfig.class, "FcPatternGetBool");
        add_cdecl(Libfontconfig.class, "FcPatternGetInteger");
        add_cdecl(Libfontconfig.class, "FcPatternGetString");
    }

    // FcBool FcConfigSubstitute(FcConfig *config, FcPattern *p, FcMatchKind kind)
    static public int FcConfigSubstitute(int config, int p, int kind) {
        Log.panic("FcConfigSubstitute not implemented");
        return 0;
    }

    // FcFontSet * FcFontList(FcConfig *config, FcPattern *p, FcObjectSet *os)
    static public int FcFontList(int config, int p, int os) {
        Log.panic("FcFontList not implemented");
        return 0;
    }

    // void FcFontSetDestroy(FcFontSet *s)
    static public void FcFontSetDestroy(int s) {
        Log.panic("FcFontSetDestroy not implemented");
    }

    // FcBool FcInit(void)
    static public int FcInit() {
        return 0;
    }

    // FcBool FcObjectSetAdd(FcObjectSet *os, const char *object)
    static public int FcObjectSetAdd(int os, int object) {
        Log.panic("FcObjectSetAdd not implemented");
        return 0;
    }

    // FcObjectSet * FcObjectSetCreate(void)
    static public int FcObjectSetCreate() {
        Log.panic("FcObjectSetCreate not implemented");
        return 0;
    }

    // void FcObjectSetDestroy(FcObjectSet *os)
    static public void FcObjectSetDestroy(int os) {
        Log.panic("FcObjectSetDestroy not implemented");
    }

    // FcPattern * FcPatternCreate(void)
    static public int FcPatternCreate() {
        Log.panic("FcPatternCreate not implemented");
        return 0;
    }

    // void FcPatternDestroy(FcPattern *p)
    static public void FcPatternDestroy(int p) {
        Log.panic("FcPatternDestroy not implemented");
    }

    // FcResult FcPatternGetBool(FcPattern *p, const char *object, int n, FcBool *b)
    static public int FcPatternGetBool(int p, int object, int n, int b) {
        Log.panic("FcPatternGetBool not implemented");
        return 0;
    }

    // FcResult FcPatternGetInteger(FcPattern *p, const char *object, int n, int *i)
    static public int FcPatternGetInteger(int p, int object, int n, int i) {
        Log.panic("FcPatternGetInteger not implemented");
        return 0;
    }

    // FcResult FcPatternGetString(FcPattern *p, const char *object, int n, FcChar8 **s)
    static public int FcPatternGetString(int p, int object, int n, int s) {
        Log.panic("FcPatternGetString not implemented");
        return 0;
    }
}

package wine.builtin.libfreetype;

import wine.emulation.Memory;
import wine.loader.BuiltinModule;
import wine.system.WineProcess;
import wine.system.WineThread;
import wine.util.Log;

public class Libfreetype extends BuiltinModule {
    public Libfreetype(String name, WineProcess process, int id) {
        super(name, process, id);

        add_cdecl(Libfreetype.class, "FT_Done_Face");
        add_cdecl(Libfreetype.class, "FT_Get_Char_Index");
        add_cdecl(Libfreetype.class, "FT_Get_First_Char");
        add_cdecl(Libfreetype.class, "FT_Get_Next_Char");
        add_cdecl(Libfreetype.class, "FT_Get_Sfnt_Name");
        add_cdecl(Libfreetype.class, "FT_Get_Sfnt_Name_Count");
        add_cdecl(Libfreetype.class, "FT_Get_Sfnt_Table");
        add_cdecl(Libfreetype.class, "FT_Get_WinFNT_Header");
        add_cdecl(Libfreetype.class, "FT_Init_FreeType");
        add_cdecl(Libfreetype.class, "FT_Library_Version");
        add_cdecl(Libfreetype.class, "FT_Load_Glyph");
        add_cdecl(Libfreetype.class, "FT_Load_Sfnt_Table");
        add_cdecl(Libfreetype.class, "FT_Matrix_Multiply");
        add_cdecl(Libfreetype.class, "FT_MulFix");
        add_cdecl(Libfreetype.class, "FT_New_Face");
        add_cdecl(Libfreetype.class, "FT_New_Memory_Face");
        add_cdecl(Libfreetype.class, "FT_Outline_Get_Bitmap");
        add_cdecl(Libfreetype.class, "FT_Outline_Get_CBox");
        add_cdecl(Libfreetype.class, "FT_Outline_Transform");
        add_cdecl(Libfreetype.class, "FT_Outline_Translate");
        add_cdecl(Libfreetype.class, "FT_Render_Glyph");
        add_cdecl(Libfreetype.class, "FT_Select_Charmap");
        add_cdecl(Libfreetype.class, "FT_Set_Charmap");
        add_cdecl(Libfreetype.class, "FT_Set_Pixel_Sizes");
        add_cdecl(Libfreetype.class, "FT_Vector_Transform");
        add_cdecl(Libfreetype.class, "FT_Vector_Unit");
    }

    // FT_Error FT_Done_Face( FT_Face face )
    static public int FT_Done_Face(int face) {
        Log.panic("FT_Done_Face not implemented");
        return 0;
    }

    // FT_UInt FT_Get_Char_Index( FT_Face face, FT_ULong charcode )
    static public int FT_Get_Char_Index(int face, int charcode) {
        Log.panic("FT_Get_Char_Index not implemented");
        return 0;
    }

    // FT_ULong FT_Get_First_Char( FT_Face face, FT_UInt *agindex )
    static public int FT_Get_First_Char(int face, int agindex) {
        Log.panic("FT_Get_First_Char not implemented");
        return 0;
    }

    // FT_ULong FT_Get_Next_Char( FT_Face face, FT_ULong char_code, FT_UInt *agindex )
    static public int FT_Get_Next_Char(int face, int char_code, int agindex) {
        Log.panic("FT_Get_Next_Char not implemented");
        return 0;
    }

    // FT_Error FT_Get_Sfnt_Name( FT_Face face, FT_UInt idx, FT_SfntName  *aname )
    static public int FT_Get_Sfnt_Name(int face, int idx, int aname) {
        Log.panic("FT_Get_Sfnt_Name not implemented");
        return 0;
    }

    // FT_UInt FT_Get_Sfnt_Name_Count( FT_Face face )
    static public int FT_Get_Sfnt_Name_Count(int face) {
        Log.panic("FT_Get_Sfnt_Name_Count not implemented");
        return 0;
    }

    // void* FT_Get_Sfnt_Table( FT_Face face, FT_Sfnt_Tag tag )
    static public int FT_Get_Sfnt_Table(int face, int tag) {
        Log.panic("FT_Get_Sfnt_Table not implemented");
        return 0;
    }

    // FT_Error FT_Get_WinFNT_Header( FT_Face face, FT_WinFNT_HeaderRec *aheader )
    static public int FT_Get_WinFNT_Header(int face, int aheader) {
        Log.panic("FT_Get_WinFNT_Header not implemented");
        return 0;
    }

    // FT_Error FT_Init_FreeType( FT_Library *alibrary )
    static public int FT_Init_FreeType(int alibrary) {
        return 1;
    }

    // void FT_Library_Version( FT_Library library, FT_Int *amajor, FT_Int *aminor, FT_Int *apatch )
    static public void FT_Library_Version(int library, int amajor, int aminor, int apatch) {
        WineThread thread = WineThread.getCurrent();
        Memory memory = thread.process.memory;
        memory.writed(amajor, 2);
        memory.writed(aminor, 5);
        memory.writed(apatch, 2);
    }

    // FT_Error FT_Load_Glyph( FT_Face face, FT_UInt glyph_index, FT_Int32 load_flags )
    static public int FT_Load_Glyph(int face, int glyph_index, int load_flags) {
        Log.panic("FT_Load_Glyph not implemented");
        return 0;
    }

    // FT_Error FT_Load_Sfnt_Table( FT_Face face, FT_ULong tag, FT_Long offset, FT_Byte* buffer, FT_ULong* length )
    static public int FT_Load_Sfnt_Table(int face, int tag, int offset, int buffer, int length) {
        Log.panic("FT_Load_Sfnt_Table not implemented");
        return 0;
    }

    // void FT_Matrix_Multiply( const FT_Matrix* a, FT_Matrix* b )
    static public void FT_Matrix_Multiply(int a, int b) {
        Log.panic("FT_Matrix_Multiply not implemented");
    }

    // FT_Long FT_MulFix( FT_Long a, FT_Long b);
    static public int FT_MulFix(int a, int b) {
        Log.panic("FT_MulFix not implemented");
        return 0;
    }

    // FT_Error FT_New_Face( FT_Library library, const char* filepathname, FT_Long face_index, FT_Face *aface )
    static public int FT_New_Face(int library, int filepathname, int face_index, int aface) {
        Log.panic("FT_New_Face not implemented");
        return 0;
    }

    // FT_Error FT_New_Memory_Face( FT_Library library, const FT_Byte* file_base, FT_Long file_size, FT_Long face_index, FT_Face *aface )
    static public int FT_New_Memory_Face(int library, int file_base, int file_size, int face_index, int aface) {
        Log.panic("FT_New_Memory_Face not implemented");
        return 0;
    }

    // FT_Error FT_Outline_Get_Bitmap( FT_Library library, FT_Outline* outline, const FT_Bitmap *abitmap )
    static public int FT_Outline_Get_Bitmap(int library, int outline, int abitmap) {
        Log.panic("FT_Outline_Get_Bitmap not implemented");
        return 0;
    }

    // void FT_Outline_Get_CBox( const FT_Outline* outline, FT_BBox *acbox );
    static public void FT_Outline_Get_CBox(int outline, int acbox) {
        Log.panic("FT_Outline_Get_CBox not implemented");
    }

    // void FT_Outline_Transform( const FT_Outline* outline, const FT_Matrix* matrix )
    static public void FT_Outline_Transform(int outline, int matrix) {
        Log.panic("FT_Outline_Transform not implemented");
    }

    // void FT_Outline_Translate( const FT_Outline* outline, FT_Pos xOffset, FT_Pos yOffset )
    static public void FT_Outline_Translate(int outline, int oxOffset, int yOffset) {
        Log.panic("FT_Outline_Translate not implemented");
    }

    // FT_Error FT_Render_Glyph( FT_GlyphSlot slot, FT_Render_Mode render_mode )
    static public int FT_Render_Glyph(int slot, int render_mode) {
        Log.panic("FT_Render_Glyph not implemented");
        return 0;
    }

    // FT_Error FT_Select_Charmap( FT_Face face, FT_Encoding encoding )
    static public int FT_Select_Charmap(int face, int encoding) {
        Log.panic("FT_Select_Charmap not implemented");
        return 0;
    }

    // FT_Error FT_Set_Charmap( FT_Face face, FT_CharMap charmap )
    static public int FT_Set_Charmap(int face, int charmap) {
        Log.panic("FT_Set_Charmap not implemented");
        return 0;
    }

    // FT_Error FT_Set_Pixel_Sizes( FT_Face face, FT_UInt pixel_width, FT_UInt pixel_height )
    static public int FT_Set_Pixel_Sizes(int face, int pixel_width, int pixel_height) {
        Log.panic("FT_Set_Pixel_Sizes not implemented");
        return 0;
    }

    // void FT_Vector_Transform( FT_Vector* vec, const FT_Matrix* matrix );
    static public void FT_Vector_Transform(int vec, int matrix) {
        Log.panic("FT_Vector_Transform not implemented");
    }

    // void FT_Vector_Unit( FT_Vector* vec, FT_Angle angle );
    static public void FT_Vector_Unit(int vec, int angle) {
        Log.panic("FT_Vector_Unit not implemented");
    }
}

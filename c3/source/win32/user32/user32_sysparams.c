#include "../include/winbase.h"

int screenCX = 800;
int screenCY = 600;

struct sysparam_rgb_entry
{
    COLORREF              val;
    HBRUSH                brush;
    HPEN                  pen;
};

static const WORD wPattern55AA[] = { 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa, 0x5555, 0xaaaa };
static HBRUSH SYSCOLOR_55AABrush = 0;

HBRUSH getSYSColor_55AABrush() {
    if (!SYSCOLOR_55AABrush) {
        HBITMAP h55AABitmap = CreateBitmap_k( 8, 8, 1, 1, wPattern55AA );
        SYSCOLOR_55AABrush = CreatePatternBrush_k( h55AABitmap );
        DeleteObject_k( h55AABitmap );
    }
    return SYSCOLOR_55AABrush;
}

static struct sysparam_rgb_entry system_colors[] =
{
#define RGB_ENTRY(name,val) { (val) }
    RGB_ENTRY( COLOR_SCROLLBAR, RGB(212, 208, 200) ),
    RGB_ENTRY( COLOR_BACKGROUND, RGB(58, 110, 165) ),
    RGB_ENTRY( COLOR_ACTIVECAPTION, RGB(10, 36, 106) ),
    RGB_ENTRY( COLOR_INACTIVECAPTION, RGB(128, 128, 128) ),
    RGB_ENTRY( COLOR_MENU, RGB(212, 208, 200) ),
    RGB_ENTRY( COLOR_WINDOW, RGB(255, 255, 255) ),
    RGB_ENTRY( COLOR_WINDOWFRAME, RGB(0, 0, 0) ),
    RGB_ENTRY( COLOR_MENUTEXT, RGB(0, 0, 0) ),
    RGB_ENTRY( COLOR_WINDOWTEXT, RGB(0, 0, 0) ),
    RGB_ENTRY( COLOR_CAPTIONTEXT, RGB(255, 255, 255) ),
    RGB_ENTRY( COLOR_ACTIVEBORDER, RGB(212, 208, 200) ),
    RGB_ENTRY( COLOR_INACTIVEBORDER, RGB(212, 208, 200) ),
    RGB_ENTRY( COLOR_APPWORKSPACE, RGB(128, 128, 128) ),
    RGB_ENTRY( COLOR_HIGHLIGHT, RGB(10, 36, 106) ),
    RGB_ENTRY( COLOR_HIGHLIGHTTEXT, RGB(255, 255, 255) ),
    RGB_ENTRY( COLOR_BTNFACE, RGB(212, 208, 200) ),
    RGB_ENTRY( COLOR_BTNSHADOW, RGB(128, 128, 128) ),
    RGB_ENTRY( COLOR_GRAYTEXT, RGB(128, 128, 128) ),
    RGB_ENTRY( COLOR_BTNTEXT, RGB(0, 0, 0) ),
    RGB_ENTRY( COLOR_INACTIVECAPTIONTEXT, RGB(212, 208, 200) ),
    RGB_ENTRY( COLOR_BTNHIGHLIGHT, RGB(255, 255, 255) ),
    RGB_ENTRY( COLOR_3DDKSHADOW, RGB(64, 64, 64) ),
    RGB_ENTRY( COLOR_3DLIGHT, RGB(212, 208, 200) ),
    RGB_ENTRY( COLOR_INFOTEXT, RGB(0, 0, 0) ),
    RGB_ENTRY( COLOR_INFOBK, RGB(255, 255, 225) ),
    RGB_ENTRY( COLOR_ALTERNATEBTNFACE, RGB(181, 181, 181) ),
    RGB_ENTRY( COLOR_HOTLIGHT, RGB(0, 0, 200) ),
    RGB_ENTRY( COLOR_GRADIENTACTIVECAPTION, RGB(166, 202, 240) ),
    RGB_ENTRY( COLOR_GRADIENTINACTIVECAPTION, RGB(192, 192, 192) ),
    RGB_ENTRY( COLOR_MENUHILIGHT, RGB(10, 36, 106) ),
    RGB_ENTRY( COLOR_MENUBAR, RGB(212, 208, 200) )
#undef RGB_ENTRY
};
#define NUM_SYS_COLORS (sizeof(system_colors) / sizeof(system_colors[0]))


UINT WINAPI GetDoubleClickTime_k(void)
{
    return 500;
}

COLORREF WINAPI GetSysColor_k( INT nIndex )
{
    COLORREF ret = 0;

    if (nIndex >= 0 && nIndex < NUM_SYS_COLORS) return system_colors[nIndex].val;
    return ret;
}

/*************************************************************************
 *		SetSysColors (USER32.@)
 */
BOOL WINAPI SetSysColors_k( INT count, const INT *colors, const COLORREF *values )
{
    int i;

    if (IS_INTRESOURCE(colors)) return FALSE; /* stupid app passes a color instead of an array */

    for (i = 0; i < count; i++)
        if (colors[i] >= 0 && colors[i] <= NUM_SYS_COLORS) {
            system_colors[colors[i]].val;
            if (system_colors[colors[i]].brush) {
                DeleteObject_k(system_colors[colors[i]].brush);
                system_colors[colors[i]].brush = 0;
            }
            if (system_colors[colors[i]].pen) {
                DeleteObject_k(system_colors[colors[i]].pen);
                system_colors[colors[i]].pen = 0;
            }
        }

    /* Send WM_SYSCOLORCHANGE message to all windows */

    SendMessageTimeoutW_k( HWND_BROADCAST, WM_SYSCOLORCHANGE, 0, 0, SMTO_ABORTIFHUNG, 2000, NULL );

    /* Repaint affected portions of all visible windows */

    RedrawWindow_k( GetDesktopWindow_k(), NULL, 0, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ALLCHILDREN );
    return TRUE;
}


/*************************************************************************
 *		SetSysColorsTemp (USER32.@)
 */
DWORD_PTR WINAPI SetSysColorsTemp_k( const COLORREF *pPens, const HBRUSH *pBrushes, DWORD_PTR n)
{
    FIXME( "no longer supported\n" );
    return FALSE;
}


/***********************************************************************
 *		GetSysColorBrush (USER32.@)
 */
HBRUSH WINAPI GetSysColorBrush_k( INT index )
{
    if (index < 0 || index >= NUM_SYS_COLORS) return 0;

    if (!system_colors[index].brush)
    {
        HBRUSH brush = CreateSolidBrush_k( GetSysColor_k( index ));
        if (InterlockedCompareExchangePointer_k( (void **)&system_colors[index].brush, brush, 0 ))
        {
            DeleteObject_k( brush );
        }
    }
    return system_colors[index].brush;
}


/***********************************************************************
 *		SYSCOLOR_GetPen
 */
HPEN SYSCOLOR_GetPen( INT index )
{
    if (!system_colors[index].pen)
    {
        HPEN pen = CreatePen_k( PS_SOLID, 1, GetSysColor_k( index ));
        if (InterlockedCompareExchangePointer_k( (void **)&system_colors[index].pen, pen, 0 ))
        {
            DeleteObject_k( pen );
        }
    }
    return system_colors[index].pen;
}


/***********************************************************************
 *		GetSystemMetrics (USER32.@)
 */
INT WINAPI GetSystemMetrics_k( INT index )
{
    //NONCLIENTMETRICSW ncm;
    //UINT ret;

    /* some metrics are dynamic */
    switch (index)
    {
    case SM_CXSCREEN:
        return screenCX;
    case SM_CYSCREEN:
        return screenCY;
    case SM_CXVSCROLL:
    case SM_CYHSCROLL:
        return 16;
    case SM_CYCAPTION:
        return GetSystemMetrics_k( SM_CYSIZE ) + 1;
    case SM_CXBORDER:
    case SM_CYBORDER:
        /* SM_C{X,Y}BORDER always returns 1 regardless of 'BorderWidth' value in registry */
        return 1;
    case SM_CXDLGFRAME:
    case SM_CYDLGFRAME:
        return 3;
    case SM_CYVTHUMB:
    case SM_CXHTHUMB:
    case SM_CYVSCROLL:
    case SM_CXHSCROLL:
        return 16;
    case SM_CXICON:
    case SM_CYICON:
        return 32;
    case SM_CXCURSOR:
    case SM_CYCURSOR:
        return 32;
    case SM_CYMENU:
        return GetSystemMetrics_k(SM_CYMENUSIZE) + 1;
    case SM_CXFULLSCREEN:
        /* see the remark for SM_CXMAXIMIZED, at least this formulation is
         * correct */
        return GetSystemMetrics_k( SM_CXMAXIMIZED) - 2 * GetSystemMetrics_k( SM_CXFRAME);
    case SM_CYFULLSCREEN:
        /* see the remark for SM_CYMAXIMIZED, at least this formulation is
         * correct */
        return GetSystemMetrics_k( SM_CYMAXIMIZED) - GetSystemMetrics_k( SM_CYMIN);
    case SM_CYKANJIWINDOW:
        return 0;
    case SM_MOUSEPRESENT:
        return 1;
    case SM_DEBUG:
        return 0;
    case SM_SWAPBUTTON:
        return 0;
    case SM_RESERVED1:
    case SM_RESERVED2:
    case SM_RESERVED3:
    case SM_RESERVED4:
        return 0;
    case SM_CXMIN:
        /*
        ncm.cbSize = sizeof(ncm);
        SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, 0, &ncm, 0 );
        get_text_metr_size( get_display_dc(), &ncm.lfCaptionFont, NULL, &ret );
        return 3 * ncm.iCaptionWidth + ncm.iCaptionHeight + 4 * ret + 2 * GetSystemMetrics_k(SM_CXFRAME) + 4;
        */
        return 112;
    case SM_CYMIN:
        return GetSystemMetrics_k( SM_CYCAPTION) + 2 * GetSystemMetrics_k( SM_CYFRAME);
    case SM_CXSIZE:
        return 18;
    case SM_CYSIZE:
        /*
        ncm.cbSize = sizeof(ncm);
        SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, 0, &ncm, 0 );
        return ncm.iCaptionHeight;
        */
        return 27;
    case SM_CXFRAME:
        return GetSystemMetrics_k(SM_CXDLGFRAME) + 1;
    case SM_CYFRAME:
        return GetSystemMetrics_k(SM_CYDLGFRAME) + 1;
    case SM_CXMINTRACK:
        return GetSystemMetrics_k(SM_CXMIN);
    case SM_CYMINTRACK:
        return GetSystemMetrics_k(SM_CYMIN);
    case SM_CXDOUBLECLK:
        return 4;
    case SM_CYDOUBLECLK:
        return 4;
    case SM_CXICONSPACING:
        return 75;
    case SM_CYICONSPACING:
        return 75;
    case SM_MENUDROPALIGNMENT:
        return 0;
    case SM_PENWINDOWS:
        return 0;
    case SM_DBCSENABLED:
    {
        CPINFO cpinfo;
        GetCPInfo_k( CP_ACP, &cpinfo );
        return (cpinfo.MaxCharSize > 1);
    }
    case SM_CMOUSEBUTTONS:
        return 3;
    case SM_SECURE:
        return 0;
    case SM_CXEDGE:
        return GetSystemMetrics_k(SM_CXBORDER) + 1;
    case SM_CYEDGE:
        return GetSystemMetrics_k(SM_CYBORDER) + 1;
    case SM_CXMINSPACING:
        return GetSystemMetrics_k(SM_CXMINIMIZED);
    case SM_CYMINSPACING:
        return GetSystemMetrics_k(SM_CYMINIMIZED);
    case SM_CXSMICON:
    case SM_CYSMICON:
        return 16;
    case SM_CYSMCAPTION:
        return GetSystemMetrics_k(SM_CYSMSIZE) + 1;
    case SM_CXSMSIZE:
        return 15;
    case SM_CYSMSIZE:
        /*
        ncm.cbSize = sizeof(ncm);
        SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, 0, &ncm, 0 );
        return ncm.iSmCaptionHeight;
        */
        return 15;
    case SM_CXMENUSIZE:
        return 18;
    case SM_CYMENUSIZE:
        /*
        ncm.cbSize = sizeof(ncm);
        SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, 0, &ncm, 0 );
        return ncm.iMenuHeight;
        */
        return 18;
    case SM_ARRANGE:
        return ARW_HIDE & 0x0f;
    case SM_CXMINIMIZED:
        return 160;
    case SM_CYMINIMIZED:
        return GetSystemMetrics_k( SM_CYSIZE ) + 6;
    case SM_CXMAXTRACK:
        return GetSystemMetrics_k(SM_CXVIRTUALSCREEN) + 4 + 2 * GetSystemMetrics_k(SM_CXFRAME);
    case SM_CYMAXTRACK:
        return GetSystemMetrics_k(SM_CYVIRTUALSCREEN) + 4 + 2 * GetSystemMetrics_k(SM_CYFRAME);
    case SM_CXMAXIMIZED:
        /* FIXME: subtract the width of any vertical application toolbars*/
        return GetSystemMetrics_k(SM_CXSCREEN) + 2 * GetSystemMetrics_k(SM_CXFRAME);
    case SM_CYMAXIMIZED:
        /* FIXME: subtract the width of any horizontal application toolbars*/
        return GetSystemMetrics_k(SM_CYSCREEN) + 2 * GetSystemMetrics_k(SM_CYCAPTION);
    case SM_NETWORK:
        return 3;  /* FIXME */
    case SM_CLEANBOOT:
        return 0; /* 0 = ok, 1 = failsafe, 2 = failsafe + network */
    case SM_CXDRAG:
        return 4;
    case SM_CYDRAG:
        return 4;
    case SM_SHOWSOUNDS:
        return 0;
    case SM_CXMENUCHECK:
    case SM_CYMENUCHECK:
        /*
    {
        TEXTMETRICW tm;
        ncm.cbSize = sizeof(ncm);
        SystemParametersInfoW( SPI_GETNONCLIENTMETRICS, 0, &ncm, 0 );
        get_text_metr_size( get_display_dc(), &ncm.lfMenuFont, &tm, NULL);
        return tm.tmHeight <= 0 ? 13 : ((tm.tmHeight + tm.tmExternalLeading + 1) / 2) * 2 - 1;
    }
    */
        return 13;
    case SM_SLOWMACHINE:
        return 0;  /* Never true */
    case SM_MIDEASTENABLED:
        return 0;  /* FIXME */
    case SM_MOUSEWHEELPRESENT:
        return 1;
    case SM_XVIRTUALSCREEN:
        return 0;
    case SM_YVIRTUALSCREEN:
        return 0;
    case SM_CXVIRTUALSCREEN:
        return GetSystemMetrics_k(SM_CXSCREEN);
    case SM_CYVIRTUALSCREEN:
        return GetSystemMetrics_k(SM_CYSCREEN);
    case SM_CMONITORS:
        return 1;
    case SM_SAMEDISPLAYFORMAT:
        return 1;
    case SM_IMMENABLED:
        return 0;  /* FIXME */
    case SM_CXFOCUSBORDER:
    case SM_CYFOCUSBORDER:
        return 1;
    case SM_TABLETPC:
    case SM_MEDIACENTER:
        return 0;
    case SM_CMETRICS:
        return SM_CMETRICS;
    default:
        return 0;
    }
}

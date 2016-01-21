/*
 * DC clipping functions
 *
 * Copyright 1993 Alexandre Julliard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>
#include <stdlib.h>
#include "../include/winbase.h"
#include "gdi_private.h"


/* return the DC device rectangle if not empty */
static INLINE BOOL get_dc_device_rect( DC *dc, RECT *rect )
{
    *rect = dc->device_rect;
    offset_rect( rect, -dc->vis_rect.left, -dc->vis_rect.top );
    return !is_rect_empty( rect );
}

/***********************************************************************
 *           get_clip_rect
 *
 * Compute a clip rectangle from its logical coordinates.
 */
static INLINE RECT get_clip_rect( DC * dc, int left, int top, int right, int bottom )
{
    RECT rect;

    rect.left   = left;
    rect.top    = top;
    rect.right  = right;
    rect.bottom = bottom;
    LPtoDP_k( dc->hSelf, (POINT *)&rect, 2 );
    if (dc->layout & LAYOUT_RTL)
    {
        int tmp = rect.left;
        rect.left = rect.right + 1;
        rect.right = tmp + 1;
    }
    return rect;
}

/***********************************************************************
 *           clip_device_rect
 *
 * Clip a rectangle to the whole DC surface.
 */
BOOL clip_device_rect( DC *dc, RECT *dst, const RECT *src )
{
    RECT clip;

    if (get_dc_device_rect( dc, &clip )) return intersect_rect( dst, src, &clip );
    *dst = *src;
    return TRUE;
}

/***********************************************************************
 *           clip_visrect
 *
 * Clip a rectangle to the DC visible rect.
 */
BOOL clip_visrect( DC *dc, RECT *dst, const RECT *src )
{
    RECT clip;

    if (!clip_device_rect( dc, dst, src )) return FALSE;
    if (GetRgnBox_k( get_dc_region(dc), &clip )) return intersect_rect( dst, dst, &clip );
    return TRUE;
}

/***********************************************************************
 *           update_dc_clipping
 *
 * Update the DC and device clip regions when the ClipRgn or VisRgn have changed.
 */
void update_dc_clipping( DC * dc )
{
    PHYSDEV physdev = GET_DC_PHYSDEV( dc, pSetDeviceClipping );
    HRGN regions[3];
    int count = 0;

    if (dc->hVisRgn)  regions[count++] = dc->hVisRgn;
    if (dc->hClipRgn) regions[count++] = dc->hClipRgn;
    if (dc->hMetaRgn) regions[count++] = dc->hMetaRgn;

    if (count > 1)
    {
        if (!dc->region) dc->region = CreateRectRgn_k( 0, 0, 0, 0 );
        CombineRgn_k( dc->region, regions[0], regions[1], RGN_AND );
        if (count > 2) CombineRgn_k( dc->region, dc->region, regions[2], RGN_AND );
    }
    else  /* only one region, we don't need the total region */
    {
        if (dc->region) DeleteObject_k( dc->region );
        dc->region = 0;
    }
    physdev->funcs->pSetDeviceClipping( physdev, get_dc_region( dc ));
}

/***********************************************************************
 *           create_default_clip_region
 *
 * Create a default clipping region when none already exists.
 */
static INLINE void create_default_clip_region( DC * dc )
{
    RECT rect;

    if (!get_dc_device_rect( dc, &rect ))
    {
        rect.left = 0;
        rect.top = 0;
        rect.right = GetDeviceCaps_k( dc->hSelf, DESKTOPHORZRES );
        rect.bottom = GetDeviceCaps_k( dc->hSelf, DESKTOPVERTRES );
    }
    dc->hClipRgn = CreateRectRgnIndirect_k( &rect );
}


/***********************************************************************
 *           null driver fallback implementations
 */

INT nulldrv_ExtSelectClipRgn( PHYSDEV dev, HRGN rgn, INT mode )
{
    DC *dc = get_nulldrv_dc( dev );
    INT ret;

    if (!rgn)
    {
        switch (mode)
        {
        case RGN_COPY:
            if (dc->hClipRgn) DeleteObject_k( dc->hClipRgn );
            dc->hClipRgn = 0;
            ret = SIMPLEREGION;
            break;

        case RGN_DIFF:
            return ERROR;

        default:
            FIXME("Unimplemented: hrgn NULL in mode: %d\n", mode);
            return ERROR;
        }
    }
    else
    {
        HRGN mirrored = 0;

        if (dc->layout & LAYOUT_RTL)
        {
            if (!(mirrored = CreateRectRgn_k( 0, 0, 0, 0 ))) return ERROR;
            mirror_region( mirrored, rgn, dc->vis_rect.right - dc->vis_rect.left );
            rgn = mirrored;
        }

        if (!dc->hClipRgn)
            create_default_clip_region( dc );

        if (mode == RGN_COPY)
            ret = CombineRgn_k( dc->hClipRgn, rgn, 0, mode );
        else
            ret = CombineRgn_k( dc->hClipRgn, dc->hClipRgn, rgn, mode);

        if (mirrored) DeleteObject_k( mirrored );
    }
    update_dc_clipping( dc );
    return ret;
}

INT nulldrv_ExcludeClipRect( PHYSDEV dev, INT left, INT top, INT right, INT bottom )
{
    DC *dc = get_nulldrv_dc( dev );
    RECT rect = get_clip_rect( dc, left, top, right, bottom );
    INT ret;
    HRGN rgn;

    if (!(rgn = CreateRectRgnIndirect_k( &rect ))) return ERROR;
    if (!dc->hClipRgn) create_default_clip_region( dc );
    ret = CombineRgn_k( dc->hClipRgn, dc->hClipRgn, rgn, RGN_DIFF );
    DeleteObject_k( rgn );
    if (ret != ERROR) update_dc_clipping( dc );
    return ret;
}

INT nulldrv_IntersectClipRect( PHYSDEV dev, INT left, INT top, INT right, INT bottom )
{
    DC *dc = get_nulldrv_dc( dev );
    RECT rect = get_clip_rect( dc, left, top, right, bottom );
    INT ret;
    HRGN rgn;

    if (!dc->hClipRgn)
    {
        dc->hClipRgn = CreateRectRgnIndirect_k( &rect );
        ret = SIMPLEREGION;
    }
    else
    {
        if (!(rgn = CreateRectRgnIndirect_k( &rect ))) return ERROR;
        ret = CombineRgn_k( dc->hClipRgn, dc->hClipRgn, rgn, RGN_AND );
        DeleteObject_k( rgn );
    }
    if (ret != ERROR) update_dc_clipping( dc );
    return ret;
}

INT nulldrv_OffsetClipRgn( PHYSDEV dev, INT x, INT y )
{
    DC *dc = get_nulldrv_dc( dev );
    INT ret = NULLREGION;

    if (dc->hClipRgn)
    {
        x = MulDiv_k( x, dc->vportExtX, dc->wndExtX );
        y = MulDiv_k( y, dc->vportExtY, dc->wndExtY );
        if (dc->layout & LAYOUT_RTL) x = -x;
        ret = OffsetRgn_k( dc->hClipRgn, x, y );
	update_dc_clipping( dc );
    }
    return ret;
}


/***********************************************************************
 *           SelectClipRgn    (GDI32.@)
 */
INT WINAPI SelectClipRgn_k( HDC hdc, HRGN hrgn )
{
    return ExtSelectClipRgn_k( hdc, hrgn, RGN_COPY );
}


/******************************************************************************
 *		ExtSelectClipRgn	[GDI32.@]
 */
INT WINAPI ExtSelectClipRgn_k( HDC hdc, HRGN hrgn, INT fnMode )
{
    PHYSDEV physdev;
    INT retval;
    DC * dc = get_dc_ptr( hdc );

    TRACE("%p %p %d\n", hdc, hrgn, fnMode );

    if (!dc) return ERROR;
    update_dc( dc );
    physdev = GET_DC_PHYSDEV( dc, pExtSelectClipRgn );
    retval = physdev->funcs->pExtSelectClipRgn( physdev, hrgn, fnMode );
    release_dc_ptr( dc );
    return retval;
}

/***********************************************************************
 *           __wine_set_visible_region   (GDI32.@)
 */
void CDECL __wine_set_visible_region( HDC hdc, HRGN hrgn, const RECT *vis_rect, const RECT *device_rect,
                                      struct window_surface *surface )
{
    DC * dc;

    if (!(dc = get_dc_ptr( hdc ))) return;

    //TRACE( "%p %p %s %s %p\n", hdc, hrgn, wine_dbgstr_rect(vis_rect), wine_dbgstr_rect(device_rect), surface );

    /* map region to DC coordinates */
    OffsetRgn_k( hrgn, -vis_rect->left, -vis_rect->top );

    if (dc->hVisRgn) DeleteObject_k( dc->hVisRgn );
    dc->dirty = 0;
    dc->vis_rect = *vis_rect;
    dc->device_rect = *device_rect;
    dc->hVisRgn = hrgn;
    dibdrv_set_window_surface( dc, surface );
    DC_UpdateXforms( dc );
    update_dc_clipping( dc );
    release_dc_ptr( dc );
}


/***********************************************************************
 *           OffsetClipRgn    (GDI32.@)
 */
INT WINAPI OffsetClipRgn_k( HDC hdc, INT x, INT y )
{
    PHYSDEV physdev;
    INT ret;
    DC *dc = get_dc_ptr( hdc );

    TRACE("%p %d,%d\n", hdc, x, y );

    if (!dc) return ERROR;
    update_dc( dc );
    physdev = GET_DC_PHYSDEV( dc, pOffsetClipRgn );
    ret = physdev->funcs->pOffsetClipRgn( physdev, x, y );
    release_dc_ptr( dc );
    return ret;
}


/***********************************************************************
 *           ExcludeClipRect    (GDI32.@)
 */
INT WINAPI ExcludeClipRect_k( HDC hdc, INT left, INT top,
                                INT right, INT bottom )
{
    PHYSDEV physdev;
    INT ret;
    DC *dc = get_dc_ptr( hdc );

    TRACE("%p %d,%d-%d,%d\n", hdc, left, top, right, bottom );

    if (!dc) return ERROR;
    update_dc( dc );
    physdev = GET_DC_PHYSDEV( dc, pExcludeClipRect );
    ret = physdev->funcs->pExcludeClipRect( physdev, left, top, right, bottom );
    release_dc_ptr( dc );
    return ret;
}


/***********************************************************************
 *           IntersectClipRect    (GDI32.@)
 */
INT WINAPI IntersectClipRect_k( HDC hdc, INT left, INT top, INT right, INT bottom )
{
    PHYSDEV physdev;
    INT ret;
    DC *dc = get_dc_ptr( hdc );

    TRACE("%p %d,%d - %d,%d\n", hdc, left, top, right, bottom );

    if (!dc) return ERROR;
    update_dc( dc );
    physdev = GET_DC_PHYSDEV( dc, pIntersectClipRect );
    ret = physdev->funcs->pIntersectClipRect( physdev, left, top, right, bottom );
    release_dc_ptr( dc );
    return ret;
}


/***********************************************************************
 *           PtVisible    (GDI32.@)
 */
BOOL WINAPI PtVisible_k( HDC hdc, INT x, INT y )
{
    POINT pt;
    RECT visrect;
    BOOL ret;
    DC *dc = get_dc_ptr( hdc );

    TRACE("%p %d,%d\n", hdc, x, y );
    if (!dc) return FALSE;

    pt.x = x;
    pt.y = y;
    LPtoDP_k( hdc, &pt, 1 );
    update_dc( dc );
    ret = (!get_dc_device_rect( dc, &visrect ) ||
           (pt.x >= visrect.left && pt.x < visrect.right &&
            pt.y >= visrect.top && pt.y < visrect.bottom));
    if (ret && get_dc_region( dc )) ret = PtInRegion_k( get_dc_region( dc ), pt.x, pt.y );
    release_dc_ptr( dc );
    return ret;
}


/***********************************************************************
 *           RectVisible    (GDI32.@)
 */
BOOL WINAPI RectVisible_k( HDC hdc, const RECT* rect )
{
    RECT tmpRect, visrect;
    BOOL ret;
    DC *dc = get_dc_ptr( hdc );
    if (!dc) return FALSE;
    //TRACE("%p %s\n", hdc, wine_dbgstr_rect( rect ));

    tmpRect = *rect;
    LPtoDP_k( hdc, (POINT *)&tmpRect, 2 );
    order_rect( &tmpRect );

    update_dc( dc );
    ret = (!get_dc_device_rect( dc, &visrect ) || intersect_rect( &visrect, &visrect, &tmpRect ));
    if (ret && get_dc_region( dc )) ret = RectInRegion_k( get_dc_region( dc ), &tmpRect );
    release_dc_ptr( dc );
    return ret;
}


/***********************************************************************
 *           GetClipBox    (GDI32.@)
 */
INT WINAPI GetClipBox_k( HDC hdc, LPRECT rect )
{
    RECT visrect;
    INT ret;
    DC *dc = get_dc_ptr( hdc );
    if (!dc) return ERROR;

    update_dc( dc );
    if (get_dc_region( dc ))
    {
        ret = GetRgnBox_k( get_dc_region( dc ), rect );
    }
    else
    {
        ret = is_rect_empty( &dc->vis_rect ) ? ERROR : SIMPLEREGION;
        *rect = dc->vis_rect;
    }

    if (get_dc_device_rect( dc, &visrect ) && !intersect_rect( rect, rect, &visrect )) ret = NULLREGION;

    if (dc->layout & LAYOUT_RTL)
    {
        int tmp = rect->left;
        rect->left = rect->right - 1;
        rect->right = tmp - 1;
    }
    DPtoLP_k( hdc, (LPPOINT)rect, 2 );
    release_dc_ptr( dc );
    //TRACE("%p => %d %s\n", hdc, ret, wine_dbgstr_rect( rect ));
    return ret;
}


/***********************************************************************
 *           GetClipRgn  (GDI32.@)
 */
INT WINAPI GetClipRgn_k( HDC hdc, HRGN hRgn )
{
    INT ret = -1;
    DC * dc;
    if ((dc = get_dc_ptr( hdc )))
    {
      if( dc->hClipRgn )
      {
          if( CombineRgn_k(hRgn, dc->hClipRgn, 0, RGN_COPY) != ERROR )
          {
              ret = 1;
              if (dc->layout & LAYOUT_RTL)
                  mirror_region( hRgn, hRgn, dc->vis_rect.right - dc->vis_rect.left );
          }
      }
      else ret = 0;
      release_dc_ptr( dc );
    }
    return ret;
}


/***********************************************************************
 *           GetMetaRgn    (GDI32.@)
 */
INT WINAPI GetMetaRgn_k( HDC hdc, HRGN hRgn )
{
    INT ret = 0;
    DC * dc = get_dc_ptr( hdc );

    if (dc)
    {
        if (dc->hMetaRgn && CombineRgn_k( hRgn, dc->hMetaRgn, 0, RGN_COPY ) != ERROR)
        {
            ret = 1;
            if (dc->layout & LAYOUT_RTL)
                mirror_region( hRgn, hRgn, dc->vis_rect.right - dc->vis_rect.left );
        }
        release_dc_ptr( dc );
    }
    return ret;
}


/***********************************************************************
 * GetRandomRgn [GDI32.@]
 *
 * NOTES
 *     This function is documented in MSDN online for the case of
 *     iCode == SYSRGN (4).
 *
 *     For iCode == 1 it should return the clip region
 *                  2 "    "       "   the meta region
 *                  3 "    "       "   the intersection of the clip with
 *                                     the meta region (== 'Rao' region).
 *
 *     See http://www.codeproject.com/gdi/cliprgnguide.asp
 */
INT WINAPI GetRandomRgn_k(HDC hDC, HRGN hRgn, INT iCode)
{
    INT ret = 1;
    DC *dc = get_dc_ptr( hDC );

    if (!dc) return -1;

    switch (iCode)
    {
    case 1:
        if (dc->hClipRgn) CombineRgn_k( hRgn, dc->hClipRgn, 0, RGN_COPY );
        else ret = 0;
        break;
    case 2:
        if (dc->hMetaRgn) CombineRgn_k( hRgn, dc->hMetaRgn, 0, RGN_COPY );
        else ret = 0;
        break;
    case 3:
        if (dc->hClipRgn && dc->hMetaRgn) CombineRgn_k( hRgn, dc->hClipRgn, dc->hMetaRgn, RGN_AND );
        else if (dc->hClipRgn) CombineRgn_k( hRgn, dc->hClipRgn, 0, RGN_COPY );
        else if (dc->hMetaRgn) CombineRgn_k( hRgn, dc->hMetaRgn, 0, RGN_COPY );
        else ret = 0;
        break;
    case SYSRGN: /* == 4 */
        update_dc( dc );
        if (dc->hVisRgn)
        {
            CombineRgn_k( hRgn, dc->hVisRgn, 0, RGN_COPY );
            /* On Windows NT/2000, the SYSRGN returned is in screen coordinates */
            if (!(GetVersion_k() & 0x80000000)) OffsetRgn_k( hRgn, dc->vis_rect.left, dc->vis_rect.top );
        }
        else if (!is_rect_empty( &dc->device_rect ))
            SetRectRgn_k( hRgn, dc->device_rect.left, dc->device_rect.top,
                        dc->device_rect.right, dc->device_rect.bottom );
        else
            ret = 0;
        break;
    default:
        WARN("Unknown code %d\n", iCode);
        ret = -1;
        break;
    }
    release_dc_ptr( dc );
    return ret;
}


/***********************************************************************
 *           SetMetaRgn    (GDI32.@)
 */
INT WINAPI SetMetaRgn_k( HDC hdc )
{
    INT ret;
    RECT dummy;
    DC *dc = get_dc_ptr( hdc );

    if (!dc) return ERROR;

    if (dc->hClipRgn)
    {
        if (dc->hMetaRgn)
        {
            /* the intersection becomes the new meta region */
            CombineRgn_k( dc->hMetaRgn, dc->hMetaRgn, dc->hClipRgn, RGN_AND );
            DeleteObject_k( dc->hClipRgn );
            dc->hClipRgn = 0;
        }
        else
        {
            dc->hMetaRgn = dc->hClipRgn;
            dc->hClipRgn = 0;
        }
    }
    /* else nothing to do */

    /* Note: no need to call update_dc_clipping, the overall clip region hasn't changed */

    ret = GetRgnBox_k( dc->hMetaRgn, &dummy );
    release_dc_ptr( dc );
    return ret;
}

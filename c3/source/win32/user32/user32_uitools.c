/*
 * User Interface Functions
 *
 * Copyright 1997 Dimitrie O. Paun
 * Copyright 1997 Bertho A. Stultiens
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

#include "../include/winbase.h"
#include "user_private.h"
#include "controls.h"
#include "../include/unicode.h"

/* These tables are used in:
 * UITOOLS_DrawDiagEdge()
 * UITOOLS_DrawRectEdge()
 */
static const signed char LTInnerNormal[] = {
    -1,           -1,                 -1,                 -1,
    -1,           COLOR_BTNHIGHLIGHT, COLOR_BTNHIGHLIGHT, -1,
    -1,           COLOR_3DDKSHADOW,   COLOR_3DDKSHADOW,   -1,
    -1,           -1,                 -1,                 -1
};

static const signed char LTOuterNormal[] = {
    -1,                 COLOR_3DLIGHT,     COLOR_BTNSHADOW, -1,
    COLOR_BTNHIGHLIGHT, COLOR_3DLIGHT,     COLOR_BTNSHADOW, -1,
    COLOR_3DDKSHADOW,   COLOR_3DLIGHT,     COLOR_BTNSHADOW, -1,
    -1,                 COLOR_3DLIGHT,     COLOR_BTNSHADOW, -1
};

static const signed char RBInnerNormal[] = {
    -1,           -1,                -1,              -1,
    -1,           COLOR_BTNSHADOW,   COLOR_BTNSHADOW, -1,
    -1,           COLOR_3DLIGHT,     COLOR_3DLIGHT,   -1,
    -1,           -1,                -1,              -1
};

static const signed char RBOuterNormal[] = {
    -1,              COLOR_3DDKSHADOW,  COLOR_BTNHIGHLIGHT, -1,
    COLOR_BTNSHADOW, COLOR_3DDKSHADOW,  COLOR_BTNHIGHLIGHT, -1,
    COLOR_3DLIGHT,   COLOR_3DDKSHADOW,  COLOR_BTNHIGHLIGHT, -1,
    -1,              COLOR_3DDKSHADOW,  COLOR_BTNHIGHLIGHT, -1
};

static const signed char LTInnerSoft[] = {
    -1,                  -1,                -1,              -1,
    -1,                  COLOR_3DLIGHT,     COLOR_3DLIGHT,   -1,
    -1,                  COLOR_BTNSHADOW,   COLOR_BTNSHADOW, -1,
    -1,                  -1,                -1,              -1
};

static const signed char LTOuterSoft[] = {
    -1,              COLOR_BTNHIGHLIGHT, COLOR_3DDKSHADOW, -1,
    COLOR_3DLIGHT,   COLOR_BTNHIGHLIGHT, COLOR_3DDKSHADOW, -1,
    COLOR_BTNSHADOW, COLOR_BTNHIGHLIGHT, COLOR_3DDKSHADOW, -1,
    -1,              COLOR_BTNHIGHLIGHT, COLOR_3DDKSHADOW, -1
};

#define RBInnerSoft RBInnerNormal   /* These are the same */
#define RBOuterSoft RBOuterNormal

static const signed char LTRBOuterMono[] = {
    -1,           COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
    COLOR_WINDOW, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME, COLOR_WINDOWFRAME,
};

static const signed char LTRBInnerMono[] = {
    -1, -1,           -1,           -1,
    -1, COLOR_WINDOW, COLOR_WINDOW, COLOR_WINDOW,
    -1, COLOR_WINDOW, COLOR_WINDOW, COLOR_WINDOW,
    -1, COLOR_WINDOW, COLOR_WINDOW, COLOR_WINDOW,
};

static const signed char LTRBOuterFlat[] = {
    -1,                COLOR_BTNSHADOW, COLOR_BTNSHADOW, COLOR_BTNSHADOW,
    COLOR_BTNFACE,     COLOR_BTNSHADOW, COLOR_BTNSHADOW, COLOR_BTNSHADOW,
    COLOR_BTNFACE,     COLOR_BTNSHADOW, COLOR_BTNSHADOW, COLOR_BTNSHADOW,
    COLOR_BTNFACE,     COLOR_BTNSHADOW, COLOR_BTNSHADOW, COLOR_BTNSHADOW,
};

static const signed char LTRBInnerFlat[] = {
    -1, -1,              -1,              -1,
    -1, COLOR_BTNFACE,     COLOR_BTNFACE,     COLOR_BTNFACE,
    -1, COLOR_BTNFACE,     COLOR_BTNFACE,     COLOR_BTNFACE,
    -1, COLOR_BTNFACE,     COLOR_BTNFACE,     COLOR_BTNFACE,
};

/* last COLOR id */
#define COLOR_MAX   COLOR_MENUBAR


/***********************************************************************
 *           set_control_clipping
 *
 * Set clipping for a builtin control that uses CS_PARENTDC.
 * Return the previous clip region if any.
 */
HRGN set_control_clipping( HDC hdc, const RECT *rect )
{
    RECT rc = *rect;
    HRGN hrgn = CreateRectRgn_k( 0, 0, 0, 0 );

    if (GetClipRgn_k( hdc, hrgn ) != 1)
    {
        DeleteObject_k( hrgn );
        hrgn = 0;
    }
    DPtoLP_k( hdc, (POINT *)&rc, 2 );
    if (GetLayout_k( hdc ) & LAYOUT_RTL)  /* compensate for the shifting done by IntersectClipRect */
    {
        rc.left++;
        rc.right++;
    }
    IntersectClipRect_k( hdc, rc.left, rc.top, rc.right, rc.bottom );
    return hrgn;
}

/***********************************************************************
 *           UITOOLS_DrawDiagEdge
 *
 * Same as DrawEdge invoked with BF_DIAGONAL
 *
 * 03-Dec-1997: Changed by Bertho Stultiens
 *
 * See also comments with UITOOLS_DrawRectEdge()
 */
static BOOL UITOOLS95_DrawDiagEdge(HDC hdc, LPRECT rc,
				     UINT uType, UINT uFlags)
{
    POINT Points[4];
    signed char InnerI, OuterI;
    HPEN InnerPen, OuterPen;
    POINT SavePoint;
    HPEN SavePen;
    int spx, spy;
    int epx, epy;
    int Width = rc->right - rc->left;
    int Height= rc->bottom - rc->top;
    int SmallDiam = Width > Height ? Height : Width;
    BOOL retval = !(   ((uType & BDR_INNER) == BDR_INNER
                       || (uType & BDR_OUTER) == BDR_OUTER)
                      && !(uFlags & (BF_FLAT|BF_MONO)) );
    int add = (LTRBInnerMono[uType & (BDR_INNER|BDR_OUTER)] != -1 ? 1 : 0)
            + (LTRBOuterMono[uType & (BDR_INNER|BDR_OUTER)] != -1 ? 1 : 0);

    /* Init some vars */
    OuterPen = InnerPen = GetStockObject_k(NULL_PEN);
    SavePen = SelectObject_k(hdc, InnerPen);
    spx = spy = epx = epy = 0; /* Satisfy the compiler... */

    /* Determine the colors of the edges */
    if(uFlags & BF_MONO)
    {
        InnerI = LTRBInnerMono[uType & (BDR_INNER|BDR_OUTER)];
        OuterI = LTRBOuterMono[uType & (BDR_INNER|BDR_OUTER)];
    }
    else if(uFlags & BF_FLAT)
    {
        InnerI = LTRBInnerFlat[uType & (BDR_INNER|BDR_OUTER)];
        OuterI = LTRBOuterFlat[uType & (BDR_INNER|BDR_OUTER)];
    }
    else if(uFlags & BF_SOFT)
    {
        if(uFlags & BF_BOTTOM)
        {
            InnerI = RBInnerSoft[uType & (BDR_INNER|BDR_OUTER)];
            OuterI = RBOuterSoft[uType & (BDR_INNER|BDR_OUTER)];
        }
        else
        {
            InnerI = LTInnerSoft[uType & (BDR_INNER|BDR_OUTER)];
            OuterI = LTOuterSoft[uType & (BDR_INNER|BDR_OUTER)];
        }
    }
    else
    {
        if(uFlags & BF_BOTTOM)
        {
            InnerI = RBInnerNormal[uType & (BDR_INNER|BDR_OUTER)];
            OuterI = RBOuterNormal[uType & (BDR_INNER|BDR_OUTER)];
        }
        else
        {
            InnerI = LTInnerNormal[uType & (BDR_INNER|BDR_OUTER)];
            OuterI = LTOuterNormal[uType & (BDR_INNER|BDR_OUTER)];
        }
    }

    if(InnerI != -1) InnerPen = SYSCOLOR_GetPen(InnerI);
    if(OuterI != -1) OuterPen = SYSCOLOR_GetPen(OuterI);

    MoveToEx_k(hdc, 0, 0, &SavePoint);

    /* Don't ask me why, but this is what is visible... */
    /* This must be possible to do much simpler, but I fail to */
    /* see the logic in the MS implementation (sigh...). */
    /* So, this might look a bit brute force here (and it is), but */
    /* it gets the job done;) */

    switch(uFlags & BF_RECT)
    {
    case 0:
    case BF_LEFT:
    case BF_BOTTOM:
    case BF_BOTTOMLEFT:
        /* Left bottom endpoint */
        epx = rc->left-1;
        spx = epx + SmallDiam;
        epy = rc->bottom;
        spy = epy - SmallDiam;
        break;

    case BF_TOPLEFT:
    case BF_BOTTOMRIGHT:
        /* Left top endpoint */
        epx = rc->left-1;
        spx = epx + SmallDiam;
        epy = rc->top-1;
        spy = epy + SmallDiam;
        break;

    case BF_TOP:
    case BF_RIGHT:
    case BF_TOPRIGHT:
    case BF_RIGHT|BF_LEFT:
    case BF_RIGHT|BF_LEFT|BF_TOP:
    case BF_BOTTOM|BF_TOP:
    case BF_BOTTOM|BF_TOP|BF_LEFT:
    case BF_BOTTOMRIGHT|BF_LEFT:
    case BF_BOTTOMRIGHT|BF_TOP:
    case BF_RECT:
        /* Right top endpoint */
        spx = rc->left;
        epx = spx + SmallDiam;
        spy = rc->bottom-1;
        epy = spy - SmallDiam;
        break;
    }

    MoveToEx_k(hdc, spx, spy, NULL);
    SelectObject_k(hdc, OuterPen);
    LineTo_k(hdc, epx, epy);

    SelectObject_k(hdc, InnerPen);

    switch(uFlags & (BF_RECT|BF_DIAGONAL))
    {
    case BF_DIAGONAL_ENDBOTTOMLEFT:
    case (BF_DIAGONAL|BF_BOTTOM):
    case BF_DIAGONAL:
    case (BF_DIAGONAL|BF_LEFT):
        MoveToEx_k(hdc, spx-1, spy, NULL);
        LineTo_k(hdc, epx, epy-1);
        Points[0].x = spx-add;
        Points[0].y = spy;
        Points[1].x = rc->left;
        Points[1].y = rc->top;
        Points[2].x = epx+1;
        Points[2].y = epy-1-add;
        Points[3] = Points[2];
        break;

    case BF_DIAGONAL_ENDBOTTOMRIGHT:
        MoveToEx_k(hdc, spx-1, spy, NULL);
        LineTo_k(hdc, epx, epy+1);
        Points[0].x = spx-add;
        Points[0].y = spy;
        Points[1].x = rc->left;
        Points[1].y = rc->bottom-1;
        Points[2].x = epx+1;
        Points[2].y = epy+1+add;
        Points[3] = Points[2];
        break;

    case (BF_DIAGONAL|BF_BOTTOM|BF_RIGHT|BF_TOP):
    case (BF_DIAGONAL|BF_BOTTOM|BF_RIGHT|BF_TOP|BF_LEFT):
    case BF_DIAGONAL_ENDTOPRIGHT:
    case (BF_DIAGONAL|BF_RIGHT|BF_TOP|BF_LEFT):
        MoveToEx_k(hdc, spx+1, spy, NULL);
        LineTo_k(hdc, epx, epy+1);
        Points[0].x = epx-1;
        Points[0].y = epy+1+add;
        Points[1].x = rc->right-1;
        Points[1].y = rc->top+add;
        Points[2].x = rc->right-1;
        Points[2].y = rc->bottom-1;
        Points[3].x = spx+add;
        Points[3].y = spy;
        break;

    case BF_DIAGONAL_ENDTOPLEFT:
        MoveToEx_k(hdc, spx, spy-1, NULL);
        LineTo_k(hdc, epx+1, epy);
        Points[0].x = epx+1+add;
        Points[0].y = epy+1;
        Points[1].x = rc->right-1;
        Points[1].y = rc->top;
        Points[2].x = rc->right-1;
        Points[2].y = rc->bottom-1-add;
        Points[3].x = spx;
        Points[3].y = spy-add;
        break;

    case (BF_DIAGONAL|BF_TOP):
    case (BF_DIAGONAL|BF_BOTTOM|BF_TOP):
    case (BF_DIAGONAL|BF_BOTTOM|BF_TOP|BF_LEFT):
        MoveToEx_k(hdc, spx+1, spy-1, NULL);
        LineTo_k(hdc, epx, epy);
        Points[0].x = epx-1;
        Points[0].y = epy+1;
        Points[1].x = rc->right-1;
        Points[1].y = rc->top;
        Points[2].x = rc->right-1;
        Points[2].y = rc->bottom-1-add;
        Points[3].x = spx+add;
        Points[3].y = spy-add;
        break;

    case (BF_DIAGONAL|BF_RIGHT):
    case (BF_DIAGONAL|BF_RIGHT|BF_LEFT):
    case (BF_DIAGONAL|BF_RIGHT|BF_LEFT|BF_BOTTOM):
        MoveToEx_k(hdc, spx, spy, NULL);
        LineTo_k(hdc, epx-1, epy+1);
        Points[0].x = spx;
        Points[0].y = spy;
        Points[1].x = rc->left;
        Points[1].y = rc->top+add;
        Points[2].x = epx-1-add;
        Points[2].y = epy+1+add;
        Points[3] = Points[2];
        break;
    }

    /* Fill the interior if asked */
    if((uFlags & BF_MIDDLE) && retval)
    {
        HBRUSH hbsave;
        HBRUSH hb = GetSysColorBrush_k(uFlags & BF_MONO ? COLOR_WINDOW
					 :COLOR_BTNFACE);
        HPEN hpsave;
        HPEN hp = SYSCOLOR_GetPen(uFlags & BF_MONO ? COLOR_WINDOW
				     : COLOR_BTNFACE);
        hbsave = SelectObject_k(hdc, hb);
        hpsave = SelectObject_k(hdc, hp);
        Polygon_k(hdc, Points, 4);
        SelectObject_k(hdc, hbsave);
        SelectObject_k(hdc, hpsave);
    }

    /* Adjust rectangle if asked */
    if(uFlags & BF_ADJUST)
    {
        if(uFlags & BF_LEFT)   rc->left   += add;
        if(uFlags & BF_RIGHT)  rc->right  -= add;
        if(uFlags & BF_TOP)    rc->top    += add;
        if(uFlags & BF_BOTTOM) rc->bottom -= add;
    }

    /* Cleanup */
    SelectObject_k(hdc, SavePen);
    MoveToEx_k(hdc, SavePoint.x, SavePoint.y, NULL);

    return retval;
}

/***********************************************************************
 *           UITOOLS_DrawRectEdge
 *
 * Same as DrawEdge invoked without BF_DIAGONAL
 *
 * 23-Nov-1997: Changed by Bertho Stultiens
 *
 * Well, I started testing this and found out that there are a few things
 * that weren't quite as win95. The following rewrite should reproduce
 * win95 results completely.
 * The colorselection is table-driven to avoid awful if-statements.
 * The table below show the color settings.
 *
 * Pen selection table for uFlags = 0
 *
 * uType |  LTI  |  LTO  |  RBI  |  RBO
 * ------+-------+-------+-------+-------
 *  0000 |   x   |   x   |   x   |   x
 *  0001 |   x   |  22   |   x   |  21
 *  0010 |   x   |  16   |   x   |  20
 *  0011 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  0100 |   x   |  20   |   x   |  16
 *  0101 |  20   |  22   |  16   |  21
 *  0110 |  20   |  16   |  16   |  20
 *  0111 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  1000 |   x   |  21   |   x   |  22
 *  1001 |  21   |  22   |  22   |  21
 *  1010 |  21   |  16   |  22   |  20
 *  1011 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  1100 |   x   |   x   |   x   |   x
 *  1101 |   x   | x (22)|   x   | x (21)
 *  1110 |   x   | x (16)|   x   | x (20)
 *  1111 |   x   |   x   |   x   |   x
 *
 * Pen selection table for uFlags = BF_SOFT
 *
 * uType |  LTI  |  LTO  |  RBI  |  RBO
 * ------+-------+-------+-------+-------
 *  0000 |   x   |   x   |   x   |   x
 *  0001 |   x   |  20   |   x   |  21
 *  0010 |   x   |  21   |   x   |  20
 *  0011 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  0100 |   x   |  22   |   x   |  16
 *  0101 |  22   |  20   |  16   |  21
 *  0110 |  22   |  21   |  16   |  20
 *  0111 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  1000 |   x   |  16   |   x   |  22
 *  1001 |  16   |  20   |  22   |  21
 *  1010 |  16   |  21   |  22   |  20
 *  1011 |   x   |   x   |   x   |   x
 * ------+-------+-------+-------+-------
 *  1100 |   x   |   x   |   x   |   x
 *  1101 |   x   | x (20)|   x   | x (21)
 *  1110 |   x   | x (21)|   x   | x (20)
 *  1111 |   x   |   x   |   x   |   x
 *
 * x = don't care; (n) = is what win95 actually uses
 * LTI = left Top Inner line
 * LTO = left Top Outer line
 * RBI = Right Bottom Inner line
 * RBO = Right Bottom Outer line
 * 15 = COLOR_BTNFACE
 * 16 = COLOR_BTNSHADOW
 * 20 = COLOR_BTNHIGHLIGHT
 * 21 = COLOR_3DDKSHADOW
 * 22 = COLOR_3DLIGHT
 */


static BOOL UITOOLS95_DrawRectEdge(HDC hdc, LPRECT rc,
				     UINT uType, UINT uFlags)
{
    signed char LTInnerI, LTOuterI;
    signed char RBInnerI, RBOuterI;
    HPEN LTInnerPen, LTOuterPen;
    HPEN RBInnerPen, RBOuterPen;
    RECT InnerRect = *rc;
    POINT SavePoint;
    HPEN SavePen;
    int LBpenplus = 0;
    int LTpenplus = 0;
    int RTpenplus = 0;
    int RBpenplus = 0;
    BOOL retval = !(   ((uType & BDR_INNER) == BDR_INNER
                       || (uType & BDR_OUTER) == BDR_OUTER)
                      && !(uFlags & (BF_FLAT|BF_MONO)) );

    /* Init some vars */
    LTInnerPen = LTOuterPen = RBInnerPen = RBOuterPen = GetStockObject_k(NULL_PEN);
    SavePen = SelectObject_k(hdc, LTInnerPen);

    /* Determine the colors of the edges */
    if(uFlags & BF_MONO)
    {
        LTInnerI = RBInnerI = LTRBInnerMono[uType & (BDR_INNER|BDR_OUTER)];
        LTOuterI = RBOuterI = LTRBOuterMono[uType & (BDR_INNER|BDR_OUTER)];
    }
    else if(uFlags & BF_FLAT)
    {
        LTInnerI = RBInnerI = LTRBInnerFlat[uType & (BDR_INNER|BDR_OUTER)];
        LTOuterI = RBOuterI = LTRBOuterFlat[uType & (BDR_INNER|BDR_OUTER)];

	if( LTInnerI != -1 ) LTInnerI = RBInnerI = COLOR_BTNFACE;
    }
    else if(uFlags & BF_SOFT)
    {
        LTInnerI = LTInnerSoft[uType & (BDR_INNER|BDR_OUTER)];
        LTOuterI = LTOuterSoft[uType & (BDR_INNER|BDR_OUTER)];
        RBInnerI = RBInnerSoft[uType & (BDR_INNER|BDR_OUTER)];
        RBOuterI = RBOuterSoft[uType & (BDR_INNER|BDR_OUTER)];
    }
    else
    {
        LTInnerI = LTInnerNormal[uType & (BDR_INNER|BDR_OUTER)];
        LTOuterI = LTOuterNormal[uType & (BDR_INNER|BDR_OUTER)];
        RBInnerI = RBInnerNormal[uType & (BDR_INNER|BDR_OUTER)];
        RBOuterI = RBOuterNormal[uType & (BDR_INNER|BDR_OUTER)];
    }

    if((uFlags & BF_BOTTOMLEFT) == BF_BOTTOMLEFT)   LBpenplus = 1;
    if((uFlags & BF_TOPRIGHT) == BF_TOPRIGHT)       RTpenplus = 1;
    if((uFlags & BF_BOTTOMRIGHT) == BF_BOTTOMRIGHT) RBpenplus = 1;
    if((uFlags & BF_TOPLEFT) == BF_TOPLEFT)         LTpenplus = 1;

    if(LTInnerI != -1) LTInnerPen = SYSCOLOR_GetPen(LTInnerI);
    if(LTOuterI != -1) LTOuterPen = SYSCOLOR_GetPen(LTOuterI);
    if(RBInnerI != -1) RBInnerPen = SYSCOLOR_GetPen(RBInnerI);
    if(RBOuterI != -1) RBOuterPen = SYSCOLOR_GetPen(RBOuterI);

    MoveToEx_k(hdc, 0, 0, &SavePoint);

    /* Draw the outer edge */
    SelectObject_k(hdc, LTOuterPen);
    if(uFlags & BF_TOP)
    {
        MoveToEx_k(hdc, InnerRect.left, InnerRect.top, NULL);
        LineTo_k(hdc, InnerRect.right, InnerRect.top);
    }
    if(uFlags & BF_LEFT)
    {
        MoveToEx_k(hdc, InnerRect.left, InnerRect.top, NULL);
        LineTo_k(hdc, InnerRect.left, InnerRect.bottom);
    }
    SelectObject_k(hdc, RBOuterPen);
    if(uFlags & BF_BOTTOM)
    {
        MoveToEx_k(hdc, InnerRect.left, InnerRect.bottom-1, NULL);
        LineTo_k(hdc, InnerRect.right, InnerRect.bottom-1);
    }
    if(uFlags & BF_RIGHT)
    {
        MoveToEx_k(hdc, InnerRect.right-1, InnerRect.top, NULL);
        LineTo_k(hdc, InnerRect.right-1, InnerRect.bottom);
    }

    /* Draw the inner edge */
    SelectObject_k(hdc, LTInnerPen);
    if(uFlags & BF_TOP)
    {
        MoveToEx_k(hdc, InnerRect.left+LTpenplus, InnerRect.top+1, NULL);
        LineTo_k(hdc, InnerRect.right-RTpenplus, InnerRect.top+1);
    }
    if(uFlags & BF_LEFT)
    {
        MoveToEx_k(hdc, InnerRect.left+1, InnerRect.top+LTpenplus, NULL);
        LineTo_k(hdc, InnerRect.left+1, InnerRect.bottom-LBpenplus);
    }
    SelectObject_k(hdc, RBInnerPen);
    if(uFlags & BF_BOTTOM)
    {
        MoveToEx_k(hdc, InnerRect.left+LBpenplus, InnerRect.bottom-2, NULL);
        LineTo_k(hdc, InnerRect.right-RBpenplus, InnerRect.bottom-2);
    }
    if(uFlags & BF_RIGHT)
    {
        MoveToEx_k(hdc, InnerRect.right-2, InnerRect.top+RTpenplus, NULL);
        LineTo_k(hdc, InnerRect.right-2, InnerRect.bottom-RBpenplus);
    }

    if( ((uFlags & BF_MIDDLE) && retval) || (uFlags & BF_ADJUST) )
    {
        int add = (LTRBInnerMono[uType & (BDR_INNER|BDR_OUTER)] != -1 ? 1 : 0)
                + (LTRBOuterMono[uType & (BDR_INNER|BDR_OUTER)] != -1 ? 1 : 0);

        if(uFlags & BF_LEFT)   InnerRect.left   += add;
        if(uFlags & BF_RIGHT)  InnerRect.right  -= add;
        if(uFlags & BF_TOP)    InnerRect.top    += add;
        if(uFlags & BF_BOTTOM) InnerRect.bottom -= add;

        if((uFlags & BF_MIDDLE) && retval)
	{
            FillRect_k(hdc, &InnerRect, GetSysColorBrush_k(uFlags & BF_MONO ?
						       COLOR_WINDOW : COLOR_BTNFACE));
	}

	if(uFlags & BF_ADJUST)
	    *rc = InnerRect;
    }

    /* Cleanup */
    SelectObject_k(hdc, SavePen);
    MoveToEx_k(hdc, SavePoint.x, SavePoint.y, NULL);
    return retval;
}


/**********************************************************************
 *          DrawEdge   (USER32.@)
 */
BOOL WINAPI DrawEdge_k( HDC hdc, LPRECT rc, UINT edge, UINT flags )
{
    //TRACE("%p %s %04x %04x\n", hdc, wine_dbgstr_rect(rc), edge, flags );

    if(flags & BF_DIAGONAL)
      return UITOOLS95_DrawDiagEdge(hdc, rc, edge, flags);
    else
      return UITOOLS95_DrawRectEdge(hdc, rc, edge, flags);
}


/************************************************************************
 *	UITOOLS_MakeSquareRect
 *
 * Utility to create a square rectangle and returning the width
 */
static int UITOOLS_MakeSquareRect(LPRECT src, LPRECT dst)
{
    int Width  = src->right - src->left;
    int Height = src->bottom - src->top;
    int SmallDiam = Width > Height ? Height : Width;

    *dst = *src;

    /* Make it a square box */
    if(Width < Height)      /* SmallDiam == Width */
    {
        dst->top += (Height-Width)/2;
        dst->bottom = dst->top + SmallDiam;
    }
    else if(Width > Height) /* SmallDiam == Height */
    {
        dst->left += (Width-Height)/2;
        dst->right = dst->left + SmallDiam;
    }

   return SmallDiam;
}

static void UITOOLS_DrawCheckedRect( HDC dc, LPRECT rect )
{
    if(GetSysColor_k(COLOR_BTNHIGHLIGHT) == RGB(255, 255, 255))
    {
      HBRUSH hbsave;
      COLORREF bg;

      FillRect_k(dc, rect, GetSysColorBrush_k(COLOR_BTNFACE));
      bg = SetBkColor_k(dc, RGB(255, 255, 255));
      hbsave = SelectObject_k(dc, getSYSCOLOR_55AABrush());
      PatBlt_k(dc, rect->left, rect->top, rect->right-rect->left, rect->bottom-rect->top, 0x00FA0089);
      SelectObject_k(dc, hbsave);
      SetBkColor_k(dc, bg);
    }
    else
    {
        FillRect_k(dc, rect, GetSysColorBrush_k(COLOR_BTNHIGHLIGHT));
    }
}

/************************************************************************
 *	UITOOLS_DFC_ButtonPush
 *
 * Draw a push button coming from DrawFrameControl()
 *
 * Does a pretty good job in emulating MS behavior. Some quirks are
 * however there because MS uses a TrueType font (Marlett) to draw
 * the buttons.
 */
static BOOL UITOOLS95_DFC_ButtonPush(HDC dc, LPRECT r, UINT uFlags)
{
    UINT edge;
    RECT myr = *r;

    if(uFlags & (DFCS_PUSHED | DFCS_CHECKED | DFCS_FLAT))
        edge = EDGE_SUNKEN;
    else
        edge = EDGE_RAISED;

    if(uFlags & DFCS_CHECKED)
    {
        if(uFlags & DFCS_MONO)
            UITOOLS95_DrawRectEdge(dc, &myr, edge, BF_MONO|BF_RECT|BF_ADJUST);
        else
            UITOOLS95_DrawRectEdge(dc, &myr, edge, (uFlags&DFCS_FLAT)|BF_RECT|BF_SOFT|BF_ADJUST);

	if (!(uFlags & DFCS_TRANSPARENT))
	    UITOOLS_DrawCheckedRect( dc, &myr );
    }
    else
    {
        if(uFlags & DFCS_MONO)
        {
            UITOOLS95_DrawRectEdge(dc, &myr, edge, BF_MONO|BF_RECT|BF_ADJUST);
	    if (!(uFlags & DFCS_TRANSPARENT))
                FillRect_k(dc, &myr, GetSysColorBrush_k(COLOR_BTNFACE));
        }
        else
        {
            UITOOLS95_DrawRectEdge(dc, r, edge, (uFlags & DFCS_FLAT) | ((uFlags & DFCS_TRANSPARENT) ? 0 : BF_MIDDLE) | BF_RECT | BF_SOFT);
        }
    }

    /* Adjust rectangle if asked */
    if(uFlags & DFCS_ADJUSTRECT)
    {
        r->left   += 2;
        r->right  -= 2;
        r->top    += 2;
        r->bottom -= 2;
    }

    return TRUE;
}


/************************************************************************
 *	UITOOLS_DFC_ButtonChcek
 *
 * Draw a check/3state button coming from DrawFrameControl()
 *
 * Does a pretty good job in emulating MS behavior. Some quirks are
 * however there because MS uses a TrueType font (Marlett) to draw
 * the buttons.
 */

static BOOL UITOOLS95_DFC_ButtonCheck(HDC dc, LPRECT r, UINT uFlags)
{
    RECT myr, bar;
    UINT flags = BF_RECT | BF_ADJUST;

    UITOOLS_MakeSquareRect(r, &myr);

    if(uFlags & DFCS_FLAT) flags |= BF_FLAT;
    else if(uFlags & DFCS_MONO) flags |= BF_MONO;

    UITOOLS95_DrawRectEdge( dc, &myr, EDGE_SUNKEN, flags );

    if (!(uFlags & DFCS_TRANSPARENT))
    {
	if(uFlags & (DFCS_INACTIVE | DFCS_PUSHED))
            FillRect_k(dc, &myr, GetSysColorBrush_k(COLOR_BTNFACE));
	else if( (uFlags & DFCS_BUTTON3STATE) && (uFlags & DFCS_CHECKED) )
            UITOOLS_DrawCheckedRect( dc, &myr );
	else
            FillRect_k(dc, &myr, GetSysColorBrush_k(COLOR_WINDOW));
    }

    if(uFlags & DFCS_CHECKED)
    {
        int i, k;
        i = (uFlags & DFCS_INACTIVE) || (uFlags & 0xff) == DFCS_BUTTON3STATE ?
		COLOR_BTNSHADOW : COLOR_WINDOWTEXT;

        /* draw 7 bars, with h=3w to form the check */
        bar.left = myr.left;
	bar.top = myr.top + 2;
        for (k = 0; k < 7; k++) {
            bar.left = bar.left + 1;
	    bar.top = (k < 3) ? bar.top + 1 : bar.top - 1;
	    bar.bottom = bar.top + 3;
   	    bar.right = bar.left + 1;
            FillRect_k(dc, &bar, GetSysColorBrush_k(i));
	}
    }
    return TRUE;
}


/************************************************************************
 *	UITOOLS_DFC_ButtonRadio
 *
 * Draw a radio/radioimage/radiomask button coming from DrawFrameControl()
 *
 * Does a pretty good job in emulating MS behavior. Some quirks are
 * however there because MS uses a TrueType font (Marlett) to draw
 * the buttons.
 */
static BOOL UITOOLS95_DFC_ButtonRadio(HDC dc, LPRECT r, UINT uFlags)
{
    RECT myr;
    int i;
    int SmallDiam = UITOOLS_MakeSquareRect(r, &myr);
    int BorderShrink = SmallDiam / 16;
    HPEN hpsave;
    HBRUSH hbsave;
    int xc, yc;

    if(BorderShrink < 1) BorderShrink = 1;

    if((uFlags & 0xff) == DFCS_BUTTONRADIOIMAGE)
        FillRect_k(dc, r, GetStockObject_k(BLACK_BRUSH));
    else if((uFlags & 0xff) == DFCS_BUTTONRADIOMASK)
        FillRect_k(dc, r, GetStockObject_k(WHITE_BRUSH));

    xc = myr.left + SmallDiam - SmallDiam/2;
    yc = myr.top  + SmallDiam - SmallDiam/2;

    /* Define bounding box */
    i = 14*SmallDiam/16;
    myr.left   = xc - i+i/2;
    myr.right  = xc + i/2;
    myr.top    = yc - i+i/2;
    myr.bottom = yc + i/2;

    if((uFlags & 0xff) == DFCS_BUTTONRADIOMASK)
    {
        hbsave = SelectObject_k(dc, GetStockObject_k(BLACK_BRUSH));
        Ellipse_k(dc, myr.left, myr.top, myr.right, myr.bottom);
        SelectObject_k(dc, hbsave);
    }
    else
    {
        if(uFlags & (DFCS_FLAT|DFCS_MONO))
        {
            hpsave = SelectObject_k(dc, SYSCOLOR_GetPen(COLOR_WINDOWFRAME));
            hbsave = SelectObject_k(dc, GetSysColorBrush_k(COLOR_WINDOWFRAME));
            Ellipse_k(dc, myr.left, myr.top, myr.right, myr.bottom);
            SelectObject_k(dc, hbsave);
            SelectObject_k(dc, hpsave);
        }
        else
        {
            hpsave = SelectObject_k(dc, SYSCOLOR_GetPen(COLOR_BTNHIGHLIGHT));
            hbsave = SelectObject_k(dc, GetSysColorBrush_k(COLOR_BTNHIGHLIGHT));
            Pie_k(dc, myr.left, myr.top, myr.right+1, myr.bottom+1, myr.left-1, myr.bottom, myr.right+1, myr.top);

            SelectObject_k(dc, SYSCOLOR_GetPen(COLOR_BTNSHADOW));
            SelectObject_k(dc, GetSysColorBrush_k(COLOR_BTNSHADOW));
            Pie_k(dc, myr.left, myr.top, myr.right+1, myr.bottom+1, myr.right+1, myr.top, myr.left-1, myr.bottom);

            myr.left   += BorderShrink;
            myr.right  -= BorderShrink;
            myr.top    += BorderShrink;
            myr.bottom -= BorderShrink;

            SelectObject_k(dc, SYSCOLOR_GetPen(COLOR_3DLIGHT));
            SelectObject_k(dc, GetSysColorBrush_k(COLOR_3DLIGHT));
            Pie_k(dc, myr.left, myr.top, myr.right+1, myr.bottom+1, myr.left-1, myr.bottom, myr.right+1, myr.top);

            SelectObject_k(dc, SYSCOLOR_GetPen(COLOR_3DDKSHADOW));
            SelectObject_k(dc, GetSysColorBrush_k(COLOR_3DDKSHADOW));
            Pie_k(dc, myr.left, myr.top, myr.right+1, myr.bottom+1, myr.right+1, myr.top, myr.left-1, myr.bottom);
            SelectObject_k(dc, hbsave);
            SelectObject_k(dc, hpsave);
        }

        i = 10*SmallDiam/16;
        myr.left   = xc - i+i/2;
        myr.right  = xc + i/2;
        myr.top    = yc - i+i/2;
        myr.bottom = yc + i/2;
        i= !(uFlags & (DFCS_INACTIVE|DFCS_PUSHED)) ? COLOR_WINDOW : COLOR_BTNFACE;
        hpsave = SelectObject_k(dc, SYSCOLOR_GetPen(i));
        hbsave = SelectObject_k(dc, GetSysColorBrush_k(i));
        Ellipse_k(dc, myr.left, myr.top, myr.right, myr.bottom);
        SelectObject_k(dc, hbsave);
        SelectObject_k(dc, hpsave);
    }

    if(uFlags & DFCS_CHECKED)
    {
        i = 6*SmallDiam/16;
        i = i < 1 ? 1 : i;
        myr.left   = xc - i+i/2;
        myr.right  = xc + i/2;
        myr.top    = yc - i+i/2;
        myr.bottom = yc + i/2;

        i = uFlags & DFCS_INACTIVE ? COLOR_BTNSHADOW : COLOR_WINDOWTEXT;
        hbsave = SelectObject_k(dc, GetSysColorBrush_k(i));
        hpsave = SelectObject_k(dc, SYSCOLOR_GetPen(i));
        Ellipse_k(dc, myr.left, myr.top, myr.right, myr.bottom);
        SelectObject_k(dc, hpsave);
        SelectObject_k(dc, hbsave);
    }

    /* FIXME: M$ has a polygon in the center at relative points: */
    /* 0.476, 0.476 (times SmallDiam, SmallDiam) */
    /* 0.476, 0.525 */
    /* 0.500, 0.500 */
    /* 0.500, 0.499 */
    /* when the button is unchecked. The reason for it is unknown. The */
    /* color is COLOR_BTNHIGHLIGHT, although the polygon gets painted at */
    /* least 3 times (it looks like a clip-region when you see it happen). */
    /* I do not really see a reason why this should be implemented. If you */
    /* have a good reason, let me know. Maybe this is a quirk in the Marlett */
    /* font. */

    return TRUE;
}

/***********************************************************************
 *           UITOOLS_DrawFrameButton
 */
static BOOL UITOOLS95_DrawFrameButton(HDC hdc, LPRECT rc, UINT uState)
{
    switch(uState & 0xff)
    {
    case DFCS_BUTTONPUSH:
        return UITOOLS95_DFC_ButtonPush(hdc, rc, uState);

    case DFCS_BUTTONCHECK:
    case DFCS_BUTTON3STATE:
        return UITOOLS95_DFC_ButtonCheck(hdc, rc, uState);

    case DFCS_BUTTONRADIOIMAGE:
    case DFCS_BUTTONRADIOMASK:
    case DFCS_BUTTONRADIO:
        return UITOOLS95_DFC_ButtonRadio(hdc, rc, uState);

    default:
        WARN("Invalid button state=0x%04x\n", uState);
    }

    return FALSE;
}

/***********************************************************************
 *           UITOOLS_DrawFrameCaption
 *
 * Draw caption buttons (win95), coming from DrawFrameControl()
 */

static BOOL UITOOLS95_DrawFrameCaption(HDC dc, LPRECT r, UINT uFlags)
{
    RECT myr;
    int SmallDiam = UITOOLS_MakeSquareRect(r, &myr)-2;
    HFONT hfsave, hf;
    int colorIdx = uFlags & DFCS_INACTIVE ? COLOR_BTNSHADOW : COLOR_BTNTEXT;
    int xc = (myr.left+myr.right)/2;
    int yc = (myr.top+myr.bottom)/2;
    WCHAR str[] = {0, 0};
    static const WCHAR glyphFontName[] = { 'M','a','r','l','e','t','t',0 };
    UINT alignsave;
    int bksave;
    COLORREF clrsave;
    SIZE size;

    UITOOLS95_DFC_ButtonPush(dc, r, uFlags & 0xff00);

    switch(uFlags & 0xf)
    {
    case DFCS_CAPTIONCLOSE:     str[0] = 0x72; break;
    case DFCS_CAPTIONHELP:      str[0] = 0x73; break;
    case DFCS_CAPTIONMIN:       str[0] = 0x30; break;
    case DFCS_CAPTIONMAX:       str[0] = 0x31; break;
    case DFCS_CAPTIONRESTORE:   str[0] = 0x32; break;
    default:
        WARN("Invalid caption; flags=0x%04x\n", uFlags);
        return FALSE;
    }
    
    hf = CreateFontW_k(-SmallDiam, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                    SYMBOL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE, glyphFontName);
    alignsave = SetTextAlign_k(dc, TA_TOP|TA_LEFT);
    bksave = SetBkMode_k(dc, TRANSPARENT);
    clrsave = GetTextColor_k(dc);
    hfsave = SelectObject_k(dc, hf);
    GetTextExtentPoint32W_k(dc, str, 1, &size);

    if(uFlags & DFCS_INACTIVE)
    {
        SetTextColor_k(dc, GetSysColor_k(COLOR_BTNHIGHLIGHT));
        TextOutW_k(dc, xc-size.cx/2+1, yc-size.cy/2+1, str, 1);
    }
    SetTextColor_k(dc, GetSysColor_k(colorIdx));
    TextOutW_k(dc, xc-size.cx/2, yc-size.cy/2, str, 1);

    SelectObject_k(dc, hfsave);
    SetTextColor_k(dc, clrsave);
    SetBkMode_k(dc, bksave);
    SetTextAlign_k(dc, alignsave);
    DeleteObject_k(hf);
  
    return TRUE;
}


/************************************************************************
 *      UITOOLS_DrawFrameScroll
 *
 * Draw a scroll-bar control coming from DrawFrameControl()
 */
static BOOL UITOOLS95_DrawFrameScroll(HDC dc, LPRECT r, UINT uFlags)
{
    POINT Line[4];
    RECT myr;
    int SmallDiam = UITOOLS_MakeSquareRect(r, &myr) - 2;
    int i;
    HBRUSH hbsave, hb, hb2;
    HPEN hpsave, hp, hp2;
    int tri = 290*SmallDiam/1000 - 1;
    int d46, d93;

    /*
     * This fixes a problem with really tiny "scroll" buttons. In particular
     * with the updown control.
     * Making sure that the arrow is as least 3 pixels wide (or high).
     */
    tri = max( 2, tri );

    switch(uFlags & 0xff)
    {
    case DFCS_SCROLLCOMBOBOX:
    case DFCS_SCROLLDOWN:
        Line[2].x = myr.left + 470*SmallDiam/1000 + 2;
        Line[2].y = myr.top  + 687*SmallDiam/1000 + 1;
        Line[0].x = Line[2].x - tri;
        Line[1].x = Line[2].x + tri;
        Line[0].y = Line[1].y = Line[2].y - tri;
        break;

    case DFCS_SCROLLUP:
        Line[2].x = myr.left + 470*SmallDiam/1000 + 2;
        Line[2].y = myr.bottom - (687*SmallDiam/1000 + 1);
        Line[0].x = Line[2].x - tri;
        Line[1].x = Line[2].x + tri;
        Line[0].y = Line[1].y = Line[2].y + tri;
        break;

    case DFCS_SCROLLLEFT:
        Line[2].x = myr.right - (687*SmallDiam/1000 + 1);
        Line[2].y = myr.top  + 470*SmallDiam/1000 + 2;
        Line[0].y = Line[2].y - tri;
        Line[1].y = Line[2].y + tri;
        Line[0].x = Line[1].x = Line[2].x + tri;
        break;

    case DFCS_SCROLLRIGHT:
        Line[2].x = myr.left + 687*SmallDiam/1000 + 1;
        Line[2].y = myr.top  + 470*SmallDiam/1000 + 2;
        Line[0].y = Line[2].y - tri;
        Line[1].y = Line[2].y + tri;
        Line[0].x = Line[1].x = Line[2].x - tri;
        break;

    case DFCS_SCROLLSIZEGRIP:
        /* This one breaks the flow... */
        UITOOLS95_DrawRectEdge(dc, r, EDGE_BUMP, BF_MIDDLE | ((uFlags&(DFCS_MONO|DFCS_FLAT)) ? BF_MONO : 0));
        hpsave = SelectObject_k(dc, GetStockObject_k(NULL_PEN));
        hbsave = SelectObject_k(dc, GetStockObject_k(NULL_BRUSH));
        if(uFlags & (DFCS_MONO|DFCS_FLAT))
        {
            hp = hp2 = SYSCOLOR_GetPen(COLOR_WINDOWFRAME);
            hb = hb2 = GetSysColorBrush_k(COLOR_WINDOWFRAME);
        }
        else
        {
            hp  = SYSCOLOR_GetPen(COLOR_BTNHIGHLIGHT);
            hp2 = SYSCOLOR_GetPen(COLOR_BTNSHADOW);
            hb  = GetSysColorBrush_k(COLOR_BTNHIGHLIGHT);
            hb2 = GetSysColorBrush_k(COLOR_BTNSHADOW);
        }
        Line[0].x = Line[1].x = r->right-1;
        Line[2].y = Line[3].y = r->bottom-1;
        d46 = 46*SmallDiam/750;
        d93 = 93*SmallDiam/750;

        i = 586*SmallDiam/750;
        Line[0].y = r->bottom - i - 1;
        Line[3].x = r->right - i - 1;
        Line[1].y = Line[0].y + d46;
        Line[2].x = Line[3].x + d46;
        SelectObject_k(dc, hb);
        SelectObject_k(dc, hp);
        Polygon_k(dc, Line, 4);

        Line[1].y++; Line[2].x++;
        Line[0].y = Line[1].y + d93;
        Line[3].x = Line[2].x + d93;
        SelectObject_k(dc, hb2);
        SelectObject_k(dc, hp2);
        Polygon_k(dc, Line, 4);

        i = 398*SmallDiam/750;
        Line[0].y = r->bottom - i - 1;
        Line[3].x = r->right - i - 1;
        Line[1].y = Line[0].y + d46;
        Line[2].x = Line[3].x + d46;
        SelectObject_k(dc, hb);
        SelectObject_k(dc, hp);
        Polygon_k(dc, Line, 4);

        Line[1].y++; Line[2].x++;
        Line[0].y = Line[1].y + d93;
        Line[3].x = Line[2].x + d93;
        SelectObject_k(dc, hb2);
        SelectObject_k(dc, hp2);
        Polygon_k(dc, Line, 4);

        i = 210*SmallDiam/750;
        Line[0].y = r->bottom - i - 1;
        Line[3].x = r->right - i - 1;
        Line[1].y = Line[0].y + d46;
        Line[2].x = Line[3].x + d46;
        SelectObject_k(dc, hb);
        SelectObject_k(dc, hp);
        Polygon_k(dc, Line, 4);

        Line[1].y++; Line[2].x++;
        Line[0].y = Line[1].y + d93;
        Line[3].x = Line[2].x + d93;
        SelectObject_k(dc, hb2);
        SelectObject_k(dc, hp2);
        Polygon_k(dc, Line, 4);

        SelectObject_k(dc, hpsave);
        SelectObject_k(dc, hbsave);
        return TRUE;

    default:
        WARN("Invalid scroll; flags=0x%04x\n", uFlags);
        return FALSE;
    }

    /* Here do the real scroll-bar controls end up */
    if( ! (uFlags & (0xff00 & ~DFCS_ADJUSTRECT)) )
      /* UITOOLS95_DFC_ButtonPush always uses BF_SOFT which we don't */
      /* want for the normal scroll-arrow button. */
      UITOOLS95_DrawRectEdge( dc, r, EDGE_RAISED, (uFlags&DFCS_ADJUSTRECT) | BF_MIDDLE | BF_RECT);
    else
      UITOOLS95_DFC_ButtonPush(dc, r, (uFlags & 0xff00) );

    if(uFlags & DFCS_INACTIVE)
    {
        hbsave = SelectObject_k(dc, GetSysColorBrush_k(COLOR_BTNHIGHLIGHT));
        hpsave = SelectObject_k(dc, SYSCOLOR_GetPen(COLOR_BTNHIGHLIGHT));
        Polygon_k(dc, Line, 3);
        SelectObject_k(dc, hpsave);
        SelectObject_k(dc, hbsave);
    }

    if( (uFlags & DFCS_INACTIVE) || !(uFlags & DFCS_PUSHED) )
      for(i = 0; i < 3; i++)
      {
        Line[i].x--;
        Line[i].y--;
      }

    i = uFlags & DFCS_INACTIVE ? COLOR_BTNSHADOW : COLOR_BTNTEXT;
    hbsave = SelectObject_k(dc, GetSysColorBrush_k(i));
    hpsave = SelectObject_k(dc, SYSCOLOR_GetPen(i));
    Polygon_k(dc, Line, 3);
    SelectObject_k(dc, hpsave);
    SelectObject_k(dc, hbsave);

    return TRUE;
}

/************************************************************************
 *      UITOOLS_DrawFrameMenu
 *
 * Draw a menu control coming from DrawFrameControl()
 */
static BOOL UITOOLS95_DrawFrameMenu(HDC dc, LPRECT r, UINT uFlags)
{
    POINT Points[6];
    RECT myr;
    int SmallDiam = UITOOLS_MakeSquareRect(r, &myr);
    int i;
    HBRUSH hbsave;
    HPEN hpsave;
    int xe, ye;
    int xc, yc;
    BOOL retval = TRUE;

    /* Using black and white seems to be utterly wrong, but win95 doesn't */
    /* use anything else. I think I tried all sys-colors to change things */
    /* without luck. It seems as if this behavior is inherited from the */
    /* win31 DFC() implementation... (you remember, B/W menus). */

    FillRect_k(dc, r, GetStockObject_k(WHITE_BRUSH));

    hbsave = SelectObject_k(dc, GetStockObject_k(BLACK_BRUSH));
    hpsave = SelectObject_k(dc, GetStockObject_k(BLACK_PEN));

    switch(uFlags & 0xff)
    {
    case DFCS_MENUARROW:
        i = 187*SmallDiam/750;
        Points[2].x = myr.left + 468*SmallDiam/750;
        Points[2].y = myr.top  + 352*SmallDiam/750+1;
        Points[0].y = Points[2].y - i;
        Points[1].y = Points[2].y + i;
        Points[0].x = Points[1].x = Points[2].x - i;
        Polygon_k(dc, Points, 3);
        break;

    case DFCS_MENUBULLET:
        xe = myr.left;
        ye = myr.top  + SmallDiam - SmallDiam/2;
        xc = myr.left + SmallDiam - SmallDiam/2;
        yc = myr.top  + SmallDiam - SmallDiam/2;
        i = 234*SmallDiam/750;
        i = i < 1 ? 1 : i;
        myr.left   = xc - i+i/2;
        myr.right  = xc + i/2;
        myr.top    = yc - i+i/2;
        myr.bottom = yc + i/2;
        Pie_k(dc, myr.left, myr.top, myr.right, myr.bottom, xe, ye, xe, ye);
        break;

    case DFCS_MENUCHECK:
        Points[0].x = myr.left + 253*SmallDiam/1000;
        Points[0].y = myr.top  + 445*SmallDiam/1000;
        Points[1].x = myr.left + 409*SmallDiam/1000;
        Points[1].y = Points[0].y + (Points[1].x-Points[0].x);
        Points[2].x = myr.left + 690*SmallDiam/1000;
        Points[2].y = Points[1].y - (Points[2].x-Points[1].x);
        Points[3].x = Points[2].x;
        Points[3].y = Points[2].y + 3*SmallDiam/16;
        Points[4].x = Points[1].x;
        Points[4].y = Points[1].y + 3*SmallDiam/16;
        Points[5].x = Points[0].x;
        Points[5].y = Points[0].y + 3*SmallDiam/16;
        Polygon_k(dc, Points, 6);
        break;

    default:
        WARN("Invalid menu; flags=0x%04x\n", uFlags);
        retval = FALSE;
        break;
    }

    SelectObject_k(dc, hpsave);
    SelectObject_k(dc, hbsave);
    return retval;
}


/**********************************************************************
 *          DrawFrameControl  (USER32.@)
 */
BOOL WINAPI DrawFrameControl_k( HDC hdc, LPRECT rc, UINT uType,
                                  UINT uState )
{
    switch(uType)
    {
    case DFC_BUTTON:
      return UITOOLS95_DrawFrameButton(hdc, rc, uState);
    case DFC_CAPTION:
      return UITOOLS95_DrawFrameCaption(hdc, rc, uState);
    case DFC_MENU:
      return UITOOLS95_DrawFrameMenu(hdc, rc, uState);
    /*case DFC_POPUPMENU:
      FIXME("DFC_POPUPMENU: not implemented\n");
      break;*/
    case DFC_SCROLL:
      return UITOOLS95_DrawFrameScroll(hdc, rc, uState);
    default:
      WARN("(%p,%p,%d,%x), bad type!\n", hdc,rc,uType,uState );
    }
    return FALSE;
}


/***********************************************************************
 *		SetRect (USER32.@)
 */
BOOL WINAPI SetRect_k( LPRECT rect, INT left, INT top,
                       INT right, INT bottom )
{
    if (!rect) return FALSE;
    rect->left   = left;
    rect->right  = right;
    rect->top    = top;
    rect->bottom = bottom;
    return TRUE;
}


/***********************************************************************
 *		SetRectEmpty (USER32.@)
 */
BOOL WINAPI SetRectEmpty_k( LPRECT rect )
{
    if (!rect) return FALSE;
    rect->left = rect->right = rect->top = rect->bottom = 0;
    return TRUE;
}


/***********************************************************************
 *		CopyRect (USER32.@)
 */
BOOL WINAPI CopyRect_k( RECT *dest, const RECT *src )
{
    if (!dest || !src) return FALSE;
    *dest = *src;
    return TRUE;
}


/***********************************************************************
 *		IsRectEmpty (USER32.@)
 *
 * Bug compat: Windows checks for 0 or negative width/height.
 */
BOOL WINAPI IsRectEmpty_k( const RECT *rect )
{
    if (!rect) return TRUE;
    return ((rect->left >= rect->right) || (rect->top >= rect->bottom));
}


/***********************************************************************
 *		PtInRect (USER32.@)
 */
BOOL WINAPI PtInRect_k( const RECT *rect, POINT pt )
{
    if (!rect) return FALSE;
    return ((pt.x >= rect->left) && (pt.x < rect->right) &&
            (pt.y >= rect->top) && (pt.y < rect->bottom));
}


/***********************************************************************
 *		OffsetRect (USER32.@)
 */
BOOL WINAPI OffsetRect_k( LPRECT rect, INT x, INT y )
{
    if (!rect) return FALSE;
    rect->left   += x;
    rect->right  += x;
    rect->top    += y;
    rect->bottom += y;
    return TRUE;
}


/***********************************************************************
 *		InflateRect (USER32.@)
 */
BOOL WINAPI InflateRect_k( LPRECT rect, INT x, INT y )
{
    if (!rect) return FALSE;
    rect->left   -= x;
    rect->top    -= y;
    rect->right  += x;
    rect->bottom += y;
    return TRUE;
}


/***********************************************************************
 *		IntersectRect (USER32.@)
 */
BOOL WINAPI IntersectRect_k( LPRECT dest, const RECT *src1, const RECT *src2 )
{
    if (!dest || !src1 || !src2) return FALSE;
    if (IsRectEmpty_k(src1) || IsRectEmpty_k(src2) ||
        (src1->left >= src2->right) || (src2->left >= src1->right) ||
        (src1->top >= src2->bottom) || (src2->top >= src1->bottom))
    {
        SetRectEmpty_k( dest );
        return FALSE;
    }
    dest->left   = max( src1->left, src2->left );
    dest->right  = min( src1->right, src2->right );
    dest->top    = max( src1->top, src2->top );
    dest->bottom = min( src1->bottom, src2->bottom );
    return TRUE;
}


/***********************************************************************
 *		UnionRect (USER32.@)
 */
BOOL WINAPI UnionRect_k( LPRECT dest, const RECT *src1, const RECT *src2 )
{
    if (!dest) return FALSE;
    if (IsRectEmpty_k(src1))
    {
        if (IsRectEmpty_k(src2))
        {
            SetRectEmpty_k( dest );
            return FALSE;
        }
        else *dest = *src2;
    }
    else
    {
        if (IsRectEmpty_k(src2)) *dest = *src1;
        else
        {
            dest->left   = min( src1->left, src2->left );
            dest->right  = max( src1->right, src2->right );
            dest->top    = min( src1->top, src2->top );
            dest->bottom = max( src1->bottom, src2->bottom );
        }
    }
    return TRUE;
}


/***********************************************************************
 *		EqualRect (USER32.@)
 */
BOOL WINAPI EqualRect_k( const RECT* rect1, const RECT* rect2 )
{
    if (!rect1 || !rect2) return FALSE;
    return ((rect1->left == rect2->left) && (rect1->right == rect2->right) &&
            (rect1->top == rect2->top) && (rect1->bottom == rect2->bottom));
}


/***********************************************************************
 *		SubtractRect (USER32.@)
 */
BOOL WINAPI SubtractRect_k( LPRECT dest, const RECT *src1, const RECT *src2 )
{
    RECT tmp;

    if (!dest) return FALSE;
    if (IsRectEmpty_k( src1 ))
    {
        SetRectEmpty_k( dest );
        return FALSE;
    }
    *dest = *src1;
    if (IntersectRect_k( &tmp, src1, src2 ))
    {
        if (EqualRect_k( &tmp, dest ))
        {
            SetRectEmpty_k( dest );
            return FALSE;
        }
        if ((tmp.top == dest->top) && (tmp.bottom == dest->bottom))
        {
            if (tmp.left == dest->left) dest->left = tmp.right;
            else if (tmp.right == dest->right) dest->right = tmp.left;
        }
        else if ((tmp.left == dest->left) && (tmp.right == dest->right))
        {
            if (tmp.top == dest->top) dest->top = tmp.bottom;
            else if (tmp.bottom == dest->bottom) dest->bottom = tmp.top;
        }
    }
    return TRUE;
}


/***********************************************************************
 *		FillRect (USER32.@)
 */
INT WINAPI FillRect_k( HDC hdc, const RECT *rect, HBRUSH hbrush )
{
    HBRUSH prev_brush;

    if (hbrush <= (HBRUSH) (COLOR_MAX + 1)) hbrush = GetSysColorBrush_k( HandleToULong(hbrush) - 1 );

    prev_brush = SelectObject_k( hdc, hbrush );
    PatBlt_k( hdc, rect->left, rect->top,
              rect->right - rect->left, rect->bottom - rect->top, PATCOPY );
    if (prev_brush) SelectObject_k( hdc, prev_brush );
    return 1;
}


/***********************************************************************
 *		InvertRect (USER32.@)
 */
BOOL WINAPI InvertRect_k( HDC hdc, const RECT *rect )
{
    return PatBlt_k( hdc, rect->left, rect->top,
                   rect->right - rect->left, rect->bottom - rect->top, DSTINVERT );
}


/***********************************************************************
 *		FrameRect (USER32.@)
 */
INT WINAPI FrameRect_k( HDC hdc, const RECT *rect, HBRUSH hbrush )
{
    HBRUSH prevBrush;
    RECT r = *rect;

    if ( (r.right <= r.left) || (r.bottom <= r.top) ) return 0;
    if (!(prevBrush = SelectObject_k( hdc, hbrush ))) return 0;

    PatBlt_k( hdc, r.left, r.top, 1, r.bottom - r.top, PATCOPY );
    PatBlt_k( hdc, r.right - 1, r.top, 1, r.bottom - r.top, PATCOPY );
    PatBlt_k( hdc, r.left, r.top, r.right - r.left, 1, PATCOPY );
    PatBlt_k( hdc, r.left, r.bottom - 1, r.right - r.left, 1, PATCOPY );

    SelectObject_k( hdc, prevBrush );
    return TRUE;
}


/***********************************************************************
 *		DrawFocusRect (USER32.@)
 *
 * FIXME: PatBlt_k(PATINVERT) with background brush.
 */
BOOL WINAPI DrawFocusRect_k( HDC hdc, const RECT* rc )
{
    HBRUSH hOldBrush;
    HPEN hOldPen, hNewPen;
    INT oldDrawMode, oldBkMode;
    LOGBRUSH lb;

    hOldBrush = SelectObject_k(hdc, GetStockObject_k(NULL_BRUSH));
    lb.lbStyle = BS_SOLID;
    lb.lbColor = 0;
    hNewPen = ExtCreatePen_k(PS_COSMETIC|PS_ALTERNATE, 1, &lb, 0, NULL);
    hOldPen = SelectObject_k(hdc, hNewPen);
    oldDrawMode = SetROP2_k(hdc, R2_NOT);
    oldBkMode = SetBkMode_k(hdc, TRANSPARENT);

    Rectangle_k(hdc, rc->left, rc->top, rc->right, rc->bottom);

    SetBkMode_k(hdc, oldBkMode);
    SetROP2_k(hdc, oldDrawMode);
    SelectObject_k(hdc, hOldPen);
    DeleteObject_k(hNewPen);
    SelectObject_k(hdc, hOldBrush);

    return TRUE;
}


/**********************************************************************
 *		DrawAnimatedRects (USER32.@)
 */
BOOL WINAPI DrawAnimatedRects_k( HWND hwnd, INT idAni, const RECT* lprcFrom, const RECT* lprcTo )
{
    FIXME("(%p,%d,%p,%p): stub\n",hwnd,idAni,lprcFrom,lprcTo);
    return TRUE;
}


/**********************************************************************
 *          UITOOLS_DrawStateJam
 *
 * Jams in the requested type in the dc
 */
static BOOL UITOOLS_DrawStateJam( HDC hdc, UINT opcode, DRAWSTATEPROC func, LPARAM lp, WPARAM wp,
                                  LPRECT rc, UINT dtflags, BOOL unicode )
{
    HDC memdc;
    HBITMAP hbmsave;
    BOOL retval;
    INT cx = rc->right - rc->left;
    INT cy = rc->bottom - rc->top;

    switch(opcode)
    {
    case DST_TEXT:
    case DST_PREFIXTEXT:
        if(unicode)
            return DrawTextW_k(hdc, (LPWSTR)lp, (INT)wp, rc, dtflags);
        else
            return DrawTextA_k(hdc, (LPSTR)lp, (INT)wp, rc, dtflags);

    case DST_ICON:
        return DrawIconEx_k(hdc, rc->left, rc->top, (HICON)lp, 0, 0, 0, NULL, DI_NORMAL);

    case DST_BITMAP:
        memdc = CreateCompatibleDC_k(hdc);
        if(!memdc) return FALSE;
        hbmsave = SelectObject_k(memdc, (HBITMAP)lp);
        if(!hbmsave)
        {
            DeleteDC_k(memdc);
            return FALSE;
        }
        retval = BitBlt_k(hdc, rc->left, rc->top, cx, cy, memdc, 0, 0, SRCCOPY);
        SelectObject_k(memdc, hbmsave);
        DeleteDC_k(memdc);
        return retval;

    case DST_COMPLEX:
        if(func) {
            BOOL bRet;
            /* DRAWSTATEPROC assumes that it draws at the center of coordinates  */

            OffsetViewportOrgEx_k(hdc, rc->left, rc->top, NULL);
            bRet = func(hdc, lp, wp, cx, cy);
            /* Restore origin */
            OffsetViewportOrgEx_k(hdc, -rc->left, -rc->top, NULL);
            return bRet;
        } else
            return FALSE;
    }
    return FALSE;
}

/**********************************************************************
 *      UITOOLS_DrawState()
 */
static BOOL UITOOLS_DrawState(HDC hdc, HBRUSH hbr, DRAWSTATEPROC func, LPARAM lp, WPARAM wp,
                              INT x, INT y, INT cx, INT cy, UINT flags, BOOL unicode )
{
    HBITMAP hbm, hbmsave;
    HFONT hfsave;
    HBRUSH hbsave, hbrtmp = 0;
    HDC memdc;
    RECT rc;
    UINT dtflags = DT_NOCLIP;
    COLORREF fg, bg;
    UINT opcode = flags & 0xf;
    INT len = wp;
    BOOL retval, tmp;

    if((opcode == DST_TEXT || opcode == DST_PREFIXTEXT) && !len)    /* The string is '\0' terminated */
    {
        if (!lp) return FALSE;

        if(unicode)
            len = strlenW((LPWSTR)lp);
        else
            len = strlen((LPSTR)lp);
    }

    /* Find out what size the image has if not given by caller */
    if(!cx || !cy)
    {
        SIZE s;
        BITMAP bm;

        switch(opcode)
        {
        case DST_TEXT:
        case DST_PREFIXTEXT:
            if(unicode)
                retval = GetTextExtentPoint32W_k(hdc, (LPWSTR)lp, len, &s);
            else
                retval = GetTextExtentPoint32A_k(hdc, (LPSTR)lp, len, &s);
            if(!retval) return FALSE;
            break;

        case DST_ICON:
            if (!get_icon_size( (HICON)lp, &s )) return FALSE;
            break;

        case DST_BITMAP:
            if(!GetObjectA_k((HBITMAP)lp, sizeof(bm), &bm)) return FALSE;
            s.cx = bm.bmWidth;
            s.cy = bm.bmHeight;
            break;

        case DST_COMPLEX: /* cx and cy must be set in this mode */
            return FALSE;
        }

        if(!cx) cx = s.cx;
        if(!cy) cy = s.cy;
    }

    rc.left   = x;
    rc.top    = y;
    rc.right  = x + cx;
    rc.bottom = y + cy;

    if(flags & DSS_RIGHT)    /* This one is not documented in the win32.hlp file */
        dtflags |= DT_RIGHT;
    if(opcode == DST_TEXT)
        dtflags |= DT_NOPREFIX;

    /* For DSS_NORMAL we just jam in the image and return */
    if((flags & 0x7ff0) == DSS_NORMAL)
    {
        return UITOOLS_DrawStateJam(hdc, opcode, func, lp, len, &rc, dtflags, unicode);
    }

    /* For all other states we need to convert the image to B/W in a local bitmap */
    /* before it is displayed */
    fg = SetTextColor_k(hdc, RGB(0, 0, 0));
    bg = SetBkColor_k(hdc, RGB(255, 255, 255));
    hbm = NULL; hbmsave = NULL;
    memdc = NULL; hbsave = NULL;
    retval = FALSE; /* assume failure */

    /* From here on we must use "goto cleanup" when something goes wrong */
    hbm     = CreateBitmap_k(cx, cy, 1, 1, NULL);
    if(!hbm) goto cleanup;
    memdc   = CreateCompatibleDC_k(hdc);
    if(!memdc) goto cleanup;
    hbmsave = SelectObject_k(memdc, hbm);
    if(!hbmsave) goto cleanup;
    rc.left = rc.top = 0;
    rc.right = cx;
    rc.bottom = cy;
    if(!FillRect_k(memdc, &rc, GetStockObject_k(WHITE_BRUSH))) goto cleanup;
    SetBkColor_k(memdc, RGB(255, 255, 255));
    SetTextColor_k(memdc, RGB(0, 0, 0));
    hfsave  = SelectObject_k(memdc, GetCurrentObject_k(hdc, OBJ_FONT));
    SetLayout_k( memdc, GetLayout_k( hdc ));

    /* DST_COMPLEX may draw text as well,
     * so we must be sure that correct font is selected
     */
    if(!hfsave && (opcode <= DST_PREFIXTEXT)) goto cleanup;
    tmp = UITOOLS_DrawStateJam(memdc, opcode, func, lp, len, &rc, dtflags, unicode);
    if(hfsave) SelectObject_k(memdc, hfsave);
    if(!tmp) goto cleanup;

    /* This state cause the image to be dithered */
    if(flags & DSS_UNION)
    {
        hbsave = SelectObject_k(memdc, getSYSCOLOR_55AABrush());
        if(!hbsave) goto cleanup;
        tmp = PatBlt_k(memdc, 0, 0, cx, cy, 0x00FA0089);
        SelectObject_k(memdc, hbsave);
        if(!tmp) goto cleanup;
    }

    if (flags & DSS_DISABLED)
       hbrtmp = CreateSolidBrush_k(GetSysColor_k(COLOR_3DHILIGHT));
    else if (flags & DSS_DEFAULT)
       hbrtmp = CreateSolidBrush_k(GetSysColor_k(COLOR_3DSHADOW));

    /* Draw light or dark shadow */
    if (flags & (DSS_DISABLED|DSS_DEFAULT))
    {
       if(!hbrtmp) goto cleanup;
       hbsave = SelectObject_k(hdc, hbrtmp);
       if(!hbsave) goto cleanup;
       if(!BitBlt_k(hdc, x+1, y+1, cx, cy, memdc, 0, 0, 0x00B8074A)) goto cleanup;
       SelectObject_k(hdc, hbsave);
       DeleteObject_k(hbrtmp);
       hbrtmp = 0;
    }

    if (flags & DSS_DISABLED)
    {
       hbr = hbrtmp = CreateSolidBrush_k(GetSysColor_k(COLOR_3DSHADOW));
       if(!hbrtmp) goto cleanup;
    }
    else if (!hbr)
    {
       hbr = GetStockObject_k(BLACK_BRUSH);
    }

    hbsave = SelectObject_k(hdc, hbr);

    if(!BitBlt_k(hdc, x, y, cx, cy, memdc, 0, 0, 0x00B8074A)) goto cleanup;

    retval = TRUE; /* We succeeded */

cleanup:
    SetTextColor_k(hdc, fg);
    SetBkColor_k(hdc, bg);

    if(hbsave)  SelectObject_k(hdc, hbsave);
    if(hbmsave) SelectObject_k(memdc, hbmsave);
    if(hbrtmp)  DeleteObject_k(hbrtmp);
    if(hbm)     DeleteObject_k(hbm);
    if(memdc)   DeleteDC_k(memdc);

    return retval;
}

/**********************************************************************
 *		DrawStateA (USER32.@)
 */
BOOL WINAPI DrawStateA_k(HDC hdc, HBRUSH hbr,
                   DRAWSTATEPROC func, LPARAM ldata, WPARAM wdata,
                   INT x, INT y, INT cx, INT cy, UINT flags)
{
    return UITOOLS_DrawState(hdc, hbr, func, ldata, wdata, x, y, cx, cy, flags, FALSE);
}

/**********************************************************************
 *		DrawStateW (USER32.@)
 */
BOOL WINAPI DrawStateW_k(HDC hdc, HBRUSH hbr,
                   DRAWSTATEPROC func, LPARAM ldata, WPARAM wdata,
                   INT x, INT y, INT cx, INT cy, UINT flags)
{
    return UITOOLS_DrawState(hdc, hbr, func, ldata, wdata, x, y, cx, cy, flags, TRUE);
}

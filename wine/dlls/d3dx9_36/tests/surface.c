/*
 * Tests for the D3DX9 surface functions
 *
 * Copyright 2009 Tony Wasserka
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

#define COBJMACROS
#include "wine/test.h"
#include "d3dx9tex.h"
#include "resources.h"

static inline int get_ref(IUnknown *obj)
{
    IUnknown_AddRef(obj);
    return IUnknown_Release(obj);
}

static inline void check_ref(IUnknown *obj, int exp)
{
    int ref = get_ref(obj);
    ok (exp == ref, "Invalid refcount. Expected %d, got %d\n", exp, ref);
}

static inline void check_release(IUnknown *obj, int exp)
{
    int ref = IUnknown_Release(obj);
    ok (ref == exp, "Invalid refcount. Expected %d, got %d\n", exp, ref);
}

/* 1x1 bmp (1 bpp) */
static const unsigned char bmp01[66] = {
0x42,0x4d,0x42,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x28,0x00,
0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x00,0x00,
0x00,0x00,0x04,0x00,0x00,0x00,0x12,0x0b,0x00,0x00,0x12,0x0b,0x00,0x00,0x02,0x00,
0x00,0x00,0x02,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0x00,0x00,0x00,
0x00,0x00
};

/* 2x2 A8R8G8B8 pixel data */
static const unsigned char pixdata[] = {
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

/* invalid image file */
static const unsigned char noimage[4] = {
0x11,0x22,0x33,0x44
};

/* 2x2 24-bit dds, 2 mipmaps */
static const unsigned char dds_24bit[] = {
0x44,0x44,0x53,0x20,0x7c,0x00,0x00,0x00,0x07,0x10,0x0a,0x00,0x02,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,
0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0xff,0x00,
0x00,0xff,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x10,0x40,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

/* 2x2 16-bit dds, no mipmaps */
static const unsigned char dds_16bit[] = {
0x44,0x44,0x53,0x20,0x7c,0x00,0x00,0x00,0x07,0x10,0x08,0x00,0x02,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,
0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x7c,0x00,0x00,
0xe0,0x03,0x00,0x00,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xff,0x7f,0xff,0x7f,0xff,0x7f,0xff,0x7f
};

/* 4x4 cube map dds */
static const unsigned char dds_cube_map[] = {
0x44,0x44,0x53,0x20,0x7c,0x00,0x00,0x00,0x07,0x10,0x08,0x00,0x04,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x44,0x58,0x54,0x35,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x10,0x00,0x00,
0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50
};

/* 4x4x2 volume map dds, 2 mipmaps */
static const unsigned char dds_volume_map[] = {
0x44,0x44,0x53,0x20,0x7c,0x00,0x00,0x00,0x07,0x10,0x8a,0x00,0x04,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x44,0x58,0x54,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x10,0x40,0x00,
0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xef,0x87,0x0f,0x78,0x05,0x05,0x50,0x50,
0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x2f,0x7e,0xcf,0x79,0x01,0x54,0x5c,0x5c,
0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x84,0xef,0x7b,0xaa,0xab,0xab,0xab
};

static HRESULT create_file(const char *filename, const unsigned char *data, const unsigned int size)
{
    DWORD received;
    HANDLE hfile;

    hfile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(hfile == INVALID_HANDLE_VALUE) return HRESULT_FROM_WIN32(GetLastError());

    if(WriteFile(hfile, data, size, &received, NULL))
    {
        CloseHandle(hfile);
        return D3D_OK;
    }

    CloseHandle(hfile);
    return D3DERR_INVALIDCALL;
}

static void test_D3DXGetImageInfo(void)
{
    HRESULT hr;
    D3DXIMAGE_INFO info;
    BOOL testdummy_ok, testbitmap_ok;

    hr = create_file("testdummy.bmp", noimage, sizeof(noimage));  /* invalid image */
    testdummy_ok = SUCCEEDED(hr);

    hr = create_file("testbitmap.bmp", bmp01, sizeof(bmp01));  /* valid image */
    testbitmap_ok = SUCCEEDED(hr);

    /* D3DXGetImageInfoFromFile */
    if(testbitmap_ok) {
        hr = D3DXGetImageInfoFromFileA("testbitmap.bmp", &info);
        ok(hr == D3D_OK, "D3DXGetImageInfoFromFile returned %#x, expected %#x\n", hr, D3D_OK);

        hr = D3DXGetImageInfoFromFileA("testbitmap.bmp", NULL); /* valid image, second parameter is NULL */
        ok(hr == D3D_OK, "D3DXGetImageInfoFromFile returned %#x, expected %#x\n", hr, D3D_OK);
    } else skip("Couldn't create \"testbitmap.bmp\"\n");

    if(testdummy_ok) {
        hr = D3DXGetImageInfoFromFileA("testdummy.bmp", NULL); /* invalid image, second parameter is NULL */
        ok(hr == D3D_OK, "D3DXGetImageInfoFromFile returned %#x, expected %#x\n", hr, D3D_OK);

        hr = D3DXGetImageInfoFromFileA("testdummy.bmp", &info);
        ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromFile returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);
    } else skip("Couldn't create \"testdummy.bmp\"\n");

    hr = D3DXGetImageInfoFromFileA("filedoesnotexist.bmp", &info);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromFile returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXGetImageInfoFromFileA("filedoesnotexist.bmp", NULL);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromFile returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXGetImageInfoFromFileA("", &info);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromFile returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXGetImageInfoFromFileA(NULL, &info);
    ok(hr == D3DERR_INVALIDCALL, "D3DXGetImageInfoFromFile returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXGetImageInfoFromFileA(NULL, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXGetImageInfoFromFile returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);


    /* D3DXGetImageInfoFromResource */
    todo_wine {
        hr = D3DXGetImageInfoFromResourceA(NULL, MAKEINTRESOURCEA(IDB_BITMAP_1x1), &info); /* RT_BITMAP */
        ok(hr == D3D_OK, "D3DXGetImageInfoFromResource returned %#x, expected %#x\n", hr, D3D_OK);

        hr = D3DXGetImageInfoFromResourceA(NULL, MAKEINTRESOURCEA(IDB_BITMAP_1x1), NULL);
        ok(hr == D3D_OK, "D3DXGetImageInfoFromResource returned %#x, expected %#x\n", hr, D3D_OK);
    }

    hr = D3DXGetImageInfoFromResourceA(NULL, MAKEINTRESOURCEA(IDD_BITMAPDATA_1x1), &info); /* RT_RCDATA */
    ok(hr == D3D_OK, "D3DXGetImageInfoFromResource returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXGetImageInfoFromResourceA(NULL, MAKEINTRESOURCEA(IDS_STRING), &info);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromResource returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXGetImageInfoFromResourceA(NULL, MAKEINTRESOURCEA(IDS_STRING), NULL);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromResource returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXGetImageInfoFromResourceA(NULL, "resourcedoesnotexist", &info);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromResource returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXGetImageInfoFromResourceA(NULL, "resourcedoesnotexist", NULL);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromResource returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXGetImageInfoFromResourceA(NULL, NULL, NULL);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromResource returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);


    /* D3DXGetImageInfoFromFileInMemory */
    hr = D3DXGetImageInfoFromFileInMemory(bmp01, sizeof(bmp01), &info);
    ok(hr == D3D_OK, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXGetImageInfoFromFileInMemory(bmp01, sizeof(bmp01)+5, &info); /* too large size */
    ok(hr == D3D_OK, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXGetImageInfoFromFileInMemory(bmp01, sizeof(bmp01), NULL);
    ok(hr == D3D_OK, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXGetImageInfoFromFileInMemory(noimage, sizeof(noimage), NULL);
    ok(hr == D3D_OK, "D3DXGetImageInfoFromResource returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXGetImageInfoFromFileInMemory(noimage, sizeof(noimage), &info);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    todo_wine {
        hr = D3DXGetImageInfoFromFileInMemory(bmp01, sizeof(bmp01)-1, &info);
        ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);
    }

    hr = D3DXGetImageInfoFromFileInMemory(bmp01+1, sizeof(bmp01)-1, &info);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXGetImageInfoFromFileInMemory(bmp01, 0, &info);
    ok(hr == D3DERR_INVALIDCALL, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXGetImageInfoFromFileInMemory(bmp01, 0, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXGetImageInfoFromFileInMemory(noimage, 0, &info);
    ok(hr == D3DERR_INVALIDCALL, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXGetImageInfoFromFileInMemory(noimage, 0, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXGetImageInfoFromFileInMemory(NULL, 0, &info);
    ok(hr == D3DERR_INVALIDCALL, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXGetImageInfoFromFileInMemory(NULL, 4, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXGetImageInfoFromFileInMemory(NULL, 4, &info);
    ok(hr == D3DERR_INVALIDCALL, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXGetImageInfoFromFileInMemory(NULL, 0, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);


    /* test DDS support */
    todo_wine {
        hr = D3DXGetImageInfoFromFileInMemory(dds_24bit, sizeof(dds_24bit), &info);
        ok(hr == D3D_OK, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3D_OK);
        if (hr == D3D_OK) {
            ok(info.Width == 2, "Got width %u, expected 2\n", info.Width);
            ok(info.Height == 2, "Got height %u, expected 2\n", info.Height);
            ok(info.Depth == 1, "Got depth %u, expected 1\n", info.Depth);
            ok(info.MipLevels == 2, "Got miplevels %u, expected 2\n", info.MipLevels);
            ok(info.Format == D3DFMT_R8G8B8, "Got format is %#x, expected %#x\n", info.Format, D3DFMT_R8G8B8);
            ok(info.ResourceType == D3DRTYPE_TEXTURE, "Got resource type %#x, expected %#x\n", info.ResourceType, D3DRTYPE_TEXTURE);
            ok(info.ImageFileFormat == D3DXIFF_DDS, "Got image file format %#x, expected %#x\n", info.ImageFileFormat, D3DXIFF_DDS);
        } else skip("Couldn't get image info from 24-bit DDS file in memory\n");

        hr = D3DXGetImageInfoFromFileInMemory(dds_16bit, sizeof(dds_16bit), &info);
        ok(hr == D3D_OK, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3D_OK);
        if (hr == D3D_OK) {
            ok(info.Width == 2, "Got width %u, expected 2\n", info.Width);
            ok(info.Height == 2, "Got height %u, expected 2\n", info.Height);
            ok(info.Depth == 1, "Got depth %u, expected 1\n", info.Depth);
            ok(info.MipLevels == 1, "Got miplevels %u, expected 1\n", info.MipLevels);
            ok(info.Format == D3DFMT_X1R5G5B5, "Got format is %#x, expected %#x\n", info.Format, D3DFMT_X1R5G5B5);
            ok(info.ResourceType == D3DRTYPE_TEXTURE, "Got resource type %#x, expected %#x\n", info.ResourceType, D3DRTYPE_TEXTURE);
            ok(info.ImageFileFormat == D3DXIFF_DDS, "Got image file format %#x, expected %#x\n", info.ImageFileFormat, D3DXIFF_DDS);
        } else skip("Couldn't get image info from 16-bit DDS file in memory\n");

        hr = D3DXGetImageInfoFromFileInMemory(dds_cube_map, sizeof(dds_cube_map), &info);
        ok(hr == D3D_OK, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3D_OK);
        if (hr == D3D_OK) {
            ok(info.Width == 4, "Got width %u, expected 4\n", info.Width);
            ok(info.Height == 4, "Got height %u, expected 4\n", info.Height);
            ok(info.Depth == 1, "Got depth %u, expected 1\n", info.Depth);
            ok(info.MipLevels == 1, "Got miplevels %u, expected 1\n", info.MipLevels);
            ok(info.Format == D3DFMT_DXT5, "Got format is %#x, expected %#x\n", info.Format, D3DFMT_DXT5);
            ok(info.ResourceType == D3DRTYPE_CUBETEXTURE, "Got resource type %#x, expected %#x\n", info.ResourceType, D3DRTYPE_CUBETEXTURE);
            ok(info.ImageFileFormat == D3DXIFF_DDS, "Got image file format %#x, expected %#x\n", info.ImageFileFormat, D3DXIFF_DDS);
        } else skip("Couldn't get image info from cube map in memory\n");

        hr = D3DXGetImageInfoFromFileInMemory(dds_volume_map, sizeof(dds_volume_map), &info);
        ok(hr == D3D_OK, "D3DXGetImageInfoFromFileInMemory returned %#x, expected %#x\n", hr, D3D_OK);
        if (hr == D3D_OK) {
            ok(info.Width == 4, "Got width %u, expected 4\n", info.Width);
            ok(info.Height == 4, "Got height %u, expected 4\n", info.Height);
            ok(info.Depth == 2, "Got depth %u, expected 2\n", info.Depth);
            ok(info.MipLevels == 3, "Got miplevels %u, expected 3\n", info.MipLevels);
            ok(info.Format == D3DFMT_DXT3, "Got format is %#x, expected %#x\n", info.Format, D3DFMT_DXT3);
            ok(info.ResourceType == D3DRTYPE_VOLUMETEXTURE, "Got resource type %#x, expected %#x\n", info.ResourceType, D3DRTYPE_VOLUMETEXTURE);
            ok(info.ImageFileFormat == D3DXIFF_DDS, "Got image file format %#x, expected %#x\n", info.ImageFileFormat, D3DXIFF_DDS);
        } else skip("Couldn't get image info from volume map in memory\n");
    }


    /* cleanup */
    if(testdummy_ok) DeleteFileA("testdummy.bmp");
    if(testbitmap_ok) DeleteFileA("testbitmap.bmp");
}

#define check_pixel_1bpp(lockrect, x, y, color) \
ok(((BYTE*)(lockrect).pBits)[(x) + (y) * (lockrect).Pitch] == color, "Got color 0x%02x, expected 0x%02x.\n", \
((BYTE*)(lockrect).pBits)[(x) + (y) * (lockrect).Pitch], color)

#define check_pixel_2bpp(lockrect, x, y, color) \
ok(((WORD*)(lockrect).pBits)[(x) + (y) * (lockrect).Pitch / 2] == color, "Got color 0x%04x, expected 0x%04x.\n", \
((WORD*)(lockrect).pBits)[(x) + (y) * (lockrect).Pitch / 2], color)

#define check_pixel_4bpp(lockrect, x, y, color) \
ok(((DWORD*)(lockrect).pBits)[(x) + (y) * (lockrect).Pitch / 4] == color, "Got color 0x%08x, expected 0x%08x.\n", \
((DWORD*)(lockrect).pBits)[(x) + (y) * (lockrect).Pitch / 4], color)
static void test_D3DXLoadSurface(IDirect3DDevice9 *device)
{
    HRESULT hr;
    BOOL testdummy_ok, testbitmap_ok;
    IDirect3DSurface9 *surf, *newsurf;
    RECT rect, destrect;
    D3DLOCKED_RECT lockrect;
    const WORD pixdata_a8r3g3b2[] = { 0x57df, 0x98fc, 0xacdd, 0xc891 };
    const WORD pixdata_a1r5g5b5[] = { 0x46b5, 0x99c8, 0x06a2, 0x9431 };
    const WORD pixdata_r5g6b5[] = { 0x9ef6, 0x658d, 0x0aee, 0x42ee };
    const WORD pixdata_a8l8[] = { 0xff00, 0x00ff, 0xff30, 0x7f7f };
    const DWORD pixdata_g16r16[] = { 0x07d23fbe, 0xdc7f44a4, 0xe4d8976b, 0x9a84fe89 };
    const DWORD pixdata_a8b8g8r8[] = { 0xc3394cf0, 0x235ae892, 0x09b197fd, 0x8dc32bf6 };
    const DWORD pixdata_a2r10g10b10[] = { 0x57395aff, 0x5b7668fd, 0xb0d856b5, 0xff2c61d6 };

    hr = create_file("testdummy.bmp", noimage, sizeof(noimage));  /* invalid image */
    testdummy_ok = SUCCEEDED(hr);

    hr = create_file("testbitmap.bmp", bmp01, sizeof(bmp01));  /* valid image */
    testbitmap_ok = SUCCEEDED(hr);

    hr = IDirect3DDevice9_CreateOffscreenPlainSurface(device, 256, 256, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &surf, NULL);
    if(FAILED(hr)) {
        skip("Failed to create a surface (%#x)\n", hr);
        if(testdummy_ok) DeleteFileA("testdummy.bmp");
        if(testbitmap_ok) DeleteFileA("testbitmap.bmp");
        return;
    }

    /* D3DXLoadSurfaceFromFile */
    if(testbitmap_ok) {
        todo_wine {
            hr = D3DXLoadSurfaceFromFileA(surf, NULL, NULL, "testbitmap.bmp", NULL, D3DX_DEFAULT, 0, NULL);
            ok(hr == D3D_OK, "D3DXLoadSurfaceFromFile returned %#x, expected %#x\n", hr, D3D_OK);
        }

        hr = D3DXLoadSurfaceFromFileA(NULL, NULL, NULL, "testbitmap.bmp", NULL, D3DX_DEFAULT, 0, NULL);
        ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromFile returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);
    } else skip("Couldn't create \"testbitmap.bmp\"\n");

    if(testdummy_ok) {
        hr = D3DXLoadSurfaceFromFileA(surf, NULL, NULL, "testdummy.bmp", NULL, D3DX_DEFAULT, 0, NULL);
        ok(hr == D3DXERR_INVALIDDATA, "D3DXLoadSurfaceFromFile returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);
    } else skip("Couldn't create \"testdummy.bmp\"\n");

    hr = D3DXLoadSurfaceFromFileA(surf, NULL, NULL, NULL, NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromFile returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXLoadSurfaceFromFileA(surf, NULL, NULL, "", NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXLoadSurfaceFromFile returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);


    /* D3DXLoadSurfaceFromResource */
    todo_wine {
        hr = D3DXLoadSurfaceFromResourceA(surf, NULL, NULL, NULL, MAKEINTRESOURCE(IDB_BITMAP_1x1), NULL, D3DX_DEFAULT, 0, NULL);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromResource returned %#x, expected %#x\n", hr, D3D_OK);
    }

    hr = D3DXLoadSurfaceFromResourceA(surf, NULL, NULL, NULL, MAKEINTRESOURCE(IDD_BITMAPDATA_1x1), NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3D_OK, "D3DXLoadSurfaceFromResource returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXLoadSurfaceFromResourceA(surf, NULL, NULL, NULL, NULL, NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXLoadSurfaceFromResource returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXLoadSurfaceFromResourceA(NULL, NULL, NULL, NULL, MAKEINTRESOURCE(IDB_BITMAP_1x1), NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromResource returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXLoadSurfaceFromResourceA(surf, NULL, NULL, NULL, MAKEINTRESOURCE(IDS_STRING), NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXLoadSurfaceFromResource returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);


    /* D3DXLoadSurfaceFromFileInMemory */
    todo_wine {
        hr = D3DXLoadSurfaceFromFileInMemory(surf, NULL, NULL, bmp01, sizeof(bmp01), NULL, D3DX_DEFAULT, 0, NULL);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromFileInMemory returned %#x, expected %#x\n", hr, D3D_OK);
    }

    hr = D3DXLoadSurfaceFromFileInMemory(surf, NULL, NULL, noimage, sizeof(noimage), NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DXERR_INVALIDDATA, "D3DXLoadSurfaceFromFileInMemory returned %#x, expected %#x\n", hr, D3DXERR_INVALIDDATA);

    hr = D3DXLoadSurfaceFromFileInMemory(surf, NULL, NULL, bmp01, 0, NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXLoadSurfaceFromFileInMemory(NULL, NULL, NULL, bmp01, sizeof(bmp01), NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXLoadSurfaceFromFileInMemory(surf, NULL, NULL, NULL, 8, NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXLoadSurfaceFromFileInMemory(surf, NULL, NULL, NULL, 0, NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXLoadSurfaceFromFileInMemory(NULL, NULL, NULL, NULL, 0, NULL, D3DX_DEFAULT, 0, NULL);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);


    /* D3DXLoadSurfaceFromMemory */
    SetRect(&rect, 0, 0, 2, 2);

    hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata, D3DFMT_A8R8G8B8, sizeof(pixdata), NULL, &rect, D3DX_FILTER_NONE, 0);
    ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata, D3DFMT_A8R8G8B8, 0, NULL, &rect, D3DX_FILTER_NONE, 0);
    ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);

    hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, NULL, D3DFMT_A8R8G8B8, sizeof(pixdata), NULL, &rect, D3DX_DEFAULT, 0);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXLoadSurfaceFromMemory(NULL, NULL, NULL, pixdata, D3DFMT_A8R8G8B8, sizeof(pixdata), NULL, &rect, D3DX_DEFAULT, 0);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata, D3DFMT_A8R8G8B8, sizeof(pixdata), NULL, NULL, D3DX_DEFAULT, 0);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata, D3DFMT_UNKNOWN, sizeof(pixdata), NULL, &rect, D3DX_DEFAULT, 0);
    ok(hr == E_FAIL, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, E_FAIL);

    SetRect(&destrect, -1, -1, 1, 1); /* destination rect is partially outside texture boundaries */
    hr = D3DXLoadSurfaceFromMemory(surf, NULL, &destrect, pixdata, D3DFMT_A8R8G8B8, sizeof(pixdata), NULL, &rect, D3DX_FILTER_NONE, 0);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    SetRect(&destrect, 255, 255, 257, 257); /* destination rect is partially outside texture boundaries */
    hr = D3DXLoadSurfaceFromMemory(surf, NULL, &destrect, pixdata, D3DFMT_A8R8G8B8, sizeof(pixdata), NULL, &rect, D3DX_FILTER_NONE, 0);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    SetRect(&destrect, 1, 1, 0, 0); /* left > right, top > bottom */
    hr = D3DXLoadSurfaceFromMemory(surf, NULL, &destrect, pixdata, D3DFMT_A8R8G8B8, sizeof(pixdata), NULL, &rect, D3DX_FILTER_NONE, 0);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

    SetRect(&destrect, 0, 0, 0, 0); /* left = right, top = bottom */
    hr = D3DXLoadSurfaceFromMemory(surf, NULL, &destrect, pixdata, D3DFMT_A8R8G8B8, sizeof(pixdata), NULL, &rect, D3DX_FILTER_NONE, 0);
    ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);

    SetRect(&destrect, 257, 257, 257, 257); /* left = right, top = bottom, but invalid values */
    hr = D3DXLoadSurfaceFromMemory(surf, NULL, &destrect, pixdata, D3DFMT_A8R8G8B8, sizeof(pixdata), NULL, &rect, D3DX_FILTER_NONE, 0);
    ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);


    /* D3DXLoadSurfaceFromSurface */
    hr = IDirect3DDevice9_CreateOffscreenPlainSurface(device, 256, 256, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &newsurf, NULL);
    if(SUCCEEDED(hr)) {
        hr = D3DXLoadSurfaceFromSurface(newsurf, NULL, NULL, surf, NULL, NULL, D3DX_DEFAULT, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromSurface returned %#x, expected %#x\n", hr, D3D_OK);

        hr = D3DXLoadSurfaceFromSurface(NULL, NULL, NULL, surf, NULL, NULL, D3DX_DEFAULT, 0);
        ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromSurface returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXLoadSurfaceFromSurface(newsurf, NULL, NULL, NULL, NULL, NULL, D3DX_DEFAULT, 0);
        ok(hr == D3DERR_INVALIDCALL, "D3DXLoadSurfaceFromSurface returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        check_release((IUnknown*)newsurf, 0);
    } else skip("Failed to create a second surface\n");

    check_release((IUnknown*)surf, 0);


    /* test color conversion */
    /* A8R8G8B8 */
    hr = IDirect3DDevice9_CreateOffscreenPlainSurface(device, 2, 2, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &surf, NULL);
    if(FAILED(hr)) skip("Failed to create a surface (%#x)\n", hr);
    else {
        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a8r3g3b2, D3DFMT_A8R3G3B2, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        check_pixel_4bpp(lockrect, 0, 0, 0x57dbffff);
        check_pixel_4bpp(lockrect, 1, 0, 0x98ffff00);
        check_pixel_4bpp(lockrect, 0, 1, 0xacdbff55);
        check_pixel_4bpp(lockrect, 1, 1, 0xc8929255);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a1r5g5b5, D3DFMT_A1R5G5B5, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        check_pixel_4bpp(lockrect, 0, 0, 0x008cadad);
        check_pixel_4bpp(lockrect, 1, 0, 0xff317342);
        check_pixel_4bpp(lockrect, 0, 1, 0x0008ad10);
        check_pixel_4bpp(lockrect, 1, 1, 0xff29088c);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_r5g6b5, D3DFMT_R5G6B5, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        check_pixel_4bpp(lockrect, 0, 0, 0xff9cdfb5);
        check_pixel_4bpp(lockrect, 1, 0, 0xff63b26b);
        check_pixel_4bpp(lockrect, 0, 1, 0xff085d73);
        check_pixel_4bpp(lockrect, 1, 1, 0xff425d73);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_g16r16, D3DFMT_G16R16, 8, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        todo_wine {
            check_pixel_4bpp(lockrect, 0, 0, 0xff3f08ff);
        }
        check_pixel_4bpp(lockrect, 1, 0, 0xff44dcff);
        check_pixel_4bpp(lockrect, 0, 1, 0xff97e4ff);
        check_pixel_4bpp(lockrect, 1, 1, 0xfffe9aff);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a8b8g8r8, D3DFMT_A8B8G8R8, 8, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        check_pixel_4bpp(lockrect, 0, 0, 0xc3f04c39);
        check_pixel_4bpp(lockrect, 1, 0, 0x2392e85a);
        check_pixel_4bpp(lockrect, 0, 1, 0x09fd97b1);
        check_pixel_4bpp(lockrect, 1, 1, 0x8df62bc3);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a2r10g10b10, D3DFMT_A2R10G10B10, 8, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        check_pixel_4bpp(lockrect, 0, 0, 0x555c95bf);
        check_pixel_4bpp(lockrect, 1, 0, 0x556d663f);
        check_pixel_4bpp(lockrect, 0, 1, 0xaac385ad);
        todo_wine {
            check_pixel_4bpp(lockrect, 1, 1, 0xfffcc575);
        }
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a8l8,
                D3DFMT_A8L8, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(SUCCEEDED(hr), "Failed to load surface, hr %#x.\n", hr);
        hr = IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        ok(SUCCEEDED(hr), "Failed to lock surface, hr %#x.\n", hr);
        check_pixel_4bpp(lockrect, 0, 0, 0xff000000);
        check_pixel_4bpp(lockrect, 1, 0, 0x00ffffff);
        check_pixel_4bpp(lockrect, 0, 1, 0xff303030);
        check_pixel_4bpp(lockrect, 1, 1, 0x7f7f7f7f);
        hr = IDirect3DSurface9_UnlockRect(surf);
        ok(SUCCEEDED(hr), "Failed to unlock surface, hr %#x.\n", hr);

        check_release((IUnknown*)surf, 0);
    }

    /* A1R5G5B5 */
    hr = IDirect3DDevice9_CreateOffscreenPlainSurface(device, 2, 2, D3DFMT_A1R5G5B5, D3DPOOL_DEFAULT, &surf, NULL);
    if(FAILED(hr)) skip("Failed to create a surface (%#x)\n", hr);
    else {
        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a8r3g3b2, D3DFMT_A8R3G3B2, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        check_pixel_2bpp(lockrect, 0, 0, 0x6fff);
        check_pixel_2bpp(lockrect, 1, 0, 0xffe0);
        check_pixel_2bpp(lockrect, 0, 1, 0xefea);
        check_pixel_2bpp(lockrect, 1, 1, 0xca4a);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a1r5g5b5, D3DFMT_A1R5G5B5, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        check_pixel_2bpp(lockrect, 0, 0, 0x46b5);
        check_pixel_2bpp(lockrect, 1, 0, 0x99c8);
        check_pixel_2bpp(lockrect, 0, 1, 0x06a2);
        check_pixel_2bpp(lockrect, 1, 1, 0x9431);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_r5g6b5, D3DFMT_R5G6B5, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        check_pixel_2bpp(lockrect, 0, 0, 0xcf76);
        check_pixel_2bpp(lockrect, 1, 0, 0xb2cd);
        check_pixel_2bpp(lockrect, 0, 1, 0x856e);
        check_pixel_2bpp(lockrect, 1, 1, 0xa16e);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_g16r16, D3DFMT_G16R16, 8, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        todo_wine {
            check_pixel_2bpp(lockrect, 0, 0, 0xa03f);
        }
        check_pixel_2bpp(lockrect, 1, 0, 0xa37f);
        check_pixel_2bpp(lockrect, 0, 1, 0xcb9f);
        check_pixel_2bpp(lockrect, 1, 1, 0xfe7f);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a8b8g8r8, D3DFMT_A8B8G8R8, 8, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        todo_wine {
            check_pixel_2bpp(lockrect, 0, 0, 0xf527);
            check_pixel_2bpp(lockrect, 1, 0, 0x4b8b);
        }
        check_pixel_2bpp(lockrect, 0, 1, 0x7e56);
        check_pixel_2bpp(lockrect, 1, 1, 0xf8b8);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a2r10g10b10, D3DFMT_A2R10G10B10, 8, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        check_pixel_2bpp(lockrect, 0, 0, 0x2e57);
        todo_wine {
            check_pixel_2bpp(lockrect, 1, 0, 0x3588);
        }
        check_pixel_2bpp(lockrect, 0, 1, 0xe215);
        check_pixel_2bpp(lockrect, 1, 1, 0xff0e);
        IDirect3DSurface9_UnlockRect(surf);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a8l8,
                D3DFMT_A8L8, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(SUCCEEDED(hr), "Failed to load surface, hr %#x.\n", hr);
        hr = IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        ok(SUCCEEDED(hr), "Failed to lock surface, hr %#x.\n", hr);
        check_pixel_2bpp(lockrect, 0, 0, 0x8000);
        check_pixel_2bpp(lockrect, 1, 0, 0x7fff);
        check_pixel_2bpp(lockrect, 0, 1, 0x98c6);
        check_pixel_2bpp(lockrect, 1, 1, 0x3def);
        hr = IDirect3DSurface9_UnlockRect(surf);
        ok(SUCCEEDED(hr), "Failed to unlock surface, hr %#x.\n", hr);

        check_release((IUnknown*)surf, 0);
    }

    /* A8L8 */
    hr = IDirect3DDevice9_CreateOffscreenPlainSurface(device, 2, 2, D3DFMT_A8L8, D3DPOOL_DEFAULT, &surf, NULL);
    if (FAILED(hr))
        skip("Failed to create A8L8 surface, hr %#x.\n", hr);
    else
    {
        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a8r3g3b2,
                D3DFMT_A8R3G3B2, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(SUCCEEDED(hr), "Failed to load surface, hr %#x.\n", hr);
        hr = IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        ok(SUCCEEDED(hr), "Failed to lock surface, hr %#x.\n", hr);
        check_pixel_2bpp(lockrect, 0, 0, 0x57f7);
        check_pixel_2bpp(lockrect, 1, 0, 0x98ed);
        check_pixel_2bpp(lockrect, 0, 1, 0xaceb);
        check_pixel_2bpp(lockrect, 1, 1, 0xc88d);
        hr = IDirect3DSurface9_UnlockRect(surf);
        ok(SUCCEEDED(hr), "Failed to unlock surface, hr %#x.\n", hr);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a1r5g5b5,
                D3DFMT_A1R5G5B5, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(hr == D3D_OK, "D3DXLoadSurfaceFromMemory returned %#x, expected %#x\n", hr, D3D_OK);
        hr = IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        ok(SUCCEEDED(hr), "Failed to lock surface, hr %#x.\n", hr);
        check_pixel_2bpp(lockrect, 0, 0, 0x00a6);
        check_pixel_2bpp(lockrect, 1, 0, 0xff62);
        check_pixel_2bpp(lockrect, 0, 1, 0x007f);
        check_pixel_2bpp(lockrect, 1, 1, 0xff19);
        hr = IDirect3DSurface9_UnlockRect(surf);
        ok(SUCCEEDED(hr), "Failed to unlock surface, hr %#x.\n", hr);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_r5g6b5,
                D3DFMT_R5G6B5, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(SUCCEEDED(hr), "Failed to load surface, hr %#x.\n", hr);
        hr = IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        ok(SUCCEEDED(hr), "Failed to lock surface, hr %#x.\n", hr);
        check_pixel_2bpp(lockrect, 0, 0, 0xffce);
        check_pixel_2bpp(lockrect, 1, 0, 0xff9c);
        check_pixel_2bpp(lockrect, 0, 1, 0xff4d);
        check_pixel_2bpp(lockrect, 1, 1, 0xff59);
        hr = IDirect3DSurface9_UnlockRect(surf);
        ok(SUCCEEDED(hr), "Failed to unlock surface, hr %#x.\n", hr);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_g16r16,
                D3DFMT_G16R16, 8, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(SUCCEEDED(hr), "Failed to load surface, hr %#x.\n", hr);
        hr = IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        ok(SUCCEEDED(hr), "Failed to lock surface, hr %#x.\n", hr);
        check_pixel_2bpp(lockrect, 0, 0, 0xff25);
        check_pixel_2bpp(lockrect, 1, 0, 0xffbe);
        check_pixel_2bpp(lockrect, 0, 1, 0xffd6);
        check_pixel_2bpp(lockrect, 1, 1, 0xffb6);
        hr = IDirect3DSurface9_UnlockRect(surf);
        ok(SUCCEEDED(hr), "Failed to unlock surface, hr %#x.\n", hr);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a8b8g8r8,
                D3DFMT_A8B8G8R8, 8, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(SUCCEEDED(hr), "Failed to load surface, hr %#x.\n", hr);
        hr = IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        ok(SUCCEEDED(hr), "Failed to lock surface, hr %#x.\n", hr);
        check_pixel_2bpp(lockrect, 0, 0, 0xc36d);
        check_pixel_2bpp(lockrect, 1, 0, 0x23cb);
        check_pixel_2bpp(lockrect, 0, 1, 0x09af);
        check_pixel_2bpp(lockrect, 1, 1, 0x8d61);
        hr = IDirect3DSurface9_UnlockRect(surf);
        ok(SUCCEEDED(hr), "Failed to unlock surface, hr %#x.\n", hr);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a2r10g10b10,
                D3DFMT_A2R10G10B10, 8, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(SUCCEEDED(hr), "Failed to load surface, hr %#x.\n", hr);
        hr = IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        ok(SUCCEEDED(hr), "Failed to lock surface, hr %#x.\n", hr);
        check_pixel_2bpp(lockrect, 0, 0, 0x558c);
        check_pixel_2bpp(lockrect, 1, 0, 0x5565);
        check_pixel_2bpp(lockrect, 0, 1, 0xaa95);
        check_pixel_2bpp(lockrect, 1, 1, 0xffcb);
        hr = IDirect3DSurface9_UnlockRect(surf);
        ok(SUCCEEDED(hr), "Failed to unlock surface, hr %#x.\n", hr);

        hr = D3DXLoadSurfaceFromMemory(surf, NULL, NULL, pixdata_a8l8,
                D3DFMT_A8L8, 4, NULL, &rect, D3DX_FILTER_NONE, 0);
        ok(SUCCEEDED(hr), "Failed to load surface, hr %#x.\n", hr);
        hr = IDirect3DSurface9_LockRect(surf, &lockrect, NULL, D3DLOCK_READONLY);
        ok(SUCCEEDED(hr), "Failed to lock surface, hr %#x.\n", hr);
        check_pixel_2bpp(lockrect, 0, 0, 0xff00);
        check_pixel_2bpp(lockrect, 1, 0, 0x00ff);
        check_pixel_2bpp(lockrect, 0, 1, 0xff30);
        check_pixel_2bpp(lockrect, 1, 1, 0x7f7f);
        hr = IDirect3DSurface9_UnlockRect(surf);
        ok(SUCCEEDED(hr), "Failed to unlock surface, hr %#x.\n", hr);

        check_release((IUnknown*)surf, 0);
    }

    /* cleanup */
    if(testdummy_ok) DeleteFileA("testdummy.bmp");
    if(testbitmap_ok) DeleteFileA("testbitmap.bmp");
}

static void test_D3DXCreateCubeTexture(IDirect3DDevice9 *device)
{
    HRESULT hr;
    ULONG ref;
    DWORD levelcount;
    IDirect3DCubeTexture9 *cube_texture;
    D3DSURFACE_DESC surface_desc;

    todo_wine {
        hr = D3DXCreateCubeTextureFromFileInMemory(NULL, dds_cube_map, sizeof(dds_cube_map), &cube_texture);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateCubeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateCubeTextureFromFileInMemory(device, NULL, sizeof(dds_cube_map), &cube_texture);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateCubeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateCubeTextureFromFileInMemory(device, dds_cube_map, 0, &cube_texture);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateCubeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateCubeTextureFromFileInMemory(device, dds_cube_map, sizeof(dds_cube_map), NULL);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateCubeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateCubeTextureFromFileInMemory(device, dds_cube_map, sizeof(dds_cube_map), &cube_texture);
        if(SUCCEEDED(hr)) {
            levelcount = IDirect3DCubeTexture9_GetLevelCount(cube_texture);
            ok(levelcount == 3, "GetLevelCount returned %u, expected 3\n", levelcount);

            hr = IDirect3DCubeTexture9_GetLevelDesc(cube_texture, 0, &surface_desc);
            ok(hr == D3D_OK, "GetLevelDesc returned %#x, expected %#x\n", hr, D3D_OK);
            ok(surface_desc.Width == 4, "Got width %u, expected 4\n", surface_desc.Width);
            ok(surface_desc.Height == 4, "Got height %u, expected 4\n", surface_desc.Height);

            hr = IDirect3DCubeTexture9_GetLevelDesc(cube_texture, 3, &surface_desc);
            ok(hr == D3DERR_INVALIDCALL, "GetLevelDesc returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

            ref = IDirect3DCubeTexture9_Release(cube_texture);
            ok(ref == 0, "Invalid reference count. Got %u, expected 0\n", ref);
        } else skip("Couldn't create cube texture\n");
    }
}

static void test_D3DXCreateVolumeTexture(IDirect3DDevice9 *device)
{
    HRESULT hr;
    ULONG ref;
    DWORD levelcount;
    IDirect3DVolumeTexture9 *volume_texture;
    D3DVOLUME_DESC volume_desc;

    todo_wine {
        hr = D3DXCreateVolumeTextureFromFileInMemory(NULL, dds_volume_map, sizeof(dds_volume_map), &volume_texture);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateVolumeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateVolumeTextureFromFileInMemory(device, NULL, sizeof(dds_volume_map), &volume_texture);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateVolumeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateVolumeTextureFromFileInMemory(device, dds_volume_map, 0, &volume_texture);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateVolumeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateVolumeTextureFromFileInMemory(device, dds_volume_map, sizeof(dds_volume_map), NULL);
        ok(hr == D3DERR_INVALIDCALL, "D3DXCreateVolumeTextureFromFileInMemory returned %#x, expected %#x\n", hr, D3DERR_INVALIDCALL);

        hr = D3DXCreateVolumeTextureFromFileInMemory(device, dds_volume_map, sizeof(dds_volume_map), &volume_texture);
        if(SUCCEEDED(hr)) {
            levelcount = IDirect3DVolumeTexture9_GetLevelCount(volume_texture);
            ok(levelcount == 3, "GetLevelCount returned %u, expected 3\n", levelcount);

            hr = IDirect3DVolumeTexture9_GetLevelDesc(volume_texture, 0, &volume_desc);
            ok(hr == D3D_OK, "GetLevelDesc returend %#x, expected %#x\n", hr, D3D_OK);
            ok(volume_desc.Width == 4, "Got width %u, expected 4\n", volume_desc.Width);
            ok(volume_desc.Height == 4, "Got height %u, expected 4\n", volume_desc.Height);
            ok(volume_desc.Depth == 2, "Got depth %u, expected 2\n", volume_desc.Depth);

            hr = IDirect3DVolumeTexture9_GetLevelDesc(volume_texture, 1, &volume_desc);
            ok(hr == D3D_OK, "GetLevelDesc returned %#x, expected %#x\n", hr, D3D_OK);
            ok(volume_desc.Width == 2, "Got width %u, expected 2\n", volume_desc.Width);
            ok(volume_desc.Height == 2, "Got height %u, expected 2\n", volume_desc.Height);
            ok(volume_desc.Depth == 1, "Got depth %u, expected 1\n", volume_desc.Depth);

            ref = IDirect3DVolumeTexture9_Release(volume_texture);
            ok(ref == 0, "Invalid reference count. Got %u, expected 0\n", ref);
        }
    }
}

START_TEST(surface)
{
    HWND wnd;
    IDirect3D9 *d3d;
    IDirect3DDevice9 *device;
    D3DPRESENT_PARAMETERS d3dpp;
    HRESULT hr;

    wnd = CreateWindow("static", "d3dx9_test", 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
    if (!wnd) {
        skip("Couldn't create application window\n");
        return;
    }
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) {
        skip("Couldn't create IDirect3D9 object\n");
        DestroyWindow(wnd);
        return;
    }

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed   = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    hr = IDirect3D9_CreateDevice(d3d, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &device);
    if(FAILED(hr)) {
        skip("Failed to create IDirect3DDevice9 object %#x\n", hr);
        IDirect3D9_Release(d3d);
        DestroyWindow(wnd);
        return;
    }

    test_D3DXGetImageInfo();
    test_D3DXLoadSurface(device);
    test_D3DXCreateCubeTexture(device);
    test_D3DXCreateVolumeTexture(device);

    check_release((IUnknown*)device, 0);
    check_release((IUnknown*)d3d, 0);
    DestroyWindow(wnd);
}

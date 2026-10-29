#pragma once

#include "Surface.h"
#include <Windows.h>

namespace KaGuYa::Graphics
{
// 原CDIBitmap供用户DIB、Draw及文字工作区共用；各调用者自行决定Ref的发布与失败保留。
class DibBitmap final
{
public:
    struct UnprotectedDrawConstruction {};
    DibBitmap();
    explicit DibBitmap(UnprotectedDrawConstruction);
    static DibBitmap* CreateForDraw();
    virtual ~DibBitmap() noexcept(false);
    DibBitmap(const DibBitmap&) = delete;
    DibBitmap& operator=(const DibBitmap&) = delete;

    bool Create(int width, int height, int depth, unsigned char alpha);
    void Close();

    int bitsPerPixel = 0;
    HBITMAP bitmap = nullptr;
    HDC dc = nullptr;
    RendererReference<SurfaceInfo> info;
};
static_assert(sizeof(DibBitmap) == 0x1C);
}

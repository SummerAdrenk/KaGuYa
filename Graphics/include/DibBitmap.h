#pragma once

#include "Surface.h"
#include <Windows.h>

namespace KaGuYa::Graphics
{
// 原CDIBitmap供用户DIB、Draw及文字工作区共用；各调用者自行决定Ref的发布与失败保留。
// final只禁止派生；原对象的vptr及析构虚槽仍是布局的一部分，不能移除virtual。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunnecessary-virtual-specifier"
#endif
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
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
static_assert(sizeof(DibBitmap) == 0x1C);
}

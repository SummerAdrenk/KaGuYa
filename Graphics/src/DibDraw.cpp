#include "GdiDraw.h"
#include "DibBitmap.h"
#include "SurfaceTransfer.h"

namespace KaGuYa::Graphics
{
DibDraw::DibDraw() { ::new (bitmap_) DibBitmapReference; }
DibDraw::~DibDraw() noexcept(false)
{
    // 原05D80两次退休；第一次异常则没有第二次补偿。
    Close();
    Bitmap().~DibBitmapReference();
}
GdiDraw* DibDraw::Destroy(std::uint32_t flags)
{
    this->~DibDraw();
    if (flags & 1u) ::operator delete(this);
    return this;
}
void DibDraw::Close() { Bitmap().RetireOwned(); }

int DibDraw::Initialize(HWND window, int width, int height)
{
    // 原071EC/0720A两层均无构造失败raw-delete，不能用new表达式补偿外层。
    Bitmap().Publish(DibBitmap::CreateForDraw());
    if (!Bitmap()->Create(width, height, 32, 0))
    {
        Close();
        return 7;
    }
    // 原072C5成功尾才写Draw的逻辑尺寸及窗口。
    width_ = width;
    height_ = height;
    window_ = window;
    return 0;
}
bool DibDraw::Copy(RECT clip, SurfaceInfo& source)
{
    return CopyGdiSurface(*Bitmap()->info, source, &clip) == 0;
}
bool DibDraw::Blt(HDC target, int x, int y, int width, int height, int sx, int sy, DWORD rop)
{
    return BitBlt(target, x, y, width, height, Bitmap()->dc, sx, sy, rop) != FALSE;
}
bool DibDraw::Stretch(HDC target, int x, int y, int width, int height, int sx, int sy, int sw, int sh, DWORD rop)
{
    // 原07429明确为3，非HALFTONE；失败返回也恢复原模式，异常则无补偿finally。
    const int old = SetStretchBltMode(target, COLORONCOLOR);
    const BOOL result = StretchBlt(target, x, y, width, height, Bitmap()->dc, sx, sy, sw, sh, rop);
    SetStretchBltMode(target, old);
    return result != FALSE;
}
}

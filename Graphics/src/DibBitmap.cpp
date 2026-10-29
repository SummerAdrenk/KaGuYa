#include "DibBitmap.h"

#include <bit>
#include <cstdint>

namespace KaGuYa::Graphics
{
DibBitmap::DibBitmap()
{
    // 原06EF0及E6C0内联构造：Info完成后发布独立Ref，Create在body Ref建立之后调用。
    info.Publish(new SurfaceInfo);
}

DibBitmap::DibBitmap(UnprotectedDrawConstruction)
{
    // 原071E0内联路径无raw Info删除保护；普通06EF0构造仍使用上面的P路径。
    void* storage = ::operator new(sizeof(SurfaceInfo));
    auto* created = ::new (storage) SurfaceInfo;
    info.Publish(created);
}

DibBitmap* DibBitmap::CreateForDraw()
{
    // 原071EC/0720A两层U/U；此工厂由真实DibDraw调用，构造失败不删raw外层。
    void* storage = ::operator new(sizeof(DibBitmap));
    return ::new (storage) DibBitmap(UnprotectedDrawConstruction{});
}

DibBitmap::~DibBitmap() noexcept(false) { Close(); info.RetireOwned(); }

void DibBitmap::Close()
{
    // 原06C10/06F80：先DC再bitmap，清有效位；描述字段不因Create失败被统一清空。
    if (dc) { DeleteDC(dc); dc = nullptr; }
    if (bitmap) { DeleteObject(bitmap); bitmap = nullptr; }
    if (info) info->valid = false;
}

bool DibBitmap::Create(const int width, const int height, int depth, const unsigned char alpha)
{
    Close();
    if (alpha) depth = 32;
    if (width <= 0 || height <= 0 || depth < 15) return false;

    BITMAPINFO header{};
    header.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    header.bmiHeader.biWidth = width;
    header.bmiHeader.biHeight = -height;
    header.bmiHeader.biPlanes = 1;
    header.bmiHeader.biBitCount = static_cast<WORD>(depth);
    header.bmiHeader.biCompression = BI_RGB;
    HDC screen = GetDC(nullptr);
    if (!screen) return false;
    void* pixels = nullptr;
    bitmap = CreateDIBSection(screen, &header, DIB_RGB_COLORS, &pixels, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!bitmap) return false;

    bitsPerPixel = depth;
    dc = CreateCompatibleDC(nullptr);
    // 原10007083/91没有DC/Select结果检查，也没有为Win32调用增加异常补偿动作。
    SelectObject(dc, bitmap);
    int bytesPerPixel = 0;
    switch (depth)
    {
    case 15: case 16: bytesPerPixel = 2; break;
    case 24: bytesPerPixel = 3; break;
    case 32: bytesPerPixel = 4; break;
    default: Close(); return false;
    }

    info->pitch = std::bit_cast<int>((static_cast<std::uint32_t>(width) *
        static_cast<std::uint32_t>(bytesPerPixel) + 3u) & ~3u);
    info->width = width;
    info->height = height;
    info->pixelSize = bytesPerPixel;
    info->sourcePixelFormat = depth <= 16 ? 4 : depth == 24 ? 5 : alpha ? 12 : 7;
    info->pixels.Borrow(static_cast<std::uint8_t*>(pixels),
        static_cast<std::size_t>(info->pitch) * static_cast<std::size_t>(height));
    info->valid = true;
    return true;
}
}

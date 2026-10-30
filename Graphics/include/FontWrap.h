#pragma once

#include "NativeObjectReference.h"
#include <Windows.h>
#include <atlstr.h>
#include <cstddef>
#include <cstdint>
#include <new>

namespace KaGuYa::Graphics
{
// 原字体对象保留vptr和析构虚槽；final不表示可以改变0x54字节布局。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunnecessary-virtual-specifier"
#endif
class FontWrap final
{
public:
    FontWrap(const LOGFONTA& source, COLORREF text, unsigned char isVertical);
    virtual ~FontWrap() noexcept(false);
    bool Create();
    bool DrawGlyph(HDC device, std::uint16_t character, int x, int y) noexcept;
    ATL::CStringA& Face() noexcept
    { return *std::launder(reinterpret_cast<ATL::CStringA*>(faceStorage_)); }
    const ATL::CStringA& Face() const noexcept
    { return *std::launder(reinterpret_cast<const ATL::CStringA*>(faceStorage_)); }

    HFONT handle;
    LOGFONTA logFont;
private:
    // 原40B40无本地finally：HFONT清理正常完成之后才销毁face，不能由成员自动展开提前代做。
    alignas(ATL::CStringA) std::byte faceStorage_[sizeof(ATL::CStringA)];
public:
    COLORREF textColor;
    COLORREF backgroundColor;
    unsigned char vertical;
private:
    ATL::CStringA DisplayFace() const;
};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
static_assert(sizeof(FontWrap) == 0x54);
using FontReference = NativeObjectReference<FontWrap, 0x54>;
static_assert(sizeof(FontReference) == 12);
}

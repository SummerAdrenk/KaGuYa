#include "FontWrap.h"
#include <Logging/Logger.h>

#include <cstring>
#include <new>

namespace KaGuYa::Graphics
{
FontWrap::FontWrap(const LOGFONTA& source, const COLORREF text, const unsigned char isVertical)
    : handle(nullptr)
{
    // 原40E50：空CString先建立，选项和LOGFONT随后写入；只有face赋值阶段可能抛分配异常。
    ::new (faceStorage_) ATL::CStringA;
    textColor = text;
    vertical = isVertical;
    backgroundColor = CLR_INVALID;
    std::memcpy(&logFont, &source, sizeof(logFont));
    try { Face() = source.lfFaceName; }
    catch (...) { Face().~CStringT(); throw; }
}

FontWrap::~FontWrap() noexcept(false)
{
    // 原40B40先DeleteObject，返回后才清HFONT并销毁face；不检查BOOL或重试。
    if (handle) { DeleteObject(handle); handle = nullptr; }
    Face().~CStringT();
}

ATL::CStringA FontWrap::DisplayFace() const
{
    const auto& face = Face();
    const int length = face.GetLength();
    if (vertical)
    {
        if (length < 0) AtlThrow(E_INVALIDARG);
        if (length == 0 || face.GetString()[0] != '@') return "@" + face;
    }
    return face;
}

bool FontWrap::Create()
{
    if (handle) { DeleteObject(handle); handle = nullptr; }
    // 原40DE0没有覆盖lstrcpy的临时串展开；正常路径显式退临时串后才进入GDI创建。
    alignas(ATL::CStringA) std::byte storage[sizeof(ATL::CStringA)];
    auto* display = ::new (storage) ATL::CStringA(DisplayFace());
    if (display->GetLength() >= LF_FACESIZE)
    {
        display->~CStringT();
        Logging::Write(Logging::Level::Error, "FontWrap: 拒绝生成后超过LOGFONT容量的字体名；原越界域不模拟成功。");
        return false;
    }
    lstrcpyA(logFont.lfFaceName, display->GetString());
    display->~CStringT();
    handle = CreateFontIndirectA(&logFont);
    return handle != nullptr;
}

bool FontWrap::DrawGlyph(HDC device, const std::uint16_t character, const int x, const int y) noexcept
{
    try
    {
        if (!character) throw 1;
        if (!handle) throw 2;
        const auto previous = SelectObject(device, handle);
        if (!previous) throw 3;
        if (textColor != CLR_INVALID)
        {
            if (backgroundColor == CLR_INVALID) SetBkMode(device, TRANSPARENT);
            else { SetBkMode(device, OPAQUE); SetBkColor(device, backgroundColor); }
        }
        SetTextColor(device, textColor);
        const char bytes[]{static_cast<char>(character), static_cast<char>(character >> 8)};
        const bool result = TextOutA(device, x, y, bytes, (character & 0xFF00u) ? 2 : 1) != FALSE;
        SelectObject(device, previous);
        return result;
    }
    catch (...) { return false; } // 原40BA0仅在正常尾恢复选入字体，异常不补做恢复。
}
}

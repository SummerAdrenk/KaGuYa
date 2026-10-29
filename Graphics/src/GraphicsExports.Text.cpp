#include "GraphicsApi.h"
#include "GraphicsState.h"
#include "Surface.h"
#include "TextRenderer.h"

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>

extern "C" int __cdecl SetTextBasePos(const int x, const int y)
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    return KaGuYa::Graphics::SetTextBasePosition(x, y);
}

extern "C" int __cdecl SetTextFirstName(const char* const name)
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    return static_cast<int>(reinterpret_cast<std::intptr_t>(KaGuYa::Graphics::SetTextName(name, false)));
}

extern "C" int __cdecl SetTextSecondName(const char* const name)
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    return static_cast<int>(reinterpret_cast<std::intptr_t>(KaGuYa::Graphics::SetTextName(name, true)));
}

extern "C" int __cdecl TextInitialize(const char* const text, const int x, const int y)
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    // 原1003B210的catch-all(1003B530/536)将内部1..5等C++异常转为AL0。
    // 不清理或回滚已提交的布局字段；Start据false另抛自己的错误码3。
    try { return KaGuYa::Graphics::InitializeTextLayout(text, x, y) ? 1 : 0; }
    catch (...) { return 0; }
}

extern "C" int __cdecl TextCreateFont(const LOGFONTA* const logFont,
    const unsigned char characterSpacing, const unsigned char lineSpacing,
    const unsigned char renderMode, const unsigned char vertical, const COLORREF textColor,
    const COLORREF shadowColor, const int shadowOffsetX, const int shadowOffsetY,
    const COLORREF outlineColor, const unsigned char outlineWidth)
{
    if (logFont == nullptr)
    {
        return 0;
    }
    KaGuYa::Graphics::TextFontOptions options{};
    std::memcpy(&options.logFont, logFont, sizeof(LOGFONTA));
    options.characterSpacing = characterSpacing;
    options.lineSpacing = lineSpacing;
    options.renderMode = renderMode;
    options.vertical = vertical != 0; // 原10004807机器码80 7D 18 00只比较低BYTE，再setnz。
    options.textColor = textColor;
    options.shadowColor = shadowColor;
    options.shadowOffsetX = shadowOffsetX;
    options.shadowOffsetY = shadowOffsetY;
    options.outlineColor = outlineColor;
    options.outlineWidth = outlineWidth;

    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    return KaGuYa::Graphics::ConfigureTextFont(options) ? 1 : 0;
}

extern "C" int __cdecl GetTextLength()
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    return KaGuYa::Graphics::GetTextGlyphCount();
}

extern "C" RECT* __cdecl GetTextDrawRect(RECT* const rect)
{
    if (rect == nullptr)
    {
        return nullptr;
    }
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    *rect = KaGuYa::Graphics::GetTextBounds();
    return rect;
}

extern "C" int __cdecl TextDraw(const int index, RECT* const drawnRect)
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    return runtime.Screen() != nullptr && KaGuYa::Graphics::DrawTextGlyph(
        *runtime.Screen(), static_cast<unsigned int>(index), true, drawnRect) ? 1 : 0;
}

extern "C" int __cdecl TextDrawAll(RECT* const drawnRect)
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    // 原1003BD20 -> 1003C138：已绘前缀不撤销，内部mode异常不穿过导出。
    try { return runtime.Screen() != nullptr &&
        KaGuYa::Graphics::DrawAllText(*runtime.Screen(), true, drawnRect) ? 1 : 0; }
    catch (...) { return 0; }
}

extern "C" int __cdecl TextBlt(const GraphicsHandle destinationHandle, const int index, RECT* const drawnRect)
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    auto* destination = KaGuYa::Graphics::ResolveSurface(destinationHandle);
    return destination != nullptr && KaGuYa::Graphics::DrawTextGlyph(
        *destination, static_cast<unsigned int>(index), false, drawnRect) ? 1 : 0;
}

extern "C" int __cdecl TextBltAll(const GraphicsHandle destinationHandle, RECT* const drawnRect)
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    auto* destination = KaGuYa::Graphics::ResolveSurface(destinationHandle);
    // 原1003C3E0 -> 1003C7F8同样以AL0结束；不把单字TextBlt也改成此边界。
    try { return destination != nullptr &&
        KaGuYa::Graphics::DrawAllText(*destination, false, drawnRect) ? 1 : 0; }
    catch (...) { return 0; }
}

extern "C" int __cdecl CreateMojiSurface(const unsigned int character)
{
    auto& runtime = KaGuYa::Graphics::GetRuntime();
    std::scoped_lock lock(runtime.mutex);
    using namespace KaGuYa::Graphics;
    auto& manager = RequireSurfaceManager(); // 原1000E5C7保存manager，跨Into/回调不重取或增持。
    alignas(SurfaceReference) std::byte storage[sizeof(SurfaceReference)];
    auto* surface = ::new (storage) SurfaceReference;
    // 原1000E5F1返回之前，输出仅由1003CAA0的s0负责；不得提前给调用者也加自动析构。
    CreateCharacterSurfaceInto(*surface, static_cast<std::uint16_t>(character));
    ReferenceRetirement retirement(*surface); // 原1000E5F6才激活本层Ref展开。
    if (*surface) (void)RegisterSurface(manager, *surface); // 原1000E651使用保存的manager。
    const int result = ToHandle(surface->get());  // 原1000E656登记后重读输出Ref。
    // 原1000E603/1000E676先撤本层EH，再退局部Ref；正常退役抛出不能重退一次。
    retirement.Dismiss();
    surface->RetireLocalControl(); // 原1000E681减缓存C，1000E696重读槽C删除；不清死槽。
    return result;
}

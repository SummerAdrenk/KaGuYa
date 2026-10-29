#include <Runtime/GraphicsTextNativeApi.h>

#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using FontFunction = std::uint8_t (__cdecl*)(const LOGFONTA*, std::uint32_t,
    std::uint32_t, std::uint32_t, std::uint32_t, COLORREF, COLORREF,
    std::int32_t, std::int32_t, COLORREF, std::uint32_t);

GraphicsModuleNativeBody8h* ResolveCapturedModule(
    const volatile YtlReferenceSlotRuntime& slot, std::uintptr_t base)
{
    const auto* control = slot.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
    const auto bias = slot.biasAt04;
    return reinterpret_cast<GraphicsModuleNativeBody8h*>(base + bias * stride);
}
}

GraphicsTextNativeFontParameters* InitializeGraphicsTextFontNativeAt40C6E0(
    GraphicsTextNativeFontParameters& p) noexcept
{
    p.fontAt00.lfHeight = -16;
    p.fontAt00.lfWidth = 0;
    p.fontAt00.lfEscapement = 0;
    p.fontAt00.lfOrientation = 0;
    p.fontAt00.lfWeight = 300;
    const std::uint32_t flags14 = 0x80000000u;
    const std::uint32_t flags18 = 0x30020000u;
    std::memcpy(reinterpret_cast<std::byte*>(&p.fontAt00) + 0x14, &flags14, sizeof(flags14));
    std::memcpy(reinterpret_cast<std::byte*>(&p.fontAt00) + 0x18, &flags18, sizeof(flags18));
    // 18号原PE 40C6E4指向5ADC0C；完整CP932字节，不依赖编译器源字符集。
    ::lstrcpyA(p.fontAt00.lfFaceName, "\x82\x6C\x82\x72 \x83\x53\x83\x56\x83\x62\x83\x4E");
    p.characterSpacingAt3C = 0;
    p.lineSpacingAt3D = 20;
    p.renderModeAt3E = 0;
    p.textColorAt40 = 0x00FFFFFFu;
    p.shadowColorAt44 = 0xFFFFFFFFu;
    p.outlineColorAt48 = 0xFFFFFFFFu;
    p.shadowYAt50 = 0;
    p.shadowXAt4C = 0;
    p.outlineWidthAt54 = 0;
    p.verticalAt55 = 0;
    return &p;
}

GraphicsTextNativeApi::GraphicsTextNativeApi(GraphicsRootOwner& owner,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept
    : owner_(owner), strings_(strings), win32_(win32)
{
}

bool GraphicsTextNativeApi::CaptureModule(GraphicsModuleNativeBody8h*& module)
{
    auto* const root = owner_.CaptureAt4182F0();
    const auto& slot = static_cast<const volatile YtlReferenceSlotRuntime&>(root->moduleAt00);
    const auto base = slot.baseAt00;
    if (!base) return false;
    module = ResolveCapturedModule(slot, base);
    return true;
}

bool GraphicsTextNativeApi::CreateTextFontNativeAt4388B0(
    const GraphicsTextNativeFontParameters* parameters)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return false;
    const auto function = reinterpret_cast<FontFunction>(ResolveGraphicsModuleExportWithCString(
        module, "TextCreateFont", 14, strings_, win32_));
    // 43896C..43899E：名字正常释放后才读调用者正文；保留各byte及DWORD读取顺序。
    const volatile auto& p = *parameters;
    const auto outlineWidth = p.outlineWidthAt54;
    const auto vertical = p.verticalAt55 != 0;
    const auto outlineColor = p.outlineColorAt48;
    const auto shadowY = p.shadowYAt50;
    const auto shadowX = p.shadowXAt4C;
    const auto shadowColor = p.shadowColorAt44;
    const auto textColor = p.textColorAt40;
    const auto renderMode = p.renderModeAt3E;
    const auto lineSpacing = p.lineSpacingAt3D;
    const auto characterSpacing = p.characterSpacingAt3C;
    return function(&parameters->fontAt00, characterSpacing, lineSpacing, renderMode,
        vertical ? 1U : 0U, textColor, shadowColor, shadowX, shadowY,
        outlineColor, outlineWidth) != 0;
}

bool GraphicsTextNativeApi::CreateTextFont(const GraphicsTextFontParameters& parameters)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return false;
    const auto function = reinterpret_cast<FontFunction>(ResolveGraphicsModuleExportWithCString(
        module, "TextCreateFont", 14, strings_, win32_));
    return function(&parameters.font, parameters.characterSpacing, parameters.lineSpacing,
        parameters.renderMode, parameters.vertical ? 1U : 0U, parameters.textColor,
        parameters.shadowColor, parameters.shadowX, parameters.shadowY,
        parameters.outlineColor, parameters.outlineWidth) != 0;
}

bool GraphicsTextNativeApi::InitializeText(const char* textCp932, std::int32_t x, std::int32_t y)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return false;
    using Function = std::uint8_t (__cdecl*)(const char*, std::int32_t, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "TextInitialize", 14, strings_, win32_));
    return function(textCp932, x, y) != 0;
}

RECT* GraphicsTextNativeApi::ReadTextDrawRectNativeAt418A30(RECT* output)
{
    auto* const root = owner_.CaptureAt4182F0();
    const auto& slot = static_cast<const volatile YtlReferenceSlotRuntime&>(root->moduleAt00);
    const auto base = slot.baseAt00;
    // 418A5D..418A7E：先固定base，再清调用者输出，随后才读control/stride/bias。
    std::memset(output, 0, sizeof(*output));
    if (!base) return output;
    auto* const module = ResolveCapturedModule(slot, base);
    using Function = RECT* (__cdecl*)(RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "GetTextDrawRect", 15, strings_, win32_));
    RECT temporary;
    // DLL可返回不同地址；原418B00从返回指针复制，不能改读temporary或容忍null。
    const RECT* const result = function(&temporary);
    std::memcpy(output, result, sizeof(*output));
    return output;
}

RECT GraphicsTextNativeApi::ReadTextDrawRect()
{
    RECT output;
    ReadTextDrawRectNativeAt418A30(&output);
    return output;
}

std::int32_t GraphicsTextNativeApi::GetTextLength()
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return 0;
    using Function = std::int32_t (__cdecl*)();
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "GetTextLength", 13, strings_, win32_));
    return function();
}

void GraphicsTextNativeApi::SetTextBasePosition(std::int32_t x, std::int32_t y)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return;
    using Function = void (__cdecl*)(std::int32_t, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "SetTextBasePos", 14, strings_, win32_));
    function(x, y);
}

bool GraphicsTextNativeApi::BlitAllTextToSurface(GraphicsSurfaceHandle target, RECT* output)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return false;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "TextBltAll", 10, strings_, win32_));
    return function(target, output) != 0;
}

bool GraphicsTextNativeApi::BlitTextGlyphToSurface(GraphicsSurfaceHandle target,
    std::uint32_t index, RECT* output)
{
    GraphicsModuleNativeBody8h* module = nullptr;
    if (!CaptureModule(module)) return false;
    using Function = std::uint8_t (__cdecl*)(GraphicsSurfaceHandle, std::uint32_t, RECT*);
    const auto function = reinterpret_cast<Function>(ResolveGraphicsModuleExportWithCString(
        module, "TextBlt", 7, strings_, win32_));
    return function(target, index, output) != 0;
}
}

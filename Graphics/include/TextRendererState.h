#pragma once

#include "TextRenderer.h"
#include "DibBitmap.h"
#include "DibBitmapReference.h"
#include "FontWrap.h"
#include "GlyphLayout.h"
#include "NativeTextName.h"
#include <stdexcept>

namespace KaGuYa::Graphics
{
struct TextWorkDib
{
    DibBitmapReference reference;
    bool Present() const noexcept { return static_cast<bool>(reference); }
    DibBitmap& Native() const
    {
        if (!reference) throw std::logic_error("Text DIB slot has no body");
        return *reference.get();
    }
    SurfaceInfo& Info() const { return *Native().info; }
    HDC Device() const { return Native().dc; }
    int Width() const { return Info().width; }
    int Height() const { return Info().height; }
};

struct TextState
{
    TextState();
    ~TextState() noexcept(false);
    TextState(const TextState&) = delete;
    TextState& operator=(const TextState&) = delete;

    // 原1000F5C0/1000F270的构造、析构展开表不同；union禁止自动补退当前槽。
    union { FontReference font; };
    union { FontReference oversampledFont; };
    TextFontOptions options{};
    union { GlyphLayout glyphs; };
    union { TextWorkDib baseWork; };
    union { TextWorkDib verticalWork; };
    union { TextWorkDib scaledWork; };
    union { TextWorkDib shadowWork; };
    union { TextWorkDib outlineWork; };
    int baseX;
    int baseY;
    // 原1000F699/69C在两个名字构造成功之后才写基点。
    union { NativeTextName firstName; };
    union { NativeTextName secondName; };
};

// 只处理既有工作区，结果留94；mode由调用者原分派时点决定，不生成结果owner。
bool RenderTextGlyphWork(TextState& state, const GlyphRecord& glyph, unsigned char mode, bool vertical);
}

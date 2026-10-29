#pragma once

#include <Runtime/TextWindowLayerNative.h>
#include <Runtime/TextWindowTextRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 42D5F0/415150/419090直接消费E8h，绝不复制到现代TextWindowTextState再写回。
class TextWindowNativeText final
{
public:
    TextWindowNativeText(TextWindowLayerNativeBodyE8h& body, IGraphicsTextApi& text,
        IGraphicsTextGlyphApi& glyphs, ITextWindowTextServices& services,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    [[nodiscard]] bool PrepareNameAt42D5F0(const YtlReferenceSlotRuntime& name);
    // 原返回来自UnionRect；本函数不将false变成新的文字初始化失败分支。
    BOOL PrepareTextAt415150();
    [[nodiscard]] bool BlitRangeAt419090(std::uint32_t begin, std::uint32_t end);
private:
    TextWindowLayerNativeBodyE8h& body_;
    IGraphicsTextApi& text_;
    IGraphicsTextGlyphApi& glyphs_;
    ITextWindowTextServices& services_;
    IRuntimeStringManagerSource& strings_;
};
}

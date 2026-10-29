#pragma once
#include <Runtime/TextWindowNativeTextServices.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class ITextWindowNativeFormatSource;
struct TextWindowLayerNativeServices;
class IGraphicsTextApi;
class IGraphicsTextGlyphApi;
class ITextWindowTextServices;
class IRuntimeStringManagerSource;
class IScrVmCommandErrorSink;
class TextWindowFormatRuntime;
class TextWindowTextServicesRuntime;
class ScrVmValueTable;

// 服务只借用原来源，不能持有另一个E8h/格式窗口或缓存CParams/文字内容。
struct DrawTextNativeRebuildServices final
{
    DrawTextNativeRebuildServices(ITextWindowNativeFormatSource& source,
        TextWindowLayerNativeServices& window,IGraphicsTextApi& text,IGraphicsTextGlyphApi& glyphs,
        ITextWindowTextServices& layout,IScrVmCommandErrorSink& errors,IRuntimeStringManagerSource& strings) noexcept;
    // 长期生产图不预取E8；4C1650的ESI已由caller固定，执行时才建立该receiver的借用view。
    DrawTextNativeRebuildServices(ITextWindowNativeFormatSource& source,
        TextWindowLayerNativeServices& window,GraphicsTextNativeApi& graphics,const char* const& fontFace,
        IScrVmCommandErrorSink& errors,IRuntimeStringManagerSource& strings,
        std::span<const TextWindowTextDescriptor,2> descriptors=GameTextWindowNativeDescriptors()) noexcept;
    bool RebuildForFixedBody(TextWindowLayerNativeBodyE8h&,std::uint32_t refreshUnread);
    ITextWindowNativeFormatSource& source;
    TextWindowLayerNativeServices& window;
    IGraphicsTextApi& text;
    IGraphicsTextGlyphApi& glyphs;
    IScrVmCommandErrorSink& errors;
    IRuntimeStringManagerSource& strings;
private:
    struct NativeLayout final
    {
        GraphicsTextNativeApi& graphics;
        const char* const& fontFace;
        std::span<const TextWindowTextDescriptor,2> descriptors;
    };
    ITextWindowTextServices* fixedLayout_;
    std::optional<NativeLayout> nativeLayout_;
};
struct DrawTextControlledRebuildServices final
{
    TextWindowFormatRuntime& format;
    TextWindowTextServicesRuntime& layout;
    ScrVmValueTable& values;
};
}

#pragma once
#include <Runtime/TextWindowNativeTextServices.h>
#include <optional>
#include <type_traits>
#include <utility>

namespace KaGuYa::Start::Runtime
{
// 返回帧只拥有借用view的C++寿命，不拥有/复制E8、文字、Params或Graphics状态。
struct SettingDialogNativeApplyTextServices final
{
private:
    std::optional<TextWindowNativeTextServices> nativeLayout_;
public:
    SettingDialogNativeApplyTextServices(TextWindowLayerNativeServices& sourceWindow,
        ITextWindowTextServices& sourceLayout,IGraphicsTextApi& sourceText,IGraphicsTextGlyphApi& sourceGlyphs)noexcept
        :window(sourceWindow),layout(sourceLayout),text(sourceText),glyphs(sourceGlyphs){}
    SettingDialogNativeApplyTextServices(TextWindowLayerNativeServices& sourceWindow,TextWindowLayerNativeBodyE8h& body,
        GraphicsTextNativeApi& graphics,const char* const& fontFace,IScrVmCommandErrorSink& errors,
        std::span<const TextWindowTextDescriptor,2> descriptors=GameTextWindowNativeDescriptors())noexcept
        :nativeLayout_(std::in_place,body,sourceWindow.state,sourceWindow.environment,graphics,fontFace,errors,descriptors),
         window(sourceWindow),layout(*nativeLayout_),text(graphics),glyphs(graphics){}
    // 原Apply两个helper按值传帧；拥有view的副本必须指向自己的optional，而非源帧。
    SettingDialogNativeApplyTextServices(const SettingDialogNativeApplyTextServices& source)noexcept
        :nativeLayout_(source.nativeLayout_),window(source.window),
         layout(nativeLayout_?static_cast<ITextWindowTextServices&>(*nativeLayout_):source.layout),
         text(source.text),glyphs(source.glyphs){}
    SettingDialogNativeApplyTextServices(SettingDialogNativeApplyTextServices&& source)noexcept
        :nativeLayout_(std::move(source.nativeLayout_)),window(source.window),
         layout(nativeLayout_?static_cast<ITextWindowTextServices&>(*nativeLayout_):source.layout),
         text(source.text),glyphs(source.glyphs){}
    SettingDialogNativeApplyTextServices& operator=(const SettingDialogNativeApplyTextServices&)=delete;
    SettingDialogNativeApplyTextServices& operator=(SettingDialogNativeApplyTextServices&&)=delete;
    TextWindowLayerNativeServices& window;
    ITextWindowTextServices& layout;
    IGraphicsTextApi& text;
    IGraphicsTextGlyphApi& glyphs;
};
static_assert(std::is_nothrow_copy_constructible_v<TextWindowNativeTextServices>);
static_assert(std::is_nothrow_move_constructible_v<TextWindowNativeTextServices>);
}

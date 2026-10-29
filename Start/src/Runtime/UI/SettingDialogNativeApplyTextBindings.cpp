#include <Runtime/SettingDialogNativeApplyTextBindings.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SettingDialogNativeApplyTextBindings::SettingDialogNativeApplyTextBindings(TextWindowLayerNativeBodyE8h& body,
    TextWindowLayerNativeServices& window,GraphicsTextNativeApi& graphics,const char* const& face,
    IScrVmCommandErrorSink& errors,std::span<const TextWindowTextDescriptor,2> descriptors) noexcept
    :SettingDialogNativeApplyTextBindings(std::optional<std::reference_wrapper<TextWindowLayerNativeBodyE8h>>{std::ref(body)},
        window,graphics,face,errors,descriptors){}
SettingDialogNativeApplyTextBindings::SettingDialogNativeApplyTextBindings(TextWindowLayerNativeServices& window,
    GraphicsTextNativeApi& graphics,const char* const& face,IScrVmCommandErrorSink& errors,
    std::span<const TextWindowTextDescriptor,2> descriptors)noexcept
    :SettingDialogNativeApplyTextBindings(std::nullopt,window,graphics,face,errors,descriptors){}
SettingDialogNativeApplyTextBindings::SettingDialogNativeApplyTextBindings(
    std::optional<std::reference_wrapper<TextWindowLayerNativeBodyE8h>> fixedBody,
    TextWindowLayerNativeServices& window,GraphicsTextNativeApi& graphics,const char* const& face,
    IScrVmCommandErrorSink& errors,std::span<const TextWindowTextDescriptor,2> descriptors)noexcept
    :body_(fixedBody),window_(window),graphics_(graphics),fontFace_(face),errors_(errors),descriptors_(descriptors)
{
    if(body_)layout_.emplace(body_->get(),window.state,window.environment,graphics,face,errors,descriptors);
}
SettingDialogNativeApplyTextServices SettingDialogNativeApplyTextBindings::ForFixedBody(TextWindowLayerNativeBodyE8h& body)
{
    if(body_)
    {
        // 固定域错装配仍明确拒绝，不切换到另一代Root或动态域。
        if(&body!=&body_->get())throw std::logic_error("Setting Apply text bindings belong to a different native E8 body");
        return {window_,*layout_,graphics_,graphics_};
    }
    return {window_,body,graphics_,fontFace_,errors_,descriptors_};
}
}

#pragma once
#include <Runtime/SettingDialogNativeApply.h>
#include <Runtime/TextWindowNativeTextServices.h>
#include <functional>
#include <optional>

namespace KaGuYa::Start::Runtime
{
// 固定域只接受指定E8；调用期域不预建正文，直接绑定ForFixedBody已选定的receiver。
// 两者都不另取Root，所有返回帧的借用必须短于真实正文及外部服务。
class SettingDialogNativeApplyTextBindings final : public ISettingDialogNativeApplyTextBindings
{
public:
    SettingDialogNativeApplyTextBindings(TextWindowLayerNativeBodyE8h& body,TextWindowLayerNativeServices& window,
        GraphicsTextNativeApi& graphics,const char* const& fontFace,IScrVmCommandErrorSink& errors,
        std::span<const TextWindowTextDescriptor,2> descriptors=GameTextWindowNativeDescriptors()) noexcept;
    SettingDialogNativeApplyTextBindings(TextWindowLayerNativeServices& window,GraphicsTextNativeApi& graphics,
        const char* const& fontFace,IScrVmCommandErrorSink& errors,
        std::span<const TextWindowTextDescriptor,2> descriptors=GameTextWindowNativeDescriptors())noexcept;
    // nullopt显式选择调用期域，不是固定域失败后的兜底。
    SettingDialogNativeApplyTextBindings(std::optional<std::reference_wrapper<TextWindowLayerNativeBodyE8h>> fixedBody,
        TextWindowLayerNativeServices& window,GraphicsTextNativeApi& graphics,const char* const& fontFace,
        IScrVmCommandErrorSink& errors,
        std::span<const TextWindowTextDescriptor,2> descriptors=GameTextWindowNativeDescriptors())noexcept;
    SettingDialogNativeApplyTextServices ForFixedBody(TextWindowLayerNativeBodyE8h&) override;
private:
    std::optional<std::reference_wrapper<TextWindowLayerNativeBodyE8h>> body_;
    TextWindowLayerNativeServices& window_;
    GraphicsTextNativeApi& graphics_;
    const char* const& fontFace_;
    IScrVmCommandErrorSink& errors_;
    std::span<const TextWindowTextDescriptor,2> descriptors_;
    std::optional<TextWindowNativeTextServices> layout_;
};
}

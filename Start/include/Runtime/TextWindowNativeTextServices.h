#pragma once

#include <Runtime/GraphicsTextNativeApi.h>
#include <Runtime/TextWindowLayerNative.h>
#include <Runtime/TextWindowTextRuntime.h>

#include <span>

namespace KaGuYa::Start::Runtime
{
// 18号601188/6011C8；固定两项，不与602198的TextDisplay字体表混用。
std::span<const TextWindowTextDescriptor, 2> MamatomoTextWindowNativeDescriptors() noexcept;
std::span<const TextWindowTextDescriptor, 2> GameTextWindowNativeDescriptors() noexcept;

// 只借E8h及各来源；没有第二份文字状态、Save、CMesWin或Graphics快照。
// fontFace是进程字体指针槽的借用；其619824初始化/更新属于上层装配，不硬编码在服务内。
class TextWindowNativeTextServices final : public ITextWindowTextServices
{
public:
    TextWindowNativeTextServices(TextWindowLayerNativeBodyE8h& body,
        IMessageWindowNativeStateSource& state, EnvironmentRootOwner& environment,
        GraphicsTextNativeApi& graphics, const char* const& fontFace,
        IScrVmCommandErrorSink& errors,
        std::span<const TextWindowTextDescriptor, 2> descriptors = GameTextWindowNativeDescriptors()) noexcept;
    const TextWindowTextDescriptor* ReadDescriptor() override;
    bool PrepareFont(const TextWindowTextFontDescriptor& record, std::uint32_t color,
        const std::uint32_t* shadowOverride) override;
    std::uint32_t ReadBodyColor() override;
    const std::uint32_t* ReadNameColorOverride() override;
    const std::uint32_t* ReadNameShadowOverride() override;
private:
    TextWindowLayerNativeBodyE8h& body_;
    IMessageWindowNativeStateSource& state_;
    EnvironmentRootOwner& environment_;
    GraphicsTextNativeApi& graphics_;
    const char* const& fontFace_;
    IScrVmCommandErrorSink& errors_;
    std::span<const TextWindowTextDescriptor, 2> descriptors_;
};
}

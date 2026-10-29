#pragma once

#include <Runtime/TextWindowTextRuntime.h>

namespace KaGuYa::Start::Runtime
{
class TextWindowLayerRuntime;
class IMessageWindowLayerState;
class DrawManagerLayerRuntime;
class SaveDataEnvironmentArchive;

// 4172D0/414BD0/418980 的具体状态源。全部引用必须保持有效；fontFaceCp932
// 对应原全局 lpString2 的实时值，不是另设字体默认值。根 owner 尚须装配该来源。
class TextWindowTextServicesRuntime final : public ITextWindowTextServices
{
public:
    TextWindowTextServicesRuntime(const TextWindowLayerRuntime& owner,
        IMessageWindowLayerState& selection, const DrawManagerLayerRuntime& layers,
        const SaveDataEnvironmentArchive& save, IGraphicsTextApi& graphics,
        const std::string& fontFaceCp932,
        std::span<const TextWindowTextDescriptor, 2> descriptors = GameTextWindowNativeDescriptors()) noexcept;

    [[nodiscard]] const TextWindowTextDescriptor* ReadDescriptor() override;
    [[nodiscard]] bool PrepareFont(const TextWindowTextFontDescriptor& record,
        std::uint32_t color, const std::uint32_t* shadowOverride) override;
    [[nodiscard]] std::uint32_t ReadBodyColor() override;
    [[nodiscard]] const std::uint32_t* ReadNameColorOverride() override;
    [[nodiscard]] const std::uint32_t* ReadNameShadowOverride() override;
    [[nodiscard]] TextWindowTextServicesRuntime ForWindow(const TextWindowLayerRuntime& owner) const noexcept;

private:
    const TextWindowLayerRuntime& owner_;
    IMessageWindowLayerState& selection_;
    const DrawManagerLayerRuntime& layers_;
    const SaveDataEnvironmentArchive& save_;
    IGraphicsTextApi& graphics_;
    const std::string& fontFace_;
    std::span<const TextWindowTextDescriptor, 2> descriptors_;
};
}

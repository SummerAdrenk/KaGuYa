#pragma once
#include <Platform/InputViews.h>

#include <Runtime/PreSceneEngineUpdateRuntime.h>

namespace KaGuYa::Start::Platform { class IInputRuntime; class MouseView; }
namespace KaGuYa::Start::Runtime
{
class EnvironmentRegistryRuntime;

// 46D420前半段的真实来源；registry与Input集合均借用，不缓存其可替换正文。
class EnvironmentPreSceneUpdateState final : public IPreSceneEngineUpdateState
{
public:
    EnvironmentPreSceneUpdateState(EnvironmentRegistryRuntime& registry,
        Platform::IInputRuntime& input) noexcept;

    std::uint32_t ConfigDescriptor2Field14() const override;
    void MaskConfigDescriptor2Field14(std::uint32_t mask) override;
    bool InputDescriptor0Byte13() const override;
    bool InputDescriptor0Byte14() const override;
    bool InputDescriptor2Action(std::uint32_t action) const override;
    void SetInputDescriptor0Byte14(std::uint8_t value) override;
    void SetInputDescriptor0Byte13(std::uint8_t value) override;
    void SetInputDescriptor0Field28(std::uint32_t value) override;
    bool ConfigDescriptor1Field38Bit20() const override;

private:
    Platform::MouseView CaptureMouse() const;
    EnvironmentRegistryRuntime& registry_;
    Platform::IInputRuntime& input_;
};
}

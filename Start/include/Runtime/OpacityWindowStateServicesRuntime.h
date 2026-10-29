#pragma once

#include <Runtime/OpacityDialogObjectGraphRuntime.h>

namespace KaGuYa::Start::Runtime
{
class SaveDataEnvironmentArchive;
class DrawManagerLayerRuntime;
class MessageWindowLayerRuntime;
struct MessageWindowLayerNativeBody50h;
class EnvironmentStartupRuntime;
class EnvironmentPostWindowStartupRuntime;
class RootRuntimeOwner;

// 50D730 / 52DF20 均查询当前 manager[1]，不缓存 selector 或 Save 副本。
class OpacityWindowStateServicesRuntime final
    : public IOpacityDialogResetStateServices, public IOpacityDialogUpdateServices
{
public:
    OpacityWindowStateServicesRuntime(DrawManagerLayerRuntime& layers,
        SaveDataEnvironmentArchive& save) noexcept;
    OpacityWindowStateServicesRuntime(EnvironmentPostWindowStartupRuntime& root,
        EnvironmentStartupRuntime& environment) noexcept;
    OpacityWindowStateServicesRuntime(RootRuntimeOwner& root,
        EnvironmentStartupRuntime& environment) noexcept;
    bool SelectMessageWindowOpacitySelectorNonZeroAt50D730() const override;
    bool SelectMessageWindowOpacitySelectorNonZeroAt52DF20() const override;
    std::int32_t ReadSaveSlot1OpacitySelectorZeroOrNonZero(bool nonzero) const override;
    void WriteSaveSlot1OpacitySelectorZeroOrNonZero(bool nonzero, std::int32_t value) override;
    void RefreshMessageWindowStyleAt5325F0(std::int32_t selector) override;
private:
    [[nodiscard]] MessageWindowLayerRuntime& ModernMessageWindow() const;
    [[nodiscard]] MessageWindowLayerNativeBody50h& NativeMessageWindow() const;
    [[nodiscard]] EnvironmentPostWindowStartupRuntime& CaptureNativePost() const;
    [[nodiscard]] bool SelectedNonzero() const;
    [[nodiscard]] SaveDataEnvironmentArchive& Save() const;
    EnvironmentPostWindowStartupRuntime* root_ = nullptr;
    RootRuntimeOwner* owner_ = nullptr;
    EnvironmentStartupRuntime* environment_ = nullptr;
    DrawManagerLayerRuntime* layers_ = nullptr;
    SaveDataEnvironmentArchive* save_ = nullptr;
};
}

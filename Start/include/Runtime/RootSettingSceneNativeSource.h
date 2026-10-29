#pragma once
#include <Runtime/SettingSceneNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
struct RootSettingSceneNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ISettingDialogNativeConstruction& dialog;
};
// 只补原来源并转发父/四页元数据；完整进程及模态验证仍独立保留。
class RootSettingSceneNativeSource final : public ISettingSceneNativeSource
{
public:
    explicit RootSettingSceneNativeSource(RootSettingSceneNativeDependencies d) noexcept : dependencies_(d) {}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override;
    YtlReferenceSlotRuntime& RootSceneManagerSlotAt30(void* fixedRoot) override;
    void PopThenActivatePendingAt4C1760(void* nativeManager48h) override;
    GuiDialogNativeBody5Ch* ConstructSettingDialogAt4F1BF0(void* storageACh) override;
    SettingDialogNativeRuntimeServices* NativeRuntimeServices() const noexcept override
    {return dependencies_.dialog.NativeRuntimeServices();}
private:
    RootSettingSceneNativeDependencies dependencies_;
};
}

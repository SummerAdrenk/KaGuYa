#pragma once
#include <Runtime/RootSettingDialogNativeApplySource.h>
#include <Runtime/SettingDialogNativeApplyTextBindings.h>
#include <Runtime/RootDrawServicesRuntime.h>
#include <Runtime/RootArchiveResourceSource.h>
#include <Runtime/SoundManagerNativeOpen.h>
#include <functional>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class NativeSoundRuntimeLibrary;
struct RootSettingDialogNativeApplyDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    NativeGraphicsRuntimeLibrary& graphics;
    NativeSoundRuntimeLibrary& sound;
    RootDrawServicesRuntime& draws;
    // nullopt显式选择调用期绑定，不是固定域失败后的兜底。
    std::optional<std::reference_wrapper<TextWindowLayerNativeBodyE8h>> textBody=std::nullopt;
    const char* const& fontFace;
    IScrVmCommandErrorSink& errors;
    decltype(&::MessageBoxA) messageBox=&::MessageBoxA;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
    IRuntimeWideStringNativeApi& wide=DefaultRuntimeWideStringNativeApi();
};
// 稳定服务集合不捕获/创建Root，也不拥有AC/E8/50h或Snapshot。
// caller须在本集合退役前先退役所有使用它的业务对象。
class RootSettingDialogNativeApplyRuntime final
{
public:
    explicit RootSettingDialogNativeApplyRuntime(RootSettingDialogNativeApplyDependencies) noexcept;
    RootSettingDialogNativeApplyRuntime(const RootSettingDialogNativeApplyRuntime&)=delete;
    RootSettingDialogNativeApplyRuntime& operator=(const RootSettingDialogNativeApplyRuntime&)=delete;
    SettingDialogNativeApplyServices& Services() noexcept{return services_;}
    void ApplyAt51CFC0(SettingDialogNativeBodyACh&);
private:
    RootSettingDialogNativeApplySource source_;
    RootArchiveResourceSource resources_;
    SoundManagerNativeOpen open_;
    SoundManagerNativePlayback playback_;
    SoundManagerNativeMusic music_;
    SoundManagerNativeEffects effects_;
    SettingDialogNativeApplyTextBindings text_;
    SettingDialogNativeApplyServices services_;
};
}

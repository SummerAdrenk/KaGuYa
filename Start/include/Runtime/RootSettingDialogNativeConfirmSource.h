#pragma once
#include <Runtime/SettingDialogNativeConfirm.h>
#include <Runtime/NativeSerialize18.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
// 构造只借来源；不提前Capture、发布根或创建默认归档。
class RootSettingDialogNativeConfirmSource final : public ISettingDialogNativeConfirmSource
{
public:
    RootSettingDialogNativeConfirmSource(RootRuntimeOwner&,EnvironmentRootOwner&,ParamsRuntimeRootOwner&,
        NativeSerialize18Services&,decltype(&::PostMessageA)=&::PostMessageA) noexcept;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t) override;
    ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t) override;
    void EnsureRootAt413BE0(std::uint32_t) override;
    void* RootAt6198C4(std::uint32_t) override;
    YtlReferenceSlotRuntime& RootSystemSlotAt3C(void*) override;
    BOOL PostMessageA(HWND,UINT,WPARAM,LPARAM) override;
    const void* SerializeMethodsAt5B22F4() override;
    const void* ISerializeMethods() override;
    void SerializeArchiveAtVslot00(void*,SettingDialogNativeConfirmSerialize18h&,std::uint32_t) override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    ParamsRuntimeRootOwner& params_;
    NativeSerialize18Services& serialization_;
    decltype(&::PostMessageA) post_;
};
}

#pragma once
#include <Runtime/LogSceneNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class RootLogSceneNativeSource final:public ILogSceneNativeSource
{
public:
    RootLogSceneNativeSource(RootRuntimeOwner& root,EnvironmentRootOwner& environment,ILogDialogNativeConstruction& dialog) noexcept
        :root_(root),environment_(environment),dialog_(dialog){}
    GuiDialogNativeBody5Ch* ConstructLogDialogAt4E23D0(void*) override;
    LogDialogNativeServices* NativeDialogServices() const noexcept override{return dialog_.NativeDialogServices();}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t) override;
    void EnsureRootAt413BE0(std::uint32_t) override;
    void* RootAt6198C4(std::uint32_t) override;
    std::uint8_t RootByteAt218(void*) override;
    std::uint8_t RootByteAt219(void*) override;
    void WriteRootByteAt218(void*,std::uint8_t) override;
    void WriteRootByteAt219(void*,std::uint8_t) override;
    YtlReferenceSlotRuntime& RootSceneManagerSlotAt30(void*) override;
    void PopThenActivatePendingAt4C1760(void*) override;
private:
    RootRuntimeOwner& root_;EnvironmentRootOwner& environment_;ILogDialogNativeConstruction& dialog_;
};
}

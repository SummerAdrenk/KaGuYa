#pragma once
#include <Runtime/TitleDialogNative.h>
#include <Runtime/ReadScriptInputNative.h>

namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;
class ParamsRuntimeRootOwner;
struct QuickOperationsNativeServices;
class IModalDialogRunnerServices;
class ITitleNameRegistrationNativeConstruction
{
public:
    virtual ~ITitleNameRegistrationNativeConstruction()=default;
    virtual void* ConstructAt4E4340(void* storage94h)=0;
    virtual NameRegistrationDialogNativeServices* NativeServices() const noexcept{return nullptr;}
};
class TitleNameRegistrationNativeConstruction final:public ITitleNameRegistrationNativeConstruction
{
public:
    explicit TitleNameRegistrationNativeConstruction(NameRegistrationDialogNativeServices& s) noexcept:services_(s){}
    void* ConstructAt4E4340(void* storage) override{return ConstructNameRegistrationDialogNativeAt4E4340(storage,services_);}
    NameRegistrationDialogNativeServices* NativeServices() const noexcept override{return &services_;}
private:
    NameRegistrationDialogNativeServices& services_;
};
struct TitleDialogNativeCueNames final
{
    const RuntimeCString& titleBgmAt61A654;
    const RuntimeCString& confirmAt61A658;
    const RuntimeCString& hoverAt61A65C;
};
struct RootTitleDialogNativeDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    InputRootOwner& input;
    NativeGraphicsRuntimeLibrary& graphics;
    IGuiControlResourceLoadServices& pictures;
    QuickOperationsNativeServices& quick;
    IModalDialogRunnerServices& modal;
    ITitleNameRegistrationNativeConstruction& confirmation;
    TitleDialogNativeCueNames cues;
    IScrVmCommandErrorSink& scriptErrors;
    ScriptNamedValueNativeTables names;
    IRuntimeStringManagerSource& strings;
    decltype(&::PostMessageA) postMessage=&::PostMessageA;
    TitleButtonResources resources = TitleButtonResources::Mamatomo;
    ParamsRuntimeRootOwner* params = nullptr;
    const HINSTANCE* resourceInstance = nullptr;
    const TitleResourceProfile* resourceProfile = nullptr;
};
// 无业务镜像；固定opaque Root身份只通过其唯一槽accessor取Ref，getter不Capture。
// quick/94h construction是明确外部依赖，不在失败时换modern/shared对象。
class RootTitleDialogNativeSource final:public ITitleDialogNativeSource
{
public:
    explicit RootTitleDialogNativeSource(RootTitleDialogNativeDependencies) noexcept;
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override;
    YtlReferenceSlotRuntime& RootDrawSlotAt18(void*) override;
    YtlReferenceSlotRuntime& RootSoundSlotAt24(void*) override;
    YtlReferenceSlotRuntime& RootScenesSlotAt30(void*) override;
    void* SystemAt4180C0() override;
    void* RegistryAt417990(void*,std::int32_t) override;
    void* SoundParamsAt4188C0(void*) override;
    const std::int32_t* BgmVolumeAtVslot14(void*) override;
    const std::int32_t* EffectVolumeAtVslot24(void*) override;
    const char* TitleBgmAt61A654() override;
    const char* ConfirmSoundAt61A658() override;
    const char* HoverSoundAt61A65C() override;
    std::uint8_t LoadSurfaceAt480000(void*,YtlReferenceSlotRuntime*,const char*) override;
    std::uint8_t InputAt4C3430(void*) override;
    std::uint8_t InputAt4AA970(void*) override;
    std::uint8_t InputAt47F6D0(void*) override;
    std::uint8_t LoadAvailableAt485CE0(void*) override;
    std::uint8_t PlayBgmAt42BFE0(void*,const char*,std::int32_t,std::int32_t) override;
    std::uint8_t ClearSoundAt4384D0(void*) override;
    void StopSoundAt413270(void*,EnvironmentVectorStorage<RuntimeCString>*) override;
    void* SoundAt4188F0(void*) override;
    std::uint8_t StartSoundAt4BCD70(void*,const char*,std::int32_t,std::int32_t,std::uint8_t) override;
    void* ObjectAt50A620(void*) override;
    void* CurrentSceneAt4186D0(void*) override;
    void* ConstructConfirmationAt4E4340(void*) override;
    void* PrepareModalAt4AD230(void*) override;
    std::int32_t RunModalAt5071D0(void*,const YtlReferenceSlotRuntime*) override;
    std::int32_t PostMessageAt522D2F(void*,std::uint32_t,std::uint32_t,std::int32_t) override;
    void* GraphicsAt4182F0() override;
    void CommitAt4B02E0(void*,std::int32_t) override;
    NameRegistrationDialogNativeServices* NameRegistrationServices() const noexcept override
    {return dependencies_.confirmation.NativeServices();}
    std::int32_t RunResourceNamePrompt() override;
    std::uint8_t NameEditFlags() override;
private:
    RootTitleDialogNativeDependencies dependencies_;
};
}

#pragma once
#include <Runtime/SoundManagerNativeRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;class EnvironmentRootOwner;
// 设备变化接收者不增持/替换50h正文；GUID以原16字节值传递。
class ISoundDeviceRebuildNative
{
public:
    virtual ~ISoundDeviceRebuildNative()=default;
    virtual void RebuildAt432C20(SoundManagerNativeBody50h* selected,GUID)=0;
};
class ISoundDeviceRebuildNativeSource
{
public:
    virtual ~ISoundDeviceRebuildNativeSource()=default;
    virtual void EnsureRootAt413BE0()=0;
    virtual void* RootAt6198C4()=0;
    virtual const YtlReferenceSlotRuntime& SoundManagerAt24(void*)=0;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0()=0;
    virtual void* GameSystemAt418480()=0;
    virtual int MessageBoxA(HWND,const char*,const char*,UINT)=0;
};
class ISoundDeviceRebuildNativeCalls
{
public:
    virtual ~ISoundDeviceRebuildNativeCalls()=default;
    virtual void StopAbsoluteAt4383B0(SoundManagerNativeBody50h*,DWORD)=0;
    virtual void StopEffectsAt413270(SoundManagerNativeBody50h*)=0;
    virtual BYTE ResetDeviceAt432C99(HWND)=0;
    virtual void SetGlobalFocusAt432D45()=0;
    virtual void InitializeHandlesAt42CA60(SoundManagerNativeBody50h*)=0;
    virtual bool PlayBgmAt42BFE0(SoundManagerNativeBody50h*,const char*,std::int32_t,std::int32_t)=0;
    virtual bool RestoreFirstAt42C1B0(SoundManagerNativeBody50h*,const char*,DWORD,BYTE)=0;
    virtual bool RestoreSecondAt42C070(SoundManagerNativeBody50h*,const char*,DWORD,std::int32_t)=0;
};
void RebuildSoundDeviceNativeAt432C20(SoundManagerNativeBody50h*,GUID,
    ISoundDeviceRebuildNativeSource&,ISoundDeviceRebuildNativeCalls&);
// 与MainFrame同Root/Env/Params/唯一Sound服务；构造只借用，不预建manager或捕获根。
class RootSoundDeviceRebuildNativeRuntime final:public ISoundDeviceRebuildNative,
    private ISoundDeviceRebuildNativeSource,private ISoundDeviceRebuildNativeCalls
{
public:
    RootSoundDeviceRebuildNativeRuntime(RootRuntimeOwner&,EnvironmentRootOwner&,ParamsRuntimeRootOwner&,
        SoundManagerNativeRuntime&)noexcept;
    void RebuildAt432C20(SoundManagerNativeBody50h*,GUID)override;
private:
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    const YtlReferenceSlotRuntime& SoundManagerAt24(void*)override;
    EnvironmentRootStorage* EnvironmentAt4180C0()override;
    void* GameSystemAt418480()override;
    int MessageBoxA(HWND,const char*,const char*,UINT)override;
    void StopAbsoluteAt4383B0(SoundManagerNativeBody50h*,DWORD)override;
    void StopEffectsAt413270(SoundManagerNativeBody50h*)override;
    BYTE ResetDeviceAt432C99(HWND)override;
    void SetGlobalFocusAt432D45()override;
    void InitializeHandlesAt42CA60(SoundManagerNativeBody50h*)override;
    bool PlayBgmAt42BFE0(SoundManagerNativeBody50h*,const char*,std::int32_t,std::int32_t)override;
    bool RestoreFirstAt42C1B0(SoundManagerNativeBody50h*,const char*,DWORD,BYTE)override;
    bool RestoreSecondAt42C070(SoundManagerNativeBody50h*,const char*,DWORD,std::int32_t)override;
    RootRuntimeOwner& root_;EnvironmentRootOwner& environment_;ParamsRuntimeRootOwner& params_;
    SoundManagerNativeRuntime& sounds_;
};
}

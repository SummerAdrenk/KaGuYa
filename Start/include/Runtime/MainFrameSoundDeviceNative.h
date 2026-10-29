#pragma once
#include <Runtime/MainFrameNativeDispatch.h>
#include <Runtime/SoundDeviceRebuildNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class NativeSoundRuntimeLibrary;
// 424070外层只借用来源；Default返回的原指针一直保留到424208，不增持或提前复制。
class IMainFrameSoundDeviceNativeSource
{
public:
    virtual ~IMainFrameSoundDeviceNativeSource()=default;
    virtual const GUID* DefaultDeviceAt4240A9()=0;
    virtual void EnsureRootAt413BE0()=0;
    virtual void* RootAt6198C4()=0;
    virtual YtlReferenceSlotRuntime& ManagerRefAt24(void* selectedRoot)=0;
};
// 依赖须长于本Source；getter仅投影已选根，不另行Ensure或取得现代安全receiver。
class RootMainFrameSoundDeviceNativeSource final:public IMainFrameSoundDeviceNativeSource
{
public:
    RootMainFrameSoundDeviceNativeSource(RootRuntimeOwner&,NativeSoundRuntimeLibrary&)noexcept;
    const GUID* DefaultDeviceAt4240A9()override;
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    YtlReferenceSlotRuntime& ManagerRefAt24(void*)override;
private:
    RootRuntimeOwner& root_;
    NativeSoundRuntimeLibrary& library_;
};
// 原第四栈参数BOOL*未读写，由Main的terminal适配层保留；这里只实现424070业务叶。
// 432C20独立服务消费已选50h和16字节GUID值；本叶不实现、登记或改动其内层。
LRESULT DeviceChangeMainFrameNativeAt424070(MainFrameNativeBody90h&,UINT,WPARAM,LPARAM,
    IMainFrameSoundDeviceNativeSource&,IMainFrameNativeWindowApi&,ISoundDeviceRebuildNative&);
}

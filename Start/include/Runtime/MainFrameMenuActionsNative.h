#pragma once
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/SceneControllerNative.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
struct ToolWindowNativeBody40h;
struct ToolWindowNativeInteractionServices;

// getter只投影已选根；Ensure与6198C4读取分开，不增持Ref或缓存跨回调receiver。
class IMainFrameMenuActionsNativeSource
{
public:
    virtual ~IMainFrameMenuActionsNativeSource()=default;
    virtual EnvironmentRootStorage* EnvironmentAt4180C0()=0;
    virtual void EnsureRootAt413BE0()=0;
    virtual void* RootAt6198C4()=0;
    virtual const YtlReferenceSlotRuntime& DrawAt18(void* selectedRoot)=0;
    virtual const YtlReferenceSlotRuntime& ScenesAt30(void* selectedRoot)=0;
    virtual void SetToolModeAt434FB0(ToolWindowNativeBody40h*,std::int32_t)=0;
    virtual void PushSceneTaskAt40B050(EnvironmentVectorStorage<YtlReferenceSlotRuntime>&,TaskNativeBody4h*)=0;
};

// 必须借入真实服务，构造不取根、不选择固定profile；依赖须长于source及调用期。
// Tool服务内部仍须逐点取得当前根/Env；不能装配固定快照或现代安全receiver。
class RootMainFrameMenuActionsNativeSource final:public IMainFrameMenuActionsNativeSource
{
public:
    RootMainFrameMenuActionsNativeSource(RootRuntimeOwner&,EnvironmentRootOwner&,
        ToolWindowNativeInteractionServices&,SceneControllerNativeServices&)noexcept;
    EnvironmentRootStorage* EnvironmentAt4180C0()override;
    void EnsureRootAt413BE0()override;
    void* RootAt6198C4()override;
    const YtlReferenceSlotRuntime& DrawAt18(void*)override;
    const YtlReferenceSlotRuntime& ScenesAt30(void*)override;
    void SetToolModeAt434FB0(ToolWindowNativeBody40h*,std::int32_t)override;
    void PushSceneTaskAt40B050(EnvironmentVectorStorage<YtlReferenceSlotRuntime>&,TaskNativeBody4h*)override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    ToolWindowNativeInteractionServices& tools_;
    SceneControllerNativeServices& scenes_;
};

// 原ECX=M，四个DWORD栈参数为HIWORD通知码、完整ID、LPARAM、BOOL*。
// 三叶均不消费这些实参（含M），不读写handled，正常返回0；不在本层过滤ID。
// 这是可注入C++业务接口，不是可直接写进原虚表的__thiscall thunk。
LRESULT MainFrameMenuActionNativeAt4230E0(void* menu,DWORD,DWORD,LPARAM,BOOL*,IMainFrameMenuActionsNativeSource&);
LRESULT MainFrameMenuActionNativeAt42AA70(void* menu,DWORD,DWORD,LPARAM,BOOL*,IMainFrameMenuActionsNativeSource&);
LRESULT MainFrameMenuActionNativeAt42AF70(void* menu,DWORD,DWORD,LPARAM,BOOL*,IMainFrameMenuActionsNativeSource&);
}

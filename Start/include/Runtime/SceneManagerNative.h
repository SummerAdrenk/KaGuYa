#pragma once
#include <Runtime/SceneControllerNative.h>
#include <Runtime/TaskControllerNative.h>
#include <Runtime/ModeSelectButtonNative.h>
#include <Runtime/OptionCharSelectDialogNative.h>
#include <Runtime/SceneFactoryNativeTaskServices.h>

namespace KaGuYa::Start::Runtime
{
struct SceneManagerNativeBody48h;
struct SceneManagerNativeVtable1 final
{
    SceneManagerNativeBody48h* (__thiscall* deleteAt00)(SceneManagerNativeBody48h*,std::uint32_t);
};
struct SceneManagerNativeBody48h final
{
    const SceneManagerNativeVtable1* vptrAt00;
    union{YtlReferenceSlotRuntime schedulerAt04;};
    union{YtlReferenceSlotRuntime controllerAt10;};
    union{RuntimeCString commandAt1C;};
    union{YtlReferenceSlotRuntime modeAt20;};
    union{YtlReferenceSlotRuntime dialogAt2C;};
    std::int32_t unwrittenAt38;
    union{RuntimeCString demoNameAt3C;};
    std::uint32_t unwrittenAt40;
    std::uint32_t fieldAt44;
    SceneManagerNativeBody48h() noexcept{}
    ~SceneManagerNativeBody48h() noexcept{}
};
static_assert(sizeof(SceneManagerNativeBody48h)==0x48);
static_assert(sizeof(SceneManagerNativeVtable1)==4);
static_assert(offsetof(SceneManagerNativeBody48h,controllerAt10)==0x10);
static_assert(offsetof(SceneManagerNativeBody48h,commandAt1C)==0x1C);
static_assert(offsetof(SceneManagerNativeBody48h,dialogAt2C)==0x2C);
static_assert(offsetof(SceneManagerNativeBody48h,demoNameAt3C)==0x3C);
class ISceneFactoryNativeSource
{
public:
    virtual ~ISceneFactoryNativeSource()=default;
    // 原工厂直接返回raw ITask；没有shared_ptr返回或额外task bridge。
    virtual TaskNativeBody4h* CreateAt4AAF90(const char*)=0;
    virtual SceneFactoryNativeTaskServices TaskServices() const noexcept{return {};}
};
struct SceneManagerNativeServices final
{
    IEnvironmentRootMemory& memory;
    IRuntimeStringManagerSource& strings;
    ISceneFactoryNativeSource& factory;
    ModeSelectButtonNativeServices& mode;
    OptionCharSelectDialogNativeDependencies& dialog;
    TaskControllerNativeServices scheduler{memory};
    SceneControllerNativeServices controller{memory};
};
class SceneManagerNativeScope final
{
public:
    explicit SceneManagerNativeScope(SceneManagerNativeServices&) noexcept;
    ~SceneManagerNativeScope() noexcept;
    SceneManagerNativeScope(const SceneManagerNativeScope&)=delete;
    SceneManagerNativeScope& operator=(const SceneManagerNativeScope&)=delete;
private:
    SceneManagerNativeServices* previous_;
    SceneControllerNativeScope controller_;
    ModeSelectButtonNativeScope mode_;
    OptionCharSelectDialogNativeScope dialog_;
    SceneFactoryNativeTaskScope tasks_;
};
SceneManagerNativeServices& RequireSceneManagerNativeServices();
const SceneManagerNativeVtable1& SceneManagerNativeVtable() noexcept;
SceneManagerNativeBody48h* ConstructSceneManagerNativeAt45B890(SceneManagerNativeBody48h&,SceneManagerNativeServices&);
SceneManagerNativeBody48h* NewSceneManagerNativeAt48ABE1(SceneManagerNativeServices&);
void DestroySceneManagerNativeAt463AF0(SceneManagerNativeBody48h&,SceneManagerNativeServices&) noexcept;
SceneManagerNativeBody48h* DeleteSceneManagerNativeAt469300(SceneManagerNativeBody48h*,std::uint32_t,SceneManagerNativeServices&) noexcept;
void AdoptSceneManagerNativeAt48AC01(YtlReferenceSlotRuntime&,SceneManagerNativeBody48h*,SceneManagerNativeServices&);
void ConfigureSceneManagerNativeAt4703D0(SceneManagerNativeBody48h&,std::int32_t,SceneManagerNativeServices&);
void ClearSceneManagerPublicNativeAt46DB90(SceneManagerNativeBody48h&);
YtlReferenceSlotRuntime* BorrowModeButtonsNativeAt47B7B0(SceneManagerNativeBody48h&) noexcept;
YtlReferenceSlotRuntime* BorrowOptionCharDialogNativeAt478A30(SceneManagerNativeBody48h&) noexcept;
}

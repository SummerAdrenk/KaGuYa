#pragma once
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct TaskNativeBody4h;
struct TaskNativeVtable final
{
    std::uint8_t (__thiscall* runAt00)(TaskNativeBody4h*,std::int32_t);
    TaskNativeBody4h* (__thiscall* deleteAt04)(TaskNativeBody4h*,std::uint32_t);
};
struct TaskNativeBody4h final{const TaskNativeVtable* vptrAt00;};
struct SceneFactoryNativeBody4h;
struct SceneFactoryNativeVtable final
{
    TaskNativeBody4h* (__thiscall* createAt00)(SceneFactoryNativeBody4h*,const char*);
};
struct SceneFactoryNativeBody4h final{const SceneFactoryNativeVtable* vptrAt00;};
struct SceneControllerNativeBody28h final
{
    const TaskNativeVtable* vptrAt00;
    union{EnvironmentVectorStorage<const char*> pendingAt04;};
    union{EnvironmentVectorStorage<YtlReferenceSlotRuntime> activeAt10;};
    union{YtlReferenceSlotRuntime factoryAt1C;};
    SceneControllerNativeBody28h() noexcept{}
    ~SceneControllerNativeBody28h() noexcept{}
};
static_assert(sizeof(TaskNativeBody4h)==4&&sizeof(TaskNativeVtable)==8);
// 5B2B18只有Create；后DWORD是RTTI。factory正文由其control直接delete4。
static_assert(sizeof(SceneFactoryNativeBody4h)==4&&sizeof(SceneFactoryNativeVtable)==4);
static_assert(sizeof(SceneControllerNativeBody28h)==0x28);
static_assert(offsetof(SceneControllerNativeBody28h,pendingAt04)==4);
static_assert(offsetof(SceneControllerNativeBody28h,activeAt10)==0x10);
static_assert(offsetof(SceneControllerNativeBody28h,factoryAt1C)==0x1C);
struct SceneControllerNativeServices final{IEnvironmentRootMemory& memory;};
class SceneControllerNativeScope final
{
public:
    explicit SceneControllerNativeScope(SceneControllerNativeServices&) noexcept;
    ~SceneControllerNativeScope() noexcept;
private:
    SceneControllerNativeServices* previous_;
};
SceneControllerNativeBody28h* ConstructSceneControllerNativeAt45BA63(SceneControllerNativeBody28h&);
SceneControllerNativeBody28h* NewSceneControllerNativeAt45BA52(SceneControllerNativeServices&);
void AdoptSceneFactoryNativeAt45BB78(SceneControllerNativeBody28h&,SceneFactoryNativeBody4h*,SceneControllerNativeServices&);
void GetCurrentTaskNativeAt481790(SceneControllerNativeBody28h&,YtlReferenceSlotRuntime&) noexcept;
std::uint8_t RunSceneControllerNativeAt48E100(SceneControllerNativeBody28h&,std::int32_t,SceneControllerNativeServices&);
void QueueSceneControllerNativeAt47F966(SceneControllerNativeBody28h&,const char*,SceneControllerNativeServices&);
void PopSceneTaskNativeAt4BCF80(SceneControllerNativeBody28h&);
void ClearSceneControllerNativeAt4C14A5(SceneControllerNativeBody28h&) noexcept;
void ActivatePendingSceneNativeAt4C1782(SceneControllerNativeBody28h&,SceneControllerNativeServices&);
void PushSceneTaskNativeAt40B050(EnvironmentVectorStorage<YtlReferenceSlotRuntime>&,TaskNativeBody4h*,SceneControllerNativeServices&);
void DestroySceneControllerNative(SceneControllerNativeBody28h&,SceneControllerNativeServices&) noexcept;
std::uint32_t PendingSceneCountNative(const SceneControllerNativeBody28h&) noexcept;
std::int32_t ActiveSceneCountNative(const SceneControllerNativeBody28h&) noexcept;
// 4836AA：pending字节数无符号>=4才短路，否则读取active的signed Ref12计数。
bool HasSceneControllerWorkNativeAt4836AA(const SceneControllerNativeBody28h&) noexcept;

// 两个原vector的分配/增持/normal-EH尾由独立容器核实现，不用std::vector接管。
const char** InsertSceneNameNativeAt453B50(EnvironmentVectorStorage<const char*>&,const char**,
    const char* const*,IEnvironmentRootMemory&);
YtlReferenceSlotRuntime* InsertSceneTaskNativeAt4092C0(EnvironmentVectorStorage<YtlReferenceSlotRuntime>&,
    YtlReferenceSlotRuntime*,const YtlReferenceSlotRuntime&,IEnvironmentRootMemory&);
void DestroySceneNameVectorNative(EnvironmentVectorStorage<const char*>&,IEnvironmentRootMemory&) noexcept;
void DestroySceneTaskVectorNative(EnvironmentVectorStorage<YtlReferenceSlotRuntime>&,IEnvironmentRootMemory&) noexcept;
void ReleaseSceneTaskRangeNativeAt407FF0(YtlReferenceSlotRuntime*,YtlReferenceSlotRuntime*) noexcept;
}

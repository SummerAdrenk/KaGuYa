#pragma once
#include <Runtime/SceneControllerNative.h>

namespace KaGuYa::Start::Runtime
{
struct TaskControllerItemNativeBody10h final
{
    TaskNativeBody4h* taskAt00; // 仅借用，不由调度器释放任务正文
    std::int32_t priorityAt04;
    std::int32_t requestedPriorityAt08;
    std::uint8_t removedAt0C;
    std::uint8_t unwrittenAt0D[3];
};
struct TaskControllerNodeNativeBody14h final
{
    TaskControllerNodeNativeBody14h* nextAt00;
    TaskControllerNodeNativeBody14h* previousAt04;
    union{YtlReferenceSlotRuntime itemAt08;};
    TaskControllerNodeNativeBody14h() noexcept{}
    ~TaskControllerNodeNativeBody14h() noexcept{}
};
struct TaskControllerNativeBody18h final
{
    TaskControllerNodeNativeBody14h* headAt00;
    std::uint32_t countAt04;
    std::uint8_t flagAt08;
    std::uint8_t reprioritizeAt09;
    std::uint8_t unwrittenAt0A[2];
    union{EnvironmentVectorStorage<std::int32_t> blockedAt0C;};
    TaskControllerNativeBody18h() noexcept{}
    ~TaskControllerNativeBody18h() noexcept{}
};
static_assert(sizeof(TaskControllerItemNativeBody10h)==0x10);
static_assert(sizeof(TaskControllerNodeNativeBody14h)==0x14);
static_assert(sizeof(TaskControllerNativeBody18h)==0x18);
static_assert(offsetof(TaskControllerNativeBody18h,blockedAt0C)==0x0C);
static_assert(offsetof(TaskControllerItemNativeBody10h,removedAt0C)==0x0C);
struct TaskControllerNativeServices final{IEnvironmentRootMemory& memory;};
TaskControllerNativeBody18h* ConstructTaskControllerNativeAt45B975(TaskControllerNativeBody18h&,TaskControllerNativeServices&);
TaskControllerNativeBody18h* NewTaskControllerNativeAt45B960(TaskControllerNativeServices&);
void ClearTaskControllerNativeAt4912B0(TaskControllerNativeBody18h&) noexcept;
void DestroyTaskControllerNativeAt4644A0(TaskControllerNativeBody18h&,TaskControllerNativeServices&) noexcept;
void DeleteTaskControllerNative(TaskControllerNativeBody18h*,TaskControllerNativeServices&) noexcept;
void RegisterTaskNativeAt46A330(TaskControllerNativeBody18h&,TaskNativeBody4h*,std::int32_t,TaskControllerNativeServices&);
void RunTaskControllerNativeAt46BC90(TaskControllerNativeBody18h&,std::int32_t,TaskControllerNativeServices&);

// 独立列表核：仅连接字段与原Ref资格，不存另一套任务或优先级镜像。
TaskControllerNodeNativeBody14h* NewTaskListNodeNativeAt48F550(TaskControllerNodeNativeBody14h*,TaskControllerNodeNativeBody14h*,IEnvironmentRootMemory&);
TaskControllerNodeNativeBody14h* CopyTaskListNodeNativeAt4520A0(TaskControllerNodeNativeBody14h*,TaskControllerNodeNativeBody14h*,const YtlReferenceSlotRuntime&,IEnvironmentRootMemory&);
void PublishTaskControllerItemNative(YtlReferenceSlotRuntime&,TaskControllerItemNativeBody10h*,IEnvironmentRootMemory&);
void ReleaseTaskControllerItemCachedNative(YtlReferenceSlotRuntime&,YtlReferenceControlRuntime*) noexcept;
void EraseTaskControllerNodeNative(TaskControllerNativeBody18h&,TaskControllerNodeNativeBody14h*) noexcept;
}

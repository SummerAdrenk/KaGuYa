#pragma once

#include <Runtime/ColorTransWindowNative.h>
#include <Runtime/MessageWindowNativeConfig.h>
#include <Runtime/EnvironmentRootRuntime.h>

#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct MessageWindowPaneNativeBody14h;
struct MessageWindowPaneNativeVtable final
{
    std::int32_t (__thiscall* typeAt00)(MessageWindowPaneNativeBody14h*);
    RECT* (__thiscall* boundsAt04)(MessageWindowPaneNativeBody14h*, RECT*);
    // 5B2BF8/5B2C10：调用方不消费刷新返回值；normal具体入口另保留AL，variable没有成功协议。
    void (__thiscall* refreshAt08)(MessageWindowPaneNativeBody14h*);
    void (__thiscall* drawAt0C)(MessageWindowPaneNativeBody14h*, const RECT*);
    void (__thiscall* compositeAt10)(MessageWindowPaneNativeBody14h*, GraphicsSurfaceNativeBody*, const RECT*);
};
struct MessageWindowPaneNativeBody14h final
{
    const MessageWindowPaneNativeVtable* vptrAt00;
    RECT boundsAt04;
};
struct MessageWindowNormalPaneNativeBody2Ch final
{
    MessageWindowPaneNativeBody14h pane;
    union { YtlReferenceSlotRuntime colorTransAt14; };
    union { YtlReferenceSlotRuntime configAt20; };
    MessageWindowNormalPaneNativeBody2Ch() noexcept {}
};
using MessageWindowEdgeReferenceVector = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
struct MessageWindowVariablePaneNativeBody3Ch final
{
    MessageWindowPaneNativeBody14h pane;
    union { MessageWindowEdgeReferenceVector edgesAt14; };
    union { YtlReferenceSlotRuntime centerAt20; };
    std::int32_t columnsAt2C, maxColumnsAt30, rowsAt34, maxRowsAt38;
    MessageWindowVariablePaneNativeBody3Ch() noexcept {}
};
struct MessageWindowEdgeNativeBody10h final
{
    std::uint32_t indexAt00;
    union { YtlReferenceSlotRuntime normalAt04; };
    MessageWindowEdgeNativeBody10h() noexcept {}
};
struct MessageWindowStyleNativeBody34h final
{
    union { YtlReferenceSlotRuntime bodyAt00; };
    union { YtlReferenceSlotRuntime nameAt0C; };
    union { YtlReferenceSlotRuntime configAt18; };
    RECT nameBaselineAt24;
    MessageWindowStyleNativeBody34h() noexcept {}
};
static_assert(sizeof(MessageWindowPaneNativeVtable) == 0x14);
static_assert(sizeof(MessageWindowPaneNativeBody14h) == 0x14);
static_assert(sizeof(MessageWindowNormalPaneNativeBody2Ch) == 0x2C);
static_assert(offsetof(MessageWindowNormalPaneNativeBody2Ch, colorTransAt14) == 0x14);
static_assert(offsetof(MessageWindowNormalPaneNativeBody2Ch, configAt20) == 0x20);
static_assert(sizeof(MessageWindowVariablePaneNativeBody3Ch) == 0x3C);
static_assert(offsetof(MessageWindowVariablePaneNativeBody3Ch, edgesAt14) == 0x14);
static_assert(offsetof(MessageWindowVariablePaneNativeBody3Ch, centerAt20) == 0x20);
static_assert(offsetof(MessageWindowVariablePaneNativeBody3Ch, columnsAt2C) == 0x2C);
static_assert(offsetof(MessageWindowVariablePaneNativeBody3Ch, maxRowsAt38) == 0x38);
static_assert(sizeof(MessageWindowEdgeNativeBody10h) == 0x10);
static_assert(offsetof(MessageWindowEdgeNativeBody10h, normalAt04) == 4);
static_assert(sizeof(MessageWindowStyleNativeBody34h) == 0x34);
static_assert(offsetof(MessageWindowStyleNativeBody34h, configAt18) == 0x18);
static_assert(offsetof(MessageWindowStyleNativeBody34h, nameBaselineAt24) == 0x24);
static_assert(std::is_trivially_destructible_v<MessageWindowNormalPaneNativeBody2Ch>);
static_assert(std::is_trivially_destructible_v<MessageWindowVariablePaneNativeBody3Ch>);
static_assert(std::is_trivially_destructible_v<MessageWindowStyleNativeBody34h>);

// 原只读描述符，仅构造期借用；不含modern pane或名称副本。
struct MessageWindowNormalPaneNativeDescriptor final
{
    const char* firstNameAt00;
    const char* secondNameAt04;
    std::int32_t modeAt08;
};
struct MessageWindowVariablePaneNativeDescriptor final
{
    const MessageWindowNormalPaneNativeDescriptor* centerAt00;
    std::int32_t maxColumnsAt04, maxRowsAt08;
    const MessageWindowNormalPaneNativeDescriptor* edgesAt0C[4];
};
struct MessageWindowStyleNativeDescriptor final
{
    const MessageWindowNormalPaneNativeDescriptor* normalBodyAt00;
    const MessageWindowVariablePaneNativeDescriptor* variableBodyAt04;
    const MessageWindowNormalPaneNativeDescriptor* normalNameAt08;
    const MessageWindowVariablePaneNativeDescriptor* variableNameAt0C;
    RECT nameBaselineAt10;
    std::int32_t configDiscriminatorAt20;
};
static_assert(sizeof(MessageWindowNormalPaneNativeDescriptor) == 0x0C);
static_assert(sizeof(MessageWindowVariablePaneNativeDescriptor) == 0x1C);
static_assert(sizeof(MessageWindowStyleNativeDescriptor) == 0x24);
// 601408固定两项；返回只读表首址，不复制modern descriptor或创造第三项。
const MessageWindowStyleNativeDescriptor* MessageWindowStylesNativeSample18At601408() noexcept;

struct MessageWindowNativePaneServices final
{
    ColorTransWindowNativeServices& colorTrans;
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
};
// 五槽thunk只借当前scope，嵌套恢复；不把服务塞进正文/control，也不偷建Graphics。
class MessageWindowNativePaneScope final
{
public:
    explicit MessageWindowNativePaneScope(MessageWindowNativePaneServices& services) noexcept;
    ~MessageWindowNativePaneScope() noexcept;
    MessageWindowNativePaneScope(const MessageWindowNativePaneScope&) = delete;
    MessageWindowNativePaneScope& operator=(const MessageWindowNativePaneScope&) = delete;
private:
    MessageWindowNativePaneServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
};
const MessageWindowPaneNativeVtable& MessageWindowNormalPaneNativeMethods() noexcept;
const MessageWindowPaneNativeVtable& MessageWindowVariablePaneNativeMethods() noexcept;

// Construct消耗一份已经增持的按值参数Ref；正常尾与EH均由本入口释放，不是借用或move成员。
MessageWindowNormalPaneNativeBody2Ch* ConstructMessageWindowNormalPaneNativeAt45F2E0(
    MessageWindowNormalPaneNativeBody2Ch&, const MessageWindowNormalPaneNativeDescriptor*,
    YtlReferenceSlotRuntime ownedConfigParameter, MessageWindowNativePaneServices&);
MessageWindowVariablePaneNativeBody3Ch* ConstructMessageWindowVariablePaneNativeAt45F720(
    MessageWindowVariablePaneNativeBody3Ch&, const MessageWindowVariablePaneNativeDescriptor*,
    YtlReferenceSlotRuntime ownedConfigParameter, MessageWindowNativePaneServices&);
MessageWindowStyleNativeBody34h* ConstructMessageWindowStyleNativeAt45F040(
    MessageWindowStyleNativeBody34h&, const MessageWindowStyleNativeDescriptor*,
    YtlReferenceSlotRuntime ownedConfigParameter, MessageWindowNativePaneServices&);
// New先分配raw，再复制/增持当时config为上述参数；只在构造失败时回收raw，返回完成正文。
MessageWindowNormalPaneNativeBody2Ch* NewMessageWindowNormalPaneNative(
    const MessageWindowNormalPaneNativeDescriptor*, const YtlReferenceSlotRuntime& config,
    MessageWindowNativePaneServices&);
MessageWindowVariablePaneNativeBody3Ch* NewMessageWindowVariablePaneNative(
    const MessageWindowVariablePaneNativeDescriptor*, const YtlReferenceSlotRuntime& config,
    MessageWindowNativePaneServices&);
MessageWindowStyleNativeBody34h* NewMessageWindowStyleNative(
    const MessageWindowStyleNativeDescriptor*, const YtlReferenceSlotRuntime& config,
    MessageWindowNativePaneServices&);

void AdoptMessageWindowNormalPaneNativeAt451B20(YtlReferenceSlotRuntime&,
    MessageWindowNormalPaneNativeBody2Ch*, IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
void AdoptMessageWindowNormalPaneNativeAt451BC0(YtlReferenceSlotRuntime&,
    MessageWindowNormalPaneNativeBody2Ch*, IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
void AdoptMessageWindowVariablePaneNativeAt451D00(YtlReferenceSlotRuntime&,
    MessageWindowVariablePaneNativeBody3Ch*, IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
// 45A2C5新局部槽：先清槽；非空正文base先发布，refs/owning/stride先写，control发布后才写count。
void ConstructMessageWindowStyleReferenceAt45A2C5(YtlReferenceSlotRuntime&,
    MessageWindowStyleNativeBody34h*, IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
MessageWindowPaneNativeBody14h* ResolveMessageWindowPaneNative(const YtlReferenceSlotRuntime&) noexcept;
MessageWindowStyleNativeBody34h* ResolveMessageWindowStyleNative(const YtlReferenceSlotRuntime&) noexcept;
MessageWindowEdgeNativeBody10h* ResolveMessageWindowEdgeNative(const YtlReferenceSlotRuntime&) noexcept;
// 正文无虚析构；以下只执行成员尾，不释放本身。最后control负责原sized delete。
void DestroyMessageWindowNormalPaneNativeAt465570(MessageWindowNormalPaneNativeBody2Ch&) noexcept;
void DestroyMessageWindowVariablePaneNative(MessageWindowVariablePaneNativeBody3Ch&) noexcept;
void DestroyMessageWindowEdgeNative(MessageWindowEdgeNativeBody10h&) noexcept;
void DestroyMessageWindowStyleNativeAt465480(MessageWindowStyleNativeBody34h&) noexcept;

RECT* ReadMessageWindowPaneBoundsNativeAt479DD0(MessageWindowPaneNativeBody14h&, RECT*) noexcept;
GraphicsSurfaceNativeBody* ResolveMessageWindowNormalProcessedNativeAt47DFE0(
    const MessageWindowNormalPaneNativeBody2Ch&) noexcept;
MessageWindowVariablePaneNativeBody3Ch* ResolveMessageWindowNamePaneNativeAt46CBF0(
    const MessageWindowStyleNativeBody34h&) noexcept;
std::uint8_t RefreshMessageWindowNormalPaneNativeAt4874C0(
    MessageWindowNormalPaneNativeBody2Ch&, MessageWindowNativePaneServices&);
void RefreshMessageWindowVariablePaneNativeAt487540(
    MessageWindowVariablePaneNativeBody3Ch&, MessageWindowNativePaneServices&);
void DrawMessageWindowNormalPaneNativeAt482DB0(
    MessageWindowNormalPaneNativeBody2Ch&, const RECT*, MessageWindowNativePaneServices&);
void CompositeMessageWindowNormalPaneNativeAt481D50(
    MessageWindowNormalPaneNativeBody2Ch&, GraphicsSurfaceNativeBody*, const RECT*, MessageWindowNativePaneServices&);
void DrawMessageWindowVariablePaneNativeAt482E80(
    MessageWindowVariablePaneNativeBody3Ch&, const RECT*, MessageWindowNativePaneServices&);
void CompositeMessageWindowVariablePaneNativeAt481E20(
    MessageWindowVariablePaneNativeBody3Ch&, GraphicsSurfaceNativeBody*, const RECT*, MessageWindowNativePaneServices&);
class IMessageWindowNativeNameSource
{
public:
    virtual ~IMessageWindowNativeNameSource() = default;
    // 单次413BE0 -> 实际root+18 -> 418C10(3) -> 真实TextWin+40，返回当次借用字段地址。
    virtual const RECT* CaptureTextWindowNameBoundsAt46B558() = 0;
};
void PrepareMessageWindowNativeNameAt46B470(MessageWindowStyleNativeBody34h&,
    IMessageWindowNativeNameSource&);
}

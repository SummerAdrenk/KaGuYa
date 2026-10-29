#pragma once

#include <Runtime/DrawManagerNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/SceneControllerNative.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
struct ScriptParamsNativeBody;
struct SceneManagerNativeBody48h;
class IScrVmCommandErrorSink;
struct GraffitiPageNativeBody20h;
struct GraffitiCountDialogNativeServices;

// 原PE 5B5824：仅四槽；5B5834是下一类型的RTTI，不是第五槽。
struct GraffitiPageNativeVtable final
{
    std::uint8_t (__thiscall* runAt00)(GraffitiPageNativeBody20h*, std::int32_t);
    GraffitiPageNativeBody20h* (__thiscall* deleteAt04)(GraffitiPageNativeBody20h*, std::uint32_t);
    void (__thiscall* updateAt08)(GraffitiPageNativeBody20h*);
    void (__thiscall* drawAt0C)(GraffitiPageNativeBody20h*);
};
struct GraffitiPageNativeBody20h final
{
    const GraffitiPageNativeVtable* vptrAt00;
    std::uint32_t phaseAt04;
    union { YtlReferenceSlotRuntime dialogAt08; };
    std::uint8_t unwrittenAt14[4];
    std::uint32_t savedSlot2At18;
    std::uint32_t drawGateAt1C;

    // 49DCA0自行按指令写前缀；默认new也不得提前清Ref或+14。
    GraffitiPageNativeBody20h() noexcept {}
};
static_assert(sizeof(GraffitiPageNativeVtable) == 0x10);
static_assert(offsetof(GraffitiPageNativeVtable, runAt00) == 0);
static_assert(offsetof(GraffitiPageNativeVtable, deleteAt04) == 4);
static_assert(offsetof(GraffitiPageNativeVtable, updateAt08) == 8);
static_assert(offsetof(GraffitiPageNativeVtable, drawAt0C) == 0x0C);
static_assert(sizeof(GraffitiPageNativeBody20h) == 0x20);
static_assert(alignof(GraffitiPageNativeBody20h) == 4);
static_assert(std::is_standard_layout_v<GraffitiPageNativeBody20h>);
static_assert(std::is_trivially_destructible_v<GraffitiPageNativeBody20h>);
static_assert(offsetof(GraffitiPageNativeBody20h, phaseAt04) == 4);
static_assert(offsetof(GraffitiPageNativeBody20h, dialogAt08) == 8);
static_assert(offsetof(GraffitiPageNativeBody20h, unwrittenAt14) == 0x14);
static_assert(offsetof(GraffitiPageNativeBody20h, savedSlot2At18) == 0x18);
static_assert(offsetof(GraffitiPageNativeBody20h, drawGateAt1C) == 0x1C);

// 49DDF5缓存ESI根，498020返回后才在这个根上调用47BDB0。
// 只借接收者/函数地址，不增持root、Params或任何现代Dialog owner。
struct GraffitiPageNativeParamsReceiver final
{
    void* root;
    ScriptParamsNativeBody& (*paramsAt47BDB0)(void*);
};
class IGraffitiPageNativeSource : public IDrawManagerNativeRootSource
{
public:
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    virtual GraffitiPageNativeParamsReceiver CaptureParamsRootAfter413BE0() = 0;
    // 4B6152：仅本次借用，4C1760的controller解Ref由正文执行。
    virtual SceneManagerNativeBody48h* CaptureSceneManagerAfter413BE0() = 0;
};
class IGraffitiPageNativeDialogSource
{
public:
    virtual ~IGraffitiPageNativeDialogSource() = default;
    // raw分配、EH资格及Ref发布均由49DCA0负责；叶必须完成自己的成员EH。
    // 尚无生产native派生构造实现，禁止以shared IGraffitiPageDialog冒充返回。
    virtual GuiDialogNativeBody5Ch* ConstructLikableAt4E13A0(void* rawB4h) = 0;
    virtual GuiDialogNativeBody5Ch* ConstructWeightAt4F4BC0(void* rawB8h) = 0;
    // 只借正文外服务；真实Count构造源声明后续虚调用所需绑定，非Count来源明确为空。
    virtual GraffitiCountDialogNativeServices* CountDialogServices() const noexcept{return nullptr;}
};
struct GraffitiPageNativeServices final
{
    IGraffitiPageNativeSource& source;
    IGraffitiPageNativeDialogSource& dialogs;
    IEnvironmentRootMemory& memory;
    IScrVmCommandErrorSink& errors;
    IRuntimeStringManagerSource& strings;
    ScriptNamedValueNativeTables names;
    SceneControllerNativeServices controller;
};
class GraffitiPageNativeScope final
{
public:
    explicit GraffitiPageNativeScope(GraffitiPageNativeServices&) noexcept;
    ~GraffitiPageNativeScope() noexcept;
    GraffitiPageNativeScope(const GraffitiPageNativeScope&) = delete;
    GraffitiPageNativeScope& operator=(const GraffitiPageNativeScope&) = delete;
private:
    GraffitiPageNativeServices* previous_;
};
// 主线SceneManager的统一调用范围可使用这个短名；与构造局部scope共享同一TLS。
// 必须涵盖Run、Delete和最后一次Task/Dialog Ref释放，服务本身不随正文拥有。
using GraffitiNativeServices = GraffitiPageNativeServices;
using GraffitiNativeScope = GraffitiPageNativeScope;
const GraffitiPageNativeVtable& GraffitiPageNativeMethods() noexcept;
const TaskNativeVtable& GraffitiPageRetiredTaskMethods() noexcept;
GraffitiPageNativeBody20h* ConstructGraffitiPageNativeAt49DCA0(
    GraffitiPageNativeBody20h&, GraffitiPageNativeServices&);
// 只返回raw正文，调用方的原Task Ref管理保活；不参与SceneFactory17分派。
GraffitiPageNativeBody20h* NewGraffitiPageNative(GraffitiPageNativeServices&);
void DestroyGraffitiPageNativeAt4A6920(GraffitiPageNativeBody20h&) noexcept;
GraffitiPageNativeBody20h* DeleteGraffitiPageNativeAt4A6920(
    GraffitiPageNativeBody20h*, std::uint32_t) noexcept;
std::uint8_t RunGraffitiPageNativeAt4C3630(
    GraffitiPageNativeBody20h&, std::int32_t, GraffitiPageNativeServices&);
void UpdateGraffitiPageNativeAt4B6130(GraffitiPageNativeBody20h&, GraffitiPageNativeServices&);
void DrawGraffitiPageNativeAt4B5860(GraffitiPageNativeBody20h&, GraffitiPageNativeServices&);
void MergeGraffitiPageDirtyNativeAt4AA9F0(GraffitiPageNativeBody20h&, GraffitiPageNativeServices&);
}

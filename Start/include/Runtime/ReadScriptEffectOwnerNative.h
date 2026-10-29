#pragma once

#include <Runtime/ReadScriptEffectNative.h>
#include <Runtime/ReadScriptSuspendWaitRuntime.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>

namespace KaGuYa::Start::Runtime
{
// CReadScript+1C..4B（4A1509..4A1556）：不含服务、影子状态或隐式释放。
struct ReadScriptEffectSlotsNative30h final
{
    YtlReferenceSlotRuntime effectAt00;
    YtlReferenceSlotRuntime sourceAt0C;
    YtlReferenceSlotRuntime previousAt18;
    YtlReferenceSlotRuntime auxiliaryAt24;
};
static_assert(sizeof(ReadScriptEffectSlotsNative30h) == 0x30);
static_assert(alignof(ReadScriptEffectSlotsNative30h) == 4);
static_assert(offsetof(ReadScriptEffectSlotsNative30h, effectAt00) == 0);
static_assert(offsetof(ReadScriptEffectSlotsNative30h, sourceAt0C) == 0x0C);
static_assert(offsetof(ReadScriptEffectSlotsNative30h, previousAt18) == 0x18);
static_assert(offsetof(ReadScriptEffectSlotsNative30h, auxiliaryAt24) == 0x24);

// 仅用于尚未拥有资源的存储；初始化全部十二个DWORD，包括各槽bias。
void InitializeReadScriptEffectSlotsAt4A14D0(ReadScriptEffectSlotsNative30h& slots) noexcept;
// 正序effect/source/previous/auxiliary；payload回调后重读该槽control。
// 异常保留当前槽已递减的计数及字段，后续槽不提前清理。
void ReleaseReadScriptEffectSlotsAt4C0990(ReadScriptEffectSlotsNative30h& slots);
// 构造EH按成员逆序退役，不额外调用4C0990或Finish。
// 第一个可抛成员构造调用4A15E5之前，四个Ref已经全部初始化。
void UnwindReadScriptEffectSlotsAt4A14D0(ReadScriptEffectSlotsNative30h& slots) noexcept;
// 仅为4A4920的Ref成员尾析构；外层noexcept析构必须先调用4C0990，
// 再析构CString+B0及FreeScroll+60，最后调用此入口。
void DestroyReadScriptEffectSlotMembersAt4A4920(ReadScriptEffectSlotsNative30h& slots) noexcept;

// 候选正文已完成构造；这里不检查Ready，也不补失败回滚。
// 4A7DE5/4B47ED/4C3C17/4C3781：释放旧槽、发布base/bias、分配control18h。
// Packet的原stride为3Ch，Move为24h（4C4EAD），不能传sizeof(Base8h)。
// control+14拥有原正文，不是加过bias的视图指针。
// control分配失败保留已发布的新base和空control，不删除候选正文。
void InstallReadScriptEffectNative(YtlReferenceSlotRuntime& slot, void* created,
    std::uint32_t originalStride);

// 仅借用切片，适配器析构不处理四个Ref；由CReadScript拥有者显式管理生命周期，
// 不能并存另一套unique_ptr/shared_ptr业务拥有权。
// 外部scope须覆盖Tick/Draw、Install中的旧对象删除、Release、成员尾析构及EH：
// Surface24h需要GraphicsSurfaceNativeScope；Move需要MoveEffectNativeScope；
// Packet需要ReadScriptPacketNativeScope提供借用的draw/screen服务。
// scope仅绑定服务，必须晚于构造展开退出；不提前拒绝实际无需服务的分支。
// 适配器、切片及Effect control均不保存scope/API指针。
class ReadScriptEffectOwnerNative final : public IReadScriptSuspendEffectOwner
{
public:
    explicit ReadScriptEffectOwnerNative(ReadScriptEffectSlotsNative30h& slots) noexcept : slots_(slots) {}
    ReadScriptEffectOwnerNative(const ReadScriptEffectOwnerNative&) = delete;
    ReadScriptEffectOwnerNative& operator=(const ReadScriptEffectOwnerNative&) = delete;

    bool HasEffectAt1C() const override;
    bool TickEffectAtVslot00() override;
    void DrawEffectAtVslot04() override;
    [[nodiscard]] std::uint8_t ReadyEffectAtVslot08() const;
    void ReleaseResourcesAt4C0990() override;
    void InstallEffect(void* created, std::uint32_t originalStride);

    [[nodiscard]] ReadScriptEffectSlotsNative30h& Slots() noexcept { return slots_; }
    [[nodiscard]] const ReadScriptEffectSlotsNative30h& Slots() const noexcept { return slots_; }
    [[nodiscard]] YtlReferenceSlotRuntime& SourceAt28() noexcept { return slots_.sourceAt0C; }
    [[nodiscard]] YtlReferenceSlotRuntime& PreviousAt34() noexcept { return slots_.previousAt18; }
    [[nodiscard]] YtlReferenceSlotRuntime& AuxiliaryAt40() noexcept { return slots_.auxiliaryAt24; }
    [[nodiscard]] GraphicsSurfaceNativeBody* SourceBodyAt28() const noexcept;
private:
    ReadScriptEffectSlotsNative30h& slots_;
};
}

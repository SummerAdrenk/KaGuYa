#include <Runtime/CgUiButtonSurfaceAppendNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct LocalReferenceUnwind final
{
    YtlReferenceSlotRuntime& slot;
    bool active = true;
    ~LocalReferenceUnwind() noexcept
    {
        // 5EDA8C state0 -> 59C280 -> 40D2F0 (FuncInfo 5DF450 flags5)：
        // EH 才按槽晚取 control；payload 返回后再读 control，清 base/control，保留 bias。
        // 这里复用已有 Ref 释放叶；若退休抛出，noexcept 与原 flags5 一样终止。
        if (active) ReleaseYtlReference(slot);
    }
};

void ReleaseLocalNormally(YtlReferenceControlRuntime* control) noexcept
{
    // 4999B6 state1 的终止边界 (5EDA94 -> 541E55)，不能被局部 Ref EH 再次减持。
    // 4999C1..4999D9：计数是 DWORD 非原子加减；owning 只在计数归零时查。
    // 正常路径固定 ESI；payload 后仍用同一 control，且不清局部 Ref 的位模式。
    if (!control || --control->referenceCountAt04 != 0) return;
    if (control->owningAt08) control->DestroyPayloadAtVslot00();
    control->DeleteControlAtVslot04(1);
}
}

void AppendCgUiButtonSurfaceReferenceAt499920(GuiControlSurfaceReferences& references,
    const YtlReferenceSlotRuntime& source)
{
    // 49994A..49995E：先清三个局部字段，再启用 state0。没有 vector 或正文新 owner。
    YtlReferenceSlotRuntime local{};
    LocalReferenceUnwind unwind{local};
    // 499972/499977/499979：control、base、bias 的读取顺序；随后才递增局部持有。
    // 原 49996D 的 self 检查只针对不可由正常调用方命名的 fresh 栈局部。
    auto* const sourceControl = source.controlAt08;
    local.baseAt00 = source.baseAt00;
    local.biasAt04 = source.biasAt04;
    local.controlAt08 = sourceControl;
    if (sourceControl) ++sourceControl->referenceCountAt04;

    // 49998C 之后才读 end/capacity。快路再增持一次、end += 12；慢路仍是 4540A0。
    // 必须传 local：分配回调改变外部 source / 旧元素别名，不得改本次插入值。
    AppendGuiControlSurfaceReference(references, local);

    // 4999B3 慢路返回后重读局部 control；快路无中间回调，读到同一 ESI。
    auto* const normalControl = local.controlAt08;
    unwind.active = false;
    ReleaseLocalNormally(normalControl);
}
}

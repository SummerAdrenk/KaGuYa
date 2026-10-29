#include <Runtime/MoveEffectNativeConstruction.h>
#include <Runtime/MoveEffectNativeResources.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/LayerWindowNative.h>
#include <Runtime/ScriptNativeBody.h>

#include <bit>
#include <new>
#include <emmintrin.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Item = MoveEffectItemNativeBody3Ch;
class ItemControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ItemControl(Item* item) noexcept
        : EnvironmentObjectReferenceControl(item, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* item = static_cast<Item*>(payloadAt14);
        if (item)
        {
            // 条目不拥有Pattern；snapshot删除使用调用方正文外的GraphicsSurfaceNativeScope。
            [&]() noexcept { ReleaseYtlReference(item->snapshotAt28); }();
            item->~Item();
            ::operator delete(item, 0x3C);
        }
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ItemControl) == 0x18);

float SignedFloat(std::int32_t n) noexcept
    { return _mm_cvtss_f32(_mm_cvtepi32_ps(_mm_cvtsi32_si128(n))); }
float UnsignedFloat(std::uint32_t n) noexcept
    { return _mm_cvtss_f32(_mm_cvtpd_ps(_mm_set_sd(static_cast<double>(n)))); }
float Sub(float a, float b) noexcept
    { return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a), _mm_set_ss(b))); }
float Add(float a, float b) noexcept
    { return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(a), _mm_set_ss(b))); }
float Div(float a, float b) noexcept
    { return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a), _mm_set_ss(b))); }
std::int32_t WrapSub(std::uint32_t a, std::int32_t b) noexcept
    { return std::bit_cast<std::int32_t>(a - static_cast<std::uint32_t>(b)); }

// 4A3000/4C59F0都在payload回调后重读局部槽的control；不能用RAII智能指针替换。
void ReleaseLocal(Slot& slot) noexcept { ReleaseYtlReference(slot); }

PatternLayerNativeBody80h* CapturePattern(IMoveEffectNativeConstructionSource& source, std::uint32_t index)
{
    auto* manager = source.CaptureDrawManagerAfter413BE0();
    // 49C2C4直接消费owning.begin[0]，不先检查Draw vector长度。
    auto* window = ResolveEnvironmentObjectReference<LayerWindowNativeBody4Ch>(manager->ownedLayersAt14.begin[0]);
    const auto count = static_cast<std::uint32_t>(DrawLayerReferenceCount(window->patternsAt30));
    if (index > count) return nullptr; // 49C2FF的jnb包含物理end槽。
    return ResolveEnvironmentObjectReference<PatternLayerNativeBody80h>(window->patternsAt30.begin[index]);
}

void ComputeSteps(Item& item, IMoveEffectNativeConstructionSource& source)
{
    float dx = 0, dy = 0;
    const auto tx = SignedFloat(item.targetXAt08), ty = SignedFloat(item.targetYAt0C);
    if (item.currentXAt00 != tx)
    {
        if (tx > item.currentXAt00) { item.directionsAt20 |= 1; dx = Sub(tx, item.currentXAt00); }
        else { item.directionsAt20 |= 2; dx = Sub(item.currentXAt00, tx); }
    }
    if (item.currentYAt04 != ty)
    {
        if (ty > item.currentYAt04) { item.directionsAt20 |= 4; dy = Sub(ty, item.currentYAt04); }
        else { item.directionsAt20 |= 8; dy = Sub(item.currentYAt04, ty); }
    }
    float denominator = SignedFloat(source.ReadDenominatorAt602648(source.ReadSpeedIndexAt49C86B()));
    const float distance = dy > dx ? dy : dx;
    // 49C890/49CBB0及49CA20/49D288各自再取根；严格大于，等于仍继续加分母。
    while (!(SignedFloat(source.ReadThresholdAt602658(source.ReadSpeedIndexAt49C86B())) > Div(distance, denominator)))
        denominator = Add(denominator, SignedFloat(source.ReadDenominatorAt602648(source.ReadSpeedIndexAt49C86B())));
    item.stepXAt18 = Div(dx, denominator);
    item.stepYAt1C = Div(dy, denominator);
}
}

std::int32_t IMoveEffectNativeConstructionSource::ReadDenominatorAt602648(std::uint32_t index)
{
    // 原PE 602648..602657。index>=4无已证实业务范围，保留原直接索引前提。
    static constexpr std::int32_t values[]{64, 32, 16, 8};
    return values[index];
}
std::int32_t IMoveEffectNativeConstructionSource::ReadThresholdAt602658(std::uint32_t index)
{
    static constexpr std::int32_t values[]{8, 12, 16, 32};
    return values[index];
}

void CreateMoveEffectItemAt49C33B(Slot& local, IEnvironmentRootMemory& memory)
{
    void* storage = memory.AllocateBody(0x3C);
    auto* item = storage ? ::new(storage) Item : nullptr;
    if (item)
    {
        item->targetXAt08 = item->targetYAt0C = 0;
        item->finalXAt10 = item->finalYAt14 = 0;
        ::new(&item->snapshotAt28) Slot;
        item->offsetXAt34 = item->offsetYAt38 = 0;
    }
    local = {};
    if (item)
    {
        local.baseAt00 = reinterpret_cast<std::uintptr_t>(item);
        void* storageControl = memory.AllocateBody(0x18);
        auto* control = storageControl ? ::new(storageControl) ItemControl(item) : nullptr;
        // 49C3D0首先写+04；null/control失败不转成bad_alloc或补回滚正文。
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 0x3C;
        local.controlAt08 = control;
        control->countAt10 = 1;
    }
}

Slot* GrowMoveEffectItemsAt498DC0(DrawLayerReferenceArray& array, Slot* position,
    const Slot& source, IEnvironmentRootMemory& memory)
{
    // 498DC0和454580逐段同构：Ref12、1.5倍增长、1000h大块对齐、旧范围正序释放后晚发布。
    // 两者释放均在payload后重读槽control，且范围释放异常终止；不引入现代vector容量语义。
    return GrowDrawLayerReferencesAt454580(array, position, source, memory);
}
void AppendMoveEffectItemAt49D236(DrawLayerReferenceArray& array, const Slot& source,
    IEnvironmentRootMemory& memory)
{
    auto* position = array.end;
    if (position == array.capacity)
        static_cast<void>(GrowMoveEffectItemsAt498DC0(array, position, source, memory));
    else
    {
        *position = source;
        if (position->controlAt08) ++position->controlAt08->referenceCountAt04;
        ++array.end;
    }
}

MoveEffectNativeBody24h* ConstructMoveEffectNativeAt49C0F0(MoveEffectNativeBody24h& body,
    IMoveEffectNativeConstructionSource& source, PatternLayerNativeRenderServices& resources,
    IEnvironmentRootMemory& memory)
{
    body.readyAt04 = 0;
    body.vptrAt00 = &MoveEffectNativeMethods();
    body.itemsAt08 = {};
    body.viewportAt14 = {};
    GraphicsSurfaceNativeScope graphics(resources.surfaces);
    // PE 5EEFB0：try=[2,16h]，catchHigh=17h；成员前缀在try之外，catch不回滚已追加项。
    // unwind：5->2释放var_68；15h->5释放var_74；16h->2 terminate。
    // 3/6/8/A/C/E分别展开GS/Program锁，4/7/9/B/D/F是其Leave异常terminate边界。
    // 10h..14h的Surface/string/clone局部由独立资源入口保留，不跨入口伪造owner。
    try
    {
        const auto size = source.ReadScreenSizeAt47CC00();
        body.viewportAt14 = {0, 0, size.cx, size.cy};
        for (std::uint32_t index = 0;
            std::bit_cast<std::int32_t>(index) < source.ReadLiveGameSystemCountAt49C282(); ++index)
        {
            auto* const pattern = CapturePattern(source, index);
            if (!pattern->layer.enabledAt0C) continue;
            Slot local;
            // 49C3F1才获得state5资格：control申请抛出时只保留已写前缀，不析构item。
            CreateMoveEffectItemAt49C33B(local, memory);
            auto* const item = ResolveEnvironmentObjectReference<Item>(local);
            try
            {
                item->patternAt24 = pattern;
                item->directionsAt20 = 0;
                item->currentXAt00 = SignedFloat(std::bit_cast<std::int32_t>(pattern->layer.valueAt04));
                item->currentYAt04 = SignedFloat(std::bit_cast<std::int32_t>(pattern->layer.valueAt08));
                auto* const script = source.CaptureScriptAfter413BE0();
                auto* const effect = ResolveEnvironmentObjectReference<ScriptEffectNative>(script->effectAt0C);
                const volatile auto* pair = ResolveScriptPair(*effect, index);
                // 49C498..49C4B3四次读取交错写入，保留别名与故障时的已写前缀。
                item->targetXAt08 = pair->first;
                item->targetYAt0C = pair->second;
                item->finalXAt10 = pair->first;
                item->finalYAt14 = pair->second;
                item->stepXAt18 = item->stepYAt1C = 0;
                const auto mirror = pattern->filterAt40.modeAt04 == 11 ? *pattern->filterAt40.argumentsAt08.begin : 0u;
                if (mirror & 1)
                {
                    item->currentXAt00 = Sub(UnsignedFloat(source.CaptureProgramAt4180C0()->widthAt18), item->currentXAt00);
                    item->targetXAt08 = WrapSub(source.CaptureProgramAt4180C0()->widthAt18, item->targetXAt08);
                }
                if (mirror & 2)
                {
                    item->currentYAt04 = Sub(UnsignedFloat(source.CaptureProgramAt4180C0()->heightAt1C), item->currentYAt04);
                    item->targetYAt0C = WrapSub(source.CaptureProgramAt4180C0()->heightAt1C, item->targetYAt0C);
                }
                if (item->currentXAt00 != SignedFloat(item->targetXAt08) || item->currentYAt04 != SignedFloat(item->targetYAt0C))
                {
                    ComputeSteps(*item, source);
                    if (!item->directionsAt20) { ReleaseLocal(local); continue; }
                    if (!source.SuppressSnapshotAt49CDC7())
                        CaptureMoveSnapshotAt49CDE4(local, *pattern, mirror, resources);
                }
                Slot argument = local;
                if (argument.controlAt08) ++argument.controlAt08->referenceCountAt04;
                try { AppendMoveEffectItemAt49D236(body.itemsAt08, argument, memory); }
                catch (...) { ReleaseLocal(argument); throw; }
                // var_74与var_68是两个独立局部Ref；保留各自control至对应释放时点。
                ReleaseLocal(argument); // 49D267 -> 4A3000：EHFlags=5，动态释放异常终止。
            }
            catch (...) { ReleaseLocal(local); throw; }
            ReleaseLocal(local); // 49D276，state16h动作541E55，不让外层catch吞释放异常。
        }
        auto* const manager = source.CaptureDrawManagerAfter413BE0();
        DrawLayerAt418C10(*manager, 0)->enabledAt0C = 0;
        body.readyAt04 = 1;
    }
    catch (...) { /* 49D451只返回this，保留viewport、已追加前缀及失败前写入。 */ }
    return &body;
}
}

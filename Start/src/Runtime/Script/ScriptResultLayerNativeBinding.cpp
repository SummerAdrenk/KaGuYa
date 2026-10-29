#include <Runtime/ScriptResultLayerNativeBinding.h>
#include <Runtime/ScriptResultLayerListNative.h>
#include <Runtime/ScriptResultLayerNative.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/LayerWindowNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/PatternLayerNativeResourceTarget.h>
#include <Runtime/SecretCgNativeArchive.h>

#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Result = ScriptResultLayerNativeBody34h;

std::uint32_t Difference(const void* end, const void* begin) noexcept
{
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
}

std::uint32_t ReferenceCount(const void* end, const void* begin) noexcept
{
    // 4CB44C/4CCD5D：signed除12后按DWORD作无符号比较，不是size_t指针差。
    return std::bit_cast<std::uint32_t>(std::bit_cast<std::int32_t>(Difference(end, begin)) / 12);
}

std::uint32_t ArgumentCount(const EnvironmentVectorStorage<std::uint32_t>& values) noexcept
{
    return std::bit_cast<std::uint32_t>(std::bit_cast<std::int32_t>(
        Difference(values.end, values.begin)) >> 2);
}

[[noreturn]] void ThrowLayerNumber()
{
    // 4CD0D6：PE 5B5D70的CP932字节，异常类型是char*，不是工具异常或CString。
    static const char message[] = "\x83\x8C\x83\x43\x83\x84\x82\xCC\x8E\x77\x92\xE8\x94\xD4"
        "\x8D\x86\x82\xAA\x82\xA8\x82\xA9\x82\xB5\x82\xA2\x81\x49";
    throw const_cast<char*>(message);
}

PatternLayerNativeBody80h* CapturePattern(IDrawManagerNativeRootSource& source, std::uint32_t index)
{
    auto* const manager = source.CaptureDrawManagerAfter413BE0();
    // 4CB41C不读manager数组的end；必须是实际4Ch，不能走现代LayerAt工具检查。
    auto* const window = ResolveEnvironmentObjectReference<LayerWindowNativeBody4Ch>(
        *manager->ownedLayersAt14.begin);
    const auto* const begin = window->patternsAt30.begin;
    const auto count = ReferenceCount(window->patternsAt30.end, begin);
    if (count < index) ThrowLayerNumber();
    const auto* const slot = reinterpret_cast<const Slot*>(
        reinterpret_cast<std::uintptr_t>(begin) + index * 12u);
    auto* const pattern = ResolveEnvironmentObjectReference<PatternLayerNativeBody80h>(*slot);
    if (!pattern) ThrowLayerNumber();
    return pattern;
}

std::int32_t CaptureLayerCount(ParamsRuntimeRootOwner& owner)
{
    auto* const root = owner.CaptureAt418480();
    auto* const control = static_cast<const EnvironmentObjectReferenceControl*>(
        root->gameSystemAt04->startupConfigBytes.storage.RawSlot().controlAt08);
    // 4CB3DA：只读实际+18的control.count，不要求byteRef有base，也不减bias。
    return std::bit_cast<std::int32_t>(control ? control->countAt10 : 0u);
}

class NativePatternSource final : public IParamsPatternSource
{
public:
    explicit NativePatternSource(ParamsRuntimeRootOwner& owner) noexcept : owner_(owner) {}
    const ParamsPatternNative* CapturePattern() const override
    { return owner_.CaptureAt418480()->patternAt10.get(); }
private:
    ParamsRuntimeRootOwner& owner_;
};

struct ListUnwind final
{
    ScriptResultLayerReferenceList& list;
    ~ListUnwind() noexcept { DestroyScriptResultLayersNativeAt4C9C70(list); }
};

struct ReferenceUnwind final
{
    Slot& slot;
    bool armed = true;
    ~ReferenceUnwind() noexcept
    { if (armed) ReleaseScriptResultLayerReferenceAt4C93A0(slot); }
};

struct SelectedResult final
{
    Slot slot;
    std::uintptr_t cachedBase;
    YtlReferenceControlRuntime* cachedControl;

    Result* Resolve() const noexcept
    {
        if (!cachedBase) return nullptr;
        const auto stride = cachedControl ?
            static_cast<const EnvironmentObjectReferenceControl*>(cachedControl)->strideAt0C : 0u;
        // 4CB68A等：base/control固定，bias与缓存control内的stride仍在每次解析时读取。
        return reinterpret_cast<Result*>(cachedBase + slot.biasAt04 * stride);
    }
};

void SelectResult(SelectedResult& selected, const ScriptResultLayerReferenceList& list,
    std::uint32_t index, const Slot& defaultSlot, YtlReferenceControlRuntime* defaultControl)
{
    const auto* cursor = list.begin;
    const auto* const end = list.end;
    for (; cursor != end; ++cursor)
    {
        const auto base = cursor->baseAt00;
        if (ResolveScriptResultLayerReferenceAt4CA1C0(*cursor)->valueAt04 != index) continue;
        if (cursor == &selected.slot) break;
        auto* const control = cursor->controlAt08;
        selected.slot.baseAt00 = base;
        selected.slot.biasAt04 = cursor->biasAt04;
        selected.cachedBase = base;
        selected.cachedControl = control;
        selected.slot.controlAt08 = control;
        if (control) ++control->referenceCountAt04;
        if (base) return;
        // 4CB628：此分支普通释放仍用缓存control；失败由state9按当前槽展开。
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
        break;
    }
    selected.cachedBase = defaultSlot.baseAt00;
    selected.slot.baseAt00 = selected.cachedBase;
    selected.slot.biasAt04 = 0;
    selected.cachedControl = defaultControl;
    selected.slot.controlAt08 = defaultControl;
    if (defaultControl) ++defaultControl->referenceCountAt04;
}

struct ResourcePlanStorage final
{
    union { PatternResourcePlan plan; };
    bool armed = false;
    ResourcePlanStorage() noexcept {}
    ~ResourcePlanStorage() noexcept { if (armed) plan.~PatternResourcePlan(); }
};

void ReleasePlanNormal(PatternResourcePlan& plan) noexcept
{
    // state2C/43B790：普通尾读当前槽，异常直接终止，不能先展开selected/default/list。
    plan.Reset();
}

void ReleaseSelectedNormal(Slot& selected) noexcept
{ ReleaseScriptResultLayerReferenceAt4D71D0(selected); }

void MarkPrimary(EnvironmentRootOwner& owner, const Result* fixedResult)
{
    auto* const root = owner.CaptureAt4180C0();
    const auto* const end = root->archivesAt30.end;
    const auto* const begin = root->archivesAt30.begin;
    auto* const archive = ReferenceCount(end, begin) > 3u ?
        ResolveEnvironmentObjectReference<SecretCgNativeArchive>(begin[3]) : nullptr;
    // 4CCD7D：先固定Result正文，取根/解registry[3]之后才读取它的item DWORD。
    const auto id = fixedResult->valueAt00;
    auto* cursor = archive->recordsAt0C.begin;
    const auto* const recordsEnd = archive->recordsAt0C.end;
    for (; cursor != recordsEnd; ++cursor)
    {
        if (ResolveSecretCgFlag(*cursor)->idAt00 != id) continue;
        ResolveSecretCgFlag(*cursor)->byteFlagAt04 = 1;
        break;
    }
}

bool SameFilter(const PatternFilterNativeBody20h& target, const PatternFilterNativeBody20h& source)
{
    if (target.modeAt04 != source.modeAt04) return false;
    const auto* const targetBegin = target.argumentsAt08.begin;
    const auto count = ArgumentCount(target.argumentsAt08);
    const auto* const sourceBegin = source.argumentsAt08.begin;
    if (count != ArgumentCount(source.argumentsAt08)) return false;
    for (std::uint32_t i = 0; i != count; ++i)
        if (targetBegin[i] != sourceBegin[i]) return false;
    // 4CCE6A..4CCEA0不比较temporary Ref。
    return true;
}
}

void ApplyScriptResultLayersNativeAt4CB030(ScriptNativeBody& receiver,
    std::int32_t layerGroupIndex, ScriptResultLayerNativeBindingServices& services)
{
    ScriptResultLayerReferenceList list;
    CollectScriptResultLayersNativeAt4CB08D(receiver.parserAtA0, layerGroupIndex, list, services.memory);
    ListUnwind listUnwind{list};

    Slot defaultSlot;
    // default控制块申请失败仍只清列表，不补删已经完成的34h正文。
    ConstructScriptResultLayerReferenceAt4CB27C(defaultSlot, NewScriptResultLayerNativeAt4CB208());
    auto* const defaultControl = defaultSlot.controlAt08;
    ReferenceUnwind defaultUnwind{defaultSlot};

    NativePatternSource params(services.params);
    PatternResourceNativeInitializationSource initialization(services.environment);
    const auto& supplied = services.resources.loads;
    const PatternResourceLoadServices loads{supplied.source, supplied.transaction, supplied.files,
        supplied.memory, supplied.key, initialization, supplied.codecs, services.strings};
    PatternLayerNativeResourceServices resources{loads, services.resources.surfaces,
        services.resources.graphics, services.resources.win32};

    for (std::uint32_t index = 0;
         std::bit_cast<std::int32_t>(index) < CaptureLayerCount(services.params); ++index)
    {
        auto* const pattern = CapturePattern(services.draw, index);
        RECT oldRect{};
        if (pattern->layer.enabledAt0C) ReadPatternBoundsNativeAt4D3490(*pattern, &oldRect);

        SelectedResult selected{};
        ReferenceUnwind selectedUnwind{selected.slot};
        SelectResult(selected, list, index, defaultSlot, defaultControl);
        auto item = pattern->itemIndexAt14;
        if (static_cast<std::uint32_t>(item) != selected.Resolve()->valueAt00)
        {
            if (std::bit_cast<std::int32_t>(selected.Resolve()->valueAt00) < 0)
            {
                {
                    GraphicsSurfaceNativeScope scope(resources.surfaces);
                    static_cast<void>(ClearPatternSpritesNativeAt442700(*pattern, -1));
                }
                InvalidatePatternLayerNativeAt443650(*pattern, &oldRect);
            }
            else
            {
                if (pattern->filterAt40.modeAt04)
                {
                    pattern->filterAt40.modeAt04 = 0;
                    pattern->filterAt40.argumentsAt08.end = pattern->filterAt40.argumentsAt08.begin;
                }
                const auto* const fixedResult = selected.Resolve();
                const auto* const model = params.CapturePattern();
                ResourcePlanStorage storage;
                ::new (static_cast<void*>(&storage.plan)) PatternResourcePlan(
                    BuildPatternResourcePlanNativeAt4AD900(*model, fixedResult->valueAt00, services.strings));
                storage.armed = true;
                PatternLayerNativeResourceTarget target(*pattern, resources);
                ExecutePatternResourcePlanNative(params, target, storage.plan, services.strings);
                if (!index) MarkPrimary(services.environment, selected.Resolve());
                storage.armed = false;
                ReleasePlanNormal(storage.plan);
            }
            item = std::bit_cast<std::int32_t>(selected.Resolve()->valueAt00);
            pattern->itemIndexAt14 = item;
        }

        if (item >= 0)
        {
            if (!SameFilter(pattern->filterAt40, selected.Resolve()->filterAt14))
            {
                AssignPatternFilterNativeAt4CA040(pattern->filterAt40, selected.Resolve()->filterAt14,
                    services.memory);
                pattern->pendingAt60 = 1;
                ApplyPatternAnimationCommandsNativeAt4CAF10(*pattern);
                RECT changed;
                ReadPatternBoundsNativeAt4D3490(*pattern, &changed);
                InvalidatePatternLayerNativeAt443650(*pattern, &changed);
            }
            const auto* position = selected.Resolve();
            if (pattern->layer.valueAt04 != static_cast<std::uint32_t>(position->valueAt08) ||
                pattern->layer.valueAt08 != static_cast<std::uint32_t>(position->valueAt0C))
            {
                InvalidatePatternLayerNativeAt443650(*pattern, &oldRect);
                position = selected.Resolve();
                const auto x = static_cast<std::uint32_t>(position->valueAt08);
                const auto y = static_cast<std::uint32_t>(position->valueAt0C);
                pattern->layer.valueAt04 = x;
                pattern->layer.valueAt08 = y;
                RECT changed;
                ReadPatternBoundsNativeAt4D3490(*pattern, &changed);
                InvalidatePatternLayerNativeAt443650(*pattern, &changed);
            }
        }

        if (pattern->resultAt7D != selected.Resolve()->valueAt10)
            pattern->resultAt7D = selected.Resolve()->valueAt10;
        pattern->hasItemAt7C = static_cast<std::uint8_t>(selected.Resolve()->valueAt00 != 0xFFFFFFFFu);
        selectedUnwind.armed = false;
        ReleaseSelectedNormal(selected.slot);
    }
    // 4CD0EA..4CD0FB：default后list；两者均按当前槽进入原终止清理边界。
}
}

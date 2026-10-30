#include "QuickOperationsNative.Internal.h"
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <bit>
#include <cstring>
#include <optional>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace { thread_local QuickOperationsNativeServices* services = nullptr; }
QuickOperationsNativeScope::QuickOperationsNativeScope(QuickOperationsNativeServices& value) noexcept
    : previous_(services), confirmation_(value.confirmation) { services = &value; }
QuickOperationsNativeScope::~QuickOperationsNativeScope() noexcept { services = previous_; }
QuickOperationsNativeServices& RequireQuickOperationsNativeServices()
{
    if (!services) throw std::logic_error("Quick operations native services are not bound");
    return *services;
}

namespace QuickOperationsNativeDetail
{
ATL::CStringData* Header(const char* chars) noexcept
{ return reinterpret_cast<ATL::CStringData*>(reinterpret_cast<std::uintptr_t>(chars) - 0x10u); }
void PublishText(RuntimeCString& text, ATL::CStringData* header) noexcept
{
    const auto chars = reinterpret_cast<std::uintptr_t>(header) + 0x10u;
    // 只发布调用方已取得的引用指针，Clone/释放责任不在此处增减。
    std::memcpy(static_cast<void*>(&text), &chars, sizeof(chars));
}
TextLifetime::~TextLifetime() noexcept { if (armed) DestroyRuntimeCStringAt40CD40(&text); }
void TextLifetime::FinishCached(const char* chars)
{
    armed = false;
    ReleaseRuntimeCStringHeader(Header(chars));
}
FactoryLifetime::~FactoryLifetime() noexcept
{ if (armed) DestroySaveFileDeleteFactoryNativeAt463A90(body); }
void FactoryLifetime::Finish()
{
    armed = false;
    DestroySaveFileDeleteFactoryNativeAt463A90(body);
}
void ReleaseConfirmationNormally(YtlReferenceSlotRuntime& slot) noexcept
{
    auto* control = slot.controlAt08;
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        // 485E3E/48611E在payload回调后重读局部control；不缓存删除目标、不清槽。
        if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    }
}
ConfirmationLifetime::~ConfirmationLifetime() noexcept { if (armed) ReleaseYtlReference(slot); }
void ConfirmationLifetime::FinishNormally() noexcept
{
    armed = false;
    ReleaseConfirmationNormally(slot);
}
bool Confirm(std::int32_t mode)
{
    auto& deps = RequireQuickOperationsNativeServices();
    auto& source = deps.source;
    // 声明先于Ref守卫：正常和EH最后一次Ref释放都仍处于显式Program服务Scope内。
    std::optional<GeneralConfirmDialogNativeServices> fixed;
    std::optional<GeneralConfirmDialogNativeScope> programScope;
    YtlReferenceSlotRuntime reference;
    CreateGeneralConfirmDialogNativeReference(reference, mode);
    ConfirmationLifetime unwind{reference};
    // 485D95/486075成功之后才取Root并consume；不得提前consumeRootTransition。
    source.EnsureRootAt413BE0();
    void* captureRoot = source.RootAt6198C4();
    YtlReferenceSlotRuntime argument;
    ConsumeGraphicsSurfaceReferenceAt477C60(argument, source.TransitionFrameAt1F0(captureRoot));
    auto* dialog = ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(reference);
    AssignGeneralConfirmDialogNativeCaptureAt48CA30(dialog, argument); // 参数由setter消费。

    source.EnsureRootAt413BE0();
    void* programRoot = source.RootAt6198C4();
    auto& modal = source.ModalForProgramAt3C(programRoot);
    auto& base = deps.confirmation;
    fixed.emplace(GeneralConfirmDialogNativeServices{base.pictures, base.surfaces, base.drawing,
        base.graphics, base.input, base.screen, base.root, modal, base.strings});
    programScope.emplace(*fixed);
    const auto result = RunGeneralConfirmDialogNativeAt5071D0(reference);
    // state2仅终止处理，不再具有state1的Ref EH资格；先释放，再接受/拒绝。
    unwind.FinishNormally();
    return result == 6;
}
std::int32_t Increment32(std::int32_t value) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(value) + 1u); }
void* CaptureInlineArchive(std::uint32_t index, bool beginFirst, bool biasFirst)
{
    auto* env = RequireQuickOperationsNativeServices().source.CaptureEnvironmentAt4180C0();
    const volatile auto& vector = env->archivesAt30;
    YtlReferenceSlotRuntime* begin;
    YtlReferenceSlotRuntime* end;
    if (beginFirst) { begin = vector.begin; end = vector.end; }
    else { end = vector.end; begin = vector.begin; }
    // 488AF7保留用于解槽的begin，488B00另读begin计算长度；其它站点只读一次。
    const auto countBegin = beginFirst ? vector.begin : begin;
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(countBegin));
    const auto count = std::uint32_t(std::bit_cast<std::int32_t>(bytes) / 12);
    if (count <= index) return nullptr;
    const volatile auto& slot = begin[index];
    const auto base = slot.baseAt00;
    if (!base) return nullptr;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
    std::uint32_t bias, stride;
    if (biasFirst && control) { bias = slot.biasAt04; stride = control->strideAt0C; }
    else { stride = control ? control->strideAt0C : 0u; bias = slot.biasAt04; }
    return reinterpret_cast<void*>(base + bias * stride);
}
std::int32_t& CaptureLatestInline(bool beginFirst, bool biasFirst)
{
    auto* catalog = CaptureInlineArchive(6, beginFirst, biasFirst);
    return *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(catalog) + 4u);
}
std::int32_t& CaptureLatestVia417990()
{
    auto& source = RequireQuickOperationsNativeServices().source;
    auto* env = source.CaptureEnvironmentAt4180C0();
    auto* catalog = source.RegistryAt417990(env, 6);
    return *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(catalog) + 4u);
}
ScriptParamsNativeBody* CaptureParams()
{
    auto& source = RequireQuickOperationsNativeServices().source;
    source.EnsureRootAt413BE0();
    return static_cast<ScriptParamsNativeBody*>(source.ParamsAt47BDB0(source.RootAt6198C4()));
}
void SetTitleAt48CF90(ScriptParamsNativeBody& params, const char* chars)
{
    // 原setter先解Ref/取+08目标，后扫描首NUL；不采用显式长度归档串赋值。
    auto& target = params.ResolveSaveParamsAt08()->saveTitleAt08;
    const int length = chars ? static_cast<int>(std::strlen(chars)) : 0;
    SetRuntimeCStringAt437680(target, chars, length);
}
}

void ConstructQuickOperationsNativeFactory(QuickOperationsNativeFactoryBody10h& body,
    std::uint8_t preview)
{
    auto& source = RequireQuickOperationsNativeServices().source;
    // 现共享rolling组合固定preview=1并包含第二Set，不能先调用再覆盖preview。
    body.base.vptrAt00 = &BaseSaveFileDeleteFactoryNativeVtable();
    InitializeEmptyRuntimeCString(body.base.prefixAt04, source.Strings());
    QuickOperationsNativeDetail::FactoryLifetime unwind{body.base};
    body.base.vptrAt00 = &RegularSaveFileDeleteFactoryNativeVtable();
    body.base.slotCountAt08 = 0;
    SetRuntimeCStringAt437680(body.base.prefixAt04, "save", 4); // 45B864
    body.base.vptrAt00 = &QuickSaveFileDeleteFactoryNativeVtable();
    body.previewAt0C = preview; // 48615E，仅写低byte。
    body.base.slotCountAt08 = 8;
    SetRuntimeCStringAt437680(body.base.prefixAt04, "qsave", 5); // 485E86/486168
    unwind.armed = false;
}
}

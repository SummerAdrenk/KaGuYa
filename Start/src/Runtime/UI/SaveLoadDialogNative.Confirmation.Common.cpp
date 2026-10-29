#include "SaveLoadDialogNative.Confirmation.Internal.h"
#include <Runtime/ScriptParamsNativeBody.h>

namespace KaGuYa::Start::Runtime::SaveLoadConfirmationDetail
{
SaveFileStateNativeArchive* Catalog(ISaveLoadDialogNativeConfirmationSource& source)
{
    return static_cast<SaveFileStateNativeArchive*>(ResolveEnvironmentArchiveAt417990(
        *source.CaptureEnvironmentAt4180C0(), 6));
}
void* InlineSaveData(ISaveLoadDialogNativeConfirmationSource& source)
{
    auto* env = source.CaptureEnvironmentAt4180C0();
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(env->archivesAt30.end) -
        reinterpret_cast<std::uintptr_t>(env->archivesAt30.begin));
    const auto count = std::uint32_t(std::bit_cast<std::int32_t>(bytes) / 12);
    return count > 1 ? ResolveEnvironmentObjectReference<void>(env->archivesAt30.begin[1]) : nullptr;
}
SaveFileNativeItem* Item(const Slot& page, YtlReferenceControlRuntime* cached, std::int32_t index)
{
    const auto stride = cached ? static_cast<EnvironmentObjectReferenceControl*>(cached)->strideAt0C : 0u;
    const auto* body = reinterpret_cast<const SaveFileNativePage*>(page.baseAt00 + page.biasAt04 * stride);
    const auto address = reinterpret_cast<std::uintptr_t>(body->itemsAt04.begin) + std::uint32_t(index) * 12u;
    return ResolveEnvironmentObjectReference<SaveFileNativeItem>(*reinterpret_cast<const Slot*>(address));
}
void CopyPageReferenceAt4FA0D0(Slot& target, const Slot& source)
{
    if (&target == &source) return;
    ReleaseYtlReference(target); target = source;
    if (target.controlAt08) ++target.controlAt08->referenceCountAt04;
}
GuiDialogNativeBody5Ch* ResolveConfirmation(const Slot& slot) noexcept
{
    return ResolveEnvironmentObjectReference<GuiDialogNativeBody5Ch>(slot);
}
void WriteConfirmationMode(GuiDialogNativeBody5Ch* body, std::int32_t mode)
{
    using Write = void (__thiscall*)(GuiDialogNativeBody5Ch*, std::int32_t);
    const auto* table = *reinterpret_cast<const std::byte* const*>(body);
    (*reinterpret_cast<const Write*>(table + 0x98))(body, mode);
}
void CloneText(RuntimeCString& fresh, const RuntimeCString& source)
{
    auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(source.GetString())) - 1;
    const auto chars = reinterpret_cast<std::uintptr_t>(CloneRuntimeCStringDataAt414540(header)) + 16u;
    std::memcpy(&fresh, &chars, sizeof(chars));
}
void ResolveCurrentDescription(RuntimeCString& label, ISaveLoadDialogNativeConfirmationSource& source)
{
    source.EnsureRootAt413BE0();
    auto* params = static_cast<ScriptParamsNativeBody*>(source.ParamsAt47BDB0(source.RootAt6198C4()));
    const auto index = params->messageFormatIndexAt60;
    if (index == -1)
    {
        source.EnsureRootAt413BE0();
        params = static_cast<ScriptParamsNativeBody*>(source.ParamsAt47BDB0(source.RootAt6198C4()));
        TextSlot title; CloneText(title.value, params->SaveParamsAt08().saveTitleAt08);
        TextUnwind unwind{title.value}; const auto* cached = title.value.GetString();
        AssignRuntimeCStringAt410220(label, title.value); unwind.FinishCached(cached);
    }
    else
    {
        Format format;
        auto* save = ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(), 1);
        const auto variant = std::uint32_t(!SaveLoadDefaultNameMatchesNativeAt50FEA0(save, source));
        source.EnsureRootAt413BE0();
        auto* files = ResolveEnvironmentObjectReference<void>(source.RootFileManagerSlotAt00(source.RootAt6198C4()));
        auto* messages = ResolveEnvironmentObjectReference<void>(source.FileManagerMessageSlotAt10(files));
        if (source.ReadFormatAt417C40(messages, index, format.Target(), variant, false) < 0 || !format.body.baseAt00) throw 1;
        TextSlot text; const auto* result = ReadMessageText(ResolveEnvironmentObjectReference<void>(format.body), &text.value);
        TextUnwind textUnwind{text.value};
        TextSlot expanded; const auto* labelResult = ExpandSaveLoadLabelNativeAt4DC660(&expanded.value, result->GetString(), source);
        TextUnwind expandedUnwind{expanded.value};
        AssignChars(label, labelResult->GetString());
        expandedUnwind.Finish(); textUnwind.Finish(); format.Finish();
    }
}
Labels::Labels(ISaveLoadDialogNativeConfirmationSource& source)
{
    struct PrefixUnwind final
    {
        Labels* value;
        ~PrefixUnwind() noexcept { if (value) value->DestroyCurrent(); }
    } unwind{this};
    RuntimeCString* values[]{&page.value, &slot.value, &time.value, &scene.value};
    for (auto* value : values) { InitializeEmptyRuntimeCString(*value, source.Strings()); ++completed_; }
    unwind.value = nullptr;
}
void Labels::DestroyCurrent() noexcept
{
    RuntimeCString* values[]{&page.value, &slot.value, &time.value, &scene.value};
    while (completed_) DestroyRuntimeCStringAt40CD40(values[--completed_]);
}
Labels::~Labels() noexcept { DestroyCurrent(); }
void Labels::Configure(GuiDialogNativeBody5Ch* body, std::uint8_t saveMode,
    ISaveLoadDialogNativeConfirmationSource& source)
{
    TextSlot d, c, b, a;
    // 525B21/52107C：从scene向page反序构造按值参数，同时固定caller正常尾header。
    cached_[3] = scene.value.GetString(); CloneText(d.value, scene.value); TextUnwind dd{d.value};
    cached_[2] = time.value.GetString(); CloneText(c.value, time.value); TextUnwind cc{c.value};
    cached_[1] = slot.value.GetString(); CloneText(b.value, slot.value); TextUnwind bb{b.value};
    cached_[0] = page.value.GetString(); CloneText(a.value, page.value); TextUnwind aa{a.value};
    aa.armed = bb.armed = cc.armed = dd.armed = false;
    source.ConfigureAt535FF0(body, a.value, b.value, c.value, d.value, saveMode);
}
void Labels::Finish()
{
    RuntimeCString* values[]{&page.value, &slot.value, &time.value, &scene.value};
    // 原normal场景退scene/time/slot/page；异常前先撤当前资格，不重试失败元素。
    while (completed_)
    {
        const auto index = --completed_;
        ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(cached_[index])) - 1);
    }
}
}

#include "AutoSaveOperationsNative.Internal.h"
#include <Runtime/EnvironmentObjectReference.h>

#include <bit>
#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace { thread_local AutoSaveOperationsNativeServices* services = nullptr; }
AutoSaveOperationsNativeScope::AutoSaveOperationsNativeScope(AutoSaveOperationsNativeServices& value) noexcept
    : previous_(services) { services = &value; }
AutoSaveOperationsNativeScope::~AutoSaveOperationsNativeScope() noexcept { services = previous_; }
AutoSaveOperationsNativeServices& RequireAutoSaveOperationsNativeServices()
{
    if (!services) throw std::logic_error("AutoSave operations native services are not bound");
    return *services;
}

namespace AutoSaveOperationsNativeDetail
{
ATL::CStringData* Header(const char* chars) noexcept
{ return reinterpret_cast<ATL::CStringData*>(reinterpret_cast<std::uintptr_t>(chars) - 0x10u); }
void PublishText(RuntimeCString& text, ATL::CStringData* header) noexcept
{
    const auto chars = reinterpret_cast<std::uintptr_t>(header) + 0x10u;
    // 只发布已有引用的原生指针，保持TextLifetime及缓存header的释放方式。
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
std::int32_t Increment32(std::int32_t value) noexcept
{ return std::bit_cast<std::int32_t>(std::uint32_t(value) + 1u); }

void* CaptureInlineArchive(std::uint32_t index, bool beginFirst, bool biasFirst)
{
    auto* env = RequireAutoSaveOperationsNativeServices().source.CaptureEnvironmentAt4180C0();
    const volatile auto& vector = env->archivesAt30;
    YtlReferenceSlotRuntime* begin;
    YtlReferenceSlotRuntime* end;
    if (beginFirst) { begin = vector.begin; end = vector.end; }
    else { end = vector.end; begin = vector.begin; }
    // 488627保留解槽begin，488630再次读begin计算长度；其它站点不增加此读取。
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
    return *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(catalog) + 8u);
}
std::int32_t& CaptureLatestVia417990()
{
    auto& source = RequireAutoSaveOperationsNativeServices().source;
    auto* env = source.CaptureEnvironmentAt4180C0();
    auto* catalog = source.RegistryAt417990(env, 6);
    return *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(catalog) + 8u);
}
ScriptParamsNativeBody* CaptureParams()
{
    auto& source = RequireAutoSaveOperationsNativeServices().source;
    source.EnsureRootAt413BE0();
    void* root = source.RootAt6198C4();
    return static_cast<ScriptParamsNativeBody*>(source.ParamsAt47BDB0(root));
}
void SetTitleAt48CF90(ScriptParamsNativeBody& params, const char* chars)
{
    // 先解捕获的Params+08 Ref/取标题槽，再扫描首NUL；不采用归档显式长度。
    auto& title = params.ResolveSaveParamsAt08()->saveTitleAt08;
    const int length = chars ? static_cast<int>(std::strlen(chars)) : 0;
    SetRuntimeCStringAt437680(title, chars, length);
}
}

void ConstructAutoSaveOperationsNativeFactory(AutoSaveOperationsNativeFactoryBody10h& body,
    std::uint8_t preview)
{
    auto& source = RequireAutoSaveOperationsNativeServices().source;
    body.base.vptrAt00 = &BaseSaveFileDeleteFactoryNativeVtable();
    InitializeEmptyRuntimeCString(body.base.prefixAt04, source.Strings());
    AutoSaveOperationsNativeDetail::FactoryLifetime unwind{body.base};
    body.base.vptrAt00 = &RegularSaveFileDeleteFactoryNativeVtable();
    body.base.slotCountAt08 = 0;
    SetRuntimeCStringAt437680(body.base.prefixAt04, "save", 4); // 45B864
    // 45B800已成功；46B2BA state0在第二Set之前生效，不能先调用固定preview=1组合。
    body.base.vptrAt00 = &AutoSaveFileDeleteFactoryNativeVtable();
    body.previewAt0C = preview;
    body.base.slotCountAt08 = 8;
    SetRuntimeCStringAt437680(body.base.prefixAt04, "asave", 5); // 46B2D2
    unwind.armed = false;
}

std::int32_t RollingAutoSaveNativeAt4883F0(AutoSaveOperationsNativeFactoryBody10h& factory)
{
    using namespace AutoSaveOperationsNativeDetail;
    auto& source = RequireAutoSaveOperationsNativeServices().source;
    const auto previous = CaptureLatestInline(); // 48841B: Env A
    auto& incrementTarget = CaptureLatestInline(); // 48846C: Env B，不复用A。
    incrementTarget = Increment32(incrementTarget);
    if (incrementTarget >= 8) CaptureLatestVia417990() = 0; // 4884C2/4884C9: Env C

    TextSlot oldTitle;
    const char* oldChars = CaptureParams()->ResolveSaveParamsAt08()->saveTitleAt08.GetString();
    PublishText(oldTitle.value, CloneRuntimeCStringDataAt414540(Header(oldChars)));
    const char* const savedTitle = oldTitle.value.GetString(); // 48851A: 缓存ESI。
    TextLifetime oldLifetime{oldTitle.value}; // state0；Clone失败尚无清理资格。
    TextSlot newTitle;
    InitializeEmptyRuntimeCString(newTitle.value, source.Strings());
    TextLifetime newLifetime{newTitle.value}; // state1；GetNil失败只展开旧标题。
    FormatRuntimeCStringAt417130(newTitle.value, AutoSaveOperationsNativeTitleFormat(),
        Increment32(CaptureLatestInline())); // 488545: 独立Env/目录+08

    source.EnsureRootAt413BE0(); // 4885AB先Ensure，随后才缓存EDI。
    const char* const formattedTitle = newTitle.value.GetString();
    void* root = source.RootAt6198C4();
    auto* params = static_cast<ScriptParamsNativeBody*>(source.ParamsAt47BDB0(root));
    SetTitleAt48CF90(*params, formattedTitle);

    auto* save = static_cast<std::byte*>(CaptureInlineArchive(1)); // 4885C6: 独立Env
    std::int32_t mode = 0;
    if (std::uint8_t(save[0x74]) != 0) mode = factory.previewAt0C != 0 ? 2 : 1;
    const auto slot = Increment32(CaptureLatestInline(true, true)); // 488622: begin先读/bias先读
    const bool failed = SaveFileFactoryNativeAt488CF0(factory.base, slot, mode, source) != 0;
    if (failed)
    {
        const auto deleteSlot = Increment32(CaptureLatestVia417990()); // 48868C/488693
        DeleteSaveFileFactorySlotNativeAt476460(factory.base, deleteSlot, 1, source);
        // 删除抛出则没有latest/title恢复资格；DeleteFileA FALSE仍继续。
        CaptureLatestVia417990() = previous; // 4886A6/4886AD，另取Env。
    }
    SetTitleAt48CF90(*CaptureParams(), savedTitle); // 4886C6/4886D2，另取Root/Params。
    // 正常先state0释放缓存EDI，再state-1释放缓存ESI；EH只读局部TEXT，绝不补回滚。
    newLifetime.FinishCached(formattedTitle);
    oldLifetime.FinishCached(savedTitle);
    return failed ? 1 : 0;
}

void __stdcall AutoSaveNativeAt46B280(std::uint32_t preview)
{
    using namespace AutoSaveOperationsNativeDetail;
    auto& deps = RequireAutoSaveOperationsNativeServices();
    AutoSaveOperationsNativeFactoryBody10h factory;
    ConstructAutoSaveOperationsNativeFactory(factory, std::uint8_t(preview));
    FactoryLifetime lifetime{factory.base}; // 46B2DC state1，462B50与463A90正文等价。
    auto* params = CaptureParams(); // 46B2E3/46B2EE
    SetRuntimeCStringAt437680(params->ResolveSaveParamsAt08()->saveDescriptionAt04, "", 0);
    if (RollingAutoSaveNativeAt4883F0(factory) == 0)
    {
        const auto item = CaptureLatestVia417990(); // 46B333/46B33A；刷新用零基index。
        RefreshSaveFileCatalogItemNativeAt48E5F0(-2, item, deps.catalog, deps.memory);
    }
    lifetime.Finish(); // 46B353先state-1，正常Free首异常外传且不重试。
}
}

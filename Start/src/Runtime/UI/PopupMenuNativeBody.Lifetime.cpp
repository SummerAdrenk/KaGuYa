#include <Runtime/PopupMenuNativeBody.h>
#include <Runtime/PopupMenuNativeSample18Data.h>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local PopupMenuNativeDependencies* activeDependencies;
using Body = GuiDialogNativeBody5Ch;
Body* __fastcall Delete(Body* common, void*, std::uint32_t flags)
{
    auto* body = reinterpret_cast<PopupMenuNativeBodyA4h*>(common);
    DestroyPopupMenuAt4F8CB0(body);
    if (flags & 1) ::operator delete(body, std::size_t{0xA4});
    return common;
}
std::int32_t __fastcall Result(Body*, void*) { return -1; }
class PopupControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit PopupControl(PopupMenuNativeBodyA4h* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        if (auto* body = static_cast<PopupMenuNativeBodyA4h*>(payloadAt14))
            body->common.vptrAt00->deletingDestructorAt04(&body->common, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(PopupControl) == 0x18);
struct ConstructionUnwind final
{
    PopupMenuNativeBodyA4h* body;
    bool active = true;
    ~ConstructionUnwind() noexcept
    {
        if (!active) return;
        DestroyGuiPopupOpaqueStorageAt4F6A90(body->recordsAt98);
        DestroyGuiPopupOpaqueStorageAt4F6A90(body->recordsAt8C);
        DestroyGuiClickedEventReferencesAt4F6D10(body->eventsAt80);
        ReleaseYtlReference(body->capturedAt5C);
        DestroyGuiDialogAt4F7AC0(&body->common);
    }
};
}
const GuiDialogNativeVtable35& PopupMenuNativeVtable() noexcept
{
    static const auto table = []
    {
        auto value = GuiDialogBaseVtable();
        // 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
        // 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
#define SLOT(field, function) value.field = reinterpret_cast<decltype(value.field)>(&function)
        SLOT(deletingDestructorAt04, Delete);
        SLOT(activeInputAt38, PopupMenuNativeEntries::ActiveInput);
        SLOT(resetInputAt3C, PopupMenuNativeEntries::Reset);
        SLOT(beginAt44, PopupMenuNativeEntries::Begin);
        SLOT(pollAt48, PopupMenuNativeEntries::Poll);
        SLOT(endAt4C, PopupMenuNativeEntries::End);
        SLOT(resultAt50, Result);
        SLOT(processInputAt54, PopupMenuNativeEntries::Input);
        SLOT(flushDirtyAt58, PopupMenuNativeEntries::Flush);
#undef SLOT
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
        return value;
    }();
    return table;
}
PopupMenuNativeScope::PopupMenuNativeScope(PopupMenuNativeDependencies& deps) noexcept
    : previous_(activeDependencies), surfaces_(deps.surfaces), graphics_(deps.graphics),
      input_(deps.input), screen_(deps.screen)
{ activeDependencies = &deps; }
PopupMenuNativeScope::~PopupMenuNativeScope() noexcept { activeDependencies = previous_; }
PopupMenuNativeDependencies& RequirePopupMenuNativeDependencies()
{
    if (!activeDependencies) throw std::logic_error("Popup native dependencies have not been bound");
    return *activeDependencies;
}
PopupMenuNativeBodyA4h* ConstructPopupMenuAt4E9FF0(PopupMenuNativeBodyA4h* body)
{
    auto& deps = RequirePopupMenuNativeDependencies();
    ConstructGuiDialogAt4E0BE0(&body->common, nullptr, -1);
    body->common.vptrAt00 = &PopupMenuNativeVtable();
    body->capturedAt5C = {};
    body->constantAt74 = 1;
    body->eventsAt80 = {};
    body->recordsAt8C = {};
    body->recordsAt98 = {};
    // 68/6C/70/78/7C在原构造中不写；不能用清零整个A4h代替成员初始化。
    ConstructionUnwind unwind{body};
    GuiFactoryConstructionLifetime factory;
    const auto statics = PopupNativeStaticDescriptorsSample18();
    const auto checks = PopupNativeCheckBoxDescriptorsSample18();
    PopulatePopupNativeControlsAt4EA0D1(&body->common, body->common.childrenAt0C, body->eventsAt80,
        std::span<const GuiStaticDescriptorNative24h, 12>(statics.data(), 12),
        std::span<const GuiCheckBoxDescriptorNative2Ch, 9>(checks.data(), 9),
        PopupMenuCheckStampsSample18(), deps.loader, deps.surfaces, deps.copy, deps.graphics, deps.strings);
    AdoptGuiInputAt536EA0(body->common.inputAt18, NewGameGuiInputAt4EAAED());
    factory.Finish();
    unwind.active = false;
    return body;
}
void DestroyPopupMenuAt4F8CB0(PopupMenuNativeBodyA4h* body) noexcept
{
    body->common.vptrAt00 = &PopupMenuNativeVtable();
    DestroyGuiPopupOpaqueStorageAt4F6A90(body->recordsAt98);
    DestroyGuiPopupOpaqueStorageAt4F6A90(body->recordsAt8C);
    DestroyGuiClickedEventReferencesAt4F6D10(body->eventsAt80);
    ReleaseYtlReference(body->capturedAt5C);
    DestroyGuiDialogAt4F7AC0(&body->common);
}
PopupMenuNativeBodyA4h* GetPopupMenuAt47C8A0(YtlReferenceSlotRuntime& rootSlot)
{
    if (!rootSlot.baseAt00)
    {
        void* allocation = ::operator new(0xA4);
        PopupMenuNativeBodyA4h* body;
        try { body = allocation ? ConstructPopupMenuAt4E9FF0(static_cast<PopupMenuNativeBodyA4h*>(allocation)) : nullptr; }
        catch (...)
        {
            struct RawUnwind
            {
                void* allocation;
                ~RawUnwind() noexcept { ::operator delete(allocation, std::size_t{0xA4}); }
            } unwind{allocation};
            throw;
        }
        // 47C902之后新正文不再受回滚保护；沿固定接收者释放当前旧槽。
        ReleaseYtlReference(rootSlot);
        if (body)
        {
            if (rootSlot.controlAt08) rootSlot.controlAt08->DeleteControlAtVslot04(1);
            rootSlot.biasAt04 = 0;
            rootSlot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
            void* rawControl = ::operator new(0x18);
            auto* control = rawControl ? ::new(rawControl) PopupControl(body) : nullptr;
            rootSlot.controlAt08 = control;
            // 原47C987对null写+04；现代分配器异常已按原序外传，非标准null返回在此明确终止。
            // 此安全故障边界不是原程序调用invalid_parameter的证据。
            if (!control) _invalid_parameter_noinfo_noreturn();
            control->referenceCountAt04 = 1;
            rootSlot.controlAt08->owningAt08 = 1;
            static_cast<EnvironmentObjectReferenceControl*>(rootSlot.controlAt08)->strideAt0C = 0xA4;
            static_cast<EnvironmentObjectReferenceControl*>(rootSlot.controlAt08)->countAt10 = 1;
        }
    }
    return ResolveEnvironmentObjectReference<PopupMenuNativeBodyA4h>(rootSlot);
}
}

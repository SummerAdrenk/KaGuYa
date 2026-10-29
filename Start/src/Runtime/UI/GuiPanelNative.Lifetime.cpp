#include "GuiPanelNative.Internal.h"
#include <array>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace GuiPanelNativeDetail;
class PanelControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit PanelControl(Body* p) noexcept
        : EnvironmentObjectReferenceControl(p, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 53D8E0读取当时正文虚表，flags0退化后同样如此。
        if (auto* p = static_cast<Common*>(payloadAt14)) p->vptrAt00->deletingDestructorAt00(p, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t f) override { RetireEnvironmentObjectControl(this, f); }
};
static_assert(sizeof(PanelControl) == 0x18);
constexpr std::array<std::uint32_t, 26> original{
    0x4FDA10, 0x51E640, 0x531230, 0x531D30, 0x531F20, 0x51FA60, 0x5215D0,
    0x500840, 0x51E300, 0x51E310, 0x5101C0, 0x521A70, 0x521AA0, 0x51E630,
    0x50AEE0, 0x50A110, 0x503030, 0x536CE0, 0x50FF30, 0x538670, 0x510620,
    0x5311C0, 0x502920, 0x4FF0E0, 0x4FF290, 0x535B10};
}
const GuiPanelNativeVtable26& GuiPanelVtable() noexcept
{
    using namespace GuiPanelNativeDetail::Entries;
    static const auto table = [] {
        GuiPanelNativeVtable26 t{GuiStaticVtable(),
            reinterpret_cast<decltype(GuiPanelNativeVtable26::addIdAt5C)>(&AddId),
            reinterpret_cast<decltype(GuiPanelNativeVtable26::addNavigationAt60)>(&AddNavigation),
            reinterpret_cast<decltype(GuiPanelNativeVtable26::selectAt64)>(&Select)};
        t.common.deletingDestructorAt00 = reinterpret_cast<decltype(t.common.deletingDestructorAt00)>(&Delete);
        t.common.keyboardAt14 = reinterpret_cast<decltype(t.common.keyboardAt14)>(&Keyboard);
        t.common.writeStateAt44 = reinterpret_cast<decltype(t.common.writeStateAt44)>(&WriteState);
        t.common.writeEnabledAt4C = reinterpret_cast<decltype(t.common.writeEnabledAt4C)>(&WriteEnabled);
        return t;
    }();
    return table;
}
std::span<const std::uint32_t, 26> GuiPanelOriginalVtableSample18() noexcept { return original; }

GuiPanelNativeBody6Ch* ConstructGuiPanelInlineAt504F20(void* storage, void* parent,
    std::int32_t event, YtlReferenceSlotRuntime ownedGroup)
{
    using namespace GuiPanelNativeDetail;
    LocalReference extra{ownedGroup};
    auto* captured = ownedGroup.controlAt08;
    auto* body = ::new (storage) Body;
    ConstructGuiStaticAt4F4050(&body->common, parent, event, CopyWithControl(ownedGroup, captured));
    body->common.vptrAt00 = &GuiPanelVtable().common;
    body->idsAt50 = {};
    body->navigationAt5C = {};
    body->selectedAt68 = -1;
    body->common.typeAt08 = 11;
    // state3→state1的正常尾是终止边界；此处失败没有Panel析构资格。
    extra.Captured(captured);
    return body;
}
GuiPanelNativeBody6Ch* DeleteGuiPanelAt4FDA10(GuiPanelNativeBody6Ch* body, std::uint32_t flags) noexcept
{
    DestroyGuiNavigationReferencesAt4F6E70(body->navigationAt5C);
    GuiPanelNativeDetail::DestroyIds(body->idsAt50);
    // flags0基类删除恰好提供表退化及event→resources退役，不二次释放正文。
    GuiControlBaseVtable().deletingDestructorAt00(&body->common, 0);
    if (flags & 1) ::operator delete(body, std::size_t{0x6C});
    return body;
}
YtlReferenceSlotRuntime* ConstructGuiPanelReferenceAt504FC4(YtlReferenceSlotRuntime& output,
    GuiPanelNativeBody6Ch* body)
{
    output = {};
    if (!body) return &output;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* storage = ::operator new(0x18);
    auto* control = storage ? ::new (storage) PanelControl(body) : nullptr;
    output.controlAt08 = control;
    // 50500D在分配返回NULL时保留原写故障，不补回收孤立正文。
    control->referenceCountAt04 = 1;
    output.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0x6C;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
    return &output;
}
namespace GuiPanelNativeDetail::Entries
{
Common* __fastcall Delete(Common* p, void*, std::uint32_t flags) noexcept
{ return &DeleteGuiPanelAt4FDA10(&Panel(p), flags)->common; }
}
}

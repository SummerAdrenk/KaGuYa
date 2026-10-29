#include "ProgramSelectDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramSelectNativeDetail;
template<bool System, bool Mode01>
class ChildControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ChildControl(Common* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        if (auto* body = static_cast<Common*>(payloadAt14))
            Read(body->vptrAt00)->deletingDestructorAt04(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ChildControl<false, false>) == 0x18);
struct ConstructorFrame final
{
    Body& body;
    union { Slot temporary; };
    void* raw = nullptr;
    std::size_t mainSize;
    int state = -1;
    ConstructorFrame(Body& b, std::size_t bytes) noexcept : body(b), mainSize(bytes) {}
    ~ConstructorFrame() noexcept
    {
        switch (state)
        {
        case -1: return;
        case 1: ::operator delete(raw, mainSize); break;
        case 2: ReleaseYtlReference(temporary); break;
        case 4: ::operator delete(raw, std::size_t{0x68}); break;
        case 5: ReleaseYtlReference(temporary); break;
        case 7: ::operator delete(raw, std::size_t{0x24}); break;
        case 3: case 6: std::terminate();
        default: break;
        }
        DestroyProgramSelectDialogNativeAt4F8D50(body);
    }
};
template<bool System, bool Mode01>
void AppendChild(ConstructorFrame& frame)
{
    auto& source = RequireProgramSelectDialogNativeServices().source;
    constexpr std::size_t bytes = System ? 0x68 : Mode01 ? 0x88 : 0xC0;
    frame.raw = ::operator new(bytes);
    frame.state = System ? 4 : 1;
    Common* created = nullptr;
    if (frame.raw)
    {
        if constexpr (System && Mode01) created = source.ConstructSystem01At4DFCA0(frame.raw, &Base(frame.body), 0);
        else if constexpr (System) created = source.ConstructSystem00At4DF420(frame.raw, &Base(frame.body), 0);
        else if constexpr (Mode01) created = source.ConstructMain01At4DE520(frame.raw, &Base(frame.body), 1);
        else created = source.ConstructMain00At4DCEB0(frame.raw, &Base(frame.body), 1);
    }
    frame.state = 0;
    frame.temporary = {};
    if (created)
    {
        frame.temporary.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
        void* raw = ::operator new(0x18);
        auto* control = raw ? ::new(raw) ChildControl<System, Mode01>(created) : nullptr;
        // 原局部control先初始化，再发布到临时Ref；失败不补删已完成child。
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = static_cast<std::uint32_t>(bytes);
        frame.temporary.controlAt08 = control;
        control->countAt10 = 1;
    }
    auto* const captured = frame.temporary.controlAt08;
    frame.state = System ? 5 : 2;
    GuiDialogIntoBaseVtable().appendDialogAt8C(&Base(frame.body), Copy(frame.temporary));
    frame.state = System ? 6 : 3;
    ReleaseCaptured(captured);
    frame.state = 0;
}
template<bool Mode01> Body* ConstructTop(void* raw)
{
    LeafScope leaves;
    auto* body = ConstructProgramSelectDialogNativeAt4EAB70(raw);
    ConstructorFrame frame{*body, Mode01 ? 0x88u : 0xC0u};
    frame.state = 0;
    Base(*body).vptrAt00 = &(Mode01 ? ProgramSelectDialogNativeTop01Vtable() :
        ProgramSelectDialogNativeTop00Vtable()).inherited.common;
    auto& source = leaves.services.source;
    const auto height = Read(source.CaptureEnvironmentAt4180C0(Mode01 ? 0x4DF11E : 0x4DE21E)->heightAt1C);
    const auto width = Read(source.CaptureEnvironmentAt4180C0(Mode01 ? 0x4DF126 : 0x4DE226)->widthAt18);
    body->boundsAt74 = {0, 0, std::bit_cast<std::int32_t>(width), std::bit_cast<std::int32_t>(height)};
    AppendChild<false, Mode01>(frame);
    AppendChild<true, Mode01>(frame);
    auto* root = Root(source, Mode01 ? 0x4DF2EC : 0x4DE3EC, Mode01 ? 0x4DF2F1 : 0x4DE3F1);
    auto* draw = Resolve(source.RootDrawSlotAt18(root));
    if (!source.LoadSurfaceAt480000(draw, &body->blackAt98, "bg_black.bgd",
        Mode01 ? 0x4DF324 : 0x4DE424)) throw 1;
    frame.raw = ::operator new(0x24);
    frame.state = 7;
    auto* surface = frame.raw ? ::new(frame.raw) GraphicsSurfaceNativeBody(leaves.services.surfaces, leaves.services.strings) : nullptr;
    frame.state = 0;
    AdoptGraphicsSurfaceAt407A80(body->canvasAtA4, surface);
    const auto canvasHeight = Read(source.CaptureEnvironmentAt4180C0(Mode01 ? 0x4DF361 : 0x4DE461)->heightAt1C);
    const auto canvasWidth = Read(source.CaptureEnvironmentAt4180C0(Mode01 ? 0x4DF36C : 0x4DE46C)->widthAt18);
    if (!Resolve<GraphicsSurfaceNativeBody>(body->canvasAtA4)->CreateAt415010(leaves.services.surfaces,
        std::bit_cast<std::int32_t>(canvasWidth), std::bit_cast<std::int32_t>(canvasHeight), 0, 1)) throw 1;
    AdoptGuiInputAt536EA0(Base(*body).inputAt18, NewGameGuiInputAt4EAAED());
    frame.state = -1;
    return body;
}
}
ProgramSelectDialogNativeBodyECh* ConstructProgramSelectDialogNativeAt4EAB70(void* raw)
{
    using namespace ProgramSelectNativeDetail;
    LeafScope leaves;
    auto* body = ::new(raw) Body;
    ConstructGuiDialogAt4E0BE0(&Base(*body), nullptr, -1);
    body->into.dialogsAt5C = {};
    Base(*body).vptrAt00 = &ProgramSelectDialogNativeBaseVtable().inherited.common;
    body->boundsAt74 = {};
    for (auto& value : body->initializedUnknownAt84) value = 0;
    ::new(&body->blackAt98) Slot;
    ::new(&body->canvasAtA4) Slot;
    ResetOperation(body->operationAtB0);
    return body;
}
ProgramSelectDialogNativeBodyECh* ConstructProgramSelectDialogNativeAt4DE1E0(void* raw)
{ return ConstructTop<false>(raw); }
ProgramSelectDialogNativeBodyECh* ConstructProgramSelectDialogNativeAt4DF0E0(void* raw)
{ return ConstructTop<true>(raw); }
ProgramSelectDialogNativeBodyECh* ConstructProgramSelectDialogNativeAt4DE1E0(
    void* raw, ProgramSelectDialogNativeServices& services)
{ ProgramSelectDialogNativeScope scope{services}; return ConstructProgramSelectDialogNativeAt4DE1E0(raw); }
ProgramSelectDialogNativeBodyECh* ConstructProgramSelectDialogNativeAt4DF0E0(
    void* raw, ProgramSelectDialogNativeServices& services)
{ ProgramSelectDialogNativeScope scope{services}; return ConstructProgramSelectDialogNativeAt4DF0E0(raw); }
}

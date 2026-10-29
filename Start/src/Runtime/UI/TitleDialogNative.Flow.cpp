#include "TitleDialogNative.Internal.h"

#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace TitleDialogNativeDetail;
class ConfirmationControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ConfirmationControl(void* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        if (payloadAt14)
        {
            using DeleteFunction = void* (__thiscall*)(void*, std::uint32_t);
            auto* table = Field<const std::uintptr_t*>(payloadAt14, 0);
            reinterpret_cast<DeleteFunction>(table[1])(payloadAt14, 1);
        }
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ConfirmationControl) == 0x18);
void CreateConfirmation(Slot& output, ITitleDialogNativeSource& source)
{
    void* raw = ::operator new(0x94);
    void* body = nullptr;
    struct RawUnwind final
    {
        void* raw;bool active=true;
        ~RawUnwind() noexcept{if(active)::operator delete(raw,std::size_t{0x94});}
    } allocation{raw};
    // 522D98 state0：raw94在真实异常展开中释放，不在catch内清理后重抛。
    if (raw) body = source.ConstructConfirmationAt4E4340(raw);
    allocation.active=false;
    // 4D78D0 failure has NO completed-94h-body unwind eligibility.
    output = {};
    if (!body) return;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* controlRaw = ::operator new(0x18);
    auto* control = controlRaw ? ::new(controlRaw) ConfirmationControl(body) : nullptr;
    output.controlAt08 = control;
    control->referenceCountAt04 = 1;
    output.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 0x94;
    static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
}
}
void AdvanceTitleDialogNativeAt51CC70(TitleDialogNativeBodyD8h& body)
{
    using namespace TitleDialogNativeDetail;
    auto& source = RequireTitleDialogNativeServices().source;
    auto& op = body.operationAt90;
    switch (body.transitionPhaseAt60)
    {
    case 0:
        if (body.phaseAt5C == 1)
        {
            body.common.vptrAt00->compositeAt5C(&body.common, Copy(body.canvasAt84), nullptr);
            ResetOperation(op);
            op.opcode = 57;
            op.source = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.canvasAt84);
            auto* params = Resolve(RefField(Registry(source, 1), 0x1C));
            const auto* name = source.TitleBgmAt61A654();
            auto* soundRoot = Root(source);
            auto* sound = Resolve(source.RootSoundSlotAt24(soundRoot));
            const auto volume = *source.BgmVolumeAtVslot14(params);
            auto& currentName = Field<RuntimeCString>(Registry(source, 2), 0x2C);
            SetRuntimeCStringAt437680(currentName, name, name ? static_cast<int>(std::strlen(name)) : 0);
            (void)source.PlayBgmAt42BFE0(sound, name, volume, 1);
            auto* clearRoot = Root(source);
            (void)source.ClearSoundAt4384D0(Resolve(source.RootSoundSlotAt24(clearRoot)));
            auto* stopRoot = Root(source);
            auto* stopReceiver = Resolve(source.RootSoundSlotAt24(stopRoot));
            auto* stopNames = &Field<EnvironmentVectorStorage<RuntimeCString>>(Registry(source, 2), 0x30);
            source.StopSoundAt413270(stopReceiver, stopNames);
            body.nextPhaseAt64 = 3;
        }
        else
        {
            ResetOperation(op);
            op.opcode = 58;
            op.source = ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(body.blackAt78);
            body.nextPhaseAt64 = 4;
        }
        Field<std::uint32_t>(Registry(source, 2), 4) = 0;
        body.stepAtC0 = 20;
        body.transitionPhaseAt60 = 1;
        return;
    case 1:
        if (op.opcode == -1 || op.current >= op.target) body.transitionPhaseAt60 = 2;
        if (source.InputAt4C3430(Registry(source, 5)) || source.InputAt4AA970(Registry(source, 5)))
        {
            op.current = Add(op.current, 256);
            if (op.current >= op.target) op.current = op.target;
        }
        {
            auto* drawRoot = Root(source);
            auto* graphics = Resolve(source.RootDrawSlotAt18(drawRoot));
            auto* transit = ResolveEnvironmentObjectReference<TransitRuntime>(RefField(graphics, 0x20));
            (void)transit->DispatchRawAt449DD0(op);
        }
        op.current = Add(op.current, body.stepAtC0);
        if (op.current >= op.target) op.current = op.target;
        source.CommitAt4B02E0(source.GraphicsAt4182F0(), 0);
        return;
    case 2:
        body.phaseAt5C = body.nextPhaseAt64;
        Field<std::uint32_t>(Registry(source, 2), 4) = 4;
        return;
    default: return;
    }
}
bool ShouldPromptForTitleName(ITitleDialogNativeSource& source, TitleNamePromptGate gate)
{
    using namespace TitleDialogNativeDetail;
    switch (gate)
    {
    case TitleNamePromptGate::EditableNameBits:
    {
        if ((source.NameEditFlags() & 3u) == 0) return false;
        return Field<std::uint8_t>(Registry(source, 1), 0x54) == 0;
    }
    case TitleNamePromptGate::PathSwitchAndSavedPreference:
        // Mamatomo 522D51..522D81：PATH开关短路后才取SaveData，不能提前捕获两个根。
        if (Field<std::uint32_t>(source.ObjectAt50A620(source.SystemAt4180C0()), 0x1C) == 0)
            return false;
        [[fallthrough]];
    case TitleNamePromptGate::SavedPreferenceOnly:
        // Hakoniwa 4FBF70..4FBF8B、Nurse 4FCD8B..4FCDA6不取PATH对象。
        // 重写统一SaveData模型使用+54；不是把原Nurse+58直接套进当前内存。
        return Field<std::uint8_t>(Registry(source, 1), 0x54) == 0;
    default:
        throw std::logic_error("Title name prompt policy is not implemented or audited");
    }
}
void ProcessTitleSelectionNativeAt522BD0(TitleDialogNativeBodyD8h& body)
{
    using namespace TitleDialogNativeDetail;
    auto& source = RequireTitleDialogNativeServices().source;
    if (source.InputAt47F6D0(Root(source)))
    {
        auto* scenesRoot = Root(source);
        if (source.LoadAvailableAt485CE0(Resolve(source.RootScenesSlotAt30(scenesRoot))))
        {
            body.selectionAtD4 = 1;
            body.phaseAt5C = 2;
            body.transitionPhaseAt60 = 0;
        }
        return;
    }
    body.common.vptrAt00->processInputAt54(&body.common);
    const auto event = PollManagerAt50FDB0(Manager(body), 1);
    if (event < 0 || std::uint32_t(event) - 3u > 5u) return;
    const auto selection = std::uint32_t(event) - 3u;
    PlayCue(0, false);
    if (event == 8)
    {
        auto* window = Field<void*>(source.SystemAt4180C0(), 0x2C);
        (void)source.PostMessageAt522D2F(window, 0x10, 0, 0);
        body.phaseAt5C = 5;
        return;
    }
    if (event == 4)
    {
        auto* scene = source.CurrentSceneAt4186D0(Root(source));
        SetRuntimeCStringAt437680(Field<RuntimeCString>(scene, 0x1C),
            ModernTitleDialogLoadSceneName(), 10);
        body.phaseAt5C = 5;
        return;
    }
    if (event == 3 && ShouldPromptForTitleName(source, RequireTitleDialogNativeServices().profile.namePromptGate))
    {
        if (RequireTitleDialogNativeServices().profile.namePromptGate == TitleNamePromptGate::EditableNameBits)
        {
            const auto result = source.RunResourceNamePrompt();
            if (result < 0) throw std::runtime_error("Game resource name dialog failed");
            if (result == 0) { body.phaseAt5C = 5; return; }
            body.selectionAtD4 = selection;
            body.phaseAt5C = 2;
            body.transitionPhaseAt60 = 0;
            return;
        }
        UnwrittenReference target;
        CreateConfirmation(target.slot, source);
        LocalReference lifetime{target.slot};
        auto* modal = source.PrepareModalAt4AD230(Root(source));
        const auto result = source.RunModalAt5071D0(modal,&target.slot);
        if (result != 6)
        {
            body.phaseAt5C = 5;
            lifetime.Reload();
            return;
        }
        lifetime.Reload();
    }
    body.selectionAtD4 = selection;
    body.phaseAt5C = 2;
    body.transitionPhaseAt60 = 0;
}
namespace TitleDialogNativeDetail
{
void __fastcall Begin(Common* common, void*)
{
    auto& body = Dialog(common);
    ClearGuiClickedEventFlagsAt532200(*Manager(body));
    common->vptrAt00->writeEnabledAt24(common, 1);
    common->vptrAt00->resetInputAt3C(common);
    GuiDialogBaseVtable().resetChildrenAt68(common);
    body.phaseAt5C = 0;
    body.selectionAtD4 = 0xFFFFFFFFu;
}
std::int32_t __fastcall Poll(Common* common, void*)
{
    auto& body = Dialog(common);
    switch (body.phaseAt5C)
    {
    case 0:
        body.phaseAt5C = 1;
        body.transitionPhaseAt60 = 0;
        [[fallthrough]];
    case 1: case 2: AdvanceTitleDialogNativeAt51CC70(body); return 1;
    case 3:
        ProcessTitleSelectionNativeAt522BD0(body);
        common->vptrAt00->flushDirtyAt58(common);
        return 1;
    case 4: body.phaseAt5C = -1; return 0;
    case 5:
        GuiDialogBaseVtable().resetChildrenAt68(common);
        // Bunny 4FE041/Pure 5000C1 omit the parent reset; Begin still performs it.
        // Nurse 5027ED/Hakoniwa 507A28/Mamatomo 530A88 retain the direct call.
        switch (RequireTitleDialogNativeServices().profile.phase5Recovery)
        {
        case TitlePhase5Recovery::ResetChildrenOnly: break;
        case TitlePhase5Recovery::ResetChildrenAndParentInput:
            common->vptrAt00->resetInputAt3C(common);
            break;
        default: throw std::logic_error("Title phase5 recovery is not implemented or audited");
        }
        {
            auto* dirty = common->vptrAt00->dirtyAt00(common);
            dirty->vptrAt00->addRectAt08(dirty, &body.boundsAt68);
        }
        body.phaseAt5C = 3;
        common->vptrAt00->flushDirtyAt58(common);
        return 1;
    default: return 0;
    }
}
std::int32_t __fastcall End(Common* common, void*)
{ common->vptrAt00->writeEnabledAt24(common, 0); return 1; }
std::int32_t __fastcall Result(Common* common, void*)
{ return std::bit_cast<std::int32_t>(Dialog(common).selectionAtD4); }
void __fastcall Redraw(Common* common, void*)
{
    auto* dirty = common->vptrAt00->dirtyAt00(common);
    dirty->vptrAt00->addRectAt08(dirty, &Dialog(common).boundsAt68);
    common->vptrAt00->flushDirtyAt58(common);
}
}
}

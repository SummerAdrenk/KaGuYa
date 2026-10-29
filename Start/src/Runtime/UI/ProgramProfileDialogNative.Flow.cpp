#include "ProgramProfileDialogNative.Internal.h"
#include <Runtime/InputRootNative.h>
#include <cstring>

namespace KaGuYa::Start::Runtime::ProgramProfileDetail
{
namespace
{
template<class T> T& Field(void* p, std::size_t offset)
{ return *reinterpret_cast<T*>(static_cast<std::byte*>(p) + offset); }
void* Archive(unsigned index)
{
    auto& s = RequireProgramSelectDialogNativeServices().source;
    return s.RegistryAt417990(s.CaptureEnvironmentAt4180C0(0), index, 0);
}
void Find(Panel& p, Ref& child, int id)
{
    p.base.common.vptrAt00->findChildAt14(&p.base.common, &child.slot, id);
    if (!child.slot.baseAt00) throw std::logic_error("Program profile references an absent child");
}
void State(Panel& p, int id, int value)
{
    Ref r; Find(p, r, id);
    auto* control = Resolve<Child>(r.slot);
    control->vptrAt00->writeStateAt44(control, value);
}
void ReadSkip(Panel& p)
{
    Ref r; Find(p, r, p.owner->mode->system.skipToggle);
    Resolve<GuiCheckBoxNativeBody60h>(r.slot)->checkedAt5C = (Field<unsigned>(Archive(2), 0x14) >> 1) & 1u;
    auto* control = Resolve<Child>(r.slot);
    control->vptrAt00->writeStateAt44(control, 0);
}
int Event(Panel& p)
{
    return RequireProgramSystemDialogNativeServices().source.PollClickedEventManagerAt50FDB0(
        Resolve<GuiClickedEventReferences>(p.base.managerAt5C), 1, 0);
}
bool CancelPressed()
{
    // Hakoniwa 4FDC60: action 0 edge or unsuppressed right-button edge.
    auto* input = RequireProgramMain00DialogNativeServices().source.InputAt418210(0);
    auto* keyboard = Resolve<KeyboardManagerNativeBody438h>(input->keyboardAt18);
    if (!keyboard || keyboard->bufferAt10 > 1) throw std::logic_error("Program confirmation keyboard unavailable");
    const auto buffer = keyboard->bufferAt10;
    for (const auto* p = keyboard->actionsAt14[0].begin; p != keyboard->actionsAt14[0].end; ++p)
        if (p->states[buffer] && !p->states[buffer ^ 1u]) return true;
    auto* mouse = Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
    if (!mouse) throw std::logic_error("Program confirmation mouse unavailable");
    return !mouse->previousAt30[1] && mouse->pressedAt10[1] && !mouse->suppressionAt28;
}
void RouteSystem(Panel& p, int event)
{
    const auto& profile = p.owner->mode->system;
    if (event == profile.readToggle)
    {
        Ref child; Find(p, child, event);
        auto* check = Resolve<GuiCheckBoxNativeBody60h>(child.slot);
        check->checkedAt5C = check->checkedAt5C == 0;
        Field<unsigned>(Archive(2), 0x18) ^= 2u;
        return;
    }
    for (const auto& r : profile.routes)
    {
        if (r.event != event) continue;
        Ref child; Find(p, child, event);
        auto* control = Resolve<Child>(child.slot);
        control->vptrAt00->resetAt08(control);
        auto& source = RequireProgramSelectDialogNativeServices().source;
        source.EnsureRootAt413BE0(0);
        auto* root = source.RootAt6198C4(0);
        auto* scenes = Resolve(RequireProgramSystemDialogNativeServices().source.RootProgramSlotAt30(root));
        SetRuntimeCStringAt437680(Field<RuntimeCString>(scenes, 0x1C), r.name, static_cast<int>(std::strlen(r.name)));
        return;
    }
    // Original system panels ignore skip and every unrouted event here.
}
}
int Value(const char* name)
{
    if (!name) throw std::logic_error("Missing Program variable binding");
    auto& source = RequireProgramSelectDialogNativeServices().source;
    auto* archive = Archive(0);
    source.EnsureRootAt413BE0(0);
    auto* root = source.RootAt6198C4(0);
    const auto index = source.ResolveNamedAt498020(archive, name, 0);
    return source.ReadValueAt4C73A0(source.ParamsAt47BDB0(root, 0), index, 0);
}
void Rules(Panel& p, std::span<const ProgramControlRule> rules)
{
    for (const auto& rule : rules)
    {
        const int value = rule.test == ProgramRuleTest::Constant ? rule.operand : Value(rule.variable);
        int enabled;
        switch (rule.test)
        {
        case ProgramRuleTest::Constant: enabled = value; break;
        case ProgramRuleTest::Nonzero: enabled = value != 0; break;
        case ProgramRuleTest::NotMinusOne: enabled = value != -1; break;
        case ProgramRuleTest::Nonnegative: enabled = value >= 0; break;
        case ProgramRuleTest::Equal: enabled = value == rule.operand; break;
        case ProgramRuleTest::AnyNonnegative:
        { const int alternative = Value(rule.alternative); enabled = value >= 0 || alternative >= 0; break; }
        default: throw std::logic_error("Invalid Program availability predicate");
        }
        Ref child; Find(p, child, rule.control);
        auto* c = Resolve<Child>(child.slot);
        if (rule.target == ProgramRuleTarget::State) c->vptrAt00->writeStateAt44(c, enabled);
        else c->vptrAt00->writeEnabledAt4C(c, enabled);
    }
}
void BeginPanel(Panel& p)
{
    auto& common = p.base.common;
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(p.base.managerAt5C));
    common.vptrAt00->writeEnabledAt24(&common, 1);
    common.vptrAt00->resetInputAt3C(&common);
    common.vptrAt00->resetChildrenAt68(&common);
    p.base.selectionAt80 = -1;
    if (p.role == Role::System)
    {
        const auto& system = p.owner->mode->system;
        for (int id : system.resetControls) State(p, id, 0);
        Ref child; Find(p, child, system.readToggle);
        if (Field<unsigned>(Archive(2), 0x18) & 2u)
            Resolve<GuiCheckBoxNativeBody60h>(child.slot)->checkedAt5C = 1;
        ReadSkip(p);
    }
    else Rules(p, p.data->rules);
}
int PollPanel(Panel& p)
{
    if (p.role == Role::Confirmation && p.owner->mode->confirmation.cancelOnBackOrRight && CancelPressed())
    { p.base.selectionAt80 = 0; return 0; }
    auto& common = p.base.common;
    common.vptrAt00->processInputAt54(&common);
    const int event = Event(p);
    if (event < 0) return 1;
    if (p.role == Role::System) { RouteSystem(p, event); return 1; }
    if (p.role == Role::Confirmation)
    {
        for (const auto& r : p.owner->mode->confirmation.events)
            if (event == r.event) { p.base.selectionAt80 = r.outcome; return 0; }
        return 1;
    }
    for (const auto& choice : p.owner->mode->choices)
    {
        if (event != choice.event) continue;
        if (choice.confirmation) { OpenConfirmation(*p.owner, choice); return 1; }
        p.base.selectionAt80 = choice.result;
        return 0;
    }
    return 1;
}
void OpenConfirmation(Top& b, const ProgramChoiceProfile& choice)
{
    if (b.modal || !b.confirmation.baseAt00) throw std::logic_error("Program confirmation state is invalid");
    auto& p = *Resolve<Panel>(b.confirmation);
    Ref capture;
    CaptureProgramSelectDialogNativeAt5039A0(b.base, &capture.slot, 0);
    ReleaseYtlReference(p.base.backgroundAt74);
    p.base.backgroundAt74 = Copy(capture.slot);
    BeginPanel(p);
    Rules(p, choice.confirmationRules);
    RequireProgramSelectDialogNativeServices().source.SetProgramModalFlag(true);
    b.pendingChoice = &choice;
    b.modal = true;
    Redraw(b);
}
void CloseConfirmation(Top& b, int outcome)
{
    if (!b.pendingChoice || outcome < 0 || outcome > 2) throw std::logic_error("Invalid Program confirmation result");
    const auto* choice = b.pendingChoice;
    RequireProgramSelectDialogNativeServices().source.SetProgramModalFlag(false);
    b.modal = false; b.pendingChoice = nullptr;
    auto& p = *Resolve<Panel>(b.confirmation);
    p.base.common.vptrAt00->writeEnabledAt24(&p.base.common, 0);
    ReleaseYtlReference(p.base.backgroundAt74);
    if (outcome)
    {
        b.base.resultAt94 = choice->confirmedResults[static_cast<std::size_t>(outcome - 1)];
        b.base.phaseAt68 = 2; b.base.subphaseAt6C = 0;
        Redraw(b);
    }
    else
    {
        auto& main = ChildPanel(b, 1).base.common;
        main.vptrAt00->resetChildrenAt68(&main);
        Reopen(b);
    }
}
void UpdateTop(Top& b)
{
    if (b.modal)
    {
        auto& modal = *Resolve<Panel>(b.confirmation);
        if (!PollPanel(modal)) CloseConfirmation(b, modal.base.selectionAt80);
        else modal.base.common.vptrAt00->flushDirtyAt58(&modal.base.common);
        return;
    }
    auto& system = ChildPanel(b, 0);
    if (system.base.common.vptrAt00->readStateAt20(&system.base.common)) PollPanel(system);
    auto& main = ChildPanel(b, 1);
    if (main.base.common.vptrAt00->readEnabledAt28(&main.base.common) && !PollPanel(main))
    {
        b.base.resultAt94 = main.base.selectionAt80;
        b.base.phaseAt68 = 2; b.base.subphaseAt6C = 0;
    }
}
void Reopen(Top& b) { ReadSkip(ChildPanel(b, 0)); Redraw(b); }
}

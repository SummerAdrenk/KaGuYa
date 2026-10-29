#include "ProgramMain00DialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramMain00NativeDetail;
struct NamedSites final
{
    std::uint32_t ensure, root, environment, registry, resolve, params, value;
};
template<class Name> const char* SelectedName(Name& name)
{
    if constexpr (std::is_invocable_v<Name>) return name();
    else return name;
}
template<class Name> std::int32_t ReadRegistered(Name name, const NamedSites& at)
{
    auto& source = RequireProgramMain00DialogNativeServices().shared.source;
    auto* root = Root(at.ensure, at.root);
    const auto* selectedName = SelectedName(name);
    auto* environment = source.CaptureEnvironmentAt4180C0(at.environment);
    auto* archive = source.RegistryAt417990(environment, 0, at.registry);
    return ReadNamed(root, archive, selectedName, at.resolve, at.params, at.value);
}
void WriteRegistered(const char* name, std::int32_t value, const NamedSites& at)
{
    auto& source = RequireProgramMain00DialogNativeServices().shared.source;
    auto* root = Root(at.ensure, at.root);
    auto* environment = source.CaptureEnvironmentAt4180C0(at.environment);
    auto* archive = source.RegistryAt417990(environment, 0, at.registry);
    WriteNamed(root, archive, name, value, at.resolve, at.params, at.value);
}
template<class Operation> void WithChild(Body& b, std::int32_t event, Operation&& operation)
{
    Slot child;
    Methods(b).findChildAt14(&b.common, &child, event);
    RefFrame frame{child};
    if (Read(child.baseAt00)) operation(child);
    frame.Reload(false);
}
void SetChild(Body& b, std::int32_t event, bool changeState, std::int32_t state)
{
    WithChild(b, event, [&](Slot& child) {
        auto* control = Resolve<GuiControlNativeBody50h>(child);
        Read(control->vptrAt00)->writeEnabledAt4C(control, 1);
        if (changeState)
        {
            control = Resolve<GuiControlNativeBody50h>(child);
            Read(control->vptrAt00)->writeStateAt44(control, state);
        }
    });
}
template<class Name> std::int32_t ReadAfterBorrow(Name name, bool inlined, std::uint32_t environment,
    std::uint32_t ensure, std::uint32_t rootAt, std::uint32_t resolve,
    std::uint32_t params, std::uint32_t read, std::uint32_t environmentRead = 0)
{
    void* archive = inlined ? BorrowInlineEnvironment(environment, environmentRead) : BorrowEnvironment(environment);
    auto* root = Root(ensure, rootAt);
    return ReadNamed(root, archive, SelectedName(name), resolve, params, read);
}
}
std::int32_t TrainingRecordProgramMain00DialogNativeAt50B470(ProgramMain00DialogNativeBodyC0h& b,
    std::int32_t operation)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    if (operation >= 3) return 27;
    const auto character = std::uint32_t(Read(b.characterAt8C));
    if (character >= 3) return 27;
    auto* archive = BorrowInlineEnvironment(0x50B4D6, 0x50B5B0);
    auto* root = Root(0x50B603, 0x50B60D);
    const auto level = ReadNamed(root, archive, Sample18::params[character * 6 + 2], 0x50B61D, 0x50B625, 0x50B62C);
    if (level >= 3) return 27;
    const auto selected = Read(b.characterAt8C);
    const auto row = Add(Add(selected, operation), Add(selected, selected));
    return Add(Add(row, level), Add(row, row));
}
std::int32_t PrepareProgramMain00DialogNativeAt50BCC0(ProgramMain00DialogNativeBodyC0h& b)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    if (Read(b.operationAtBC) >= 3) return 0;
    const auto character = std::uint32_t(Read(b.characterAt8C));
    if (character >= 3) return 0;
    b.weightBeforeAtA0 = ReadRegistered(Sample18::params[character * 6],
        {0x50BCF2, 0x50BCF7, 0x50BD09, 0x50BD10, 0x50BD17, 0x50BD1F, 0x50BD26});
    b.stressBeforeAtA8 = ReadRegistered(Sample18::params[character * 6 + 1],
        {0x50BD36, 0x50BD42, 0x50BD4A, 0x50BD51, 0x50BD58, 0x50BD60, 0x50BD67});
    const auto record = std::uint32_t(TrainingRecordProgramMain00DialogNativeAt50B470(b, Read(b.operationAtBC)));
    if (record >= 27) return 0;
    const auto& selected = ProgramMain00DialogNativeRecordsSample18()[record];
    auto after = Sub(Read(b.weightBeforeAtA0), selected.weightCost);
    if (after < 0) after = 0;
    b.weightAfterAtA4 = after;
    auto stress = std::uint32_t(Add(selected.stressGain, Read(b.stressBeforeAtA8)));
    if (stress >= 4) stress = 4;
    b.stressAfterAtAC = std::bit_cast<std::int32_t>(stress);
    const auto affection = ReadRegistered(Sample18::params[character * 6 + 4],
        {0x50BDCC, 0x50BDD4, 0x50BDE6, 0x50BDED, 0x50BDF4, 0x50BDFC, 0x50BE03});
    auto value = Add(selected.affectionGain, affection);
    if (value >= 100) value = 100;
    WriteRegistered(Sample18::params[character * 6 + 4], value,
        {0x50BE1F, 0x50BE27, 0x50BE3A, 0x50BE41, 0x50BE48, 0x50BE50, 0x50BE57});
    const auto dependence = ReadRegistered(Sample18::params[character * 6 + 5],
        {0x50BE61, 0x50BE69, 0x50BE7B, 0x50BE82, 0x50BE89, 0x50BE91, 0x50BE98});
    value = Add(selected.dependenceGain, dependence);
    if (value >= 100) value = 100;
    WriteRegistered(Sample18::params[character * 6 + 5], value,
        {0x50BEB5, 0x50BEBA, 0x50BED0, 0x50BED7, 0x50BEDE, 0x50BEE6, 0x50BEED});
    b.trainingLevelAtB0 = ReadRegistered(Sample18::params[character * 6 + 2],
        {0x50BEF7, 0x50BEFC, 0x50BF0E, 0x50BF15, 0x50BF1C, 0x50BF24, 0x50BF2B});
    return 1;
}
void RefreshProgramMain00DialogNativeAt5009C0(ProgramMain00DialogNativeBodyC0h& b, std::int32_t enabled)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    for (auto* at = Read(b.common.childrenAt0C.begin); at != Read(b.common.childrenAt0C.end); ++at)
    {
        auto* child = Resolve<GuiControlNativeBody50h>(*at);
        Read(child->vptrAt00)->writeEnabledAt4C(child, 0);
    }
    const auto offset = std::uint32_t(Read(b.characterAt8C)) * 12u;
    SetChild(b, Sample18::events[offset], false, 0);
    SetChild(b, Sample18::events[offset + 1], false, 0);
    for (std::int32_t i = 0; i != 3; ++i)
        WithChild(b, Sample18::events[i * 12 + 11], [&](Slot& slot) {
            auto* child = Resolve<GuiControlNativeBody50h>(slot);
            Read(child->vptrAt00)->writeEnabledAt4C(child, 1);
            const auto state = enabled && i != Read(b.characterAt8C);
            child = Resolve<GuiControlNativeBody50h>(slot);
            Read(child->vptrAt00)->writeStateAt44(child, state);
        });
    if (enabled)
    {
        auto value = ReadAfterBorrow([&]() { return Sample18::params[Read(b.characterAt8C) * 6 + 1]; }, false,
            0x500BE6, 0x500C32, 0x500C3F, 0x500C4F, 0x500C57, 0x500C5E);
        const auto lowStress = std::uint32_t(value) < 4;
        SetChild(b, 43, false, 0);
        SetChild(b, 44, true, lowStress);
        SetChild(b, 45, true, lowStress);
        value = ReadAfterBorrow([&]() { return Sample18::other[Read(b.characterAt8C)]; }, false,
            0x500DF6, 0x500E42, 0x500E4F, 0x500E5C, 0x500E64, 0x500E6B);
        // 原500E78/500F1E后的JA按无符号比较，-1等事件哨兵不能开放特殊菜单。
        const auto specialAvailable = !lowStress && std::uint32_t(value) <= 2u;
        SetChild(b, 46, true, specialAvailable);
        if (specialAvailable)
            for (std::int32_t i = 0; i != 3; ++i)
                if (i == value) { SetChild(b, Sample18::extraEvents[i], false, 0); break; }
    }
    bool eligible = false;
    for (std::int32_t i = 0; i != 3; ++i)
    {
        if (ReadAfterBorrow(Sample18::other[i + 3], true,
            0x500FDA, 0x501101, 0x50110B, 0x501117, 0x50111F, 0x501126, 0x5010AD) != 100) continue;
        if (ReadAfterBorrow(Sample18::params[i * 6], true,
            0x50114C, 0x501281, 0x501289, 0x501291, 0x501299, 0x5012A0, 0x50122D)) continue;
        const auto affection = ReadAfterBorrow(Sample18::params[i * 6 + 4], true,
            0x5012C5, 0x5013FA, 0x501401, 0x501409, 0x501411, 0x501418, 0x5013A6);
        const auto dependence = ReadAfterBorrow(Sample18::params[i * 6 + 5], true,
            0x501438, 0x50156D, 0x501577, 0x501580, 0x501588, 0x50158F, 0x501519);
        if (affection >= 50 || dependence >= 50) { eligible = true; break; }
    }
    SetChild(b, 47, true, enabled && eligible);
    if (ReadRegistered([&]() { return Sample18::other[Read(b.characterAt8C) + 3]; },
        {0x50166D, 0x501678, 0x501687, 0x50168E, 0x501695, 0x50169D, 0x5016A4}) == 100)
    {
        const auto selected = Read(b.characterAt8C);
        if (!ReadRegistered(Sample18::params[selected * 6],
            {0x5016BD, 0x5016C2, 0x5016D9, 0x5016E0, 0x5016E7, 0x5016EF, 0x5016F6}))
        {
            const auto affection = ReadRegistered(Sample18::params[selected * 6 + 4],
                {0x501708, 0x50170D, 0x50171F, 0x501726, 0x50172D, 0x501735, 0x50173C});
            const auto dependence = ReadRegistered(Sample18::params[selected * 6 + 5],
                {0x501748, 0x501750, 0x50175E, 0x501765, 0x50176C, 0x501774, 0x50177B});
            if (affection >= 50 || dependence >= 50) SetChild(b, 5, false, 0);
        }
    }
    SetChild(b, 39, false, 0);
}
void CommitProgramMain00DialogNativeAt508860(ProgramMain00DialogNativeBodyC0h& b)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    const auto selected = std::uint32_t(Read(b.characterAt8C));
    if (selected >= 3) return;
    auto* archive = BorrowInlineEnvironment(0x5088B4, 0x50898E);
    auto* root = Root(0x5089E1, 0x5089EB);
    WriteNamed(root, archive, Sample18::params[selected * 6], Read(b.weightAfterAtA4), 0x508A01, 0x508A09, 0x508A10);
    archive = BorrowInlineEnvironment(0x508A2D, 0x508B07);
    root = Root(0x508B5B, 0x508B65);
    WriteNamed(root, archive, Sample18::params[selected * 6 + 1], Read(b.stressAfterAtAC), 0x508B7B, 0x508B83, 0x508B8A);
}
void BeginProgramMain00DialogNativeAt50C230(ProgramMain00DialogNativeBodyC0h& b)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    ClearGuiClickedEventFlagsAt532200(*Resolve<GuiClickedEventReferences>(b.eventsAt5C));
    Methods(b).writeEnabledAt24(&b.common, 1);
    Methods(b).resetInputAt3C(&b.common);
    Methods(b).resetChildrenAt68(&b.common);
    b.characterAt8C = 0;
    RefreshProgramMain00DialogNativeAt5009C0(b, 1);
    b.resultAt90 = -1;
    b.phaseAt98 = 0;
}
std::int32_t EndProgramMain00DialogNativeAt508C10(ProgramMain00DialogNativeBodyC0h& b)
{
    using namespace ProgramMain00NativeDetail;
    LeafScope leaves;
    b.resultAt90 = Read(b.characterAt8C);
    auto* archive = BorrowEnvironment(0x508C21);
    leaves.services.shared.source.EnsureRootAt413BE0(0x508C6D);
    const auto value = Read(b.operationAtBC);
    auto* root = leaves.services.shared.source.RootAt6198C4(0x508C78);
    WriteNamed(root, archive, "prog_param00", value, 0x508C85, 0x508C8D, 0x508C94);
    return 1;
}
}

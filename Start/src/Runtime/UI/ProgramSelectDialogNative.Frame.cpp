#include "ProgramSelectDialogNative.Internal.h"
#include <cstring>
#include <Runtime/ProgramProfileDialogNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace ProgramSelectNativeDetail;
struct OutputFrame final
{
    Slot& output;
    bool active = false;
    ~OutputFrame() noexcept { if (active) ReleaseYtlReference(output); }
};
struct RawSurfaceFrame final
{
    void* raw;
    bool active = true;
    ~RawSurfaceFrame() noexcept { if (active) ::operator delete(raw, std::size_t{0x24}); }
};
struct TransitionSites final
{
    std::uint32_t finishEnvironment, finishRegistry;
    std::uint32_t accelerateEnvironment1, accelerateRegistry1;
    std::uint32_t accelerateEnvironment2, accelerateRegistry2;
    std::uint32_t drawEnsure, drawRoot, graphics, commit;
    std::uint32_t timeEnsure, timeRoot, timeEnvironment, timeRegistry, named, params, readValue;
    std::uint32_t volumeEnvironment, volumeRegistry;
    std::uint32_t soundEnsure, soundRoot, nameEnvironment, nameRegistry, play;
    std::uint32_t clearEnsure, clearRoot, clear;
    std::uint32_t stopEnsure, stopRoot, stopEnvironment, stopRegistry, stop;
    std::uint32_t gateEnvironment, gateRegistry, speedEnvironment, speedRegistry;
    std::uint32_t paramsEnvironment, paramsRegistry;
};
constexpr TransitionSites sites00{
    0x51C14D,0x51C154,0x51C18D,0x51C194,0x51C1A6,0x51C1AD,
    0x51C1E0,0x51C1E5,0x51C254,0x51C25B,
    0x51C2D4,0x51C2D9,0x51C2E6,0x51C2ED,0x51C2F4,0x51C2FC,0x51C303,
    0x51C317,0x51C31E,0x51C353,0x51C358,0x51C38D,0x51C394,0x51C3CC,
    0x51C3D6,0x51C3DB,0x51C402,0x51C40C,0x51C411,0x51C43A,0x51C441,0x51C44C,
    0x51C484,0x51C48B,0x51C499,0x51C4A0,0x51C4AB,0x51C4B2};
constexpr TransitionSites sites01{
    0x51C50D,0x51C514,0x51C54D,0x51C554,0x51C566,0x51C56D,
    0x51C5A0,0x51C5A5,0x51C614,0x51C61B,
    0x51C694,0x51C699,0x51C6A6,0x51C6AD,0x51C6B4,0x51C6BC,0x51C6C3,
    0x51C6D7,0x51C6DE,0x51C713,0x51C718,0x51C74D,0x51C754,0x51C78C,
    0x51C796,0x51C79B,0x51C7C2,0x51C7CC,0x51C7D1,0x51C7FA,0x51C801,0x51C80C,
    0x51C844,0x51C84B,0x51C859,0x51C860,0x51C86B,0x51C872};
}
namespace ProgramSelectNativeDetail
{
void Advance(Body& body, bool mode01)
{
    auto& source = RequireProgramSelectDialogNativeServices().source;
    const auto& at = mode01 ? sites01 : sites00;
    auto& operation = body.operationAtB0;
    switch (Read(body.subphaseAt6C))
    {
    case 0:
        if (Read(body.phaseAt68) == 1)
        {
            Read(Base(body).vptrAt00)->compositeAt5C(&Base(body), Copy(body.canvasAtA4), nullptr);
            ResetOperation(operation);
            operation.opcode = 0;
            operation.source = Resolve<GraphicsSurfaceNativeBody>(body.canvasAtA4);
            body.pendingAt70 = 3;
            auto* timeRoot = Root(source, at.timeEnsure, at.timeRoot);
            auto* archive = Registry(source, 0, at.timeEnvironment, at.timeRegistry);
            const auto index = source.ResolveNamedAt498020(archive, "time", at.named);
            auto* params = source.ParamsAt47BDB0(timeRoot, at.params);
            const auto value = source.ReadValueAt4C73A0(params, index, at.readValue);
            const auto* profile = SelectedProgramDialogProfile();
            if (profile && (value < 0 || profile->timeBgm.empty()))
                throw std::out_of_range("Program time BGM outside its confirmed domain");
            auto time = std::uint32_t(value);
            if (time >= (profile ? profile->timeBgm.size() : 3u)) time = 0;
            auto* soundParams = Resolve(Field<Slot>(Registry(source, 1, at.volumeEnvironment, at.volumeRegistry), 0x1C));
            const auto* name = profile ? profile->timeBgm[time] : source.TimeSoundNameAt61A664(time);
            auto* soundRoot = Root(source, at.soundEnsure, at.soundRoot);
            auto* sound = Resolve(source.RootSoundSlotAt24(soundRoot));
            const auto volume = Read(*source.BgmVolumeAtVslot14(soundParams));
            auto* script = Registry(source, 2, at.nameEnvironment, at.nameRegistry);
            SetRuntimeCStringAt437680(Field<RuntimeCString>(script, 0x2C), name,
                name ? static_cast<std::int32_t>(std::strlen(name)) : 0);
            (void)source.PlayBgmAt42BFE0(sound, name, volume, 1, at.play);
            auto* clearRoot = Root(source, at.clearEnsure, at.clearRoot);
            (void)source.ClearSoundAt4384D0(Resolve(source.RootSoundSlotAt24(clearRoot)), at.clear);
            auto* stopRoot = Root(source, at.stopEnsure, at.stopRoot);
            auto* stopSound = Resolve(source.RootSoundSlotAt24(stopRoot));
            auto* names = &Field<EnvironmentVectorStorage<RuntimeCString>>(
                Registry(source, 2, at.stopEnvironment, at.stopRegistry), 0x30);
            source.StopSoundAt413270(stopSound, names, at.stop);
        }
        else
        {
            ResetOperation(operation);
            operation.opcode = 0;
            operation.source = Resolve<GraphicsSurfaceNativeBody>(body.blackAt98);
            body.pendingAt70 = 4;
        }
        Field<std::uint32_t>(Registry(source, 2, at.gateEnvironment, at.gateRegistry), 4) = 0;
        {
            auto* speedOwner = Registry(source, 1, at.speedEnvironment, at.speedRegistry);
            const auto speed = Read(Field<std::uint32_t>(speedOwner, 0x70));
            // 497900不消费ECX，但原来先查询slot0的时序仍须保留。
            (void)Registry(source, 0, at.paramsEnvironment, at.paramsRegistry);
            constexpr std::int32_t steps[]{4,8,16,32};
            body.stepAtE0 = steps[speed < 4u ? speed : 0u];
        }
        body.subphaseAt6C = 1;
        return;
    case 1:
        if (Read(operation.opcode) == -1 || Read(operation.current) >= Read(operation.target))
            body.subphaseAt6C = 2;
        if (source.InputAt4C3430(Registry(source, 5, at.accelerateEnvironment1, at.accelerateRegistry1),
            mode01 ? 0x51C55B : 0x51C19B) ||
            source.InputAt4AA970(Registry(source, 5, at.accelerateEnvironment2, at.accelerateRegistry2),
                mode01 ? 0x51C574 : 0x51C1B4))
        {
            operation.current = Add(Read(operation.current), 256);
            if (Read(operation.current) >= Read(operation.target)) operation.current = Read(operation.target);
        }
        {
            auto* drawRoot = Root(source, at.drawEnsure, at.drawRoot);
            auto* draw = Resolve(source.RootDrawSlotAt18(drawRoot));
            auto* transit = Resolve<TransitRuntime>(Field<Slot>(draw, 0x20));
            (void)transit->DispatchRawAt449DD0(operation);
        }
        operation.current = Add(Read(operation.current), Read(body.stepAtE0));
        if (Read(operation.current) >= Read(operation.target)) operation.current = Read(operation.target);
        source.CommitAt4B02E0(source.GraphicsAt4182F0(at.graphics), 0, at.commit);
        return;
    case 2:
        body.phaseAt68 = Read(body.pendingAt70);
        Field<std::uint32_t>(Registry(source, 2, at.finishEnvironment, at.finishRegistry), 4) = 1;
        return;
    default: return;
    }
}
void UpdateChildren(Body& body, bool mode01)
{
    UnwrittenReference system;
    Methods(body).inherited.findDialogAt90(&Base(body), &system.slot, 0);
    {
        LocalReference temporary{system.slot};
        auto* child = Resolve<Common>(system.slot);
        if (Read(child->vptrAt00)->readStateAt20(child))
        {
            child = Resolve<Common>(system.slot);
            (void)Read(child->vptrAt00)->pollAt48(child);
        }
        temporary.Finish();
    }
    UnwrittenReference main;
    Methods(body).inherited.findDialogAt90(&Base(body), &main.slot, 1);
    LocalReference temporary{main.slot};
    auto* child = Resolve<Common>(main.slot);
    if (Read(child->vptrAt00)->readEnabledAt28(child))
    {
        child = Resolve<Common>(main.slot);
        if (Read(child->vptrAt00)->pollAt48(child) == 0)
        {
            if (!mode01)
            {
                child = Resolve<Common>(main.slot);
                (void)Read(child->vptrAt00)->endAt4C(child);
            }
            body.phaseAt68 = 2;
            body.subphaseAt6C = 0;
            body.resultAt94 = Read(Field<std::int32_t>(Resolve(main.slot), mode01 ? 0x80 : 0x90));
        }
    }
    temporary.Finish();
}
void Reopen(Body& body, bool mode01)
{
    auto& source = RequireProgramSelectDialogNativeServices().source;
    UnwrittenReference system;
    Methods(body).inherited.findDialogAt90(&Base(body), &system.slot, 0);
    LocalReference systemTemporary{system.slot};
    UnwrittenReference control;
    auto* dialog = Resolve<Common>(system.slot);
    Read(dialog->vptrAt00)->findChildAt14(dialog, &control.slot, mode01 ? 0x1E : 0x37);
    LocalReference controlTemporary{control.slot};
    auto* archive = Archive(source.CaptureEnvironmentAt4180C0(mode01 ? 0x53107E : 0x530EEE), 2);
    auto* selectedControl = Resolve<GuiControlNativeBody50h>(control.slot);
    Field<std::int32_t>(selectedControl, 0x5C) = std::int32_t((Read(Field<std::uint32_t>(archive, 0x14)) >> 1u) & 1u);
    selectedControl = Resolve<GuiControlNativeBody50h>(control.slot);
    Read(selectedControl->vptrAt00)->writeStateAt44(selectedControl, 0);
    controlTemporary.Finish(true);
    Methods(body).redrawAtA0(&Base(body));
    systemTemporary.Finish();
}
Slot* CaptureWithSystem(Body& body, Slot* output, std::int32_t includeSystem)
{
    OutputFrame outputFrame{*output};
    UnwrittenReference system;
    Methods(body).inherited.findDialogAt90(&Base(body), &system.slot, 0);
    LocalReference systemTemporary{system.slot};
    if (!includeSystem)
    {
        auto* dialog = Resolve<Common>(system.slot);
        Read(dialog->vptrAt00)->writeStateAt1C(dialog, 0);
    }
    CaptureProgramSelectDialogNativeAt503AA0(body, output, 1);
    outputFrame.active = true;
    if (!includeSystem)
    {
        auto* dialog = Resolve<Common>(system.slot);
        Read(dialog->vptrAt00)->writeStateAt1C(dialog, 1);
    }
    systemTemporary.Finish();
    outputFrame.active = false;
    return output;
}
}
void AdvanceProgramSelectDialogNativeAt51C120(ProgramSelectDialogNativeBodyECh& body)
{ ProgramSelectNativeDetail::LeafScope leaves; ProgramSelectNativeDetail::Advance(body, false); }
void AdvanceProgramSelectDialogNativeAt51C4E0(ProgramSelectDialogNativeBodyECh& body)
{ ProgramSelectNativeDetail::LeafScope leaves; ProgramSelectNativeDetail::Advance(body, true); }
void UpdateProgramSelectDialogNativeAt521FD0(ProgramSelectDialogNativeBodyECh& body)
{ ProgramSelectNativeDetail::LeafScope leaves; ProgramSelectNativeDetail::UpdateChildren(body, false); }
void UpdateProgramSelectDialogNativeAt5221A0(ProgramSelectDialogNativeBodyECh& body)
{ ProgramSelectNativeDetail::LeafScope leaves; ProgramSelectNativeDetail::UpdateChildren(body, true); }
void ReopenProgramSelectDialogNativeAt530E80(ProgramSelectDialogNativeBodyECh& body)
{ ProgramSelectNativeDetail::LeafScope leaves; ProgramSelectNativeDetail::Reopen(body, false); }
void ReopenProgramSelectDialogNativeAt531010(ProgramSelectDialogNativeBodyECh& body)
{ ProgramSelectNativeDetail::LeafScope leaves; ProgramSelectNativeDetail::Reopen(body, true); }
void RedrawProgramSelectDialogNativeAt530E40(ProgramSelectDialogNativeBodyECh& body)
{
    using namespace ProgramSelectNativeDetail;
    LeafScope leaves;
    auto* dirty = Read(Base(body).vptrAt00)->dirtyAt00(&Base(body));
    Read(dirty->vptrAt00)->addRectAt08(dirty, &body.boundsAt74);
    Read(Base(body).vptrAt00)->flushDirtyAt58(&Base(body));
}
YtlReferenceSlotRuntime* CaptureProgramSelectDialogNativeAt503AA0(
    ProgramSelectDialogNativeBodyECh& body, YtlReferenceSlotRuntime* output, std::int32_t)
{
    using namespace ProgramSelectNativeDetail;
    LeafScope leaves;
    OutputFrame outputFrame{*output};
    void* raw = ::operator new(0x24);
    RawSurfaceFrame rawFrame{raw};
    auto* surface = raw ? ::new(raw) GraphicsSurfaceNativeBody(leaves.services.surfaces, leaves.services.strings) : nullptr;
    rawFrame.active = false;
    ConstructGraphicsSurfaceReferenceAt451500(*output, surface, 1);
    outputFrame.active = true;
    POINT size;
    auto& source = leaves.services.source;
    source.ScreenSizeAt47CC00(source.GraphicsAt4182F0(0x503B1D), &size, 0x503B24);
    if (!Resolve<GraphicsSurfaceNativeBody>(*output)->CreateAt415010(leaves.services.surfaces, size.x, size.y, 0, 0))
        throw 1;
    Read(Base(body).vptrAt00)->compositeAt5C(&Base(body), Copy(*output), nullptr);
    outputFrame.active = false;
    return output;
}
YtlReferenceSlotRuntime* CaptureProgramSelectDialogNativeAt5038A0(
    ProgramSelectDialogNativeBodyECh& body, YtlReferenceSlotRuntime* output, std::int32_t includeSystem)
{ ProgramSelectNativeDetail::LeafScope leaves; return ProgramSelectNativeDetail::CaptureWithSystem(body, output, includeSystem); }
YtlReferenceSlotRuntime* CaptureProgramSelectDialogNativeAt5039A0(
    ProgramSelectDialogNativeBodyECh& body, YtlReferenceSlotRuntime* output, std::int32_t includeSystem)
{ ProgramSelectNativeDetail::LeafScope leaves; return ProgramSelectNativeDetail::CaptureWithSystem(body, output, includeSystem); }
}

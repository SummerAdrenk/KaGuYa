#include <Runtime/LoadAfterSelectionServicesRuntime.h>

#include <Runtime/DrawManagerRuntime.h>
#include <Runtime/GraphicsRuntimeApi.h>
#include <Runtime/GraphicsTransitionSourceRuntime.h>
#include <Runtime/ReadScriptEffectSurfaceRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SceneManagerRuntime.h>
#include <Runtime/ScrParamsResetRuntime.h>
#include <Runtime/TextWindowSoundSource.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
LoadAfterTransitionSourceRef WrapSurface(std::shared_ptr<RuntimeGraphicsSurface> surface)
{
    return surface ? std::make_shared<GraphicsTransitionSourceRuntime>(std::move(surface)) : nullptr;
}
}

LoadAfterSelectionServicesRuntime::LoadAfterSelectionServicesRuntime(
    LoadAfterSelectionDependencies dependencies) noexcept
    : dependencies_{dependencies.graphics,dependencies.resources,dependencies.save,dependencies.unsave,
        dependencies.sound,dependencies.scenes,dependencies.paramsReset,dependencies.mouse,dependencies.keyboard,
        dependencies.rootByteAt218,dependencies.rootByteAt219,dependencies.rootServiceDwordAt10},
      input_(dependencies.mouse,dependencies.keyboard),fixedDrawing_(&dependencies.drawing),drawing_(&fixedDrawing_),
      controlledText_(std::in_place,dependencies.textFormat,dependencies.textLayout,dependencies.values),text_(&*controlledText_){}
LoadAfterSelectionServicesRuntime::LoadAfterSelectionServicesRuntime(LoadAfterSelectionCoreDependencies dependencies,
    LoadAfterDrawBindings bindings) noexcept
    :dependencies_(dependencies),input_(dependencies.mouse,dependencies.keyboard),fixedDrawing_(nullptr),drawing_(&bindings.source),
      nativeText_(bindings.nativeText),text_(bindings.controlledText),patterns_(bindings.patterns),resetText_(bindings.resetText){}

LoadAfterTransitionSourceRef LoadAfterSelectionServicesRuntime::LoadTransitionSource(const char* name)
{
    std::shared_ptr<RuntimeGraphicsSurface> surface;
    if (!dependencies_.resources.LoadStaticAt480000(surface, name)) return {};
    return WrapSurface(std::move(surface));
}

std::uint32_t LoadAfterSelectionServicesRuntime::ReadRegistrySlot2DwordAt04() const
    { return dependencies_.unsave.ModeAt04(); }
void LoadAfterSelectionServicesRuntime::WriteRegistrySlot2DwordAt04(std::uint32_t value)
    { dependencies_.unsave.ModeAt04() = value; }
void LoadAfterSelectionServicesRuntime::WriteRootByteAt218(std::uint8_t value)
    { dependencies_.rootByteAt218 = value; }
void LoadAfterSelectionServicesRuntime::WriteRootByteAt219(std::uint8_t value)
    { dependencies_.rootByteAt219 = value; }
std::uint32_t LoadAfterSelectionServicesRuntime::ReadTransitionSpeedIndexAt70() const
    { return dependencies_.save.valueAt70; }
bool LoadAfterSelectionServicesRuntime::ReadTransitionAccelerationGateAt6F() const
    { return dependencies_.save.valueAt6F != 0; }

bool LoadAfterSelectionServicesRuntime::PollTransitionAccelerationAtSlot5()
{
    // 4AA970：Keyboard action 5 当前态，或 Mouse +30/+10/+28；不是 click-info 偏移5。
    const auto& mouse = dependencies_.mouse;
    return input_.ReadHeldAtVslot00(5) || (mouse.PreviousPressed(0) == 0 &&
        mouse.Pressed(0) != 0 && mouse.ReleaseSuppressionCountdown() == 0);
}

void LoadAfterSelectionServicesRuntime::RenderTransition(LoadAfterTransitionOperation& operation)
{
    // 4B63BD/4B6719 清零整个包后只写来源、57/58及进度。旧整数槽不能强转成指针。
    if (operation.valueAt00 || operation.valueAt08 || operation.valueAt0C ||
        operation.valueAt10 || operation.valueAt14 || operation.valueAt18 ||
        operation.valueAt20 || (operation.selectorAt1C != 57 &&
            operation.selectorAt1C != 58 && operation.selectorAt1C != -1))
        throw std::invalid_argument("LoadAfter transition is outside its verified screen contract");
    TransitOperation packet;
    if (operation.sourceAt04)
    {
        const auto* source = dynamic_cast<GraphicsTransitionSourceRuntime*>(operation.sourceAt04.get());
        if (!source) throw std::invalid_argument("LoadAfter transition source has no production CSurface");
        packet.source = &source->Surface().NativeBody();
    }
    packet.opcode = operation.selectorAt1C;
    packet.current = operation.progressAt24;
    packet.target = operation.maximumAt28;
    packet.range = operation.valueAt2C;
    static_cast<void>(drawing_->CaptureTransitionAfter413BE0().RenderTransitionAt4C39E0(packet));
}

void LoadAfterSelectionServicesRuntime::CommitTransitionFrame(std::int32_t argument)
{
    if (argument != 0) throw std::invalid_argument("LoadAfter InvalidatedRect argument is not null");
    dependencies_.graphics.InvalidatedRectIfLoaded(nullptr);
}
void LoadAfterSelectionServicesRuntime::WriteRootServiceDwordAt10(std::uint32_t value)
    { dependencies_.rootServiceDwordAt10 = value; }
void LoadAfterSelectionServicesRuntime::StopSoundManagerEntryNamedAt10()
{
    // 4B6508 是 DWORD 绝对槽号，不是资源名字。
    const auto sound = dependencies_.sound.CaptureRootAfter413BE0().Resolve();
    sound.slots.StopAt4383B0(sound.slots.MusicSlotAt10());
}
void LoadAfterSelectionServicesRuntime::ClearSoundManagerEntries()
    { dependencies_.sound.CaptureRootAfter413BE0().Resolve().voice.StopAndClearAllVoiceSlots(); }
void LoadAfterSelectionServicesRuntime::StopRootSoundEntryNamedByRegistrySlot2At30()
{
    // 4B6531固定root，654A取得当时映射地址，6550再从原root解析manager。
    const auto root = dependencies_.sound.CaptureRootAfter413BE0();
    auto* const mappings = dependencies_.sound.CaptureMappingsAt30();
    root.Resolve().effects.StopEffectsAt413270(mappings);
}
void LoadAfterSelectionServicesRuntime::WriteSceneMarkerAt38(std::int32_t value)
    { dependencies_.scenes.WriteSceneMarkerAt38(value); }
void LoadAfterSelectionServicesRuntime::SynchronizeRestoredGroup0BeforeGroup3()
{
    drawing_->CaptureDrawLayersAt413BE0().WindowZero().RestoreSavedPatternsAt4C14E0(dependencies_.graphics,patterns_);
}
void LoadAfterSelectionServicesRuntime::RestoreEnvironmentGroup3(std::int32_t argument)
{
    // 原 slot3 是 TextWin，不是 Environment slot3 的声音参数；返回AL被忽略。
    static_cast<void>(drawing_->CaptureDrawLayersAt413BE0().TextAtSlotThree().RebuildAt4C1650(
        static_cast<std::uint32_t>(argument),nativeText_,text_));
}
void LoadAfterSelectionServicesRuntime::SynchronizeRestoredGroup0AfterGroup3()
{
    drawing_->CaptureDrawLayersAt413BE0().WindowZero().PropagateDirtyRectsAt4C4250();
}
void LoadAfterSelectionServicesRuntime::SynchronizeRestoredRootOwners()
{
    // 4AC490 是注册列表绘制+18；不是 owning 数组的47E940恢复/推进。
    drawing_->CaptureDrawLayersAt413BE0().RegisteredLayers().RenderDirtyRects(dependencies_.graphics);
}
LoadAfterTransitionSourceRef LoadAfterSelectionServicesRuntime::CaptureScreenBackup()
    { return WrapSurface(dependencies_.resources.CaptureScreenAt4ACDD0()); }
void LoadAfterSelectionServicesRuntime::ApplyPostRestoreScreenState(std::int32_t argument)
{
    if (argument != 0) throw std::invalid_argument("LoadAfter ClearScreen argument is not null");
    dependencies_.graphics.ClearScreenIfLoaded(nullptr);
}
void LoadAfterSelectionServicesRuntime::ClearCurrentSceneName()
    { dependencies_.scenes.WritePendingCommandAt1C({}); }
void LoadAfterSelectionServicesRuntime::ClearScenes() noexcept
    { dependencies_.scenes.ClearScenes(); }
void LoadAfterSelectionServicesRuntime::PushActiveScene(const char* name)
    { dependencies_.scenes.PushActiveScene(name); }
void LoadAfterSelectionServicesRuntime::QueueScene(const char* name)
    { dependencies_.scenes.QueueScene(name); }
void LoadAfterSelectionServicesRuntime::ResetScriptForDefaultRoute()
    { dependencies_.paramsReset.ResetAt46CE30(); }
void LoadAfterSelectionServicesRuntime::ResetOwnerGroup3ForDefaultRoute()
    { drawing_->CaptureDrawLayersAt413BE0().TextAtSlotThree().ResetTextAt46D170(resetText_); }
}

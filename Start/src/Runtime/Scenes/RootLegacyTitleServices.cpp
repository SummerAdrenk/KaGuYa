#include <Runtime/RootLegacyTitleServices.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/DrawManagerResourceRuntime.h>
#include <Runtime/SpriteNativeDrawing.h>
#include <Runtime/SpriteNativeResourceState.h>
#include <Runtime/SpriteAnimationNativeExecution.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/GuiControlResourceLoadRuntime.h>
#include <bit>
#include <stdexcept>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
RootLegacyTitleServices::RootLegacyTitleServices(RootLegacyTitleDependencies d) noexcept
    : dependencies_(d), names_(*this, d.nameApi, d.common.strings),
      menu_{*this, d.buttons, d.common.graphics, d.common.graphics, d.common.memory, d.archives,
        d.common.transaction, d.common.files, d.common.key, *this, d.common.codecs},
      scene_{*this, d.data.menu, menu_, *this, names_, d.quickLoad, d.common.graphics,
        d.common.graphics, d.common.strings, d.common.memory}
{}
SceneManagerView RootLegacyTitleServices::CaptureScenes()
{
    dependencies_.common.root.EnsureCreatedAt413BE0();
    return dependencies_.common.root.Published()->PostWindow().BorrowSceneManagerAt30();
}
UnSaveDataRuntime& RootLegacyTitleServices::CaptureCommon()
{
    auto* env = dependencies_.common.environment.RootSource().CaptureAt4180C0();
    return *static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*env, 2));
}
SaveDataEnvironmentArchive& RootLegacyTitleServices::Save()
{
    auto* env = dependencies_.common.environment.RootSource().CaptureAt4180C0();
    return *static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*env, 1));
}
bool RootLegacyTitleServices::Pressed(std::int32_t action)
{
    auto* input = dependencies_.common.input.CaptureAt418210();
    return ReadKeyboardPressedAt47F4E0(*ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18), action) != 0;
}
bool RootLegacyTitleServices::Held(std::int32_t action)
{
    auto* input = dependencies_.common.input.CaptureAt418210();
    return ReadKeyboardHeldAt4198D0(*ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input->keyboardAt18), action) != 0;
}
bool RootLegacyTitleServices::MousePressedEdge(std::int32_t button)
{
    auto* input = dependencies_.common.input.CaptureAt418210();
    const auto* mouse = ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(input->mouseAt00);
    const auto read = [](const auto& value) { return *static_cast<const volatile std::remove_cvref_t<decltype(value)>*>(&value); };
    return !read(mouse->previousAt30[button]) && read(mouse->pressedAt10[button]) && !read(mouse->suppressionAt28);
}
std::int32_t RootLegacyTitleServices::ReplayValue()
{
    // 44E190：输入已在caller判定；本端口独立选择当时Root/CParams。
    dependencies_.common.root.EnsureCreatedAt413BE0();
    auto* root = dependencies_.common.root.Published();
    const auto index = ResolveScriptNamedValueNativeAt498020("replay", dependencies_.namedValues, dependencies_.common.strings);
    return ReadScriptValueAt4C73A0(root->ParamsAt47BDB0(), index, dependencies_.errors);
}
void RootLegacyTitleServices::CaptureFrame(YtlReferenceSlotRuntime& output)
{
    // 49E40B先捕获draw，4C9190本体不消费此ECX；截图仍用同一Graphics拥有者。
    dependencies_.common.root.EnsureCreatedAt413BE0();
    static_cast<void>(dependencies_.common.root.Published()->PostWindow().DrawManagerAt18());
    auto& graphics = dependencies_.common.graphics;
    CaptureGraphicsScreenAt4ACDD0(output, graphics, graphics, graphics);
}
DrawTransitionView RootLegacyTitleServices::CaptureDraw()
{
    dependencies_.common.root.EnsureCreatedAt413BE0();
    return DrawTransitionView(*dependencies_.common.root.Published()->PostWindow().DrawManagerAt18(), dependencies_.common.graphics);
}
std::uint8_t RootLegacyTitleServices::LoadStartupScript(const char* name, std::int32_t argument)
{ return dependencies_.scripts.LoadStartupScriptAfter413BE0(name, argument); }
void RootLegacyTitleServices::WriteStartupFlag(std::uint8_t value)
{ dependencies_.scripts.WriteRootByteAfter413BE0(0x218, value); }
void RootLegacyTitleServices::WriteStartupReadyFlag(std::uint8_t value)
{ dependencies_.scripts.WriteRootByteAfter413BE0(0x219, value); }
void RootLegacyTitleServices::WriteLoadEntryFlag(std::uint8_t value)
{ dependencies_.scripts.WriteRootByteAfter413BE0(0x1AF, value); }
void RootLegacyTitleServices::WriteSpecialMenuFlag(std::uint8_t value)
{
    // Baku416A40/40AA40的1E8是popup index，尺寸同组由416A80写；不是另一个byte开关。
    dependencies_.common.root.EnsureCreatedAt413BE0();
    dependencies_.common.root.Published()->MenuPositionAt208().indexAt208 = value;
}
std::int32_t RootLegacyTitleServices::PostWindowMessage(std::uint32_t message, std::uint32_t w, std::int32_t l)
{
    auto* env = dependencies_.common.environment.RootSource().CaptureAt4180C0();
    return dependencies_.postMessage(reinterpret_cast<HWND>(env->windowAt2C), message, w, l);
}
const char* RootLegacyTitleServices::BlackSurfaceName() { return dependencies_.data.blackName; }
bool RootLegacyTitleServices::LoadBlackSurface(YtlReferenceSlotRuntime& out, const char* name)
{ return LoadBackground(out, name); }
std::int32_t RootLegacyTitleServices::ScreenHeight()
{ return std::bit_cast<std::int32_t>(dependencies_.common.environment.RootSource().CaptureAt4180C0()->heightAt1C); }
std::int32_t RootLegacyTitleServices::ScreenWidth()
{ return std::bit_cast<std::int32_t>(dependencies_.common.environment.RootSource().CaptureAt4180C0()->widthAt18); }
void RootLegacyTitleServices::ClearReplay() { dependencies_.scripts.ClearReplayAt4A2DD6(); }

SoundManagerNativeBody50h* RootLegacyTitleServices::Sound()
{
    dependencies_.common.root.EnsureCreatedAt413BE0();
    return ResolveEnvironmentObjectReference<SoundManagerNativeBody50h>(dependencies_.common.root.Published()->SoundManagerSlotAt24());
}
std::int32_t RootLegacyTitleServices::ReadBgmVolume()
{ return std::bit_cast<std::int32_t>(*Save().BorrowSoundParamsAt1C().MusicVolumeAt14()); }
const char* RootLegacyTitleServices::TitleBgmName() { return dependencies_.data.bgmName; }
std::uint8_t RootLegacyTitleServices::PlayBgm(const char* name, std::int32_t volume, std::int32_t loop)
{
    // 49E07F先取共同BGM名字槽，再取Sound；4C8D30先写该名字再Play，不能用不写名的入口。
    auto& mapping = CaptureCommon().StringAt2C();
    auto* sound = Sound();
    return dependencies_.common.root.NativeSoundServices().PlayMusicCapturedAt4BCCC0(sound, mapping, name, volume, loop);
}
std::uint8_t RootLegacyTitleServices::ClearSound()
{ return dependencies_.common.root.NativeSoundServices().ClearVoicesCapturedAt4384D0(Sound()); }
void RootLegacyTitleServices::StopCommonSounds()
{
    auto& mappings = CaptureCommon().mappingsAt30_;
    auto* sound = Sound();
    dependencies_.common.root.NativeSoundServices().StopEffectsCapturedAt413270(sound, &mappings);
}
const char* RootLegacyTitleServices::BackgroundName() { return dependencies_.data.names.background; }
const char* RootLegacyTitleServices::OverlayName() { return dependencies_.data.names.overlay; }
bool RootLegacyTitleServices::LoadBackground(YtlReferenceSlotRuntime& out, const char* name)
{
    auto* draw = dependencies_.pictures.CaptureLoadReceiverAt413BE0();
    return LoadGuiControlSurfaceReferenceAt480000(out, draw, name, dependencies_.pictures,
        dependencies_.common.graphics, dependencies_.common.strings);
}
bool RootLegacyTitleServices::LoadOverlay(YtlReferenceSlotRuntime& out, const char* name, int mode)
{
    // 49CB8E先取draw，4697C0内才惰建Sprite。
    auto* draw = dependencies_.sprites.CaptureDrawReceiverAt413BE0();
    if (!out.baseAt00)
        InstallSpriteReferenceAt48140F(out, NewSpriteNativeAt4813E4(dependencies_.common.strings));
    return dependencies_.sprites.LoadAt4814F0(draw, ResolveSpriteReference(out), name, static_cast<std::uint8_t>(mode));
}
std::int32_t RootLegacyTitleServices::UpdateOverlay(const YtlReferenceSlotRuntime& ref)
{ return AdvanceSpriteNativeAt46B1D0(*ResolveSpriteReference(ref)); }
RECT RootLegacyTitleServices::OverlayBounds(const YtlReferenceSlotRuntime& ref)
{ return ReadSpriteDrawableRectNativeAt4AD080(*ResolveSpriteReference(ref)).rect; }
void RootLegacyTitleServices::BlitOverlay(const YtlReferenceSlotRuntime& ref, GraphicsSurfaceNativeBody& target, const RECT* clip)
{ static_cast<void>(CompositeSpriteNativeAt46B390(*ResolveSpriteReference(ref), &target, clip, dependencies_.common.graphics)); }
void RootLegacyTitleServices::DrawOverlay(const YtlReferenceSlotRuntime& ref, const RECT* clip)
{ static_cast<void>(DrawSpriteNativeAt4768D0(*ResolveSpriteReference(ref), clip, dependencies_.common.graphics)); }
std::int32_t RootLegacyTitleServices::ReadCueVolume()
{ return std::bit_cast<std::int32_t>(*Save().BorrowSoundParamsAt1C().NormalEffectVolumeAt24()); }
const char* RootLegacyTitleServices::CueName(LegacyTitleMenuCue cue)
{ return cue == LegacyTitleMenuCue::Click ? dependencies_.data.names.clickCue : dependencies_.data.names.selectCue; }
std::uint8_t RootLegacyTitleServices::PlayCue(const char* name, int channel, int volume, int enabled)
{
    auto* sound = Sound();
    return dependencies_.common.root.NativeSoundServices().PlayCueAt4BCD70(sound, name,
        static_cast<std::uint32_t>(channel), volume, static_cast<std::uint8_t>(enabled));
}

bool RootLegacyTitleServices::EditableNameBit(unsigned bit)
{
    if (bit > 1) throw std::invalid_argument("Legacy title name bit must be 0 or 1");
    return (dependencies_.params.CaptureAt418480()->GameSystem()->defaultNameMode & (1u << bit)) != 0;
}
bool RootLegacyTitleServices::SkipNamePrompt() { return Save().valueAt54 != 0; }
RuntimeCString& RootLegacyTitleServices::SavedGivenName() { return Save().stringAt58; }
RuntimeCString& RootLegacyTitleServices::SavedFamilyName() { return Save().stringAt5C; }
const RuntimeCString& RootLegacyTitleServices::DefaultGivenName()
{ return dependencies_.params.CaptureAt418480()->GameSystem()->defaultGivenName.Native(); }
const RuntimeCString& RootLegacyTitleServices::DefaultFamilyName()
{ return dependencies_.params.CaptureAt418480()->GameSystem()->defaultFamilyName.Native(); }
void RootLegacyTitleServices::WriteSkipNamePrompt(bool value) { Save().valueAt54 = value; }
ILegacyTitleNameRoot& RootLegacyTitleServices::CaptureRoot()
{ capturedNameEnvironment_ = dependencies_.common.environment.RootSource().CaptureAt4180C0(); return *this; }
HWND RootLegacyTitleServices::MainWindow() { return reinterpret_cast<HWND>(capturedNameEnvironment_->windowAt2C); }
const RuntimeCString& RootLegacyTitleServices::Caption() { return capturedNameEnvironment_->titleAt10; }
void RootLegacyTitleServices::WriteInputGate(bool value)
{
    // 原416A00写19A：模态进入/离开使用的窗口最小化处理门；统一模型对应1B2。
    dependencies_.common.root.EnsureCreatedAt413BE0();
    dependencies_.common.root.Published()->Frame().WriteMinimizeByteAt1B2(value);
}
HINSTANCE RootLegacyTitleServices::ResourceInstance()
{ return *static_cast<const volatile HINSTANCE*>(&dependencies_.resourceInstance); }
void RootLegacyTitleServices::ShowAt480A2D(const RuntimeCString& text)
{
    auto* env = dependencies_.common.environment.RootSource().CaptureAt4180C0();
    const auto* caption = env->titleAt10.GetString();
    const auto* message = text.GetString();
    const auto window = reinterpret_cast<HWND>(env->windowAt2C);
    dependencies_.common.messages.MessageBoxAt4801C2(window, message, caption, MB_ICONHAND);
}
}

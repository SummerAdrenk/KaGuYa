#include "GraffitiCountDialogNative.Internal.h"
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime::GraffitiCountDialogNativeDetail
{
namespace
{
void BorrowUnusedSlot0Inline(EnvironmentRootStorage& root) noexcept
{
    // 4E1985/4F5291/508D5A：仅count==0拒绝；不能换成417990的无符号index检查。
    auto* begin = Read(root.archivesAt30.begin);
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Read(root.archivesAt30.end)) -
        reinterpret_cast<std::uintptr_t>(begin));
    if (std::bit_cast<std::int32_t>(bytes) / 12 == 0) return;
    const auto base = Read(begin->baseAt00);
    if (!base) return;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(begin->controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    static_cast<void>(base + Read(begin->biasAt04) * stride);
}
}
std::int32_t ReadNamed(const char* name, bool rootFirst)
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    const auto receiver = [&]() -> GraffitiPageNativeParamsReceiver
    {
        if(rootFirst)
        {
            const auto root=s.source.CaptureParamsRootAfter413BE0();
            auto* environment=s.source.CaptureEnvironmentAt4180C0();
            // 保留原417990解址，即使498020不读ECX，LTCG也不能删除这次查询。
            auto* volatile archive=ResolveEnvironmentArchiveAt417990(*environment,0);
            static_cast<void>(archive);return root;
        }
        BorrowUnusedSlot0Inline(*s.source.CaptureEnvironmentAt4180C0());
        return s.source.CaptureParamsRootAfter413BE0();
    }();
    const auto index = ResolveScriptNamedValueNativeAt498020(name, s.names, s.strings);
    return ReadScriptValueAt4C73A0(receiver.paramsAt47BDB0(receiver.root), index, s.errors);
}
void WriteNamedLate(const char* name, const std::int32_t& late)
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    BorrowUnusedSlot0Inline(*s.source.CaptureEnvironmentAt4180C0());
    const auto receiver = s.source.CaptureParamsRootAfter413BE0();
    const auto value = Read(late); // 508DA6/509676：Ensure之后、498020之前push。
    const auto index = ResolveScriptNamedValueNativeAt498020(name, s.names, s.strings);
    WriteScriptValueAt4C8220(receiver.paramsAt47BDB0(receiver.root), index, value, s.errors);
}
void AddFullScreen(Common& body)
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    auto* dirty = Read(body.dialog.vptrAt00)->dirtyAt00(&body.dialog);
    const auto height = Read(s.source.CaptureEnvironmentAt4180C0()->heightAt1C);
    const auto width = Read(s.source.CaptureEnvironmentAt4180C0()->widthAt18);
    RECT rectangle{0, 0, std::bit_cast<LONG>(width), std::bit_cast<LONG>(height)};
    Read(dirty->vptrAt00)->addRectAt08(dirty, &rectangle);
}
void AddDialogRectangle(Common& body)
{
    auto* dirty = Read(body.dialog.vptrAt00)->dirtyAt00(&body.dialog);
    Read(dirty->vptrAt00)->addRectAt08(dirty, &body.rectangleAt5C);
}
void PlayCue(std::uint32_t address)
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    auto* save = static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*s.source.CaptureEnvironmentAt4180C0(), 1));
    auto* parameters = Resolve<EnvironmentSoundParamsArchive>(save->soundParamsAt1C);
    const char* name = s.source.ReadCueName(address); // 必须早于新的413BE0。
    const auto sound = s.source.CaptureSoundAfter413BE0();
    const auto volume = *parameters->NormalEffectVolumeAt24();
    static_cast<void>(sound.playAt4BCD70(sound.object,sound.service, name, 0, std::bit_cast<std::int32_t>(volume), 1));
}
SkipReason SkipRequested()
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    auto keyboard = s.input.InputAt418210().KeyboardAt18();
    const auto heldCount = keyboard.BindingCount(6); // +14+6*0C == +5C；不是action3。
    for (std::size_t i = 0; i != heldCount; ++i)
        if (keyboard.CurrentState(6, i)) return SkipReason::Latched;
    auto* archive = ResolveEnvironmentArchiveAt417990(*s.source.CaptureEnvironmentAt4180C0(), 2);
    if (*reinterpret_cast<const volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(archive) + 0x14u) & 2u)
        return SkipReason::Latched;
    keyboard = s.input.InputAt418210().KeyboardAt18();
    for (std::size_t i = 0; i != keyboard.BindingCount(5); ++i)
        if (keyboard.CurrentState(5, i) && !keyboard.PreviousState(5, i)) return SkipReason::Action5;
    const auto mouse = s.input.InputAt418210().MouseAt00();
    if (!mouse.PreviousLeftAt30() && mouse.CurrentLeftAt10() && !mouse.SuppressionAt28())
        return SkipReason::Latched;
    return SkipReason::None;
}
}

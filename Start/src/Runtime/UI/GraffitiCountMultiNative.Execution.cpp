#include "GraffitiCountMultiNative.Internal.h"
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::GraffitiCountMultiDetail
{
namespace
{
SkipReason MultiSkipRequested()
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    auto keyboard = s.input.InputAt418210().KeyboardAt18();
    // 16:4FE720读取旧16字节vector表的action 6。
    for (std::size_t i = 0; i != keyboard.BindingCount(6); ++i)
        if (keyboard.CurrentState(6, i)) return SkipReason::Latched;
    auto* archive = ResolveEnvironmentArchiveAt417990(*s.source.CaptureEnvironmentAt4180C0(), 2);
    if (*reinterpret_cast<const std::uint8_t*>(reinterpret_cast<std::uintptr_t>(archive) + 0x14) & 2)
        return SkipReason::Latched;
    keyboard = s.input.InputAt418210().KeyboardAt18();
    for (std::size_t i = 0; i != keyboard.BindingCount(5); ++i)
        if (keyboard.CurrentState(5, i) && !keyboard.PreviousState(5, i)) return SkipReason::Action5;
    const auto mouse = s.input.InputAt418210().MouseAt00();
    // 左键边沿与action 5均不锁存，区别于18 Likable。
    if (!mouse.PreviousLeftAt30() && mouse.CurrentLeftAt10() && !mouse.SuppressionAt28())
        return SkipReason::Action5;
    return SkipReason::None;
}
void PlayCountCue(const char* name)
{
    auto& s = RequireGraffitiCountDialogNativeServices();
    auto* save = static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*s.source.CaptureEnvironmentAt4180C0(), 1));
    auto* volume = Resolve<EnvironmentSoundParamsArchive>(save->soundParamsAt1C);
    const auto sound = s.source.CaptureSoundAfter413BE0();
    static_cast<void>(sound.playAt4BCD70(sound.object, sound.service, name, 0,
        std::bit_cast<std::int32_t>(*volume->NormalEffectVolumeAt24()), 1));
}
}
void __fastcall Begin(Dialog* dialog, void*)
{
    auto& b = *reinterpret_cast<Multi*>(dialog);
    const auto& d = *b.data;
    dialog->vptrAt00->writeEnabledAt24(dialog, 1);
    b.rows.clear();
    const auto count = ReadNamed(d.countVariable);
    if (count > 3) throw std::out_of_range("Count: row count exceeds three parameter triplets");
    for (std::int32_t i = 0; i < count; ++i)
    {
        const auto& p = d.rows[i];
        GraffitiCountMultiRow row{};
        row.selector = ReadNamed(p.selector);
        if (row.selector < 0 || static_cast<std::size_t>(row.selector) >= d.variables.size())
            throw std::out_of_range("Count: character selector outside verified mapping");
        const auto& binding = d.variables[row.selector];
        row.current = ReadNamed(binding.variable);
        row.upperLimit = ReadNamed(p.upperLimit);
        row.target = Add(row.current, ReadNamed(p.delta));
        if (row.target > row.upperLimit) row.target = row.upperLimit;
        const auto height = b.gauges[binding.frameIndex + 1]->heightAt0C;
        if (row.upperLimit)
        {
            row.pixelsPerValue = height / row.upperLimit;
            row.remainder = height % row.upperLimit;
        }
        b.rows.push_back(row);
    }
    const auto x = ReadNamed(d.centerXVariable), y = ReadNamed(d.centerYVariable);
    auto* canvas = Resolve<GraphicsSurfaceNativeBody>(b.common.canvasAt6C);
    canvas->originXAt10 = Sub(x, b.canvasSize.cx / 2);
    canvas->originYAt14 = Sub(y, b.canvasSize.cy / 2);
    auto* env = RequireGraffitiCountDialogNativeServices().source.CaptureEnvironmentAt4180C0();
    RECT moved{canvas->originXAt10, canvas->originYAt14,
        Add(canvas->originXAt10, b.canvasSize.cx), Add(canvas->originYAt14, b.canvasSize.cy)};
    RECT screen{0, 0, static_cast<LONG>(env->widthAt18), static_cast<LONG>(env->heightAt1C)};
    ::IntersectRect(&b.common.rectangleAt5C, &moved, &screen);
    RefreshGraffitiCountMultiNative(b);
    AddFullScreen(b.common);
    dialog->activeAt4C = nullptr;
    b.animation = b.lifecycle = 0;
    b.delay = ReadNamed(d.initialDelayVariable);
    b.skipResume = 2;
    b.skip = false;
}
std::int32_t __fastcall Poll(Dialog* dialog, void*)
{
    auto& b = *reinterpret_cast<Multi*>(dialog);
    switch (b.lifecycle)
    {
    case 0: b.lifecycle = 3; [[fallthrough]];
    case 3: dialog->vptrAt00->processInputAt54(dialog); return 1;
    case 4:
        AddFullScreen(b.common);
        dialog->vptrAt00->writeEnabledAt24(dialog, 0);
        b.lifecycle = -1;
        return 1;
    default: return 0;
    }
}
std::int32_t __fastcall End(Dialog* dialog, void*)
{
    const auto& b = *reinterpret_cast<Multi*>(dialog);
    // 16:4F3F40按序提交，包含重复selector；不写中心坐标/result。
    for (const auto& row : b.rows)
        WriteNamedLate(b.data->variables[row.selector].variable, row.target);
    return 1;
}
void __fastcall Input(Dialog* dialog, void*)
{
    auto& b = *reinterpret_cast<Multi*>(dialog);
    if (!b.skip)
    {
        const auto reason = MultiSkipRequested();
        if (reason == SkipReason::Latched)
        {
            b.skip = true;
            if (b.animation >= 2) b.delay = 0;
            else b.animation = 2;
        }
        else if (reason == SkipReason::Action5) b.animation = b.skipResume;
    }
    bool again = true;
    while (again)
    {
        again = false;
        switch (b.animation)
        {
        case 0:
        case 3:
            if (b.delay == 0) { ++b.animation; again = true; }
            else b.delay = Sub(b.delay, 1);
            break;
        case 1:
        {
            bool complete = true;
            for (auto& row : b.rows)
            {
                if (row.current != row.target)
                {
                    row.pixelProgress = Add(row.pixelProgress, 1);
                    if (row.pixelProgress >= row.pixelsPerValue &&
                        (Add(row.current, 1) != row.upperLimit ||
                         row.pixelProgress >= Add(row.pixelsPerValue, row.remainder)))
                    {
                        row.pixelProgress = 0;
                        row.current = Add(row.current, 1);
                        if (row.current == row.target && row.current == row.upperLimit)
                            row.pixelProgress = row.remainder;
                    }
                }
                complete = complete && row.current == row.target;
            }
            if (complete) { b.animation = 2; again = true; break; }
            b.delay = ReadNamed(b.data->stepDelayVariable);
            b.animation = 0;
            RefreshGraffitiCountMultiNative(b);
            AddDialogRectangle(b.common);
            PlayCountCue(b.data->countCue);
            break;
        }
        case 2:
            for (auto& row : b.rows)
            {
                row.current = row.target;
                row.pixelProgress = row.current == row.upperLimit ? row.remainder : 0;
            }
            b.animation = b.skip ? 4 : 3;
            b.delay = ReadNamed(b.data->finalDelayVariable);
            b.skipResume = 4;
            RefreshGraffitiCountMultiNative(b);
            AddDialogRectangle(b.common);
            break;
        case 4: b.lifecycle = 4; break;
        default: break;
        }
    }
    GuiDialogBaseVtable().processInputAt54(dialog);
}
}

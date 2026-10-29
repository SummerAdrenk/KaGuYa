#include <Runtime/TextWindowNativePlaybackServices.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::uint32_t Word(const std::uint8_t* bytes) noexcept
{
    std::uint32_t result;
    std::memcpy(&result, bytes, 4);
    return result;
}
UnSaveDataRuntime* CaptureUnsave(EnvironmentRootOwner& environment)
{
    return static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*environment.CaptureAt4180C0(), 2));
}
SaveDataEnvironmentArchive* CaptureSave(EnvironmentRootOwner& environment)
{
    return static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*environment.CaptureAt4180C0(), 1));
}
KeyboardManagerNativeBody438h* CaptureKeyboard(InputRootOwner& input)
{
    return ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input.CaptureAt418210()->keyboardAt18);
}
template<class T>
T* CaptureInlineAt4C2B69(EnvironmentRootOwner& environment, std::uint32_t index)
{
    const volatile auto& slots = environment.CaptureAt4180C0()->archivesAt30;
    // 4C2B69/4C2C07等内联段先end再begin；不能混同417990的反向读取顺序。
    const auto end = reinterpret_cast<std::uintptr_t>(slots.end);
    const auto begin = reinterpret_cast<std::uintptr_t>(slots.begin);
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(end - begin)) / 12);
    if (index >= count) return nullptr;
    const auto* slot = reinterpret_cast<const volatile YtlReferenceSlotRuntime*>(begin + index * 12u);
    const auto base = slot->baseAt00;
    if (!base) return nullptr;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot->controlAt08);
    const auto stride = control ? control->strideAt0C : 0u;
    const auto bias = slot->biasAt04;
    return reinterpret_cast<T*>(base + bias * stride);
}
}

TextWindowNativePlaybackServices::TextWindowNativePlaybackServices(EnvironmentRootOwner& environment,
    InputRootOwner& input, ITextWindowNativePlaybackSource& source, IRuntimeStringManagerSource& strings) noexcept
    : environment_(environment), input_(input), source_(source), strings_(strings) {}

bool TextWindowNativePlaybackServices::ReadSkipAt4AA540(TextWindowLayerNativeBodyE8h& body)
{
    if (ReadKeyboardSixHeldAt4AA573(*CaptureKeyboard(input_))) return true;
    if ((CaptureUnsave(environment_)->FlagsAt14() & 3u) != 3u) return false;
    if ((CaptureSave(environment_)->bytesAt34[4] & 1u) == 0) return true;
    return body.unreadAt9A == 0;
}

bool TextWindowNativePlaybackServices::ReadAdvanceInputAt4B0550(ClickInfoArchive& click)
{return ReadAdvanceInputNativeAt4B0550(click,input_);}
bool ReadAdvanceInputNativeAt4B0550(ClickInfoArchive& click,InputRootOwner& input)
{
    std::int32_t action;
    switch (Word(click.bytesAt04.data() + 4))
    {
    case 1: action = 5; break;
    case 2: action = 0; break;
    case 3: action = 8; break;
    default: return ReadMouseSelectorAt4B0630(input, Word(click.bytesAt04.data()));
    }
    if (ReadKeyboardPressedAt47F4E0(*CaptureKeyboard(input), action)) return true;
    // 4B0617才读同一Click的+04；捕获键盘回调可修改此selector。
    return ReadMouseSelectorAt4B0630(input, Word(click.bytesAt04.data()));
}

bool TextWindowNativePlaybackServices::IsPacedMouseAt4BB5D1()
{
    auto* const root = input_.CaptureAt418210();
    auto* const mouse = ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(root->mouseAt00);
    return mouse->secondaryAt2C != 0 || mouse->wheelAt34 >= 0;
}

void TextWindowNativePlaybackServices::ShowVoiceFailure()
{
    union Local { RuntimeCString text; Local() noexcept {} ~Local() noexcept {} } local;
    InitializeEmptyRuntimeCString(local.text, strings_); // 4C2FE3在下一次Environment捕获之前。
    struct Unwind final
    {
        RuntimeCString* text;
        ~Unwind() noexcept { if (text) DestroyRuntimeCStringAt40CD40(text); }
    } cleanup{&local.text};
    const auto names = CaptureInlineAt4C2B69<UnSaveDataRuntime>(environment_, 2)->ArrayReferenceAt3C().RawSlot();
    source_.FormatVoiceFailureAt417130(local.text, names);
    auto* const root = environment_.CaptureAt4180C0();
    source_.ShowMessageBoxA(reinterpret_cast<HWND>(root->windowAt2C),
        local.text.GetString(), root->titleAt10.GetString(), MB_ICONSTOP);
    // 4C3268先撤销清理资格，随后Free失败不重试，也不补433F60。
    cleanup.text = nullptr;
    DestroyRuntimeCStringAt40CD40(&local.text);
}

bool TextWindowNativePlaybackServices::PreparePlaybackAt4C2A40(TextWindowLayerNativeBodyE8h& body)
{
    // 4C2BAF只看base；非空零数量语音数组仍须进入批次Stop路径。
    if (CaptureInlineAt4C2B69<UnSaveDataRuntime>(environment_, 2)->ArrayReferenceAt3C().RawSlot().baseAt00 != 0)
    {
        bool skip = ReadKeyboardSixHeldAt4AA573(*CaptureKeyboard(input_));
        if (!skip && (CaptureInlineAt4C2B69<UnSaveDataRuntime>(environment_, 2)->FlagsAt14() & 3u) == 3u)
            skip = (CaptureInlineAt4C2B69<SaveDataEnvironmentArchive>(environment_, 1)->bytesAt34[4] & 1u) == 0 || body.unreadAt9A == 0;
        if (!skip || (CaptureInlineAt4C2B69<SaveDataEnvironmentArchive>(environment_, 1)->bytesAt34[4] & 2u) == 0)
        {
            // 4C2F4D先固定voices槽的地址，再Ensure并解析声音receiver。
            const auto* const names = &CaptureInlineAt4C2B69<UnSaveDataRuntime>(environment_, 2)->ArrayReferenceAt3C().RawSlot();
            auto sound = source_.CaptureSoundAfter413BE0();
            if (sound.Play(names))
            {
                sound = source_.CaptureSoundAfter413BE0();
                sound.Adjust();
                return true; // 成功分支不补433F60。
            }
            ShowVoiceFailure();
            sound = source_.CaptureSoundAfter413BE0();
            sound.Refresh();
            return false;
        }
    }
    const auto sound = source_.CaptureSoundAfter413BE0();
    sound.Refresh();
    return true;
}

void TextWindowNativePlaybackServices::ClearRootUpdateEnabledAt1B0After413BE0()
{
    source_.ClearRootUpdateEnabledAt1B0After413BE0();
}
}

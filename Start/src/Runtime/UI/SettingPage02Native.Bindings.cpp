#include "SettingPage02Native.Internal.h"
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <cstring>

namespace KaGuYa::Start::Runtime::SettingPage02NativeDetail
{
namespace
{
SaveDataEnvironmentArchive& Saved()
{
    auto* p = static_cast<SaveDataEnvironmentArchive*>(Registry1());
    if (!p) throw std::logic_error("Settings SaveData is missing");
    return *p;
}
GlobalParamsArchive& Globals()
{
    auto* root = RequireSettingPage02NativeServices().source.CaptureEnvironmentAt4180C0();
    auto* p = static_cast<GlobalParamsArchive*>(ResolveEnvironmentArchiveAt417990(*root, 7));
    if (!p) throw std::logic_error("Settings GlobalParams is missing");
    return *p;
}
template<std::size_t N> std::uint32_t Dword(const std::array<std::uint8_t,N>& b, std::size_t index)
{ std::uint32_t value; std::memcpy(&value, b.data() + index, sizeof(value)); return value; }
template<std::size_t N> void Dword(std::array<std::uint8_t,N>& b, std::size_t index, std::uint32_t value)
{ std::memcpy(b.data() + index, &value, sizeof(value)); }
unsigned Mask(SettingTextBoolField field) noexcept
{
    using F = SettingTextBoolField;
    switch (field)
    {
    case F::HideMessageDuringVoice: return 0x10;
    case F::StopVoiceOnAdvance: return 4;
    case F::SkipUnread: return 1;
    case F::ContinueSkip: return 8;
    case F::SkipVoice: return 2;
    case F::SkipEffectSound: return 0x20;
    default: return 0;
    }
}
std::uint8_t& Byte(SaveDataEnvironmentArchive& save, SettingTextBoolField field)
{
    using F = SettingTextBoolField;
    switch (field)
    {
    case F::Shadow: return save.bytesAt60[0];
    case F::Antialias: return save.bytesAt60[1];
    case F::ReadColor: return save.valueAt6C;
    case F::ScreenEffects: return save.valueAt6F;
    case F::ConfirmSave: return save.bytesAt77[0];
    case F::ConfirmLoad: return save.bytesAt77[1];
    case F::ConfirmQuickSave: return save.bytesAt77[2];
    case F::ConfirmQuickLoad: return save.bytesAt77[3];
    case F::ConfirmOverwrite: return save.bytesAt77[4];
    case F::ConfirmExit: return save.bytesAt77[5];
    case F::ConfirmTitle: return save.bytesAt77[6];
    case F::ConfirmDefaults: return save.bytesAt77[7];
    case F::ConfirmDeleteSave: return save.bytesAt77[8];
    case F::ConfirmDeleteAll: return save.bytesAt77[9];
    case F::SaveThumbnail: return save.bytesAt74[0];
    case F::SaveHistory: return save.bytesAt74[1];
    case F::AutoSaveChoice: return save.bytesAt74[2];
    default: throw std::logic_error("Unknown text/display boolean field");
    }
}
}
// Original offsets are evidence only. These bindings target the shared SaveData contract.
bool ReadSettingTextBool(SettingTextBoolField field)
{
    auto& save = Saved();
    if (const auto mask = Mask(field))
        return ((Dword(save.bytesAt34, 4) & mask) != 0) != (field == SettingTextBoolField::SkipUnread);
    return Read(Byte(save, field)) != 0;
}
void WriteSettingTextBool(SettingTextBoolField field, bool value)
{
    auto& save = Saved();
    if (const auto mask = Mask(field))
    {
        const auto before = Dword(save.bytesAt34, 4);
        Dword(save.bytesAt34, 4, value != (field == SettingTextBoolField::SkipUnread) ? before | mask : before & ~mask);
    }
    else Byte(save, field) = static_cast<std::uint8_t>(value);
}
int ReadSettingTextChoice(SettingTextChoiceField field)
{
    switch (field)
    {
    case SettingTextChoiceField::RightClick: return std::bit_cast<int>(Dword(Saved().bytesAt8C, 0));
    case SettingTextChoiceField::Insertion: return static_cast<std::int16_t>(Read(Globals().wordsAt0C[2]));
    case SettingTextChoiceField::Oral: return static_cast<std::int16_t>(Read(Globals().wordsAt0C[3]));
    }
    throw std::logic_error("Unknown text/display choice field");
}
void WriteSettingTextChoice(SettingTextChoiceField field, int value)
{
    switch (field)
    {
    case SettingTextChoiceField::RightClick: Dword(Saved().bytesAt8C, 0, static_cast<std::uint32_t>(value)); return;
    case SettingTextChoiceField::Insertion: Globals().wordsAt0C[2] = static_cast<std::uint16_t>(value); return;
    case SettingTextChoiceField::Oral: Globals().wordsAt0C[3] = static_cast<std::uint16_t>(value); return;
    }
    throw std::logic_error("Unknown text/display choice field");
}
std::uint32_t ReadSettingTextSlider(unsigned index)
{
    if (index > 1) throw std::logic_error("Unknown text slider field");
    return Dword(Saved().bytesAt34, index == 0 ? 0 : 8);
}
void WriteSettingTextSlider(unsigned index, std::uint32_t value)
{
    if (index > 1) throw std::logic_error("Unknown text slider field");
    Dword(Saved().bytesAt34, index == 0 ? 0 : 8, value);
}
std::uint32_t ReadSettingTextAlpha(unsigned index)
{
    if (index > 1) throw std::logic_error("Unknown message alpha field");
    auto& save = Saved();
    return Dword(index == 0 ? save.bytesAt40 : save.bytesAt48, 4);
}
void WriteSettingTextAlpha(unsigned index, std::uint32_t value)
{
    if (index > 1) throw std::logic_error("Unknown message alpha field");
    auto& save = Saved();
    Dword(index == 0 ? save.bytesAt40 : save.bytesAt48, 4, value);
}
}

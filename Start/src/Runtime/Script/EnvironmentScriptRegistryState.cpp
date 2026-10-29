#include <Runtime/EnvironmentScriptRegistryState.h>
#include <Runtime/EnvironmentRegistryRuntime.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentScriptRegistryState::EnvironmentScriptRegistryState(EnvironmentRegistryRuntime& registry) noexcept
    : registry_(registry), cg_(registry, false), scene_(registry, true),
      program_(*this, *this, cg_, scene_, *this) {}

void EnvironmentScriptRegistryState::WriteSlot2DwordAt04(std::uint32_t value)
{ registry_.UnSaveData().ModeAt04() = value; }
void EnvironmentScriptRegistryState::WriteSlot2ByteAt0C(std::uint8_t value)
{
    // 4D5423/4D557C是mov byte，不覆盖同一DWORD的高三字节。
    reinterpret_cast<unsigned char*>(&registry_.UnSaveData().DwordAt0C())[0] = value;
}
void EnvironmentScriptRegistryState::WriteSlot2DwordAt18(std::uint32_t value)
{ registry_.UnSaveData().FlagsAt18() = value; }
void EnvironmentScriptRegistryState::WriteSlot2DwordAt14(std::uint32_t value)
{ registry_.UnSaveData().FlagsAt14() = value; }
void EnvironmentScriptRegistryState::WriteSlot2DwordAt1C(std::uint32_t value)
{ registry_.UnSaveData().FlagsAt1C() = value; }
void EnvironmentScriptRegistryState::WriteSlot2DwordAt20(std::uint32_t value)
{ registry_.UnSaveData().FlagsAt20() = value; }
void EnvironmentScriptRegistryState::WriteSlot2ByteAt08(std::uint8_t value)
{ registry_.UnSaveData().BytesAt08()[0] = value; }
void EnvironmentScriptRegistryState::WriteSlot2ByteAt09(std::uint8_t value)
{ registry_.UnSaveData().BytesAt08()[1] = value; }
void EnvironmentScriptRegistryState::WriteSlot2ByteAt28(std::uint8_t value)
{ registry_.UnSaveData().BytesAt24()[4] = value; }
void EnvironmentScriptRegistryState::WriteSlot2ByteAt29(std::uint8_t value)
{ registry_.UnSaveData().BytesAt24()[5] = value; }
std::uint16_t EnvironmentScriptRegistryState::ReadSlot7WordAt0C() const
{ return registry_.GlobalParams().wordsAt0C[0]; }
const char* EnvironmentScriptRegistryState::ReadSlot1NameAt58() const
{ return registry_.SaveData().stringAt58.GetString(); }
const char* EnvironmentScriptRegistryState::ReadSlot1NameAt5C() const
{ return registry_.SaveData().stringAt5C.GetString(); }
void EnvironmentScriptRegistryState::CallSlot4Nullsub3()
{
    // 4D5733/4D573A必须取根解槽；4985A0仅retn，空正文不解引用、不增持。
    static_cast<void>(registry_.CaptureArchiveAt4180C0(4));
}

std::uint8_t EnvironmentScriptRegistryState::ReadByteAt84() const
{ return registry_.SaveData().bytesAt84[0]; }
std::uint8_t EnvironmentScriptRegistryState::ReadByteAt6F() const
{ return registry_.SaveData().valueAt6F; }
void EnvironmentScriptRegistryState::WriteChildVslot84Byte(std::int32_t index, bool value)
{
    // 4D0489解实际SaveData+1C，再按4D0495虚调；不能直接寻址人物表绕过虚槽。
    auto& sound = registry_.SaveData().BorrowSoundParamsAt1C();
    *sound.CharacterDisplayAt84(index) = value ? 1 : 0;
}
bool EnvironmentScriptRegistryState::ReadChildVslot84Byte(std::int32_t index) const
{
    auto& sound = registry_.SaveData().BorrowSoundParamsAt1C();
    return *sound.CharacterDisplayAt84(index) != 0;
}
void EnvironmentScriptRegistryState::WriteDwordAt14(std::int32_t value)
{ WriteSlot2DwordAt14(static_cast<std::uint32_t>(value)); }
void EnvironmentScriptRegistryState::WriteWordAt0C(std::uint32_t index, std::uint16_t value)
{
    // 合法下标由PROGRAM调用者检查；越界原为任意内存写，安全边界明确拒绝，不钳位。
    registry_.GlobalParams().wordsAt0C.at(index) = value;
}
std::uint16_t EnvironmentScriptRegistryState::ReadWordAt0C(std::uint32_t index) const
{ return registry_.GlobalParams().wordsAt0C.at(index); }
void EnvironmentScriptRegistryState::CallNullsub2()
{
    // 4980F0仅retn；4D03DE/4D03E7的查询已由RequireRegistrySlot4完成，不重复取根。
}
void EnvironmentScriptRegistryState::Toggle::CallVslot18(std::int32_t value)
{
    if (scene_) registry_.SecretSc().FillFlagsAtVslot18(static_cast<std::uint8_t>(value));
    else registry_.SecretCg().FillFlagsAtVslot18(static_cast<std::uint8_t>(value));
}
void EnvironmentScriptRegistryState::Toggle::ResolveForNullsub()
{ static_cast<void>(registry_.CaptureArchiveAt4180C0(scene_ ? 4 : 3)); }
}

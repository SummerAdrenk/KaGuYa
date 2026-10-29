#include <Runtime/SaveDataEnvironmentOwner.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
class FixedSounds final : public ISoundParamsSettingsSource
{
public:
    explicit FixedSounds(const ParamsGameSystem& game) noexcept : game_(game) {}
    const ParamsGameSystem& ReadGameSystem(SoundParamsSettingsReadPoint) override { return game_; }
private:
    const ParamsGameSystem& game_;
};
EnvironmentSoundParamsArchive& Sound(const YtlReferenceSlotRuntime& slot) noexcept
{
    auto* value = ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(slot);
    if (!value) _invalid_parameter_noinfo_noreturn();
    return *value;
}
}

SaveDataEnvironmentOwner::ArchiveOwner SaveDataEnvironmentOwner::Create(
    ISaveDataConstructionSource& source, const SaveDataConstructionStorage& storage,
    ISoundParamsSettingsSource& sounds, ISettingTagErrorSource& errors)
{
    auto* raw = static_cast<std::byte*>(::operator new(0x90));
    // 现有启动接口显式指定六个原未写字节，在构造前放进原存储，构造函数不补零。
    std::memcpy(raw + 0x06, storage.storageBytesAt06.data(), 2);
    std::memcpy(raw + 0x62, storage.storageBytesAt62.data(), 2);
    std::memcpy(raw + 0x86, storage.storageBytesAt86.data(), 2);
    try { return ArchiveOwner(::new (raw) SaveDataEnvironmentArchive(source, sounds, errors)); }
    catch (...) { ::operator delete(raw, std::size_t{0x90}); throw; }
}

SaveDataEnvironmentOwner::ArchiveOwner SaveDataEnvironmentOwner::CreateFixed(
    const SystemDataEnvironmentArchive& system, const ParamsRuntimeProfile& profile,
    const SaveDataConstructionStorage& storage, ISoundParamsSettingsSource* sounds,
    ISettingTagErrorSource& errors)
{
    SaveDataFixedConstructionSource source(system, profile.gameSystem);
    FixedSounds fixedSounds(profile.gameSystem);
    return Create(source, storage, sounds ? *sounds : fixedSounds, errors);
}

SaveDataEnvironmentOwner::SaveDataEnvironmentOwner(ArchiveOwner archive)
    : saveData_(std::move(archive)),
      soundParamsAt1C_(EnvironmentSoundParamsRuntime::BorrowConstructedTag{}, Sound(saveData_->soundParamsAt1C)),
      soundParamsAt28_(EnvironmentSoundParamsRuntime::BorrowConstructedTag{}, Sound(saveData_->soundParamsAt28))
{
}
SaveDataEnvironmentOwner::SaveDataEnvironmentOwner(const SystemDataEnvironmentArchive& system,
    const ParamsRuntimeProfile& profile, const SaveDataConstructionStorage& storage,
    ISettingTagErrorSource& errors)
    : SaveDataEnvironmentOwner(CreateFixed(system, profile, storage, nullptr, errors)) {}
SaveDataEnvironmentOwner::SaveDataEnvironmentOwner(const SystemDataEnvironmentArchive& system,
    const ParamsRuntimeProfile& profile, const SaveDataConstructionStorage& storage,
    ISoundParamsSettingsSource& sounds, ISettingTagErrorSource& errors)
    : SaveDataEnvironmentOwner(CreateFixed(system, profile, storage, &sounds, errors)) {}
SaveDataEnvironmentOwner::SaveDataEnvironmentOwner(ISaveDataConstructionSource& source,
    const SaveDataConstructionStorage& storage, ISoundParamsSettingsSource& sounds,
    ISettingTagErrorSource& errors)
    : SaveDataEnvironmentOwner(Create(source, storage, sounds, errors)) {}

const YtlReferenceSlotRuntime& SaveDataEnvironmentOwner::SoundReferenceAt1C() const noexcept
{ return saveData_->soundParamsAt1C; }
SaveDataEnvironmentArchive& SaveDataEnvironmentOwner::SaveData() noexcept { return *saveData_; }
const SaveDataEnvironmentArchive& SaveDataEnvironmentOwner::SaveData() const noexcept { return *saveData_; }
EnvironmentSoundParamsArchive& SaveDataEnvironmentOwner::SoundParamsAt1C() noexcept
{ return Sound(saveData_->soundParamsAt1C); }
const EnvironmentSoundParamsArchive& SaveDataEnvironmentOwner::SoundParamsAt1C() const noexcept
{ return Sound(saveData_->soundParamsAt1C); }
EnvironmentSoundParamsRuntime& SaveDataEnvironmentOwner::SoundRuntimeAt1C() noexcept { return soundParamsAt1C_; }
const EnvironmentSoundParamsRuntime& SaveDataEnvironmentOwner::SoundRuntimeAt1C() const noexcept { return soundParamsAt1C_; }
EnvironmentSoundParamsArchive& SaveDataEnvironmentOwner::SoundParamsAt28() noexcept
{ return Sound(saveData_->soundParamsAt28); }
const EnvironmentSoundParamsArchive& SaveDataEnvironmentOwner::SoundParamsAt28() const noexcept
{ return Sound(saveData_->soundParamsAt28); }
EnvironmentSoundParamsRuntime& SaveDataEnvironmentOwner::SoundRuntimeAt28() noexcept { return soundParamsAt28_; }
const EnvironmentSoundParamsRuntime& SaveDataEnvironmentOwner::SoundRuntimeAt28() const noexcept { return soundParamsAt28_; }
}

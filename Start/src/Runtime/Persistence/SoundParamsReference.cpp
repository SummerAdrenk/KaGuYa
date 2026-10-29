#include <Runtime/SoundParamsReference.h>
#include <array>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredControl final : public YtlReferenceControlRuntime
{
public:
    RetiredControl() noexcept : YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { if (flags & 1u) ::operator delete(this, std::size_t{0x14}); }
    std::uint32_t stride, count;
};
class SoundParamsControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SoundParamsControl(EnvironmentSoundParamsArchive* body) noexcept :
        EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // PE5B2FD0 -> 492D40：调用正文+04(flags1)，不是按静态C++类型delete。
        if (payloadAt14) static_cast<IEnvironmentArchiveRuntime*>(payloadAt14)->DeleteAtVslot04(1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        std::array<std::byte, 20> preserved;
        auto* raw = reinterpret_cast<std::byte*>(this);
        std::memcpy(preserved.data(), raw + 4, preserved.size());
        ::new (raw) RetiredControl;
        std::memcpy(raw + 4, preserved.data(), preserved.size());
        if (flags & 1u) ::operator delete(raw, std::size_t{0x18});
    }
};
static_assert(sizeof(RetiredControl) == 0x14 && sizeof(SoundParamsControl) == 0x18);
class FixedSource final : public ISoundParamsSettingsSource
{
public:
    explicit FixedSource(const ParamsGameSystem& game) noexcept : game_(game) {}
    const ParamsGameSystem& ReadGameSystem(SoundParamsSettingsReadPoint) override { return game_; }
private:
    const ParamsGameSystem& game_;
};
}
EnvironmentSoundParamsArchive* CreateSoundParamsAt45B4D8(ISoundParamsSettingsSource& source,
    ISettingTagErrorSource& errors, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    auto* raw = memory.AllocateBody(0x44);
    if (!raw) return nullptr;
    try { return ::new (raw) EnvironmentSoundParamsArchive(source, errors, strings); }
    catch (...) { memory.DeleteBody(raw, 0x44); throw; } // 45B4EC/state5 -> 5968D4。
}
void ReplaceSoundParamsAt451800(YtlReferenceSlotRuntime& slot, EnvironmentSoundParamsArchive* created,
    IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!created) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    // 正文发布后才分配control；失败时不能自动回收正文或恢复旧Ref。
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new (raw) SoundParamsControl(created) : nullptr;
    slot.controlAt08 = control;
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x44;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
SoundParamsReference::SoundParamsReference(ISoundParamsSettingsSource& source, ISettingTagErrorSource& errors)
{ ReplaceSoundParamsAt451800(slot_, CreateSoundParamsAt45B4D8(source, errors)); }
void ShareSoundParamsAt45D1A3(YtlReferenceSlotRuntime& destination,
    const YtlReferenceSlotRuntime& source)
{
    if (&destination == &source) return;
    ReleaseYtlReference(destination);
    destination.baseAt00 = source.baseAt00;
    destination.biasAt04 = source.biasAt04;
    destination.controlAt08 = source.controlAt08;
    if (destination.controlAt08) ++destination.controlAt08->referenceCountAt04;
}
SoundParamsReference::SoundParamsReference(const YtlReferenceSlotRuntime& source)
{ ShareSoundParamsAt45D1A3(slot_, source); }
SoundParamsReference::SoundParamsReference(const ParamsGameSystem& gameSystem, ISettingTagErrorSource& errors)
{
    FixedSource source(gameSystem);
    ReplaceSoundParamsAt451800(slot_, CreateSoundParamsAt45B4D8(source, errors));
}
SoundParamsReference::~SoundParamsReference() noexcept { ReleaseYtlReference(slot_); }
EnvironmentSoundParamsArchive& SoundParamsReference::Get() const noexcept
{
    auto* body = ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(slot_);
    if (!body) _invalid_parameter_noinfo_noreturn();
    return *body;
}
}

#include <Runtime/SoundParamsBaseNative.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
std::int32_t Count(const EnvironmentVectorStorage<YtlReferenceSlotRuntime>& table) noexcept
{
    return std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(table.end) - reinterpret_cast<std::uintptr_t>(table.begin))) / 12;
}
SoundKeyValueNative& Entry(const YtlReferenceSlotRuntime& slot)
{
    auto* entry = ResolveEnvironmentObjectReference<SoundKeyValueNative>(slot);
    if (!entry) _invalid_parameter_noinfo_noreturn();
    return *entry;
}
}
SoundParamsBaseNative::~SoundParamsBaseNative() noexcept
{
    // 4640BB：进入基类释放阶段前必须切回三槽表，不能保留派生46槽。
}
void SoundParamsBaseNative::Serialize(SerializeBuffer& serializer)
{
    const auto& table = keyValuesAt04.Native();
    auto count = serializer.IsWriting() ? static_cast<std::uint32_t>(Count(table)) : 0u;
    serializer.SerializeSlot58Dword(count);
    if (serializer.IsWriting())
    {
        if (count)
            for (auto* item = table.begin; item != table.end; ++item) Entry(*item).Serialize(serializer);
        return;
    }
    for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(count); ++index)
    {
        if (index < Count(table)) Entry(table.begin[index]).Serialize(serializer);
        else
        {
            // 保留原GetNil及首kind读取的时点；不将超额项当kind=0或FFFFFFFF来消费。
            ArchiveRawResourceNameRuntime key(ArchiveRawResourceNameRuntime::ProducerTag{},
                [](RuntimeCString* out) { InitializeEmptyRuntimeCString(*out); });
            std::uint32_t incomingKind;
            serializer.SerializeSlot58Dword(incomingKind);
            throw SoundIndeterminateArchiveKind{};
        }
    }
}
void* SoundParamsBaseNative::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    // 删除虚槽内固定析构本类型；保留基类退表及flags控制的10h外块释放。
    this->SoundParamsBaseNative::~SoundParamsBaseNative();
    if (flags & 1u) ::operator delete(this, std::size_t{0x10});
    return this;
}
std::int32_t SoundParamsBaseNative::AddKeyValueAt08(const char* key, std::uint32_t kind,
    const YtlReferenceSlotRuntime& value)
{ return keyValuesAt04.AddAt46A650(key, kind, value); }
}

#pragma once
#include <Runtime/SoundKeyValueNative.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
// 48BCB4将未初始化的栈kind交给48BD00。没有稳定原值时不能伪造类型或读未初始化C++对象。
class SoundIndeterminateArchiveKind final : public std::runtime_error
{
public:
    SoundIndeterminateArchiveKind() : std::runtime_error("48BCB4: original surplus KeyValue kind is indeterminate") {}
};

// PE5B24D0只有三个虚槽，析构不能增加槽。派生CSoundParams覆盖Serialize为48B750。
class SoundParamsBaseNative : public IEnvironmentArchiveRuntime
{
public:
    SoundParamsBaseNative() noexcept = default;
    ~SoundParamsBaseNative() noexcept;
    void Serialize(SerializeBuffer& serializer);
    void SerializeNative18(void* serializer18) override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;
    virtual std::int32_t AddKeyValueAt08(const char* key, std::uint32_t kind,
        const YtlReferenceSlotRuntime& value);
    SoundKeyValueTableNative keyValuesAt04;
};
static_assert(sizeof(SoundParamsBaseNative) == 0x10);
static_assert(offsetof(SoundParamsBaseNative, keyValuesAt04) == 4);
}

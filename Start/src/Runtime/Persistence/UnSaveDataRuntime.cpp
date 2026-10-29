#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/RuntimeCStringVector.h>

#include <bit>
#include <cstring>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
void InitializeMusicName(UnSaveDataRuntime& value, IRuntimeStringManagerSource& strings)
{
    auto* manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL);
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    GetNil getNil;
    std::memcpy(&getNil, table + 0x0C, sizeof(getNil));
    // 45E31B..45E320：原可抛虚槽返回后才发布，不调用新版noexcept GetNilString。
    const auto data = reinterpret_cast<std::uintptr_t>(getNil(manager)) + 0x10u;
    std::memcpy(&value.stringAt2C_, &data, sizeof(data));
}

struct ConstructorCleanup final
{
    UnSaveDataRuntime* value;
    IEnvironmentRootMemory& memory;
    ~ConstructorCleanup() noexcept
    {
        if (!value) return;
        // 596DCE ->596DC3 ->596DB8；清理再抛出仍在展开边界终止。
        value->arrayReferenceAt3C_.Reset();
        DestroyRuntimeCStringVectorAt40DD20(value->mappingsAt30_, memory);
        DestroyRuntimeCStringAt40CD40(&value->stringAt2C_);
    }
};

template<class ReadCount>
void ConstructAt45E2D0(UnSaveDataRuntime& value, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory, ReadCount readCount)
{
    InitializeMusicName(value, strings);
    value.mappingsAt30_ = {nullptr, nullptr, nullptr};
    ::new (&value.arrayReferenceAt3C_) UnSaveDataStringArrayReference;
    ConstructorCleanup cleanup{&value, memory};
    std::memset(&value.modeAt04_, 0, 0x28);
    SetRuntimeCStringAt437680(value.stringAt2C_, "", 0);
    const auto count = readCount();
    ResizeRuntimeCStringVectorAt456E80(value.mappingsAt30_, count, strings, memory);
    // 45E3E7每轮重新取end，不能在Set可能重入前缓存整个范围。
    for (auto* item = value.mappingsAt30_.begin; item != value.mappingsAt30_.end; ++item)
        SetRuntimeCStringAt437680(*item, "", 0);
    value.arrayReferenceAt3C_.Reset();
    value.dwordAt48_ = 0;
    cleanup.value = nullptr;
}

}

const ParamsGameSystem* UnSaveDataParamsSource::CaptureGameSystemAt418480()
{
    return params_.GetOrCreateAt418480().GameSystem().get();
}

UnSaveDataRuntime::UnSaveDataRuntime(IUnSaveDataGameSystemSource& source,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    ConstructAt45E2D0(*this, strings, memory, [&source]
    {
        const auto* first = source.CaptureGameSystemAt418480();
        const auto* second = source.CaptureGameSystemAt418480();
        const auto secondCount = second->soundSlotCounts[1];
        return secondCount + first->soundSlotCounts[3];
    });
}

UnSaveDataRuntime::UnSaveDataRuntime(const ParamsGameSystem& gameSystem)
{
    ConstructAt45E2D0(*this, DefaultRuntimeStringManagerSource(), DefaultEnvironmentRootMemory(),
        [&gameSystem] { return gameSystem.soundSlotCounts[1] + gameSystem.soundSlotCounts[3]; });
}

UnSaveDataRuntime::UnSaveDataRuntime(
    const std::uint32_t firstEffectCountAtGameSystem50,
    const std::uint32_t secondEffectCountAtGameSystem48)
{
    ConstructAt45E2D0(*this, DefaultRuntimeStringManagerSource(), DefaultEnvironmentRootMemory(),
        [=] { return firstEffectCountAtGameSystem50 + secondEffectCountAtGameSystem48; });
}

UnSaveDataRuntime::~UnSaveDataRuntime() noexcept
{
    arrayReferenceAt3C_.Reset();
    DestroyRuntimeCStringVectorAt40DD20(mappingsAt30_, DefaultEnvironmentRootMemory());
    DestroyRuntimeCStringAt40CD40(&stringAt2C_);
}

void* UnSaveDataRuntime::DeleteAtVslot04(const std::uint32_t flags) noexcept
{
    this->~UnSaveDataRuntime();
    if (flags & 1u) ::operator delete(this, OriginalObjectSize);
    return this;
}

std::size_t UnSaveDataRuntime::Size() const noexcept
{
    return (reinterpret_cast<std::uintptr_t>(mappingsAt30_.end) -
        reinterpret_cast<std::uintptr_t>(mappingsAt30_.begin)) / sizeof(RuntimeCString);
}

std::string_view UnSaveDataRuntime::ResourceNameAt(const std::size_t port) const
{
    if (port >= Size())
        throw std::out_of_range("CUnSaveData sound mapping port is out of range");
    return RuntimeStringView(mappingsAt30_.begin[port]);
}

void UnSaveDataRuntime::ClearAll()
{
    for (auto* item = mappingsAt30_.begin; item != mappingsAt30_.end; ++item)
        SetRuntimeCStringAt437680(*item, "", 0);
}

void UnSaveDataRuntime::Clear(const std::size_t port)
{
    if (port >= Size())
        throw std::out_of_range("CUnSaveData sound mapping port is out of range");
    SetRuntimeCStringAt437680(mappingsAt30_.begin[port], "", 0);
}

void UnSaveDataRuntime::Store(
    const std::size_t port,
    const std::string_view resourceName)
{
    if (port >= Size())
        throw std::out_of_range("CUnSaveData sound mapping port is out of range");

    SetRuntimeCStringAt437680(mappingsAt30_.begin[port], resourceName.data(),
        std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(resourceName.size())));
}

void UnSaveDataRuntime::StorePatternName(const std::size_t port,
    const ScrResourceNameReceiver receiver, const std::int32_t resourceId)
{
    auto& mapping = mappingsAt30_;
    const char* name = receiver.FindNameAt4AD1B0(resourceId);
    // 已捕获vector；查询返回后才取begin，再扫描名字。重入不能换成新slot2。
    auto* destination = reinterpret_cast<RuntimeCString*>(
        reinterpret_cast<std::uintptr_t>(mapping.begin) + static_cast<std::uint32_t>(port) * 4u);
    const auto length = name ? static_cast<std::uint32_t>(std::strlen(name)) : 0u;
    SetRuntimeCStringAt437680(*destination, name, std::bit_cast<std::int32_t>(length));
}

void UnSaveDataRuntime::Serialize(SerializeBuffer& serializer) noexcept
{
    // 48BA10 是仅执行 retn 4 的 nullsub：不读取模式，不访问对象，也不移动游标。
    static_cast<void>(serializer);
}

std::uint32_t& UnSaveDataRuntime::ModeAt04() noexcept { return modeAt04_; }
const std::uint32_t& UnSaveDataRuntime::ModeAt04() const noexcept { return modeAt04_; }
std::array<std::uint8_t, 4>& UnSaveDataRuntime::BytesAt08() noexcept { return bytesAt08_; }
const std::array<std::uint8_t, 4>& UnSaveDataRuntime::BytesAt08() const noexcept { return bytesAt08_; }
std::uint32_t& UnSaveDataRuntime::DwordAt0C() noexcept { return dwordAt0C_; }
const std::uint32_t& UnSaveDataRuntime::DwordAt0C() const noexcept { return dwordAt0C_; }
std::uint32_t& UnSaveDataRuntime::DwordAt10() noexcept { return dwordAt10_; }
const std::uint32_t& UnSaveDataRuntime::DwordAt10() const noexcept { return dwordAt10_; }
std::uint32_t& UnSaveDataRuntime::FlagsAt14() noexcept { return flagsAt14_; }
const std::uint32_t& UnSaveDataRuntime::FlagsAt14() const noexcept { return flagsAt14_; }
std::uint32_t& UnSaveDataRuntime::FlagsAt18() noexcept { return flagsAt18_; }
const std::uint32_t& UnSaveDataRuntime::FlagsAt18() const noexcept { return flagsAt18_; }
std::uint32_t& UnSaveDataRuntime::FlagsAt1C() noexcept { return flagsAt1C_; }
const std::uint32_t& UnSaveDataRuntime::FlagsAt1C() const noexcept { return flagsAt1C_; }
std::uint32_t& UnSaveDataRuntime::FlagsAt20() noexcept { return flagsAt20_; }
const std::uint32_t& UnSaveDataRuntime::FlagsAt20() const noexcept { return flagsAt20_; }
std::array<std::uint8_t, 8>& UnSaveDataRuntime::BytesAt24() noexcept { return bytesAt24_; }
const std::array<std::uint8_t, 8>& UnSaveDataRuntime::BytesAt24() const noexcept { return bytesAt24_; }
RuntimeCString& UnSaveDataRuntime::StringAt2C() noexcept { return stringAt2C_; }
const RuntimeCString& UnSaveDataRuntime::StringAt2C() const noexcept { return stringAt2C_; }

std::span<const RuntimeCString> UnSaveDataRuntime::MappingsAt30() const noexcept
{
    return {mappingsAt30_.begin, Size()};
}

UnSaveDataStringArrayReference& UnSaveDataRuntime::ArrayReferenceAt3C() noexcept
{
    return arrayReferenceAt3C_;
}

const UnSaveDataStringArrayReference&
UnSaveDataRuntime::ArrayReferenceAt3C() const noexcept
{
    return arrayReferenceAt3C_;
}

std::uint32_t& UnSaveDataRuntime::DwordAt48() noexcept { return dwordAt48_; }
const std::uint32_t& UnSaveDataRuntime::DwordAt48() const noexcept { return dwordAt48_; }

}

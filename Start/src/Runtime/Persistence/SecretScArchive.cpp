#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/ParamsRuntimeRoot.h>

#include <bit>
#include <cstdlib>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class FixedCount final : public ISecretScCountSource
{
public:
    explicit FixedCount(std::uint32_t count) noexcept : count_(count) {}
    std::uint32_t CaptureSceneCountAt45CB5A() override { return count_; }
private:
    std::uint32_t count_;
};

// 498100每次重新读虚表；读模式在key调用前固定表、调用后取setter。
template<class Function>
Function Slot(const SecretScArchive* self, std::size_t index) noexcept
{
    return reinterpret_cast<Function>((*reinterpret_cast<void* const* const*>(self))[index]);
}
using Count = std::uint32_t(__thiscall*)(const SecretScArchive*);
using Get = bool(__thiscall*)(const SecretScArchive*, std::uint32_t);
using Set = void(__thiscall*)(SecretScArchive*, std::uint32_t, std::uint8_t);
}

std::uint32_t SecretScParamsSource::CaptureSceneCountAt45CB5A()
{
    const auto* gameSystem = params_.GetOrCreateAt418480().GameSystem().get();
    // 原45CB80在null正文处硬故障，不能转成C++异常后被构造catch伪装为ready=0。
    // 现代边界明确终止，不执行未定义的null成员读取。
    if (!gameSystem) _invalid_parameter_noinfo_noreturn();
    return static_cast<std::uint32_t>(gameSystem->sceneNames.size());
}

SecretScArchive::SecretScArchive(ISecretScCountSource& source, IEnvironmentRootMemory& memory)
    : bitsAt0C{nullptr, nullptr, nullptr, 0}
{
    InitializeAt45CAF0(source, memory);
}

SecretScArchive::SecretScArchive(std::uint32_t count) : bitsAt0C{nullptr, nullptr, nullptr, 0}
{
    FixedCount source(count);
    InitializeAt45CAF0(source, DefaultEnvironmentRootMemory());
}

void SecretScArchive::InitializeAt45CAF0(ISecretScCountSource& source, IEnvironmentRootMemory& memory)
{
    readyAt04 = false;
    baseKeyOffsetAt08 = 0;
    try
    {
        const auto count = source.CaptureSceneCountAt45CB5A();
        if (count) ResizeRuntimeBitVectorAt494BD0(bitsAt0C, count, false, memory);
        // 4978F0为nullsub；45CBA4仍显式逐bit清零，最后才发布ready。
        FillFlagsAtVslot18(false);
        readyAt04 = true;
    }
    catch (...)
    {
        // 5E8634/5E8624 -> 45CC0B/45CC11：保留同一正文及部分vector，正常返回。
        // 这是原明确catch分支，不销毁成员、不创建替代空对象，也不阻止工厂登记。
    }
}

SecretScArchive::~SecretScArchive() noexcept
{
    DestroyRuntimeBitVectorAt462850(bitsAt0C);
}

void* SecretScArchive::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    this->~SecretScArchive();
    if (flags & 1u) ::operator delete(this, OriginalObjectSize);
    return this;
}

std::uint32_t SecretScArchive::CountAtVslot08() const noexcept { return bitsAt0C.countAt0C; }

bool SecretScArchive::GetFlagAtVslot0C(std::uint32_t index) const noexcept
{
    // 47A130无范围检查；上层现代安全检查不移入原槽。
    return (bitsAt0C.beginAt00[index >> 5] & (1u << (index & 31u))) != 0;
}

void SecretScArchive::SetFlagAtVslot10(std::uint32_t index, std::uint8_t value) noexcept
{
    auto& word = bitsAt0C.beginAt00[index >> 5];
    const auto mask = 1u << (index & 31u);
    word = value ? word | mask : word & ~mask;
}

void SecretScArchive::SetKeyBaseAtVslot14(std::uint32_t base) noexcept { baseKeyOffsetAt08 = base; }

void SecretScArchive::FillFlagsAtVslot18(std::uint8_t value) noexcept
{
    auto cursor = reinterpret_cast<std::uintptr_t>(bitsAt0C.beginAt00);
    std::uint32_t bit = 0;
    for (;;)
    {
        // 46D0F0每轮重取begin/count；负count的结束迭代器也按x86 DWORD回绕计算。
        const auto count = bitsAt0C.countAt0C;
        const auto words = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(count) >> 5);
        const auto end = reinterpret_cast<std::uintptr_t>(bitsAt0C.beginAt00) + words * 4u;
        if (cursor == end && bit == (count & 31u)) return;
        auto& word = *reinterpret_cast<std::uint32_t*>(cursor);
        word = value ? word | (1u << bit) : word & ~(1u << bit);
        if (bit < 31) ++bit;
        else { bit = 0; cursor += 4; }
    }
}

void SecretScArchive::Serialize(SerializeBuffer& serializer)
{
    if (serializer.IsWriting())
    {
        auto count = Slot<Count>(this, 2)(this);
        serializer.SerializeSlot58Dword(count);
        for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(Slot<Count>(this, 2)(this)); ++index)
        {
            std::uint8_t encoded = 0;
            if (Slot<Get>(this, 3)(this, static_cast<std::uint32_t>(index)))
                encoded = serializer.EnvironmentKeys().NormalizedByteAt(baseKeyOffsetAt08 + index);
            serializer.SerializeSlot50Byte(encoded);
        }
        return;
    }
    std::uint32_t remaining = 0;
    serializer.SerializeSlot58Dword(remaining);
    for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(Slot<Count>(this, 2)(this)); ++index)
    {
        if (!remaining) Slot<Set>(this, 4)(this, index, false);
        else
        {
            std::uint8_t encoded = 0;
            serializer.SerializeSlot50Byte(encoded);
            const auto keyIndex = baseKeyOffsetAt08 + index;
            auto* const table = *reinterpret_cast<void* const* const*>(this);
            const auto key = serializer.EnvironmentKeys().NormalizedByteAt(keyIndex);
            reinterpret_cast<Set>(table[4])(this, index, encoded == key);
            --remaining;
        }
    }
    // 498100不消费超出运行时count的尾项，不回滚已写bit，也不改ready。
}
}

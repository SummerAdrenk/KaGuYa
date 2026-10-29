#include <Runtime/ParamsTextArrayRuntime.h>

#include <algorithm>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
static_assert(alignof(ParamsText) <= alignof(std::uint32_t));

std::size_t AllocationBytes(std::uint32_t count)
{
    constexpr auto maximum = (std::numeric_limits<std::size_t>::max)();
    if (count > (maximum - sizeof(std::uint32_t)) / sizeof(ParamsText)) return maximum;
    return sizeof(std::uint32_t) + std::size_t(count) * sizeof(ParamsText);
}

class Memory final : public IParamsTextArrayMemory
{
public:
    void* Allocate(ParamsTextArrayAllocation, std::size_t bytes) override { return ::operator new(bytes); }
    void Deallocate(ParamsTextArrayAllocation, void* address, std::size_t) noexcept override { ::operator delete(address); }
    void Construct(ParamsText* element) override { new (element) ParamsText; }
    void Destroy(ParamsText* element) override { element->~ParamsText(); }
};

void DestroyPrefix(IParamsTextArrayMemory& memory, ParamsText* base, std::uint32_t count) noexcept
{
    // 53FB52展开已完成的构造前缀；展开中的析构异常终止，不替换原异常。
    while (count) memory.Destroy(base + --count);
}

class Control final : public ParamsTextArrayControl
{
public:
    Control(ParamsText* base, std::uint32_t count, IParamsTextArrayMemory& memory) noexcept : memory_(memory)
        { baseAt14 = base; countAt10 = count; }
    void DestroyPayloadAtVslot00() override
    {
        if (!baseAt14) return;
        auto* const base = baseAt14;
        auto* const cookie = reinterpret_cast<std::uint32_t*>(base) - 1;
        auto count = *cookie;
        while (count)
        {
            --count;
            try { memory_.Destroy(base + count); }
            catch (...)
            {
                // 53FAB4/53FB2D继续展开更低索引；不重试失败元素，也不释放外层块。
                DestroyPrefix(memory_, base, count);
                throw;
            }
        }
        // 43BB3B在析构回调后重新读取cookie用于释放大小，不使用control.count。
        memory_.Deallocate(ParamsTextArrayAllocation::Elements, cookie, AllocationBytes(*cookie));
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        auto& memory = memory_;
        this->~Control();
        if (flags & 1) memory.Deallocate(ParamsTextArrayAllocation::Control, this, sizeof(Control));
    }
private:
    IParamsTextArrayMemory& memory_;
};

void Release(ParamsTextArraySlot& slot, bool reread)
{
    auto* control = slot.controlAt08;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        if (reread) control = slot.controlAt08;
        if (control) control->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}

std::span<const ParamsText> SnapshotValues(const ParamsTextArray& source)
{
    if (!source.begin() && !source.empty())
        throw std::logic_error("Params value snapshot has a count but no payload");
    return {source.begin(), source.size()};
}
}

IParamsTextArrayMemory& DefaultParamsTextArrayMemory() { static Memory memory; return memory; }

ParamsTextArrayReference::ParamsTextArrayReference(const ParamsTextArrayReference& other) noexcept : slot_(other.slot_)
    { if (slot_.controlAt08) ++slot_.controlAt08->referenceCountAt04; }
ParamsTextArrayReference::ParamsTextArrayReference(ParamsTextArrayReference&& other) noexcept
    : slot_(std::exchange(other.slot_, {})),
      releaseOnDestruction_(std::exchange(other.releaseOnDestruction_, true)) {}
ParamsTextArrayReference& ParamsTextArrayReference::operator=(const ParamsTextArrayReference& other)
{
    if (this != &other)
    {
        Reset(); slot_ = other.slot_; releaseOnDestruction_ = true;
        if (slot_.controlAt08) ++slot_.controlAt08->referenceCountAt04;
    }
    return *this;
}
ParamsTextArrayReference& ParamsTextArrayReference::operator=(ParamsTextArrayReference&& other)
{
    if (this != &other)
    {
        Reset(); slot_ = std::exchange(other.slot_, {});
        releaseOnDestruction_ = std::exchange(other.releaseOnDestruction_, true);
    }
    return *this;
}
ParamsTextArrayReference::~ParamsTextArrayReference() noexcept { if (releaseOnDestruction_) Reset(); }
ParamsText* ParamsTextArrayReference::Get() const noexcept
{
    if (!slot_.baseAt00) return nullptr;
    const auto stride = slot_.controlAt08 ? slot_.controlAt08->strideAt0C : 0u;
    return reinterpret_cast<ParamsText*>(slot_.baseAt00 + slot_.biasAt04 * stride);
}
ParamsTextArrayReference ParamsTextArrayReference::Adopt(ParamsTextArraySlot& slot) noexcept
{
    ParamsTextArrayReference result;
    result.slot_ = std::exchange(slot, {});
    return result;
}
void ParamsTextArrayReference::Reset() { Release(slot_, true); }
void ParamsTextArrayReference::ResetSavedControlAt46EFA1()
{
    releaseOnDestruction_ = false;
    Release(slot_, false);
}

void ParamsTextArrayReference::RebuildAt412870(std::int32_t count, IParamsTextArrayMemory& memory)
{
    Reset();
    releaseOnDestruction_ = true;
    if (count <= 0) return;
    auto* const cookie = static_cast<std::uint32_t*>(memory.Allocate(
        ParamsTextArrayAllocation::Elements, AllocationBytes(static_cast<std::uint32_t>(count))));
    ParamsText* base = nullptr;
    if (cookie)
    {
        *cookie = static_cast<std::uint32_t>(count);
        base = reinterpret_cast<ParamsText*>(cookie + 1);
        std::uint32_t built = 0;
        try { for (; built < static_cast<std::uint32_t>(count); ++built) memory.Construct(base + built); }
        catch (...)
        {
            DestroyPrefix(memory, base, built);
            // 412870未在元素构造失败时回收外层数组块，不追加与原版不同的回滚。
            throw;
        }
    }
    // 元素构造可回调改槽；4128FC只删除当时的control，不重复减计数或释放正文。
    if (slot_.controlAt08) slot_.controlAt08->DeleteControlAtVslot04(1);
    slot_.biasAt04 = 0;
    slot_.baseAt00 = reinterpret_cast<std::uintptr_t>(base);
    void* const storage = memory.Allocate(ParamsTextArrayAllocation::Control, sizeof(Control));
    slot_.controlAt08 = storage ? new (storage) Control(base, static_cast<std::uint32_t>(count), memory) : nullptr;
    // 原空返回随后412931空写；现代明确拒绝，保留base而不伪装为空Ref成功。
    if (!slot_.controlAt08) throw std::bad_alloc();
}

ParamsTextArray::ParamsTextArray(std::initializer_list<ParamsText> values) { AssignSnapshot(values); }
ParamsTextArray::ParamsTextArray(const ParamsTextArray& other) { AssignSnapshot(SnapshotValues(other)); }
ParamsTextArray& ParamsTextArray::operator=(const ParamsTextArray& other)
{
    if (this != &other) AssignSnapshot(SnapshotValues(other));
    return *this;
}
ParamsTextArray& ParamsTextArray::operator=(std::initializer_list<ParamsText> values)
    { AssignSnapshot(values); return *this; }
void ParamsTextArray::AssignSnapshot(std::span<const ParamsText> values)
{
    if (values.size() > static_cast<std::size_t>((std::numeric_limits<std::int32_t>::max)()))
        throw std::length_error("Params text snapshot exceeds signed array count");
    ParamsTextArrayReference copy;
    copy.RebuildAt412870(static_cast<std::int32_t>(values.size()), DefaultParamsTextArrayMemory());
    if (!values.empty()) std::copy(values.begin(), values.end(), copy.Get());
    reference_ = std::move(copy);
}
void ParamsTextArray::RebuildAt412870(std::int32_t count, IParamsTextArrayMemory& memory)
    { reference_.RebuildAt412870(count, memory); }
std::size_t ParamsTextArray::size() const noexcept
    { return reference_.Slot().controlAt08 ? reference_.Slot().controlAt08->countAt10 : 0; }
ParamsText* ParamsTextArray::end() noexcept { auto* base = begin(); return base ? base + size() : nullptr; }
const ParamsText* ParamsTextArray::end() const noexcept { auto* base = begin(); return base ? base + size() : nullptr; }
ParamsText& ParamsTextArray::operator[](std::size_t index)
{
    if (index >= size() || !begin()) throw std::out_of_range("Params text array index has no element");
    return begin()[index];
}
const ParamsText& ParamsTextArray::operator[](std::size_t index) const
{
    if (index >= size() || !begin()) throw std::out_of_range("Params text array index has no element");
    return begin()[index];
}
bool ParamsTextArray::operator==(const ParamsTextArray& other) const
{
    if (size() != other.size()) return false;
    if (empty()) return true;
    if (!begin() || !other.begin()) throw std::logic_error("Params array comparison has no payload");
    return std::equal(begin(), end(), other.begin());
}
}

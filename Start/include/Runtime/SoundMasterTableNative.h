#pragma once
#include <Runtime/SoundParameterNative.h>

#include <bit>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// CSoundParams+20 的唯一存储：12-byte Ref vector，条目正文不随 vector 扩容移动。
// 此 owner 只封装该成员，不冒充尚未接完的 44h 根对象。
class SoundMasterTableNative final
{
public:
    SoundMasterTableNative() noexcept = default;
    ~SoundMasterTableNative() noexcept;
    SoundMasterTableNative(const SoundMasterTableNative&) = delete;
    SoundMasterTableNative& operator=(const SoundMasterTableNative&) = delete;

    template<bool Constant> class Iterator final
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = SoundTypeNative;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<Constant, const SoundTypeNative*, SoundTypeNative*>;
        using reference = std::conditional_t<Constant, const SoundTypeNative&, SoundTypeNative&>;
        explicit Iterator(const YtlReferenceSlotRuntime* slot = nullptr) noexcept : slot_(slot) {}
        reference operator*() const { return *operator->(); }
        pointer operator->() const
        {
            auto* body = ResolveEnvironmentObjectReference<SoundTypeNative>(*slot_);
            if (!body) _invalid_parameter_noinfo_noreturn();
            return body;
        }
        Iterator& operator++() noexcept { ++slot_; return *this; }
        Iterator operator++(int) noexcept { auto previous = *this; ++*this; return previous; }
        bool operator==(const Iterator&) const = default;
    private:
        const YtlReferenceSlotRuntime* slot_;
    };
    auto begin() noexcept { return Iterator<false>(storage_.begin); }
    auto end() noexcept { return Iterator<false>(storage_.end); }
    auto begin() const noexcept { return Iterator<true>(storage_.begin); }
    auto end() const noexcept { return Iterator<true>(storage_.end); }
    [[nodiscard]] std::uint32_t size() const noexcept
    {
        const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(storage_.end) -
            reinterpret_cast<std::uintptr_t>(storage_.begin));
        return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
    }
    [[nodiscard]] bool empty() const noexcept { return storage_.begin == storage_.end; }
    SoundTypeNative& operator[](std::size_t index) { return *Iterator<false>(storage_.begin + index); }
    const SoundTypeNative& operator[](std::size_t index) const { return *Iterator<true>(storage_.begin + index); }
    SoundTypeNative& at(std::size_t index)
    {
        if (index >= size()) throw std::out_of_range("sound master index");
        return (*this)[index];
    }
    const SoundTypeNative& at(std::size_t index) const
    {
        if (index >= size()) throw std::out_of_range("sound master index");
        return (*this)[index];
    }
    SoundTypeNative& front() { return (*this)[0]; }
    SoundTypeNative& back() { return (*this)[size() - 1u]; }
    void clear() noexcept;
    [[nodiscard]] const SoundTypeNative* FindAt47B0C0(const char* name) const;
    void SerializeAt48B750(SerializeBuffer& serializer,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    void AppendReferenceAt47025E(const YtlReferenceSlotRuntime& source,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
    auto& Native() noexcept { return storage_; }
    const auto& Native() const noexcept { return storage_; }
private:
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> storage_{};
};
static_assert(sizeof(SoundMasterTableNative) == 12);
}

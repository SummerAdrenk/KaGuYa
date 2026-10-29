#include "GlyphLayout.h"

#include <algorithm>
#include <bit>
#include <new>
#include <stdexcept>

namespace KaGuYa::Graphics
{
namespace
{
constexpr std::uint32_t MaximumCount = 0x15555555;
std::uint32_t Count(const GlyphReference* first, const GlyphReference* last) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(last) -
        reinterpret_cast<std::uintptr_t>(first));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}
}

std::uint32_t GlyphLayout::size() const noexcept { return Count(begin_, end_); }
std::uint32_t GlyphLayout::Capacity() const noexcept { return Count(begin_, capacity_); }

void GlyphLayout::clear()
{
    // 原05800/F1C0按地址正序退休，全部正常返回后才提交end；容量保留。
    for (auto* slot = begin_; slot != end_; ++slot) slot->RetireOwned();
    end_ = begin_;
}

GlyphLayout::~GlyphLayout() noexcept(false)
{
    clear();
    ::operator delete(begin_);
    begin_ = end_ = capacity_ = nullptr;
}

void GlyphLayout::Reserve(const std::uint32_t requested)
{
    if (requested > MaximumCount) throw std::length_error("vector<T> too long");
    if (requested <= Capacity()) return;
    auto* replacement = static_cast<GlyphReference*>(::operator new(requested * sizeof(GlyphReference)));
    try
    {
        auto* output = replacement;
        for (auto* input = begin_; input != end_; ++input, ++output)
            ::new (output) GlyphReference(*input);
    }
    catch (...) { ::operator delete(replacement); throw; }
    // 原F07A在退旧之前撤去新存储保护；旧引用退休异常不能触发强回滚。
    const auto count = size();
    for (auto* slot = begin_; slot != end_; ++slot) slot->RetireOwned();
    ::operator delete(begin_);
    capacity_ = replacement + requested;
    end_ = replacement + count;
    begin_ = replacement;
}

void GlyphLayout::AppendReference(const GlyphReference& glyph)
{
    const auto source = reinterpret_cast<std::uintptr_t>(&glyph);
    const bool internal = source >= reinterpret_cast<std::uintptr_t>(begin_) &&
        source < reinterpret_cast<std::uintptr_t>(end_);
    const auto index = internal ? Count(begin_, &glyph) : 0;
    if (end_ == capacity_)
    {
        const auto count = size();
        if (count > MaximumCount - 1) throw std::length_error("vector<T> too long");
        const auto oldCapacity = Capacity();
        const auto half = oldCapacity >> 1;
        const auto grown = oldCapacity > MaximumCount - half ? 0u : oldCapacity + half;
        Reserve((std::max)(count + 1, grown));
    }
    ::new (end_) GlyphReference(internal ? begin_[index] : glyph);
    ++end_;
}

void GlyphLayout::Append(const GlyphReference& glyph)
{
    // 原ECD0另持一临时Ref：正常尾用缓存control，异常展开才走05230的槽内重读。
    alignas(GlyphReference) std::byte storage[sizeof(GlyphReference)];
    auto* temporary = ::new (storage) GlyphReference(glyph);
    try { AppendReference(*temporary); }
    catch (...) { temporary->~GlyphReference(); throw; }
    temporary->RetireCachedControl();
}
}

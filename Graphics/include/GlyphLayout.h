#pragma once

#include "NativeObjectReference.h"
#include <Windows.h>
#include <cstddef>
#include <cstdint>

namespace KaGuYa::Graphics
{
struct GlyphRecord
{
    // 原3B77F只初始化WORD；坐标/尺寸在测量路径写入，padding不作为数据。
    GlyphRecord() noexcept : character(0) {}
    std::uint16_t character;
    int x;
    int y;
    SIZE extent;
};
static_assert(sizeof(GlyphRecord) == 0x14);
static_assert(offsetof(GlyphRecord, x) == 4 && offsetof(GlyphRecord, extent) == 0xC);
using GlyphReference = NativeObjectReference<GlyphRecord, 0x14>;
static_assert(sizeof(GlyphReference) == 12);

class GlyphLayout final
{
public:
    GlyphLayout() = default;
    GlyphLayout(const GlyphLayout&) = delete;
    GlyphLayout& operator=(const GlyphLayout&) = delete;
    ~GlyphLayout() noexcept(false);
    std::uint32_t size() const noexcept;
    bool empty() const noexcept { return begin_ == end_; }
    const GlyphRecord& operator[](std::uint32_t index) const noexcept { return *begin_[index].get(); }
    const GlyphRecord& front() const noexcept { return (*this)[0]; }
    const GlyphReference* BeginReferences() const noexcept { return begin_; }
    const GlyphReference* EndReferences() const noexcept { return end_; }
    void clear();
    void Append(const GlyphReference& glyph);
private:
    void Reserve(std::uint32_t requested);
    void AppendReference(const GlyphReference& glyph);
    std::uint32_t Capacity() const noexcept;
    GlyphReference* begin_ = nullptr;
    GlyphReference* end_ = nullptr;
    GlyphReference* capacity_ = nullptr;
};
static_assert(sizeof(GlyphLayout) == 12);
}

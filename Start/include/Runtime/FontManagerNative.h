#pragma once

#include <Runtime/EnvironmentObjectReference.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
class IFontResourcePlatformApi;

// 第一表仅确证control+0C stride和正文+04名称；不猜正文类型/大小。
class FontResourceReferenceControlRuntime : public YtlReferenceControlRuntime
{
public:
    std::uint32_t strideAt0C = 0;
protected:
    ~FontResourceReferenceControlRuntime() = default;
};
struct FontResourceReferenceSlotRuntime final
{
    std::uintptr_t baseAt00 = 0;
    std::uint32_t biasAt04 = 0;
    FontResourceReferenceControlRuntime* controlAt08 = nullptr;
};

template<class Slot>
struct FontReferenceStorage final
{
    Slot* begin;
    Slot* end;
    Slot* capacity;
};
using FontResourceStorage = FontReferenceStorage<FontResourceReferenceSlotRuntime>;
using FontSecondaryStorage = FontReferenceStorage<YtlReferenceSlotRuntime>;

struct FontManagerNativeBody1Ch;
struct FontManagerNativeVtable final
{
    // 5B2B78唯一槽468AE0，不把后一DWORD的RTTI COL当作方法。
    FontManagerNativeBody1Ch* (__thiscall* deleteAt00)(FontManagerNativeBody1Ch*, std::uint32_t);
};

struct FontManagerNativeBody1Ch final
{
    const FontManagerNativeVtable* vptrAt00;
    FontResourceStorage resourcesAt04;
    FontSecondaryStorage secondaryAt10;

    // 非虚现代view，直接借用唯一原头；不缓存size/capacity或数组。
    [[nodiscard]] std::span<FontResourceReferenceSlotRuntime> ResourceReferences() noexcept;
    [[nodiscard]] std::span<YtlReferenceSlotRuntime> SecondaryReferences() noexcept;
};
static_assert(sizeof(FontResourceReferenceSlotRuntime) == 0x0C);
static_assert(sizeof(FontResourceStorage) == 0x0C && sizeof(FontSecondaryStorage) == 0x0C);
static_assert(sizeof(FontManagerNativeBody1Ch) == 0x1C && alignof(FontManagerNativeBody1Ch) == 4);
static_assert(offsetof(FontManagerNativeBody1Ch, vptrAt00) == 0);
static_assert(offsetof(FontManagerNativeBody1Ch, resourcesAt04) == 4);
static_assert(offsetof(FontManagerNativeBody1Ch, secondaryAt10) == 0x10);
static_assert(sizeof(FontManagerNativeVtable) == 4);
static_assert(std::is_trivially_destructible_v<FontManagerNativeBody1Ch>);

// 原依赖是进程RemoveFontResourceExA；测试/宿主接口只借在正文外。
class FontManagerNativeScope final
{
public:
    explicit FontManagerNativeScope(IFontResourcePlatformApi& platform) noexcept;
    ~FontManagerNativeScope() noexcept;
    FontManagerNativeScope(const FontManagerNativeScope&) = delete;
    FontManagerNativeScope& operator=(const FontManagerNativeScope&) = delete;
private:
    IFontResourcePlatformApi* previous_;
};

[[nodiscard]] const FontManagerNativeVtable& FontManagerNativeMethods() noexcept;
FontManagerNativeBody1Ch* ConstructFontManagerAt48ACA9(FontManagerNativeBody1Ch& body) noexcept;
[[nodiscard]] FontManagerNativeBody1Ch* NewFontManagerAt48AC96();
void DestroyFontManagerAt463090(FontManagerNativeBody1Ch& body) noexcept;
FontManagerNativeBody1Ch* DeleteFontManagerAt468AE0(FontManagerNativeBody1Ch& body, std::uint32_t flags) noexcept;

// 捕获的root+60 Ref；新body先构造，旧release失败不回收新body，control失败保留已发布base。
void CreateAndReplaceFontManagerAt48AC96(YtlReferenceSlotRuntime& slot);
// 局部4826A0的state=-1可抛；完整4640E0的EHFlags=5终止，不相互替代。
void ReleaseFontManagerReferenceAt482749(YtlReferenceSlotRuntime& slot);
void ReleaseFontManagerReferenceAt4642B1(YtlReferenceSlotRuntime& slot) noexcept;
[[nodiscard]] FontManagerNativeBody1Ch* ResolveFontManagerReference(const YtlReferenceSlotRuntime& slot) noexcept;

// 原范围释放固定begin/end；资源Remove循环的晚读end在463090正文中独立处理。
void ReleaseFontResourceRangeAt48FA30(FontResourceReferenceSlotRuntime* begin,
    FontResourceReferenceSlotRuntime* end) noexcept;
void DestroyFontResourceStorageAt461C20(FontResourceStorage& storage) noexcept;
void DestroyFontSecondaryStorageAt462140(FontSecondaryStorage& storage) noexcept;

// 以下仅为既有现代Adopt/测试填充口，不声称找到了原注册/选择/增长业务。
// 分配成功才接收引用；存储仍为原头，释放沿原aligned backing规则。
void AdoptFontResourceReferenceModern(FontResourceStorage& storage, FontResourceReferenceSlotRuntime reference);
void AdoptFontSecondaryReferenceModern(FontSecondaryStorage& storage, YtlReferenceSlotRuntime reference);
// 兼容facade的原有Adopt安全边界；标志在正文外，原native析构本身不带此参数。
void DestroyFontManagerModernOwner(FontManagerNativeBody1Ch& body, bool& secondaryStorageDestroyed) noexcept;
}

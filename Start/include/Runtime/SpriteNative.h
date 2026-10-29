#pragma once

#include <Runtime/SpriteNativeStorage.h>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct SpriteNativeBodyA0h;
struct SpriteNativeVtable final
{
    SpriteNativeBodyA0h* (__thiscall* deletingDestructorAt00)(SpriteNativeBodyA0h*, std::uint32_t);
};

// 45D7D0：无服务指针、现代Sprite镜像或隐藏owner。构析由下方原地址入口显式管理。
struct SpriteNativeBodyA0h final
{
    const SpriteNativeVtable* vptrAt00;
    std::uint32_t modeAt04;
    std::uint8_t activeAt08;
    std::uint8_t unwrittenAt09[3];
    union { RuntimeCString logicalNameAt0C; };
    std::uint32_t argumentAt10;
    union { RuntimeCString primaryNameAt14; };
    EnvironmentVectorStorage<RuntimeCString> memberNamesAt18;
    std::int32_t widthAt24;
    std::int32_t heightAt28;
    std::int32_t xAt2C;
    std::int32_t yAt30;
    SpriteAnimationNativeBody54h animationAt34;
    union { YtlReferenceSlotRuntime primarySurfacesAt88; };
    GuiControlSurfaceReferences auxiliarySurfacesAt94;
    SpriteNativeBodyA0h() noexcept {}
    // 不自动销毁union成员；否则显式原析构后会多一轮CString/Ref释放。
    ~SpriteNativeBodyA0h() noexcept {}
    SpriteNativeBodyA0h(const SpriteNativeBodyA0h&) = delete;
    SpriteNativeBodyA0h& operator=(const SpriteNativeBodyA0h&) = delete;
};
static_assert(sizeof(SpriteNativeVtable) == 4);
static_assert(sizeof(SpriteNativeBodyA0h) == 0xA0 && alignof(SpriteNativeBodyA0h) == 4);
static_assert(std::is_standard_layout_v<SpriteNativeBodyA0h>);
static_assert(offsetof(SpriteNativeBodyA0h, modeAt04) == 4);
static_assert(offsetof(SpriteNativeBodyA0h, activeAt08) == 8);
static_assert(offsetof(SpriteNativeBodyA0h, logicalNameAt0C) == 0x0C);
static_assert(offsetof(SpriteNativeBodyA0h, argumentAt10) == 0x10);
static_assert(offsetof(SpriteNativeBodyA0h, primaryNameAt14) == 0x14);
static_assert(offsetof(SpriteNativeBodyA0h, memberNamesAt18) == 0x18);
static_assert(offsetof(SpriteNativeBodyA0h, widthAt24) == 0x24);
static_assert(offsetof(SpriteNativeBodyA0h, heightAt28) == 0x28);
static_assert(offsetof(SpriteNativeBodyA0h, xAt2C) == 0x2C);
static_assert(offsetof(SpriteNativeBodyA0h, yAt30) == 0x30);
static_assert(offsetof(SpriteNativeBodyA0h, animationAt34) == 0x34);
static_assert(offsetof(SpriteNativeBodyA0h, primarySurfacesAt88) == 0x88);
static_assert(offsetof(SpriteNativeBodyA0h, auxiliarySurfacesAt94) == 0x94);

const SpriteNativeVtable& SpriteNativeMethods() noexcept;
SpriteNativeBodyA0h* ConstructSpriteNativeAt45D7D0(SpriteNativeBodyA0h& body,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void ClearSpriteNativeAt43F140(SpriteNativeBodyA0h& body);
void DestroySpriteNativeAt43ECB0(SpriteNativeBodyA0h& body) noexcept;
// flags&2时返回cookie地址，不是首元素；flags&1控制回收。
SpriteNativeBodyA0h* DeleteSpriteNativeAt43EDB0(SpriteNativeBodyA0h* body, std::uint32_t flags) noexcept;

SpriteNativeBodyA0h* NewSpriteNativeAt4813E4(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void InstallSpriteReferenceAt48140F(YtlReferenceSlotRuntime& slot, SpriteNativeBodyA0h* completed);
// 与Pattern45B327同序：即使count为0仍申请4-byte cookie。安装signed<=0不接管该块。
SpriteNativeBodyA0h* NewSpriteArrayAt45B327(std::uint32_t count,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void InstallSpriteArrayReferenceAt45B361(YtlReferenceSlotRuntime& slot,
    SpriteNativeBodyA0h* completed, std::int32_t count);
SpriteNativeBodyA0h* ResolveSpriteReference(const YtlReferenceSlotRuntime& slot) noexcept;
void ReleaseSpriteReference(YtlReferenceSlotRuntime& slot);
void ReleaseSpriteReferenceAt460260(YtlReferenceSlotRuntime& slot) noexcept;
}

#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/GuiDirtyRectNative.h>
#include <Runtime/LayerItemNative.h>

namespace KaGuYa::Start::Runtime
{
class IGraphicsScreenCalls;
struct LayerListNativeBody18h;

struct LayerListNativeNode0Ch final
{
    LayerListNativeNode0Ch* nextAt00;
    LayerListNativeNode0Ch* previousAt04;
    LayerItemNativeBody14h* itemAt08;
};
struct LayerListNativeRegistration8h final
{
    LayerListNativeNode0Ch* sentinelAt00;
    std::uint32_t countAt04;
};

// 5B1F48十槽；注册表持裸层指针，与DrawManager的owning Ref数组不同。
struct LayerListNativeVtable final
{
    void (__thiscall* registerAt00)(LayerListNativeBody18h*, LayerItemNativeBody14h*);
    void (__thiscall* unregisterAt04)(LayerListNativeBody18h*, LayerItemNativeBody14h*);
    void (__thiscall* clearRegisteredAt08)(LayerListNativeBody18h*);
    std::uint8_t (__thiscall* emptyAt0C)(LayerListNativeBody18h*);
    LayerListNativeRegistration8h* (__thiscall* registrationAt10)(LayerListNativeBody18h*);
    GuiDirtyRectNativeBody80h* (__thiscall* dirtyAt14)(LayerListNativeBody18h*);
    void (__thiscall* drawDirtyAt18)(LayerListNativeBody18h*);
    void (__thiscall* compositeAt1C)(LayerListNativeBody18h*, GraphicsSurfaceNativeBody*, const RECT*);
    void (__thiscall* addDirtyAt20)(LayerListNativeBody18h*, const RECT*);
    LayerListNativeBody18h* (__thiscall* deletingDestructorAt24)(LayerListNativeBody18h*, std::uint32_t);
};

struct LayerListNativeBody18h final
{
    const LayerListNativeVtable* vptrAt00;
    LayerListNativeRegistration8h registrationAt04;
    // 440E30在dirty正文申请完成后才初始化Ref；禁止默认成员初始化清毒字节。
    union { YtlReferenceSlotRuntime dirtyAt0C; };
    LayerListNativeBody18h() noexcept {}
};
static_assert(sizeof(LayerListNativeNode0Ch) == 0x0C);
static_assert(offsetof(LayerListNativeNode0Ch, nextAt00) == 0);
static_assert(offsetof(LayerListNativeNode0Ch, previousAt04) == 4);
static_assert(offsetof(LayerListNativeNode0Ch, itemAt08) == 8);
static_assert(sizeof(LayerListNativeRegistration8h) == 8);
static_assert(sizeof(LayerListNativeVtable) == 0x28);
static_assert(offsetof(LayerListNativeVtable, registerAt00) == 0);
static_assert(offsetof(LayerListNativeVtable, unregisterAt04) == 4);
static_assert(offsetof(LayerListNativeVtable, clearRegisteredAt08) == 8);
static_assert(offsetof(LayerListNativeVtable, emptyAt0C) == 0x0C);
static_assert(offsetof(LayerListNativeVtable, registrationAt10) == 0x10);
static_assert(offsetof(LayerListNativeVtable, dirtyAt14) == 0x14);
static_assert(offsetof(LayerListNativeVtable, drawDirtyAt18) == 0x18);
static_assert(offsetof(LayerListNativeVtable, compositeAt1C) == 0x1C);
static_assert(offsetof(LayerListNativeVtable, addDirtyAt20) == 0x20);
static_assert(offsetof(LayerListNativeVtable, deletingDestructorAt24) == 0x24);
static_assert(sizeof(LayerListNativeBody18h) == 0x18 && alignof(LayerListNativeBody18h) == 4);
static_assert(offsetof(LayerListNativeBody18h, registrationAt04) == 4);
static_assert(offsetof(LayerListNativeBody18h, dirtyAt0C) == 0x0C);
static_assert(std::is_standard_layout_v<LayerListNativeBody18h>);
static_assert(std::is_trivially_destructible_v<LayerListNativeBody18h>);

// 分配器/Graphics只在正文外借用；嵌套scope恢复旧源，不另建Graphics或owner。
// 未提供Graphics时非空dirty绘制明确失败；空dirty路径不要求Graphics。
// 分配源须兼容全局sized delete；scope内后续注册沿用同一分配源，正文不记忆source。
class LayerListNativeScope final
{
public:
    explicit LayerListNativeScope(IEnvironmentRootMemory& memory,
        IGraphicsScreenCalls* graphics = nullptr) noexcept;
    ~LayerListNativeScope() noexcept;
    LayerListNativeScope(const LayerListNativeScope&) = delete;
    LayerListNativeScope& operator=(const LayerListNativeScope&) = delete;
private:
    IEnvironmentRootMemory* previousMemory_;
    IGraphicsScreenCalls* previousGraphics_;
};

[[nodiscard]] const LayerListNativeVtable& LayerListNativeMethods() noexcept;
LayerListNativeBody18h* ConstructLayerListNativeAt440E30(
    LayerListNativeBody18h& body, IEnvironmentRootMemory& memory);
[[nodiscard]] LayerListNativeBody18h* CreateLayerListNativeAt440E30(IEnvironmentRootMemory& memory);
LayerListNativeBody18h* DeleteLayerListNativeAt441400(
    LayerListNativeBody18h& body, std::uint32_t flags) noexcept;

// 4516C0先释放旧Ref，再发布完成正文；control失败不补删新正文。
void InstallLayerListReferenceAt4516C0(YtlReferenceSlotRuntime& slot,
    LayerListNativeBody18h* created, IEnvironmentRootMemory& memory);
[[nodiscard]] LayerListNativeBody18h* ResolveLayerListReference(
    const YtlReferenceSlotRuntime& slot) noexcept;
void ReleaseLayerListReference(YtlReferenceSlotRuntime& slot);
void ReleaseLayerListReferenceFinal(YtlReferenceSlotRuntime& slot) noexcept;
}

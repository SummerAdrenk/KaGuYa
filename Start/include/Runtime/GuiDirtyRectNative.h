#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>

#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct GuiDirtyRectNativeBody80h;

// 5B1EE4：九个原 thiscall 槽；Add 的 EAX 没有稳定业务返回合同。
struct GuiDirtyRectNativeVtable final
{
    void (__thiscall* clearAt00)(GuiDirtyRectNativeBody80h*);
    void (__thiscall* coalesceAt04)(GuiDirtyRectNativeBody80h*);
    void (__thiscall* addRectAt08)(GuiDirtyRectNativeBody80h*, const RECT*);
    void (__thiscall* addLTRBAt0C)(GuiDirtyRectNativeBody80h*, LONG, LONG, LONG, LONG);
    void (__thiscall* addXYWHAt10)(GuiDirtyRectNativeBody80h*, LONG, LONG, LONG, LONG);
    void (__thiscall* addXYWHAt14)(GuiDirtyRectNativeBody80h*, LONG, LONG, LONG, LONG);
    std::uint32_t (__thiscall* countAt18)(GuiDirtyRectNativeBody80h*);
    RECT* (__thiscall* rectsAt1C)(GuiDirtyRectNativeBody80h*);
    GuiDirtyRectNativeBody80h* (__thiscall* deletingDestructorAt20)(
        GuiDirtyRectNativeBody80h*, std::uint32_t);
};
static_assert(sizeof(GuiDirtyRectNativeVtable) == 0x24);
static_assert(offsetof(GuiDirtyRectNativeVtable, clearAt00) == 0x00);
static_assert(offsetof(GuiDirtyRectNativeVtable, coalesceAt04) == 0x04);
static_assert(offsetof(GuiDirtyRectNativeVtable, addRectAt08) == 0x08);
static_assert(offsetof(GuiDirtyRectNativeVtable, addLTRBAt0C) == 0x0C);
static_assert(offsetof(GuiDirtyRectNativeVtable, addXYWHAt10) == 0x10);
static_assert(offsetof(GuiDirtyRectNativeVtable, addXYWHAt14) == 0x14);
static_assert(offsetof(GuiDirtyRectNativeVtable, countAt18) == 0x18);
static_assert(offsetof(GuiDirtyRectNativeVtable, rectsAt1C) == 0x1C);
static_assert(offsetof(GuiDirtyRectNativeVtable, deletingDestructorAt20) == 0x20);

struct GuiDirtyRectNativeBody80h final
{
    const GuiDirtyRectNativeVtable* vptrAt00;
    RECT rectsAt04[6];
    std::int32_t areasAt64[6];
    std::uint32_t countAt7C;

    // 4E0D58/5E：只写 vptr/count，两个数组保留分配器原字节。
    GuiDirtyRectNativeBody80h() noexcept;
    void ClearAt4418C0() noexcept;
    void AddAt441680(LONG left, LONG top, LONG right, LONG bottom);
    void CoalesceAt442060();
};
static_assert(std::is_standard_layout_v<GuiDirtyRectNativeBody80h>);
static_assert(sizeof(GuiDirtyRectNativeBody80h) == 0x80);
static_assert(alignof(GuiDirtyRectNativeBody80h) == 4);
static_assert(offsetof(GuiDirtyRectNativeBody80h, vptrAt00) == 0);
static_assert(offsetof(GuiDirtyRectNativeBody80h, rectsAt04) == 4);
static_assert(offsetof(GuiDirtyRectNativeBody80h, areasAt64) == 0x64);
static_assert(offsetof(GuiDirtyRectNativeBody80h, countAt7C) == 0x7C);

// 原两次分配均可注入；释放仍走全局 sized delete，memory 必须提供兼容存储。
// control 分配失败保留已发布裸正文，不在此回滚或另设 owner。
void CreateGuiDirtyRectReferenceAt4E0D42(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 45B227：先申请正文，之后才初始化fresh槽；不释放此前的原始位模式。
void ConstructGuiDirtyRectReferenceAt45B227(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] GuiDirtyRectNativeBody80h* ResolveGuiDirtyRectReference(
    const YtlReferenceSlotRuntime& slot) noexcept;
// 460500 的 EHFlags=5；需要可抛释放的构造阶段使用公共 ReleaseYtlReference。
void ReleaseGuiDirtyRectReferenceAt460500(YtlReferenceSlotRuntime& slot) noexcept;
// 498860接已构造正文；原null control写+04，不能复用旧显式终止保护。
void AdoptGuiDirtyRectReferenceAt498860(YtlReferenceSlotRuntime&,GuiDirtyRectNativeBody80h*,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}

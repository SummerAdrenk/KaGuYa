#pragma once

#include <Runtime/GuiControlChildrenNative.h>
#include <Runtime/GuiControlNative.h>
#include <Runtime/GuiDirtyRectNative.h>
#include <Runtime/GuiInputNative.h>
#include <Runtime/GuiNavigationNative.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct GuiDialogNativeBody5Ch;

// 5BDCB8，共35槽；44/48/4C为原purecall，由实际派生表提供业务。
struct GuiDialogNativeVtable35 final
{
    GuiDirtyRectNativeBody80h* (__thiscall* dirtyAt00)(GuiDialogNativeBody5Ch*);
    GuiDialogNativeBody5Ch* (__thiscall* deletingDestructorAt04)(GuiDialogNativeBody5Ch*, std::uint32_t);
    void (__thiscall* clearChildrenAt08)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* idAt0C)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* appendChildAt10)(GuiDialogNativeBody5Ch*, YtlReferenceSlotRuntime);
    YtlReferenceSlotRuntime* (__thiscall* findChildAt14)(GuiDialogNativeBody5Ch*, YtlReferenceSlotRuntime*, std::int32_t);
    void (__thiscall* setDirtyAt18)(GuiDialogNativeBody5Ch*, YtlReferenceSlotRuntime);
    void (__thiscall* writeStateAt1C)(GuiDialogNativeBody5Ch*, std::int32_t);
    std::int32_t (__thiscall* readStateAt20)(GuiDialogNativeBody5Ch*);
    void (__thiscall* writeEnabledAt24)(GuiDialogNativeBody5Ch*, std::int32_t);
    std::int32_t (__thiscall* readEnabledAt28)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* enabledAt2C)(GuiDialogNativeBody5Ch*);
    void (__thiscall* setActiveAt30)(GuiDialogNativeBody5Ch*, GuiControlNativeBody50h*);
    std::int32_t (__thiscall* upperInputAt34)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* activeInputAt38)(GuiDialogNativeBody5Ch*);
    void (__thiscall* resetInputAt3C)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* priorityInputAt40)(GuiDialogNativeBody5Ch*);
    void (__thiscall* beginAt44)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* pollAt48)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* endAt4C)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* resultAt50)(GuiDialogNativeBody5Ch*);
    void (__thiscall* processInputAt54)(GuiDialogNativeBody5Ch*);
    void (__thiscall* flushDirtyAt58)(GuiDialogNativeBody5Ch*);
    void (__thiscall* compositeAt5C)(GuiDialogNativeBody5Ch*, YtlReferenceSlotRuntime, const RECT*);
    void (__thiscall* drawAt60)(GuiDialogNativeBody5Ch*, const RECT*);
    void (__thiscall* invalidateAt64)(GuiDialogNativeBody5Ch*, RECT);
    void (__thiscall* resetChildrenAt68)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* heldAt6C)(GuiDialogNativeBody5Ch*, std::int32_t);
    std::int32_t (__thiscall* pressedAt70)(GuiDialogNativeBody5Ch*, std::int32_t);
    POINT* (__thiscall* pointerAt74)(GuiDialogNativeBody5Ch*, POINT*);
    std::int32_t (__thiscall* wheelAt78)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* movedAt7C)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* mousePressAt80)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* mouseReleaseAt84)(GuiDialogNativeBody5Ch*);
    void (__thiscall* childChangedAt88)(GuiDialogNativeBody5Ch*, YtlReferenceSlotRuntime);
};

// Popup A4h首成员直接嵌入本正文；没有State、服务指针或第二份容器。
struct GuiDialogNativeBody5Ch final
{
    const GuiDialogNativeVtable35* vptrAt00;
    GuiDialogNativeBody5Ch* parentAt04;
    std::int32_t idAt08;
    GuiControlChildrenStorage childrenAt0C;
    YtlReferenceSlotRuntime inputAt18;
    GuiNavigationReferences navigationAt24;
    std::int32_t inputLatchAt30;
    std::int32_t stateAt34;
    std::int32_t enabledAt38;
    std::int32_t xAt3C;
    std::int32_t yAt40;
    std::int32_t widthAt44;
    std::int32_t heightAt48;
    GuiControlNativeBody50h* activeAt4C;
    YtlReferenceSlotRuntime dirtyAt50;
};
static_assert(sizeof(GuiDialogNativeBody5Ch) == 0x5C);
static_assert(alignof(GuiDialogNativeBody5Ch) == 4);
static_assert(std::is_standard_layout_v<GuiDialogNativeBody5Ch>);
static_assert(std::is_trivially_destructible_v<GuiDialogNativeBody5Ch>);
static_assert(offsetof(GuiDialogNativeBody5Ch, childrenAt0C) == 0x0C);
static_assert(offsetof(GuiDialogNativeBody5Ch, inputAt18) == 0x18);
static_assert(offsetof(GuiDialogNativeBody5Ch, navigationAt24) == 0x24);
static_assert(offsetof(GuiDialogNativeBody5Ch, inputLatchAt30) == 0x30);
static_assert(offsetof(GuiDialogNativeBody5Ch, xAt3C) == 0x3C);
static_assert(offsetof(GuiDialogNativeBody5Ch, activeAt4C) == 0x4C);
static_assert(offsetof(GuiDialogNativeBody5Ch, dirtyAt50) == 0x50);
static_assert(sizeof(GuiDialogNativeVtable35) == 0x8C);
static_assert(offsetof(GuiDialogNativeVtable35, beginAt44) == 0x44);
static_assert(offsetof(GuiDialogNativeVtable35, compositeAt5C) == 0x5C);
static_assert(offsetof(GuiDialogNativeVtable35, pointerAt74) == 0x74);
static_assert(offsetof(GuiDialogNativeVtable35, childChangedAt88) == 0x88);

const GuiDialogNativeVtable35& GuiDialogBaseVtable() noexcept;
GuiDialogNativeBody5Ch* ConstructGuiDialogAt4E0BE0(GuiDialogNativeBody5Ch* body,
    GuiDialogNativeBody5Ch* parent, std::int32_t id);
void DestroyGuiDialogAt4F7AC0(GuiDialogNativeBody5Ch* body) noexcept;
// Popup5199D0直接调用基类函数，不经派生+54槽再次分派。
void ProcessGuiDialogInputAt518C30(GuiDialogNativeBody5Ch& body);

// 每次525FB0的子Draw返回后调用一次；实现承担屏幕singleton/模块提交的现代边界。
// 不缓存矩形副本：原传给InvalidatedRect的是先前选定的dirty数组项地址。
class IGuiDialogNativeScreenSource
{
public:
    virtual ~IGuiDialogNativeScreenSource() = default;
    virtual void InvalidateRectAt52603A(const RECT* rectangle) = 0;
};
class GuiDialogNativeScope final
{
public:
    explicit GuiDialogNativeScope(IGuiDialogNativeScreenSource& source) noexcept;
    explicit GuiDialogNativeScope(IGuiDialogNativeScreenSource* source) noexcept;
    ~GuiDialogNativeScope() noexcept;
    GuiDialogNativeScope(const GuiDialogNativeScope&) = delete;
    GuiDialogNativeScope& operator=(const GuiDialogNativeScope&) = delete;
private:
    IGuiDialogNativeScreenSource* previous_;
};
IGuiDialogNativeScreenSource& RequireGuiDialogNativeScreenSource();
}

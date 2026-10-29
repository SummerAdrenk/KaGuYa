#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

namespace KaGuYa::Start::Runtime
{
struct GuiDialogNavigationMapping final
{
    std::int32_t sourceEventValue = 0;
    std::int32_t targetEventValue = 0;
};
struct GuiNavigationNativeBody;
struct GuiNavigationNativeVtable final
{
    std::int32_t (__thiscall* nextAt00)(GuiNavigationNativeBody*, std::int32_t);
};
// 5BDCA8：唯一业务槽，无虚析构；输入和 pair 表只有这一份存储。
struct GuiNavigationNativeBody final
{
    const GuiNavigationNativeVtable* vptrAt00;
    std::int32_t inputAt04;
    EnvironmentVectorStorage<GuiDialogNavigationMapping> pairsAt08;
};
static_assert(sizeof(GuiNavigationNativeBody) == 0x14);
static_assert(offsetof(GuiNavigationNativeBody, pairsAt08) == 8);
static_assert(sizeof(GuiDialogNavigationMapping) == 8);
using GuiNavigationReferences = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;

// 仅原 ex=0 普通分支；pairs 必须以负 source 终止，不猜测 Ex 类生命周期。
YtlReferenceSlotRuntime* CreateGuiNavigationAt504CE0(YtlReferenceSlotRuntime& output,
    std::int32_t input, const GuiDialogNavigationMapping* pairs);
void AppendGuiNavigationPairAt40A880(GuiNavigationNativeBody& body,
    const GuiDialogNavigationMapping& pair);
GuiNavigationNativeBody& RequireGuiNavigation(const YtlReferenceSlotRuntime& reference);
std::int32_t GuiNavigationPairCount(const GuiNavigationNativeBody& body) noexcept;
std::int32_t GuiNavigationReferenceCount(const GuiNavigationReferences& references) noexcept;
void AppendGuiNavigationReferenceAt4D9D60(GuiNavigationReferences& references,
    const YtlReferenceSlotRuntime& source);
void ClearGuiNavigationReferencesAt4D80B0(GuiNavigationReferences& references) noexcept;
void DestroyGuiNavigationReferencesAt4F6E70(GuiNavigationReferences& references) noexcept;
// 普通工厂临时Ref：构造未成功前不登记清理，成功后由调用者追加/正常释放。
void CreateAndAppendGuiNavigation(GuiNavigationReferences& references,
    std::int32_t input, const GuiDialogNavigationMapping* pairs);
}

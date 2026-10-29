#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
class IGraphicsSurfaceCopyApi;
using GuiControlSurfaceReferences = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;
using GuiControlResourceReferences = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;

// 504908先捕获receiver，new24/Ref建立之后才用同一receiver加载；允许原manager为空。
// 4800A0的native加载、原catch与错误呈现由生产服务实现，不以现代Surface owner兜底。
class IGuiControlResourceLoadServices
{
public:
    virtual ~IGuiControlResourceLoadServices() = default;
    virtual void* CaptureLoadReceiverAt413BE0() = 0;
    virtual bool LoadIntoAt4800A0(void* capturedReceiver, GraphicsSurfaceNativeBody& destination,
        const char* resourceName) = 0;
};

// 4DABB0：无vptr的CElement；三个指针直接拥有Surface Ref12存储。
struct GuiControlResourceNativeBody38h final
{
    std::int32_t slotIndicesAt00[7];
    GuiControlSurfaceReferences surfacesAt1C;
    RECT combinedRectAt28;
};
static_assert(sizeof(GuiControlResourceNativeBody38h) == 0x38);
static_assert(alignof(GuiControlResourceNativeBody38h) == 4);
static_assert(offsetof(GuiControlResourceNativeBody38h, surfacesAt1C) == 0x1C);
static_assert(offsetof(GuiControlResourceNativeBody38h, combinedRectAt28) == 0x28);
static_assert(std::is_trivially_destructible_v<GuiControlResourceNativeBody38h>);
static_assert(!std::is_polymorphic_v<GuiControlResourceNativeBody38h>);
static_assert(sizeof(GuiControlSurfaceReferences) == 0x0C);

void ConstructGuiControlResourceBodyAt4DABB0(GuiControlResourceNativeBody38h& body) noexcept;
GuiControlResourceNativeBody38h* NewGuiControlResourceAt5047DD();
// fresh返回槽：不释放旧内容，先发布base，control失败不回收38h正文。
YtlReferenceSlotRuntime* ConstructGuiControlResourceReferenceAt4D76A0(
    YtlReferenceSlotRuntime& output, GuiControlResourceNativeBody38h* body, std::uint8_t owning);
GuiControlResourceNativeBody38h* ResolveGuiControlResource(const YtlReferenceSlotRuntime& reference) noexcept;
GuiControlResourceNativeBody38h& RequireGuiControlResource(const YtlReferenceSlotRuntime& reference);
void CopyGuiControlResourceReference(YtlReferenceSlotRuntime& output,
    const YtlReferenceSlotRuntime& source) noexcept;
// 非空Surface的删除由调用方GraphicsSurfaceNativeScope提供API；正文/control不缓存服务。
void ResetGuiControlResourceReferenceAt506CA0(YtlReferenceSlotRuntime& reference);
void ReleaseGuiControlResourceReferenceAt4F6380(YtlReferenceSlotRuntime& reference) noexcept;

std::int32_t GuiControlSurfaceReferenceCount(const GuiControlSurfaceReferences& references) noexcept;
std::int32_t GuiControlSurfaceReferenceCapacity(const GuiControlSurfaceReferences& references) noexcept;
// 原增长入口支持中间插入；分配后才读取source，旧范围释放后才发布三指针。
YtlReferenceSlotRuntime* GrowGuiControlSurfaceReferencesAt4540A0(
    GuiControlSurfaceReferences& references, YtlReferenceSlotRuntime* position,
    const YtlReferenceSlotRuntime& source);
void AppendGuiControlSurfaceReference(GuiControlSurfaceReferences& references,
    const YtlReferenceSlotRuntime& source);
// 538350消费按值Surface Ref；slot>=7不追加但仍消费，负slot沿原无界DWORD写入。
std::int32_t AppendGuiControlSurfaceArgumentAt538350(GuiControlResourceNativeBody38h&,
    std::int32_t slot, YtlReferenceSlotRuntime ownedArgument);
void ReleaseGuiControlSurfaceRangeAt43EA10(YtlReferenceSlotRuntime* begin,
    YtlReferenceSlotRuntime* end) noexcept;
void DestroyGuiControlSurfaceReferencesAt43EAA0(GuiControlSurfaceReferences& references) noexcept;

YtlReferenceSlotRuntime* GrowGuiControlResourceReferencesAt4D8ED0(
    GuiControlResourceReferences& references, YtlReferenceSlotRuntime* position,
    const YtlReferenceSlotRuntime& source);
void AppendGuiControlResourceReference(GuiControlResourceReferences& references,
    const YtlReferenceSlotRuntime& source);
// 4FF1F0：消费已增持的原按值参数；正常不清参数位模式，调用方不得再次析构。
// 调用方须覆盖可能的组/Surface释放scope；本入口不另取root或图形服务。
void AppendGuiControlResourceArgumentAt4FF1F0(GuiControlResourceReferences& references,
    YtlReferenceSlotRuntime& ownedArgument);
void ReleaseGuiControlResourceRangeAt4D7DE0(YtlReferenceSlotRuntime* begin,
    YtlReferenceSlotRuntime* end) noexcept;
void DestroyGuiControlResourceReferencesAt4F6B00(GuiControlResourceReferences& references) noexcept;

YtlReferenceSlotRuntime* LoadGuiControlResourceAt5047A0(YtlReferenceSlotRuntime& output,
    const char* const* sevenNames, IGuiControlResourceLoadServices& loader,
    IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// source是固定借用receiver；output是fresh返回槽。control成功后才启动output展开保护。
// 图形依赖由外部调用边界显式提供；本入口不冒充4182F0或5047A0的root/loader链。
YtlReferenceSlotRuntime* CopyGuiControlResourceAt502630(YtlReferenceSlotRuntime& output,
    const GuiControlResourceNativeBody38h& source, IGraphicsSurfaceApi& surfaces,
    IGraphicsSurfaceCopyApi& copy,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}

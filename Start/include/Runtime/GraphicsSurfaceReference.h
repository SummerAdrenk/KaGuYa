#pragma once

#include <Runtime/GraphicsSurfaceNativeBody.h>

namespace KaGuYa::Start::Runtime
{
// 两个Ref的原存储；不含API、现代owner或析构时隐含根访问。
struct RootSavePreviewSlots final
{
    YtlReferenceSlotRuntime thumbnailAt1D8;
    YtlReferenceSlotRuntime captureAt1E4;
};
static_assert(sizeof(RootSavePreviewSlots) == 0x18);

// 外部返回值适配：业务拥有权仍是唯一Ref12；API指针只为现代调用边界提供删除服务。
// 它不是原函数的12-byte返回ABI，原返回槽使用CopyGraphicsSurfaceReferenceAt47D740。
class GraphicsSurfaceReference final
{
public:
    explicit GraphicsSurfaceReference(IGraphicsSurfaceApi& api) noexcept : api_(&api) {}
    GraphicsSurfaceReference(const YtlReferenceSlotRuntime& slot, IGraphicsSurfaceApi& api) noexcept;
    GraphicsSurfaceReference(const GraphicsSurfaceReference& other) noexcept;
    GraphicsSurfaceReference(GraphicsSurfaceReference&& other) noexcept;
    GraphicsSurfaceReference& operator=(const GraphicsSurfaceReference& other);
    GraphicsSurfaceReference& operator=(GraphicsSurfaceReference&& other);
    ~GraphicsSurfaceReference() noexcept;

    void Reset();
    void ConstructAt40C2E0();
    // 仅转交现代适配器所拥有的一次增持；原setter另外执行其按值参数释放。
    [[nodiscard]] YtlReferenceSlotRuntime Detach() noexcept
        { const auto result = slot_; slot_ = {}; return result; }
    [[nodiscard]] GraphicsSurfaceNativeBody* Get() const noexcept;
    [[nodiscard]] GraphicsSurfaceNativeBody* operator->() const noexcept { return Get(); }
    [[nodiscard]] GraphicsSurfaceNativeBody& operator*() const noexcept { return *Get(); }
    [[nodiscard]] explicit operator bool() const noexcept { return slot_.baseAt00 != 0; }
    [[nodiscard]] YtlReferenceSlotRuntime& Slot() noexcept { return slot_; }
    [[nodiscard]] const YtlReferenceSlotRuntime& Slot() const noexcept { return slot_; }
    [[nodiscard]] IGraphicsSurfaceApi& Api() const noexcept { return *api_; }
    [[nodiscard]] bool operator==(const GraphicsSurfaceReference& other) const noexcept
        { return Get() == other.Get(); }
private:
    YtlReferenceSlotRuntime slot_;
    IGraphicsSurfaceApi* api_;
};

// 47D75F..77E：out是未拥有资源的调用者返回槽，复制bias，不主动resolve正文。
void CopyGraphicsSurfaceReferenceAt47D740(YtlReferenceSlotRuntime& out,
    const YtlReferenceSlotRuntime& source) noexcept;

// out为原调用者返回存储；失败展开释放out，正常先增持再释放根槽。
void ConsumeGraphicsSurfaceReferenceAt477C60(YtlReferenceSlotRuntime& out,
    YtlReferenceSlotRuntime& rootSlot);
// 50DCEF..50DDC9：发布和正常尾释放使用缓存字段，异常展开读取temporary当时的字段。
// temporary是未拥有资源、与rootSlot独立的工作槽；本函数构造并消费它，调用者不得再次释放。
// 不Ensure；调用者的NativeScope须覆盖完整调用。正常返回不清temporary位模式。
void TransferGraphicsSurfaceReferenceAt50DCA0(YtlReferenceSlotRuntime& target,
    YtlReferenceSlotRuntime& rootSlot, YtlReferenceSlotRuntime& temporary);
// argument是已经增持/转入的原按值参数存储，入口不会再替调用方增持。
// 正常消费后不清参数位模式；调用者不得再次析构argument。
void AssignGraphicsSurfaceReferenceAt4C1C60(YtlReferenceSlotRuntime& target,
    YtlReferenceSlotRuntime& argument);
void AssignGraphicsSurfaceReferenceAt48CA30(YtlReferenceSlotRuntime& target,
    YtlReferenceSlotRuntime& argument);
// 465870借用source，不消费；旧引用成功释放并清槽后才读取source。
void CopyAssignGraphicsSurfaceReferenceAt465870(YtlReferenceSlotRuntime& target,
    const YtlReferenceSlotRuntime& source);
}

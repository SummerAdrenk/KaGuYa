#include <Runtime/FreeScrollNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class ScrollControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ScrollControl(FreeScrollEntryNativeBody24h* entry) noexcept
        : EnvironmentObjectReferenceControl(entry, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 4C6130：不拥有 Pattern，只析构两个 Surface Ref，然后 sized-delete 24h。
        auto* entry = static_cast<FreeScrollEntryNativeBody24h*>(payloadAt14);
        if (entry)
        {
            DestroyFreeScrollEntryNativeAt4A4E90(*entry);
            ::operator delete(entry, 0x24);
        }
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ScrollControl) == 0x18);

void ReleaseSavedControl(YtlReferenceControlRuntime* control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
}

void ConstructFreeScrollNativeAt4A1559(FreeScrollNativeBody50h& body,
    IGraphicsScreenSizeApi& screenSize)
{
    body.entriesAt00 = {};
    body.axesAt0C = 0;
    body.stepAt10 = 40;
    body.xAt14 = 0;
    body.yAt18 = 0;
    body.viewportAt1C = {};
    body.limitsAt2C = {};
    // +3C/+40 与 +45..47 原构造均未写入；异常时保留以上已写前缀。
    body.draggingAt44 = 0;
    body.lastPointerAt48 = {};
    try
    {
        const SIZE size = screenSize.ReadScreenSizeAt47CC00();
        body.viewportAt1C = {0, 0, size.cx, size.cy};
    }
    catch (...)
    {
        // 4A15D3的state5：59CCE4 -> 4A3890 -> 4A3B60；包括回调重入新增的条目。
        // 只退役entries，不回滚已写字段，也不补做End。
        DestroyFreeScrollNativeAt4A3B60(body);
        throw;
    }
}

void DestroyFreeScrollEntryNativeAt4A4E90(FreeScrollEntryNativeBody24h& body) noexcept
{
    ReleaseYtlReference(body.processedAt18);
    ReleaseYtlReference(body.mainAt0C);
}

void DestroyFreeScrollNativeAt4A3B60(FreeScrollNativeBody50h& body) noexcept
{
    // 4A3B60 与 461750 的 Ref12/32-byte aligned vector 退役算法相同。
    // 只销毁存储，不执行 End、窗口恢复或鼠标状态变更。
    DestroyDrawLayerReferencesAt461750(body.entriesAt00);
}

void AppendFreeScrollEntryNativeAt4AE185(FreeScrollNativeBody50h& body,
    PatternLayerNativeBody80h* pattern, IEnvironmentRootMemory& memory)
{
    void* storage = memory.AllocateBody(0x24);
    auto* entry = storage ? ::new(storage) FreeScrollEntryNativeBody24h : nullptr;
    if (entry)
    {
        entry->unionOriginAt04 = {};
        ::new(&entry->mainAt0C) YtlReferenceSlotRuntime;
        ::new(&entry->processedAt18) YtlReferenceSlotRuntime;
    }
    YtlReferenceSlotRuntime local;
    ScrollControl* savedControl = nullptr;
    if (entry)
    {
        local.baseAt00 = reinterpret_cast<std::uintptr_t>(entry);
        void* controlStorage = memory.AllocateBody(0x18);
        savedControl = controlStorage ? ::new(controlStorage) ScrollControl(entry) : nullptr;
        // 4AE228 首次写 +04；这里没有现代空 control 兜底或已完成正文回滚。
        savedControl->referenceCountAt04 = 1;
        savedControl->owningAt08 = 1;
        savedControl->strideAt0C = 0x24;
        local.controlAt08 = savedControl;
        savedControl->countAt10 = 1;
    }
    try
    {
        entry->patternAt00 = pattern;
        YtlReferenceSlotRuntime argument = local;
        if (argument.controlAt08) ++argument.controlAt08->referenceCountAt04;
        try
        {
            // 499510 与 454580 的增长/插入/旧范围释放/晚发布算法已逐段核对。
            AppendDrawLayerReference(body.entriesAt00, argument, memory);
        }
        catch (...)
        {
            ReleaseDrawLayerRangeAt452440(&argument, &argument + 1);
            throw;
        }
        // state4：增长返回后才重取 argument.control；payload 后仍使用保存指针。
        ReleaseSavedControl(argument.controlAt08);
    }
    catch (...)
    {
        // state2/3 展开读当前槽；state4/5 的释放异常直接 terminate，不重复清理。
        ReleaseDrawLayerRangeAt452440(&local, &local + 1);
        throw;
    }
    ReleaseSavedControl(savedControl);
}
}

#include <Runtime/TaskControllerNative.h>

#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Node = TaskControllerNodeNativeBody14h;
using Item = TaskControllerItemNativeBody10h;
using Slot = YtlReferenceSlotRuntime;

class ItemControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ItemControl(Item* item) noexcept
        : EnvironmentObjectReferenceControl(item, UninitializedFieldsTag{}) {}

    void DestroyPayloadAtVslot00() override
    {
        // 5B3258 -> 492E70：只退 Item 的 10h；taskAt00 是借用指针，不判空或调用任务虚槽。
        ::operator delete(payloadAt14, std::size_t{0x10});
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // PE 5B325C -> 467EF0：先换 5ADD18，派生入口 delete18；
        // PE 5ADD1C -> 412460 的退休入口才 delete14。已核对，可复用此退役叶。
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(ItemControl) == 0x18);
}

Node* NewTaskListNodeNativeAt48F550(Node* next, Node* previous, IEnvironmentRootMemory& memory)
{
    auto* node = ::new (memory.AllocateBody(0x14)) Node;
    // 48F55A..48F576：next 空时两个链接均指自身；只写前 8 字节，不构造 Ref08。
    node->nextAt00 = next ? next : node;
    node->previousAt04 = next ? previous : node;
    return node;
}

Node* CopyTaskListNodeNativeAt4520A0(Node* next, Node* previous, const Slot& source,
    IEnvironmentRootMemory& memory)
{
    auto* node = NewTaskListNodeNativeAt48F550(next, previous, memory);
    // 4520AE..4520C6：分配及链接写完才读取 source，允许来自已有节点的别名。
    ::new (static_cast<void*>(&node->itemAt08)) Slot{
        source.baseAt00, source.biasAt04, source.controlAt08};
    if (auto* control = node->itemAt08.controlAt08) ++control->referenceCountAt04;
    return node;
}

void PublishTaskControllerItemNative(Slot& slot, Item* item, IEnvironmentRootMemory& memory)
{
    // 46A3C3..46A3D8：这是新 local 的发布口，不释放输入槽，不回滚新 Item。
    slot = {};
    if (!item) return;
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(item);
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new (raw) ItemControl(item) : nullptr;
    // 46A404：即使 allocator 返回 null，仍先在地址 4 写 refs=1，不能先发布 control。
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(control) + 4u) = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x10;
    slot.controlAt08 = control; // 46A416：own/stride 已写后才发布。
    control->countAt10 = 1;
}

void ReleaseTaskControllerItemCachedNative(Slot& slot, YtlReferenceControlRuntime* cached) noexcept
{
    // 46A451..46A477：payload 后仍用 esi 缓存，不重读槽，也不清 local。
    // 46BFFD..46C004 的另一路清槽由调用者保留，不混入这段 normal release。
    (void)slot;
    if (cached && --cached->referenceCountAt04 == 0)
    {
        if (cached->owningAt08) cached->DestroyPayloadAtVslot00();
        cached->DeleteControlAtVslot04(1);
    }
    // PE 5E4F30 state1 -> 541E55：正文或控制块异常必须 terminate。
}

void EraseTaskControllerNodeNative(TaskControllerNativeBody18h& controller, Node* node) noexcept
{
    // 46C058..46C067：先断链和减 count；不增加空节点、哨兵或 count0 保护。
    node->previousAt04->nextAt00 = node->nextAt00;
    node->nextAt00->previousAt04 = node->previousAt04;
    --controller.countAt04;
    // 46C06A..46C09D 与通用 Ref 叶同序：payload 后重读 control，清 base/control，保留 bias。
    // PE 5E4EFC state2 -> 541E55：退休异常终止，不回链，不退节点块。
    ReleaseYtlReference(node->itemAt08);
    ::operator delete(static_cast<void*>(node), std::size_t{0x14}); // 46C0A4..46C0AE
}
}

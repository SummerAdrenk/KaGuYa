#include <Runtime/LayerListNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <bit>
#include <cstdlib>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = LayerListNativeBody18h;
using Table = LayerListNativeVtable;
using Node = LayerListNativeNode0Ch;
using Registration = LayerListNativeRegistration8h;
using Item = LayerItemNativeBody14h;

thread_local IEnvironmentRootMemory* currentMemory = nullptr;
thread_local IGraphicsScreenCalls* currentGraphics = nullptr;

IEnvironmentRootMemory& Memory() noexcept
    { return currentMemory ? *currentMemory : DefaultEnvironmentRootMemory(); }

Node* NewNode(IEnvironmentRootMemory& memory, Node* next, Node* previous)
{
    auto* const node = static_cast<Node*>(memory.AllocateBody(0x0C));
    // 4421D0不初始化payload；null分配仍按原入口解引用，不伪造空链表。
    node->nextAt00 = next ? next : node;
    node->previousAt04 = next ? previous : node;
    return node;
}

Registration* __fastcall GetRegistration(Body* body, void*) { return &body->registrationAt04; }
GuiDirtyRectNativeBody80h* __fastcall GetDirty(Body* body, void*)
    { return ResolveGuiDirtyRectReference(body->dirtyAt0C); }

void __fastcall Register(Body* body, void*, Item* item)
{
    auto* const storage = body->vptrAt00->registrationAt10(body);
    auto* const head = storage->sentinelAt00;
    auto* const tail = head->previousAt04;
    auto* const node = NewNode(Memory(), head, tail);
    node->itemAt08 = item;
    // 44162A先分配，441636后读count；长度异常不补回收已完成节点。
    const auto count = storage->countAt04;
    if (0x15555554u - count < 1u) throw std::length_error("list<T> too long");
    storage->countAt04 = count + 1u;
    head->previousAt04 = node;
    tail->nextAt00 = node;
}

void __fastcall Unregister(Body* body, void*, Item* item)
{
    auto* node = body->vptrAt00->registrationAt10(body)->sentinelAt00->nextAt00;
    while (node != body->vptrAt00->registrationAt10(body)->sentinelAt00)
    {
        if (node->itemAt08 == item)
        {
            auto* const storage = body->vptrAt00->registrationAt10(body);
            auto* const next = node->nextAt00;
            node->previousAt04->nextAt00 = next;
            node->nextAt00->previousAt04 = node->previousAt04;
            --storage->countAt04;
            ::operator delete(node, std::size_t{0x0C});
            node = next;
        }
        else node = node->nextAt00;
    }
}

void __fastcall ClearRegistered(Body* body, void*)
{
    auto* const storage = body->vptrAt00->registrationAt10(body);
    auto* const head = storage->sentinelAt00;
    auto* node = head->nextAt00;
    head->nextAt00 = head;
    storage->sentinelAt00->previousAt04 = storage->sentinelAt00;
    storage->countAt04 = 0;
    while (node != storage->sentinelAt00)
    {
        auto* const next = node->nextAt00;
        ::operator delete(node, std::size_t{0x0C});
        node = next;
    }
}

void DestroyRegistration(Registration& storage)
{
    auto* head = storage.sentinelAt00;
    auto* node = head->nextAt00;
    head->nextAt00 = head;
    storage.sentinelAt00->previousAt04 = storage.sentinelAt00;
    storage.countAt04 = 0;
    head = storage.sentinelAt00;
    while (node != head)
    {
        auto* const next = node->nextAt00;
        ::operator delete(node, std::size_t{0x0C});
        head = storage.sentinelAt00;
        node = next;
    }
    // 4412E6保留悬空头字段，不额外清头；最终删除使用回调后读取的当前sentinel。
    ::operator delete(head, std::size_t{0x0C});
}

void UnwindRegistration(Registration& storage) noexcept { DestroyRegistration(storage); }
void UnwindRawBody(Body* body) noexcept { ::operator delete(body, std::size_t{0x18}); }

std::uint8_t __fastcall Empty(Body* body, void*)
    { return body->vptrAt00->registrationAt10(body)->countAt04 == 0 ? 1u : 0u; }

void __fastcall Composite(Body* body, void*, GraphicsSurfaceNativeBody* target, const RECT* clip)
{
    auto* node = body->vptrAt00->registrationAt10(body)->sentinelAt00->nextAt00;
    while (node != body->vptrAt00->registrationAt10(body)->sentinelAt00)
    {
        auto* const item = node->itemAt08;
        if (item->enabledAt0C) item->vptrAt00->compositeAt04(item, target, clip);
        node = node->nextAt00;
    }
}

void __fastcall DrawDirty(Body* body, void*)
{
    auto* dirty = body->vptrAt00->dirtyAt14(body);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = body->vptrAt00->dirtyAt14(body);
    const auto count = dirty->vptrAt00->countAt18(dirty);
    if (!count) return;
    dirty = body->vptrAt00->dirtyAt14(body);
    const RECT* rect = dirty->vptrAt00->rectsAt1C(dirty);
    for (std::int32_t index = 0; index < std::bit_cast<std::int32_t>(count); ++index, ++rect)
    {
        auto* node = body->vptrAt00->registrationAt10(body)->sentinelAt00->nextAt00;
        while (node != body->vptrAt00->registrationAt10(body)->sentinelAt00)
        {
            auto* const item = node->itemAt08;
            if (item->enabledAt0C) item->vptrAt00->drawAt00(item, rect);
            node = node->nextAt00;
        }
        // Graphics依赖只在原InvalidatedRect点读取，不构造另一全局Graphics。
        if (!currentGraphics) throw std::logic_error("native LayerList Graphics source is not installed");
        currentGraphics->InvalidatedRectIfLoaded(rect);
    }
    dirty = body->vptrAt00->dirtyAt14(body);
    dirty->vptrAt00->clearAt00(dirty);
}

void __fastcall AddDirty(Body* body, void*, const RECT* rect)
{
    auto* const dirty = body->vptrAt00->dirtyAt14(body);
    dirty->vptrAt00->addRectAt08(dirty, rect);
}

template<class Result, class... Args>
[[noreturn]] Result __fastcall PureSlot(Body*, void*, Args...) { _purecall(); std::terminate(); }
Body* __fastcall DeleteRetired(Body*, void*, std::uint32_t);
Body* __fastcall DeleteBody(Body* body, void*, std::uint32_t flags)
    { return DeleteLayerListNativeAt441400(*body, flags); }

// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table retired{
    reinterpret_cast<decltype(Table::registerAt00)>(&PureSlot<void, Item*>),
    reinterpret_cast<decltype(Table::unregisterAt04)>(&PureSlot<void, Item*>),
    reinterpret_cast<decltype(Table::clearRegisteredAt08)>(&PureSlot<void>),
    reinterpret_cast<decltype(Table::emptyAt0C)>(&PureSlot<std::uint8_t>),
    reinterpret_cast<decltype(Table::registrationAt10)>(&PureSlot<Registration*>),
    reinterpret_cast<decltype(Table::dirtyAt14)>(&PureSlot<GuiDirtyRectNativeBody80h*>),
    reinterpret_cast<decltype(Table::drawDirtyAt18)>(&PureSlot<void>),
    reinterpret_cast<decltype(Table::compositeAt1C)>(&PureSlot<void, GraphicsSurfaceNativeBody*, const RECT*>),
    reinterpret_cast<decltype(Table::addDirtyAt20)>(&PureSlot<void, const RECT*>),
    reinterpret_cast<decltype(Table::deletingDestructorAt24)>(&DeleteRetired)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{
    reinterpret_cast<decltype(Table::registerAt00)>(&Register),
    reinterpret_cast<decltype(Table::unregisterAt04)>(&Unregister),
    reinterpret_cast<decltype(Table::clearRegisteredAt08)>(&ClearRegistered),
    reinterpret_cast<decltype(Table::emptyAt0C)>(&Empty),
    reinterpret_cast<decltype(Table::registrationAt10)>(&GetRegistration),
    reinterpret_cast<decltype(Table::dirtyAt14)>(&GetDirty),
    reinterpret_cast<decltype(Table::drawDirtyAt18)>(&DrawDirty),
    reinterpret_cast<decltype(Table::compositeAt1C)>(&Composite),
    reinterpret_cast<decltype(Table::addDirtyAt20)>(&AddDirty),
    reinterpret_cast<decltype(Table::deletingDestructorAt24)>(&DeleteBody)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

Body* __fastcall DeleteRetired(Body* body, void*, std::uint32_t flags)
{
    body->vptrAt00 = &retired;
    if (flags & 1u) ::operator delete(body, std::size_t{4});
    return body;
}

class DirtyControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DirtyControl(GuiDirtyRectNativeBody80h* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<GuiDirtyRectNativeBody80h*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt20(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
class ListControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ListControl(Body* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<Body*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt24(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DirtyControl) == 0x18 && sizeof(ListControl) == 0x18);
}

LayerListNativeScope::LayerListNativeScope(IEnvironmentRootMemory& memory,
    IGraphicsScreenCalls* graphics) noexcept
    : previousMemory_(currentMemory), previousGraphics_(currentGraphics)
{
    currentMemory = &memory;
    currentGraphics = graphics;
}
LayerListNativeScope::~LayerListNativeScope() noexcept
{
    currentMemory = previousMemory_;
    currentGraphics = previousGraphics_;
}

const LayerListNativeVtable& LayerListNativeMethods() noexcept { return methods; }

LayerListNativeBody18h* ConstructLayerListNativeAt440E30(Body& body, IEnvironmentRootMemory& memory)
{
    body.vptrAt00 = &methods;
    body.registrationAt04.sentinelAt00 = nullptr;
    body.registrationAt04.countAt04 = 0;
    try
    {
        body.registrationAt04.sentinelAt00 = NewNode(memory, nullptr, nullptr);
        try
        {
            void* storage = memory.AllocateBody(0x80);
            auto* const dirty = storage ? ::new (storage) GuiDirtyRectNativeBody80h : nullptr;
            // 440EB4在80h正文分配完成后才清三字；control异常不清理新dirty正文。
            ::new (&body.dirtyAt0C) YtlReferenceSlotRuntime;
            if (dirty)
            {
                body.dirtyAt0C.baseAt00 = reinterpret_cast<std::uintptr_t>(dirty);
                storage = memory.AllocateBody(0x18);
                auto* const control = storage ? ::new (storage) DirtyControl(dirty) : nullptr;
                body.dirtyAt0C.controlAt08 = control;
                control->referenceCountAt04 = 1;
                body.dirtyAt0C.controlAt08->owningAt08 = 1;
                static_cast<EnvironmentObjectReferenceControl*>(body.dirtyAt0C.controlAt08)->strideAt0C = 0x80;
                static_cast<EnvironmentObjectReferenceControl*>(body.dirtyAt0C.controlAt08)->countAt10 = 1;
            }
        }
        catch (...)
        {
            UnwindRegistration(body.registrationAt04);
            throw;
        }
    }
    catch (...)
    {
        // 5E3EC4：state1清注册容器，state0退为ILayerList；此时没有dirty Ref清理资格。
        body.vptrAt00 = &retired;
        throw;
    }
    return &body;
}

LayerListNativeBody18h* CreateLayerListNativeAt440E30(IEnvironmentRootMemory& memory)
{
    void* const storage = memory.AllocateBody(0x18);
    if (!storage) return nullptr;
    auto* const body = ::new (storage) Body;
    try { return ConstructLayerListNativeAt440E30(*body, memory); }
    catch (...) { UnwindRawBody(body); throw; }
}

LayerListNativeBody18h* DeleteLayerListNativeAt441400(Body& body, std::uint32_t flags) noexcept
{
    // 5E3F00 EHFlags=5；dirty释放后可能被节点删除回调回填，不允许成员自动再扫。
    ReleaseYtlReference(body.dirtyAt0C);
    DestroyRegistration(body.registrationAt04);
    body.vptrAt00 = &retired;
    auto* const result = &body;
    if (flags & 1u) ::operator delete(result, std::size_t{0x18});
    return result;
}

void InstallLayerListReferenceAt4516C0(YtlReferenceSlotRuntime& slot,
    Body* created, IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!created) return;
    // 4516F3已清ECX；451702不是重新读取slot.control，不补第二次删除。
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    void* const storage = memory.AllocateBody(0x18);
    auto* const control = storage ? ::new (storage) ListControl(created) : nullptr;
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x18;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

LayerListNativeBody18h* ResolveLayerListReference(const YtlReferenceSlotRuntime& slot) noexcept
    { return ResolveEnvironmentObjectReference<Body>(slot); }
void ReleaseLayerListReference(YtlReferenceSlotRuntime& slot) { ReleaseYtlReference(slot); }
void ReleaseLayerListReferenceFinal(YtlReferenceSlotRuntime& slot) noexcept { ReleaseYtlReference(slot); }
}

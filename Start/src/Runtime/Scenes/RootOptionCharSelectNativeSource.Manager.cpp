#include "RootOptionCharSelectNativeSource.Internal.h"

#include <new>

namespace KaGuYa::Start::Runtime::RootOptionCharSelectNativeDetail
{
namespace
{
// 原5C0C08两槽：53D6A0/4FB3C0。无虚析构，无scope/allocator指针。
class ManagerControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ManagerControl(GuiClickedEventReferences* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* manager = static_cast<GuiClickedEventReferences*>(payloadAt14);
        if (!manager) return;
        // 4F6D10 flags=5：范围释放/存储删除抛出在该共享叶内终止。
        DestroyGuiClickedEventReferencesAt4F6D10(*manager);
        ::operator delete(manager, std::size_t{0x0C});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ManagerControl) == 0x18);
}

GuiClickedEventReferences* NewManager(IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(0x0C);
    return raw ? ::new(raw) GuiClickedEventReferences{nullptr, nullptr, nullptr} : nullptr;
}

void AdoptManager(YtlReferenceSlotRuntime& destination, GuiClickedEventReferences* owned,
    IEnvironmentRootMemory& memory)
{
    // 4D7B97..4D7BC5与共享release核同序：payload后重读control，不清bias。
    // 无EH表：旧release异常、new18异常均外传，不补owned正文回滚。
    ReleaseYtlReference(destination);
    if (!owned) return;
    destination.biasAt04 = 0;
    destination.baseAt00 = reinterpret_cast<std::uintptr_t>(owned);
    void* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new(raw) ManagerControl(owned) : nullptr;
    destination.controlAt08 = control;
    // 4D7C04即使new返回null也访问+04；不改成假成功或新异常类型。
    control->referenceCountAt04 = 1;
    destination.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(destination.controlAt08)->strideAt0C = 0x0C;
    static_cast<EnvironmentObjectReferenceControl*>(destination.controlAt08)->countAt10 = 1;
}

std::int32_t PollManager(GuiClickedEventReferences* manager, std::int32_t clearMatched)
{
    // 50FDB3/50FDB5捕获一次begin/end；clear回调不能令本轮扫描扩展到新end。
    auto* position = ReadAt<YtlReferenceSlotRuntime*>(manager, 0);
    const auto* end = ReadAt<YtlReferenceSlotRuntime*>(manager, 4);
    for (; position != end; ++position)
    {
        const auto base = ReadAt<std::uintptr_t>(position, 0); // 当前轮的ESI跨三次解析保持。
        const auto flagBody = reinterpret_cast<const void*>(ResolveWithCapturedBase(position, base));
        if (!ReadAt<std::int32_t>(flagBody, 4)) continue; // DWORD flag，不是AL。
        const auto valueBody = reinterpret_cast<const void*>(ResolveWithCapturedBase(position, base));
        const auto value = ReadAt<std::int32_t>(valueBody, 8);
        if (clearMatched != 0)
        {
            auto* clearBody = reinterpret_cast<GuiClickedEventNativeBody*>(ResolveWithCapturedBase(position, base));
            const auto* table = ReadAt<const GuiClickedEventNativeVtable*>(clearBody, 0);
            const auto clear = ReadAt<decltype(GuiClickedEventNativeVtable::clearAt04)>(table, 4);
            clear(clearBody); // 抛出外传；返回值仍是clear之前保存的EDI。
        }
        return value; // 仅首个匹配，不清后续事件，不因value=-1继续扫描。
    }
    return -1;
}
}

#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredControl final : public YtlReferenceControlRuntime
{
public:
    RetiredControl() noexcept : YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    // 原5ADD18的两个虚槽：541C5F purecall、412460 deleting destructor。
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 412472：退役基类再次删除传14h，不再进入派生control的18h入口。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
    std::uint32_t strideAt0C;
    std::uint32_t countAt10;
};
static_assert(sizeof(RetiredControl) == 0x14);
}

void RetireEnvironmentObjectControl(EnvironmentObjectReferenceControl* control, std::uint32_t flags) noexcept
{
    // 49761A/49764A先退化虚表，flags=0也执行；不改其余20字节或再次释放payload。
    // 用合法placement生命周期更换动态类型，不硬写编译器生成的vptr地址。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* raw = reinterpret_cast<std::byte*>(control);
    ::new (static_cast<void*>(control)) RetiredControl;
    std::memcpy(raw + 4, fields.data(), fields.size());
    // 467B72/467C32/467C62：派生入口传18h；不再次析构正文。
    if (flags & 1u) ::operator delete(static_cast<void*>(raw), std::size_t{0x18});
}
}

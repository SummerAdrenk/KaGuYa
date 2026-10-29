#include <Runtime/PopupNativeLifetime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class FactoryControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit FactoryControl(void* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 53D6D0不读正文，也不判null；factory没有vptr或成员初始化。
        ::operator delete(payloadAt14, std::size_t{1});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(FactoryControl) == 0x18);
}
YtlReferenceSlotRuntime* ConstructGuiFactoryReferenceAt4D7860(
    YtlReferenceSlotRuntime& output, void* body, std::uint8_t owning)
{
    output = {};
    if (body)
    {
        output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* allocation = ::operator new(0x18);
        auto* control = allocation ? ::new (allocation) FactoryControl(body) : nullptr;
        output.controlAt08 = control;
        // 原4D78A2为空control写+4；现代声明的终止边界，不返回伪成功。
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        output.controlAt08->owningAt08 = owning;
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->strideAt0C = 1;
        static_cast<EnvironmentObjectReferenceControl*>(output.controlAt08)->countAt10 = 1;
    }
    return &output;
}
void ReleaseGuiFactoryReferenceAt4F6540(YtlReferenceSlotRuntime& slot) noexcept
{ ReleaseYtlReference(slot); }
void FinishGuiFactoryAt4EAB14(YtlReferenceSlotRuntime& slot) noexcept
{
    auto* control = slot.controlAt08;
    if (!control || --control->referenceCountAt04 != 0) return;
    if (control->owningAt08 != 0) control->DestroyPayloadAtVslot00();
    // 4EAB31继续使用EBX；不重读slot，也不清局部三DWORD。
    control->DeleteControlAtVslot04(1);
}
GuiFactoryConstructionLifetime::GuiFactoryConstructionLifetime()
{
    void* body = ::operator new(1);
    ConstructGuiFactoryReferenceAt4D7860(slot_, body, 1);
    // control分配抛出时本守卫尚未构造完成；原生裸正文不由隐藏owner回收。
}
GuiFactoryConstructionLifetime::~GuiFactoryConstructionLifetime() noexcept
{ if (!finished_) ReleaseGuiFactoryReferenceAt4F6540(slot_); }
void GuiFactoryConstructionLifetime::Finish() noexcept
{
    finished_ = true;
    FinishGuiFactoryAt4EAB14(slot_);
}
void DestroyGuiPopupOpaqueStorageAt4F6A90(GuiPopupOpaqueStorage& storage) noexcept
{
    if (!storage.begin) return;
    void* allocation = storage.begin;
    const auto difference = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(storage.capacity) -
        reinterpret_cast<std::uintptr_t>(storage.begin));
    const auto count = std::bit_cast<std::int32_t>(difference) / 12;
    auto bytes = static_cast<std::uint32_t>(count) * 12;
    if (bytes >= 0x1000u)
    {
        std::memcpy(&allocation, reinterpret_cast<const std::byte*>(storage.begin) - 4, sizeof(allocation));
        if (reinterpret_cast<std::uintptr_t>(storage.begin) - reinterpret_cast<std::uintptr_t>(allocation) - 4 > 0x1Fu)
            _invalid_parameter_noinfo_noreturn();
        bytes += 0x23u;
    }
    ::operator delete(allocation, static_cast<std::size_t>(bytes));
    storage = {}; // 4F6AD6起，free正常返回之后才清三指针。
}
}

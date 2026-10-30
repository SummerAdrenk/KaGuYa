#include "MusicDialogNative.Internal.h"
#include <cstdlib>
#include <cstring>

namespace KaGuYa::Start::Runtime::MusicDialogNativeDetail
{
namespace
{
std::int32_t __fastcall Next(GuiNavigationNativeBody* body, void*, std::int32_t event)
{
    auto* pair = Read(body->pairsAt08.begin);
    auto* const end = Read(body->pairsAt08.end);
    for (; pair != end; ++pair) if (Read(pair->sourceEventValue) == event) return Read(pair->targetEventValue);
    return -1;
}
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const GuiNavigationNativeVtable navigationTable{
    reinterpret_cast<decltype(GuiNavigationNativeVtable::nextAt00)>(&Next)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
class DescriptorControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DescriptorControl(void* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { ::operator delete(payloadAt14, std::size_t{0x48}); }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
class NavigationControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit NavigationControl(void* body) noexcept : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<GuiNavigationNativeBody*>(payloadAt14);
        if (!body) return;
        auto* begin = Read(body->pairsAt08.begin);
        if (begin)
        {
            auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Read(body->pairsAt08.capacity)) -
                reinterpret_cast<std::uintptr_t>(begin)) & ~7u;
            void* raw = begin;
            if (bytes >= 0x1000u)
            {
                const auto address = reinterpret_cast<std::uintptr_t>(begin);
                std::memcpy(&raw, reinterpret_cast<const void*>(address - 4u), sizeof(raw));
                bytes += 0x23u;
                if (address - reinterpret_cast<std::uintptr_t>(raw) - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
            }
            ::operator delete(raw, bytes);
            body->pairsAt08 = {};
        }
        // 原53D6F0明确非虚调用：Ex分配24h，删除仍按14h基类尺寸。
        ::operator delete(body, std::size_t{0x14});
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(DescriptorControl) == 0x18);
static_assert(sizeof(NavigationControl) == 0x18);
}
void CreateDescriptorReference(Slot& output)
{
    auto& s = RequireMusicDialogNativeServices();
    void* raw = s.memory.AllocateBody(0x48);
    output = {};
    if (raw)
    {
        output.baseAt00 = reinterpret_cast<std::uintptr_t>(raw);
        void* controlRaw = s.memory.AllocateBody(0x18);
        auto* control = controlRaw ? ::new(controlRaw) DescriptorControl(raw) : nullptr;
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 0x48;
        output.controlAt08 = control;
        control->countAt10 = 1;
    }
}
void CreateNavigationExAt504CE0(Slot& output, std::int32_t input, const Pair* pairs)
{
    auto& s = RequireMusicDialogNativeServices();
    auto* body = static_cast<NavigationEx24h*>(s.memory.AllocateBody(0x24));
    if (body)
    {
        body->common.pairsAt08 = {};
        body->common.vptrAt00 = &navigationTable;
        body->countdownAt14 = 0;
        body->repeatedAt18 = 0;
        body->firstDelayAt1C = -1;
        // 此时+04与+20仍保持原分配字节。
    }
    output = {};
    if (body)
    {
        output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* controlRaw = s.memory.AllocateBody(0x18);
        auto* control = controlRaw ? ::new(controlRaw) NavigationControl(body) : nullptr;
        output.controlAt08 = control;
        control->referenceCountAt04 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(Read(output.controlAt08))->owningAt08 = 1;
        static_cast<EnvironmentObjectReferenceControl*>(Read(output.controlAt08))->strideAt0C = 0x14;
        static_cast<EnvironmentObjectReferenceControl*>(Read(output.controlAt08))->countAt10 = 1;
    }
    RefLife resultLife{output}; // 504E00在此才为输出取得展开资格。
    Resolve<GuiNavigationNativeBody>(output)->inputAt04 = input;
    for (auto* current = pairs; Read(current->sourceEventValue) >= 0; ++current)
    {
        Pair pair{Read(current->sourceEventValue), Read(current->targetEventValue)};
        AppendGuiNavigationPairAt40A880(*Resolve<GuiNavigationNativeBody>(output), pair);
    }
    resultLife.active = false;
}
}

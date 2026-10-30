#include <Runtime/GuiInputNative.h>

#include <cstdlib>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local IGuiInputNativeSource* activeSource = nullptr;

__declspec(noinline) IGuiInputNativeReceiver& CaptureInput()
{
    auto* const source = activeSource;
    if (!source) throw std::logic_error("GameInputState requires its external GuiInputNativeScope");
    return source->InputAt418210();
}

std::int32_t __fastcall NullAction(const GuiInputNativeBody*, void*, std::int32_t) { return 0; }
std::int32_t __fastcall NullScalar(const GuiInputNativeBody*, void*) { return 0; }
POINT* __fastcall NullPointer(const GuiInputNativeBody*, void*, POINT* output)
{
    output->x = 0;
    output->y = 0;
    return output;
}

std::int32_t __fastcall Held(const GuiInputNativeBody*, void*, std::int32_t action)
{
    if (action == -1) return 0;
    const auto& keyboard = CaptureInput().KeyboardAt18();
    for (std::size_t index = 0; index < keyboard.BindingCount(action); ++index)
        if (keyboard.CurrentState(action, index) != 0) return 1;
    return 0;
}

std::int32_t __fastcall Pressed(const GuiInputNativeBody*, void*, std::int32_t action)
{
    if (action == -1) return 0;
    const auto& keyboard = CaptureInput().KeyboardAt18();
    for (std::size_t index = 0; index < keyboard.BindingCount(action); ++index)
        if (keyboard.CurrentState(action, index) != 0 && keyboard.PreviousState(action, index) == 0)
            return 1;
    return 0;
}

POINT* __fastcall Pointer(const GuiInputNativeBody*, void*, POINT* output)
{
    const auto& mouse = CaptureInput().MouseAt00();
    // 50A68A..50A692按步写输出；第二次读取失败不回滚已写的x。
    output->x = mouse.XAt18();
    output->y = mouse.YAt1C();
    return output;
}

std::int32_t __fastcall Wheel(const GuiInputNativeBody*, void*)
{
    const auto& mouse = CaptureInput().MouseAt00();
    return mouse.SecondaryAt2C() != 0 ? 0 : mouse.WheelAt34();
}

std::int32_t __fastcall Moved(const GuiInputNativeBody*, void*)
{
    return CaptureInput().MouseAt00().MovedAt33() != 0 ? 1 : 0;
}

std::int32_t __fastcall MousePress(const GuiInputNativeBody*, void*)
{
    const auto& mouse = CaptureInput().MouseAt00();
    return mouse.PreviousLeftAt30() == 0 && mouse.CurrentLeftAt10() != 0 &&
        mouse.SuppressionAt28() == 0 ? 1 : 0;
}

std::int32_t __fastcall MouseRelease(const GuiInputNativeBody*, void*)
{
    const auto& mouse = CaptureInput().MouseAt00();
    return mouse.CurrentLeftAt10() == 0 || mouse.SuppressionAt28() != 0 ? 1 : 0;
}

// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const GuiInputNativeVtable nullMethods{
    reinterpret_cast<decltype(GuiInputNativeVtable::readHeldAt00)>(&NullAction),
    reinterpret_cast<decltype(GuiInputNativeVtable::readPressedAt04)>(&NullAction),
    reinterpret_cast<decltype(GuiInputNativeVtable::readPointerAt08)>(&NullPointer),
    reinterpret_cast<decltype(GuiInputNativeVtable::readWheelAt0C)>(&NullScalar),
    reinterpret_cast<decltype(GuiInputNativeVtable::readPointerMovedAt10)>(&NullScalar),
    reinterpret_cast<decltype(GuiInputNativeVtable::readMousePressAt14)>(&NullScalar),
    reinterpret_cast<decltype(GuiInputNativeVtable::readMouseReleaseAt18)>(&NullScalar)};
const GuiInputNativeVtable gameMethods{
    reinterpret_cast<decltype(GuiInputNativeVtable::readHeldAt00)>(&Held),
    reinterpret_cast<decltype(GuiInputNativeVtable::readPressedAt04)>(&Pressed),
    reinterpret_cast<decltype(GuiInputNativeVtable::readPointerAt08)>(&Pointer),
    reinterpret_cast<decltype(GuiInputNativeVtable::readWheelAt0C)>(&Wheel),
    reinterpret_cast<decltype(GuiInputNativeVtable::readPointerMovedAt10)>(&Moved),
    reinterpret_cast<decltype(GuiInputNativeVtable::readMousePressAt14)>(&MousePress),
    reinterpret_cast<decltype(GuiInputNativeVtable::readMouseReleaseAt18)>(&MouseRelease)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// Null初装与GuiInputState adoption保持独立control类型/虚表；正文均无虚析构。
template<bool NullInput>
class InputControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit InputControl(GuiInputNativeBody* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { ::operator delete(payloadAt14, std::size_t{4}); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(InputControl<true>) == 0x18);
static_assert(sizeof(InputControl<false>) == 0x18);

template<bool NullInput>
void Adopt(YtlReferenceSlotRuntime& slot, GuiInputNativeBody* body)
{
    ReleaseYtlReference(slot);
    if (!body) return;
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* raw = ::operator new(0x18);
    auto* control = raw ? ::new(raw) InputControl<NullInput>(body) : nullptr;
    slot.controlAt08 = control;
    // 原536F16空control写入故障；明确终止，不补偿删除已发布的正文。
    if (!control) _invalid_parameter_noinfo_noreturn();
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 4;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
}

__declspec(noinline) GuiInputNativeScope::GuiInputNativeScope(IGuiInputNativeSource& source) noexcept
    : GuiInputNativeScope(&source) {}
__declspec(noinline) GuiInputNativeScope::GuiInputNativeScope(IGuiInputNativeSource* source) noexcept
    : previous_(activeSource) { activeSource = source; }
__declspec(noinline) GuiInputNativeScope::~GuiInputNativeScope() noexcept { activeSource = previous_; }

const GuiInputNativeVtable& NullGuiInputVtable() noexcept { return nullMethods; }
const GuiInputNativeVtable& GameGuiInputVtable() noexcept { return gameMethods; }

GuiInputNativeBody* NewNullGuiInputAt4E0C93()
{
    void* raw = ::operator new(4);
    return raw ? ::new(raw) GuiInputNativeBody{&nullMethods} : nullptr;
}

GuiInputNativeBody* NewGameGuiInputAt4EAAED()
{
    void* raw = ::operator new(4);
    return raw ? ::new(raw) GuiInputNativeBody{&gameMethods} : nullptr;
}

void ConstructNullGuiInputReferenceAt4E0BE0(YtlReferenceSlotRuntime& out)
{
    out = {};
    auto* const body = NewNullGuiInputAt4E0C93();
    Adopt<true>(out, body);
}

void AdoptGuiInputAt536EA0(YtlReferenceSlotRuntime& slot, GuiInputNativeBody* owned)
{
    Adopt<false>(slot, owned);
}

void ReleaseGuiInputReferenceAt4F6620(YtlReferenceSlotRuntime& slot) noexcept
{
    ReleaseYtlReference(slot);
}
}

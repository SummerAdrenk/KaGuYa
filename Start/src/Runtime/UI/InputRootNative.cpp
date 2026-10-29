#include <Runtime/InputRootNative.h>
#include <Runtime/InputNativeObjects.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
InputRootOwner::InputRootOwner(IEnvironmentRootLock& lock, IEnvironmentRootMemory& memory) noexcept
    : lock_(lock), memory_(memory) {}
InputRootOwner::InputRootOwner(IEnvironmentRootLock& lock,IEnvironmentRootMemory& memory,InputNativeServices& services) noexcept
    :lock_(lock),memory_(memory),services_(&services){}
InputRootOwner::~InputRootOwner() noexcept { ReleaseAt5AB1A0(); }
void InputRootOwner::LeaveNoexcept() noexcept { lock_.Leave(); }

InputRootNativeBody24h* InputRootOwner::CaptureAt418210()
{
    lock_.Enter();
    struct Unwind final
    {
        IEnvironmentRootLock& lock;
        bool active = true;
        ~Unwind() noexcept { if (active) lock.Leave(); }
    } cleanup{lock_};
    if (!published_)
    {
        void* raw = memory_.AllocateBody(0x24);
        // 418277..4182AE只清三个Ref共九DWORD；没有子对象初始化/失败补偿。
        published_ = raw ? ::new(raw) InputRootNativeBody24h : nullptr;
    }
    cleanup.active = false;
    LeaveNoexcept();
    return published_; // 4182D2解锁回调后重读，不返回锁内快照。
}

void InputRootOwner::ReleaseAt5AB1A0() noexcept
{
    if(services_){InputNativeScope scope(*services_);ReleaseBodyAt5AB1A0();}
    else ReleaseBodyAt5AB1A0();
}

void InputRootOwner::ReleaseBodyAt5AB1A0() noexcept
{
    lock_.Enter();
    if (auto* const body = published_)
    {
        ReleaseYtlReference(body->keyboardAt18);
        ReleaseYtlReference(body->cursorAt0C);
        ReleaseYtlReference(body->mouseAt00);
        memory_.DeleteBody(body, 0x24);
        published_ = nullptr;
    }
    LeaveNoexcept();
}

void ReleaseInputMembersAt451150(InputRootNativeBody24h& body)
{
    ReleaseYtlReference(body.mouseAt00);
    ReleaseYtlReference(body.cursorAt0C);
    ReleaseYtlReference(body.keyboardAt18);
}

bool ReadKeyboardPressedAt47F4E0(const KeyboardManagerNativeBody438h& body, std::int32_t action)
{
    // 原LEA按DWORD直接定位描述符；没有actionCount/负数/容器容量guard。
    const auto address = reinterpret_cast<std::uintptr_t>(body.actionsAt14.data()) +
        static_cast<std::uint32_t>(action) * 12u;
    const auto& descriptor = *reinterpret_cast<const EnvironmentVectorStorage<KeyboardBindingNative3>*>(address);
    auto at = reinterpret_cast<std::uintptr_t>(descriptor.begin);
    if (at == reinterpret_cast<std::uintptr_t>(descriptor.end)) return false;
    const auto buffer = body.bufferAt10;
    auto previous = at - buffer + 2u;
    do
    {
        if (*reinterpret_cast<const std::uint8_t*>(at + buffer + 1u) != 0 &&
            *reinterpret_cast<const std::uint8_t*>(previous) == 0) return true;
        at += 3u;
        previous += 3u;
    } while (at != reinterpret_cast<std::uintptr_t>(descriptor.end));
    return false;
}

bool ReadKeyboardSixHeldAt4AA573(const KeyboardManagerNativeBody438h& body)
{
    const auto& descriptor = body.actionsAt14[6];
    auto at = reinterpret_cast<std::uintptr_t>(descriptor.begin);
    const auto end = reinterpret_cast<std::uintptr_t>(descriptor.end);
    if (at == end) return false;
    const auto buffer = body.bufferAt10;
    do
    {
        if (*reinterpret_cast<const std::uint8_t*>(at + buffer + 1u) != 0) return true;
        at += 3u;
    } while (at != end);
    return false;
}

bool ReadKeyboardHeldAt4198D0(const KeyboardManagerNativeBody438h& body, std::int32_t action)
{
    const auto address = reinterpret_cast<std::uintptr_t>(&body) + 0x14U + std::uint32_t(action) * 12U;
    const volatile auto& descriptor = *reinterpret_cast<const EnvironmentVectorStorage<KeyboardBindingNative3>*>(address);
    auto at = reinterpret_cast<std::uintptr_t>(descriptor.begin);
    if (at == reinterpret_cast<std::uintptr_t>(descriptor.end)) return false;
    const auto buffer = body.bufferAt10;
    do
    {
        if (*reinterpret_cast<const std::uint8_t*>(at + buffer + 1U)) return true;
        at += 3U;
    } while (at != reinterpret_cast<std::uintptr_t>(descriptor.end));
    return false;
}
bool ReadMouseSelectorAt4B0630(InputRootOwner& input, std::uint32_t selector)
{
    std::uint32_t index;
    switch (selector)
    {
    case 1: index = 1; break;
    case 2: index = 0; break;
    case 3: index = 2; break;
    default: return false; // 非法selector不捕获输入根。
    }
    auto* const root = input.CaptureAt418210();
    auto* const mouse = ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(root->mouseAt00);
    return mouse->latchedAt13[index] != 0;
}
}

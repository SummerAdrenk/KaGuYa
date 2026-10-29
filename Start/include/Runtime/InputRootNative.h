#pragma once

#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
struct InputNativeServices;
// 418266/6198B4：三个原Ref，查询不预取其它子槽，也不含现代InputAggregate。
struct InputRootNativeBody24h final
{
    YtlReferenceSlotRuntime mouseAt00;
    YtlReferenceSlotRuntime cursorAt0C;
    YtlReferenceSlotRuntime keyboardAt18;
};
static_assert(sizeof(InputRootNativeBody24h) == 0x24);
static_assert(offsetof(InputRootNativeBody24h, keyboardAt18) == 0x18);

// 44FE50/450810：仅描述已确认的正文；构造、帧刷新与hook拥有另行实现。
struct MouseManagerNativeBody3Ch final
{
    const void* vptrAt00;
    YtlReferenceSlotRuntime rawAt04;
    std::array<std::uint8_t, 3> pressedAt10;
    std::array<std::uint8_t, 3> latchedAt13;
    std::array<std::uint8_t, 2> unwrittenAt16;
    POINT logicalAt18;
    POINT clientAt20;
    std::uint32_t suppressionAt28;
    std::uint32_t secondaryAt2C;
    std::array<std::uint8_t, 3> previousAt30;
    std::uint8_t movedAt33;
    std::int32_t wheelAt34;
    std::array<std::uint8_t, 4> unwrittenAt38;
};
static_assert(sizeof(MouseManagerNativeBody3Ch) == 0x3C);
static_assert(offsetof(MouseManagerNativeBody3Ch, pressedAt10) == 0x10);
static_assert(offsetof(MouseManagerNativeBody3Ch, latchedAt13) == 0x13);
static_assert(offsetof(MouseManagerNativeBody3Ch, logicalAt18) == 0x18);
static_assert(offsetof(MouseManagerNativeBody3Ch, suppressionAt28) == 0x28);
static_assert(offsetof(MouseManagerNativeBody3Ch, secondaryAt2C) == 0x2C);
static_assert(offsetof(MouseManagerNativeBody3Ch, previousAt30) == 0x30);
static_assert(offsetof(MouseManagerNativeBody3Ch, movedAt33) == 0x33);
static_assert(offsetof(MouseManagerNativeBody3Ch, wheelAt34) == 0x34);

struct KeyboardBindingNative3 final
{
    std::uint8_t key;
    std::array<std::uint8_t, 2> states;
};
static_assert(sizeof(KeyboardBindingNative3) == 3);
// 44FCC2固定58h个描述符，+10是0/1双缓冲选择，不是数据指针。
struct KeyboardManagerNativeBody438h final
{
    const void* vptrAt00;
    YtlReferenceSlotRuntime rawAt04;
    std::uint32_t bufferAt10;
    std::array<EnvironmentVectorStorage<KeyboardBindingNative3>, 0x58> actionsAt14;
    std::int32_t actionCountAt434;
};
static_assert(sizeof(KeyboardManagerNativeBody438h) == 0x438);
static_assert(offsetof(KeyboardManagerNativeBody438h, bufferAt10) == 0x10);
static_assert(offsetof(KeyboardManagerNativeBody438h, actionsAt14) == 0x14);
static_assert(offsetof(KeyboardManagerNativeBody438h, actionCountAt434) == 0x434);

// 与4507E0装载分开；451150正序释放，singleton退出5AB1A0反序释放。
void ReleaseInputMembersAt451150(InputRootNativeBody24h&);
class InputRootOwner final
{
public:
    InputRootOwner(IEnvironmentRootLock&, IEnvironmentRootMemory& = DefaultEnvironmentRootMemory()) noexcept;
    InputRootOwner(IEnvironmentRootLock&,IEnvironmentRootMemory&,InputNativeServices&) noexcept;
    ~InputRootOwner() noexcept;
    InputRootOwner(const InputRootOwner&) = delete;
    InputRootOwner& operator=(const InputRootOwner&) = delete;
    [[nodiscard]] InputRootNativeBody24h* CaptureAt418210();
    void ReleaseAt5AB1A0() noexcept;
    [[nodiscard]] InputRootNativeBody24h* Published() const noexcept { return published_; }
    [[nodiscard]] InputRootNativeBody24h*& PublicationSlot() noexcept { return published_; }
private:
    void ReleaseBodyAt5AB1A0() noexcept;
    void LeaveNoexcept() noexcept;
    IEnvironmentRootLock& lock_;
    IEnvironmentRootMemory& memory_;
    InputRootNativeBody24h* published_ = nullptr;
    // 来源只在owner外观中借用；不进入原24h正文，也不拥有hook或平台对象。
    InputNativeServices* services_ = nullptr;
};

[[nodiscard]] bool ReadKeyboardPressedAt47F4E0(const KeyboardManagerNativeBody438h&, std::int32_t action);
[[nodiscard]] bool ReadKeyboardSixHeldAt4AA573(const KeyboardManagerNativeBody438h&);
// 4198D0与内联4AA573区别在于逐项回读descriptor.end；不扩大数组合法性检查。
[[nodiscard]] bool ReadKeyboardHeldAt4198D0(const KeyboardManagerNativeBody438h&, std::int32_t action);
[[nodiscard]] bool ReadMouseSelectorAt4B0630(InputRootOwner&, std::uint32_t selector);
}

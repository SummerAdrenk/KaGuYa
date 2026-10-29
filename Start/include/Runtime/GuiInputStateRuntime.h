#pragma once
#include <Platform/InputViews.h>

#include <Platform/Input.h>

namespace KaGuYa::Start::Runtime
{
// 原 GuiInputState 的七个业务槽；C++ 析构槽不属于原 x86 虚表布局。
class IGuiInputStateRuntime
{
public:
    virtual ~IGuiInputStateRuntime() = default;
    virtual bool ReadHeldAtVslot00(std::int32_t action) const = 0;
    virtual bool ReadPressedAtVslot04(std::int32_t action) const = 0;
    virtual POINT ReadPointerAtVslot08() const = 0;
    virtual std::int32_t ReadWheelAtVslot0C() const = 0;
    virtual bool ReadPointerMovedAtVslot10() const = 0;
    virtual bool ReadMousePressAtVslot14() const = 0;
    virtual bool ReadMouseReleaseAtVslot18() const = 0;
};

// 4E0BE0 创建的 5BDC88 虚表，七项明确返回零；不是错误时启用的兜底。
class NullGuiInputStateRuntime final : public IGuiInputStateRuntime
{
public:
    bool ReadHeldAtVslot00(std::int32_t) const override { return false; }
    bool ReadPressedAtVslot04(std::int32_t) const override { return false; }
    POINT ReadPointerAtVslot08() const override { return {}; }
    std::int32_t ReadWheelAtVslot0C() const override { return 0; }
    bool ReadPointerMovedAtVslot10() const override { return false; }
    bool ReadMousePressAtVslot14() const override { return false; }
    bool ReadMouseReleaseAtVslot18() const override { return false; }
};

// 4F4AD6 的 5C0C20 虚表。借用根输入 owner 的对象，每次读取实时状态，
// 不复制帧快照；根输入 owner 必须比所有使用它的对话框长寿。
class GameGuiInputStateRuntime final : public IGuiInputStateRuntime
{
public:
    GameGuiInputStateRuntime(Platform::MouseView mouse,
        Platform::KeyboardView keyboard) noexcept;
    explicit GameGuiInputStateRuntime(const Platform::IInputRuntime& input) noexcept;
    bool ReadHeldAtVslot00(std::int32_t action) const override;
    bool ReadPressedAtVslot04(std::int32_t action) const override;
    POINT ReadPointerAtVslot08() const override;
    std::int32_t ReadWheelAtVslot0C() const override;
    bool ReadPointerMovedAtVslot10() const override;
    bool ReadMousePressAtVslot14() const override;
    bool ReadMouseReleaseAtVslot18() const override;

private:
    static std::size_t CheckedAction(std::int32_t action, Platform::KeyboardView keyboard);
    Platform::MouseView Mouse() const;
    Platform::KeyboardView Keyboard() const;
    const Platform::IInputRuntime* input_ = nullptr;
    Platform::MouseView mouse_ = nullptr;
    Platform::KeyboardView keyboard_ = nullptr;
};
}

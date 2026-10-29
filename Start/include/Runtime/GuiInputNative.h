#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Windows.h>
#include <Platform/InputViews.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
// 以下是正文外的输入源边界，不是原24h singleton/Mouse/Keyboard的ABI。
class IGuiInputNativeMouse
{
public:
    virtual ~IGuiInputNativeMouse() = default;
    virtual std::uint8_t CurrentLeftAt10() const = 0;
    virtual std::int32_t XAt18() const = 0;
    virtual std::int32_t YAt1C() const = 0;
    virtual std::uint32_t SuppressionAt28() const = 0;
    virtual std::uint32_t SecondaryAt2C() const = 0;
    virtual std::uint8_t PreviousLeftAt30() const = 0;
    virtual std::uint8_t MovedAt33() const = 0;
    virtual std::int32_t WheelAt34() const = 0;
};

class IGuiInputNativeKeyboard
{
public:
    virtual ~IGuiInputNativeKeyboard() = default;
    virtual std::size_t BindingCount(std::int32_t action) const = 0;
    virtual std::uint8_t CurrentState(std::int32_t action, std::size_t binding) const = 0;
    virtual std::uint8_t PreviousState(std::int32_t action, std::size_t binding) const = 0;
};

// 旧受控接口与真实view都按值固定一次选中的receiver；不缓存共享selected对象。
class GuiInputMouseBorrow final
{
public:
    GuiInputMouseBorrow(const IGuiInputNativeMouse& source) noexcept:kind_(Kind::Interface),source_(&source){}
    GuiInputMouseBorrow(Platform::MouseView view) noexcept:kind_(Kind::View),view_(view){}
    std::uint8_t CurrentLeftAt10() const{return kind_==Kind::Interface?source_->CurrentLeftAt10():view_.Pressed(0);}
    std::int32_t XAt18() const{return kind_==Kind::Interface?source_->XAt18():view_.XAt18();}
    std::int32_t YAt1C() const{return kind_==Kind::Interface?source_->YAt1C():view_.YAt1C();}
    std::uint32_t SuppressionAt28() const{return kind_==Kind::Interface?source_->SuppressionAt28():view_.ReleaseSuppressionCountdown();}
    std::uint32_t SecondaryAt2C() const{return kind_==Kind::Interface?source_->SecondaryAt2C():view_.SecondaryCountdown();}
    std::uint8_t PreviousLeftAt30() const{return kind_==Kind::Interface?source_->PreviousLeftAt30():view_.PreviousPressed(0);}
    std::uint8_t MovedAt33() const{return kind_==Kind::Interface?source_->MovedAt33():view_.Position().LogicalPositionChanged();}
    std::int32_t WheelAt34() const{return kind_==Kind::Interface?source_->WheelAt34():view_.WheelSteps();}
private:
    enum class Kind{Interface,View};Kind kind_;
    const IGuiInputNativeMouse* source_=nullptr;
    Platform::MouseView view_;
};
class GuiInputKeyboardBorrow final
{
public:
    GuiInputKeyboardBorrow(const IGuiInputNativeKeyboard& source) noexcept:kind_(Kind::Interface),source_(&source){}
    GuiInputKeyboardBorrow(Platform::KeyboardView view) noexcept:kind_(Kind::View),view_(view){}
    std::size_t BindingCount(std::int32_t a) const{return kind_==Kind::Interface?source_->BindingCount(a):view_.BindingCount(view_.CheckedAction(a));}
    std::uint8_t CurrentState(std::int32_t a,std::size_t b) const{return kind_==Kind::Interface?source_->CurrentState(a,b):view_.CurrentState(view_.CheckedAction(a),b);}
    std::uint8_t PreviousState(std::int32_t a,std::size_t b) const{return kind_==Kind::Interface?source_->PreviousState(a,b):view_.PreviousState(view_.CheckedAction(a),b);}
private:
    enum class Kind{Interface,View};Kind kind_;
    const IGuiInputNativeKeyboard* source_=nullptr;
    Platform::KeyboardView view_;
};
class IGuiInputNativeReceiver
{
public:
    virtual ~IGuiInputNativeReceiver() = default;
    virtual GuiInputMouseBorrow MouseAt00() = 0;
    virtual GuiInputKeyboardBorrow KeyboardAt18() = 0;
};

class IGuiInputNativeSource
{
public:
    virtual ~IGuiInputNativeSource() = default;
    // 一次查询只捕获一次receiver，随后才解析所需子槽；不经过413BE0。
    virtual IGuiInputNativeReceiver& InputAt418210() = 0;
};

class GuiInputNativeScope final
{
public:
    explicit GuiInputNativeScope(IGuiInputNativeSource& source) noexcept;
    explicit GuiInputNativeScope(IGuiInputNativeSource* source) noexcept;
    ~GuiInputNativeScope() noexcept;
    GuiInputNativeScope(const GuiInputNativeScope&) = delete;
    GuiInputNativeScope& operator=(const GuiInputNativeScope&) = delete;
private:
    IGuiInputNativeSource* previous_;
};

struct GuiInputNativeBody;
struct GuiInputNativeVtable final
{
    std::int32_t (__thiscall* readHeldAt00)(const GuiInputNativeBody*, std::int32_t);
    std::int32_t (__thiscall* readPressedAt04)(const GuiInputNativeBody*, std::int32_t);
    POINT* (__thiscall* readPointerAt08)(const GuiInputNativeBody*, POINT*);
    std::int32_t (__thiscall* readWheelAt0C)(const GuiInputNativeBody*);
    std::int32_t (__thiscall* readPointerMovedAt10)(const GuiInputNativeBody*);
    std::int32_t (__thiscall* readMousePressAt14)(const GuiInputNativeBody*);
    std::int32_t (__thiscall* readMouseReleaseAt18)(const GuiInputNativeBody*);
};

// 5BDC88/5C0C20：唯一原字段，无源指针、隐含owner或虚析构。
struct GuiInputNativeBody final
{
    const GuiInputNativeVtable* vptrAt00;
};
static_assert(sizeof(GuiInputNativeBody) == 4);
static_assert(alignof(GuiInputNativeBody) == 4);
static_assert(offsetof(GuiInputNativeBody, vptrAt00) == 0);
static_assert(std::is_trivially_destructible_v<GuiInputNativeBody>);
static_assert(sizeof(GuiInputNativeVtable) == 0x1C);
static_assert(offsetof(GuiInputNativeVtable, readHeldAt00) == 0);
static_assert(offsetof(GuiInputNativeVtable, readPressedAt04) == 4);
static_assert(offsetof(GuiInputNativeVtable, readPointerAt08) == 8);
static_assert(offsetof(GuiInputNativeVtable, readWheelAt0C) == 0x0C);
static_assert(offsetof(GuiInputNativeVtable, readPointerMovedAt10) == 0x10);
static_assert(offsetof(GuiInputNativeVtable, readMousePressAt14) == 0x14);
static_assert(offsetof(GuiInputNativeVtable, readMouseReleaseAt18) == 0x18);

[[nodiscard]] const GuiInputNativeVtable& NullGuiInputVtable() noexcept;
[[nodiscard]] const GuiInputNativeVtable& GameGuiInputVtable() noexcept;
[[nodiscard]] GuiInputNativeBody* NewNullGuiInputAt4E0C93();
[[nodiscard]] GuiInputNativeBody* NewGameGuiInputAt4EAAED();

// fresh输出；先清Ref，再分配Null正文与独立18h控制块。control失败不回收base。
void ConstructNullGuiInputReferenceAt4E0BE0(YtlReferenceSlotRuntime& out);
// 原536EA0的input成员槽适配；旧释放可抛，新base先发布，无回滚/alias优化。
void AdoptGuiInputAt536EA0(YtlReferenceSlotRuntime& slot, GuiInputNativeBody* owned);
// 原4F6620的终止边界与可抛adopt分开；无control时保留孤立base/bias。
void ReleaseGuiInputReferenceAt4F6620(YtlReferenceSlotRuntime& slot) noexcept;
}

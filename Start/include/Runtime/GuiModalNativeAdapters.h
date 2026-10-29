#pragma once
#include <Platform/InputViews.h>
#include <Runtime/GuiInputNative.h>
#include <Runtime/GuiDirtyRectNative.h>
#include <Platform/Input.h>

namespace KaGuYa::Start::Runtime
{
// 只借同源producer的按值view；默认生产为原24h singleton，不维护GUI输入镜像。
class GuiModalInputSource final : public IGuiInputNativeSource, private IGuiInputNativeReceiver
{
public:
    explicit GuiModalInputSource(const Platform::IInputRuntime& input) noexcept : input_(input) {}
    IGuiInputNativeReceiver& InputAt418210() override { return *this; }
private:
    GuiInputMouseBorrow MouseAt00() override;
    GuiInputKeyboardBorrow KeyboardAt18() override;
    const Platform::IInputRuntime& input_;
};

// 仅借拥有槽，每次调用重新解析；不创建另一张矩形表或额外owner。
class GuiModalDirtyRectView final
{
public:
    explicit GuiModalDirtyRectView(YtlReferenceSlotRuntime& slot) noexcept : slot_(slot) {}
    void PrepareAtVslot04();
    std::uint32_t CountAtVslot18() const;
    RECT* RectsAtVslot1C() const;
    void AddAtVslot08(const RECT& rect);
    void ClearAtVslot00();
private:
    GuiDirtyRectNativeBody80h& Body() const;
    YtlReferenceSlotRuntime& slot_;
};
}

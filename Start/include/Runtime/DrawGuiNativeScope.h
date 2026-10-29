#pragma once

#include <Runtime/GuiControlNative.h>
#include <Runtime/GuiInputNative.h>
#include <Runtime/OpacityDialogNative.h>

#include <optional>

namespace KaGuYa::Start::Runtime
{
// 业务状态仍在原正文/根内；这里只借用原4182F0、418210、50D730/52DF20的来源。
struct DrawGuiNativeServices final
{
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    const IOpacityDialogResetStateServices& opacityReset;
    IOpacityDialogUpdateServices& opacityUpdate;
};

// 不Capture、不分配。空指针仅用于显式独立调用已建立外层scope的入口，
// 默认PostWindow依赖要求非空完整服务，不以此实现失败兜底。
class DrawGuiNativeScope final
{
public:
    explicit DrawGuiNativeScope(DrawGuiNativeServices* services) noexcept;
    ~DrawGuiNativeScope() noexcept;
    DrawGuiNativeScope(const DrawGuiNativeScope&) = delete;
    DrawGuiNativeScope& operator=(const DrawGuiNativeScope&) = delete;
private:
    std::optional<GuiControlGraphicsScope> graphics_;
    std::optional<GuiInputNativeScope> input_;
    std::optional<OpacityDialogNativeScope> opacity_;
};
}

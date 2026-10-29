#pragma once

#include <Runtime/ProgramSystemDialogNative.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
// 非拥有长期合同：服务、父服务及所有叶必须覆盖逃逸child/control最后释放。
// nullptr仅清空本层选择；Require在触及共享叶前终止，不借外层System兜底。
// 确认框等独立域的共享GUI绑定不应被空System选择抹去。
// Parent可直接内嵌本Scope，故此处不内嵌/构造ParentScope，也不改Parent选择。
class ProgramSystemDialogNativeScope final
{
public:
    explicit ProgramSystemDialogNativeScope(ProgramSystemDialogNativeServices&) noexcept;
    explicit ProgramSystemDialogNativeScope(ProgramSystemDialogNativeServices*) noexcept;
    ~ProgramSystemDialogNativeScope() noexcept;
    ProgramSystemDialogNativeScope(const ProgramSystemDialogNativeScope&) = delete;
    ProgramSystemDialogNativeScope& operator=(const ProgramSystemDialogNativeScope&) = delete;
private:
    ProgramSystemDialogNativeServices* previous_;
    std::optional<GraphicsSurfaceNativeScope> surfaces_;
    std::optional<GuiControlGraphicsScope> graphics_;
    std::optional<GuiInputNativeScope> input_;
    std::optional<GuiDialogNativeScope> screen_;
};
}

#pragma once
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Runtime/ProgramMain00DialogNative.h>
#include <Runtime/ProgramMain01DialogNative.h>
#include <Runtime/ProgramSystemDialogNativeScope.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
struct ProgramSelectDialogNativeServices;

// 非拥有绑定；服务必须覆盖逃逸原生Ref的最后一次释放。
class ProgramSelectDialogNativeScope final
{
public:
    explicit ProgramSelectDialogNativeScope(ProgramSelectDialogNativeServices&) noexcept;
    explicit ProgramSelectDialogNativeScope(ProgramSelectDialogNativeServices*) noexcept;
    ~ProgramSelectDialogNativeScope() noexcept;
    ProgramSelectDialogNativeScope(const ProgramSelectDialogNativeScope&) = delete;
    ProgramSelectDialogNativeScope& operator=(const ProgramSelectDialogNativeScope&) = delete;
private:
    ProgramSelectDialogNativeServices* previous_;
    ProgramSelectDialogNativeServices* selected_;
    // 子层只绑定叶，不构造本父Scope；无递归构造或隐藏堆分配。
    std::optional<ProgramMain00DialogNativeScope> main00_;
    std::optional<ProgramMain01DialogNativeScope> main01_;
    std::optional<ProgramSystemDialogNativeScope> system_;
    // 9C的Surface可逃逸到Scene正常/异常尾，不能只在GUI方法内部绑定。
    std::optional<GraphicsSurfaceNativeScope> surfaces_;
};
}

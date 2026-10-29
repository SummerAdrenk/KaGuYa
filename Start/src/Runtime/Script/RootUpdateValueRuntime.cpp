#include <Runtime/RootUpdateValueRuntime.h>

#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScrVmValueTable.h>

namespace KaGuYa::Start::Runtime
{
RootUpdateValueRuntime::RootUpdateValueRuntime(EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, decltype(&::MessageBoxA) messageBox) noexcept
    : environment_(environment), root_(root), messageBox_(messageBox)
{
}

std::int32_t RootUpdateValueRuntime::ReadUpdateValueAt4C73A0(const std::int32_t index) const
{
    // 4D2A85/4D2A90/4D2A9A：先 Ensure，再捕获当前 root 的 CParams；不查命名表或 slot0。
    root_.EnsureCreatedAt413BE0();
    RootRuntimePayload& capturedRoot = root_.RequirePublished();
    auto& params = capturedRoot.ParamsAt47BDB0();
    if (index == -1)
    {
        // 4C73EC..4C7411 -> 420C50：此分支不解引用 SaveParams2；晚取标题 raw、HWND。
        const auto& errorEnvironment = environment_.CaptureAt4180C0();
        const char* const caption = errorEnvironment.titleAt10.GetString();
        const HWND window = reinterpret_cast<HWND>(errorEnvironment.windowAt2C);
        static_cast<void>(messageBox_(window, "GetValue over num", caption, MB_ICONERROR));
        throw 1;
    }
    const auto& values = params.SaveParams2At30().values;
    // 原 4C73A0 仅检查 -1，其余越界原会非法访问；现代明确拒绝，不伪造原提示/int1。
    if (index < 0 || static_cast<std::size_t>(index) >= values.size())
        throw ScrVmValueException(ScrVmValueError::IndexOutOfRange, index,
            "UPDATE value index is outside the 512-slot table (modern safety boundary)");
    return values[static_cast<std::size_t>(index)];
}
}

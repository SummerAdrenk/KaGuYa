#pragma once

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class NativeEditWindowCollectionRuntime;

// 真实CEditWindow及其内嵌CEditControl；返回对象由root集合持有，HWND不是owner。
class NativeEditWindowRuntime final
{
public:
    [[nodiscard]] static std::shared_ptr<NativeEditWindowRuntime> CreateAt46FA30(
        std::uint32_t disableDword,
        std::uint32_t alignmentDword,
        std::uint32_t borderFlagDword,
        HWND environmentParent);
    ~NativeEditWindowRuntime() noexcept;
    NativeEditWindowRuntime(const NativeEditWindowRuntime&) = delete;
    NativeEditWindowRuntime& operator=(const NativeEditWindowRuntime&) = delete;

    [[nodiscard]] HWND Window() const noexcept;
    [[nodiscard]] HWND EditControlWindow() const noexcept;
    [[nodiscard]] RECT& LogicalRectAt4C() noexcept;
    [[nodiscard]] const RECT& LogicalRectAt4C() const noexcept;
    [[nodiscard]] RECT& DefaultMarginsAt5C() noexcept;
    [[nodiscard]] const RECT& DefaultMarginsAt5C() const noexcept;
    [[nodiscard]] const RECT* AppliedMarginsAt6C() const noexcept;
    void ApplyLayoutAt4880F0(RECT target, const RECT* margins);

    // 原构造未写+48/+78，用未发布状态保留；按键和WM_CREATE才分别发布。
    [[nodiscard]] std::optional<std::uint32_t>& EventAt48() noexcept;
    [[nodiscard]] const std::optional<std::uint32_t>& EventAt48() const noexcept;
    [[nodiscard]] std::optional<std::uint32_t> ControlIdentifierAt78() const noexcept;
    [[nodiscard]] std::int32_t& TextByteLimitAt7C() noexcept;
    [[nodiscard]] COLORREF& TextColorAt84() noexcept;
    [[nodiscard]] COLORREF BackgroundColorAt88() const noexcept;
    // 对应50D413..50D58D的实际GDI brush更新，不创建绘制替身。
    void SetBackgroundColor(COLORREF color);

private:
    friend class NativeEditWindowCollectionRuntime;
    struct State;
    NativeEditWindowRuntime();
    // root集合先建立桥接control，再进入可能同步发送WM_CREATE的窗口创建。
    void CreateWindows(std::uint32_t disableDword, std::uint32_t alignmentDword,
        std::uint32_t borderFlagDword, HWND environmentParent);
    std::unique_ptr<State> state_;
};
}

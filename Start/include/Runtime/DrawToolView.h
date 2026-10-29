#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
class FrameLayerItem;
class ToolWindowLayerRuntime;
class ToolWindowInteractionRuntime;
class IToolWindowInteractionServices;
struct ToolWindowNativeBody40h;
struct ToolWindowNativeInteractionServices;
struct DrawGuiNativeServices;

// 七项普通业务入口不是CToolWin的frame虚槽；分派只委托既有实现。
struct DrawToolViewMethods final
{
    void (*setMode)(void*, void*, std::int32_t);
    std::uint32_t (*tick)(void*, void*);
    void (*capture)(void*, void*);
    void (*restore)(void*, void*);
    void (*setCaptureMode)(void*, void*, std::uint8_t);
    void (*propagateDirty)(void*, void*);
    void (*resetSkipLatch)(void*, void*, std::uint8_t);
    std::int32_t (*readEvent)(void*);
    void (*writeLatch)(void*, std::uint8_t);
    std::uint8_t (*enabled)(void*);
};

// 按值固定本次receiver/分派表/外部服务；不保活、不取根、不复制任何字段或Dialog。
// receiver及服务必须覆盖view的使用期。空服务只允许直接字段访问，业务调用明确拒绝。
// 默认view拒绝未绑定协议；显式native null不添加现代正文保护，Controlled另保留类型拒绝。
class DrawToolView final
{
public:
    DrawToolView() noexcept = default;
    DrawToolView(std::nullptr_t) noexcept {}
    DrawToolView(ToolWindowNativeBody40h*, ToolWindowNativeInteractionServices* = nullptr) noexcept;
    DrawToolView(FrameLayerItem*, IToolWindowInteractionServices* = nullptr) noexcept;
    DrawToolView(ToolWindowNativeBody40h&, ToolWindowNativeInteractionServices&) noexcept;
    DrawToolView(ToolWindowLayerRuntime&, IToolWindowInteractionServices&) noexcept;
    // interaction必须借用同一个Tool；它没有公开receiver查询，调用者显式保证同一性。
    // 字段仍直接访问Tool，业务借既有interaction；不复制或另建interaction状态。
    DrawToolView(ToolWindowLayerRuntime&, ToolWindowInteractionRuntime&) noexcept;

    // 仅按值复制借用指针；空gui沿用调用方外层scope，不替代必需的业务服务。
    [[nodiscard]] DrawToolView WithNativeGuiServices(DrawGuiNativeServices* gui) const noexcept
    { auto result = *this; result.gui_ = gui; return result; }

    explicit operator bool() const noexcept { return body_ != nullptr; }
    const void* Receiver() const noexcept { return body_; }
    // IDA 434FDA/434FDD/435174比较完整DWORD，不能在view里缩成BYTE或bool。
    void SetModeAt434FB0(std::int32_t mode) const;
    [[nodiscard]] std::uint32_t TickAt4BE200() const;
    void CaptureAt4C01F0() const;
    void RestoreAt4BF4E0() const;
    // IDA 4C2359 movzx及4AB913字节比较；高位BYTE值也原样交给既有算法。
    void SetCaptureModeAt4C2300(std::uint8_t value) const;
    void PropagateDirtyRectsAt415910() const;
    void ResetSkipLatchAt4AB910(std::uint8_t value) const;
    // IDA 4BE3F9读+34 DWORD，4B12B2/4A175C写+38 BYTE；-1保持有符号事件语义。
    [[nodiscard]] std::int32_t ReadEventAt34() const { return Methods().readEvent(body_); }
    void WriteLatchByteAt38(std::uint8_t value) const { Methods().writeLatch(body_, value); }
    std::uint8_t EnabledByteAt0C() const { return Methods().enabled(body_); }

private:
    const DrawToolViewMethods& Methods() const;
    void* body_ = nullptr;
    const DrawToolViewMethods* methods_ = nullptr;
    void* services_ = nullptr;
    DrawGuiNativeServices* gui_ = nullptr;
};
static_assert(std::is_trivially_copyable_v<DrawToolView>);
static_assert(std::is_standard_layout_v<DrawToolView>);
static_assert(sizeof(DrawToolView) == 4 * sizeof(void*));
}

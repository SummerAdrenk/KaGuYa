#pragma once

#include <Runtime/RuntimeCString.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Windows.h>
#include <array>
#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
using WindowModeViewport = std::array<std::int32_t, 4>;

// 477934/5B2B34：仅一个deleting虚槽。平台服务不得存入原2Ch正文。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunnecessary-virtual-specifier"
#endif
class WindowModeNative2Ch final
{
public:
    explicit WindowModeNative2Ch(IRuntimeStringManagerSource& strings);
    ~WindowModeNative2Ch() noexcept;
    WindowModeNative2Ch(const WindowModeNative2Ch&) = delete;
    WindowModeNative2Ch& operator=(const WindowModeNative2Ch&) = delete;
    virtual WindowModeNative2Ch* DeleteAtVslot00(std::uint32_t flags) noexcept;

    std::uint8_t modeAt04;
    std::array<std::byte, 3> untouchedAt05;
    union { RuntimeCString deviceNameAt08; };
    WindowModeViewport viewportAt0C;
    RECT savedMonitorRectAt1C;
};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
static_assert(sizeof(WindowModeNative2Ch) == 0x2C && alignof(WindowModeNative2Ch) == 4);
static_assert(offsetof(WindowModeNative2Ch, modeAt04) == 4);
static_assert(offsetof(WindowModeNative2Ch, deviceNameAt08) == 8);
static_assert(offsetof(WindowModeNative2Ch, viewportAt0C) == 0x0C);
static_assert(offsetof(WindowModeNative2Ch, savedMonitorRectAt1C) == 0x1C);

// 47791D/599530：显式原始分配，避免已在普通new产物中确认的正文额外清零。
[[nodiscard]] WindowModeNative2Ch* NewWindowModeAt47791D(
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 47798B..477A16：先完成新正文，再释放旧Ref；发布base后才分配18h control。
// 旧release失败不回收新正文，control分配失败不回滚已发布base。
void CreateAndReplaceWindowModeAt47791D(YtlReferenceSlotRuntime& slot,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
// 安装旧槽的释放可抛；4640E0最终根析构的同一操作则处于终止边界。
void ReleaseWindowModeReferenceAt47798B(YtlReferenceSlotRuntime& slot);
void ReleaseWindowModeReferenceAt464415(YtlReferenceSlotRuntime& slot) noexcept;
[[nodiscard]] WindowModeNative2Ch* ResolveWindowModeReference(const YtlReferenceSlotRuntime& slot) noexcept;
}

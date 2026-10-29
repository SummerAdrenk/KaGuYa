#pragma once

#include <Runtime/GraphicsModuleNative.h>

namespace KaGuYa::Start::Runtime
{
// 418349/6198B8：无vptr的1Ch正文，union禁止Ref/CString的DMI和自动析构提前介入。
struct GraphicsRootNativeBody1Ch final
{
    union { YtlReferenceSlotRuntime moduleAt00; };
    HWND windowAt0C;
    std::int32_t widthAt10;
    std::int32_t heightAt14;
    union { RuntimeCString errorAt18; };
    GraphicsRootNativeBody1Ch() noexcept {}
    ~GraphicsRootNativeBody1Ch() noexcept {}
    GraphicsRootNativeBody1Ch(const GraphicsRootNativeBody1Ch&) = delete;
    GraphicsRootNativeBody1Ch& operator=(const GraphicsRootNativeBody1Ch&) = delete;
};
static_assert(sizeof(GraphicsRootNativeBody1Ch) == 0x1C);
static_assert(alignof(GraphicsRootNativeBody1Ch) == 4);
static_assert(offsetof(GraphicsRootNativeBody1Ch, moduleAt00) == 0);
static_assert(offsetof(GraphicsRootNativeBody1Ch, windowAt0C) == 0x0C);
static_assert(offsetof(GraphicsRootNativeBody1Ch, widthAt10) == 0x10);
static_assert(offsetof(GraphicsRootNativeBody1Ch, heightAt14) == 0x14);
static_assert(offsetof(GraphicsRootNativeBody1Ch, errorAt18) == 0x18);
static_assert(std::is_standard_layout_v<GraphicsRootNativeBody1Ch>);

// 41835A先清Ref，再417360/GetNil；+0C..+17不写，失败不补删raw。
[[nodiscard]] GraphicsRootNativeBody1Ch* CreateGraphicsRootNativeAt418349(
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings);

void ReleaseGraphicsModuleAt440620(GraphicsRootNativeBody1Ch& root,
    IEnvironmentRootMemory& memory, IGraphicsModuleNativeWin32Api& win32);
// 普通调用允许外传；ExitSystem返回后重读固定root的当前Ref，绝不清尺寸/错误串。
void ShutdownGraphicsRootAt440660(GraphicsRootNativeBody1Ch& root,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings,
    IGraphicsModuleNativeWin32Api& win32);
// DLL返回值仅借用；479FE0则只借根内错误串，两者不可合并。
[[nodiscard]] const char* GetGraphicsErrorMessageAt440D00(GraphicsRootNativeBody1Ch& root,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32);
[[nodiscard]] const char* ReadGraphicsErrorAt479FE0(const GraphicsRootNativeBody1Ch& root) noexcept;

// 唯一发布槽的显式owner。服务均在正文外借用，不含modern IGraphicsRuntimeCalls。
// Process登记须另用显式存储，atexit只调Release，不再登记本owner的自动静态析构。
class GraphicsRootOwner final
{
public:
    GraphicsRootOwner(IEnvironmentRootLock& lock, IEnvironmentRootMemory& memory,
        IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept;
    ~GraphicsRootOwner() noexcept;
    GraphicsRootOwner(const GraphicsRootOwner&) = delete;
    GraphicsRootOwner& operator=(const GraphicsRootOwner&) = delete;

    [[nodiscard]] GraphicsRootNativeBody1Ch* CaptureAt4182F0();
    void ReleaseAt5AADF0() noexcept;
    [[nodiscard]] GraphicsRootNativeBody1Ch* Published() const noexcept { return published_; }
    [[nodiscard]] GraphicsRootNativeBody1Ch*& PublicationSlot() noexcept { return published_; }
private:
    void LeaveNoexcept() noexcept;
    IEnvironmentRootLock& lock_;
    IEnvironmentRootMemory& memory_;
    IRuntimeStringManagerSource& strings_;
    IGraphicsModuleNativeWin32Api& win32_;
    GraphicsRootNativeBody1Ch* published_ = nullptr;
};
}

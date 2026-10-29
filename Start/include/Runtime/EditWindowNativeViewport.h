#pragma once
#include <Runtime/EditWindowNativeCollection.h>
#include <Runtime/EditWindowNativeMaps.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;

// 每个原413BE0调用点重新选根及模式槽；不能缓存一代正文或增持Ref。
class IEditWindowNativeViewportRootSource
{
public:
    virtual ~IEditWindowNativeViewportRootSource() = default;
    virtual const YtlReferenceSlotRuntime& CaptureModeSlotAfter413BE0() = 0;
};
class RootEditWindowNativeViewportSource final : public IEditWindowNativeViewportRootSource
{
public:
    explicit RootEditWindowNativeViewportSource(RootRuntimeOwner& root) noexcept : root_(root) {}
    const YtlReferenceSlotRuntime& CaptureModeSlotAfter413BE0() override;
private:
    RootRuntimeOwner& root_;
};

// 原6198C8是60h Environment singleton，不是HWND，也不是现代Root镜像。
class IEditWindowNativeViewportEnvironmentSource
{
public:
    virtual ~IEditWindowNativeViewportEnvironmentSource() = default;
    virtual EnvironmentRootStorage* CaptureAt4180C0() = 0;
};
class EnvironmentEditWindowNativeViewportSource final : public IEditWindowNativeViewportEnvironmentSource
{
public:
    explicit EnvironmentEditWindowNativeViewportSource(EnvironmentRootOwner& owner) noexcept : owner_(owner) {}
    EnvironmentRootStorage* CaptureAt4180C0() override;
private:
    EnvironmentRootOwner& owner_;
};

class IEditWindowNativeViewportPlatformApi
{
public:
    virtual ~IEditWindowNativeViewportPlatformApi() = default;
    virtual BOOL IsWindowVisible(HWND) = 0;
    virtual BOOL OffsetRect(RECT*, int, int) = 0;
};
IEditWindowNativeViewportPlatformApi& DefaultEditWindowNativeViewportPlatformApi() noexcept;

// 仅原operator new叶；返回的非空存储必须可由全局sized delete释放。
// 保留原空返回分支，不能在算法中将其升级为bad_alloc或补回滚裸rectangle。
class IEditWindowNativeViewportAllocationApi
{
public:
    virtual ~IEditWindowNativeViewportAllocationApi() = default;
    virtual void* Allocate(std::size_t) = 0;
};
IEditWindowNativeViewportAllocationApi& DefaultEditWindowNativeViewportAllocationApi() noexcept;

struct EditWindowNativeViewportServices final
{
    IEditWindowNativeViewportRootSource& root;
    IEditWindowNativeViewportEnvironmentSource& environment;
    IEditWindowNativeViewportPlatformApi& platform = DefaultEditWindowNativeViewportPlatformApi();
    IEditWindowNativeMapApi& maps = DefaultEditWindowNativeMapApi();
    IEditWindowNativeViewportAllocationApi& allocations = DefaultEditWindowNativeViewportAllocationApi();
};

// receiver只有caller已选root拥有的唯一+54头；本函数不创建/拥有另一份窗口集合。
// target非空绕过可见性检查；为空时仅处理可见项。无现代null/size/cursor保护。
void UpdateEditWindowNativeViewportAt4875F0(EditWindowNativeReferences& receiver,
    EditWindowNativeBody98h* target, EditWindowNativeViewportServices& services);
// 已有native消息scope内使用上式；独立真实Win32调用使用此重载，覆盖同步WM_SIZE回调。
// maps.api同时供4880F0与原Window98/Control28表使用，scope不新增Root/Environment捕获。
void UpdateEditWindowNativeViewportAt4875F0(EditWindowNativeReferences& receiver,
    EditWindowNativeBody98h* target, EditWindowNativeViewportServices& services,
    EditWindowNativeMapServices& maps, WindowThunkNativeServices& thunks);
}

#pragma once

#include <Runtime/YtlReferenceArrayRuntime.h>

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

namespace KaGuYa::Start::Runtime
{
class NativeEditWindowRuntime;
class WindowModeManagerRuntime;
class WindowModeView;
struct WindowEnvironmentRuntime;
class EnvironmentStartupRuntime;
class RootRuntimeOwner;

enum class NativeEditCollectionAllocation
{
    ReferenceStorage,
    WindowOwnerBridge,
    LayoutRectangle,
    LayoutControl,
};

class INativeEditWindowCollectionMemoryApi
{
public:
    virtual ~INativeEditWindowCollectionMemoryApi() = default;
    // 分配失败必须抛出；size 为现代实际存储字节数，不冒充原对象 ABI 大小。
    virtual void* Allocate(NativeEditCollectionAllocation kind, std::size_t size) = 0;
    virtual void Deallocate(NativeEditCollectionAllocation kind, void* memory, std::size_t size) noexcept = 0;
};

class NativeEditWindowCollectionMemoryApi final : public INativeEditWindowCollectionMemoryApi
{
public:
    void* Allocate(NativeEditCollectionAllocation kind, std::size_t size) override;
    void Deallocate(NativeEditCollectionAllocation kind, void* memory, std::size_t size) noexcept override;
};

class INativeEditWindowCollectionPlatformApi
{
public:
    virtual ~INativeEditWindowCollectionPlatformApi() = default;
    virtual BOOL IsWindowVisible(HWND window) = 0;
    virtual BOOL OffsetRect(RECT& rectangle, int x, int y) = 0;
};

class Win32NativeEditWindowCollectionPlatformApi final : public INativeEditWindowCollectionPlatformApi
{
public:
    BOOL IsWindowVisible(HWND window) override;
    BOOL OffsetRect(RECT& rectangle, int x, int y) override;
};

// 保留 Ref12 解析所需的 stride/count；两个虚槽沿用原 YTL 释放协议。
class NativeEditWindowReferenceControlRuntime : public YtlReferenceControlRuntime
{
public:
    std::uint32_t strideAt0C = 0;
    std::uint32_t countAt10 = 1;
protected:
    ~NativeEditWindowReferenceControlRuntime() = default;
};

struct NativeEditWindowReferenceSlotRuntime
{
    std::uintptr_t baseAt00 = 0;
    std::uint32_t biasAt04 = 0;
    NativeEditWindowReferenceControlRuntime* controlAt08 = nullptr;
};
static_assert(sizeof(NativeEditWindowReferenceSlotRuntime) == 12);

class NativeEditWindowReferenceRuntime final
{
public:
    NativeEditWindowReferenceRuntime() noexcept = default;
    NativeEditWindowReferenceRuntime(const NativeEditWindowReferenceRuntime& source) noexcept;
    NativeEditWindowReferenceRuntime(NativeEditWindowReferenceRuntime&& source) noexcept;
    NativeEditWindowReferenceRuntime& operator=(const NativeEditWindowReferenceRuntime& source);
    NativeEditWindowReferenceRuntime& operator=(NativeEditWindowReferenceRuntime&& source);
    ~NativeEditWindowReferenceRuntime() noexcept;

    // 现代显式接管一个已有 Ref12 计数；成功返回时清调用方槽，不额外增持。
    [[nodiscard]] static NativeEditWindowReferenceRuntime AdoptSlot(NativeEditWindowReferenceSlotRuntime& slot) noexcept;
    // Harvey 工厂的唯一 shared owner 移入桥接控制块，之后只能经 Ref12 复制所有权。
    // 拒绝空 shared_ptr 或 use_count!=1；桥接控制块 owning 必须保持1。
    [[nodiscard]] static NativeEditWindowReferenceRuntime AdoptCreatedWindow(
        std::shared_ptr<NativeEditWindowRuntime>&& window,
        INativeEditWindowCollectionMemoryApi& memory);
    [[nodiscard]] NativeEditWindowRuntime* Get() const noexcept;
    [[nodiscard]] const NativeEditWindowReferenceSlotRuntime& Slot() const noexcept { return slot_; }
    // 显式释放按486B60的EHFlags1可抛；析构走4600A0的EHFlags5终止契约。
    void Reset();
private:
    friend class NativeEditWindowCollectionRuntime;
    NativeEditWindowReferenceSlotRuntime slot_;
};
static_assert(sizeof(NativeEditWindowReferenceRuntime) == 12);

class NativeEditWindowCollectionRuntime final
{
public:
    // 环境、mode owning槽和两项服务须长于本容器及由Find返回的引用。
    NativeEditWindowCollectionRuntime(const WindowEnvironmentRuntime& environment,
        const std::shared_ptr<WindowModeManagerRuntime>& modeSlot,
        INativeEditWindowCollectionPlatformApi& platform,
        INativeEditWindowCollectionMemoryApi& memory) noexcept;
    NativeEditWindowCollectionRuntime(const EnvironmentStartupRuntime& environment,
        RootRuntimeOwner& root, INativeEditWindowCollectionPlatformApi& platform,
        INativeEditWindowCollectionMemoryApi& memory) noexcept;
    ~NativeEditWindowCollectionRuntime() noexcept;
    NativeEditWindowCollectionRuntime(const NativeEditWindowCollectionRuntime&) = delete;
    NativeEditWindowCollectionRuntime& operator=(const NativeEditWindowCollectionRuntime&) = delete;

    // 返回借用对象；列表注册成功后释放局部Ref，不要求caller手动拼装所有权。
    [[nodiscard]] NativeEditWindowRuntime* CreateAt46FA30(
        std::uint32_t disable, std::uint32_t alignment, std::uint32_t border);
    [[nodiscard]] NativeEditWindowRuntime* RegisterAt46FC43(const NativeEditWindowReferenceRuntime& source);
    [[nodiscard]] NativeEditWindowReferenceRuntime FindAt479DF0(NativeEditWindowRuntime* target) const noexcept;
    // nullptr为整表清空；只改end，保留容量。非空仅移除首个匹配项。
    void RemoveAt486B60(NativeEditWindowRuntime* target);
    // target非空跳过可见性检查；空列表也先读取真实viewport，不新增默认值。
    void UpdateLayoutAt4875F0(NativeEditWindowRuntime* target);
    [[nodiscard]] std::size_t Size() const noexcept { return size_; }
    [[nodiscard]] std::size_t Capacity() const noexcept { return capacity_; }
    [[nodiscard]] std::span<const NativeEditWindowReferenceSlotRuntime> References() const noexcept
        { return {storage_, size_}; }

private:
    WindowModeView CurrentMode() const;
    HWND CurrentWindow() const;
    std::int32_t CurrentWidth() const;
    std::int32_t CurrentHeight() const;
    void RequireCursor(const NativeEditWindowReferenceSlotRuntime* storage, std::size_t index) const;
    const WindowEnvironmentRuntime* environment_ = nullptr;
    const std::shared_ptr<WindowModeManagerRuntime>* modeSlot_ = nullptr;
    const EnvironmentStartupRuntime* startupEnvironment_ = nullptr;
    RootRuntimeOwner* root_ = nullptr;
    INativeEditWindowCollectionPlatformApi& platform_;
    INativeEditWindowCollectionMemoryApi& memory_;
    NativeEditWindowReferenceSlotRuntime* storage_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};
}

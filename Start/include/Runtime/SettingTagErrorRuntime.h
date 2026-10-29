#pragma once

#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ISettingTagErrorSource
{
public:
    virtual ~ISettingTagErrorSource() = default;
    [[nodiscard]] virtual RuntimeCString* CaptureAt47BCF0() = 0;
};

// 61A4F0持有一个独立4-byte CString，不是Environment.errors或查询局部临时。
class SettingTagErrorOwner final : public ISettingTagErrorSource
{
public:
    explicit SettingTagErrorOwner(IEnvironmentRootLock& lock,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    ~SettingTagErrorOwner() noexcept override;
    SettingTagErrorOwner(const SettingTagErrorOwner&) = delete;
    SettingTagErrorOwner& operator=(const SettingTagErrorOwner&) = delete;

    [[nodiscard]] RuntimeCString* CaptureAt47BCF0() override;
    void ReleaseAt5AB440() noexcept;
    [[nodiscard]] RuntimeCString* Published() const noexcept { return published_; }
    [[nodiscard]] RuntimeCString*& PublicationSlot() noexcept { return published_; }
private:
    void LeaveNoexcept() noexcept;
    IEnvironmentRootLock& lock_;
    IEnvironmentRootMemory& memory_;
    IRuntimeStringManagerSource& strings_;
    RuntimeCString* published_ = nullptr;
};

// 6010F4与Environment的601164不是同一把锁；进程内所有SettingTag查询共享此owner。
[[nodiscard]] SettingTagErrorOwner& ProcessSettingTagErrorOwner();
}

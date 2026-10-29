#pragma once

#include <Runtime/RuntimeCString.h>
#include <Runtime/YtlReferenceArrayRuntime.h>

#include <Windows.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class CategorizedSoundEnvironmentAdapter;
class SaveDataEnvironmentArchive;
template<class T>
struct EnvironmentVectorStorage final
{
    T* begin;
    T* end;
    T* capacity;
};

// 41811D 的真实 0x60 正文；不包含现代启动 services 或重复的窗口字段。
// 必须由下面的创建/销毁函数逐成员管理，不得普通 new/delete 或值初始化整块。
struct EnvironmentRootStorage final
{
    HMODULE moduleAt00;
    HINSTANCE previousAt04;
    LPSTR commandLineAt08;
    int showCommandAt0C;
    RuntimeCString titleAt10;
    RuntimeCString appendTitleAt14;
    std::uint32_t widthAt18;
    std::uint32_t heightAt1C;
    std::uint32_t styleAt20;
    std::uint32_t exStyleAt24;
    std::uint32_t fullscreenStyleAt28;
    std::uint32_t windowAt2C;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> archivesAt30;
    EnvironmentVectorStorage<RuntimeCString> errorsAt3C;
    YtlReferenceSlotRuntime pathsAt48;
    YtlReferenceSlotRuntime versionAt54;
};
static_assert(sizeof(EnvironmentRootStorage) == 0x60);
static_assert(offsetof(EnvironmentRootStorage, titleAt10) == 0x10);
static_assert(offsetof(EnvironmentRootStorage, appendTitleAt14) == 0x14);
static_assert(offsetof(EnvironmentRootStorage, windowAt2C) == 0x2C);
static_assert(offsetof(EnvironmentRootStorage, archivesAt30) == 0x30);
static_assert(offsetof(EnvironmentRootStorage, errorsAt3C) == 0x3C);
static_assert(offsetof(EnvironmentRootStorage, pathsAt48) == 0x48);
static_assert(offsetof(EnvironmentRootStorage, versionAt54) == 0x54);

class IEnvironmentRootMemory
{
public:
    virtual ~IEnvironmentRootMemory() = default;
    virtual void* AllocateBody(std::size_t bytes) = 0;
    virtual void DeleteBody(void* address, std::size_t bytes) = 0;
    // 接收53FBFB最终实参；大块已经还原raw并将bytes增加0x23。
    virtual void DeleteVector(void* address, std::size_t bytes) = 0;
};

IEnvironmentRootMemory& DefaultEnvironmentRootMemory() noexcept;

class IEnvironmentRootLock
{
public:
    virtual ~IEnvironmentRootLock() = default;
    virtual void Enter() = 0;
    virtual void Leave() = 0;
};

class EnvironmentRootCriticalSection final : public IEnvironmentRootLock
{
public:
    EnvironmentRootCriticalSection();
    ~EnvironmentRootCriticalSection() noexcept override;
    EnvironmentRootCriticalSection(const EnvironmentRootCriticalSection&) = delete;
    EnvironmentRootCriticalSection& operator=(const EnvironmentRootCriticalSection&) = delete;
    void Enter() override;
    void Leave() override;
    [[nodiscard]] bool HeldByCurrentThreadAt4076D0() const noexcept;
private:
    CRITICAL_SECTION section_;
    std::atomic<DWORD> thread_{0xFFFFFFFFu};
    std::uint32_t depth_ = 0;
};

// 创建失败只交还异常，不自动释放正文或已写的第一个 CString；见 5E0D6C unwind map。
[[nodiscard]] EnvironmentRootStorage* CreateEnvironmentRootAt41811B(
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings);
void DeleteEnvironmentRootAt4976E0(EnvironmentRootStorage& root, std::uint32_t flags,
    IEnvironmentRootMemory& memory) noexcept;

// 仅 4982AA..4982FE 的字段写入；归档清理/路径和版本替换不包含在此函数内。
void InitializeEnvironmentWindowFieldsAt4982AA(EnvironmentRootStorage& root,
    HMODULE module, HINSTANCE previousInstance, LPSTR commandLine, int showCommand);

class EnvironmentRootOwner final
{
public:
    EnvironmentRootOwner(IEnvironmentRootLock& lock,
        IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    ~EnvironmentRootOwner() noexcept;
    EnvironmentRootOwner(const EnvironmentRootOwner&) = delete;
    EnvironmentRootOwner& operator=(const EnvironmentRootOwner&) = delete;

    // 即使热路径也先加锁；解锁后重读发布槽，不返回锁内缓存值。
    [[nodiscard]] EnvironmentRootStorage* CaptureAt4180C0();
    void ReleaseAt5AB7D0() noexcept;
    [[nodiscard]] EnvironmentRootStorage* Published() const noexcept { return published_; }
    // 原6198C8的显式槽，不进行保活/加锁；仅供持有生命周期控制权的装配器使用。
    [[nodiscard]] EnvironmentRootStorage*& PublicationSlot() noexcept { return published_; }
    [[nodiscard]] IEnvironmentRootLock& LockSource() const noexcept { return lock_; }
    // 文件兼容状态属于现代owner，不进入原60h/90h/44h正文，也不随临时registry消失。
    void BindCategorizedSoundEnvironment(SaveDataEnvironmentArchive&,
        std::unique_ptr<CategorizedSoundEnvironmentAdapter>);
    void ResetCategorizedSoundEnvironment() noexcept;
    CategorizedSoundEnvironmentAdapter& RequireCategorizedSoundEnvironment(const SaveDataEnvironmentArchive&) const;
private:
    void LeaveNoexcept() noexcept;
    IEnvironmentRootLock& lock_;
    IEnvironmentRootMemory& memory_;
    IRuntimeStringManagerSource& strings_;
    EnvironmentRootStorage* published_ = nullptr;
    SaveDataEnvironmentArchive* categorizedSave_ = nullptr;
    std::unique_ptr<CategorizedSoundEnvironmentAdapter> categorizedSound_;
};
}

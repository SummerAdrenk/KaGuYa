#pragma once
#include "SoundRegistry.h"
#include <atomic>
#include <cstdint>
#include <Windows.h>
#include <mmreg.h>
#include <dsound.h>
#include <xthreads.h>

namespace KaGuYa::Sound
{
// 原根只含这五个DWORD；注册表真实持有16字节链节点。
// 两种根对象均保留原vptr/虚槽；final不允许删除virtual或改变字段偏移。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunnecessary-virtual-specifier"
#endif
class SoundSystemRoot final
{
public:
    SoundSystemRoot();
    virtual ~SoundSystemRoot() noexcept;
    IDirectSound8* device = nullptr;
    IDirectSoundBuffer* primary = nullptr;
    SoundRegistry sounds;
};
static_assert(sizeof(SoundSystemRoot) == 20);

class SoundParameterRoot final
{
public:
    virtual ~SoundParameterRoot() = default;
    virtual std::uint32_t SetFocus(std::uint32_t value) noexcept;
    virtual std::uint32_t Focus() const noexcept;
    virtual std::uint32_t SetPan(std::uint32_t value) noexcept;
    virtual std::uint32_t Pan() const noexcept;
    virtual std::uint32_t SetFrequency(std::uint32_t value) noexcept;
    virtual std::uint32_t Frequency() const noexcept;
    virtual std::uint32_t SetPosition(std::uint32_t value) noexcept;
    virtual std::uint32_t Position() const noexcept;

    std::atomic<std::int32_t> minimumVolume{-4000};
private:
    std::atomic<std::uint32_t> focus_{0}, pan_{0}, frequency_{0}, position_{0};
};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
static_assert(sizeof(SoundParameterRoot) == 24);
static_assert(std::atomic<std::uint32_t>::is_always_lock_free);

// 仅供单例冷路径；原热getter直接读字段，不经过全局锁。
class SoundColdMutex final
{
public:
    SoundColdMutex() noexcept;
    void Lock();
    void Unlock() noexcept;
private:
    _Mtx_internal_imp_t storage_{};
};
class SoundColdLock final
{
public:
    explicit SoundColdLock(SoundColdMutex& mutex) : mutex_(mutex) { mutex_.Lock(); }
    ~SoundColdLock() { mutex_.Unlock(); }
    SoundColdLock(const SoundColdLock&) = delete;
    SoundColdLock& operator=(const SoundColdLock&) = delete;
private:
    SoundColdMutex& mutex_;
};
static_assert(sizeof(SoundColdMutex) == 48);
}

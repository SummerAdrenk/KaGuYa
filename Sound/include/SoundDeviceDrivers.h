#pragma once

#include <Windows.h>
#include <cstdint>
#include <memory>

namespace KaGuYa::Sound
{
struct SoundDeviceDriver;

// 原CDirectSoundDrivers+04/+08：哨兵和计数；节点拥有driver的共享引用。
class SoundDeviceDriverList final
{
public:
    struct Node final
    {
        Node* next;
        Node* previous;
        union { std::shared_ptr<SoundDeviceDriver> driver; };
        Node() noexcept : next(this), previous(this) {}
        Node(Node* after, Node* before, const std::shared_ptr<SoundDeviceDriver>& value) noexcept
            : next(after), previous(before), driver(value) {}
        ~Node() noexcept {} // 哨兵不构造payload，普通节点由Clear显式退休。
    };

    SoundDeviceDriverList() noexcept = default;
    ~SoundDeviceDriverList();
    SoundDeviceDriverList(const SoundDeviceDriverList&) = delete;
    SoundDeviceDriverList& operator=(const SoundDeviceDriverList&) = delete;
    void Initialize();
    void Append(const std::shared_ptr<SoundDeviceDriver>& driver);
    void Clear() noexcept;

private:
    Node* head_ = nullptr;
    std::uint32_t count_ = 0;
};
static_assert(sizeof(SoundDeviceDriverList::Node) == 16);
static_assert(sizeof(SoundDeviceDriverList) == 8);

// 原默认设备根保留析构虚槽及首DWORD的vptr。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunnecessary-virtual-specifier"
#endif
class SoundDefaultDeviceRoot final
{
public:
    SoundDefaultDeviceRoot();
    virtual ~SoundDefaultDeviceRoot();
    const GUID* Query();

private:
    SoundDeviceDriverList drivers_;
    GUID defaultDevice_{};
};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
static_assert(sizeof(SoundDefaultDeviceRoot) == 28);
}

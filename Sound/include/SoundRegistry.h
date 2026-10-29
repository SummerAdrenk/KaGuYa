#pragma once

#include <cstdint>
#include <memory>

namespace KaGuYa::Sound
{
class SoundObject;

// 原CDirectSound+0C/+10：16字节双链节点，payload为对象/共享控制块两个DWORD。
class SoundRegistry final
{
public:
    struct Node final
    {
        Node* next;
        Node* previous;
        union { std::shared_ptr<SoundObject> sound; };

        Node() noexcept : next(this), previous(this) {}
        Node(Node* after, Node* before, const std::shared_ptr<SoundObject>& value) noexcept
            : next(after), previous(before), sound(value) {}
        ~Node() noexcept {} // 哨兵没有payload；普通节点由Destroy显式退休。
    };

    SoundRegistry() noexcept = default;
    ~SoundRegistry();
    SoundRegistry(const SoundRegistry&) = delete;
    SoundRegistry& operator=(const SoundRegistry&) = delete;

    void Initialize();
    Node* Begin() const noexcept { return head_->next; }
    Node* End() const noexcept { return head_; }
    bool Empty() const noexcept { return head_->next == head_; }
    Node* Find(std::uintptr_t handle) const noexcept;
    void Append(const std::shared_ptr<SoundObject>& sound);
    void Unlink(Node* node) noexcept;
    Node* DetachAll() noexcept;
    static void Destroy(Node* node) noexcept;

private:
    Node* head_ = nullptr;
    std::uint32_t count_ = 0;
};
static_assert(sizeof(SoundRegistry::Node) == 16);
static_assert(sizeof(SoundRegistry) == 8);
}

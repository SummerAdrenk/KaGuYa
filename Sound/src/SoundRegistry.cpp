#include "SoundRegistry.h"
#include <Logging/Logger.h>
#include <stdexcept>

namespace KaGuYa::Sound
{
void SoundRegistry::Initialize()
{
    if (!head_) head_ = new Node;
}
SoundRegistry::~SoundRegistry()
{
    if (!head_) return;
    auto* node = DetachAll();
    while (node != head_)
    {
        auto* next = node->next;
        Destroy(node);
        node = next;
    }
    delete head_;
}
SoundRegistry::Node* SoundRegistry::Find(const std::uintptr_t handle) const noexcept
{
    auto* node = Begin();
    while (node != head_ && reinterpret_cast<std::uintptr_t>(node->sound.get()) != handle)
        node = node->next;
    return node;
}
void SoundRegistry::Append(const std::shared_ptr<SoundObject>& sound)
{
    auto* const previous = head_->previous;
    // 10004760先分配节点并增持；10001BEC随后检查容量，再发布两条链接。
    auto* const node = new Node(head_, previous, sound);
    if (0x0FFFFFFEu - count_ < 1u)
    {
        // 原极限路径没有退未接链节点；明确安全差异，不复制泄漏。
        Destroy(node);
        Logging::Write(Logging::Level::Error, "Sound registry capacity exceeded; unpublished node retired.");
        throw std::length_error("list<T> too long");
    }
    ++count_;
    head_->previous = node;
    previous->next = node;
}
void SoundRegistry::Unlink(Node* const node) noexcept
{
    node->previous->next = node->next;
    node->next->previous = node->previous;
    --count_;
}
SoundRegistry::Node* SoundRegistry::DetachAll() noexcept
{
    auto* const first = head_->next;
    head_->next = head_;
    head_->previous = head_;
    count_ = 0;
    return first;
}
void SoundRegistry::Destroy(Node* const node) noexcept
{
    // 100030FE/1000F049：声源/控制块先退休，节点本体最后释放。
    std::destroy_at(&node->sound);
    delete node;
}
}

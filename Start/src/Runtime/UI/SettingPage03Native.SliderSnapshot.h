#pragma once
#include "SettingPage03Native.Internal.h"

namespace KaGuYa::Start::Runtime::SettingPage03NativeDetail
{
// 52E7D0的临时原map，不是配置镜像。24h头/节点、失败前缀、重复键先分配再删。
// 直接恢复4D9EE0/53B840/53B900旋转，避免当前STL改变new时机/节点大小。
struct SliderSnapshotNode final
{
    SliderSnapshotNode *left, *parent, *right;
    std::uint8_t black, nil;
    std::uint8_t unwrittenAt0E[2];
    std::int32_t key, value;
};
static_assert(sizeof(SliderSnapshotNode) == 0x18);
static_assert(offsetof(SliderSnapshotNode, key) == 0x10);
class SliderSnapshot final
{
public:
    using Node = SliderSnapshotNode;
    SliderSnapshot()
    {
        head_ = static_cast<Node*>(::operator new(0x18));
        head_->left = head_->parent = head_->right = head_;
        head_->black = head_->nil = 1; // 53B430不写0E/10/14。
    }
    ~SliderSnapshot() noexcept
    {
        Erase(head_->parent);
        head_->left = head_->parent = head_->right = head_; count_ = 0;
        ::operator delete(head_, std::size_t{0x18});
    }
    SliderSnapshot(const SliderSnapshot&) = delete;
    void Insert(int key, int value)
    {
        auto* fresh = static_cast<Node*>(::operator new(0x18));
        fresh->left = fresh->parent = fresh->right = head_;
        fresh->black = fresh->nil = 0; fresh->key = key; fresh->value = value;
        struct Raw final { Node* p; ~Raw() noexcept { if (p) ::operator delete(p, std::size_t{0x18}); } } raw{fresh};
        Node* parent = head_; Node* node = head_->parent; bool left = true;
        while (!node->nil)
        {
            parent = node;
            if (key == node->key) return;
            left = key < node->key; node = left ? node->left : node->right;
        }
        if (count_ >= 0xAAAAAA9u)
        { raw.p = nullptr; ::operator delete(fresh, std::size_t{0x18}); throw std::length_error("map/set<T> too long"); }
        ++count_; fresh->parent = parent;
        if (parent == head_) head_->parent = head_->left = head_->right = fresh;
        else if (left) { parent->left = fresh; if (head_->left == parent) head_->left = fresh; }
        else { parent->right = fresh; if (head_->right == parent) head_->right = fresh; }
        raw.p = nullptr; node = fresh;
        while (!node->parent->black)
        {
            auto* p = node->parent; auto* g = p->parent;
            if (p == g->left)
            {
                auto* uncle = g->right;
                if (!uncle->black) { p->black = uncle->black = 1; g->black = 0; node = g; }
                else
                {
                    if (node == p->right) { node = p; Left(node); }
                    node->parent->black = 1; node->parent->parent->black = 0; Right(node->parent->parent);
                }
            }
            else
            {
                auto* uncle = g->left;
                if (!uncle->black) { p->black = uncle->black = 1; g->black = 0; node = g; }
                else
                {
                    if (node == p->left) { node = p; Right(node); }
                    node->parent->black = 1; node->parent->parent->black = 0; Left(node->parent->parent);
                }
            }
        }
        head_->parent->black = 1;
    }
    Node* Begin() const noexcept { return head_->left; }
    Node* End() const noexcept { return head_; }
    static Node* Next(Node* p) noexcept
    {
        if (!p->right->nil) { p = p->right; while (!p->left->nil) p = p->left; return p; }
        auto* parent = p->parent;
        while (!parent->nil && p == parent->right) { p = parent; parent = parent->parent; }
        return parent;
    }
private:
    void Left(Node* p) noexcept
    {
        auto* r = p->right; p->right = r->left;
        if (!r->left->nil) r->left->parent = p;
        r->parent = p->parent;
        if (p == head_->parent) head_->parent = r;
        else if (p == p->parent->left) p->parent->left = r;
        else p->parent->right = r;
        r->left = p; p->parent = r;
    }
    void Right(Node* p) noexcept
    {
        auto* l = p->left; p->left = l->right;
        if (!l->right->nil) l->right->parent = p;
        l->parent = p->parent;
        if (p == head_->parent) head_->parent = l;
        else if (p == p->parent->right) p->parent->right = l;
        else p->parent->left = l;
        l->right = p; p->parent = l;
    }
    static void Erase(Node* p) noexcept
    {
        while (!p->nil)
        { Erase(p->right); auto* left = p->left; ::operator delete(p, std::size_t{0x18}); p = left; }
    }
    Node* head_;
    std::uint32_t count_ = 0;
};
}

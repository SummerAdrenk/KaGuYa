#include <Runtime/ArchiveNamespaceMapRuntime.h>

#include <mbstring.h>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Node = ArchiveNamespaceMapRuntime::Node;

int CompareKeys(const RuntimeCString& left, const RuntimeCString& right)
{
    // 493650/493682、456D10/456D2D/456DFB：只检查 Str2；不补检 Str1。
    const char* const str2 = right.GetString();
    if (!str2) throw ATL::CAtlException(E_FAIL); // 413880。
    return _mbscmp(reinterpret_cast<const unsigned char*>(left.GetString()),
        reinterpret_cast<const unsigned char*>(str2));
}

Node* PreviousNode(Node* node) noexcept
{
    // 456D9E..456DF9：包括原 iterator-- 的 head 分支，不借助另一个索引。
    if (node->isNilAt0D) return node->rightAt08;
    if (!node->leftAt00->isNilAt0D)
    {
        node = node->leftAt00;
        while (!node->rightAt08->isNilAt0D) node = node->rightAt08;
        return node;
    }
    Node* parent = node->parentAt04;
    while (!parent->isNilAt0D && node == parent->leftAt00)
    {
        node = parent;
        parent = parent->parentAt04;
    }
    if (!node->isNilAt0D) node = parent;
    return node;
}
}

ArchiveNamespaceMapRuntime::ArchiveNamespaceMapRuntime()
    : headAt00(nullptr), countAt04(0)
{
    // 458BA8..458BBA -> 48F530：先清两个槽，32-byte 分配后仅写三个链接及 0101。
    // 分离 operator new/placement，避免 /sdl 普通 new 对未写字节的额外初始化。
    void* const storage = ::operator new(sizeof(Node));
    auto* const head = ::new (storage) Node;
    head->leftAt00 = head;
    head->parentAt04 = head;
    head->rightAt08 = head;
    head->colorAt0C = 1;
    head->isNilAt0D = 1;
    headAt00 = head;
}

ArchiveNamespaceMapRuntime::~ArchiveNamespaceMapRuntime() noexcept
{
    // 45FC90，EH 5E96BC：清树、重置链接/count，最后仅释放 sentinel 原始块。
    if (headAt00) // 仅现代 owner 转移后的空源需要此分支。
    {
        Clear();
        ::operator delete(headAt00, sizeof(Node)); // 45FCCD/45FCE4传20h，不销毁head未构造的value。
    }
}

ArchiveNamespaceMapRuntime::ArchiveNamespaceMapRuntime(ArchiveNamespaceMapRuntime&& source) noexcept
    : headAt00(std::exchange(source.headAt00, nullptr)),
      countAt04(std::exchange(source.countAt04, 0))
{
}

ArchiveNamespaceMapRuntime& ArchiveNamespaceMapRuntime::operator=(
    ArchiveNamespaceMapRuntime&& source) noexcept
{
    if (this != &source)
    {
        // 仅 C++ owner 适配：目标旧树按 45FC90 释放，源树的链接和引用计数不变。
        if (headAt00)
        {
            Clear();
            ::operator delete(headAt00, sizeof(Node));
        }
        headAt00 = std::exchange(source.headAt00, nullptr);
        countAt04 = std::exchange(source.countAt04, 0);
    }
    return *this;
}

void ArchiveNamespaceMapRuntime::Clear() noexcept
{
    // 4869B7..4869E4：保存旧 head，回调全部完成后才重读成员并重置。
    Node* const head = headAt00;
    DestroySubtree(head->parentAt04);
    headAt00->parentAt04 = head;
    headAt00->leftAt00 = head;
    headAt00->rightAt08 = head;
    countAt04 = 0;
}

void ArchiveNamespaceMapRuntime::DestroyNode(Node* node) noexcept
{
    // 457BE0/48FCD0，EH 5EB8D4/5E9688：Ref 回调抛出立即 541E55/terminate，
    // 不继续释放 key 或 raw block。ArchiverInfoReference 析构保留重读 control 的语义。
    node->valueAt14.~ArchiverInfoReference();
    node->keyAt10.~RuntimeCString();
    ::operator delete(node, sizeof(Node)); // 48FD6A：原节点删除宽度20h。
}

void ArchiveNamespaceMapRuntime::DestroySubtree(Node* node) noexcept
{
    // 48FD02..48FD7B：右递归结束后才读取 left，再释放当前，最后迭代 left。
    while (!node->isNilAt0D)
    {
        DestroySubtree(node->rightAt08);
        Node* const left = node->leftAt00;
        DestroyNode(node);
        node = left;
    }
}

ArchiveNamespaceMapRuntime::Node* ArchiveNamespaceMapRuntime::Find(const RuntimeCString& key) const
{
    Node* candidate = headAt00;
    Node* cursor = candidate->parentAt04;
    while (!cursor->isNilAt0D)
    {
        if (CompareKeys(cursor->keyAt10, key) < 0)
            cursor = cursor->rightAt08;
        else
        {
            candidate = cursor;
            cursor = cursor->leftAt00;
        }
    }
    // 49367E..49369A：空树不比较；lower_bound 后反向比较，未命中返回 head。
    if (candidate == headAt00 || CompareKeys(key, candidate->keyAt10) < 0)
        return headAt00;
    return candidate;
}

ArchiveNamespaceMapRuntime::Node* ArchiveNamespaceMapRuntime::CreateNode(
    const RuntimeCString& key, const YtlReferenceSlotRuntime& source)
{
    // 48F580：分配失败尚未进入 4520D0 的 try，不做任何回收。
    void* const storage = ::operator new(sizeof(Node));
    auto* const node = ::new (storage) Node;
    node->leftAt00 = headAt00;
    node->parentAt04 = headAt00;
    node->rightAt08 = headAt00;
    try
    {
        node->colorAt0C = 0;
        node->isNilAt0D = 0;
        // 452123 -> 414540：先 CloneData；共享/locked/换 manager 分支沿用 RuntimeCString。
        ::new (static_cast<void*>(&node->keyAt10)) RuntimeCString(key);
        ::new (static_cast<void*>(&node->valueAt14)) ArchiverInfoReference;
        node->valueAt14.reference = source;
        if (node->valueAt14.reference.controlAt08)
            ++node->valueAt14.reference.controlAt08->referenceCountAt04;
    }
    catch (...)
    {
        // 5EBC94：try state 0，catch-all=452160 -> 48FDA0；没有 value/key unwind 项。
        ::operator delete(node, sizeof(Node)); // 48FDA3：构造失败也传20h。
        throw;
    }
    return node;
}

std::pair<ArchiveNamespaceMapRuntime::Node*, bool> ArchiveNamespaceMapRuntime::Insert(
    const RuntimeCString& key, const YtlReferenceSlotRuntime& source)
{
    return InsertNode(CreateNode(key, source), false); // 45927D、45928A、459293。
}

std::pair<ArchiveNamespaceMapRuntime::Node*, bool> ArchiveNamespaceMapRuntime::InsertNode(
    Node* node, bool preferLeftOnEqual)
{
    try
    {
        Node* parent = headAt00;
        Node* cursor = parent->parentAt04;
        bool insertLeft = true;
        while (!cursor->isNilAt0D)
        {
            parent = cursor;
            // 456D08..456D48：唯一已见调用传 0；保留 helper 的非零比较方向。
            insertLeft = preferLeftOnEqual ?
                CompareKeys(cursor->keyAt10, node->keyAt10) >= 0 :
                CompareKeys(node->keyAt10, cursor->keyAt10) < 0;
            cursor = insertLeft ? cursor->leftAt00 : cursor->rightAt08;
        }

        Node* predecessor = parent;
        if (insertLeft)
        {
            if (parent == headAt00->leftAt00)
                return {LinkNode(node, parent, true), true}; // 456D69..456D86，也含空树。
            predecessor = PreviousNode(parent);
        }
        if (CompareKeys(predecessor->keyAt10, node->keyAt10) < 0)
            return {LinkNode(node, parent, insertLeft), true};

        // 456E21..456E43：先 Ref、再 CString、再 raw delete；旧节点及 count 不变。
        DestroyNode(node);
        return {predecessor, false};
    }
    catch (...)
    {
        // 5EBCEC/5EBCD8：整个 state 0 都在 catch-all 内，handler=456E5B -> 48FCA0。
        // 安全差异：LinkNode 超限不预先释放，候选由这里单次清理，避免原故障路径的 UAF。
        DestroyNode(node);
        throw;
    }
}

ArchiveNamespaceMapRuntime::Node* ArchiveNamespaceMapRuntime::LinkNode(
    Node* node, Node* parent, bool insertLeft)
{
    // 456B68..456B70：unsigned >= 0x07FFFFFE；duplicate 路径不检查容量。
    if (countAt04 >= 0x07FFFFFEu)
    {
        // 已记录的原故障路径安全差异，不计完全一致：省略 456CAE 的预先释放，
        // 由 InsertNode catch 单次清理；length_error 构造自身失败也沿用同一清理路径。
        throw std::length_error("map/set<T> too long"); // 53F941；count/链接尚未改动。
    }
    ++countAt04; // 456B79：先 count，再 parent/link，不提前修复颜色。
    node->parentAt04 = parent;
    if (parent == headAt00)
    {
        headAt00->parentAt04 = node;
        headAt00->leftAt00 = node;
        headAt00->rightAt08 = node;
    }
    else if (insertLeft)
    {
        parent->leftAt00 = node;
        if (parent == headAt00->leftAt00) headAt00->leftAt00 = node;
    }
    else
    {
        parent->rightAt08 = node;
        if (parent == headAt00->rightAt08) headAt00->rightAt08 = node;
    }

    Node* cursor = node;
    while (cursor->parentAt04->colorAt0C == 0)
    {
        Node* const up = cursor->parentAt04;
        Node* const grandparent = up->parentAt04;
        if (up == grandparent->leftAt00)
        {
            Node* const uncle = grandparent->rightAt08;
            if (uncle->colorAt0C == 0)
            {
                up->colorAt0C = 1;
                uncle->colorAt0C = 1;
                cursor->parentAt04->parentAt04->colorAt0C = 0;
                cursor = cursor->parentAt04->parentAt04;
            }
            else
            {
                if (cursor == up->rightAt08)
                {
                    cursor = up;
                    RotateLeft(cursor);
                }
                cursor->parentAt04->colorAt0C = 1;
                cursor->parentAt04->parentAt04->colorAt0C = 0;
                RotateRight(cursor->parentAt04->parentAt04);
            }
        }
        else
        {
            Node* const uncle = grandparent->leftAt00;
            if (uncle->colorAt0C == 0)
            {
                up->colorAt0C = 1;
                uncle->colorAt0C = 1;
                cursor->parentAt04->parentAt04->colorAt0C = 0;
                cursor = cursor->parentAt04->parentAt04;
            }
            else
            {
                if (cursor == up->leftAt00)
                {
                    cursor = up;
                    RotateRight(cursor);
                }
                cursor->parentAt04->colorAt0C = 1;
                cursor->parentAt04->parentAt04->colorAt0C = 0;
                RotateLeft(cursor->parentAt04->parentAt04); // 456C4B..456C86 的内联左旋。
            }
        }
    }
    headAt00->parentAt04->colorAt0C = 1; // 456C96..456C9D：最后才将 root 置黑。
    return node;
}

void ArchiveNamespaceMapRuntime::RotateLeft(Node* node) noexcept
{
    // 48FFA0：不改 nil 子节点的 parent；root 链接优先于父节点的左右链接。
    Node* const right = node->rightAt08;
    node->rightAt08 = right->leftAt00;
    if (!right->leftAt00->isNilAt0D) right->leftAt00->parentAt04 = node;
    right->parentAt04 = node->parentAt04;
    if (node == headAt00->parentAt04)
        headAt00->parentAt04 = right;
    else if (node == node->parentAt04->leftAt00)
        node->parentAt04->leftAt00 = right;
    else
        node->parentAt04->rightAt08 = right;
    right->leftAt00 = node;
    node->parentAt04 = right;
}

void ArchiveNamespaceMapRuntime::RotateRight(Node* node) noexcept
{
    // 490000：先测父节点 right，与左旋原顺序镜像。
    Node* const left = node->leftAt00;
    node->leftAt00 = left->rightAt08;
    if (!left->rightAt08->isNilAt0D) left->rightAt08->parentAt04 = node;
    left->parentAt04 = node->parentAt04;
    if (node == headAt00->parentAt04)
        headAt00->parentAt04 = left;
    else if (node == node->parentAt04->rightAt08)
        node->parentAt04->rightAt08 = left;
    else
        node->parentAt04->leftAt00 = left;
    left->rightAt08 = node;
    node->parentAt04 = left;
}

ArchiveNamespaceMapRuntime::Node* ArchiveNamespaceMapRuntime::Next(Node* node) noexcept
{
    // 4666A0：原实现没有针对 end/null 的提前返回；调用方在前进前检查 End()。
    Node* next = node->rightAt08;
    if (next->isNilAt0D)
    {
        next = node->parentAt04;
        while (!next->isNilAt0D && node == next->rightAt08)
        {
            node = next;
            next = next->parentAt04;
        }
    }
    else
    {
        while (!next->leftAt00->isNilAt0D) next = next->leftAt00;
    }
    return next;
}
}

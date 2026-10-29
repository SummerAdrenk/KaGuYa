#pragma once

#include <Runtime/ArchiverInfoReference.h>
#include <Runtime/RuntimeCString.h>

#include <cstddef>
#include <cstdint>
#include <utility>

namespace KaGuYa::Start::Runtime
{
// 458B93/458BA8：嵌入 FileManager+04/+08 的原生树；仅提供 C++ 接口，不宣称原 extern ABI。
class ArchiveNamespaceMapRuntime final
{
public:
    struct Node final
    {
        Node* leftAt00;
        Node* parentAt04;
        Node* rightAt08;
        std::uint8_t colorAt0C; // 0=red，1=black。
        std::uint8_t isNilAt0D;
        std::byte unwrittenAt0E[2];
        // 48F530：head 不构造这两个成员，+0E..+1F 保留分配器原字节。
        union { RuntimeCString keyAt10; };
        union { ArchiverInfoReference valueAt14; };

        Node() noexcept {}
        ~Node() noexcept {} // 成员寿命由树按 4520D0/457BE0 管理。
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
    };

    ArchiveNamespaceMapRuntime();
    ~ArchiveNamespaceMapRuntime() noexcept;
    ArchiveNamespaceMapRuntime(const ArchiveNamespaceMapRuntime&) = delete;
    ArchiveNamespaceMapRuntime& operator=(const ArchiveNamespaceMapRuntime&) = delete;
    // 仅 C++ owner 转移，非原导出 ABI：不创建 head/Ref、不复制节点。
    // 源置为 head=null/count=0，仅用于析构、重新赋值及 size/empty/End 查询。
    // move assignment 先按原析构顺序释放目标旧树，然后接收源树。
    ArchiveNamespaceMapRuntime(ArchiveNamespaceMapRuntime&& source) noexcept;
    ArchiveNamespaceMapRuntime& operator=(ArchiveNamespaceMapRuntime&& source) noexcept;

    // 4869B7..4869E4：右子树、当前、左子树；完成后重置链接/count，保留 head。
    void Clear() noexcept;
    [[nodiscard]] Node* Find(const RuntimeCString& key) const;
    // 4520D0 -> 456CC0(arg_4=0)：先复制候选，再比较/查重；不接管 source。
    // 不含 459246..459272 调用方临时 pair 的构造；返回 pair 也不是原返回 ABI。
    // 已记录的安全差异，非完全一致：超限保留原阈值/length_error，但候选仅由外层 catch
    // 清理一次，不复现 456CAB/456E5B 原故障路径的重复释放。
    [[nodiscard]] std::pair<Node*, bool> Insert(const RuntimeCString& key,
        const YtlReferenceSlotRuntime& source);

    [[nodiscard]] Node* Begin() const noexcept { return headAt00->leftAt00; }
    [[nodiscard]] Node* End() const noexcept { return headAt00; }
    // 4891E1 -> 4666A0：直接沿原链接前进，不分配、快照或重新排序。
    [[nodiscard]] static Node* Next(Node* node) noexcept;
    [[nodiscard]] std::uint32_t size() const noexcept { return countAt04; }
    [[nodiscard]] bool empty() const noexcept { return countAt04 == 0; }

    Node* headAt00;
    std::uint32_t countAt04;

private:
    [[nodiscard]] Node* CreateNode(const RuntimeCString& key, const YtlReferenceSlotRuntime& source);
    [[nodiscard]] std::pair<Node*, bool> InsertNode(Node* node, bool preferLeftOnEqual);
    [[nodiscard]] Node* LinkNode(Node* node, Node* parent, bool insertLeft);
    void RotateLeft(Node* node) noexcept;
    void RotateRight(Node* node) noexcept;
    static void DestroyNode(Node* node) noexcept;
    static void DestroySubtree(Node* node) noexcept;
};

static_assert(sizeof(ArchiveNamespaceMapRuntime) == 8);
static_assert(offsetof(ArchiveNamespaceMapRuntime, headAt00) == 0);
static_assert(offsetof(ArchiveNamespaceMapRuntime, countAt04) == 4);
static_assert(sizeof(ArchiveNamespaceMapRuntime::Node) == 0x20);
static_assert(alignof(ArchiveNamespaceMapRuntime::Node) == 4);
static_assert(offsetof(ArchiveNamespaceMapRuntime::Node, leftAt00) == 0x00);
static_assert(offsetof(ArchiveNamespaceMapRuntime::Node, parentAt04) == 0x04);
static_assert(offsetof(ArchiveNamespaceMapRuntime::Node, rightAt08) == 0x08);
static_assert(offsetof(ArchiveNamespaceMapRuntime::Node, colorAt0C) == 0x0C);
static_assert(offsetof(ArchiveNamespaceMapRuntime::Node, isNilAt0D) == 0x0D);
static_assert(offsetof(ArchiveNamespaceMapRuntime::Node, unwrittenAt0E) == 0x0E);
static_assert(offsetof(ArchiveNamespaceMapRuntime::Node, keyAt10) == 0x10);
static_assert(offsetof(ArchiveNamespaceMapRuntime::Node, valueAt14) == 0x14);
}

#include <Runtime/ArchiveDirectoryRuntime.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
// MSVC x86 的实际成员布局；不靠额外字段或影子 vector 凑尺寸。
struct ArchiveDirectoryRuntimeLayout
{
    static_assert(offsetof(ArchiveNodeRuntime, nameAt04_) == 0x04);
    static_assert(offsetof(ArchiveFileNodeRuntime, titleAt08_) == 0x08);
    static_assert(offsetof(ArchiveFileNodeRuntime, offsetAt10) == 0x10);
    static_assert(offsetof(ArchiveFileNodeRuntime, lengthAt18) == 0x18);
    static_assert(offsetof(ArchiveFileNodeRuntime, flagsAt1C) == 0x1C);
    static_assert(offsetof(ArchiveFolderNodeRuntime, childrenAt08_) == 0x08);
};

namespace
{
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumReferences = 0x15555555u;

ATL::IAtlStringMgr* RequireManager(IRuntimeStringManagerSource& strings)
{
    auto* manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL); // 45EF9F/45EFA9/45F02E
    return manager;
}

std::uint32_t Distance(const Slot* begin, const Slot* end) noexcept
{
    // 45328F/461F75：DWORD 地址差后有符号除以 12；避免对空指针作 C++ 指针减法。
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

Slot* AllocateStorage(std::uint32_t capacity)
{
    const auto bytes = capacity <= MaximumReferences ? capacity * 12u : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
        return bytes ? static_cast<Slot*>(::operator new(bytes)) : nullptr;

    // 45330E..453330：多分配 0x23，32-byte 对齐，并在 aligned-4 保存 raw。
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* raw = ::operator new(request);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto address = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(address - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Slot*>(address);
}

void DeleteStorage(Slot* begin, std::uint32_t capacity) noexcept
{
    const auto bytes = capacity * 12u;
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::uintptr_t allocation;
        std::memcpy(&allocation, reinterpret_cast<const void*>(address - 4u), sizeof(allocation));
        if (address - allocation - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
        raw = reinterpret_cast<void*>(allocation);
    }
    // 461F80..461FA2 / 491780：原传 bytes（大块为 bytes+0x23），53FBFB 忽略 size。
    ::operator delete(raw);
}

void ReleaseRange(Slot* begin, Slot* end) noexcept
{
    // 452830 / EH 5EBA84：正序，正文回调后重读 control，释放异常进入 terminate。
    for (auto* cursor = begin; cursor != end; ++cursor) ReleaseYtlReference(*cursor);
}

void CopyRange(const Slot* begin, const Slot* end, Slot* destination) noexcept
{
    for (; begin != end; ++begin, ++destination) CopyArchiveReference(*destination, *begin);
}

// 不给正文添加析构虚槽，也不在正文释放时改写其 vptr。原控制块直接按具体类型析构成员。
void DestroyBody(ArchiveFileNodeRuntime* body)
{
    if (!body) return;
    std::destroy_at(&body->TitleAt08()); // 493178..493193：Title 先于 Name
    std::destroy_at(&body->NameAt04());
    ::operator delete(body); // 4931AF..4931B2，0x20；53FBFB 不使用 size。
}

void DestroyBody(ArchiveFolderNodeRuntime* body)
{
    if (!body) return;
    body->ChildrenAt08().DestroyStorage(); // 4931C8..4931CB：目录先于 Name
    std::destroy_at(&body->NameAt04());
    ::operator delete(body); // 4931EB..4931EE，0x14
}

template<class T>
class ArchiveNodeControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ArchiveNodeControl(T* body) noexcept : EnvironmentObjectReferenceControl(body) {}
    void DestroyPayloadAtVslot00() override { DestroyBody(static_cast<T*>(payloadAt14)); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 468280/4682B0：即使 flags=0 也退化到 ref_object 虚表，保留其余20字节。
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(ArchiveNodeControl<ArchiveFileNodeRuntime>) == 0x18);
static_assert(sizeof(ArchiveNodeControl<ArchiveFolderNodeRuntime>) == 0x18);

void ReleaseCapturedControl(YtlReferenceControlRuntime* control)
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1); // 49471F/4947CA/494B76：仍是捕获的 control。
    }
}

template<class T>
void ReplaceReference(Slot& reference, T* body, YtlReferenceControlRuntime* previous)
{
    // 不调用 ReleaseYtlReference：494709/4947B4 的正常旧引用释放不重读槽。
    if (previous)
    {
        ReleaseCapturedControl(previous); // 可抛；由调用者的局部 Ref EH 负责重读槽清理。
        previous = nullptr;
        reference.controlAt08 = nullptr;
        reference.baseAt00 = 0;
    }
    if (!body) return;
    // 49473D/4947E4 的第二次检查仍针对 esi，不改为重读可能被回调改写的槽。
    if (previous) previous->DeleteControlAtVslot04(1);
    reference.biasAt04 = 0;
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    // 49475B/494802分配后只构造已知字段；普通new会被v143 /sdl额外整块清零。
    void* storage = ::operator new(sizeof(ArchiveNodeControl<T>));
    auto* control = storage ? ::new (storage) ArchiveNodeControl<T>(body) : nullptr;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = sizeof(T);
    control->countAt10 = 1;
    reference.controlAt08 = control; // 494835..49483C；分配失败留下非零 base/null control。
}
}

ArchiveNodeRuntime::ArchiveNodeRuntime(IRuntimeStringManagerSource& strings)
    : nameAt04_(RequireManager(strings)) {}

ArchiveFileNodeRuntime::ArchiveFileNodeRuntime(IRuntimeStringManagerSource& strings)
    : ArchiveNodeRuntime(strings) {}

ArchiveFolderNodeRuntime::ArchiveFolderNodeRuntime(IRuntimeStringManagerSource& strings)
    : ArchiveNodeRuntime(strings) {}

ArchiveFileNodeRuntime* CreateArchiveFileBody(IRuntimeStringManagerSource& strings)
{
    void* raw = ::operator new(sizeof(ArchiveFileNodeRuntime));
    if (!raw) return nullptr; // 4946F2/493E51：原 body 分配返回空时不构造。
    auto* body = ::new (raw) ArchiveFileNodeRuntime(strings);
    // 45EF77：第二次独立捕获。EH 5E89A8 无清理项，失败不销毁 Name 或释放 raw。
    std::construct_at(&body->titleAt08_, RequireManager(strings));
    return body;
}

ArchiveFolderNodeRuntime* CreateArchiveFolderBody(IRuntimeStringManagerSource& strings)
{
    void* raw = ::operator new(sizeof(ArchiveFolderNodeRuntime));
    if (!raw) return nullptr;
    return ::new (raw) ArchiveFolderNodeRuntime(strings); // 45F009..45F017 清三个指针。
}

void ReplaceArchiveFileReference(Slot& reference, ArchiveFileNodeRuntime* body)
{
    ReplaceReference(reference, body, reference.controlAt08);
}

void ReplaceArchiveFolderReference(Slot& reference, ArchiveFolderNodeRuntime* body)
{
    ReplaceReference(reference, body, reference.controlAt08);
}

void ReplaceArchiveFileReference(Slot& reference, ArchiveFileNodeRuntime* body,
    YtlReferenceControlRuntime* capturedPreviousControl)
{
    ReplaceReference(reference, body, capturedPreviousControl);
}

void ReplaceArchiveFolderReference(Slot& reference, ArchiveFolderNodeRuntime* body,
    YtlReferenceControlRuntime* capturedPreviousControl)
{
    ReplaceReference(reference, body, capturedPreviousControl);
}

void CopyArchiveReference(Slot& destination, const Slot& source) noexcept
{
    destination.baseAt00 = source.baseAt00;
    destination.biasAt04 = source.biasAt04;
    destination.controlAt08 = source.controlAt08;
    if (destination.controlAt08) ++destination.controlAt08->referenceCountAt04;
}

ArchiveNodeRuntime* ResolveArchiveNode(const Slot& reference) noexcept
{
    // 493780..4937A0 / 494920..494933：base 为零才返回 null，control 为空时 stride=0。
    return ResolveEnvironmentObjectReference<ArchiveNodeRuntime>(reference);
}

void ReleaseArchiveLocalReference(const Slot& reference) noexcept
{
    ReleaseArchiveLocalReference(reference.controlAt08);
}

void ReleaseArchiveLocalReference(YtlReferenceControlRuntime* capturedControl) noexcept
{
    // EH 5E8B2C/5E8A78：正常局部析构失败必须 terminate，不重走 EH 释放。
    ReleaseCapturedControl(capturedControl);
}

void ReleaseArchiveDirectLocalReference(Slot& reference) noexcept
{
    ReleaseArchiveDirectLocalReference(reference, reference.controlAt08);
}

void ReleaseArchiveDirectLocalReference(Slot& reference, YtlReferenceControlRuntime* capturedControl) noexcept
{
    if (!capturedControl) return;
    ReleaseCapturedControl(capturedControl); // EH 5E8A60：同样 terminate。
    reference.controlAt08 = nullptr;
    reference.baseAt00 = 0;
}

void ReleaseArchiveReferenceOnUnwind(Slot& reference) noexcept
{
    ReleaseYtlReference(reference); // 460D8C：正文回调后重读；bias 始终保留。
}

ArchiveDirectoryRuntime::~ArchiveDirectoryRuntime() noexcept { DestroyStorage(); }

void ArchiveDirectoryRuntime::Append(const Slot& source)
{
    auto* end = endAt04;
    if (end != capacityAt08)
    {
        CopyArchiveReference(*end, source);
        ++endAt04; // 494B0B/493A01：复制及加计数完成后才发布 end。
        return;
    }
    GrowAndInsert(end, source);
}

void ArchiveDirectoryRuntime::GrowAndInsert(Slot* position, const Slot& source)
{
    const auto index = Distance(beginAt00, position);
    const auto oldSize = Distance(beginAt00, endAt04);
    if (oldSize == MaximumReferences) throw std::length_error("vector<T> too long"); // 490F20
    const auto required = oldSize + 1u;
    const auto capacity = Distance(beginAt00, capacityAt08);
    // 4532D5..4532EE：1.5 倍增长；溢出分支退到 required，不饱和到 max_size。
    const auto grown = capacity > MaximumReferences - capacity / 2u ? required : capacity + capacity / 2u;
    const auto nextCapacity = (std::max)(required, grown);
    auto* next = AllocateStorage(nextCapacity);
    auto* inserted = next + index;
    auto* cleanupBegin = inserted;
    try
    {
        CopyArchiveReference(*inserted, source); // 45336A：必须早于旧范围复制，支持别名。
        if (position == endAt04)
            CopyRange(beginAt00, endAt04, next);
        else
        {
            // 4533DB..4533F4：原辅助函数的非尾分支；分配期间 end 也可能被回调改写。
            CopyRange(beginAt00, position, next);
            cleanupBegin = next;
            CopyRange(position, endAt04, inserted + 1);
        }
    }
    catch (...)
    {
        // 453483..4534A3 / EH 5E8CEC：只覆盖构造阶段，不覆盖旧存储释放。
        // 三 DWORD 复制本身不抛 C++ 异常；不增加虚构的可抛钩子。
        ReleaseRange(cleanupBegin, inserted + 1);
        DeleteStorage(next, nextCapacity);
        throw;
    }
    if (beginAt00)
    {
        ReleaseRange(beginAt00, endAt04);
        DeleteStorage(beginAt00, Distance(beginAt00, capacityAt08));
    }
    // 453454..453467：旧范围释放完毕才提交三个指针，不用 move 偷换引用计数时序。
    beginAt00 = next;
    endAt04 = next + required;
    capacityAt08 = next + nextCapacity;
}

void ArchiveDirectoryRuntime::DestroyStorage() noexcept
{
    if (!beginAt00) return; // 461F58：begin 为空时另两个槽也不改写。
    ReleaseRange(beginAt00, endAt04);
    // 461F66：回调返回后重新读取 begin/capacity。
    DeleteStorage(beginAt00, Distance(beginAt00, capacityAt08));
    beginAt00 = nullptr;
    endAt04 = nullptr;
    capacityAt08 = nullptr;
}

std::size_t ArchiveDirectoryRuntime::size() const noexcept { return Distance(beginAt00, endAt04); }
std::size_t ArchiveDirectoryRuntime::Capacity() const noexcept { return Distance(beginAt00, capacityAt08); }
std::span<Slot> ArchiveDirectoryRuntime::References() noexcept { return {beginAt00, size()}; }
std::span<const Slot> ArchiveDirectoryRuntime::References() const noexcept { return {beginAt00, size()}; }

std::uint32_t ArchiveFileNodeRuntime::IsFile() const noexcept { return 1; }
std::uint32_t ArchiveFolderNodeRuntime::IsFile() const noexcept { return 0; }
std::uint32_t ArchiveFileNodeRuntime::SerializedSize() const noexcept
{
    return static_cast<std::uint32_t>(NameAt04().GetLength()) + 0x11u;
}
std::uint32_t ArchiveFolderNodeRuntime::SerializedSize() const noexcept
{
    return static_cast<std::uint32_t>(NameAt04().GetLength()) + 7u;
}

std::int32_t ArchiveFileNodeRuntime::Write(IArchiveDirectoryWriterRuntime& writer) const
{
    // 48F300：逐次短路，失败抛 int 0..5；名称只写截成 WORD 的长度，不加工具侧校验。
    const std::uint8_t isFile = 1;
    if (!writer.Write(&isFile, 1)) throw 0;
    const auto length = static_cast<std::uint16_t>(NameAt04().GetLength());
    if (!writer.Write(&length, 2)) throw 1;
    if (!writer.Write(NameAt04().GetString(), length)) throw 2;
    if (!writer.Write(&offsetAt10, 8)) throw 3;
    if (!writer.Write(&lengthAt18, 4)) throw 4;
    const auto result = writer.Write(&flagsAt1C, 2);
    if (!result) throw 5;
    return result; // 48F37C..48F388：保留完整 EAX，不归一化为 bool。
}

std::int32_t ArchiveFolderNodeRuntime::Write(IArchiveDirectoryWriterRuntime& writer) const
{
    // 48F410：先写本节点，再按混合 Ref 的实际顺序递归，不按类型重排。
    const std::uint8_t isFile = 0;
    if (!writer.Write(&isFile, 1)) throw 0;
    const auto length = static_cast<std::uint16_t>(NameAt04().GetLength());
    if (!writer.Write(&length, 2)) throw 1;
    if (!writer.Write(NameAt04().GetString(), length)) throw 2;
    const auto count = static_cast<std::uint32_t>(childrenAt08_.size());
    auto result = writer.Write(&count, 4);
    if (!result) throw 3;
    for (auto* cursor = childrenAt08_.beginAt00; cursor != childrenAt08_.endAt04; ++cursor)
        result = ResolveArchiveNode(*cursor)->Write(writer);
    return result; // 48F4CE：空目录返回 count 写入结果，否则返回最后子节点结果。
}
}

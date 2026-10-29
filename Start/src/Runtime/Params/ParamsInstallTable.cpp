#include <Runtime/ParamsInstallTable.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
static_assert(sizeof(std::uintptr_t) == 4);
static_assert(alignof(ParamsInstalledFile) <= alignof(std::uint32_t));

std::uint32_t AllocationBytes(std::uint32_t count) noexcept
{
    // 41B02D..41B049：乘法或加 cookie 溢出均饱和到 UINT_MAX，仍交给 new[]。
    constexpr auto maximum = (std::numeric_limits<std::uint32_t>::max)();
    return count > (maximum - 4u) / 8u ? maximum : count * 8u + 4u;
}

ATL::IAtlStringMgr* RequireManager(IRuntimeStringManagerSource& strings)
{
    auto* manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL); // 40C8C2 / 40C8CC
    return manager;
}

void ConstructAt40C860(ParamsInstalledFile* element, IRuntimeStringManagerSource& strings) noexcept
{
    // 实读 memory/005AD494--00601000.txt:12887..12889：FuncInfo=5DF994，EHFlags(5DF9B4)=5。
    // 含 FI_EHNOEXCEPT_FLAG；任一构造失败 terminate，不回到 iterator 清理已完成前缀。
    std::construct_at(&element->file, RequireManager(strings)); // 40C885，每串单独取 manager。
    std::construct_at(&element->media, RequireManager(strings)); // 40C89A
}

// 独立的 _install 数组控制块布局，不混用 SystemDataInstalledFiles 的具体动态类型。
class ParamsInstallControl : public YtlReferenceControlRuntime
{
public:
    std::uint32_t strideAt0C;
    std::uint32_t countAt10;
    ParamsInstalledFile* payloadAt14;
protected:
    explicit ParamsInstallControl(ParamsInstalledFile* payload) noexcept : payloadAt14(payload) {}
};
static_assert(sizeof(ParamsInstallControl) == 24);

void RetireInstallControl(ParamsInstallControl* control, std::uint32_t flags) noexcept;

class RetiredInstallControl final : public ParamsInstallControl
{
public:
    RetiredInstallControl() noexcept : ParamsInstallControl(nullptr) {}
    // 原 5ADD18 的 slot00=541C5F purecall，slot04=412460；不再拥有正文析构行为。
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireInstallControl(this, flags); }
};
static_assert(sizeof(RetiredInstallControl) == 24);

void RetireInstallControl(ParamsInstallControl* control, std::uint32_t flags) noexcept
{
    // 4110EA 的立即数实读为 5ADD18：flags=0 也退化虚表，保留其余20字节。
    // 与现有 Ref 退休口一致，用 placement 更换动态类型，不硬写编译器 vptr。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* retired = ::new (static_cast<void*>(control)) RetiredInstallControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    if (flags & 1u) ::operator delete(retired); // 4110F5 -> 53FBFB 忽略 size。
}

class InstallArrayControl final : public ParamsInstallControl
{
public:
    explicit InstallArrayControl(ParamsInstalledFile* elements) noexcept : ParamsInstallControl(elements) {}

    void DestroyPayloadAtVslot00() override
    {
        // 实读 41B0AB 的立即数=5AE388；memory:5AE388/5AE38C={43C2F0,4110E0}。
        // 43C31F 从 cookie 取数，53FAB4 逆序逐项；40FBE0 每项先 media(+04) 再 file(+00)。
        auto* const elements = payloadAt14;
        if (!elements) return;
        auto* const cookie = reinterpret_cast<std::uint32_t*>(elements) - 1;
        auto count = *cookie;
        while (count) std::destroy_at(elements + --count);
        // 43C334 在字符串回调后重读 cookie；释放大小按 DWORD 回绕，不使用 control.count。
        const std::uint32_t bytes = *cookie * 8u + 4u;
        ::operator delete[](cookie, static_cast<std::size_t>(bytes));
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireInstallControl(this, flags); }
};
static_assert(sizeof(InstallArrayControl) == 24);

const ParamsInstalledFile* ResolveInstalledFile(const YtlReferenceSlotRuntime& reference,
    std::uint32_t index) noexcept
{
    if (!reference.baseAt00) return nullptr;
    const auto stride = reference.controlAt08 ?
        static_cast<const ParamsInstallControl*>(reference.controlAt08)->strideAt0C : 0u;
    const std::uint32_t offset = (reference.biasAt04 + index) * stride;
    return reinterpret_cast<const ParamsInstalledFile*>(reference.baseAt00 + offset);
}

void SetSnapshotString(RuntimeCString& destination, const std::string& bytes)
{
    if (bytes.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params install snapshot exceeds CString length");
    destination.SetString(bytes.data(), static_cast<int>(bytes.size()));
}
}

ParamsInstallTable::~ParamsInstallTable() noexcept { Reset(); }

ParamsInstallTable::ParamsInstallTable(ParamsInstallTable&& other) noexcept
    : reference(std::exchange(other.reference, YtlReferenceSlotRuntime{})) {}

ParamsInstallTable& ParamsInstallTable::operator=(ParamsInstallTable&& other)
{
    if (this != &other)
    {
        Reset();
        reference = std::exchange(other.reference, YtlReferenceSlotRuntime{});
    }
    return *this;
}

void ParamsInstallTable::Reset()
{
    // 41B009 重读槽内 control；无 control 不清 base/bias，有 control 也不额外清 bias。
    ReleaseYtlReference(reference);
}

void ParamsInstallTable::RebuildAt41B025(std::uint32_t count, IRuntimeStringManagerSource& strings)
{
    Reset();
    if (count == 0) return;
    auto* const cookie = static_cast<std::uint32_t*>(::operator new[](AllocationBytes(count)));
    ParamsInstalledFile* elements = nullptr;
    if (cookie)
    {
        *cookie = count; // 41B063：4-byte cookie，正文从 +4 开始。
        elements = reinterpret_cast<ParamsInstalledFile*>(cookie + 1);
        for (std::uint32_t completed = 0; completed < count; ++completed)
            ConstructAt40C860(elements + completed, strings);
    }
    // 41B075 的 null 数组分支仍创建 control；不能在此伪造分配成功或提前改变发布次序。
    // 41B07C：分配/构造回调可改槽，仅虚删当时的 control，不再减计数或析构其正文。
    if (reference.controlAt08) reference.controlAt08->DeleteControlAtVslot04(1);
    reference.biasAt04 = 0;
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(elements);

    // 41B099 才分配 control；抛出时保留已发布 base/bias 和当时的 control 槽，不回滚。
    void* const storage = ::operator new(sizeof(InstallArrayControl));
    reference.controlAt08 = storage ? ::new (storage) InstallArrayControl(elements) : nullptr;
    // 安全差异：原 41B0BB 对 null control 直接写 [0+4]；这里发布 null 后显式报错。
    if (!reference.controlAt08) throw std::bad_alloc{};
    reference.controlAt08->referenceCountAt04 = 1;
    reference.controlAt08->owningAt08 = 1;
    static_cast<ParamsInstallControl*>(reference.controlAt08)->strideAt0C = 8;
    static_cast<ParamsInstallControl*>(reference.controlAt08)->countAt10 = count;
}

std::size_t ParamsInstallTable::size() const noexcept
{
    // 41B0DB..41B0E9：只读 control+10，不从 cookie/base 推断，不减 bias。
    return reference.controlAt08 ?
        static_cast<const ParamsInstallControl*>(reference.controlAt08)->countAt10 : 0u;
}

ParamsInstalledFile& ParamsInstallTable::At(std::uint32_t index)
{
    return const_cast<ParamsInstalledFile&>(std::as_const(*this).At(index));
}

const ParamsInstalledFile& ParamsInstallTable::At(std::uint32_t index) const
{
    const auto* const element = ResolveInstalledFile(reference, index);
    if (!element) throw std::logic_error("41B125: Params install reference resolves to null");
    return *element;
}

ParamsInstallTable::ParamsInstallTable(std::initializer_list<ParamsInstallEntry> values)
{
    AssignSnapshot(std::span<const ParamsInstallEntry>(values.begin(), values.size()));
}

ParamsInstallTable& ParamsInstallTable::operator=(std::initializer_list<ParamsInstallEntry> values)
{
    AssignSnapshot(std::span<const ParamsInstallEntry>(values.begin(), values.size()));
    return *this;
}

void ParamsInstallTable::AssignSnapshot(std::span<const ParamsInstallEntry> values)
{
    if (values.size() > static_cast<std::size_t>((std::numeric_limits<std::uint32_t>::max)()))
        throw std::length_error("Params install snapshot exceeds DWORD array count");
    ParamsInstallTable copy;
    const auto count = static_cast<std::uint32_t>(values.size());
    copy.RebuildAt41B025(count);
    for (std::uint32_t index = 0; index < count; ++index)
    {
        // 只导入工具提供的 cp932 字节；不从 utf16 猜编码，不附加影子值存储。
        SetSnapshotString(copy.At(index).file, values[index].file.cp932);
        SetSnapshotString(copy.At(index).media, values[index].media.cp932);
    }
    *this = std::move(copy);
}

ParamsInstallTable::ParamsInstallTable(const ParamsInstallTable& other)
{
    ParamsInstallTable copy;
    const auto count = static_cast<std::uint32_t>(other.size());
    copy.RebuildAt41B025(count);
    for (std::uint32_t index = 0; index < count; ++index)
    {
        // 不走 CString 的共享复制；完整长度保留嵌入 NUL，且不保留源 manager 所有权。
        const auto& file = other.At(index).file;
        copy.At(index).file.SetString(file.GetString(), file.GetLength());
        const auto& media = other.At(index).media;
        copy.At(index).media.SetString(media.GetString(), media.GetLength());
    }
    reference = std::exchange(copy.reference, YtlReferenceSlotRuntime{});
}

ParamsInstallTable& ParamsInstallTable::operator=(const ParamsInstallTable& other)
{
    if (this != &other)
    {
        ParamsInstallTable copy(other);
        *this = std::move(copy);
    }
    return *this;
}

bool ParamsInstallTable::operator==(const ParamsInstallTable& other) const
{
    const auto count = static_cast<std::uint32_t>(size());
    if (count != other.size()) return false;
    for (std::uint32_t index = 0; index < count; ++index)
    {
        const auto& left = At(index);
        const auto& right = other.At(index);
        if (RuntimeStringView(left.file) != RuntimeStringView(right.file) ||
            RuntimeStringView(left.media) != RuntimeStringView(right.media)) return false;
    }
    return true;
}
}

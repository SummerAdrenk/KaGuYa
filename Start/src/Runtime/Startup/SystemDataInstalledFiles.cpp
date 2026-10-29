#include <Runtime/SystemDataInstalledFiles.h>
#include <Runtime/ParamsRuntimeProfile.h>

#include <bit>
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
static_assert(sizeof(EnvironmentInstalledFile) == 8);
static_assert(sizeof(std::uint32_t) == 4);
static_assert(alignof(EnvironmentInstalledFile) <= alignof(std::uint32_t));

std::uint32_t AllocationBytes(std::uint32_t count) noexcept
{
    // 496125..49613F：乘法或加 cookie 溢出时饱和为 UINT_MAX，仍交给 new[]。
    constexpr auto maximum = (std::numeric_limits<std::uint32_t>::max)();
    return count > (maximum - 4u) / 8u ? maximum : count * 8u + 4u;
}

ATL::IAtlStringMgr* RequireManager(IRuntimeStringManagerSource& strings)
{
    auto* manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL); // 496E72 / 496E7C
    return manager;
}

void ConstructAt496E10(EnvironmentInstalledFile* element, IRuntimeStringManagerSource& strings) noexcept
{
    // FuncInfo 5ED364 的 EHFlags(5ED384)=5，含 FI_EHNOEXCEPT_FLAG，不只是无 unwind map。
    // 两次取源/构造失败直接 terminate；当前首串和此前完整项均不会退回 iterator 清理。
    std::construct_at(&element->stringAt00, RequireManager(strings));
    std::construct_at(&element->stringAt04, RequireManager(strings));
}

void DestroyPrefix(EnvironmentInstalledFile* elements, std::uint32_t count) noexcept
{
    // 53FAB4/53FB52 从尾到头；464E70 的每项析构先 +04 再 +00。
    // 已完成项才允许调用析构；展开期间析构再失败不能替换原异常。
    while (count) std::destroy_at(elements + --count);
}

class InstalledFilesControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit InstalledFilesControl(EnvironmentInstalledFile* elements) noexcept
        : EnvironmentObjectReferenceControl(elements) {}

    void DestroyPayloadAtVslot00() override
    {
        // 原数组虚表 5B2F34 的 slot00=491BC0；销毁数来自 cookie，不是 countAt10。
        auto* const elements = static_cast<EnvironmentInstalledFile*>(payloadAt14);
        if (!elements) return;
        auto* const cookie = reinterpret_cast<std::uint32_t*>(elements) - 1;
        DestroyPrefix(elements, *cookie);
        // 491C04 在元素回调后重读 cookie，释放大小按 DWORD 回绕；53FC17 忽略该大小。
        const std::uint32_t bytes = *cookie * 8u + 4u;
        ::operator delete[](cookie, static_cast<std::size_t>(bytes));
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // slot04=466B70：先退化到 5ADD18，flags&1 才释放，不再次析构正文。
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(InstalledFilesControl) == 24);
}

SystemDataInstalledFiles::~SystemDataInstalledFiles() noexcept
{
    // 45FD90 / 49739B..4973CE：正文先于 control；析构清理不允许异常外传。
    Reset();
}

SystemDataInstalledFiles::SystemDataInstalledFiles(SystemDataInstalledFiles&& other) noexcept
    : reference(std::exchange(other.reference, YtlReferenceSlotRuntime{})) {}

SystemDataInstalledFiles& SystemDataInstalledFiles::operator=(SystemDataInstalledFiles&& other)
{
    if (this != &other)
    {
        Reset();
        reference = std::exchange(other.reference, YtlReferenceSlotRuntime{});
    }
    return *this;
}

void SystemDataInstalledFiles::Reset()
{
    // 4960FE / 45FDCC 必须重读槽，不能删除正文回调前缓存的旧 control。
    ReleaseYtlReference(reference);
}

void SystemDataInstalledFiles::RebuildAt49611A(std::int32_t count, IRuntimeStringManagerSource& strings)
{
    Reset();
    if (count <= 0) return;
    const auto extent = static_cast<std::uint32_t>(count);
    auto* const cookie = static_cast<std::uint32_t*>(::operator new[](AllocationBytes(extent)));
    EnvironmentInstalledFile* elements = nullptr;
    if (cookie)
    {
        *cookie = extent; // 49615E：4 字节 cookie，正文从其后开始。
        elements = reinterpret_cast<EnvironmentInstalledFile*>(cookie + 1);
        // 496169传给通用iterator的具体回调为noexcept，不存在可返回此层的元素构造异常。
        for (std::uint32_t completed = 0; completed < extent; ++completed)
            ConstructAt496E10(elements + completed, strings);
    }
    // 49614C..496170 的 null 数组分支也继续建 control，不伪造元素构造成功。
    // 496177：分配和字符串回调可改槽，此处只虚删当前 control，不再减计数或删正文。
    if (reference.controlAt08) reference.controlAt08->DeleteControlAtVslot04(1);
    reference.biasAt04 = 0;
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(elements);

    // 496194 才分配 24 字节；分配抛出时保留部分发布，不能用 unique_ptr 回滚。
    void* const storage = ::operator new(sizeof(InstalledFilesControl));
    reference.controlAt08 = storage ? ::new (storage) InstalledFilesControl(elements) : nullptr;
    // 安全差异：原 4961B1..4961B9 发布 null 后直接写 [0+4]；现代口保留槽并拒绝。
    if (!reference.controlAt08) throw std::bad_alloc{};
    reference.controlAt08->referenceCountAt04 = 1;
    reference.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(reference.controlAt08)->strideAt0C = 8;
    static_cast<EnvironmentObjectReferenceControl*>(reference.controlAt08)->countAt10 = extent;
}

std::size_t SystemDataInstalledFiles::size() const noexcept
{
    // 4961D9..4961E7 只读 control+10；不从 cookie 或 base 推断数量，也不扣除 bias。
    return reference.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(reference.controlAt08)->countAt10 : 0u;
}

EnvironmentInstalledFile* SystemDataInstalledFiles::ResolveAt(std::uint32_t index) noexcept
{
    return const_cast<EnvironmentInstalledFile*>(std::as_const(*this).ResolveAt(index));
}

const EnvironmentInstalledFile* SystemDataInstalledFiles::ResolveAt(std::uint32_t index) const noexcept
{
    if (!reference.baseAt00) return nullptr;
    const auto stride = reference.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(reference.controlAt08)->strideAt0C : 0u;
    const std::uint32_t offset = (reference.biasAt04 + index) * stride;
    return reinterpret_cast<const EnvironmentInstalledFile*>(reference.baseAt00 + offset);
}

EnvironmentInstalledFile& SystemDataInstalledFiles::At(std::size_t index)
{
    return const_cast<EnvironmentInstalledFile&>(std::as_const(*this).At(index));
}

const EnvironmentInstalledFile& SystemDataInstalledFiles::At(std::size_t index) const
{
    const auto* const element = ResolveAt(static_cast<std::uint32_t>(index));
    if (!element) throw std::logic_error("47E7C7: installed file reference has no payload");
    return *element;
}

void InitializeSystemDataInstallFilesAt4960D5(const ParamsGameSystem& source,
    SystemDataInstalledFiles& destination, IRuntimeStringManagerSource& strings)
{
    destination.RebuildAt49611A(std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(source.installTable.size())), strings);
    for (std::uint32_t index = 0; std::bit_cast<std::int32_t>(index) <
        std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(destination.size())); ++index)
    {
        // 49620D/4962D4 分别解析源；CString 的 Clone/Free 回调可更改嵌套数组槽。
        // 4961D9 的循环界限来自目标数组，不能改成再次读取源 count。
        const auto& file = source.installTable.At(index).file;
        destination.At(index).stringAt00 = file;
        const auto& media = source.installTable.At(index).media;
        destination.At(index).stringAt04 = media;
    }
}
}

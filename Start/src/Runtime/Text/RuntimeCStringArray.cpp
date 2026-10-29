#include <Runtime/RuntimeCStringArray.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
static_assert(sizeof(void*) == 4);
static_assert(sizeof(RuntimeCString) == 4);
static_assert(alignof(RuntimeCString) <= alignof(std::uint32_t));
static_assert(sizeof(ATL::CStringData) == 16);
static_assert(offsetof(ATL::CStringData, pStringMgr) == 0);
static_assert(offsetof(ATL::CStringData, nDataLength) == 4);
static_assert(offsetof(ATL::CStringData, nAllocLength) == 8);
static_assert(offsetof(ATL::CStringData, nRefs) == 12);

std::uint32_t AllocationBytes(std::uint32_t count) noexcept
{
    // 4128B6..4128D1：乘法或加 cookie 溢出时均饱和为 UINT_MAX。
    constexpr auto maximum = (std::numeric_limits<std::uint32_t>::max)();
    return count > (maximum - 4u) / 4u ? maximum : count * 4u + 4u;
}

void DestroyPrefix(RuntimeCString* elements, std::uint32_t count) noexcept
{
    // 53FB52：前缀清理再次抛出 C++ 异常时，在该失败元素处终止。
    // 此 C++ 异常边界不声称等价于原 SEH 异常码过滤器。
    while (count) DestroyRuntimeCStringAt40CD40(elements + --count);
}

struct PrefixCleanup final
{
    RuntimeCString* elements;
    const std::uint32_t& count;
    bool active = true;
    ~PrefixCleanup() noexcept
    {
        // 53FA95/53FB1D 在异常展开期间清理，不先进入 catch。
        if (active) DestroyPrefix(elements, count);
    }
};

void DestroyElements(RuntimeCString* elements, std::uint32_t count)
{
    PrefixCleanup cleanup{elements, count};
    // 53FADB..53FAF5：调用析构前先递减 count，失败后不重试该元素。
    while (count) DestroyRuntimeCStringAt40CD40(elements + --count);
    cleanup.active = false;
}

class RetiredCStringArrayControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredCStringArrayControl() noexcept : EnvironmentObjectReferenceControl(nullptr) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：退役后的 control 按基类尺寸删除。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredCStringArrayControl) == 0x18);

class CStringArrayControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit CStringArrayControl(RuntimeCString* elements) noexcept
        : EnvironmentObjectReferenceControl(elements, UninitializedFieldsTag{}) {}

    void DestroyPayloadAtVslot00() override
    {
        auto* const elements = static_cast<RuntimeCString*>(payloadAt14);
        if (!elements) return;
        auto* const cookie = reinterpret_cast<std::uint32_t*>(elements) - 1;
        DestroyElements(elements, *cookie);
        // 43BB3B：回调后重读 cookie；释放大小按 DWORD 回绕，不沿用分配时的饱和计算。
        const std::uint32_t bytes = *cookie * 4u + 4u;
        // 显式释放裸存储结束元素生命周期，不再执行第二轮 ATL 析构。
        // 元素释放抛出时不会到达此处，按原合同保留外层数组块。
        ::operator delete[](cookie, static_cast<std::size_t>(bytes));
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 410BA0：flags=0 也退役，保留虚表指针之后的全部字节。
        std::array<std::byte, 20> fields;
        std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(this) + 4, fields.size());
        auto* retired = ::new (static_cast<void*>(this)) RetiredCStringArrayControl;
        std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }
};
static_assert(sizeof(CStringArrayControl) == 24);
}

RuntimeCStringArrayReference::RuntimeCStringArrayReference(
    const YtlReferenceSlotRuntime& source) noexcept : reference_(source)
{
    if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
}

RuntimeCStringArrayReference::RuntimeCStringArrayReference(
    const RuntimeCStringArrayReference& source) noexcept : RuntimeCStringArrayReference(source.reference_) {}

RuntimeCStringArrayReference::RuntimeCStringArrayReference(
    RuntimeCStringArrayReference&& source) noexcept
    : reference_(std::exchange(source.reference_, {})) {}

RuntimeCStringArrayReference& RuntimeCStringArrayReference::operator=(
    const RuntimeCStringArrayReference& source)
{
    if (this != &source)
    {
        // 410360：目标释放回调全部完成后才重读 source，不能提前增持。
        Reset();
        reference_ = source.reference_;
        if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
    }
    return *this;
}

RuntimeCStringArrayReference& RuntimeCStringArrayReference::operator=(
    RuntimeCStringArrayReference&& source)
{
    if (this != &source)
    {
        Reset();
        reference_ = std::exchange(source.reference_, {});
    }
    return *this;
}

RuntimeCStringArrayReference::~RuntimeCStringArrayReference() noexcept { Reset(); }

void RuntimeCStringArrayReference::Reset()
{
    // 412877..4128A3：回调后重读 control；保留 bias，无 control 时也保留孤立 base。
    ReleaseYtlReference(reference_);
}

void RuntimeCStringArrayReference::Assign(const RuntimeCStringArrayReference& source,
    std::int32_t bias)
{
    *this = source;
    reference_.biasAt04 = static_cast<std::uint32_t>(bias);
}

void RuntimeCStringArrayReference::RebuildAt412870(std::int32_t count,
    IRuntimeStringManagerSource& strings)
{
    Reset();
    if (count <= 0) return;
    const auto extent = static_cast<std::uint32_t>(count);
    auto* const cookie = static_cast<std::uint32_t*>(::operator new[](AllocationBytes(extent)));
    RuntimeCString* elements = nullptr;
    if (cookie)
    {
        *cookie = extent;
        elements = reinterpret_cast<RuntimeCString*>(cookie + 1);
        std::uint32_t completed = 0;
        PrefixCleanup cleanup{elements, completed};
        for (; completed < extent; ++completed)
        {
            // 40B5D0：raw GetNil 可抛异常，仅构造成功返回后才计入已完成前缀。
            InitializeEmptyRuntimeCString(elements[completed], strings);
        }
        // 412870 不为外层数组分配设置展开清理资格。
        cleanup.active = false;
    }

    // 4128FC：回调后重读槽，只删除当前 control，不再次减计数或释放正文。
    if (reference_.controlAt08) reference_.controlAt08->DeleteControlAtVslot04(1);
    reference_.biasAt04 = 0;
    reference_.baseAt00 = reinterpret_cast<std::uintptr_t>(elements);
    void* const storage = ::operator new(sizeof(CStringArrayControl));
    auto* const control = storage ? ::new (storage) CStringArrayControl(elements) : nullptr;
    reference_.controlAt08 = control;
    // 安全差异：原 412931 直接写 [null+4]；此处保留已发布的孤立正文，改抛 bad_alloc。
    // 不补回滚，也不伪造成功的空 Ref。
    if (!control) throw std::bad_alloc{};
    // 412931 使用分配返回的 control；后续字段写入重新读取已发布槽。
    control->referenceCountAt04 = 1;
    reference_.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(reference_.controlAt08)->strideAt0C = 4;
    static_cast<EnvironmentObjectReferenceControl*>(reference_.controlAt08)->countAt10 = extent;
}

std::size_t RuntimeCStringArrayReference::Size() const noexcept
{
    return reference_.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(reference_.controlAt08)->countAt10 : 0u;
}

RuntimeCString* RuntimeCStringArrayReference::StorageAt3C() noexcept
{
    return reinterpret_cast<RuntimeCString*>(reference_.baseAt00);
}

const RuntimeCString* RuntimeCStringArrayReference::StorageAt3C() const noexcept
{
    return reinterpret_cast<const RuntimeCString*>(reference_.baseAt00);
}

std::int32_t RuntimeCStringArrayReference::IndexBiasAt40() const noexcept
{
    return std::bit_cast<std::int32_t>(reference_.biasAt04);
}

RuntimeCString& RuntimeCStringArrayReference::At(std::uint32_t index)
{
    return const_cast<RuntimeCString&>(std::as_const(*this).At(index));
}

const RuntimeCString& RuntimeCStringArrayReference::At(std::uint32_t index) const
{
    if (!reference_.baseAt00)
        throw std::out_of_range("CString array reference has no storage");
    return *Resolve(index);
}

RuntimeCString* RuntimeCStringArrayReference::Resolve(std::uint32_t index) noexcept
{
    return const_cast<RuntimeCString*>(std::as_const(*this).Resolve(index));
}

const RuntimeCString* RuntimeCStringArrayReference::Resolve(std::uint32_t index) const noexcept
{
    if (!reference_.baseAt00) return nullptr;
    const auto stride = reference_.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(reference_.controlAt08)->strideAt0C : 0u;
    const std::uint32_t offset = (reference_.biasAt04 + index) * stride;
    return reinterpret_cast<const RuntimeCString*>(reference_.baseAt00 + offset);
}

std::string_view RuntimeCStringArrayReference::ResourceNameAt(std::size_t index) const
{
    return RuntimeStringView(At(static_cast<std::uint32_t>(index)));
}
}

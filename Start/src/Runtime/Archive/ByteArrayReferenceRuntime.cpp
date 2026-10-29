#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredByteArrayControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredByteArrayControl() noexcept
        : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 的 slot04=412460：退役后再次删除传 14h，不沿用派生 control 的 18h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredByteArrayControl) == 0x18);

void RetireByteArrayControlAt410B10(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    // 410B1A：flags=0 也先退化虚表，其余二十字节（含未写 padding/payload）保持。
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* retired = ::new (static_cast<void*>(control)) RetiredByteArrayControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    // 原入口无整体 noexcept；本次派生删除的专属实参是 18h。
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

// 原 PE：412771 写入 5AE3E8，两个虚槽依次为 43BA90、410B10。
class ByteArrayControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ByteArrayControl(std::uint8_t* bytes) noexcept
        : EnvironmentObjectReferenceControl(bytes, UninitializedFieldsTag{}) {}
    // 43BA90 只释放 +14h 正文，不按 bias/stride 计算地址，也不修改该字段。
    void DestroyPayloadAtVslot00() override { delete[] static_cast<std::uint8_t*>(payloadAt14); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 410B1A 先退化到 5ADD18；flags&1 才经 53FBFB 释放 18h 控制块。
        RetireByteArrayControlAt410B10(this, flags);
    }
};
static_assert(sizeof(ByteArrayControl) == 0x18);
}

ByteArrayReferenceRuntime::ByteArrayReferenceRuntime(const YtlReferenceSlotRuntime& source) noexcept
    : reference_(source)
{
    if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
}

ByteArrayReferenceRuntime::ByteArrayReferenceRuntime(const ByteArrayReferenceRuntime& source) noexcept
    : ByteArrayReferenceRuntime(source.reference_)
{
}

ByteArrayReferenceRuntime& ByteArrayReferenceRuntime::operator=(const ByteArrayReferenceRuntime& source)
{
    if (this != &source)
    {
        // 46581E..465864：释放可抛且回调可改来源，必须在释放完成后重读三字槽。
        ReleaseYtlReference(reference_);
        reference_ = source.reference_;
        if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
    }
    return *this;
}

ByteArrayReferenceRuntime::ByteArrayReferenceRuntime(ByteArrayReferenceRuntime&& source) noexcept
    : reference_(std::exchange(source.reference_, {}))
{
}

ByteArrayReferenceRuntime& ByteArrayReferenceRuntime::operator=(ByteArrayReferenceRuntime&& source)
{
    if (this != &source)
    {
        ReleaseYtlReference(reference_);
        reference_ = std::exchange(source.reference_, {});
    }
    return *this;
}

ByteArrayReferenceRuntime::~ByteArrayReferenceRuntime() noexcept
{
    // 40CF00：沿用回调后清槽及重读 control；析构边界不外传释放异常。
    ReleaseYtlReference(reference_);
}

void ByteArrayReferenceRuntime::AllocateAt412700(std::uint32_t count)
{
    ReleaseYtlReference(reference_);
    // 41273C 的 signed count<=0；无 control 的旧 base/bias 仍然保留。
    if (!count || count > 0x7FFFFFFFu) return;

    auto* bytes = new std::uint8_t[count];
    // 412747..412757：分配回调可改槽，只删重读到的 control，不减计数或销毁正文。
    if (reference_.controlAt08) reference_.controlAt08->DeleteControlAtVslot04(1);
    reference_.biasAt04 = 0;
    reference_.baseAt00 = reinterpret_cast<std::uintptr_t>(bytes);
    // 412765..41277E：raw new + placement 避免 /sdl 预清零 +09..+0B。
    // 控制块失败保留已发布正文，不以 RAII 增加原版没有的回滚。
    void* storage = ::operator new(sizeof(ByteArrayControl));
    auto* control = storage ? ::new (storage) ByteArrayControl(bytes) : nullptr;
    // 412771/412777 只构造 vptr/payload；41277E 先发布，再经原分配指针写 refs。
    // 不给 null control 添加成功兜底，保留此发布点之后的原故障资格。
    reference_.controlAt08 = control;
    control->referenceCountAt04 = 1;
    // 412788/412790/41279A 每次重读槽；不能用缓存 control 合并这些原始写入。
    reference_.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(reference_.controlAt08)->strideAt0C = 1;
    static_cast<EnvironmentObjectReferenceControl*>(reference_.controlAt08)->countAt10 = count;
}

void ByteArrayReferenceRuntime::Reset()
{
    ReleaseYtlReference(reference_);
}

std::uint8_t* ByteArrayReferenceRuntime::Data() const noexcept
{
    return DataAt(0);
}

std::uint8_t* ByteArrayReferenceRuntime::DataAt(std::uint32_t index) const noexcept
{
    // 43D230以及418813/4AD6B8：先相加索引，再乘control中的stride，保持x86回绕。
    if (!reference_.baseAt00) return nullptr;
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(reference_.controlAt08);
    const auto stride = control ? control->strideAt0C : 0u;
    return reinterpret_cast<std::uint8_t*>(reference_.baseAt00 + (reference_.biasAt04 + index) * stride);
}

std::uint32_t ByteArrayReferenceRuntime::Size() const noexcept
{
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(reference_.controlAt08);
    return control ? control->countAt10 : 0u; // 原 +10h 计数，不扣 bias。
}

std::span<const std::uint8_t> ByteArrayReferenceRuntime::Bytes() const noexcept
{
    return {Data(), Size()};
}

std::span<std::uint8_t> ByteArrayReferenceRuntime::MutableBytes() noexcept
{
    return {Data(), Size()};
}

ByteArrayReferenceRuntime::operator bool() const noexcept
{
    return reference_.baseAt00 != 0;
}
}

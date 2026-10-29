#include <Runtime/ArchiveReaderNative.h>
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredReaderControl final : public ArchiveReaderReferenceControl
{
public:
    RetiredReaderControl() noexcept : ArchiveReaderReferenceControl(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18/412460：退役后删除只传基类 14h，不再使用首次派生删除的 18h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
    ArchiveStreamReaderRuntime* unwrittenPayloadAt14;
};
static_assert(sizeof(RetiredReaderControl) == ArchiveReaderNativeControlSize);

class ReaderControl final : public ArchiveReaderReferenceControl
{
public:
    explicit ReaderControl(ArchiveStreamReaderRuntime* body) noexcept
        : ArchiveReaderReferenceControl(UninitializedFieldsTag{}), payloadAt14(body) {}

    void DestroyPayloadAtVslot00() override
    {
        // 43C880：使用控制块原 payload，不解析外层 Ref 的 base/bias/stride。
        if (payloadAt14) payloadAt14->DeleteAtVslot00(1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 411410 只退化虚表；其余二十字节（含未写 padding）保持不变。
        std::array<std::byte, 20> fields;
        std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(this) + 4, fields.size());
        auto* retired = ::new (static_cast<void*>(this)) RetiredReaderControl;
        std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
        if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
    }

    ArchiveStreamReaderRuntime* payloadAt14;
};
static_assert(sizeof(ReaderControl) == ArchiveReaderNativeControlSize);
}

ArchiveStreamReaderRuntime::ArchiveStreamReaderRuntime(IRuntimeStringManagerSource& strings)
{
    ::new (static_cast<void*>(&bytesAt04)) ByteArrayReferenceRuntime;
    InitializeEmptyRuntimeCString(nameAt10, strings);
}

ArchiveFileReaderRuntime::ArchiveFileReaderRuntime(IRuntimeStringManagerSource& strings)
    : ArchiveStreamReaderRuntime(strings)
{
    // 已发布派生虚表后才进入 state3；此处异常由基类完整析构展开。
    ArchiveStreamReaderRuntime::ClearAt4327B0();
}
ArchiveFileReaderRuntime::ArchiveFileReaderRuntime(InlineBeforeClearTag, IRuntimeStringManagerSource& strings)
    : ArchiveStreamReaderRuntime(strings)
{
}

void ArchiveStreamReaderRuntime::ClearAt4327B0()
{
    // 无 control 时不清 base/bias；payload 回调之后重新读取当前 control。
    bytesAt04.Reset();
    SetRuntimeCStringAt437680(nameAt10, "", 0);
}

ArchiveStreamReaderRuntime::~ArchiveStreamReaderRuntime() noexcept
{
    // 40F8A0/5DEFBC flags5：整段终止边界。字符串回调可重新填入 byte Ref，末尾再释放当前槽。
    ArchiveStreamReaderRuntime::ClearAt4327B0();
    DestroyRuntimeCStringAt40CD40(&nameAt10);
    bytesAt04.~ByteArrayReferenceRuntime();
}

void* ArchiveStreamReaderRuntime::DeleteAtVslot00(std::uint32_t flags)
{
    auto* address = this;
    this->~ArchiveStreamReaderRuntime();
    if (flags & 1u) ::operator delete(address, std::size_t{0x14});
    return address;
}

void* ArchiveFileReaderRuntime::DeleteAtVslot00(std::uint32_t flags)
{
    auto* address = this;
    this->~ArchiveFileReaderRuntime();
    if (flags & 1u) ::operator delete(address, std::size_t{0x14});
    return address;
}

bool ArchiveStreamReaderRuntime::ReadyAt419360() const
{
    return bytesAt04.RawSlot().baseAt00 != 0;
}

ByteArrayReferenceRuntime* ArchiveStreamReaderRuntime::BytesAt4172C0()
{
    return &bytesAt04;
}

const char* ArchiveStreamReaderRuntime::NameAt4179F0() const
{
    return nameAt10.GetString();
}

ArchiveFileReaderRuntime* ConstructArchiveFileReaderAt483E99(
    void* storage, IRuntimeStringManagerSource& strings)
{
    return storage ? ::new (storage) ArchiveFileReaderRuntime(strings) : nullptr;
}

ArchiveReaderReferenceControl* ConstructArchiveReaderControl(
    void* storage, ArchiveStreamReaderRuntime* body) noexcept
{
    return storage ? ::new (storage) ReaderControl(body) : nullptr;
}

std::span<const std::byte> ArchiveReaderPayloadView(const ArchiveReaderReference& reader)
{
    auto* body = reader.Get();
    if (!body) throw ATL::CAtlException(E_INVALIDARG);
    const auto* bytes = body->BytesAt4172C0();
    if (!bytes) throw ATL::CAtlException(E_INVALIDARG);
    const auto& slot = bytes->RawSlot();
    const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08);
    if (!control && slot.baseAt00) throw ATL::CAtlException(E_INVALIDARG);
    const auto count = control ? control->countAt10 : 0u;
    if (!count) return {};
    if (!slot.baseAt00 || slot.biasAt04 || control->strideAt0C != 1)
        throw ATL::CAtlException(E_INVALIDARG);
    return {reinterpret_cast<const std::byte*>(slot.baseAt00), count};
}
}

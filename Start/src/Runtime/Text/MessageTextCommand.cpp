#include <Runtime/MessageTextCommand.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <cstring>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
ATL::IAtlStringMgr* CaptureManager(IRuntimeStringManagerSource& strings)
{
    auto* manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL); // 45EF0F。
    return manager;
}
}

MessageTextCommand::MessageTextCommand(const char* source, std::int32_t size,
    IRuntimeStringManagerSource& strings) : textAt04_(CaptureManager(strings))
{
    if (size == 0)
    {
        textAt04_.SetString(source, source ? static_cast<int>(std::strlen(source)) : 0);
        return;
    }
    if (size < 0) throw ATL::CAtlException(E_INVALIDARG);
    // 安全差异：原正长度空指针将访问违规；明确拒绝，不合成空白正文。
    if (!source) throw ATL::CAtlException(E_INVALIDARG);
    // 45EEAE..45EEF5：先按声明长度分配并复制，再按首个 NUL 提交可见长度。
    // 不能先 strlen 再分配，否则内嵌 NUL 后的容量、字节及 OOM 时点均会偏离。
    char* destination = textAt04_.GetBufferSetLength(size);
    std::memmove(destination, source, static_cast<std::size_t>(size));
    textAt04_.ReleaseBuffer();
}

std::int32_t MessageTextCommand::GetLengthAt47D730() const noexcept { return textAt04_.GetLength(); }
RuntimeCString MessageTextCommand::GetTextAt47D550() const { return textAt04_; }

MessageTextCommandReference::MessageTextCommandReference(const YtlReferenceSlotRuntime& source) noexcept
    : reference_(source)
{
    if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
}
MessageTextCommandReference::MessageTextCommandReference(const MessageTextCommandReference& source) noexcept
    : MessageTextCommandReference(source.reference_) {}
MessageTextCommandReference& MessageTextCommandReference::operator=(const MessageTextCommandReference& source)
{
    if (this != &source)
    {
        // 410360：释放回调后重读来源，不能用 copy-and-swap 提前保活新正文。
        ReleaseYtlReference(reference_);
        reference_ = source.reference_;
        if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
    }
    return *this;
}
MessageTextCommandReference::MessageTextCommandReference(MessageTextCommandReference&& source) noexcept
    : reference_(std::exchange(source.reference_, {})) {}
MessageTextCommandReference& MessageTextCommandReference::operator=(MessageTextCommandReference&& source)
{
    if (this != &source)
    {
        ReleaseYtlReference(reference_);
        reference_ = std::exchange(source.reference_, {});
    }
    return *this;
}
MessageTextCommandReference::~MessageTextCommandReference() noexcept { ReleaseYtlReference(reference_); }
void MessageTextCommandReference::ReplaceOwnedAt451A80(MessageTextCommand* created)
{
    // 451A80用于Voice/legacy内部槽；与47D570返回槽不同，control先发布再写计数。
    ReplaceEnvironmentObjectReference(reference_, created, 8);
}
void MessageTextCommandReference::Reset() { ReleaseYtlReference(reference_); }
MessageTextCommand* MessageTextCommandReference::Get() const noexcept
{ return ResolveEnvironmentObjectReference<MessageTextCommand>(reference_); }

MessageTextCommand* AllocateMessageTextCommand(const char* source, std::int32_t size,
    IRuntimeStringManagerSource& strings)
{
    // 47D63B/47D675：正文构造失败由 caller EH 回收 raw 8-byte，输出尚未初始化。
    void* storage = ::operator new(sizeof(MessageTextCommand));
    if (!storage) return nullptr;
    try { return ::new (storage) MessageTextCommand(source, size, strings); }
    catch (...) { ::operator delete(storage); throw; }
}

void ConstructMessageTextCommandAt47D570(YtlReferenceSlotRuntime& output,
    const char* source, std::int32_t size, IRuntimeStringManagerSource& strings)
{
    auto* body = AllocateMessageTextCommand(source, size, strings);
    output = {};
    if (!body) return;
    output.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    // 47D698..47D6D4：控制块失败保留孤立正文；控制块先初始化，发布后才写 count。
    using Control = EnvironmentObjectControl<MessageTextCommand>;
    static_assert(sizeof(Control) == 24);
    void* storage = ::operator new(sizeof(Control));
    auto* control = storage ? ::new (storage) Control(body) : nullptr;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 8;
    output.controlAt08 = control;
    control->countAt10 = 1;
}

MessageTextCommandReference MakeMessageTextCommand(const char* source, std::int32_t size,
    IRuntimeStringManagerSource& strings)
{
    MessageTextCommandReference result;
    ConstructMessageTextCommandAt47D570(result.RawSlot(), source, size, strings);
    return result;
}
}

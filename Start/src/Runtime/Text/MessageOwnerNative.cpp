#include <Runtime/MessageOwnerNative.h>
#include <Runtime/MessageTextSelectionRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/ProcessEnvironmentRuntime.h>
#include <Runtime/RuntimeCStringConcatenation.h>

#include <cstring>
#include <new>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 局部无状态转发；根捕获仍在45A5D0/494280/43D3D0各自try中，不提前取源。
class ProcessMessagePaths final : public IMessageEnvironmentPathSource
{
public:
    const RuntimeCString& CaptureMessagePrefixAt45A5D0() const override
    { return CaptureProcessEnvironmentPathsAt4180C0()->currentAt08; }
    const RuntimeCString& CaptureUnreadPrefixAt43D3D0() const override
    { return CaptureProcessEnvironmentPathsAt4180C0()->saveAt10; }
};

class MessageControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit MessageControl(MessageOwnerNative34h* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<MessageOwnerNative34h*>(payloadAt14);
        if (!body) return;
        // 492C80必须读取当前正文表，不能按final静态类型去虚拟化。
        using Delete = MessageOwnerNative34h* (__thiscall*)(MessageOwnerNative34h*, std::uint32_t);
        const Delete* table;
        std::memcpy(&table, body, sizeof(table));
        table[0](body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(MessageControl) == 0x18);

template<class Construct>
MessageOwnerNative34h* NewBody(Construct construct)
{
    void* const storage = ::operator new(0x34);
    if (!storage) return nullptr;
    struct RawUnwind final
    {
        void* storage;
        ~RawUnwind() noexcept
        {
            // 458B60 state18h：handler失败后，仍在异常展开期间delete34h。
            if (storage) ::operator delete(storage, std::size_t{0x34});
        }
    } unwind{storage};
    auto* const body = construct(storage);
    unwind.storage = nullptr;
    return body;
}
}

MessageOwnerNative34h::MessageOwnerNative34h()
{
    ::new (static_cast<void*>(&readerAt04)) MessageReaderReference();
    ::new (static_cast<void*>(&flagsAt10)) MessageReadFlagsStorage();
    Win32StartFileApi files;
    ProcessMessagePaths paths;
    DefaultMessageReaderFactory factory;
    ConstructAt45A5D0({{files}, paths, factory});
}
MessageOwnerNative34h::MessageOwnerNative34h(MessageOwnerNativeWithSources sources)
{
    // 不在初始化列表激活variant成员；只有下方MemberUnwind拥有EH清理资格。
    ::new (static_cast<void*>(&readerAt04)) MessageReaderReference();
    ::new (static_cast<void*>(&flagsAt10)) MessageReadFlagsStorage();
    ConstructAt45A5D0(sources);
}
MessageOwnerNative34h::MessageOwnerNative34h(TableOnlyTag) noexcept {}

void MessageOwnerNative34h::ConstructAt45A5D0(MessageOwnerNativeWithSources sources)
{
    struct MemberUnwind final
    {
        MessageOwnerNative34h& body;
        bool armed = true;
        ~MemberUnwind() noexcept
        {
            if (!armed) return;
            // 596691/596686/59667B/596670；构造失败绝不执行Persist或正文析构。
            body.flagsAt10[2].Reset();
            body.flagsAt10[1].Reset();
            body.flagsAt10[0].Reset();
            body.readerAt04.Reset();
        }
    } unwind{*this};
    try
    {
        const RuntimeCStringConcatenation path(sources.paths.CaptureMessagePrefixAt45A5D0(), "message.dat", 11);
        if (path.Value().IsEmpty()) throw 1;
        sources.factory.CreateMessage2(readerAt04);
        if (!SourceWithContext(sources.context).Load(path.GetString()))
        {
            sources.factory.CreateLegacyMessage(readerAt04);
            if (!SourceWithContext(sources.context).Load(path.GetString())) throw 2;
        }
        auto flags = ReadFlagsWithSources(sources.context.files, sources.paths);
        flags.LoadUnreadFlagsAt494280();
        flags.ReconcileCurrentReaderAt45A9D6(readerAt04);
    }
    catch (...)
    {
        // 45ABAF在原try之外；不造非法空reader的替代异常，不吞handler自身失败。
        sources.factory.CreateNullMessage(readerAt04);
    }
    unwind.armed = false;
}

void MessageOwnerNative34h::PersistUnreadFlagsAt43D3D0()
{
    Win32StartFileApi files;
    ProcessMessagePaths paths;
    auto flags = ReadFlagsWithSources(files, paths);
    flags.PersistUnreadFlags();
}
std::int32_t MessageOwnerNative34h::ReadTextAt4AD590(std::int32_t index,
    MessageFormatSelectionTarget output)
{
    Win32StartFileApi files;
    auto reader = SourceWithContext({files});
    MessageTextSelectionRuntime query(reader, flagsAt10[2]);
    return query.ReadTextAt4AD590(index, output);
}
std::int32_t MessageOwnerNative34h::ReadFormatAt417C40(std::int32_t index,
    MessageFormatSelectionTarget output, std::uint32_t variant, bool markRead)
{
    Win32StartFileApi files;
    ProcessMessagePaths paths;
    auto reader = SourceWithContext({files});
    auto flags = ReadFlagsWithSources(files, paths);
    MessageFormatSelectionRuntime query(reader, flags);
    return query.ReadFormatAt417C40(index, output, variant, markRead);
}

std::int32_t MessageOwnerNative34h::ReadFormatRawAt417C40(std::int32_t index,
    MessageFormatSelectionRawTarget output, std::uint32_t variant, bool markRead)
{
    Win32StartFileApi files;
    ProcessMessagePaths paths;
    auto reader = SourceWithContext({files});
    auto flags = ReadFlagsWithSources(files, paths);
    MessageFormatSelectionRuntime query(reader, flags);
    return query.ReadFormatRawAt417C40(index, output, variant, markRead);
}

void MessageOwnerNative34h::RestoreVptrAt463245() noexcept
{
    union TableSource
    {
        TableSource() noexcept : body(TableOnlyTag{}) {}
        ~TableSource() noexcept {}
        MessageOwnerNative34h body;
    } source;
    // 只写vptr；不能重建目标正文并抹掉回调留下的四个Ref槽。
    std::memcpy(static_cast<void*>(this), &source.body, sizeof(void*));
}
void MessageOwnerNative34h::DestroyWithSourcesAt463220(IStartFileApi& files,
    const IMessageEnvironmentPathSource& paths) noexcept
{
    RestoreVptrAt463245();
    auto flags = ReadFlagsWithSources(files, paths);
    flags.PersistUnreadFlags();
    flagsAt10[0].Reset();
    flagsAt10[1].Reset();
    flagsAt10[2].Reset();
    // 4632EB/4632F2使用原null或4632E2清零的ECX，不重读body.control。
    flagsAt10[1].Reset();
    flagsAt10[0].Reset();
    readerAt04.Reset();
}
MessageOwnerNative34h::~MessageOwnerNative34h() noexcept
{
    Win32StartFileApi files;
    ProcessMessagePaths paths;
    DestroyWithSourcesAt463220(files, paths);
}
MessageOwnerNative34h* MessageOwnerNative34h::DeleteAtVslot00(std::uint32_t flags) noexcept
{
    this->~MessageOwnerNative34h();
    if (flags & 1u) ::operator delete(this, std::size_t{0x34});
    return this;
}
MessageOwnerNative34h* MessageOwnerNative34h::DeleteWithSourcesAt468E20(std::uint32_t flags,
    IStartFileApi& files, const IMessageEnvironmentPathSource& paths) noexcept
{
    DestroyWithSourcesAt463220(files, paths);
    if (flags & 1u) ::operator delete(this, std::size_t{0x34});
    return this;
}

MessageOwnerNative34h* NewMessageOwnerAt4593AD()
{ return NewBody([](void* storage) { return ::new (storage) MessageOwnerNative34h; }); }
MessageOwnerNative34h* NewMessageOwnerWithSourcesAt4593AD(MessageOwnerNativeWithSources sources)
{ return NewBody([sources](void* storage) { return ::new (storage) MessageOwnerNative34h(sources); }); }
void ReplaceMessageReferenceAt4593CF(YtlReferenceSlotRuntime& slot, MessageOwnerNative34h* created)
{
    ReleaseYtlReference(slot);
    if (!created) return;
    // 45940C同样检查cached ECX=0；并无第二次读取slot.control并删除的路径。
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    void* const storage = ::operator new(0x18);
    auto* const control = storage ? ::new (storage) MessageControl(created) : nullptr;
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = 0x34;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}
void CreateAndReplaceMessageAt4593AD(YtlReferenceSlotRuntime& slot)
{ ReplaceMessageReferenceAt4593CF(slot, NewMessageOwnerAt4593AD()); }
void CreateAndReplaceMessageWithSourcesAt4593AD(YtlReferenceSlotRuntime& slot, MessageOwnerNativeWithSources sources)
{ ReplaceMessageReferenceAt4593CF(slot, NewMessageOwnerWithSourcesAt4593AD(sources)); }
MessageOwnerNative34h* ResolveMessageReference(const YtlReferenceSlotRuntime& slot) noexcept
{ return ResolveEnvironmentObjectReference<MessageOwnerNative34h>(slot); }

MessageReference::MessageReference(const YtlReferenceSlotRuntime& source) noexcept : reference_(source)
{ if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04; }
MessageReference::MessageReference(const MessageReference& source) noexcept : MessageReference(source.reference_) {}
MessageReference& MessageReference::operator=(const MessageReference& source)
{
    if (this != &source)
    {
        ReleaseYtlReference(reference_);
        reference_ = source.reference_;
        if (reference_.controlAt08) ++reference_.controlAt08->referenceCountAt04;
    }
    return *this;
}
MessageReference::MessageReference(MessageReference&& source) noexcept
    : reference_(std::exchange(source.reference_, {})) {}
MessageReference& MessageReference::operator=(MessageReference&& source)
{
    if (this != &source)
    {
        ReleaseYtlReference(reference_);
        reference_ = std::exchange(source.reference_, {});
    }
    return *this;
}
MessageReference::~MessageReference() noexcept { ReleaseYtlReference(reference_); }
void MessageReference::Reset() { ReleaseYtlReference(reference_); }
void MessageReference::ReplaceOwned(MessageOwnerNative34h* created)
{ ReplaceMessageReferenceAt4593CF(reference_, created); }
MessageOwnerNative34h* MessageReference::Get() const noexcept { return ResolveMessageReference(reference_); }
}

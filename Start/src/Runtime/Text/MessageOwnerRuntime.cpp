#include <Runtime/MessageOwnerRuntime.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Logging/Logger.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
void DefaultMessageReaderFactory::CreateMessage2(MessageReaderReference& destination)
{ destination.ReplaceOwned(AllocateNativeMessage2Runtime(), MessageReaderKind::Message2); }
void DefaultMessageReaderFactory::CreateLegacyMessage(MessageReaderReference& destination)
{ destination.ReplaceOwned(AllocateNativeLegacyMessageRuntime(), MessageReaderKind::Legacy); }
void DefaultMessageReaderFactory::CreateNullMessage(MessageReaderReference& destination)
{ destination.ReplaceOwned(AllocateNativeNullMessageRuntime(), MessageReaderKind::Null); }

MessageOwnerRuntime::MessageOwnerRuntime(IStartFileApi& files, RuntimeStringBorrow prefix,
    RuntimeStringBorrow saveDirectory, const FileTransactionContext* transaction)
    : flags_(files, saveDirectory), context_{files, transaction}
{
    DefaultMessageReaderFactory factory;
    ConstructAt45A5D0(prefix, nullptr, factory);
}

MessageOwnerRuntime::MessageOwnerRuntime(IStartFileApi& files, RuntimeStringBorrow prefix,
    RuntimeStringBorrow saveDirectory, IMessageReaderFactory& factory)
    : flags_(files, saveDirectory), context_{files}
{
    ConstructAt45A5D0(prefix, nullptr, factory);
}

MessageOwnerRuntime::MessageOwnerRuntime(IStartFileApi& files,
    const IMessageEnvironmentPathSource& paths, const FileTransactionContext* transaction)
    : flags_(files, paths), context_{files, transaction}
{
    DefaultMessageReaderFactory factory;
    ConstructAt45A5D0(std::nullopt, &paths, factory);
}

MessageOwnerRuntime::MessageOwnerRuntime(IStartFileApi& files,
    const IMessageEnvironmentPathSource& paths, IMessageReaderFactory& factory)
    : flags_(files, paths), context_{files}
{
    ConstructAt45A5D0(std::nullopt, &paths, factory);
}

void MessageOwnerRuntime::ConstructAt45A5D0(std::optional<RuntimeStringBorrow> prefix,
    const IMessageEnvironmentPathSource* paths, IMessageReaderFactory& factory)
{
    try
    {
        // 原环境+48.payload+08，与finfo的+10及unread保存目录不是同一个字符串。
        // 4180C0的失败也在45ABAF捕获范围内，不能提前到构造初始化列表查询环境。
        const RuntimeStringBorrow current = paths ? paths->CaptureMessagePrefixAt45A5D0() : *prefix;
        const RuntimeCStringConcatenation path(current, "message.dat", 11);
        if (path.Value().IsEmpty()) throw 1;
        const char* selected = "Message2";
        factory.CreateMessage2(reader_);
        // 空正文的原虚调用会越界；仅此非法输入明确报错，不补造正常 reader。
        if (!reader_) throw std::logic_error("Message2 factory returned no object");
        if (!Source().Load(path.GetString()))
        {
            // 45A8B9只按正常返回AL==0重试；异常直接进入外层NullMessage分支。
            // 45A910..45A95C：新正文构造成功后，先销毁旧reader，再发布新reader。
            factory.CreateLegacyMessage(reader_);
            if (!reader_) throw std::logic_error("legacy Message factory returned no object");
            selected = "Message(v2/v3)";
            if (!Source().Load(path.GetString())) throw 2;
        }
        flags_.LoadUnreadFlagsAt494280();
        flags_.ReconcileCurrentReaderAt45A9D6(reader_);
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Info,
            "CMessage 45A5D0 selected=%s unread=%zu/%zu/%zu", selected,
            flags_.FormatFlagsAt10().size(), flags_.ChoiceFlagsAt1C().size(), flags_.BodyFlagsAt28().size());
    }
    catch (...) // 5E8D44 states4..6 ->45ABAF；保留已变动的unread数组。
    {
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Warning,
            "CMessage 45ABAF: construction failed; installing original NullMessage reader");
        // handler自身的分配/构造失败不能由上面的catch再次吞掉。
        // 451A1E 允许空Null正文，仍先释放旧槽；控制块失败不回滚已发布正文。
        factory.CreateNullMessage(reader_);
    }
}

MessageOwnerRuntime::~MessageOwnerRuntime() noexcept
{
    // 46324B先持久化，463250再主动顺序释放；EHFlags=5，不允许析构异常逃出。
    flags_.PersistUnreadFlags();
    flags_.ReleaseArraysAt463250();
}
}

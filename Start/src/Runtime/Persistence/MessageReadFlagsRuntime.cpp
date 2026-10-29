#include <Runtime/MessageReadFlagsRuntime.h>
#include <Runtime/MessageReaderReference.h>
#include <Runtime/FileStreamRuntime.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Logging/Logger.h>

#include <cstring>
#include <cstddef>
#include <bit>
#include <memory>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
std::int32_t MessageReadFlagsRuntime::MarkChoiceAt4C29D0(std::int32_t index, std::uint8_t mark)
{
    const auto& flags = ChoiceReferenceAt1C();
    // 4C29ED为有符号jge。
    if (index < 0 || index >= std::bit_cast<std::int32_t>(flags.Size())) return -1;
    const auto previous = *flags.DataAt(static_cast<std::uint32_t>(index));
    if (mark != 0)
    {
        SetChoiceFlag(index);
        if (previous == 0) PersistUnreadFlags();
    }
    return previous;
}

namespace
{
class UnreadFile final
{
public:
    explicit UnreadFile(IStartFileApi& files) noexcept : files_(files) {}
    void Open(const char* path, bool writing)
    {
        SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
        const FileOpenRequest request{path, writing ? GENERIC_WRITE : GENERIC_READ,
            0, static_cast<DWORD>(writing ? OPEN_ALWAYS : OPEN_EXISTING), FILE_ATTRIBUTE_NORMAL, &security};
        // 494433/43D4E7正常返回才取得state3资格；Open抛出不回收已发布的file Ref。
        cleanupArmed_ = false;
        static_cast<void>(storage_.stream.OpenWithApi(request, files_, nullptr));
        cleanupArmed_ = true;
        // 原调用者检查base；INVALID_HANDLE/LastError=0仍保留零handle的正文。
        if (!storage_.stream.IsOpenAtVslot08()) throw 2;
    }
    ~UnreadFile() noexcept
    {
        // 40ED70/5DEE64 EHFlags=5：此析构中的异常不能交给外围读写catch吞掉。
        if (cleanupArmed_) CloseAt40ED70();
    }
    void CloseAt40ED70() noexcept
    {
        // 43D5D7失败时对同址raw存储再次析构，不重建对象或写回Ref快照。
        DestroyFileStreamAt40ED70(&storage_, files_);
    }
    void DisarmCleanup() noexcept { cleanupArmed_ = false; }
    UnreadFile(const UnreadFile&) = delete;
    UnreadFile& operator=(const UnreadFile&) = delete;

    std::uint64_t SizeAt417A80()
    {
        return storage_.stream.SizeWithApi(files_);
    }
    void Read(void* data, DWORD size)
    {
        if (!storage_.stream.ReadExactWithApi(data, size, files_)) throw 3;
    }
    void Write(const void* data, DWORD size)
    {
        if (!storage_.stream.WriteWithApi(data, size, files_)) throw 3;
    }
private:
    IStartFileApi& files_;
    union Storage
    {
        Storage() noexcept : stream() {}
        ~Storage() noexcept {}
        FileStreamRuntime stream;
    } storage_;
    static_assert(sizeof(Storage) == 0x10 && offsetof(FileStreamRuntime, fileAt04) == 4);
    bool cleanupArmed_ = false;
};

std::optional<std::uint8_t> ReadFlag(const ByteArrayReferenceRuntime& group, std::int32_t index)
{
    if (index < 0 || static_cast<std::uint32_t>(index) >= group.Size()) return std::nullopt;
    return *group.DataAt(static_cast<std::uint32_t>(index));
}

void SetFlag(ByteArrayReferenceRuntime& group, std::int32_t index)
{
    if (index < 0 || static_cast<std::uint32_t>(index) >= group.Size())
        throw std::out_of_range("CMessage unread index outside array");
    *group.DataAt(static_cast<std::uint32_t>(index)) = 1;
}
}

MessageReadFlagsRuntime::MessageReadFlagsRuntime(IStartFileApi& files,
    RuntimeStringBorrow directory) noexcept
    : files_(files), saveDirectory_(directory), standalone_(std::in_place), groups_(*standalone_) {}

MessageReadFlagsRuntime::MessageReadFlagsRuntime(IStartFileApi& files,
    const IMessageEnvironmentPathSource& paths) noexcept
    : files_(files), paths_(&paths), standalone_(std::in_place), groups_(*standalone_) {}

MessageReadFlagsRuntime::MessageReadFlagsRuntime(MessageReadFlagsStorage& storage,
    IStartFileApi& files, RuntimeStringBorrow directory) noexcept
    : files_(files), saveDirectory_(directory), groups_(storage) {}

MessageReadFlagsRuntime::MessageReadFlagsRuntime(MessageReadFlagsStorage& storage,
    IStartFileApi& files, const IMessageEnvironmentPathSource& paths) noexcept
    : files_(files), paths_(&paths), groups_(storage) {}

RuntimeStringBorrow MessageReadFlagsRuntime::CurrentSaveDirectory() const
{
    // 两个构造入口分别表示动态环境来源与明确固定对象借用，不在读取失败后切换来源。
    if (paths_) return paths_->CaptureUnreadPrefixAt43D3D0();
    return *saveDirectory_;
}

void MessageReadFlagsRuntime::LoadUnreadFlagsAt494280()
{
    try
    {
        for (auto& group : groups_) group.Reset();
        // 4145E0 拼接原字符串长度；不规范化目录、不自行补分隔符或改换编码。
        const RuntimeCStringConcatenation path(CurrentSaveDirectory(), "unread.ini", 10);
        if (path.Value().IsEmpty()) throw 1;
        UnreadFile file(files_);
        file.Open(path.GetString(), false);
        if (file.SizeAt417A80() < 4) return;
        // 4943B4 路径构造将同一 Buffer 写成 2；三个 count 读取之间继续复用。
        // 正常完整读取会覆盖它，但 FALSE/LastError=0 的未写字节必须保留原值。
        std::uint32_t count = 2;
        for (auto& group : groups_)
        {
            file.Read(&count, sizeof(count));
            if (count == 0) continue;
            group.AllocateAt412700(count);
            // 494481/4944C8/49450C仍以原Buffer长度读取，不补零未写尾部。
            // signed负count的原空指针/超长读不是合法输入，安全边界在分配副作用之后明确拒绝。
            if (count > 0x7FFFFFFFu)
                throw std::length_error("CMessage unread negative signed allocation count");
            file.Read(group.Data(), count);
        }
    }
    catch (...)
    {
        // EH 5E8E24 -> 5E8E10 -> handler(type=0, flags=40) 494560；保留部分已读数组。
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Warning,
            "CMessage unread load stopped at original 494560 catch; groups=%u/%u/%u",
            groups_[0].Size(), groups_[1].Size(), groups_[2].Size());
    }
}

namespace
{
template<class ResolveReader>
bool ReconcileCounts(MessageReadFlagsStorage& groups, const ResolveReader& resolve)
{
    using Reader = std::remove_cvref_t<decltype(resolve())>;
    // 45AA10/45AA50/45AA79 都在相等时跳过：三组必须全部不等才重建，不能改成 OR。
    // 原先保存数组count，再调用reader；不能依赖==两侧未规定的求值顺序。
    const auto matches = [&](const ByteArrayReferenceRuntime& group,
        std::int32_t (Reader::*count)() const noexcept)
    {
        const auto& message = resolve(); // 4664F0 在保存数组count之前解析当前reader。
        const auto previousCount = group.Size();
        return previousCount == static_cast<std::uint32_t>((message.*count)());
    };
    if (matches(groups[0], &Reader::GetFormatCount) ||
        matches(groups[1], &Reader::GetChoiceCount) ||
        matches(groups[2], &Reader::GetMessageCount)) return false;
    const auto rebuild = [&](ByteArrayReferenceRuntime& group,
        std::int32_t (Reader::*count)() const noexcept)
    {
        // 45AA8A/45AAA6等：先查询是否为零，非零分支再次查询才分配，不能缓存第一次返回。
        if ((resolve().*count)() == 0) group.Reset();
        else group.AllocateAt412700(static_cast<std::uint32_t>((resolve().*count)()));
        // 45AAB2/45AB05/45AB58在每组分配后重新取control+10，再显式清零该组。
        const auto length = group.Size();
        if (length != 0) std::memset(group.Data(), 0, length);
    };
    rebuild(groups[0], &Reader::GetFormatCount);
    rebuild(groups[1], &Reader::GetChoiceCount);
    rebuild(groups[2], &Reader::GetMessageCount);
    return true;
}
}

void MessageReadFlagsRuntime::ReconcileReaderCountsAt45A9D6(const IMessageRuntime& message)
{
    // 固定对象入口供明确借用该对象的调用方；生产CMessage使用下面的稳定槽入口。
    if (ReconcileCounts(groups_, [&]() -> const IMessageRuntime& { return message; })) PersistUnreadFlags();
}

void MessageReadFlagsRuntime::ReconcileCurrentReaderAt45A9D6(const MessageReaderReference& reader)
{
    const auto resolve = [&]() -> const NativeMessageReader& {
        const auto* current = reader.Get();
        if (!current) throw std::logic_error("CMessage count query has no reader");
        return *current;
    };
    if (ReconcileCounts(groups_, resolve)) PersistUnreadFlags();
}

void MessageReadFlagsRuntime::ReleaseArraysAt463250() noexcept
{
    // 463250..4632E4 主动按 +10、+1C、+28 释放，后续成员逆序析构仍正常执行。
    for (auto& group : groups_) group.Reset();
}

void MessageReadFlagsRuntime::PersistUnreadFlags()
{
    try
    {
        // 43D5D7仍处于state3：正常Free若抛出，5E0F0C展开会再次清理流和路径。
        // 不能直接用临时析构代替这个正常调用；494280/45A5D0没有相同的重复路径清理。
        union PathStorage
        {
            PathStorage() noexcept {}
            ~PathStorage() noexcept {}
            RuntimeCStringConcatenation path;
        } storage;
        auto* path = std::construct_at(&storage.path, CurrentSaveDirectory(), "unread.ini", 10);
        struct PathUnwindCleanup
        {
            RuntimeCStringConcatenation* value;
            ~PathUnwindCleanup() noexcept { if (value) std::destroy_at(value); }
        } cleanup{path};
        // 守卫早于流构造：异常展开先清流再清路径，不能到catch内才清路径，
        // 否则Free回调看到的uncaught_exceptions及二次异常边界都会改变。
        if (path->Value().IsEmpty()) throw 1;
        UnreadFile file(files_);
        file.Open(path->GetString(), true);
        for (const auto& group : groups_)
        {
            const auto count = group.Size();
            file.Write(&count, sizeof(count));
            // 43D524/43D56A/43D5B0保留已写出的count，Write回调返回后重取当前base。
            if (count != 0) file.Write(group.Data(), count);
        }
        // OPEN_ALWAYS从零覆盖，不截尾。关闭后仍保留原EH清理资格直到Free返回。
        file.CloseAt40ED70();
        DestroyRuntimeCStringAt40CD40(const_cast<RuntimeCString*>(&path->Value()));
        file.DisarmCleanup();
        cleanup.value = nullptr;
        // 成功时原路径只释放一次，命名union结束存储，不再调用临时析构。
    }
    catch (...)
    {
        // EH 5E0F0C -> 5E0EF8 -> handler(type=0, flags=40) 43D5EE：不向 caller 抛出。
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Error,
            "CMessage unread write stopped at original 43D5EE catch; read flags not rolled back");
    }
}

std::optional<std::uint8_t> MessageReadFlagsRuntime::ReadFormatFlag(std::int32_t index) const
{ return ReadFlag(groups_[0], index); }
void MessageReadFlagsRuntime::SetFormatRead(std::int32_t index)
{ SetFlag(groups_[0], index); }
std::optional<bool> MessageReadFlagsRuntime::ReadChoiceFlag(std::int32_t index) const
{
    const auto value = ReadFlag(groups_[1], index);
    return value ? std::optional<bool>(*value != 0) : std::nullopt;
}
void MessageReadFlagsRuntime::SetChoiceFlag(std::int32_t index)
{ SetFlag(groups_[1], index); }
std::span<const std::uint8_t> MessageReadFlagsRuntime::FormatFlagsAt10() const noexcept
{ return groups_[0].Bytes(); }
std::span<const std::uint8_t> MessageReadFlagsRuntime::ChoiceFlagsAt1C() const noexcept
{ return groups_[1].Bytes(); }
std::span<const std::uint8_t> MessageReadFlagsRuntime::BodyFlagsAt28() const noexcept
{ return groups_[2].Bytes(); }
}

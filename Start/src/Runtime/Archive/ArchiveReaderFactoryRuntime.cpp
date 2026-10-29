#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/ArchiveEntryTransforms.h>

#include <exception>
#include <limits>
#include <new>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
void Release(ArchiveReaderReferenceSlot& slot)
{
    auto* control = slot.controlAt08;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        // 47672A：正文回调可改槽，必须重新读取，且不对新 control 再递减。
        control = slot.controlAt08;
        if (control) control->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}

void Copy(ArchiveReaderReferenceSlot& target, const ArchiveReaderReferenceSlot& source) noexcept
{
    target = source;
    if (target.controlAt08) ++target.controlAt08->referenceCountAt04;
}

void* Allocate(IArchiveReaderMemoryApi& memory, ArchiveReaderAllocation kind, std::size_t bytes)
{
    return memory.Allocate(kind, bytes);
}

}

void* ArchiveReaderMemoryApi::Allocate(ArchiveReaderAllocation, std::size_t bytes)
    { return ::operator new(bytes); }

ArchiveReaderReference::ArchiveReaderReference(const ArchiveReaderReference& source) noexcept
    { Copy(slot_, source.slot_); }
ArchiveReaderReference::ArchiveReaderReference(ArchiveReaderReference&& source) noexcept
    : slot_(std::exchange(source.slot_, {})) {}
ArchiveReaderReference& ArchiveReaderReference::operator=(const ArchiveReaderReference& source)
{
    if (this != &source) { Release(slot_); Copy(slot_, source.slot_); }
    return *this;
}
ArchiveReaderReference& ArchiveReaderReference::operator=(ArchiveReaderReference&& source)
{
    if (this != &source) { Release(slot_); slot_ = std::exchange(source.slot_, {}); }
    return *this;
}
ArchiveReaderReference::~ArchiveReaderReference() noexcept { Release(slot_); }
void ArchiveReaderReference::Reset() { Release(slot_); }
ArchiveStreamReaderRuntime* ArchiveReaderReference::Get() const noexcept
{
    if (!slot_.baseAt00) return nullptr;
    const auto stride = slot_.controlAt08 ? slot_.controlAt08->strideAt0C : 0u;
    return reinterpret_cast<ArchiveStreamReaderRuntime*>(slot_.baseAt00 + slot_.biasAt04 * stride);
}
ArchiveReaderReference ArchiveReaderReference::Adopt(ArchiveReaderReferenceSlot& slot) noexcept
{
    ArchiveReaderReference result;
    result.slot_ = std::exchange(slot, {});
    return result;
}

ArchiveReaderFactoryRuntime::ArchiveReaderFactoryRuntime(IArchiveResourceSource& source,
    const FileTransactionContext* transaction, IStartFileApi& files, IArchiveReaderMemoryApi& memory,
    ArchiveCodecServices codecs) noexcept
    : source_(source), transaction_(transaction), files_(files), memory_(memory), codecs_(codecs) {}

ArchiveReaderFactoryRuntime::ArchiveReaderFactoryRuntime(const ResourceCatalog& catalog,
    const FileTransactionContext* transaction, IStartFileApi& files, IArchiveReaderMemoryApi& memory,
    ArchiveCodecServices codecs) noexcept
    : fixtureSource_(std::in_place, catalog), source_(*fixtureSource_),
      transaction_(transaction), files_(files), memory_(memory), codecs_(codecs) {}

ArchiveReaderFactoryRuntime::ArchiveReaderFactoryRuntime(const ResourceCatalog& catalog,
    const FileTransactionContext* transaction, IStartFileApi& files, IArchiveReaderMemoryApi& memory,
    ToolArchiveCodecsTag) noexcept
    : fixtureSource_(std::in_place, catalog), source_(*fixtureSource_),
      transaction_(transaction), files_(files), memory_(memory) {}

ArchiveReaderFactoryRuntime::ArchiveReaderFactoryRuntime(IArchiveResourceSource& source,
    const FileTransactionContext* transaction, IStartFileApi& files, IArchiveReaderMemoryApi& memory,
    ToolArchiveCodecsTag) noexcept
    : source_(source), transaction_(transaction), files_(files), memory_(memory) {}

void ArchiveReaderFactoryRuntime::PublishNewReader()
{
    auto* storage = Allocate(memory_, ArchiveReaderAllocation::Reader, sizeof(ArchiveFileReaderRuntime));
    ArchiveFileReaderRuntime* created;
    try { created = storage?new (storage) ArchiveFileReaderRuntime:nullptr; }
    catch (...)
    {
        ::operator delete(storage, std::size_t{0x14});
        throw;
    }

    // 483F24..483F8F：构造保护到此结束；旧释放失败不可擅自回收 created。
    // 先发布 base，再分配 control。分配失败保留 base/control=null，Reset 不会补清它。
    reader_.Reset();
    if(!created)return; // 483F5C：原空raw跳过adopt，后继读取按旧释放后的槽进入原失败点。
    if (reader_.slot_.controlAt08) reader_.slot_.controlAt08->DeleteControlAtVslot04(1);
    reader_.slot_.biasAt04 = 0;
    reader_.slot_.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    auto* control = Allocate(memory_, ArchiveReaderAllocation::ReaderControl, 0x18);
    reader_.slot_.controlAt08 = ConstructArchiveReaderControl(control, created);
    // 483F8F..FAD：发布后逐次重读槽，不在control构造里提前初始化这些字段。
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(reader_.slot_.controlAt08)+4u)=1;
    reader_.slot_.controlAt08->owningAt08 = 1;
    reader_.slot_.controlAt08->strideAt0C = 0x14;
    reader_.slot_.controlAt08->extentAt10 = 1;
}

bool ArchiveReaderFactoryRuntime::LoadUsing(const std::string* name, const char* borrowedName,
    std::span<const std::uint8_t> key, const IEnvironmentKeyTableSource* environmentKey,
    bool propagateFailure, std::uint32_t* transformVersion, std::int32_t* nativeQueryFieldAt10)
{
    std::exception_ptr failure;
    try
    {
        // 483DC3 的 catch 已生效；getter 抛出仍走本层失败清理，不能提前到构造器。
        const auto catalog = source_.CaptureCatalogAt413BE0();
        const auto query = name ? catalog.ResolveForRead(*name) : catalog.ResolveForReadBorrowed(borrowedName);
        if (!query) throw 0;
        ArchiveEntryReadStream stream(*query, transaction_, files_);
        stream.SeekToEntry();
        PublishNewReader();
        // 483FB4/4D4731：length 的读取晚于新正文、旧释放和新 control 发布。
        if (name) stream.ReadCurrentRangeInto(*name, reader_);
        else stream.ReadCurrentBorrowedRangeInto(borrowedName, reader_);

        ResolvedResourceEntry transform;
        transform.flags = query->Flags();
        const bool selectsKey = (transform.flags & 3u) == 0 && (transform.flags & 4u) != 0;
        {
            ByteArrayReferenceRuntime retainedKey;
            if (environmentKey && selectsKey)
            {
                // 4842B5/4D4A29取manager+1C；46DE00/46E090直接异或，包括原值0。
                // 47CB00的0->FF属于持久化key访问，不得挪到归档转换。
                retainedKey = environmentKey->AcquireRawBytes();
                // 4803EA..48040C -> 4D4B19：缺表/非法长度是int4，不是正文解码错误。
                if (!retainedKey) throw 4;
                key = retainedKey.Bytes();
            }
            if (selectsKey && (key.empty() || (key.size() & 31u) != 0u)) throw 4;
            transform.archiveVersion = query->Version();
            if (codecs_) TransformArchiveEntryPayloadInPlace(transform, reader_, key, *codecs_);
            else TransformArchiveEntryPayloadInPlace(transform, reader_, key); // 显式旧工具构造域。
            // 4D4A6B/4842F7：先释放临时key Ref，再读取下面的查询字段。
        }
        if (transformVersion) *transformVersion = transform.archiveVersion;
        // 4D4A70..4D4A79/4842FC..484305：变换成功后、局部stream关闭前晚读查询原槽。
        // 原生目录始终提供R；现代离线快照无此字段，不虚构四字节值或改变其工具域。
        if (nativeQueryFieldAt10) query->CopyNativeWordAt08(*nativeQueryFieldAt10);
    }
    catch (...) { failure = std::current_exception(); }

    if (!failure) return true;
    // 48432F/4D4AA3：流已展开，catch 清理本身的异常不可再次被这个 catch 吞掉。
    reader_.Reset();
    if (propagateFailure) std::rethrow_exception(failure);
    return false;
}

bool ArchiveReaderFactoryRuntime::Load(const std::string& name, std::span<const std::uint8_t> key)
    { return LoadUsing(&name, nullptr, key, nullptr, false); }
bool ArchiveReaderFactoryRuntime::Load(const std::string& name, const IEnvironmentKeyTableSource& key)
    { return LoadUsing(&name, nullptr, {}, &key, false); }
bool ArchiveReaderFactoryRuntime::LoadBorrowedAt483D90(const char* name, const IEnvironmentKeyTableSource& key,
    std::int32_t* nativeQueryFieldAt10)
    { return LoadUsing(nullptr, name, {}, &key, false, nullptr, nativeQueryFieldAt10); }
bool ArchiveReaderFactoryRuntime::LoadBorrowedAt4D44C0(const char* name, const IEnvironmentKeyTableSource& key,
    std::int32_t* nativeQueryFieldAt10)
{
    // 4D4AA0与48432C同为catch外476710清理；清理异常外传，不用LoadOrThrow工具契约。
    return LoadUsing(nullptr, name, {}, &key, false, nullptr, nativeQueryFieldAt10);
}

void ArchiveReaderFactoryRuntime::ReleaseReaderNormalAt4CCCDD() noexcept
{
    auto* const cached = reader_.slot_.controlAt08;
    if (cached && --cached->referenceCountAt04 == 0)
    {
        if (cached->owningAt08) cached->DestroyPayloadAtVslot00();
        cached->DeleteControlAtVslot04(1);
    }
    // 原尾声把缓存ESI置零，后继析构不再读取局部Ref。撤掉现代自动成员的清理资格，
    // 即使payload回调写入了新control，也不能补做一次原版不存在的释放。
    reader_.slot_.controlAt08 = nullptr;
    reader_.slot_.baseAt00 = 0;
}

std::uint32_t ArchiveReaderFactoryRuntime::LoadOrThrow(const std::string& name, const IEnvironmentKeyTableSource& key)
{
    std::uint32_t version = 0;
    static_cast<void>(LoadUsing(&name, nullptr, {}, &key, true, &version));
    return version; // 现代 SCR 元数据，不是原 factory+10 的未知 DWORD。
}
}

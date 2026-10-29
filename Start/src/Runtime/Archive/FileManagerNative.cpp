#include <Runtime/FileManagerNative.h>
#include <Runtime/RuntimeCStringConcatenation.h>

#include <cstring>
#include <memory>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class FileManagerControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit FileManagerControl(FileManagerNative28h* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* const body = static_cast<FileManagerNative28h*>(payloadAt14);
        if (!body) return;
        // 492C30通过正文当前表派发，不能让final类型使编译器直接调用具体析构。
        using Delete = FileManagerNative28h* (__thiscall*)(FileManagerNative28h*, std::uint32_t);
        const Delete* table;
        std::memcpy(&table, body, sizeof(table));
        table[0](body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(FileManagerControl) == 0x18);
}

FileManagerNative28h::FileManagerNative28h(IFileManagerNativeConstructionSource& source,
    InstallTableCatalogSource installed, IStartFileApi& files, const char* title,
    const YtlReferenceSlotRuntime& progress, const FileTransactionContext* transaction)
{
    // 458BB5的失败外传；其后的字段还没有写，不能值初始化整个对象。
    std::construct_at(&directoryAt04);
    scanStatusAt0C = 0;
    std::construct_at(&messageAt10);
    std::construct_at(&keyAt1C);
    try
    {
        if (!source.ReadEnvironmentReadyAt458C1E()) throw 1;
        ClearAt486990();
        const auto prefix = source.CaptureCachePrefixAt458C55();
        const RuntimeCStringConcatenation path(prefix, "finfo.ini", 9);
        if (path.GetString()[0] == '\0') throw 2;
        BuildNativeCatalogFromInstallTableWithCacheInto(directoryAt04, scanStatusAt0C,
            installed, path.GetString(), transaction, files, title, progress);
        source.CreateMessageAt4593AD(messageAt10);
    }
    catch (...)
    {
        // 459487是原catch-all：局部先展开，然后返回部分this，不回滚树/scan/message/key。
        // 这里不生成可抛诊断快照，业务结果由原成员和外层日志直接观察。
    }
}

void FileManagerNative28h::ClearAt486990()
{
    directoryAt04.Clear();
    ReleaseYtlReference(messageAt10);
}

FileManagerNative28h::~FileManagerNative28h() noexcept
{
    ClearAt486990();
    // 原重复阶段不能折叠；前一回调可填入后续槽。union禁止最后再自动清理晚填字段。
    keyAt1C.Reset();
    ReleaseYtlReference(messageAt10);
    std::destroy_at(&directoryAt04);
}

FileManagerNative28h* FileManagerNative28h::DeleteAtVslot00(std::uint32_t flags) noexcept
{
    this->~FileManagerNative28h();
    if (flags & 1u) ::operator delete(this, std::size_t{0x28});
    return this;
}

FileManagerNative28h* NewFileManagerAt489E56(IFileManagerNativeConstructionSource& source,
    InstallTableCatalogSource installed, IStartFileApi& files,
    const YtlReferenceSlotRuntime& progress, const FileTransactionContext* transaction)
{
    void* const storage = ::operator new(0x28);
    if (!storage) return nullptr;
    struct AllocationUnwind final
    {
        void* storage;
        ~AllocationUnwind() noexcept
        { if (storage) ::operator delete(storage, std::size_t{0x28}); }
    } unwind{storage};
    const char* const title = source.CaptureTitleAt489E7C();
    auto* const body = ::new (storage) FileManagerNative28h(source, installed, files, title, progress, transaction);
    unwind.storage = nullptr;
    return body;
}

void InstallFileManagerAt489E8D(YtlReferenceSlotRuntime& destination, FileManagerNative28h* created)
{
    // 489E95..489EC6：释放段结束时ECX已为0；无旧control的分支ECX本来就是0。
    ReleaseYtlReference(destination);
    if (!created) return;
    destination.biasAt04 = 0;
    destination.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    void* const storage = ::operator new(0x18);
    auto* const control = storage ? ::new (storage) FileManagerControl(created) : nullptr;
    destination.controlAt08 = control;
    control->referenceCountAt04 = 1;
    destination.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(destination.controlAt08)->strideAt0C = 0x28;
    static_cast<EnvironmentObjectReferenceControl*>(destination.controlAt08)->countAt10 = 1;
}

void ReleaseFileManagerAt4777C9(YtlReferenceSlotRuntime& reference) { ReleaseYtlReference(reference); }
void ReleaseFileManagerAt46444F(YtlReferenceSlotRuntime& reference) noexcept { ReleaseYtlReference(reference); }
FileManagerNative28h* ResolveFileManagerAt479FF0(const YtlReferenceSlotRuntime& reference) noexcept
{ return ResolveEnvironmentObjectReference<FileManagerNative28h>(reference); }
}

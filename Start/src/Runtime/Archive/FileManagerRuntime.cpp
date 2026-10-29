#include <Runtime/FileManagerRuntime.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Logging/Logger.h>

#include <limits>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
class FileManagerMessageFactory final : public IFileManagerMessageFactory
{
public:
    FileManagerMessageFactory(const FileManagerConstructionInput& input, IStartFileApi& files,
        const FileTransactionContext* transaction) noexcept
        : input_(input), files_(files), transaction_(transaction) {}

    std::unique_ptr<MessageOwnerRuntime> CreateMessageOwner() override
    {
        if (input_.messagePaths)
            return std::make_unique<MessageOwnerRuntime>(files_, *input_.messagePaths, transaction_);
        return std::make_unique<MessageOwnerRuntime>(
            files_, input_.messagePrefixAt08, input_.saveDirectory, transaction_);
    }
private:
    const FileManagerConstructionInput& input_;
    IStartFileApi& files_;
    const FileTransactionContext* transaction_;
};
}

FileManagerRuntime::FileManagerRuntime(const FileManagerConstructionInput& input,
    IStartFileApi& files, IFileInfoProgress& progress, const FileTransactionContext* transaction)
    : directory_(std::make_unique<InstallTableCatalogBuildResult>())
{
    directory_->catalog.InitializeNativeMap(files); // 458BB5：sentinel失败不进入459487的构造catch。
    FileManagerMessageFactory messages(input, files, transaction);
    const YtlReferenceSlotRuntime borrowed{reinterpret_cast<std::uintptr_t>(&progress), 0, nullptr};
    ConstructAt458B60(input, files, borrowed, messages, transaction, nullptr);
}

FileManagerRuntime::FileManagerRuntime(const FileManagerConstructionInput& input,
    IStartFileApi& files, IFileInfoProgress& progress, IFileManagerMessageFactory& messages,
    const FileTransactionContext* transaction)
    : directory_(std::make_unique<InstallTableCatalogBuildResult>())
{
    directory_->catalog.InitializeNativeMap(files);
    const YtlReferenceSlotRuntime borrowed{reinterpret_cast<std::uintptr_t>(&progress), 0, nullptr};
    ConstructAt458B60(input, files, borrowed, messages, transaction, nullptr);
}

FileManagerRuntime::FileManagerRuntime(const FileManagerConstructionInput& input,
    IStartFileApi& files, const YtlReferenceSlotRuntime& progress, const char* capturedTitle,
    const FileTransactionContext* transaction)
    : directory_(std::make_unique<InstallTableCatalogBuildResult>())
{
    directory_->catalog.InitializeNativeMap(files);
    FileManagerMessageFactory messages(input, files, transaction);
    ConstructAt458B60(input, files, progress, messages, transaction, capturedTitle);
}

void FileManagerRuntime::ConstructAt458B60(const FileManagerConstructionInput& input,
    IStartFileApi& files, const YtlReferenceSlotRuntime& progress, IFileManagerMessageFactory& messages,
    const FileTransactionContext* transaction, const char* capturedTitle)
{
    try
    {
        if (!input.environmentReadyAt00) throw 1;
        ClearAt486990();
        const RuntimeStringBorrow cachePrefix = input.messagePaths ?
            input.messagePaths->CaptureUnreadPrefixAt43D3D0() : input.cachePrefixAt10;
        const RuntimeCStringConcatenation cachePath(cachePrefix, "finfo.ini", 9);
        // 458CCB比较的是C字符串而非CString记录长度，内嵌NUL首字节必须拒绝。
        if (cachePath.GetString()[0] == '\0') throw 2;
        BuildCatalogFromInstallTableWithCacheInto(
            *directory_, scanStatusAt0C_, input.installTable, cachePath.GetString(), transaction, files,
            capturedTitle, progress);
        auto next = messages.CreateMessageOwner();
        if (!next) throw std::logic_error("CFileManager message factory returned no object");
        // 4593CF..459420：新正文已构造，但旧消息必须先释放，再发布新正文。
        message_.reset();
        message_ = std::move(next);
    }
    catch (...)
    {
        // 原5E9784 states3..24 ->459487：返回部分this，不回滚目录或已经写入的+0C。
        KaGuYa::Logging::WriteFormat(KaGuYa::Logging::Level::Warning,
            "CFileManager 459487: retained partial state scan=%u archives=%zu message=%u",
            scanStatusAt0C_, directory_->catalog.ArchiveCount(), message_ ? 1u : 0u);
    }
}

void FileManagerRuntime::ClearAt486990() noexcept
{
    directory_->catalog.Clear();
    directory_->archives.clear();
    directory_->totalFileCount = 0;
    directory_->maximumDataLength = 0;
    directory_->cacheHitCount = 0;
    directory_->directScanCount = 0;
    directory_->cacheRewritten = false;
    message_.reset();
}

void FileManagerRuntime::AdoptInstalledKey(std::unique_ptr<EnvironmentKeyByteTable> key)
{
    // 显式工具入口只转交现有数组Ref；生产安装使用下面的原位412700分配。
    key_.Reset();
    if (key) key_ = key->AcquireRawBytes();
}

void FileManagerRuntime::InstallKeyFromSnapshot(std::vector<std::byte>& source)
{
    // 工具快照没有原GS身份；原生启动入口不经过此函数。
    if (source.empty())
    {
        std::vector<std::byte>{}.swap(source);
        return;
    }
    const std::span<const std::uint8_t> bytes(
        reinterpret_cast<const std::uint8_t*>(source.data()), source.size());
    const auto layout = ParamsRawBlobInstaller::InspectRuntimeSource(bytes);

    // 48A039/48A0C4：先解除旧Ref，再分配未初始化正文，发布base之后才分配control。
    // 失败保留已发布槽，不恢复旧key，也不以vector的零初始化覆盖未写区域。
    key_.AllocateAt412700(static_cast<std::uint32_t>(layout.byteCount));
    // 48A100没有按零输出跳过decoder；异常保留已发布key和params源。
    ParamsRawBlobInstaller::InstallIntoRuntime(bytes, key_.MutableBytes());
    // 48A12B..48A160在填充正常返回后解除源，而非等待整个root初始化成功。
    // 这里是唯一拥有的vector，不模拟YTL共享引用/control的析构回调。
    std::vector<std::byte>{}.swap(source);
}

FileManagerRuntime::~FileManagerRuntime() noexcept
{
    // 462EC0/4689A0 EHFlags5；保留主动清理后key、消息、目录的重复释放阶段。
    ClearAt486990();
    key_.Reset();
    message_.reset();
    directory_->catalog.Clear();
    directory_.reset();
}
}

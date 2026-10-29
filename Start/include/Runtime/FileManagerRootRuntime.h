#pragma once

#include <memory>

#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/FileInfoProgressReference.h>
#include <Runtime/FileManagerNative.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRootOwner;
class FileManagerRootRuntime;
// 原489C90局部Ref只借给本轮getter；嵌套调用有各自栈Ref，不覆盖现代拥有槽。
class FileManagerInitializationProgressBorrow final
{
public:
    FileManagerInitializationProgressBorrow(const FileManagerRootRuntime&, IFileInfoProgress&) noexcept;
    ~FileManagerInitializationProgressBorrow() noexcept;
    FileManagerInitializationProgressBorrow(const FileManagerInitializationProgressBorrow&) = delete;
    FileManagerInitializationProgressBorrow& operator=(const FileManagerInitializationProgressBorrow&) = delete;
    static IFileInfoProgress* Find(const FileManagerRootRuntime&) noexcept;
private:
    const FileManagerRootRuntime& owner_;
    IFileInfoProgress& progress_;
    FileManagerInitializationProgressBorrow* previous_;
    static thread_local FileManagerInitializationProgressBorrow* current_;
};

// 来源服务长于每一代root正文；Capture对应原413BE0，Published仅用于已捕获借用的验证。
class IFileManagerRootReferenceSource
{
public:
    virtual ~IFileManagerRootReferenceSource() = default;
    virtual YtlReferenceSlotRuntime& CaptureFileManagerSlotAt413BE0() = 0;
    [[nodiscard]] virtual const YtlReferenceSlotRuntime* PublishedFileManagerSlot() const noexcept = 0;
};

class IFileManagerStartupFactory
{
public:
    virtual ~IFileManagerStartupFactory() = default;
    virtual std::unique_ptr<IFileInfoProgress> CreateProgress() = 0;
    // 489E8D已撤销正文分配的回滚资格，返回完成的裸28h，不附自动删除owner。
    virtual FileManagerNative28h* CreateManagerFromReference(const YtlReferenceSlotRuntime& progress) = 0;
};

class IFileManagerStartupErrorReport
{
public:
    virtual ~IFileManagerStartupErrorReport() = default;
    virtual void ReportScanFailure(HWND progressWindow) = 0;
};

// 正文只由真实root+00 Ref拥有；本服务仅借稳定根来源并持489C90本轮局部progress。
// 不保存某一代manager/目录/key，也不在服务析构中第二次释放root槽。
class FileManagerRootRuntime final : public IEnvironmentKeyTableSource
{
public:
    explicit FileManagerRootRuntime(IFileManagerRootReferenceSource& source) noexcept : source_(source) {}
    ~FileManagerRootRuntime() noexcept;
    FileManagerRootRuntime(const FileManagerRootRuntime&) = delete;
    FileManagerRootRuntime& operator=(const FileManagerRootRuntime&) = delete;

    void CreateFileManager(YtlReferenceSlotRuntime& selectedRootSlot, HINSTANCE resourceInstance,
        RuntimeStringBorrow titleCp932);
    void CreateFileManager(YtlReferenceSlotRuntime& selectedRootSlot, IFileManagerStartupFactory& factory);
    void VerifyStartupFileSet(IFileManagerStartupErrorReport& report);
    void InstallEnvironmentKeyTable(ParamsRuntimeRootOwner& params, ArchiveCodecServices codecs);
    void ReleaseInitializationProgress() noexcept;
    void ReleaseFileManager(YtlReferenceSlotRuntime& selectedRootSlot);

    [[nodiscard]] FileManagerNative28h* ManagerAt00() const noexcept;
    [[nodiscard]] FileManagerNative28h* CaptureManagerAt413BE0() const;
    [[nodiscard]] IFileInfoProgress* InitializationProgress() const noexcept;
    [[nodiscard]] ByteArrayReferenceRuntime AcquireRawBytes() const override;
    [[nodiscard]] std::uint8_t NormalizedByteAt(std::uint32_t index) const override;
    [[nodiscard]] std::size_t RawByteCount() const override;
private:
    IFileManagerRootReferenceSource& source_;
    FileInfoProgressReference progress_;
};
}

#pragma once

#include <Runtime/ArchiveNamespaceMapRuntime.h>
#include <Runtime/ByteArrayReferenceRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/InstallTableCatalogBuilder.h>
#include <Runtime/RuntimeStringBorrow.h>

namespace KaGuYa::Start::Runtime
{
// 各方法只表示一个原取源时点。服务不存入28h正文；默认进程装配仍须提供真实来源。
class IFileManagerNativeConstructionSource
{
public:
    virtual ~IFileManagerNativeConstructionSource() = default;
    virtual const char* CaptureTitleAt489E7C() = 0;
    virtual bool ReadEnvironmentReadyAt458C1E() = 0;
    virtual RuntimeStringBorrow CaptureCachePrefixAt458C55() = 0;
    // 包含4593AD raw分配、45A5D0构造和4593CF..45945A替换；不是测试消息成功占位。
    virtual void CreateMessageAt4593AD(YtlReferenceSlotRuntime& destination) = 0;
};

// 5B2B20仅有一个flags删除槽。所有实际拥有字段均原位，不附诊断或服务sidecar。
class FileManagerNative28h final
{
public:
    FileManagerNative28h(IFileManagerNativeConstructionSource& source,
        InstallTableCatalogSource installed, IStartFileApi& files,
        const char* capturedTitle, const YtlReferenceSlotRuntime& progress,
        const FileTransactionContext* transaction = nullptr);
    ~FileManagerNative28h() noexcept;
    FileManagerNative28h(const FileManagerNative28h&) = delete;
    FileManagerNative28h& operator=(const FileManagerNative28h&) = delete;
    virtual FileManagerNative28h* DeleteAtVslot00(std::uint32_t flags) noexcept;

    // 普通486990允许message Ref的动态释放外传；完整析构中的调用则由其noexcept终止。
    void ClearAt486990();
    [[nodiscard]] ByteArrayReferenceRuntime& KeyReferenceAt1C() noexcept { return keyAt1C; }
    [[nodiscard]] const ByteArrayReferenceRuntime& KeyReferenceAt1C() const noexcept { return keyAt1C; }
    union { ArchiveNamespaceMapRuntime directoryAt04; };
    std::uint32_t scanStatusAt0C;
    union { YtlReferenceSlotRuntime messageAt10; };
    union { ByteArrayReferenceRuntime keyAt1C; };
};
static_assert(sizeof(FileManagerNative28h) == 0x28 && alignof(FileManagerNative28h) == 4);
static_assert(offsetof(FileManagerNative28h, directoryAt04) == 4);
static_assert(offsetof(FileManagerNative28h, scanStatusAt0C) == 0x0C);
static_assert(offsetof(FileManagerNative28h, messageAt10) == 0x10);
static_assert(offsetof(FileManagerNative28h, keyAt1C) == 0x1C);

// 先申请28h，之后才捕获标题；仅构造失败保有raw块回收资格。
[[nodiscard]] FileManagerNative28h* NewFileManagerAt489E56(
    IFileManagerNativeConstructionSource& source, InstallTableCatalogSource installed,
    IStartFileApi& files, const YtlReferenceSlotRuntime& progress,
    const FileTransactionContext* transaction = nullptr);
// 无上下文的原入口：逐次取唯一Process环境，消息及文件叶使用各自原默认来源。
[[nodiscard]] FileManagerNative28h* NewFileManagerAt489E56(const YtlReferenceSlotRuntime& progress);
// 接收已完成的正文。旧槽释放失败、18h分配失败都不能回滚或自动回收新正文。
void InstallFileManagerAt489E8D(YtlReferenceSlotRuntime& destination, FileManagerNative28h* created);
void ReleaseFileManagerAt4777C9(YtlReferenceSlotRuntime& reference);
void ReleaseFileManagerAt46444F(YtlReferenceSlotRuntime& reference) noexcept;
[[nodiscard]] FileManagerNative28h* ResolveFileManagerAt479FF0(const YtlReferenceSlotRuntime& reference) noexcept;
}

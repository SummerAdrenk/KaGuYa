#pragma once

#include <Runtime/SaveFileCatalog.h>
#include <Runtime/ScrSaveFileArchiveStateRuntime.h>

namespace KaGuYa::Start::Runtime
{
// 4706C5 构造一次临时 CParams，470A33/47154F/471B27 共用，471F12 析构。
// 每次扫描新建 session；不接收 live generation，不在槽位之间 Reset/Replace。
class SaveFileCatalogReadSession final : public ISaveFileMetadataLoader
{
public:
    // 文件 API、save、目录、random、profile 均借用，必须活过 session。
    // 目录保留原 CP932 字节及末尾分隔符；save/random 仅为 container 构造依赖，读路径不使用。
    SaveFileCatalogReadSession(IStartFileApi& files, const SaveDataEnvironmentArchive& save,
        RuntimeStringBorrow directoryCp932, IScrVmProgramRandSource& random,
        const SaveFileFormatProfile& profile);

    SaveFileCatalogReadSession(const SaveFileCatalogReadSession&) = delete;
    SaveFileCatalogReadSession& operator=(const SaveFileCatalogReadSession&) = delete;
    SaveFileCatalogReadSession(SaveFileCatalogReadSession&&) = delete;
    SaveFileCatalogReadSession& operator=(SaveFileCatalogReadSession&&) = delete;

    // 48A1E3 -> 470630：原地写 owner 的 slot6；一次 session 只允许一次扫描尝试。
    // 构造/提取/目录分配异常外传；失败扫描不可用同一 session 重试。
    void ScanInto(SaveFileStateNativeArchive& destination, SaveFileCatalogScanState& diagnostics);
    void ScanFrom(ISaveFileCatalogScanSource& source);

    // 三个循环都以 mode=1 完整读取。false 不改 metadata，临时对象保留已读前缀。
    // 470BD4/4716C3/471C80：description 非空时不提取 title，secondaryLabel 留空。
    [[nodiscard]] bool TryLoad(const std::string& path, SaveFileMetadata& metadata) override;

    // 只读观察实际临时正文；借用仅在本 session 生命周期内有效。
    [[nodiscard]] const ScrVmParamsGeneration& TemporaryParams() const noexcept;

private:
    RuntimeStringBorrow directory_;
    ScrVmParamsGeneration temporary_;
    ScrSaveFileArchiveStateRuntime archive_;
    SaveFileContainerRuntime container_;
    bool scanStarted_ = false;
};
}

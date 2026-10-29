#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <Runtime/RuntimeStringBorrow.h>
#include <Runtime/SaveFileStateNativeArchive.h>
#include <memory>

namespace KaGuYa::Start::Runtime
{
struct SaveFileMetadata final
{
    std::uint64_t lastWriteTime = 0;
    std::string primaryLabel;   // CSaveParams+4：目录优先显示的说明。
    std::string secondaryLabel; // CSaveParams+8：说明为空时使用的标题。
};

class ISaveFileMetadataLoader
{
public:
    virtual ~ISaveFileMetadataLoader() = default;

    // 目录元数据查询边界，不等同于恢复当前CParams的4C7420。
    // 原4C7420的catch-all已经由SaveFileContainerRuntime单独实现。
    [[nodiscard]] virtual bool TryLoad(
        const std::string& path,
        SaveFileMetadata& metadata) = 0;
    // 旧目录的存在性和Quick/Auto排序来自文件系统，不能用容器解码成功代替。
    virtual bool HasMatchingFiles(const std::string& pattern);
    virtual bool TryReadFileWriteTime(const std::string& path, std::uint64_t& seconds);
};

using SaveFileCatalogItem = SaveFileNativeItem;
using SaveFileCatalogPage = SaveFileNativePage;
using SaveFileCatalog = SaveFileStateNativeArchive;

int SaveFilePhysicalPageNumber(const SaveFileCatalog& catalog, int displayIndex);
int SaveFileRollingSlot(const SaveFileCatalog& catalog, bool automatic, int displaySlot);
// 01的4940C0/4977E0及各款对应证据：有序插入空页，整页删除移除目录项。
// 新增满目录返回-1；调用UI必须显示原上限提示，不静默成功。
int AddSaveFileCatalogPage(SaveFileCatalog& catalog);
void RemoveSaveFileCatalogPage(SaveFileCatalog& catalog, int displayIndex);

// 只记录写入来源，不复制 catalog 数值；重扫期间未覆盖的字段保留上次来源。
struct SaveFileCatalogScanState final
{
    bool regularLatestWritten = false;
    bool regularSelectionWritten = false;
    bool quickLatestWritten = false;
    bool autoLatestWritten = false;
    bool quickPageCreated = false;
    bool autoPageCreated = false;
    bool complete = false;
};

// 470630每个原getter独立捕获，不保活上一次slot6或路径对象。
class ISaveFileCatalogScanSource
{
public:
    virtual ~ISaveFileCatalogScanSource() = default;
    virtual SaveFileStateNativeArchive& CaptureSaveFileStateAt4180C0() = 0;
    virtual RuntimeStringBorrow CaptureSaveDirectoryAt4180C0() = 0;
};

class SaveFileCatalogScanner final
{
public:
    explicit SaveFileCatalogScanner(ISaveFileMetadataLoader& loader) noexcept;

    // directoryPrefix 与原字符串拼接保持一致，调用方负责提供末尾目录分隔符。
    [[nodiscard]] std::unique_ptr<SaveFileCatalog> Scan(RuntimeStringBorrow directoryPrefix);

    // 470630 的 catalog 写入顺序；不替换目标对象，不吞掉 loader 异常。
    // state 必须始终与同一 catalog 配对；元数据加载仍是 ISaveFileMetadataLoader 边界。
    // 显式固定正文接口供离线查询/诊断；正式Environment扫描调用下面的ScanFrom。
    // 临时CParams/元数据提取仍由session提供，不把扫描器独自标成完整470630。
    void ScanInto(SaveFileCatalog& catalog, SaveFileCatalogScanState& state,
        RuntimeStringBorrow directoryPrefix);

    // 生产入口：不附带镜像catalog或按单一对象记录的诊断资格。
    void ScanFrom(ISaveFileCatalogScanSource& source);

private:
    [[nodiscard]] bool LoadItem(SaveFileNativeItem& item, const std::string& path);
    void ScanFrom(ISaveFileCatalogScanSource& source, SaveFileCatalogScanState* fixedDiagnostics);

    ISaveFileMetadataLoader& loader_;
};
}

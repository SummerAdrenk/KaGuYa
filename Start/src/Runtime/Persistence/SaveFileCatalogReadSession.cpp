#include <Runtime/SaveFileCatalogReadSession.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
SaveFileCatalogReadSession::SaveFileCatalogReadSession(IStartFileApi& files,
    const SaveDataEnvironmentArchive& save, RuntimeStringBorrow directoryCp932,
    IScrVmProgramRandSource& random, const SaveFileFormatProfile& profile)
    : directory_(directoryCp932),
      archive_(temporary_), container_(files, archive_, save, directory_, random, profile)
{
}

void SaveFileCatalogReadSession::ScanInto(SaveFileStateNativeArchive& destination,
    SaveFileCatalogScanState& diagnostics)
{
    if (scanStarted_) throw std::logic_error("save catalog read session has already scanned");
    scanStarted_ = true;
    SaveFileCatalogScanner scanner(*this);
    scanner.ScanInto(destination, diagnostics, directory_);
}

void SaveFileCatalogReadSession::ScanFrom(ISaveFileCatalogScanSource& source)
{
    if (scanStarted_) throw std::logic_error("save catalog read session has already scanned");
    scanStarted_ = true;
    SaveFileCatalogScanner scanner(*this);
    scanner.ScanFrom(source);
}

bool SaveFileCatalogReadSession::TryLoad(const std::string& path, SaveFileMetadata& metadata)
{
    // 470A2E/47154A/471B24 push 1；4C7420 自身 catch-all，不扩大到下面的提取。
    if (!container_.ReadAt4C7420(path, std::uint8_t{1})) return false;

    const auto& current = TemporaryParams();
    SaveFileMetadata loaded;
    loaded.lastWriteTime = current.GetSaveTimestamp();
    // 470B3F/47162E/471BE8 先取 CSaveParams+04；磁盘首字符串则是 +08。
    loaded.primaryLabel = RuntimeStringView(current.SaveParamsAt08().saveDescriptionAt04);
    if (loaded.primaryLabel.empty())
        loaded.secondaryLabel = RuntimeStringView(current.SaveParamsAt08().saveTitleAt08);
    metadata = std::move(loaded);
    return true;
}

const ScrVmParamsGeneration& SaveFileCatalogReadSession::TemporaryParams() const noexcept
{
    return temporary_;
}
}

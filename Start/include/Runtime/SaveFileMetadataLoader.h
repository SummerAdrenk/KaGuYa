#pragma once

#include <Runtime/SaveFileCatalog.h>
#include <Runtime/RuntimeCString.h>

#include <cstddef>
#include <span>
#include <string>

namespace KaGuYa::Start::Runtime
{
// 原生域只借全局CString槽；默认空std::string不分配、不复制魔数或取得析构资格。
using SaveFileMagicNativeSlot = RuntimeCString;
struct SaveFileFormatProfile final
{
    // 文件中不带 NUL；必须保存原始窄字节，不能经当前系统代码页重新编码。
    std::string magicBytes;
    const SaveFileMagicNativeSlot* nativeMagicSlot = nullptr;
};

// 18#ママトモ 的 Start.exe:4055C0 从 0x5B5B14 复制的 32 字节魔数。
[[nodiscard]] SaveFileFormatProfile CreateMamatomoSaveFileFormatProfile();
[[nodiscard]] SaveFileFormatProfile CreateNativeSaveFileFormatProfile(const SaveFileMagicNativeSlot& slot) noexcept;

// 独立元数据解析器：验证三块正文后提取标签，不恢复运行对象。
// 格式错误返回false，分配异常外传；这不是原4C7420的catch-all契约。
// 完整读取、mode0、逐块提交与原异常返回由SaveFileContainerRuntime实现。
[[nodiscard]] bool TryParseSaveFileMetadataBytes(
    std::span<const std::byte> fileBytes,
    const SaveFileFormatProfile& profile,
    SaveFileMetadata& metadata);

class StartSaveFileMetadataLoader final : public ISaveFileMetadataLoader
{
public:
    explicit StartSaveFileMetadataLoader(SaveFileFormatProfile profile);

    [[nodiscard]] bool TryLoad(
        const std::string& path,
        SaveFileMetadata& metadata) override;

private:
    SaveFileFormatProfile profile_;
};
}

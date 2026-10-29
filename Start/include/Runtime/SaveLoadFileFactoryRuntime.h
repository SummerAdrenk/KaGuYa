#pragma once

#include <Runtime/SaveLoadDialogRuntime.h>

#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
class SaveFileContainerRuntime;

// 主线根截图 owner 实现；不在文件 factory 中假设 Surface/Graphics 或补图回退。
class ISaveLoadPreviewServices
{
public:
    virtual ~ISaveLoadPreviewServices() = default;
    virtual void CaptureForSaveAt48DBE0() = 0;
    [[nodiscard]] virtual bool SavePreviewAt489420(const std::string& fullPath) = 0;
};

class ISaveLoadDeleteFileApi
{
public:
    virtual ~ISaveLoadDeleteFileApi() = default;
    // 原 DeleteFileA 的 BOOL；factory 明确丢弃返回值，C++ 异常不在删除入口吞掉。
    [[nodiscard]] virtual bool DeleteFileAt47655C(const std::string& fullPath) = 0;
};

class Win32SaveLoadDeleteFileApi final : public ISaveLoadDeleteFileApi
{
public:
    [[nodiscard]] bool DeleteFileAt47655C(const std::string& fullPath) override;
};

class SaveLoadFileFactoryRuntime final : public ISaveLoadFileFactory, public ISaveLoadRegularSaveFactory
{
public:
    // prefix 为原 factory+04，slotCount 为+08；例如普通页前缀由主线按原 caller 提供。
    // latest 必须绑定真实目录字段，DeleteAll 首先将它写为 -1，不能用内部影子值。
    // 除拥有的 prefix/slotCount 外均借用，依赖必须活过 factory；目录不自动补分隔符。
    SaveLoadFileFactoryRuntime(SaveFileContainerRuntime& container, ISaveLoadPreviewServices& preview,
        ISaveLoadDeleteFileApi& files, const std::string& saveDirectoryCp932,
        std::string prefixCp932, std::int32_t slotCount, std::int32_t& latest);

    [[nodiscard]] std::string BuildDataFileNameAt47AF10(std::int32_t slotNumber);
    [[nodiscard]] std::string BuildPreviewFileNameAt47AFA0(std::int32_t slotNumber) override;
    [[nodiscard]] std::int32_t LoadSlotAt480830(std::int32_t slotNumber) override;

    // 既有普通保存接口只转 0/1；显式 int32 重载保留 mode2 的重新截图。
    [[nodiscard]] std::int32_t SaveSlotAt488CF0(std::int32_t slotNumber, bool writeScreenshot) override;
    [[nodiscard]] std::int32_t SaveSlotAt488CF0(std::int32_t slotNumber, std::int32_t previewMode);
    void DeleteSlotAt476460(std::int32_t slotNumber, bool deleteDataFile) override;
    void DeleteAllAt5022E0() override;

private:
    SaveFileContainerRuntime& container_;
    ISaveLoadPreviewServices& preview_;
    ISaveLoadDeleteFileApi& files_;
    const std::string& directory_;
    std::string prefix_;
    std::int32_t slotCount_;
    std::int32_t& latest_;
};
}

#pragma once

#include <Windows.h>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
class SaveFileStateNativeArchive;
using SaveFileCatalog = SaveFileStateNativeArchive;
class SaveDataEnvironmentArchive;
class SaveFileContainerRuntime;
class ISaveLoadPreviewServices;
class ISaveLoadDeleteFileApi;

class IQuickLoadConfirmation
{
public:
    virtual ~IQuickLoadConfirmation() = default;
    virtual void CaptureDisplayAt48CA30() = 0;
    [[nodiscard]] virtual std::int32_t RunModalAt5071D0() = 0;
};

class IQuickLoadConfirmationServices
{
public:
    virtual ~IQuickLoadConfirmationServices() = default;
    // 必须创建4E3330(mode=4)；包装持有真实强Ref，不能默认确认或替换成其他框。
    [[nodiscard]] virtual std::unique_ptr<IQuickLoadConfirmation>
        CreateQuickLoadConfirmationAt4E3330() = 0;
};

enum class QuickLoadFailure : std::int32_t
{
    NoSavedData = 1,
    LoadFailed = 2,
};

[[nodiscard]] std::string_view QuickLoadFailureTextCp932(QuickLoadFailure failure);

class IQuickLoadFailureDisplay
{
public:
    virtual ~IQuickLoadFailureDisplay() = default;
    virtual void ShowFailureAt485F39(QuickLoadFailure failure) = 0;
};

class Win32QuickLoadFailureDisplay final : public IQuickLoadFailureDisplay
{
public:
    // 对应Environment根+2C/+10，借用字段而非复制初值；调用时读取当前值。
    Win32QuickLoadFailureDisplay(const HWND& window,
        const std::string& captionCp932) noexcept;
    void ShowFailureAt485F39(QuickLoadFailure failure) override;

private:
    const HWND& window_;
    const std::string& caption_;
};

class QuickLoadRuntime final
{
public:
    // container必须绑定同一live generation的真实archive，不可用临时读档状态替代。
    // preview/delete借现成完整factory所需依赖，本入口不执行保存、预览或删除。
    QuickLoadRuntime(SaveFileCatalog& catalog, const SaveDataEnvironmentArchive& save,
        SaveFileContainerRuntime& container, ISaveLoadPreviewServices& preview,
        ISaveLoadDeleteFileApi& deletion, const std::string& directoryCp932,
        IQuickLoadConfirmationServices& confirmation, IQuickLoadFailureDisplay& failures) noexcept;

    // 原入口无参数，仅AL有定义。这里只完成读档，不Queue/Clear、不恢复场景。
    [[nodiscard]] bool QuickLoadAt485CE0();

private:
    SaveFileCatalog& catalog_;
    const SaveDataEnvironmentArchive& save_;
    SaveFileContainerRuntime& container_;
    ISaveLoadPreviewServices& preview_;
    ISaveLoadDeleteFileApi& deletion_;
    const std::string& directory_;
    IQuickLoadConfirmationServices& confirmation_;
    IQuickLoadFailureDisplay& failures_;
};
}

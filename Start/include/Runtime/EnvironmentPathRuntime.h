#pragma once

#include <Windows.h>
#include <Runtime/InstallTableCatalogBuilder.h>
#include <Runtime/RuntimeCString.h>
#include <Runtime/EnvironmentInstalledFiles.h>

#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
struct SystemDataInstallFileInfo;
struct FileManagerConstructionInput;
class IEnvironmentPathErrorSink
{
public:
    virtual ~IEnvironmentPathErrorSink() = default;
    // 4769B0向同一个Environment+3C错误数组追加，不在此弹窗或吞掉异常。
    virtual void AppendPathError(std::string textCp932) = 0;
    // 46A0F6/46A123先固定格式参数，再取Environment；不得提前拼好错误文本。
    virtual void AppendMissingInstallFile(const char* nameCp932);
    virtual void AppendInsertInstallDisc(int driveByte);
};

class IEnvironmentPathApi
{
public:
    virtual ~IEnvironmentPathApi() = default;
    virtual DWORD CurrentDirectory(DWORD capacity, char* buffer) = 0;
    virtual DWORD ProfileString(const char* section, const char* key, const char* defaultValue,
        char* buffer, DWORD capacity, const char* fileName) = 0;
    virtual DWORD Attributes(const char* path) = 0;
    virtual DWORD LastError() = 0;
    virtual BOOL CreatePathDirectory(const char* path) = 0;
};

class Win32EnvironmentPathApi final : public IEnvironmentPathApi
{
public:
    DWORD CurrentDirectory(DWORD capacity, char* buffer) override;
    DWORD ProfileString(const char* section, const char* key, const char* defaultValue,
        char* buffer, DWORD capacity, const char* fileName) override;
    DWORD Attributes(const char* path) override;
    DWORD LastError() override;
    BOOL CreatePathDirectory(const char* path) override;
};

// 494CF0：五个 CString 按成员顺序构造，失败仅析构已完成成员。
class EnvironmentPathRuntime final
{
public:
    explicit EnvironmentPathRuntime(IEnvironmentPathApi& api,
        const std::string& installFileName = "install.inf",
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    EnvironmentPathRuntime(const EnvironmentPathRuntime&) = delete;
    EnvironmentPathRuntime& operator=(const EnvironmentPathRuntime&) = delete;
    [[nodiscard]] bool ResolveInstallFile(const SystemDataInstallFileInfo& file,
        IEnvironmentPathApi& api, IEnvironmentPathErrorSink& errors);
    // 46A040的生产输入是同一8-byte双CString元素，media在路径验证后才读取/共享。
    [[nodiscard]] bool ResolveInstallFile(const EnvironmentInstalledFile& file,
        IEnvironmentPathApi& api, IEnvironmentPathErrorSink& errors);
    // 值输入适配入口；生产47E2B0在EnvironmentRegistryRuntime中逐轮重取SysData。
    void ResolveInstallFiles(std::span<const SystemDataInstallFileInfo> files,
        IEnvironmentPathApi& api, IEnvironmentPathErrorSink& errors);
    [[nodiscard]] FileManagerConstructionInput FileManagerInput() const noexcept;

    bool readyAt00 = false;
    std::uint32_t setupTypeAt04;
    RuntimeCString currentAt08;
    RuntimeCString cdDriveAt0C;
    RuntimeCString saveAt10;
    RuntimeCString temporaryAt14;
    RuntimeCString libraryAt18;
    std::uint32_t nameChangeAt1C = 1;
    // +20/+24/+28是vector<Ref<SInstallFileInfo>>，不是三个配置DWORD。
    EnvironmentInstalledFiles installedFilesAt20;
    bool aspectAt2C;
};
static_assert(sizeof(EnvironmentPathRuntime) == 0x30);
static_assert(offsetof(EnvironmentPathRuntime, readyAt00) == 0);
static_assert(offsetof(EnvironmentPathRuntime, setupTypeAt04) == 4);
static_assert(offsetof(EnvironmentPathRuntime, currentAt08) == 8);
static_assert(offsetof(EnvironmentPathRuntime, cdDriveAt0C) == 0x0C);
static_assert(offsetof(EnvironmentPathRuntime, saveAt10) == 0x10);
static_assert(offsetof(EnvironmentPathRuntime, temporaryAt14) == 0x14);
static_assert(offsetof(EnvironmentPathRuntime, libraryAt18) == 0x18);
static_assert(offsetof(EnvironmentPathRuntime, nameChangeAt1C) == 0x1C);
static_assert(offsetof(EnvironmentPathRuntime, installedFilesAt20) == 0x20);
static_assert(offsetof(EnvironmentPathRuntime, aspectAt2C) == 0x2C);
}

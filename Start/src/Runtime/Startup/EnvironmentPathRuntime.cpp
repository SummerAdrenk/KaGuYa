#include <Runtime/EnvironmentPathRuntime.h>
#include <Runtime/FileManagerRuntime.h>
#include <Runtime/SystemDataEnvironmentArchive.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <cstring>
#include <mbstring.h>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
DWORD Win32EnvironmentPathApi::CurrentDirectory(DWORD capacity, char* buffer)
{ return GetCurrentDirectoryA(capacity, buffer); }
DWORD Win32EnvironmentPathApi::ProfileString(const char* section, const char* key,
    const char* defaultValue, char* buffer, DWORD capacity, const char* fileName)
{ return GetPrivateProfileStringA(section, key, defaultValue, buffer, capacity, fileName); }
DWORD Win32EnvironmentPathApi::Attributes(const char* path) { return GetFileAttributesA(path); }
DWORD Win32EnvironmentPathApi::LastError() { return GetLastError(); }
BOOL Win32EnvironmentPathApi::CreatePathDirectory(const char* path) { return ::CreateDirectoryA(path, nullptr); }

namespace
{
// 原PE 5B3D9C的CP932哨兵，497C7C直接交给GetPrivateProfileStringA。
constexpr char MissingValue[] = "\x96\xA2\x93\x6F\x98\x5E";

void CurrentDirectoryWithSlash(IEnvironmentPathApi& api, RuntimeCString& result)
{
    std::array<char, MAX_PATH> buffer{};
    const DWORD length = api.CurrentDirectory(static_cast<DWORD>(buffer.size()), buffer.data());
    // 原版忽略失败后strlen未初始化栈数据；现代边界明确拒绝，不改用模块目录或动态长路径。
    if (length == 0 || length >= buffer.size() || !std::memchr(buffer.data(), 0, buffer.size()))
        throw std::runtime_error("494CF0: GetCurrentDirectoryA did not return a bounded path");
    result.SetString(buffer.data());
    // 494EEA/4953DC：空路径通过 CString 下标分支抛 E_INVALIDARG。
    if (result[result.GetLength() - 1] != '\\') result.AppendChar('\\');
}

class ProfileBufferControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ProfileBufferControl(char* data) noexcept : EnvironmentObjectReferenceControl(data) {}
    void DestroyPayloadAtVslot00() override { delete[] static_cast<char*>(payloadAt14); }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};

struct ProfileBuffer final
{
    YtlReferenceSlotRuntime reference;
    ~ProfileBuffer() noexcept { ReleaseYtlReference(reference); }
    char* Allocate()
    {
        // 497C27..497C73：字节先分配/发布，control 失败不回收无 control 的正文。
        auto* data = new char[512];
        reference.baseAt00 = reinterpret_cast<std::uintptr_t>(data);
        void* storage = ::operator new(sizeof(ProfileBufferControl));
        auto* control = storage ? ::new (storage) ProfileBufferControl(data) : nullptr;
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = 1;
        control->countAt10 = 512;
        reference.controlAt08 = control; // 497C73最后发布；不能用普通new清零未知区。
        return data;
    }
};

struct InstalledReferenceCleanup final
{
    YtlReferenceSlotRuntime& reference;
    bool armed = true;
    ~InstalledReferenceCleanup() noexcept
    {
        // 598900/598908 -> 45FD90：异常展开重读局部槽，正文回调后再次取 control。
        if (armed) ReleaseYtlReference(reference);
    }
    void CompleteNormal(YtlReferenceControlRuntime* control) noexcept
    {
        // 46A288/46A2AB：先撤销本槽清理资格；正常路径保持捕获的 control，且不清空槽。
        armed = false;
        if (control && --control->referenceCountAt04 == 0)
        {
            if (control->owningAt08) control->DestroyPayloadAtVslot00();
            control->DeleteControlAtVslot04(1);
        }
    }
};

void TrimSetting(RuntimeCString& value)
{
    // 48E540/4985B0：分别写时分离；不提前在原始 profile 缓冲中裁剪。
    const auto* start = reinterpret_cast<const unsigned char*>(value.GetString());
    const auto* first = start;
    while (*first == ' ' || *first == '\t') first = _mbsinc(first);
    if (first != start)
    {
        const int skipped = static_cast<int>(first - start);
        const int length = value.GetLength();
        char* data = value.GetBuffer(length);
        std::memmove(data, data + skipped, static_cast<std::size_t>(length - skipped) + 1);
        value.ReleaseBufferSetLength(length - skipped);
    }
    start = reinterpret_cast<const unsigned char*>(value.GetString());
    const unsigned char* trailing = nullptr;
    for (const auto* cursor = start; *cursor; cursor = _mbsinc(cursor))
    {
        if (*cursor == ' ' || *cursor == '\t')
        {
            if (!trailing) trailing = cursor;
        }
        else trailing = nullptr;
    }
    if (trailing)
    {
        const int length = static_cast<int>(trailing - start);
        static_cast<void>(value.GetBuffer(length));
        value.ReleaseBufferSetLength(length);
    }
}

bool ReadSetting(IEnvironmentPathApi& api, const RuntimeCString& file,
    const char* key, RuntimeCString& value, IRuntimeStringManagerSource& strings)
{
    // 497BD0先检测profile文件名，读取返回DWORD不用作成败判据。
    if (file.IsEmpty()) return false;
    ProfileBuffer allocation;
    char* buffer = allocation.Allocate();
    // 原缓冲未初始化；现代安全口用非NUL哨兵识别API未提供可读字符串的情况，
    // 不把未写输出误判成空配置并进入默认初始化分支。
    std::memset(buffer, 0x7F, 512);
    static_cast<void>(api.ProfileString("PATH", key, MissingValue, buffer, 512, file.GetString()));
    if (!std::memchr(buffer, 0, 512))
        throw std::runtime_error("497BD0: profile result is not terminated");
    RuntimeCString raw(strings.CaptureAt417360());
    raw.SetString(buffer); // 40B530先取得nil，再经437680赋值，不用直接分配构造。
    const auto* start = reinterpret_cast<const unsigned char*>(raw.GetString());
    if (const auto* comment = _mbsstr(start, reinterpret_cast<const unsigned char*>("//")))
    {
        // 419D32 -> 40B490：指定长度的直接分配构造，长度为零也不改用共享 nil。
        RuntimeCString left(raw.GetString(), static_cast<int>(comment - start), raw.GetManager());
        value = left;
    }
    else value = raw;
    TrimSetting(value);
    return !value.IsEmpty() && _mbsicmp(reinterpret_cast<const unsigned char*>(value.GetString()),
        reinterpret_cast<const unsigned char*>(MissingValue)) != 0;
}

std::uint32_t SetupType(std::string_view text)
{
    // 495043追加h再40BEB0：不接受0x前缀/符号，非法字符使结果为0；DWORD运算自然回绕。
    std::uint32_t result = 0;
    for (const unsigned char c : text)
    {
        unsigned int digit;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10u;
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10u;
        else return 0;
        result = result * 16u + digit;
    }
    return result;
}

void Lowercase(RuntimeCString& value)
{
    const int length = value.GetLength();
    const int error = _mbslwr_s(reinterpret_cast<unsigned char*>(value.GetBuffer(length)),
        static_cast<std::size_t>(length) + 1);
    // 4136D0：80与0正常返回，ENOMEM/EINVAL/ERANGE使用不同 HRESULT。
    if (error != 0 && error != 80)
        throw ATL::CAtlException(error == ENOMEM ? E_OUTOFMEMORY :
            (error == EINVAL || error == ERANGE ? E_INVALIDARG : E_FAIL));
    value.ReleaseBufferSetLength(length);
}

void AppendSlash(RuntimeCString& value)
{
    if (!value.IsEmpty() && value[value.GetLength() - 1] != '\\') value.AppendChar('\\');
}

bool PathExists(IEnvironmentPathApi& api, const char* source,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource())
{
    RuntimeCString path(strings.CaptureAt417360());
    path.SetString(source);
    // 46CC90只去掉一个非盘符根目录的尾反斜杠；属性成功不要求DIRECTORY位。
    const int length = path.GetLength();
    if (length >= 2 && path[length - 1] == '\\' && !(length >= 3 && path[length - 2] == ':'))
    {
        static_cast<void>(path.GetBuffer(length));
        path.ReleaseBufferSetLength(length - 1);
    }
    if (api.Attributes(path.GetString()) != INVALID_FILE_ATTRIBUTES) return true;
    while (api.LastError() == ERROR_NOT_READY)
        if (api.Attributes(path.GetString()) != INVALID_FILE_ATTRIBUTES) return true;
    return false;
}
}

EnvironmentPathRuntime::EnvironmentPathRuntime(IEnvironmentPathApi& api, const std::string& installFileName,
    IRuntimeStringManagerSource& strings)
    : currentAt08(strings.CaptureAt417360()), cdDriveAt0C(strings.CaptureAt417360()),
      saveAt10(strings.CaptureAt417360()), temporaryAt14(strings.CaptureAt417360()),
      libraryAt18(strings.CaptureAt417360())
{
    // 494E8A..494F9C以当前目录拼接61A530，403F9A初始化该全局为install.inf。
    RuntimeCString scanner(strings.CaptureAt417360());
    scanner.SetString("", 0);
    CurrentDirectoryWithSlash(api, scanner);
    scanner.Append(installFileName.c_str());
    RuntimeCString profile(strings.CaptureAt417360());
    profile.SetString(scanner.GetString());
    aspectAt2C = false; // 494FC3：在 profile 字符串成功建立后才写。
    RuntimeCString value(strings.CaptureAt417360());
    if (!ReadSetting(api, profile, "SetupType", value, strings))
    {
        // 49536C分支再次询问cwd，不能复用第一次的快照。
        RuntimeCString directory(strings.CaptureAt417360());
        directory.SetString("", 0);
        CurrentDirectoryWithSlash(api, directory);
        setupTypeAt04 = 0; // 495462：不能在构造入口提前写。
        cdDriveAt0C.SetString(directory.GetString());
        currentAt08 = cdDriveAt0C;
        libraryAt18.SetString("", 0);
        temporaryAt14 = libraryAt18;
        saveAt10 = temporaryAt14;
    }
    else
    {
        value.AppendChar('h');
        setupTypeAt04 = SetupType(RuntimeStringView(value).substr(0, value.GetLength() - 1));
        if (!ReadSetting(api, profile, "Current", currentAt08, strings) ||
            !ReadSetting(api, profile, "CDDrive", cdDriveAt0C, strings))
            return;
        if (!ReadSetting(api, profile, "Save", saveAt10, strings)) saveAt10.SetString("", 0);
        if (!ReadSetting(api, profile, "Temporary", temporaryAt14, strings)) temporaryAt14.SetString("", 0);
        {
            RuntimeCString setting(strings.CaptureAt417360());
            if (ReadSetting(api, profile, "aspect", setting, strings))
            {
                Lowercase(setting);
                aspectAt2C = _mbscmp(reinterpret_cast<const unsigned char*>(setting.GetString()),
                    reinterpret_cast<const unsigned char*>("on")) == 0;
            }
        }
        if (!ReadSetting(api, profile, "Library", libraryAt18, strings)) libraryAt18.SetString("", 0);
        {
            RuntimeCString setting(strings.CaptureAt417360());
            if (ReadSetting(api, profile, "NameChange", setting, strings))
            {
                Lowercase(setting);
                nameChangeAt1C = _mbscmp(reinterpret_cast<const unsigned char*>(setting.GetString()),
                    reinterpret_cast<const unsigned char*>("false")) != 0;
            }
        }
    }
    AppendSlash(currentAt08);
    AppendSlash(cdDriveAt0C);
    if (saveAt10.IsEmpty()) saveAt10 = currentAt08 + "Save\\";
    AppendSlash(saveAt10);
    if (temporaryAt14.IsEmpty()) temporaryAt14 = currentAt08;
    if (libraryAt18.IsEmpty()) libraryAt18 = currentAt08;
    AppendSlash(temporaryAt14);
    if (!PathExists(api, saveAt10.GetString(), strings)) static_cast<void>(api.CreatePathDirectory(saveAt10.GetString()));
    // 495B0A附近CreateDirectory的BOOL不影响ready；不能把创建失败改成初始化失败。
    readyAt00 = true;
}

void IEnvironmentPathErrorSink::AppendMissingInstallFile(const char* nameCp932)
{
    // 显式值型sink适配；原生registry覆写此入口，直接使用4769B0的格式化链。
    AppendPathError(std::string("\x81\x77 ") + nameCp932 +
        " \x81\x78\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xAA\x8C\xA9\x82\xC2\x82\xA9\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49");
}

void IEnvironmentPathErrorSink::AppendInsertInstallDisc(int driveByte)
{
    std::string text = "\x83\x68\x83\x89\x83\x43\x83\x75 ";
    text.push_back(static_cast<char>(driveByte));
    text += " \x82\xC9\x83\x66\x83\x42\x83\x58\x83\x4E\x82\xF0\x93\xFC\x82\xEA\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81\x42";
    AppendPathError(std::move(text));
}

bool EnvironmentPathRuntime::ResolveInstallFile(const SystemDataInstallFileInfo& file,
    IEnvironmentPathApi& api, IEnvironmentPathErrorSink& errors)
{
    // 仅用于已有值模型调用者；生产不能经过此快照适配。
    if (!readyAt00) return false;
    auto& strings = DefaultRuntimeStringManagerSource();
    EnvironmentInstalledFile value{RuntimeCString(strings.CaptureAt417360()), RuntimeCString(strings.CaptureAt417360())};
    value.stringAt00.SetString(file.stringAt00.data(), static_cast<int>(file.stringAt00.size()));
    value.stringAt04.SetString(file.stringAt04.data(), static_cast<int>(file.stringAt04.size()));
    return ResolveInstallFile(value, api, errors);
}

bool EnvironmentPathRuntime::ResolveInstallFile(const EnvironmentInstalledFile& file,
    IEnvironmentPathApi& api, IEnvironmentPathErrorSink& errors)
{
    if (!readyAt00) return false; // 46A06D..46A071，不尝试目录、不写错误。
    RuntimeCString path = currentAt08 + file.stringAt00.GetString();
    if (!PathExists(api, path.GetString()))
    {
        // 原本明确存在的CD分支，不能改为目录遍历或其他来源回退。
        path = cdDriveAt0C + file.stringAt00.GetString();
        if (!PathExists(api, path.GetString()))
        {
            // 原PE 5B26B8/5B26DC的两个格式字符串，依次进入4769B0。
            errors.AppendMissingInstallFile(file.stringAt00.GetString());
            if (setupTypeAt04)
            {
                errors.AppendInsertInstallDisc(static_cast<signed char>(cdDriveAt0C.GetString()[0]));
            }
            return false;
        }
    }
    auto* const installed = CreateEnvironmentInstalledFileAt46A16C();
    YtlReferenceSlotRuntime reference;
    InitializeEnvironmentInstalledFileReference(reference, installed);
    // 46A219 才建立清理资格；此前正文/CString/control分配失败不能用unique_ptr回滚。
    InstalledReferenceCleanup release{reference};
    auto* const ownerControl = reference.controlAt08; // ESI 跨越两个 CString 赋值和 append。
    installed->stringAt00 = path;
    // 46A225..46A232：重新读取同一SysData元素的+04，调用410220共享/写时分离。
    installed->stringAt04 = file.stringAt04;
    YtlReferenceSlotRuntime copy;
    CopyEnvironmentInstalledFileReference(copy, reference);
    InstalledReferenceCleanup releaseCopy{copy};
    auto* copyControl = copy.controlAt08;
    const bool grows = installedFilesAt20.storage.end == installedFilesAt20.storage.capacity;
    installedFilesAt20.AppendCopy(copy);
    if (grows) copyControl = copy.controlAt08; // 46A285：只有增长调用返回后重读临时槽。
    releaseCopy.CompleteNormal(copyControl);
    release.CompleteNormal(ownerControl);
    return true;
}

void EnvironmentPathRuntime::ResolveInstallFiles(std::span<const SystemDataInstallFileInfo> files,
    IEnvironmentPathApi& api, IEnvironmentPathErrorSink& errors)
{
    for (const auto& file : files)
        if (!ResolveInstallFile(file, api, errors)) throw 2;
}

FileManagerConstructionInput EnvironmentPathRuntime::FileManagerInput() const noexcept
{
    // 458B60从env+48读ready、已解析安装表、+10缓存目录及+08消息目录。
    return {readyAt00, installedFilesAt20, saveAt10, currentAt08, saveAt10};
}
}

#include <Runtime/SystemDataEnvironmentArchive.h>
#include <Runtime/NativeArchiveRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <locale.h>
#include <mbstring.h>
#include <memory>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
void RetireArchive(void* object) noexcept
{
    // 4973D5/4978B9：只退化IArchive的vptr，其余0x64字节保持原值。
    RetireNativeArchive(object);
}

void AssignVersion(RuntimeCString& destination, SystemDataVersionString query,
    ISystemDataEnvironmentValueSource& values, IRuntimeStringManagerSource& strings)
{
    const auto value = values.ReadVersionString(query, strings);
    // 4964DF/49653A/49659A ->410220，先共享/复制赋值，再解除临时清理资格并减持。
    AssignRuntimeCStringAt410220(destination, value.Value());
}

// sample18 内存 5B3EC0/5B3ECC/5B3ED4/5B3EDC：可変設定、文字色、既読色、可変設定2。
constexpr std::string_view VariableSettings = "\x89\xC2\x95\xCF\x90\xDD\x92\xE8";
constexpr std::string_view TextColor = "\x95\xB6\x8E\x9A\x90\x46";
constexpr std::string_view ReadColor = "\x8A\xF9\x93\xC7\x90\x46";
constexpr std::string_view VariableSettings2 = "\x89\xC2\x95\xCF\x90\xDD\x92\xE8" "2";
// 5B2C44/5B2C58：背景色、透過率。
constexpr std::string_view BackgroundColor = "\x94\x77\x8C\x69\x90\x46";
constexpr std::string_view Transparency = "\x93\xA7\x89\xDF\x97\xA6";
// 5B24DC/5B2504：原格式串有两个 %s，IDA 的行尾注释只显示第一个前缀。
constexpr char MissingChildFormat[] = "%s \x83\x5E\x83\x4F\x82\xC9 %s "
    "\x8D\x80\x96\xDA\x82\xCD\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49";
constexpr char MissingValueFormat[] = "%s \x83\x5E\x83\x4F\x82\xC9 %s "
    "\x91\xAE\x90\xAB\x82\xCD\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49";

class SettingCp932Locale final
{
public:
    SettingCp932Locale() : value_(_create_locale(LC_CTYPE, ".932"))
    {
        if (!value_) throw std::runtime_error("Setting lookup requires CP932 locale");
    }
    ~SettingCp932Locale() { _free_locale(value_); }
    SettingCp932Locale(const SettingCp932Locale&) = delete;
    SettingCp932Locale& operator=(const SettingCp932Locale&) = delete;
    bool Equal(const char* left, const char* right) const
    {
        return _mbscmp_l(reinterpret_cast<const unsigned char*>(left),
            reinterpret_cast<const unsigned char*>(right), value_) == 0;
    }
private:
    _locale_t value_;
};

const ParamsSettingTagRuntime& RequireSettingNode(const YtlReferenceSlotRuntime& slot)
{
    const auto* node = ResolveParamsSettingTagAt410560(slot);
    if (!node) throw MissingSettingsRoot{};
    return *node;
}

class SettingsSource final
{
public:
    SettingsSource(const ParamsGameSystem& fixed, const ParamsRuntimeRoot* current) noexcept
        : fixed_(fixed), current_(current) {}

    const YtlReferenceSlotRuntime& Root(std::size_t index) const
    {
        const auto* game = current_ ? current_->GameSystem().get() : &fixed_;
        if (!game) throw MissingSettingsRoot{};
        return game->settingTags[index].Native();
    }
private:
    const ParamsGameSystem& fixed_;
    const ParamsRuntimeRoot* current_;
};

template<class Action>
void RecoverAt(ISystemDataEnvironmentDiagnostics& diagnostics,
    const std::uint32_t handlerAddress, const SettingTagNodeCapture& retained,
    YtlReferenceControlRuntime*& savedControl, Action action)
{
    // 5ED500..5ED588：adjectives=0x40、type=0，确认是 catch(...)。
    // 报告放在 catch 的最后，不包进另一层原恢复范围。
    try { action(); }
    // 空 Ref 在原版是非法解引用，不是缺少配置项；明确安全拒绝不能伪装局部恢复。
    catch (const MissingSettingsRoot&) { throw; }
    catch (...)
    {
        // 4967E8/49686E、496A77/496AB1/496ADC、496CDC/496D16/496D41：
        // 只有 catch 恢复才重载 EDI；正常求值即使改过 local.control 也保留旧值。
        savedControl = retained.Native().controlAt08;
        diagnostics.ReportRecoveredConstructorException(handlerAddress, std::current_exception());
    }
}

std::int32_t ScaleTransparency(const std::int32_t percent) noexcept
{
    // 496AB7..496ACB / 496D1C..496D30：先按 x86 32 位 SHL 回绕，再有符号 /100。
    // 不能用 int64(percent)*256/100，也不能对负值移位造成 C++ 未定义行为。
    const auto shifted = static_cast<std::uint32_t>(percent) << 8u;
    return std::bit_cast<std::int32_t>(shifted) / 100;
}

void InitializeBackground(const SettingsSource& source, const std::string_view name,
    ISystemDataEnvironmentValueSource& values, ISystemDataEnvironmentDiagnostics& diagnostics,
    ISettingTagErrorSource& errors,
    std::uint32_t& color, std::int32_t& transparency,
    const std::array<std::uint32_t, 3>& handlers)
{
    SettingTagNodeCapture setting;
    YtlReferenceControlRuntime* savedControl = nullptr;
    RecoverAt(diagnostics, handlers[0], setting, savedControl, [&]
    {
        const auto& parent = source.Root(2);
        auto* errorCString = errors.CaptureAt47BCF0(); // 496903 / 496B68
        setting = CaptureSettingChildAt477E90(errorCString, parent, name.data());
        savedControl = setting.Native().controlAt08; // 49692D / 496B92
    });
    if (setting.Get())
    {
        // 496986..496A47 / 496BEB..496CAC：背景色字符串是 +7C 下的节点名，
        // 不是字面颜色；空串保留 -1。颜色与透過率的 catch 彼此独立。
        RecoverAt(diagnostics, handlers[1], setting, savedControl, [&]
        {
            auto* errorCString = errors.CaptureAt47BCF0(); // 496981 / 496BE6
            const auto colorName = CaptureSettingValueAt47D470(
                errorCString, setting.Native(), BackgroundColor.data());
            if (colorName.Value().GetString()[0] != '\0')
            {
                std::uint32_t evaluated;
                {
                    // 4969B6 / 496C1B：先捕获名称指针，不在两次 getter 后重读 CString 槽。
                    const auto* nameCp932 = colorName.Value().GetString();
                    const auto& parent = source.Root(1);
                    auto* colorError = errors.CaptureAt47BCF0(); // 4969E6 / 496C4B
                    auto leaf = CaptureSettingChildAt477E90(colorError, parent, nameCp932);
                    evaluated = values.ReadColorAt4797C0(colorError, leaf.Native(), "Value");
                } // 496A1B..496A47：先释放颜色叶临时，再写入目标。
                color = evaluated;
            }
        });
        std::int32_t percent = 50;
        RecoverAt(diagnostics, handlers[2], setting, savedControl, [&]
        {
            auto* errorCString = errors.CaptureAt47BCF0(); // 496A8C / 496CF1
            percent = values.ReadIntegerAt47BBE0(errorCString, setting.Native(), Transparency);
        });
        transparency = ScaleTransparency(percent);
    }
    // 496AE6..496AEF / 496D4B..496D54：只有有符号上限截断，没有下限截断。
    if (transparency >= 256) transparency = 255;
    setting.ReleaseCached(savedControl);
}
}

SettingTagNodeCapture::SettingTagNodeCapture(const YtlReferenceSlotRuntime& source) noexcept
    : slot_(source)
{
    if (slot_.controlAt08) ++slot_.controlAt08->referenceCountAt04;
}

SettingTagNodeCapture::~SettingTagNodeCapture() noexcept
{
    ReleaseParamsSettingTagAt41FEAE(slot_);
}

SettingTagNodeCapture::SettingTagNodeCapture(SettingTagNodeCapture&& other) noexcept
    : slot_(std::exchange(other.slot_, {}))
{
}

SettingTagNodeCapture& SettingTagNodeCapture::operator=(SettingTagNodeCapture&& other) noexcept
{
    if (this != &other)
    {
        ReleaseParamsSettingTagAt41FEAE(slot_);
        slot_ = std::exchange(other.slot_, {});
    }
    return *this;
}

void SettingTagNodeCapture::ReleaseCached() noexcept
{
    ReleaseCached(slot_.controlAt08);
}

void SettingTagNodeCapture::ReleaseCached(YtlReferenceControlRuntime* const control) noexcept
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
    slot_.baseAt00 = 0;
    slot_.controlAt08 = nullptr;
}

ParamsSettingTagRuntime* SettingTagNodeCapture::Get() const noexcept
{
    return ResolveParamsSettingTagAt410560(slot_);
}

SettingTagNodeCapture CaptureSettingChildAt477E90(
    RuntimeCString* errorCString, const YtlReferenceSlotRuntime& parent, const char* nameCp932)
{
    // 477EA3 早于父槽解析；清空回调可以改变同一个 Ref 槽。
    if (!errorCString) _invalid_parameter_noinfo_noreturn();
    SetRuntimeCStringAt437680(*errorCString, "", 0);
    const SettingCp932Locale locale;
    auto* child = RequireSettingNode(parent).childrenAt10.begin;
    // 477ECB 缓存 begin；477ED0/477F18 每次重新 resolve 父槽取得 end。
    while (child != RequireSettingNode(parent).childrenAt10.end)
    {
        const auto* body = ResolveParamsSettingTagAt410560(*child);
        if (!nameCp932) throw ATL::CAtlException(E_FAIL);
        if (!body) throw MissingSettingsRoot{};
        if (locale.Equal(body->nameAt00.GetString(), nameCp932))
            return SettingTagNodeCapture(*child);
        ++child;
    }
    FormatRuntimeCStringAt417130(*errorCString, MissingChildFormat,
        RequireSettingNode(parent).nameAt00.GetString(), nameCp932);
    throw const_cast<char*>("error !");
}

RuntimeCStringResult CaptureSettingValueAt47D470(
    RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, const char* keyCp932)
{
    // 47D483 先清空真实 receiver；返回槽此时尚未构造。
    if (!errorCString) _invalid_parameter_noinfo_noreturn();
    SetRuntimeCStringAt437680(*errorCString, "", 0);
    return RuntimeCStringResult([&](RuntimeCString* destination)
    {
        const SettingCp932Locale locale;
        auto* pair = RequireSettingNode(tag).pairsAt04.begin;
        while (pair != RequireSettingNode(tag).pairsAt04.end)
        {
            if (!keyCp932) throw ATL::CAtlException(E_FAIL);
            if (locale.Equal(pair->keyAt00.GetString(), keyCp932))
            {
                auto* header = reinterpret_cast<ATL::CStringData*>(
                    const_cast<char*>(pair->valueAt04.GetString())) - 1;
                auto* cloned = CloneRuntimeCStringDataAt414540(header);
                auto* chars = reinterpret_cast<char*>(cloned + 1);
                // 隐藏返回槽尚未构造；Clone 回调之后不得再访问旧 pair。
                std::memcpy(destination, &chars, sizeof(chars));
                return;
            }
            ++pair;
        }
        FormatRuntimeCStringAt417130(*errorCString, MissingValueFormat,
            RequireSettingNode(tag).nameAt00.GetString(), keyCp932);
        throw const_cast<char*>("error !");
    });
}

IEnvironmentArchiveRuntime::~IEnvironmentArchiveRuntime() noexcept
{
    RetireArchive(this);
}

void* IEnvironmentArchiveRuntime::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    RetireArchive(this);
    if (flags & 1u) ::operator delete(this, sizeof(IEnvironmentArchiveRuntime));
    return this;
}

SystemDataEnvironmentArchive::SystemDataEnvironmentArchive(
    UnreadyTag, IRuntimeStringManagerSource& strings)
    : readyAt04(false)
{
    std::construct_at(&installFilesAt08);
    struct InstallCleanup
    {
        SystemDataInstalledFiles* files;
        ~InstallCleanup() noexcept { if (files) std::destroy_at(files); }
    } cleanup{&installFilesAt08};
    // 原state=1覆盖三次Capture/GetNil；直到第三串成功才取得整个组的清理资格。
    // 第二/第三次失败时不回收已经写入的前缀串，不能用逐成员RAII改变异常分支。
    InitializeEmptyRuntimeCString(stringAt18, strings);
    InitializeEmptyRuntimeCString(stringAt1C, strings);
    InitializeEmptyRuntimeCString(stringAt20, strings);
    cleanup.files = nullptr;
}

SystemDataEnvironmentArchive::~SystemDataEnvironmentArchive() noexcept
{
    // 497320/497800：只释放三个CString，再释放安装Ref；四个literal指针不拥有内存。
    DestroyRuntimeCStringAt40CD40(&stringAt20);
    DestroyRuntimeCStringAt40CD40(&stringAt1C);
    DestroyRuntimeCStringAt40CD40(&stringAt18);
    std::destroy_at(&installFilesAt08);
}

void* SystemDataEnvironmentArchive::DeleteAtVslot04(std::uint32_t flags) noexcept
{
    this->~SystemDataEnvironmentArchive();
    if (flags & 1u) ::operator delete(this, OriginalObjectSize);
    return this;
}

SystemDataEnvironmentArchive::SystemDataEnvironmentArchive(
    const ParamsRuntimeProfile& loadedProfile,
    ISystemDataEnvironmentValueSource& values,
    ISystemDataEnvironmentDiagnostics& diagnostics,
    SystemDataEnvironmentWindowTarget ownerTarget,
    IRuntimeStringManagerSource& strings, ISettingTagErrorSource& errors)
    : SystemDataEnvironmentArchive(UnreadyTag{}, strings)
{
    // 496066 的非负版本前置条件；现代边界同时拒绝 NaN，绝不制造 ready 成功对象。
    if (!(loadedProfile.version >= 0.0))
        throw std::invalid_argument("CSysData requires a successfully loaded params profile");
    InitializeFromGameSystem(loadedProfile.gameSystem, values, diagnostics, ownerTarget, nullptr, strings, errors);
}

SystemDataEnvironmentArchive::SystemDataEnvironmentArchive(
    const ParamsRuntimeRoot& loadedRoot,
    ISystemDataEnvironmentValueSource& values,
    ISystemDataEnvironmentDiagnostics& diagnostics,
    SystemDataEnvironmentWindowTarget ownerTarget,
    IRuntimeStringManagerSource& strings, ISettingTagErrorSource& errors)
    : SystemDataEnvironmentArchive(UnreadyTag{}, strings)
{
    // 显式现代构造口保留诊断前置条件，不让其约束495E10的真实文件读取链。
    if (!loadedRoot.Version() || !(*loadedRoot.Version() >= 0.0))
        throw std::invalid_argument("CSysData requires a successfully loaded params root");
    InitializeFromLoadedRoot(loadedRoot, values, diagnostics, ownerTarget, strings, errors);
}

void SystemDataEnvironmentArchive::InitializeFromLoadedRoot(const ParamsRuntimeRoot& loadedRoot,
    ISystemDataEnvironmentValueSource& values,
    ISystemDataEnvironmentDiagnostics& diagnostics,
    SystemDataEnvironmentWindowTarget ownerTarget,
    IRuntimeStringManagerSource& strings, ISettingTagErrorSource& errors)
{
    // 496090只捕获GameSystem；496061已在前一代根上比较版本，此处不再预检诊断。
    // 保留一次引用直到构造核结束；安装/窗口/启动字节用它，配置段另取当前根。
    auto retainedGameSystem = loadedRoot.GameSystem();
    auto* const savedGameControl = retainedGameSystem.Native().controlAt08;
    if (!retainedGameSystem)
        throw std::logic_error("4960D5: params GameSystem reference has no payload");
    InitializeFromGameSystem(*retainedGameSystem, values, diagnostics, ownerTarget, &loadedRoot, strings, errors);
    // 496D7E已置ready；496D86使用缓存ESI，正常尾声失败终止，不重走496F00展开释放。
    retainedGameSystem.ReleaseCached(savedGameControl);
}

void SystemDataEnvironmentArchive::InitializeFromGameSystem(
    const ParamsGameSystem& gameSystem,
    ISystemDataEnvironmentValueSource& values,
    ISystemDataEnvironmentDiagnostics& diagnostics,
    SystemDataEnvironmentWindowTarget ownerTarget,
    const ParamsRuntimeRoot* currentRoot,
    IRuntimeStringManagerSource& strings, ISettingTagErrorSource& errors)
{
    InitializeSystemDataInstallFilesAt4960D5(gameSystem, installFilesAt08, strings);

    // 4963C6..4964A1 在版本查询之前写共享根；不能等整个 CSysData 成功返回后再提交。
    // owner不属于68h正文；外部显式目标的后续异常不撤销已写字段。
    ownerTarget.Apply(gameSystem);

    // 4964D2 -> 49652D -> 49658D，顺序不是字段 offset 顺序。
    AssignVersion(stringAt18, SystemDataVersionString::FileVersionAt497920, values, strings);
    AssignVersion(stringAt20, SystemDataVersionString::ProductVersionAt497A60, values, strings);
    AssignVersion(stringAt1C, SystemDataVersionString::LegalCopyrightAt497E00, values, strings);

    // 4965C1..4965DF：仅原文字面文件名；目录及 owner 由主线处理。
    stringAt24 = "help.chm";
    stringAt28 = "env.ini";
    stringAt2C = "secret.ini";
    stringAt30 = "global.ini";
    // 4965E6..4966B0：params+10 数组的无符号 byte 最大值，空数组明确为 0。
    valueAt34 = 0; // 4965DF，覆盖版本回调期间可能改变的旧值。
    for (std::uint32_t index = 0;; ++index)
    {
        const auto count = static_cast<std::uint32_t>(gameSystem.startupConfigBytes.size());
        if (std::bit_cast<std::int32_t>(index) >= std::bit_cast<std::int32_t>(count)) break;
        const auto readByte = [&]() {
            const auto* const byte = gameSystem.startupConfigBytes.Resolve(index);
            if (!byte) throw std::logic_error("SystemData startup byte body is null");
            return static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(*byte));
        };
        // 49665D 再次 resolve/read；不把比较时的字节当作提交值。
        if (valueAt34 < readByte()) valueAt34 = readByte();
    }

    // 4966B7/4966BE：这是阶段写入，不只是对象分配时的安全初值。
    valueAt38 = 0x00FFFFFF;
    valueAt3C = 0x00808080;
    const SettingsSource settings(gameSystem, currentRoot);
    {
        SettingTagNodeCapture colorSettings;
        YtlReferenceControlRuntime* savedControl = nullptr;
        RecoverAt(diagnostics, 0x49687A, colorSettings, savedControl, [&]
        {
            const auto& parent = settings.Root(1);
            auto* errorCString = errors.CaptureAt47BCF0(); // 496700
            colorSettings = CaptureSettingChildAt477E90(errorCString, parent, VariableSettings.data());
            savedControl = colorSettings.Native().controlAt08; // 496722
        });
        if (colorSettings.Get())
        {
            // 4966B7/4966BE 明确初始化 FFFFFF/808080；两次颜色失败分别保留原值。
            RecoverAt(diagnostics, 0x4967DC, colorSettings, savedControl, [&]
            {
                std::uint32_t evaluated;
                {
                    auto* errorCString = errors.CaptureAt47BCF0(); // 49677A
                    auto leaf = CaptureSettingChildAt477E90(errorCString, colorSettings.Native(), TextColor.data());
                    evaluated = values.ReadColorAt4797C0(errorCString, leaf.Native(), "Value");
                }
                valueAt38 = evaluated;
            });
            RecoverAt(diagnostics, 0x496862, colorSettings, savedControl, [&]
            {
                std::uint32_t evaluated;
                {
                    auto* errorCString = errors.CaptureAt47BCF0(); // 4967F6
                    auto leaf = CaptureSettingChildAt477E90(errorCString, colorSettings.Native(), ReadColor.data());
                    evaluated = values.ReadColorAt4797C0(errorCString, leaf.Native(), "Value");
                }
                valueAt3C = evaluated;
            });
        }
        // 496883/49688A 早于父节点释放；不复制已配置的 +38/+3C。
        valueAt48 = 0x00FFFFFF;
        valueAt4C = 0x00808080;
        colorSettings.ReleaseCached(savedControl);
    }
    // 4968C7/4968CE、496B2C/496B33：两组背景初值明确为 -1/128。
    valueAt40 = 0xFFFFFFFF;
    valueAt44 = 128;
    InitializeBackground(settings, VariableSettings, values, diagnostics, errors,
        valueAt40, valueAt44, {0x496AD0, 0x496A6B, 0x496AA5});
    valueAt50 = 0xFFFFFFFF;
    valueAt54 = 128;
    InitializeBackground(settings, VariableSettings2, values, diagnostics, errors,
        valueAt50, valueAt54, {0x496D35, 0x496CD0, 0x496D0A});
    // 496D7E：原 ready 表示构造尾到达，原允许已报告的配置局部恢复。
    readyAt04 = true;
}

void SystemDataEnvironmentArchive::Serialize(SerializeBuffer&) noexcept
{
    // 498220: retn 4。包括空/截断输入、任意游标、读写模式和 ready=0，均不访问 buffer。
}
}

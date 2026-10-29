#pragma once

#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/SerializeBuffer.h>
#include <Runtime/SystemDataInstalledFiles.h>
#include <Runtime/RuntimeCStringResult.h>
#include <Runtime/SettingTagErrorRuntime.h>
#include <Runtime/SystemDataEnvironmentWindowTarget.h>

#include <array>
#include <cstdint>
#include <exception>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRoot;

// 原空正文非法访问的现代安全边界，不属于原查找失败，不得被配置 catch 恢复。
class MissingSettingsRoot final : public std::invalid_argument
{
public:
    MissingSettingsRoot() : std::invalid_argument("Setting root has no payload (safe boundary)") {}
};

// 消费期只增持原 Ref，不调用 ParamsSettingTagReference 的现代深快照复制。
// 本批正常 Ref 释放的 EH state 均终止，展开时的 40D910 也终止。
// CString 返回临时仍单独使用可抛的 RuntimeCStringResult，不能套此边界。
class SettingTagNodeCapture final
{
public:
    SettingTagNodeCapture() noexcept = default;
    explicit SettingTagNodeCapture(const YtlReferenceSlotRuntime& source) noexcept;
    ~SettingTagNodeCapture() noexcept;
    SettingTagNodeCapture(const SettingTagNodeCapture&) = delete;
    SettingTagNodeCapture& operator=(const SettingTagNodeCapture&) = delete;
    SettingTagNodeCapture(SettingTagNodeCapture&& other) noexcept;
    SettingTagNodeCapture& operator=(SettingTagNodeCapture&& other) noexcept;
    // 479BFF/470309 在正常尾声才捕获 control；SysData 则传跨求值保存的 EDI。
    // 默认析构沿 40D910 重读当前槽，供普通临时及 EH 展开使用。
    void ReleaseCached() noexcept;
    void ReleaseCached(YtlReferenceControlRuntime* capturedControl) noexcept;
    [[nodiscard]] const YtlReferenceSlotRuntime& Native() const noexcept { return slot_; }
    [[nodiscard]] ParamsSettingTagRuntime* Get() const noexcept;
private:
    YtlReferenceSlotRuntime slot_{};
};

// 477E90/47D470 的原序 CP932 首匹配；输入是实际 Ref 槽，不能先转成 DTO。
// 缺项抛 char* "error !"；非空循环内 null search 抛 ATL E_FAIL。
// receiver 由原调用点显式 Capture；先清空它，再 resolve 父槽，缺项重新取名格式化。
// null receiver 在首次访问处明确终止，代替原非法访问，不成为可恢复的缺项。
[[nodiscard]] SettingTagNodeCapture CaptureSettingChildAt477E90(
    RuntimeCString* errorCString, const YtlReferenceSlotRuntime& parent, const char* nameCp932);
[[nodiscard]] RuntimeCStringResult CaptureSettingValueAt47D470(
    RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, const char* keyCp932);

enum class SystemDataVersionString : std::uint32_t
{
    FileVersionAt497920 = 0x497920,
    ProductVersionAt497A60 = 0x497A60,
    LegalCopyrightAt497E00 = 0x497E00,
};

// 外部叶接口，不加载文件、不创建版本对象、不拥有路径。
class ISystemDataEnvironmentValueSource
{
public:
    virtual ~ISystemDataEnvironmentValueSource() = default;

    // 4964D2/49652D/49658D：逐次重新捕获 Environment+54 的版本对象，依次返回
    // 文件版本、产品版本、LegalCopyright 的 CP932 字符串。来源失败必须抛出；
    // 仅原版本对象本身已确认没有资源时，才允许按 497920/497A60/497E00 返回空串。
    // 497920 取版本对象+10 指向结构的 WORD[5,4,7,6]；497A60 取 WORD[9,8,11,10]，
    // 均以 "%d.%d.%d.%d" 格式化；497E00 查询键恰为 "LegalCopyright"。
    [[nodiscard]] virtual RuntimeCStringResult ReadVersionString(SystemDataVersionString query,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) = 0;

    // 4797C0 不是普通整数读取：读取 key 后解析 RGB 三分量表达式并返回打包颜色。
    // 5B2574 的 "RGB" 区分大小写；求值后三分量分别取低 byte，结果 R|(G<<8)|(B<<16)，
    // 不是 RRGGBB。表达式/类型/缺键错误均须抛出，不能返回任意“成功”颜色。
    // 47BBE0 通过 45E770 求值并要求整数类型；不得以 atoi/stoul 或默认值替代。
    // tag 为本次选中并增持的真实 Ref 槽；实现逐次 resolve，不复制正文。
    // key 为 CP932、无 NUL 的键名。
    [[nodiscard]] virtual std::uint32_t ReadColorAt4797C0(
        RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, std::string_view keyCp932) = 0;
    [[nodiscard]] virtual std::int32_t ReadIntegerAt47BBE0(
        RuntimeCString* errorCString, const YtlReferenceSlotRuntime& tag, std::string_view keyCp932) = 0;
};

class ISystemDataEnvironmentDiagnostics
{
public:
    virtual ~ISystemDataEnvironmentDiagnostics() = default;

    // 5ED500..5ED588 的九个 catch(...) 均有原始保留值行为。必须显式报告，
    // handlerAddress 是 catch 入口而非续执行地址；exception 保留原异常。
    // 接收方可重抛以禁止降级；此回调的异常不再被本模型吞掉。
    virtual void ReportRecoveredConstructorException(
        std::uint32_t handlerAddress, std::exception_ptr exception) = 0;
};

struct SystemDataInstallFileInfo final
{
    // 工具/夹具的值快照；生产 CSysData+08 使用 SystemDataInstalledFiles，
    // 不能把此结构当成原 8-byte 元素，也不能用其副本代替共享 CString。
    static constexpr std::uint32_t OriginalObjectSize = 8;
    // 对应 ParamsInstallEntry.file.cp932 / media.cp932；路径 owner 可借用整项，
    // 不得将 vector 的物理地址解释为原 Ref 三元组或原 8 字节存储。
    std::string stringAt00;
    std::string stringAt04;
    bool operator==(const SystemDataInstallFileInfo&) const = default;
};

struct SystemDataEnvironmentOwnerInitialization final
{
    // 4963C6..4964A1 写 Environment owner，不是 CSysData 字段。
    // 主线在既有 owner 初始化中消费这些值；本模型不执行发布或路径拼接。
    std::uint32_t widthAt18;
    std::uint32_t heightAt1C;
    std::uint32_t valueAt20;
    std::uint32_t valueAt24;
    // 498280 不初始化 +28；只有 4963F4 才写入 fullscreen style。
    std::optional<std::uint32_t> valueAt28;
    std::string stringAt10;
    std::string stringAt14;
    bool operator==(const SystemDataEnvironmentOwnerInitialization&) const = default;
};

class ParamsRuntimeRootOwner;
class IStartFileApi;

// 原 IArchive 只有 Serialize 和 flags-delete 两槽；非虚析构不能增加第三槽。
class IEnvironmentArchiveRuntime
{
public:
    virtual void SerializeNative18(void* serializer18) = 0;
    virtual void* DeleteAtVslot04(std::uint32_t flags) noexcept;
protected:
    ~IEnvironmentArchiveRuntime() noexcept;
};
static_assert(sizeof(IEnvironmentArchiveRuntime) == 4);

class SystemDataEnvironmentArchive final : public IEnvironmentArchiveRuntime
{
public:
    // 原生堆入口必须raw分配后placement构造，见Loader；普通new的/sdl清零不等价。
    // sample18：4984A0 分配 0x68，4984BA 调用 495E10；49852D..49855C 将
    // IArchive 引用追加到 Environment+30 的首项，即环境 slot0。
    // 495E40 写 vftable=5B3E68；内存表两个槽为 498220 / 497800。
    // 497800 是 deleting destructor，497320 是非 deleting 析构；不是 Serialize。
    static constexpr std::uint32_t OriginalObjectSize = 0x68;
    static constexpr std::uint32_t OriginalVtableAddress = 0x5B3E68;
    static constexpr std::uint32_t OriginalSerializeAddress = 0x498220;
    static constexpr std::uint32_t OriginalDeletingDestructorAddress = 0x497800;

    // 仅还原成功读取 params 后的构造核，不提供“空白即成功”的默认构造。
    // 主线先完成 495EE4..496066：路径对象+08 拼 params.dat；GENERIC_READ、
    // share=0、OPEN_EXISTING、FILE_ATTRIBUTE_NORMAL、继承句柄，完整读取非空文件；
    // CSerialize 读模式/cursor=0 -> params 根 Serialize(434940)，检查版本非负。
    // 原失败路径 496DA4..496DD6 返回 readyAt04=0，其余未写字段不可视为默认值。
    // 值接口要求已加载的 profile，负版本显式抛出；缺根在实际配置读点安全拒绝。
    // 41CC65/41CC8A/41CCAC：settingTags 文件序号 [0]/[1]/[2] 对应 +88/+7C/+94。
    SystemDataEnvironmentArchive(
        const ParamsRuntimeProfile& loadedProfile,
        ISystemDataEnvironmentValueSource& values,
        ISystemDataEnvironmentDiagnostics& diagnostics,
        SystemDataEnvironmentWindowTarget ownerTarget = {},
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());

    // 496090..4960B4：构造期间增持当前 GameSystem，不深拷贝整个 params。
    // 安装/窗口/启动字节使用已保留 GameSystem；配置段另按读点取当前根。
    // 嵌套安装表每个字段重新解析；根再次装载不改变这份保留引用。
    // 根的 shared_ptr 仍是现代所有权边界，不冒充原三字 Ref 的布局与分配 ABI。
    SystemDataEnvironmentArchive(
        const ParamsRuntimeRoot& loadedRoot,
        ISystemDataEnvironmentValueSource& values,
        ISystemDataEnvironmentDiagnostics& diagnostics,
        SystemDataEnvironmentWindowTarget ownerTarget = {},
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());

    // 完整495E10入口：同一正文正常返回ready=0/1，不用variant另造失败对象。
    SystemDataEnvironmentArchive(RuntimeStringBorrow currentDirectory,
        ParamsRuntimeRootOwner& params, IStartFileApi& fileApi,
        ISystemDataEnvironmentValueSource& values,
        ISystemDataEnvironmentDiagnostics& diagnostics,
        SystemDataEnvironmentWindowTarget ownerTarget = {},
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());
    // 495EBC在正文三次GetNil之后才捕获Environment+48；不得在工厂入口提前借路径。
    SystemDataEnvironmentArchive(EnvironmentRootOwner& environment,
        ParamsRuntimeRootOwner& params, IStartFileApi& fileApi,
        ISystemDataEnvironmentValueSource& values,
        ISystemDataEnvironmentDiagnostics& diagnostics,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource(),
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());
    ~SystemDataEnvironmentArchive() noexcept;
    SystemDataEnvironmentArchive(const SystemDataEnvironmentArchive&) = delete;
    SystemDataEnvironmentArchive& operator=(const SystemDataEnvironmentArchive&) = delete;

    // 498220 仅 retn 4：无 mode/ready 分支，无读来源，无字段或游标读写。
    // env.ini 的内容绝不覆盖本对象；SerializeBuffer 参数仅保持既有 Archive 接口形状。
    void Serialize(SerializeBuffer& serializer) noexcept;
    void SerializeNative18(void* serializer18) noexcept override;
    void* DeleteAtVslot04(std::uint32_t flags) noexcept override;

    bool readyAt04;
    std::array<std::uint8_t, 3> storageBytesAt05;
    // 495E51..495E5F 的 +08/+0C/+10 是 array Ref 的 data/index/control。
    // 顺序、重复项、空字符串均保留；47E2B0 的逐项路径验证由主线接入。
    union { SystemDataInstalledFiles installFilesAt08; };
    std::uint32_t valueAt14;
    // 495E66..495EB8把三串作为一组发布EH资格，不能自动逐成员展开。
    union { RuntimeCString stringAt18; };
    union { RuntimeCString stringAt1C; };
    union { RuntimeCString stringAt20; };
    const char* stringAt24;
    const char* stringAt28;
    const char* stringAt2C;
    const char* stringAt30;
    std::uint32_t valueAt34;
    std::uint32_t valueAt38;
    std::uint32_t valueAt3C;
    std::uint32_t valueAt40;
    std::int32_t valueAt44;
    std::uint32_t valueAt48;
    std::uint32_t valueAt4C;
    std::uint32_t valueAt50;
    std::int32_t valueAt54;

    // 495E10 汇编未写 +05..+07、+14..+17、+58..+67。
    // 不把未知值改成optional或零；仅原写入点初始化相应字段。
    std::array<std::uint32_t, 4> storageDwordsAt58;

private:
    struct UnreadyTag {};
    SystemDataEnvironmentArchive(UnreadyTag, IRuntimeStringManagerSource& strings);
    void InitializeFromDirectory(RuntimeStringBorrow currentDirectory,
        ParamsRuntimeRootOwner& params, IStartFileApi& fileApi,
        ISystemDataEnvironmentValueSource& values,
        ISystemDataEnvironmentDiagnostics& diagnostics,
        SystemDataEnvironmentWindowTarget ownerTarget,
        IRuntimeStringManagerSource& strings, ISettingTagErrorSource& errors);
    void InitializeFromLoadedRoot(const ParamsRuntimeRoot& root,
        ISystemDataEnvironmentValueSource& values,
        ISystemDataEnvironmentDiagnostics& diagnostics,
        SystemDataEnvironmentWindowTarget ownerTarget,
        IRuntimeStringManagerSource& strings,
        ISettingTagErrorSource& errors);
    void InitializeFromGameSystem(const ParamsGameSystem& gameSystem,
        ISystemDataEnvironmentValueSource& values,
        ISystemDataEnvironmentDiagnostics& diagnostics,
        SystemDataEnvironmentWindowTarget ownerTarget,
        const ParamsRuntimeRoot* currentRoot,
        IRuntimeStringManagerSource& strings,
        ISettingTagErrorSource& errors);
};
static_assert(sizeof(SystemDataEnvironmentArchive) == 0x68);
static_assert(offsetof(SystemDataEnvironmentArchive, readyAt04) == 0x04);
static_assert(offsetof(SystemDataEnvironmentArchive, storageBytesAt05) == 0x05);
static_assert(offsetof(SystemDataEnvironmentArchive, installFilesAt08) == 0x08);
static_assert(offsetof(SystemDataEnvironmentArchive, valueAt14) == 0x14);
static_assert(offsetof(SystemDataEnvironmentArchive, stringAt18) == 0x18);
static_assert(offsetof(SystemDataEnvironmentArchive, stringAt1C) == 0x1C);
static_assert(offsetof(SystemDataEnvironmentArchive, stringAt20) == 0x20);
static_assert(offsetof(SystemDataEnvironmentArchive, stringAt24) == 0x24);
static_assert(offsetof(SystemDataEnvironmentArchive, stringAt28) == 0x28);
static_assert(offsetof(SystemDataEnvironmentArchive, stringAt2C) == 0x2C);
static_assert(offsetof(SystemDataEnvironmentArchive, stringAt30) == 0x30);
static_assert(offsetof(SystemDataEnvironmentArchive, valueAt34) == 0x34);
static_assert(offsetof(SystemDataEnvironmentArchive, valueAt54) == 0x54);
static_assert(offsetof(SystemDataEnvironmentArchive, storageDwordsAt58) == 0x58);
}

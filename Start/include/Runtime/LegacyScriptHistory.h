#pragma once

#include <Runtime/MessageFormatSelectionRuntime.h>
#include <Runtime/PersistenceFileAccess.h>
#include <Runtime/ScriptParamsNativeBody.h>

#include <span>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 仅Baku已核合同：借用当前资源服务，不构造另一份CParams。
class ILegacyScriptHistorySource
{
public:
    virtual ~ILegacyScriptHistorySource() = default;
    // 416C30 -> 416AE0 -> 4C7340(nullptr)：构造未初始化的返回槽。
    // 4873C6的PATH/Temporary映射当前temporaryAt14，不是saveAt10。
    virtual void ConstructDirectoryAt4C7340(RuntimeCString* output) = 0;
    // 416C30 -> Registry(1) -> 41B450：比较当前姓/名与默认值。
    virtual std::uint8_t NamesMatchDefaultsAt41B450() = 0;
    // 姓名查询后另取当时的资源根/Message（416AD0 -> 41C380），返回拥有引用。
    // 不返回旧布局指针、不标记已读、不复用快读入口先前捕获的CParams。
    virtual std::int32_t ReadFormatAt41B4C0(std::int32_t index,
        MessageFormatSelectionRawTarget output, std::uint32_t variant, bool markRead) = 0;
    // 416C30 -> 416B00(text,0x40,nullptr)：使用当时标题/HWND，忽略普通返回。
    virtual void WarnAt416B00(const char* cp932Text, UINT type) = 0;
};

// 既有运行时没有TempFile；这里只补实际使用的三个系统叶。
class ILegacyScriptHistoryTempApi
{
public:
    virtual ~ILegacyScriptHistoryTempApi() = default;
    virtual BOOL MakeDirectoryPath(const char* path) = 0;
    virtual UINT CreateName(const char* directory, const char* prefix, char* output) = 0;
    virtual BOOL Delete(const char* path) = 0;
};
ILegacyScriptHistoryTempApi& DefaultLegacyScriptHistoryTempApi() noexcept;

class LegacyScriptHistoryTempFile;
class LegacyScriptHistoryContext final
{
public:
    ~LegacyScriptHistoryContext() noexcept(false);
    LegacyScriptHistoryContext(const LegacyScriptHistoryContext&) = delete;
    LegacyScriptHistoryContext& operator=(const LegacyScriptHistoryContext&) = delete;
    [[nodiscard]] const char* Path() const noexcept;
    [[nodiscard]] const char* BaseName() const noexcept;
    [[nodiscard]] std::span<const std::uint32_t> RecordOffsets() const noexcept { return offsets_; }
    [[nodiscard]] const ScriptParamsNativeBody& Owner() const noexcept { return owner_; }

private:
    friend LegacyScriptHistoryContext& InstallBakuLegacyScriptHistory(ScriptParamsNativeBody&,
        IStartFileApi&, ILegacyScriptHistoryTempApi&, IRuntimeStringManagerSource&);
    friend LegacyScriptHistoryContext* FindBakuLegacyScriptHistory(ScriptParamsNativeBody&);
    friend void ResetBakuLegacyScriptHistoryAt4CD090(ScriptParamsNativeBody&, ILegacyScriptHistorySource&);
    friend std::uint8_t RebuildBakuLegacyScriptHistoryAt4CD230(ScriptParamsNativeBody&,
        ILegacyScriptHistorySource&) noexcept;
    friend std::uint8_t WriteBakuLegacyScriptHistoryAt4CD400(ScriptParamsNativeBody&,
        std::int32_t, class FileStreamRuntime&, ILegacyScriptHistorySource&) noexcept;
    LegacyScriptHistoryContext(ScriptParamsNativeBody&, IStartFileApi&,
        ILegacyScriptHistoryTempApi&, IRuntimeStringManagerSource&) noexcept;
    ScriptParamsNativeBody& owner_;
    IStartFileApi& files_;
    ILegacyScriptHistoryTempApi& tempApi_;
    IRuntimeStringManagerSource& strings_;
    LegacyScriptHistoryTempFile* temp_ = nullptr;
    std::vector<std::uint32_t> offsets_;
};

// 显式Baku装配：CParams的referenceAt54唯一拥有上下文，析构/替换时释放。
// 不建全局索引或复制历史。依赖须长于CParams；槽被其它对象占用时拒绝，不覆盖。
LegacyScriptHistoryContext& InstallBakuLegacyScriptHistory(ScriptParamsNativeBody& params,
    IStartFileApi& files,
    ILegacyScriptHistoryTempApi& temp = DefaultLegacyScriptHistoryTempApi(),
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] LegacyScriptHistoryContext* FindBakuLegacyScriptHistory(ScriptParamsNativeBody& params);

// Reset无catch：先构造新TempFile再退旧者，替换后才清偏移，不清historyAt3C。
void ResetBakuLegacyScriptHistoryAt4CD090(ScriptParamsNativeBody& params,
    ILegacyScriptHistorySource& source);
[[nodiscard]] std::uint8_t RebuildBakuLegacyScriptHistoryAt4CD230(ScriptParamsNativeBody& params,
    ILegacyScriptHistorySource& source) noexcept;
// 单项catch-all返回0仍保留部分文件/索引，交外层重建器重置。
[[nodiscard]] std::uint8_t WriteBakuLegacyScriptHistoryAt4CD400(ScriptParamsNativeBody& params,
    std::int32_t format, FileStreamRuntime& file, ILegacyScriptHistorySource& source) noexcept;
[[nodiscard]] const char* BakuLegacyScriptHistoryWarning() noexcept;
}

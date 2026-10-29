#pragma once

#include <Runtime/ParamsPatternNative.h>
#include <Runtime/PatternResourceNameBorrow.h>
#include <Runtime/PatternResourceAccess.h>
#include <Runtime/RuntimeCStringArray.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
enum class PatternResourceKind : std::uint32_t
{
    FileNameItem = 0,
    FileGroup = 1,
    ExcPosition = 2,
    FileConvert = 3,
};

enum class PatternSpriteUpdateKind : std::uint8_t
{
    KeepCurrent,
    ClearSlot,
    LoadDirectFile,
    LoadFileGroup,
    LoadExcPosition,
    LoadFileConvert,
    MissingLogicalResource,
};

enum class FileGroupChange : std::uint8_t
{
    None = 0,
    PrimaryResource = 1,
    AuxiliaryCount = 2,
    AuxiliaryResource = 4,
};

// 4AD900 只生成名称 CString[] Ref，不提前复制 kind、成员数组、坐标或 flags。
using PatternResourcePlan = RuntimeCStringArrayReference;
[[nodiscard]] PatternResourcePlan BuildPatternResourcePlan(
    const ParamsPatternNative& pattern,
    std::uint32_t patternItemIndex,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] PatternResourcePlan BuildPatternResourcePlanNativeAt4AD900(
    const ParamsPatternNative& pattern, std::uint32_t patternItemIndex,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 两处均走45DA40 -> 40B530并正常释放；普通Pattern额外Clone输入，CG只借用chars。
// scanner的CString必须先完成40C760同序初始化；调用者自行管理scanner及结果串的不同寿命。
void ScanPatternResourceSuffixAt45DA40(RuntimeCString& suffix,const char* source,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
// 返回分类在释放回调前固定；正常Free可抛，展开清理不重试已撤资格的临时。
[[nodiscard]] bool HasPatternResourceSuffixAt4CB905(
    const RuntimeCString& source,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
[[nodiscard]] bool HasCgDisplayResourceSuffixAt4A8680(
    const char* source,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());

// 对应 0x47A020：使用当前 CRT 多字节区域规则，不区分大小写并返回首个匹配项。
[[nodiscard]] std::optional<std::size_t> FindPatternItemByResourceName(
    const ParamsPatternNative& pattern,
    const char* resourceNameCp932);
[[nodiscard]] std::optional<std::size_t> FindPatternItemByResourceName(
    const ParamsPatternNative& pattern, const char* resourceNameCp932, PatternResourceAccess access);
[[nodiscard]] inline std::optional<std::size_t> FindPatternItemByResourceName(
    const ParamsPatternNative& pattern, const std::string& resourceNameCp932)
{
    return FindPatternItemByResourceName(pattern, resourceNameCp932.c_str());
}

// 4D35A0/4A8780：只在首个同名项 kind==1 时复制原数组 Ref，不深复制字符。
[[nodiscard]] RuntimeCStringArrayReference FindPatternFileGroupByResourceName(
    const ParamsPatternNative& pattern, const char* resourceNameCp932);
[[nodiscard]] RuntimeCStringArrayReference FindPatternFileGroupByResourceName(
    const ParamsPatternNative& pattern, const char* resourceNameCp932, PatternResourceAccess access);

// 正常出口使用调用者缓存的 control；展开仍由数组 Ref 析构读取当前槽。
void ReleasePatternResourceArrayNormal(RuntimeCStringArrayReference& reference,
    YtlReferenceControlRuntime* cachedControl) noexcept;

// 仅查询当前分类；正式执行器在旧区域失效之后才逐分支读取 native item。
[[nodiscard]] PatternSpriteUpdateKind ClassifyPatternSpriteUpdate(
    const std::string& currentLogicalNameCp932,
    const RuntimeCString& targetName,
    const ParamsPatternNative& pattern);

// 4CBC56/59先读目标并验null，4CBC65才读取当前primary；不复制CString或重复读目标。
using PatternResourcePrimaryNameReader = const char* (*)(const void* context);
[[nodiscard]] FileGroupChange BuildFileGroupChangeMask(
    const RuntimeCStringArrayReference& targetComponents,
    const void* primaryContext, PatternResourcePrimaryNameReader readPrimary,
    PatternResourceNamesBorrow currentAuxiliaryResources, PatternResourceAccess access);
[[nodiscard]] FileGroupChange BuildFileGroupChangeMask(
    const RuntimeCStringArrayReference& targetComponents,
    const void* primaryContext,
    PatternResourcePrimaryNameReader readPrimary,
    PatternResourceNamesBorrow currentAuxiliaryResources);

// 4CBC61..4CBD69：1=主资源变化，2=辅助数量变化，4=辅助成员变化。
// 显式工具调用方已固定primary字符；此重载仅转借该指针到同一核。
[[nodiscard]] FileGroupChange BuildFileGroupChangeMask(
    const RuntimeCStringArrayReference& targetComponents,
    const char* currentPrimaryResourceCharacters,
    PatternResourceNamesBorrow currentAuxiliaryResources);

// 现代fixture仅转借既有string/vector，比较核不建立名称镜像。
[[nodiscard]] FileGroupChange BuildFileGroupChangeMask(
    const RuntimeCStringArrayReference& targetComponents,
    const std::string& currentPrimaryResourceCp932,
    const std::vector<std::string>& currentAuxiliaryResourcesCp932);
}

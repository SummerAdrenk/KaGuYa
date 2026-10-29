#pragma once

#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <Runtime/RuntimeCString.h>
#include <Runtime/SpriteAnimationInitializationSource.h>
#include <Runtime/PatternResourceAccess.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class EnvironmentRootOwner;

// 4CC61E/4CCA78：只借当前Environment来源；构造不Capture、不增持registry。
class PatternResourceNativeInitializationSource final : public ISpriteAnimationInitializationSource
{
public:
    explicit PatternResourceNativeInitializationSource(EnvironmentRootOwner& owner) noexcept : owner_(owner) {}
    std::uint8_t ReadRegistrySlot1ByteAt6D() const override;
private:
    EnvironmentRootOwner& owner_;
};

enum class PatternResourceLoadPath
{
    NormalDirect, NormalExc, NormalFileConvert, NormalGroup,
    CgDirect, CgExc, CgGroup
};

struct PatternResourceLoadRequest final
{
    PatternResourceLoadPath path;
    const char* capturedInputCharacters;
    std::uint32_t memberIndex;
    std::uintptr_t loadArgument;
    // native target原样转交此请求；旧四字段aggregate仍选原工具检查。
    PatternResourceAccess access = PatternResourceAccess::Checked;
};

class IPatternResourceFailureName
{
public:
    virtual ~IPatternResourceFailureName() = default;
    // 失败点才解析 Plan/inner Ref/局部 CString；返回后固定此槽，不在 Clone 后重解 Ref。
    [[nodiscard]] virtual const RuntimeCString* ResolveFailurePrefix() const = 0;
};

class IPatternResourceLoadAction
{
public:
    virtual ~IPatternResourceLoadAction() = default;
    // 仅 CG Direct：reader base 非零后、registry getter 前捕获中间 owner。
    virtual void CaptureCgDirectOwnerAfterRead() = 0;
    // 仅 Direct：registry getter 后检查目标并解析槽；false 进入原 load 错误链。
    [[nodiscard]] virtual bool ResolveDirectTargetAfterInitialization() = 0;
    // Exc/Convert/Group 的目标由 caller 在各自原捕获点提前固定，不在此重新取得。
    [[nodiscard]] virtual bool LoadFromReader(const ArchiveReaderReference& reader,
        std::uint8_t initialization, const PatternResourceLoadRequest& request,
        const ArchiveCodecServices& codecs) = 0;
    // 坐标/名称/更新矩形仍在 factory 存活期执行。Group 此处为空，整组提交留在循环外。
    virtual void CommitWhileFactoryAlive() = 0;
};

struct PatternResourceLoadServices final
{
    IArchiveResourceSource& source;
    const FileTransactionContext* transaction;
    IStartFileApi& files;
    IArchiveReaderMemoryApi& memory;
    const IEnvironmentKeyTableSource& key;
    const ISpriteAnimationInitializationSource& initialization;
    ArchiveCodecServices codecs;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};

// 4CB030/4A84E0 的单次读取/装载范围；不接管 Plan、inner Ref 或 caller 的局部 CString。
// 借用真实14h reader；Sprite/PL整体拥有链仍另有现代边界。
void ExecutePatternResourceLoad(const PatternResourceLoadRequest& request,
    const IPatternResourceFailureName& failureName, IPatternResourceLoadAction& action,
    const PatternResourceLoadServices& services);
}

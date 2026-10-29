#pragma once

#include <Runtime/ArchiveEntryReader.h>
#include <Runtime/PatternLayerRuntime.h>
#include <Runtime/SpriteRuntime.h>
#include <Runtime/SpriteAnimationInitializationSource.h>

#include <cstdint>
#include <stdexcept>
#include <string>

namespace KaGuYa::Start::Runtime
{
enum class SpriteArchiveLoadResult : std::uint8_t
{
    Success,
    ResourceReadFailed,
};

// 既有固定目录工具/非 P6 调用口。4CB030/4A84E0 必须使用下方实际 source 服务。
// AN/PL 必须先由各自格式解析器分流，不允许在这里启发式回落成 BMP/AP。
class SpriteArchiveResourceLoader final
{
public:
    SpriteArchiveResourceLoader(
        const ResourceCatalog& catalog,
        const FileTransactionContext* transactionContext,
        IStartFileApi& fileApi,
        const IEnvironmentKeyTableSource& environmentKeyBytes, ArchiveCodecServices codecs) noexcept;

    [[nodiscard]] SpriteArchiveLoadResult LoadDirectStatic(
        SpriteRuntime& sprite,
        const std::string& resourceNameCp932,
        std::uintptr_t loadArgument) const;

    [[nodiscard]] SpriteArchiveLoadResult LoadDirect(
        SpriteRuntime& sprite,
        const std::string& resourceNameCp932,
        bool playAnimationFromStart,
        std::uintptr_t loadArgument) const;

    [[nodiscard]] SpriteArchiveLoadResult LoadDirectFromEnvironment(
        SpriteRuntime& sprite,
        const std::string& resourceNameCp932,
        const ISpriteAnimationInitializationSource& environment,
        std::uintptr_t loadArgument) const;

    [[nodiscard]] SpriteArchiveLoadResult LoadFileGroupMemberFromEnvironment(
        SpriteRuntime& sprite,
        std::size_t memberIndex,
        const std::string& resourceNameCp932,
        const ISpriteAnimationInitializationSource& environment) const;

    [[nodiscard]] SpriteArchiveLoadResult LoadFileGroupPrimaryStatic(
        SpriteRuntime& sprite,
        const std::string& memberNameCp932,
        std::uint32_t bitmapFlags) const;

    [[nodiscard]] SpriteArchiveLoadResult LoadFileGroupPrimary(
        SpriteRuntime& sprite,
        const std::string& memberNameCp932,
        std::uint8_t animationInitialization) const;

    [[nodiscard]] SpriteArchiveLoadResult LoadFileGroupAuxiliary(
        SpriteRuntime& sprite,
        std::size_t memberIndex,
        const std::string& memberNameCp932) const;

private:
    const ResourceCatalog& catalog_;
    const FileTransactionContext* transactionContext_ = nullptr;
    IStartFileApi& fileApi_;
    const IEnvironmentKeyTableSource& environmentKeyBytes_;
    ArchiveCodecServices codecs_;
};

class PatternLayerArchiveResourceException final : public std::runtime_error
{
public:
    PatternLayerArchiveResourceException(
        std::string resourceNameCp932,
        std::size_t memberIndex);

    [[nodiscard]] const std::string& ResourceNameCp932() const noexcept;
    [[nodiscard]] std::size_t MemberIndex() const noexcept;

private:
    std::string resourceNameCp932_;
    std::size_t memberIndex_ = 0;
};

// 原生reader入口，名字沿用既有Modern后缀仅表示Sprite/44h动画临时正文仍为现代实现。
// 只借用outerRef并交给Sprite，在原getter点逐次解析；不复制name/payload或增持reader。
[[nodiscard]] bool LoadPatternSpriteFromReaderModern(SpriteRuntime& sprite,
    const ArchiveReaderReference& reader, std::uint8_t initialization,
    const PatternResourceLoadRequest& request, const ArchiveCodecServices& codecs);

// 只借用服务依赖；每次到原 getter 时点才由 factory 取得实际 source 的目录。
class PatternLayerArchiveResourceLoader final
    : public IPatternLayerResourceLoader
{
public:
    explicit PatternLayerArchiveResourceLoader(const PatternResourceLoadServices& services) noexcept;
    void ExecuteLoad(const PatternResourceLoadRequest& request,
        const IPatternResourceFailureName& failureName, IPatternResourceLoadAction& action,
        IRuntimeStringManagerSource& strings) override;

private:
    PatternResourceLoadServices services_;
};
}

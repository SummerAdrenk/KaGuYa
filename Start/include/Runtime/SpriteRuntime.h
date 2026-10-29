#pragma once

#include <Runtime/AnimationResourceRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/PatternResourceExecutor.h>
#include <Runtime/PictureLayerResourceRuntime.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class ArchiveReaderReference;

enum class SpriteLoadMode : std::uint32_t
{
    StaticWithoutAlpha = 0,
    StaticWithAlpha = 1,
    Animation = 2,
};

struct SpriteAnimationRuntimeState final
{
    std::int32_t stateFlags = 0;
    std::int32_t waitTicks = 0;
    std::int32_t commandIndex = 0;
    std::int32_t resultCode = -1;
    bool repeatCommands = false;
    std::int32_t frameMappingSlotCount = 0;
    std::vector<std::int32_t> currentFrameMappings;
    std::vector<std::int32_t> finalFrameMappings;
    std::vector<std::int32_t> branchRepeatCounters;
    std::vector<AnimationCommand> commands;
    std::vector<AnimationBranch> branches;

    void Reset() noexcept;
    void Initialize(
        const ParsedAnimationResource& resource,
        bool playFromStart);
    void Restart(bool playFromStart);
    void ApplyPatternFilterCommand(std::int32_t commandCount);
    // 4C1D20：只推进到末尾命令之前，不额外暂停或复制最终帧。
    void AdvanceToFinalCommandAt4C1D20();
    // 4C23A0：按既有槽数 memmove，不改变任何 vector 的大小或容量。
    void CopyFinalFrameMappingsAt4C23A0();
    [[nodiscard]] std::int32_t Advance() noexcept;
};

class SpriteRuntime final
{
public:
    explicit SpriteRuntime(IGraphicsSurfaceApi& graphicsApi) noexcept;

    SpriteRuntime(const SpriteRuntime&) = delete;
    SpriteRuntime& operator=(const SpriteRuntime&) = delete;
    SpriteRuntime(SpriteRuntime&&) noexcept = default;
    SpriteRuntime& operator=(SpriteRuntime&&) noexcept = default;

    // 43F140：清空内容但保留各 vector 的容量。
    void Clear();

    // 只借用外层Ref；缓存比较、Clear后的解析、成功命名分别重新Get和调用虚getter。
    // Sprite/AN/PL正文仍为现代实现，不把入口处的reader正文或名称冻结为快照。
    [[nodiscard]] bool LoadArchiveReaderAt480650(
        const ArchiveReaderReference& reader,
        std::uint8_t restart,
        std::uintptr_t loadArgument,
        const ArchiveCodecServices& codecs,
        bool* cacheHit = nullptr);
    [[nodiscard]] bool LoadArchiveReaderMemberAt4B0840(
        std::size_t memberIndex,
        const ArchiveReaderReference& reader,
        std::uint8_t animationInitialization,
        const ArchiveCodecServices& codecs);

    // 显式静态工具分支；仍保留reader getter时点，不先导出LoadedArchiveEntry。
    [[nodiscard]] bool LoadDirectStatic(
        const ArchiveReaderReference& reader,
        std::uintptr_t loadArgument);
    [[nodiscard]] bool LoadFileGroupPrimaryStatic(
        const ArchiveReaderReference& reader,
        std::uint32_t bitmapFlags);

    // 以下名称/span接口仅为显式现代值/fixture入口。
    // 480650 的静态分支。调用者必须先按 448010 将 AN 与静态资源分流。
    [[nodiscard]] bool LoadDirectStatic(
        const std::string& resourceNameCp932,
        std::uintptr_t loadArgument,
        std::span<const std::byte> bytes);

    // 480650 的完整 AN/静态分流入口；AN 解析错误返回 false，不能回落成静态图。
    [[nodiscard]] bool LoadDirectResource(
        const std::string& resourceNameCp932,
        bool playAnimationFromStart,
        std::uintptr_t loadArgument,
        std::span<const std::byte> bytes);

    // 4B0840 的 FileGroup 主成员静态分支；不提交逻辑组名与 active。
    [[nodiscard]] bool LoadFileGroupPrimaryStatic(
        const std::string& memberNameCp932,
        std::uint32_t bitmapFlags,
        std::span<const std::byte> bytes);

    // 4B0840 的主成员入口：先严格识别 AN，仅在异常码 3（非 AN）时转静态图片。
    [[nodiscard]] bool LoadFileGroupPrimaryResource(
        const std::string& memberNameCp932,
        std::uint8_t animationInitialization,
        std::span<const std::byte> bytes);

    // 4B0840 的辅助成员入口：仅 Animation Sprite 可进入；PL 失败只有异常码 3 才转静态图片。
    [[nodiscard]] bool LoadFileGroupAuxiliaryResource(
        std::size_t memberIndex,
        const std::string& memberNameCp932,
        std::span<const std::byte> bytes);

    // 4CB030:4CBD54..4CBE1C：按 1/2/4 变化掩码预清理 FileGroup 状态。
    void PrepareFileGroup(FileGroupChange changes);

    [[nodiscard]] PatternSpriteRectResult LocalDrawableRect() const noexcept;

    [[nodiscard]] SpriteLoadMode LoadMode() const noexcept;
    [[nodiscard]] bool Active() const noexcept;
    [[nodiscard]] std::uint8_t ActiveByte() const noexcept;
    [[nodiscard]] const std::string& LogicalName() const noexcept;
    [[nodiscard]] std::uintptr_t LoadArgument() const noexcept;
    [[nodiscard]] const std::string& PrimaryMemberName() const noexcept;
    [[nodiscard]] const std::vector<std::string>& AuxiliaryMemberNames() const noexcept;
    [[nodiscard]] std::int32_t Width() const noexcept;
    [[nodiscard]] std::int32_t Height() const noexcept;
    [[nodiscard]] std::int32_t X() const noexcept;
    [[nodiscard]] std::int32_t Y() const noexcept;
    // 480778/480780与4B50F6/4B50F9是同一对坐标；raw接口只改变位模式视图。
    [[nodiscard]] std::uint32_t OpaqueValueAt2C() const noexcept;
    [[nodiscard]] std::uint32_t OpaqueValueAt30() const noexcept;
    [[nodiscard]] std::size_t PrimarySurfaceCount() const noexcept;
    [[nodiscard]] const RuntimeGraphicsSurface& PrimarySurfaceAt(
        std::size_t surfaceIndex) const;
    [[nodiscard]] std::size_t AuxiliarySurfaceGroupCount() const noexcept;
    [[nodiscard]] std::size_t AuxiliarySurfaceCount(
        std::size_t groupIndex) const;
    [[nodiscard]] const RuntimeGraphicsSurface& AuxiliarySurfaceAt(
        std::size_t groupIndex,
        std::size_t surfaceIndex) const;
    [[nodiscard]] const SpriteAnimationRuntimeState& AnimationState() const noexcept;
    [[nodiscard]] const std::vector<std::int32_t>& CurrentFrameMappings() const noexcept;

    void SetLogicalName(std::string value);
    // P6 在原命名时点导入唯一现代字段；不是原目的 CString 的 Clone/Free ABI。
    void SetLogicalNameModernFromCString(const RuntimeCString& source);
    void SetActive(bool value) noexcept;
    // 4BFBF1/4BFC2D：Disp 写入原 byte，不归一化为 bool。
    void SetActiveByte(std::uint8_t value) noexcept;
    void SetOrigin(std::int32_t x, std::int32_t y) noexcept;
    void SetOpaqueValuesAt2CAnd30(
        std::uint32_t valueAt2C,
        std::uint32_t valueAt30) noexcept;
    void RestartAnimation(bool playFromStart);
    void ApplyPatternFilterAnimationCommand(std::int32_t commandCount);
    // 4C540E..4C5433：仅具名、Animation、非循环对象清 index/flags 并提交最终映射。
    // 保留 waitTicks/resultCode；Pattern 标脏与刷新仍由 Wait 调用方执行。
    void FinishNonRepeatingAnimationAt4C4FC0();
    // 4C15BA..4C1627：读档恢复需保留末尾 opcode 4 的独立处理。
    void RestoreAnimationAt4C14E0();

    // 46B1F0 -> 443740：由图层按帧推进当前动画，并原样返回动画解释器结果。
    [[nodiscard]] std::int32_t AdvanceAnimation() noexcept;

private:
    class ResourceInput;
    [[nodiscard]] bool LoadDirectInput(const ResourceInput& input,
        bool playAnimationFromStart, std::uintptr_t loadArgument, bool parseAnimation,
        bool* cacheHit = nullptr);
    [[nodiscard]] bool LoadPrimaryInput(const ResourceInput& input,
        std::uint8_t initialization, std::uint32_t bitmapFlags, bool parseAnimation);
    [[nodiscard]] bool LoadAuxiliaryInput(std::size_t memberIndex, const ResourceInput& input);
    [[nodiscard]] bool DirectCacheMatches(
        const char* resourceNameCp932,
        std::uintptr_t loadArgument) const noexcept;

    IGraphicsSurfaceApi* graphicsApi_ = nullptr;
    SpriteLoadMode loadMode_ = SpriteLoadMode::StaticWithoutAlpha;
    std::uint8_t active_ = 0;
    std::string logicalNameCp932_;
    std::uintptr_t loadArgument_ = 0;
    std::string primaryMemberNameCp932_;
    std::vector<std::string> auxiliaryMemberNamesCp932_;
    std::int32_t width_ = 0;
    std::int32_t height_ = 0;
    std::int32_t x_ = 0;
    std::int32_t y_ = 0;
    SpriteAnimationRuntimeState animation_{};
    std::vector<RuntimeGraphicsSurface> primarySurfaces_;
    std::vector<std::vector<RuntimeGraphicsSurface>> auxiliarySurfaces_;
};
}

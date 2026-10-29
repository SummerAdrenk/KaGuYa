#pragma once

#include <Runtime/ReadScriptEffectOwnerRuntime.h>
#include <Runtime/ScrVmUpdateHandler.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
// 各方法必须完成对应工厂的资源提交和 Ready 检查，而非只有 new CEffect。
class IReadScriptEffectFactory
{
public:
    virtual ~IReadScriptEffectFactory() = default;
    [[nodiscard]] virtual bool PrepareLayersAt46A980() = 0;
    virtual void RestorePatternsAt4C4330() = 0;
    virtual void CreateBlendAt4A7BF0(ReadScriptEffectOwnerRuntime& owner) = 0;
    virtual void CreateMosaicAt4B45C0(ReadScriptEffectOwnerRuntime& owner) = 0;
    virtual void CreateUniversalAt4C3A20(ReadScriptEffectOwnerRuntime& owner, const char* nameCp932) = 0;
    virtual void CreateTransitAt4C3650(ReadScriptEffectOwnerRuntime& owner, std::int32_t opcode) = 0;
    [[nodiscard]] virtual std::unique_ptr<IReadScriptEffectRuntime> CreateMoveAt49C0F0() = 0;
    [[nodiscard]] virtual std::unique_ptr<IReadScriptEffectRuntime> CreateFlashAt49BCA0(
        const std::string& nameCp932, std::int32_t count) = 0;
    [[nodiscard]] virtual std::unique_ptr<IReadScriptEffectRuntime> CreateShakeAt49D4E0(
        std::int32_t mode, std::int32_t scale, std::int32_t interval) = 0;
};

// main1只借这些调用，不拥有第二份Effect/Surface/Script状态。
// 原生实现按每个原调用点重新取根，固定实现仅供既有显式现代构造。
class IReadScriptUpdateEffectActions
{
public:
    virtual ~IReadScriptUpdateEffectActions() = default;
    virtual bool PrepareLayersAt46A980() = 0;
    virtual void RestorePatternsAt4C4330() = 0;
    virtual std::uint16_t ReadSelector() = 0;
    virtual const char* ReadResourceName() = 0;
    virtual void CreateBlend() = 0;
    virtual void CreateMosaic() = 0;
    virtual void CreateUniversal(const char*) = 0;
    virtual void CreateTransit(std::int32_t) = 0;
    virtual void CreateMove() = 0;
    virtual void CreateFlash(const char*, std::int32_t) = 0;
    virtual void CreateShake(std::int32_t, std::int32_t, std::int32_t) = 0;
    virtual bool HasEffect() const = 0;
    virtual std::uint8_t ReadyEffect() const = 0;
    virtual void ReleaseResources() = 0;
};
class FixedReadScriptUpdateEffectActions final : public IReadScriptUpdateEffectActions
{
public:
    FixedReadScriptUpdateEffectActions(ReadScriptEffectOwnerRuntime& owner,
        const ScrVmScriptStorage& storage, IReadScriptEffectFactory& factory) noexcept
        : owner_(owner), storage_(storage), factory_(factory) {}
    bool PrepareLayersAt46A980() override;
    void RestorePatternsAt4C4330() override;
    std::uint16_t ReadSelector() override;
    const char* ReadResourceName() override;
    void CreateBlend() override;
    void CreateMosaic() override;
    void CreateUniversal(const char*) override;
    void CreateTransit(std::int32_t) override;
    void CreateMove() override;
    void CreateFlash(const char*, std::int32_t) override;
    void CreateShake(std::int32_t, std::int32_t, std::int32_t) override;
    bool HasEffect() const override;
    std::uint8_t ReadyEffect() const override;
    void ReleaseResources() override;
private:
    ReadScriptEffectOwnerRuntime& owner_;
    const ScrVmScriptStorage& storage_;
    IReadScriptEffectFactory& factory_;
};

class ReadScriptUpdateEffectRuntime final
{
public:
    ReadScriptUpdateEffectRuntime(ReadScriptEffectOwnerRuntime& owner,
        const ScrVmScriptStorage& storage, IReadScriptEffectFactory& factory) noexcept;
    explicit ReadScriptUpdateEffectRuntime(IReadScriptUpdateEffectActions& actions) noexcept;
    ReadScriptUpdateEffectRuntime(const ReadScriptUpdateEffectRuntime& other) noexcept;
    ReadScriptUpdateEffectRuntime& operator=(const ReadScriptUpdateEffectRuntime&) = delete;
    [[nodiscard]] bool RunAt4C4760(ReadScriptSceneState& state);
private:
    void CreateDeferredEffect();
    std::optional<FixedReadScriptUpdateEffectActions> fixed_;
    IReadScriptUpdateEffectActions* actions_;
};
}

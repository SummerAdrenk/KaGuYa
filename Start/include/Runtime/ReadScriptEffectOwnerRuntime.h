#pragma once

#include <Runtime/ReadScriptEffectRuntime.h>
#include <Runtime/ReadScriptSuspendWaitRuntime.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

#include <memory>

namespace KaGuYa::Start::Runtime
{
// 每个 CReadScript 独有 +1C/+28/+34/+40 四个引用；不能由场景工厂全局共享。
// 正常退出须显式 ReleaseResourcesAt4C0990，析构仅清理引用，不重试已抛出的 Finish。
class ReadScriptEffectOwnerRuntime final : public IReadScriptSuspendEffectOwner
{
public:
    ReadScriptEffectOwnerRuntime() = default;
    ~ReadScriptEffectOwnerRuntime();
    ReadScriptEffectOwnerRuntime(const ReadScriptEffectOwnerRuntime&) = delete;
    ReadScriptEffectOwnerRuntime& operator=(const ReadScriptEffectOwnerRuntime&) = delete;

    bool HasEffectAt1C() const override;
    bool TickEffectAtVslot00() override;
    void DrawEffectAtVslot04() override;
    [[nodiscard]] std::uint8_t ReadyEffectAtVslot08() const;
    void ReleaseResourcesAt4C0990() override;
    // newEffect 已构造完毕，此时才结束旧效果；构造失败不会提前删除旧对象。
    void InstallEffect(std::unique_ptr<IReadScriptEffectRuntime> newEffect);

    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface>& SourceAt28() noexcept;
    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface>& PreviousAt34() noexcept;
    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface>& AuxiliaryAt40() noexcept;

private:
    void FinishAndReleaseEffect();
    [[nodiscard]] IReadScriptEffectRuntime& RequireEffect() const;
    std::unique_ptr<IReadScriptEffectRuntime> effect_;
    std::shared_ptr<RuntimeGraphicsSurface> source_, previous_, auxiliary_;
    bool finishAttempted_ = false;
};
}

#pragma once
#include <Platform/InputViews.h>

#include <Runtime/WaitMarkRuntime.h>
#include <Platform/Input.h>

namespace KaGuYa::Start::Runtime
{
class UnSaveDataRuntime;
class CountDownWindowResources;
class EnvironmentStartupRuntime;

// 4873CC 的真实 CALL 为418210；其+18是同一键盘，不是 Mouse 或新的输入快照。
class WaitMarkStateSourceRuntime final : public IWaitMarkStateSource
{
public:
    WaitMarkStateSourceRuntime(Platform::KeyboardView keyboard,
        const UnSaveDataRuntime& unsave) noexcept;
    WaitMarkStateSourceRuntime(const Platform::IInputRuntime& input,
        const EnvironmentStartupRuntime& environment) noexcept;
    bool AutomaticMarkSelected() const override;
    bool AnyRuntimeTripletSecondByteSet() const override;
    bool SlotTwoField20BitOneSet() const override;
private:
    [[nodiscard]] Platform::KeyboardView Keyboard() const;
    [[nodiscard]] const UnSaveDataRuntime& UnSaved() const;
    Platform::KeyboardView keyboard_ = nullptr;
    const UnSaveDataRuntime* unsave_ = nullptr;
    const Platform::IInputRuntime* input_ = nullptr;
    const EnvironmentStartupRuntime* environment_ = nullptr;
};

// 共用真正的 CDrawManager::LoadSprite；CountDown 名称不限制4814F0所装的资源。
class WaitMarkResourceLoaderRuntime final : public IWaitMarkResourceLoader
{
public:
    explicit WaitMarkResourceLoaderRuntime(CountDownWindowResources& resources) noexcept;
    bool Load(SpriteRuntime& sprite, const std::string& name, bool playFromStart) override;
private:
    CountDownWindowResources& resources_;
};

// 4768D0/46B390。当前 WaitMark owner 以 SetOrigin 保存原+2C/+30，故使用 X/Y；
// 不冒充采用另一套 OpaqueValue 位置的通用 Sprite renderer。
class WaitMarkRendererRuntime final : public IWaitMarkRenderer
{
public:
    explicit WaitMarkRendererRuntime(IGraphicsWindowSurfaceApi& graphics) noexcept;
    std::int32_t DrawToScreen(const SpriteRuntime& sprite, const RECT& dirty) override;
    std::int32_t DrawToSurface(const SpriteRuntime& sprite,
        GraphicsSurfaceDestination destination, const RECT* clip) override;
private:
    std::int32_t Render(const SpriteRuntime& sprite, const RECT* clip,
        const GraphicsSurfaceDestination* destination);
    IGraphicsWindowSurfaceApi& graphics_;
};
}

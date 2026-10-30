#pragma once

#include <Runtime/DemoSceneProductionRuntime.h>
#include <Runtime/DemoSpriteLayerRuntime.h>
#include <Runtime/DemoTransitionRuntime.h>
#include <Runtime/ParamsDemoStorage.h>
#include <Runtime/DemoSoundCommandServices.h>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
// 现有图形 owner 的显式边界，不宣称是原 Surface Ref ABI。
// loader 收到槽本身；返回后调用者重新解析当前目标，不缓存调用前的 body。
using DemoSurfaceSlot = std::unique_ptr<RuntimeGraphicsSurface>;

// 原 CDrawManager 两个装载入口和 CDemoSeq 的声音分流边界。
class IDemoSequenceResourceServices : public IDemoSoundCommandServices
{
public:
    virtual ~IDemoSequenceResourceServices() = default;
    [[nodiscard]] virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0() = 0;
    [[nodiscard]] virtual void* CaptureDrawManagerAt413BE0() = 0;
    [[nodiscard]] virtual bool LoadDemoSpriteAt4814F0(void* receiver,
        SpriteRuntime& target, const char* borrowedName, std::uint8_t flag) = 0;
    [[nodiscard]] virtual bool LoadDemoMaskAt480000(void* receiver,
        DemoSurfaceSlot& target, const char* borrowedName) = 0;
    virtual void ExecuteDemoSoundAt4BF692(const ParamsDemoCmdSound& body) = 0;
};

class DemoSequenceRuntime final : public ILogoDemoSequenceRuntime,
    public IEndingDemoSequenceRuntime
{
public:
    DemoSequenceRuntime(IDemoSequenceResourceServices& resources, IGraphicsSurfaceApi& graphics,
        IGraphicsSurfaceCompositionApi& composition, IGraphicsScreenUpdateApi& screen,
        IGraphicsSurfaceTransitionApi& transition, const DrawManagerRuntime& manager,
        std::int32_t width, std::int32_t height);
    ~DemoSequenceRuntime() override;
    void BindDemoDataAt4C2220(const YtlReferenceSlotRuntime& source) override;
    [[nodiscard]] std::int32_t ExecuteCommandAt4BF5A0();
    [[nodiscard]] std::int32_t Run(std::uint8_t completionRequested) override;
    [[nodiscard]] bool AdvanceTransition() override;
    void UpdateSpritesAndRedraw() override;

    [[nodiscard]] std::uint32_t CommandIndex() const noexcept;
    [[nodiscard]] std::int32_t WaitTicks() const noexcept;
    [[nodiscard]] DemoSpriteLayerRuntime& Sprites() noexcept;
    [[nodiscard]] const DemoTransitionRuntime& Transition() const noexcept;
    [[nodiscard]] YtlReferenceSlotRuntime& DemoDataReference() noexcept;
    [[nodiscard]] const YtlReferenceSlotRuntime& DemoDataReference() const noexcept;

private:
    [[nodiscard]] void* ResolveCommandBody(std::uint32_t byteOffset) const;
    [[nodiscard]] bool LoadMask(const char* borrowedName);
    void BeginTransition(const ParamsDemoCmdTransit& command);
    [[nodiscard]] DemoSpriteRuntime& ExistingSprite(std::size_t index);

    IDemoSequenceResourceServices& resources_;
    // 保留既有服务槽及布局；构造参数已交给各画布和sprites，后续由它们调用。
    [[maybe_unused]] IGraphicsSurfaceApi& graphics_;
    IGraphicsSurfaceCompositionApi& composition_;
    IGraphicsScreenUpdateApi& screen_;
    IGraphicsSurfaceTransitionApi& transitionApi_;
    RECT bounds_;
    // 声明顺序同时保留构造中途失败时的 +2C/+20/+14/+08 释放次序。
    DemoSurfaceSlot canvas_;
    DemoSurfaceSlot mask_;
    DemoSurfaceSlot previous_;
    DemoSurfaceSlot alphaCanvas_;
    std::unique_ptr<DemoSpriteLayerRuntime> sprites_;
    YtlReferenceSlotRuntime demoData_{};
    DemoTransitionRuntime transition_;
    std::int32_t waitTicks_ = 0;
    std::uint32_t commandIndex_ = 0;
};

// 同一个工厂可供 Logo/Ending 使用，但每次调用都构造独立 sequence。
class DemoSequenceFactoryRuntime final : public ILogoDemoSequenceFactory,
    public IEndingDemoSequenceFactory
{
public:
    DemoSequenceFactoryRuntime(IDemoSequenceResourceServices& resources, IGraphicsSurfaceApi& graphics,
        IGraphicsSurfaceCompositionApi& composition, IGraphicsScreenUpdateApi& screen,
        IGraphicsSurfaceTransitionApi& transition, const DrawManagerRuntime& manager,
        std::int32_t width, std::int32_t height) noexcept;
    std::unique_ptr<ILogoDemoSequenceRuntime> CreateLogoDemoSequence() override;
    std::unique_ptr<IEndingDemoSequenceRuntime> CreateEndingDemoSequence() override;
private:
    std::unique_ptr<DemoSequenceRuntime> Create();
    IDemoSequenceResourceServices& resources_;
    IGraphicsSurfaceApi& graphics_;
    IGraphicsSurfaceCompositionApi& composition_;
    IGraphicsScreenUpdateApi& screen_;
    IGraphicsSurfaceTransitionApi& transition_;
    const DrawManagerRuntime& manager_;
    std::int32_t width_, height_;
};
}

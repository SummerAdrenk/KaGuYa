#pragma once

#include <Runtime/ReadScriptUpdateEffectRuntime.h>

namespace KaGuYa::Start::Runtime
{
class DrawManagerRuntime;
class IGraphicsDirtyRectApi;
class IReadScriptEffectSurfaceServices;
class IReadScriptTransitionMaskLoader;
class IReadScriptMoveEffectFactory;
class LayerWindowRuntime;
class SaveDataEnvironmentArchive;

// 借用同一根对象图；所有接口均为必需实体，不创建私有 manager/Transit 或 Save 副本。
struct ReadScriptEffectFactoryDependencies final
{
    DrawManagerRuntime& manager;
    const SaveDataEnvironmentArchive& save;
    IReadScriptEffectSurfaceServices& resources;
    IReadScriptTransitionMaskLoader& masks;
    IReadScriptMoveEffectFactory& move;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsScreenSizeApi& sizes;
    IGraphicsSurfaceCompositionApi& composition;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsScreenUpdateApi& screen;
    IGraphicsDirtyRectApi& dirty;
};

// main1 的实际资源工厂。按原时点直接改写传入 owner 的槽，不作事务/旧资源保活。
// 正常连续流程由 main2 的 4C0990 先清旧四 Ref；工厂不自行补清理或前移 Finish。
class ReadScriptEffectFactoryRuntime final : public IReadScriptEffectFactory
{
public:
    explicit ReadScriptEffectFactoryRuntime(ReadScriptEffectFactoryDependencies dependencies) noexcept;
    [[nodiscard]] bool PrepareLayersAt46A980() override;
    void RestorePatternsAt4C4330() override;
    void CreateBlendAt4A7BF0(ReadScriptEffectOwnerRuntime& owner) override;
    void CreateMosaicAt4B45C0(ReadScriptEffectOwnerRuntime& owner) override;
    void CreateUniversalAt4C3A20(ReadScriptEffectOwnerRuntime& owner, const char* nameCp932) override;
    void CreateTransitAt4C3650(ReadScriptEffectOwnerRuntime& owner, std::int32_t opcode) override;
    [[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateMoveAt49C0F0() override;
    [[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateFlashAt49BCA0(
        const std::string& nameCp932, std::int32_t count) override;
    [[nodiscard]] std::unique_ptr<IReadScriptEffectRuntime> CreateShakeAt49D4E0(
        std::int32_t mode, std::int32_t scale, std::int32_t interval) override;

private:
    [[nodiscard]] LayerWindowRuntime& WindowZero() const;
    [[nodiscard]] std::shared_ptr<RuntimeGraphicsSurface> CompositeAt4A8340(bool alpha);
    ReadScriptEffectFactoryDependencies dependencies_;
};
}

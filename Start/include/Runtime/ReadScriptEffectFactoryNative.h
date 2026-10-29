#pragma once

#include <Runtime/ReadScriptEffectOwnerNative.h>
#include <Runtime/ReadScriptPacketNative.h>
#include <Runtime/ReadScriptFlashShakeNative.h>
#include <Runtime/MoveEffectNative.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/LayerWindowNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/DrawLayersView.h>
#include <Runtime/PostWindowNativeDrawScope.h>
#include <optional>

namespace KaGuYa::Start::Runtime
{
class IGraphicsDirtyRectApi;
class IGraphicsSurfaceTransitionApi;

// 480B30的外部资源边界，caller仍先捕获实际Draw receiver；不接shared_ptr适配。
class IReadScriptEffectMaskNativeServices
{
public:
    virtual ~IReadScriptEffectMaskNativeServices() = default;
    virtual bool LoadMaskAt480B30(DrawManagerNativeBody2Ch* receiver,
        YtlReferenceSlotRuntime& output, const char* name) = 0;
};

// 仅借同一根的外部服务；scope覆盖候选构造、旧Effect删除及临时Surface退役。
struct ReadScriptEffectFactoryNativeServices final
{
    IDrawManagerNativeRootSource& draw;
    PatternLayerNativeRenderServices& pattern;
    ReadScriptPacketNativeServices& packet;
    IReadScriptPacketEnvironmentSource& environment;
    IGraphicsScreenSizeApi& sizes;
    IGraphicsSurfaceTransitionApi& transition;
    IGraphicsDirtyRectApi& dirty;
    ReadScriptFlashShakeNativeServices& flashShake;
    LayerWindowNativeServices& layerWindow;
    // 原全局八层的调用期来源；独立效果夹具可明确借已有外层scope，生产必须提供同源集合。
    DrawLayersNativeCallServices drawCalls{};
};

class ReadScriptEffectNativeCallScope final
{
public:
    explicit ReadScriptEffectNativeCallScope(ReadScriptEffectFactoryNativeServices&);
    ReadScriptEffectNativeCallScope(const ReadScriptEffectNativeCallScope&) = delete;
    ReadScriptEffectNativeCallScope& operator=(const ReadScriptEffectNativeCallScope&) = delete;
private:
    MoveEffectNativeServices moveServices_;
    GraphicsSurfaceNativeScope surface_;
    ReadScriptPacketNativeScope packet_;
    MoveEffectNativeScope move_;
    ReadScriptFlashShakeNativeScope flashShake_;
    LayerWindowNativeScope window_;
    LayerListNativeScope layers_;
    std::optional<PostWindowNativeDrawScope> postLayers_;
};

// 对应四个完整资源准备函数；检查原base字段与Ready AL，不回滚已提交前缀。
void CreateReadScriptBlendNativeAt4A7BF0(ReadScriptEffectOwnerNative&,
    ReadScriptEffectFactoryNativeServices&);
void CreateReadScriptMosaicNativeAt4B45C0(ReadScriptEffectOwnerNative&,
    ReadScriptEffectFactoryNativeServices&);
void CreateReadScriptTransitNativeAt4C3650(ReadScriptEffectOwnerNative&, std::int32_t opcode,
    ReadScriptEffectFactoryNativeServices&);
void CreateReadScriptUniversalNativeAt4C3A20(ReadScriptEffectOwnerNative&, const char* name,
    ReadScriptEffectFactoryNativeServices&, IReadScriptEffectMaskNativeServices&);
}

#pragma once

#include <Runtime/MoveEffectNative.h>
#include <Runtime/DrawManagerNative.h>

namespace KaGuYa::Start::Runtime
{
struct ScriptNativeBody;
class RootRuntimeOwner;
class ParamsRuntimeRootOwner;
class InputRootOwner;
class IGraphicsScreenSizeApi;

// 每个方法对应一次原调用；不缓存GS数量、Program、Script或SaveData正文。
class IMoveEffectNativeConstructionSource : public IDrawManagerNativeRootSource
{
public:
    virtual SIZE ReadScreenSizeAt47CC00() = 0;
    virtual std::int32_t ReadLiveGameSystemCountAt49C282() = 0;
    virtual ScriptNativeBody* CaptureScriptAfter413BE0() = 0;
    virtual EnvironmentRootStorage* CaptureProgramAt4180C0() = 0;
    virtual std::uint32_t ReadSpeedIndexAt49C86B() = 0;
    virtual bool SuppressSnapshotAt49CDC7() = 0;
    // 原PE只证实四个表项；默认实现直接DWORD索引，不裁剪、不.at、不补速度。
    virtual std::int32_t ReadDenominatorAt602648(std::uint32_t index);
    virtual std::int32_t ReadThresholdAt602658(std::uint32_t index);
};

// Draw服务借用实际原生根源；不把现有现代PostWindow producer强转成2Ch正文。
class RootMoveEffectNativeConstructionSource final : public IMoveEffectNativeConstructionSource
{
public:
    RootMoveEffectNativeConstructionSource(RootRuntimeOwner& root, ParamsRuntimeRootOwner& params,
        EnvironmentRootOwner& environment, InputRootOwner& input,
        IDrawManagerNativeRootSource& draw, IGraphicsScreenSizeApi& screen) noexcept;
    SIZE ReadScreenSizeAt47CC00() override;
    std::int32_t ReadLiveGameSystemCountAt49C282() override;
    DrawManagerNativeBody2Ch* CaptureDrawManagerAfter413BE0() override;
    ScriptNativeBody* CaptureScriptAfter413BE0() override;
    EnvironmentRootStorage* CaptureProgramAt4180C0() override;
    std::uint32_t ReadSpeedIndexAt49C86B() override;
    bool SuppressSnapshotAt49CDC7() override;
private:
    RootRuntimeOwner& root_;
    ParamsRuntimeRootOwner& params_;
    EnvironmentRootOwner& environment_;
    InputRootOwner& input_;
    IDrawManagerNativeRootSource& draw_;
    IGraphicsScreenSizeApi& screen_;
};

// 仅创建条目及control；control失败时不补删已完成的3Ch正文。
void CreateMoveEffectItemAt49C33B(YtlReferenceSlotRuntime& local,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
YtlReferenceSlotRuntime* GrowMoveEffectItemsAt498DC0(DrawLayerReferenceArray& array,
    YtlReferenceSlotRuntime* position, const YtlReferenceSlotRuntime& source,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void AppendMoveEffectItemAt49D236(DrawLayerReferenceArray& array,
    const YtlReferenceSlotRuntime& source, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
MoveEffectNativeBody24h* ConstructMoveEffectNativeAt49C0F0(MoveEffectNativeBody24h& body,
    IMoveEffectNativeConstructionSource& source, PatternLayerNativeRenderServices& resources,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
}

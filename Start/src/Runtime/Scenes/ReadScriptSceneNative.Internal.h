#pragma once
#include <Runtime/ReadScriptSceneNative.h>
#include <Runtime/FreeScrollRuntime.h>
#include <Runtime/ReadScriptEffectFactoryNative.h>
#include <Runtime/ReadScriptSceneRestoreNative.h>
#include <Runtime/ReadScriptTextInteractionRuntime.h>
#include <Runtime/ReadScriptMenuInteractionRuntime.h>
#include <Runtime/ReadScriptTextServicesRuntime.h>
#include <Runtime/ReadScriptUpdateEffectNative.h>
#include <Runtime/ReadScriptSoundWaitSource.h>
#include <optional>

namespace KaGuYa::Start::Runtime::ReadScriptSceneNativeDetail
{
using Body = ReadScriptSceneNativeBodyBCh;
using Table = ReadScriptSceneNativeVtable;
ReadScriptSceneNativeServices& ActiveServices();
void RetireTask(Body&) noexcept;

// 只借正文各字段，不含optional/string/独立状态值；不冒充现代SceneState。
struct OwnedStateView final : IScrVmYieldOwnerState
{
    Body& body;
    std::int32_t& mainState;
    std::uint8_t& drawEnabled;
    std::int32_t& substate;
    std::int32_t& interactionValueAt10;
    std::int32_t& interactionValueAt14;
    std::int32_t& savedMainStateAt4C;
    std::int32_t& savedSubstateAt50;
    std::int32_t& neutralDwordAt54;
    std::int32_t& waitValueAt58;
    std::int32_t& waitValueAt5C;
    RuntimeCString& errorMessageAtB0;
    std::int32_t& suspendValueAtB4;
    std::uint8_t& interactionInitializedAtB8;
    explicit OwnedStateView(Body& b) noexcept
        : body(b), mainState(b.mainStateAt04), drawEnabled(b.drawEnabledAt08), substate(b.substateAt0C),
          interactionValueAt10(b.interactionValueAt10), interactionValueAt14(b.interactionValueAt14),
          savedMainStateAt4C(b.savedMainStateAt4C), savedSubstateAt50(b.savedSubstateAt50),
          neutralDwordAt54(b.neutralDwordAt54), waitValueAt58(b.waitValueAt58), waitValueAt5C(b.waitValueAt5C),
          errorMessageAtB0(b.errorMessageAtB0), suspendValueAtB4(b.suspendValueAtB4),
          interactionInitializedAtB8(b.interactionInitializedAtB8) {}
    void SetStateAndClearSubstate(std::int32_t value) noexcept override { mainState = value; substate = 0; }
};
// 服务scope必须早于展开guard成立、晚于guard退出；不写入BCh正文。
class EffectCallScope final
{
public:
    explicit EffectCallScope(ReadScriptSceneNativeServices& s)
    { if (s.effectCalls) calls_.emplace(*s.effectCalls); }
private:
    std::optional<ReadScriptEffectNativeCallScope> calls_;
};
// 独立单步用于验证状态7保留CL，生产Update仍保留原忙循环，不设现代次数上限。
void DispatchUpdateStep(Body&, std::uint8_t& retainedAl, ReadScriptSceneNativeServices&);
bool RunVm(OwnedStateView&, IReadScriptSceneNativeSource&);
bool RunEffect(OwnedStateView&, ReadScriptEffectOwnerNative&, IReadScriptSceneNativeSource&);
bool RunSuspend(OwnedStateView&, ReadScriptEffectOwnerNative&, IReadScriptSceneNativeSource&);
bool RunWait(OwnedStateView&, IReadScriptSceneNativeSource&);
bool RunText(OwnedStateView&, Body&, IReadScriptSceneNativeSource&);
bool RunMenu(OwnedStateView&, Body&, IReadScriptSceneNativeSource&);
bool RunCommandMenu(OwnedStateView&, Body&, IReadScriptSceneNativeSource&);

// Layer业务借当前根，HiddenWindow三个叶只消费本body+60，不由Source另造owner。
class InteractionLayers final : public IReadScriptLayerServices, private IFreeScrollOperations
{
public:
    InteractionLayers(Body& body, IReadScriptSceneNativeSource& source) noexcept : body_(body), source_(source) {}
    bool PrepareLayersAt46AD20(std::int32_t mode = 0) override;
    bool PrepareLayersAt46A980() override;
    void SetLayerVisible(std::uint32_t, std::uint8_t, std::int32_t) override;
    void ResetOpacityAt47EE40() override;
    void BeginHiddenWindowAt4ADF70() override;
    void EndHiddenWindowAt4AC970() override;
    bool UpdateHiddenWindowAt4BDD00() override;
    bool UpdateLayerSixAt4BE530() override;
private:
    bool BeginAt4ADF70() override;
    bool UpdateAt4BDD00() override;
    void EndAt4AC970() override;
    Body& body_;
    IReadScriptSceneNativeSource& source_;
};

class TextFlow final
{
public:
    TextFlow(IReadScriptSceneNativeSource& source, IReadScriptLayerServices& layers) noexcept
        : source_(source), layers_(layers) {}
    bool RunAt4B2730(OwnedStateView&);
private:
    bool RunRevealing(OwnedStateView&);
    bool RunWaiting(OwnedStateView&);
    bool RunCommand(OwnedStateView&);
    bool SuspendForLayers(OwnedStateView&);
    bool StartSaveLoad(OwnedStateView&, std::int32_t);
    void EnterWait(OwnedStateView&);
    void ToggleAuto();
    void EnableSkip();
    void StopForScene();
    [[noreturn]] void ThrowTextFailure(OwnedStateView&);
    IReadScriptSceneNativeSource& source_;
    IReadScriptLayerServices& layers_;
};
class MenuFlow final
{
public:
    MenuFlow(IReadScriptSceneNativeSource& source, IReadScriptLayerServices& layers, bool command = false) noexcept
        : source_(source), layers_(layers), command_(command) {}
    bool RunAt4B11B0(OwnedStateView&);
private:
    bool RunSelecting(OwnedStateView&);
    bool RunCommand(OwnedStateView&);
    bool SuspendForLayers(OwnedStateView&);
    bool StartSaveLoad(OwnedStateView&, std::int32_t);
    void StopForScene();
    IReadScriptMenuServices& Menu();
    IReadScriptSceneNativeSource& source_;
    IReadScriptLayerServices& layers_;
    bool command_;
};
class EffectFlow final
{
public:
    explicit EffectFlow(IReadScriptUpdateEffectActions& actions) noexcept : actions_(&actions) {}
    bool RunAt4C4760(OwnedStateView&);
private:
    void CreateDeferredEffect();
    IReadScriptUpdateEffectActions* actions_;
};
class WaitFlow final
{
public:
    WaitFlow(ReadScriptEffectOwnerNative& effects, IReadScriptSceneNativeSource& source) noexcept
        : effects_(effects), source_(source) {}
    bool RunSuspendAt4AC4C0(OwnedStateView&);
    bool RunWaitAt4C4FC0(OwnedStateView&);
private:
    bool SuppressEffectAt4C3430() const;
    bool SkipEffectAt4AA970() const;
    bool MousePressedEdge(std::size_t) const;
    void WriteWaitSkipped(std::int32_t);
    bool CompleteWait(OwnedStateView&);
    bool WaitForSound(OwnedStateView&, bool);
    bool WaitForSprite(OwnedStateView&, bool);
    DrawLayersView CaptureLayers() const;
    std::uint8_t PrepareLayersAt46A980();
    ReadScriptEffectOwnerNative& effects_;
    IReadScriptSceneNativeSource& source_;
};
}

#pragma once
#include <Runtime/LegacyCharacterSelectionSource.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiClickedEventNative.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <Runtime/SceneControllerNative.h>
#include <memory>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IScrVmCommandErrorSink;
struct LegacyCharacterSelectionServices final
{
    const LegacyCharacterSelectionProfile& profile;
    ILegacyCharacterSelectionSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsTransitApi& transitionGraphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IScrVmCommandErrorSink& errors;
    ScriptNamedValueNativeTables names;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
struct LegacyCharacterSelectionControl final
{
    GuiControlNativeBody50h* body;
    int view;
    bool visible=true, enabled=true;
};
enum class LegacyCharacterSelectionPhase { Opening, Ready, Confirming, Hold, Closing, Finished };
struct LegacyCharacterSelectionDialog final
{
    GuiDialogNativeBody5Ch common;
    LegacyCharacterSelectionServices* services;
    GuiClickedEventReferences events{};
    std::vector<LegacyCharacterSelectionControl> controls;
    std::vector<std::unique_ptr<SpriteRuntime>> animations;
    std::vector<GraphicsSurfaceReference> canvases;
    GraphicsSurfaceReference image, black, gauge;
    LegacyCharacterSelectionPhase phase=LegacyCharacterSelectionPhase::Opening;
    TransitOperation operation{};
    int mode=0, option=0, page=0, selected=-1, activeTab=0, hold=0, transitionStage=0;
    explicit LegacyCharacterSelectionDialog(LegacyCharacterSelectionServices&);
};
class LegacyCharacterSelectionScope final
{
public:
    explicit LegacyCharacterSelectionScope(LegacyCharacterSelectionServices&) noexcept;
private:
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
GuiDialogNativeBody5Ch* NewLegacyCharacterSelectionDialog(LegacyCharacterSelectionServices&);
const GuiDialogNativeVtable35& LegacyCharacterSelectionDialogMethods() noexcept;
void ResolveLegacyCharacterSelectionConfirmation(LegacyCharacterSelectionDialog&, int result);

struct LegacyCharacterSelectionScene;
struct LegacyCharacterSelectionSceneVtable final
{
    std::uint8_t (__thiscall* run)(LegacyCharacterSelectionScene*,int);
    LegacyCharacterSelectionScene* (__thiscall* destroy)(LegacyCharacterSelectionScene*,unsigned);
    void (__thiscall* update)(LegacyCharacterSelectionScene*);
    void (__thiscall* draw)(LegacyCharacterSelectionScene*);
};
// 现代正文按sizeof分配，服务由Root持有，不复刻旧0x18及EH。
struct LegacyCharacterSelectionScene final
{
    const LegacyCharacterSelectionSceneVtable* methods;
    LegacyCharacterSelectionServices* services;
    GuiDialogNativeBody5Ch* dialog;
    unsigned phase, savedFlags;
};
LegacyCharacterSelectionScene* ConstructLegacyCharacterSelectionScene(void* storage, LegacyCharacterSelectionServices&);
void UpdateLegacyCharacterSelectionScene(LegacyCharacterSelectionScene&);
void DestroyLegacyCharacterSelectionScene(LegacyCharacterSelectionScene&) noexcept;
}

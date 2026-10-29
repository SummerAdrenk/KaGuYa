#pragma once

#include <Runtime/LegacyMapSelectionSource.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/ScriptNamedValueNative.h>
#include <memory>
#include <utility>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class IScrVmCommandErrorSink;
struct LegacyMapSelectionServices final
{
    const LegacyMapSelectionProfile& profile;
    ILegacyMapSelectionSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGraphicsWindowSurfaceApi& drawing;
    IGraphicsTransitApi& transitionGraphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IScrVmCommandErrorSink& errors;
    ScriptNamedValueNativeTables names;
    IRuntimeStringManagerSource& strings=DefaultRuntimeStringManagerSource();
};
struct LegacyMapSelectionControl final
{
    GuiControlNativeBody50h* body;
    bool visible=true, enabled=true;
};
struct LegacyMapSelectionPanel final
{
    GuiDialogNativeBody5Ch common{};
    GuiClickedEventReferences events{};
    std::vector<LegacyMapSelectionControl> controls;
    int id;
    bool shown=true, interactive=true, dimmed=false;
    explicit LegacyMapSelectionPanel(int value):id(value){}
};
struct LegacyMapSelectionAnimation final
{
    int view, event;
    bool normalOnly;
    std::unique_ptr<SpriteRuntime> sprite;
};
struct LegacyMapSelectionCanvas final
{
    int view, event;
    GraphicsSurfaceReference surface;
    LegacyMapSelectionCanvas(int v,int e,IGraphicsSurfaceApi& api):view(v),event(e),surface(api){}
};
enum class LegacyMapSelectionPhase { Opening, Ready, Closing, Finished };
struct LegacyMapSelectionDialog final
{
    GuiDialogNativeBody5Ch common{};
    LegacyMapSelectionServices* services;
    std::vector<std::unique_ptr<LegacyMapSelectionPanel>> panels;
    std::vector<LegacyMapSelectionAnimation> animations;
    std::vector<LegacyMapSelectionCanvas> canvases;
    std::vector<GraphicsSurfaceReference> badges;
    std::vector<std::pair<int,GuiControlNativeBody50h*>> synchronizedMarkers;
    GraphicsSurfaceReference image,black;
    TransitOperation operation{};
    LegacyMapSelectionPhase phase=LegacyMapSelectionPhase::Opening;
    int transitionStage=0,area=-1,page=0,selected=-1,secondary=-1;
    int content=0, suspendedContent=0;
    bool toolResume=false;
    explicit LegacyMapSelectionDialog(LegacyMapSelectionServices&);
};
class LegacyMapSelectionScope final
{
public:
    explicit LegacyMapSelectionScope(LegacyMapSelectionServices&) noexcept;
private:
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
GuiDialogNativeBody5Ch* NewLegacyMapSelectionDialog(LegacyMapSelectionServices&);
const GuiDialogNativeVtable35& LegacyMapSelectionDialogMethods() noexcept;
void ResumeLegacyMapSelectionSettings(LegacyMapSelectionDialog&);

struct LegacyMapSelectionScene;
struct LegacyMapSelectionSceneVtable final
{
    std::uint8_t (__thiscall* run)(LegacyMapSelectionScene*,int);
    LegacyMapSelectionScene* (__thiscall* destroy)(LegacyMapSelectionScene*,unsigned);
    void (__thiscall* update)(LegacyMapSelectionScene*);
    void (__thiscall* draw)(LegacyMapSelectionScene*);
};
struct LegacyMapSelectionScene final
{
    const LegacyMapSelectionSceneVtable* methods;
    LegacyMapSelectionServices* services;
    GuiDialogNativeBody5Ch* dialog;
    unsigned phase,savedFlags;
};
LegacyMapSelectionScene* ConstructLegacyMapSelectionScene(void*,LegacyMapSelectionServices&);
void UpdateLegacyMapSelectionScene(LegacyMapSelectionScene&);
void DestroyLegacyMapSelectionScene(LegacyMapSelectionScene&) noexcept;
}

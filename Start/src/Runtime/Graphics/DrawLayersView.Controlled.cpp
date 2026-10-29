#include <Runtime/DrawLayersView.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/DrawManagerLayerRuntime.h>
#include <Runtime/LayerWindowRuntime.h>
#include <Runtime/TextWindowLayerRuntime.h>
#include <Runtime/MessageWindowLayerRuntime.h>
#include <Runtime/PatternLayerRuntime.h>
#include <Runtime/TextWindowFormatRuntime.h>
#include <Runtime/TextWindowTextServicesRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 显式现代模型的借用协议，默认producer迁原生后本文件移入tests，不作失败后的换路。
FrameLayerItem& Layer(void* p)
{if(!p)throw std::logic_error("controlled Draw owning slot is empty");return *static_cast<FrameLayerItem*>(p);}
const DrawLayerViewMethods layerMethods{
    [](void* p){return Layer(p).EnabledByteAt0C();},
    [](void* p,std::uint8_t v){Layer(p).SetEnabledByteAt0C(v);},
    [](void* p,GraphicsSurfaceDestination target,const RECT* clip){Layer(p).CompositeToSurface(target,clip);}};
const RegisteredLayersViewMethods registeredMethods{
    [](void* p,GraphicsSurfaceDestination target,const RECT* clip){static_cast<LayerList*>(p)->CompositeToSurface(target,clip);},
    [](void* p,IGraphicsScreenCalls& graphics,IEnvironmentRootMemory&){
        static_cast<LayerList*>(p)->RenderDirtyRects([&graphics](const RECT& rect){graphics.InvalidatedRectIfLoaded(&rect);});},
    [](void* p,const RECT* rect){static_cast<LayerList*>(p)->DirtyRects().Add(*rect);}};
PatternLayerRuntime& Pattern(void* p)
{
    auto* pattern=dynamic_cast<PatternLayerRuntime*>(static_cast<ILayerWindowPatternLayer*>(p));
    if(!pattern)throw std::logic_error("capture Pattern owner has the wrong concrete type");return *pattern;
}
const DrawPatternViewMethods patternMethods{
    [](void* p){
        if(!p)throw std::logic_error("controlled Draw window0 has an empty Pattern slot");
        return static_cast<ILayerWindowPatternLayer*>(p)->EnabledByteAt0C();},
    [](void*)->PatternLayerNativeBody80h&{throw std::logic_error("controlled Pattern cannot bind a native snapshot executor");},
    [](void* p)->PatternLayerRuntime&{return Pattern(p);},
    [](void* p){return static_cast<std::uint32_t>(Pattern(p).SpriteSlotCount());},
    [](void* p,std::uint32_t index){return DrawSpriteView(&Pattern(p).SpriteAt(index));},
    [](void* p){auto& pattern=Pattern(p);const auto& canvas=pattern.MainSurfaceAt18();pattern.DirtyRects().Add({0,0,canvas.Width(),canvas.Height()});},
    [](void* p,PatternLayerNativeRenderServices*){Pattern(p).RefreshMainAndPropagateAt443B00();}};
LayerWindowRuntime& Window(void* p)
{
    auto* window=dynamic_cast<LayerWindowRuntime*>(static_cast<FrameLayerItem*>(p));
    if(!window)throw std::logic_error("controlled Draw window0 is not CLayerWin");return *window;
}
const DrawWindowViewMethods windowMethods{
    [](void* p,DrawLayersNativeWindowServices*){Window(p).AdvanceEffectAndQueueDirtyAt4B5AC5();},
    [](void* p){Window(p).PropagateDirtyRectsAt4C4250();},
    [](void* p){return Window(p).EnabledByteAt0C();},
    [](void* p,std::uint32_t index){
        auto* pattern=Window(p).PatternLayerAt(index);
        if(!pattern)throw std::logic_error("controlled Draw window0 has an empty Pattern slot");
        return pattern->EnabledByteAt0C();},
    [](void* p){return static_cast<std::uint32_t>(Window(p).PatternLayerCount());},
    [](void* p,std::uint32_t index){
        auto& window=Window(p);
        if(index>window.PatternLayerCount())return DrawPatternView(static_cast<ILayerWindowPatternLayer*>(nullptr));
        return DrawPatternView(window.PatternLayerAt(index));},
    [](void* p,IGraphicsWindowSurfaceApi& graphics,PatternLayerNativeRenderServices*){Window(p).RestorePatternsAt4C4330(graphics);},
    [](void* p,IGraphicsWindowSurfaceApi& graphics,PatternLayerNativeRenderServices*){Window(p).RestoreSavedPatternsAt4C14E0(graphics);}};
TextWindowLayerRuntime& Text(void* p)
{
    // 4A95A2只固定receiver，类型检查在真正的字段读取点，不能提前到view选择。
    auto* text=dynamic_cast<TextWindowLayerRuntime*>(static_cast<FrameLayerItem*>(p));
    if(!text)throw std::logic_error("controlled Draw slot3 is not CTextWin");return *text;
}
const DrawTextViewMethods textMethods{
    [](void* p){return Text(p).GlyphCursorAt50();},
    [](void* p){return Text(p).HasNameAt70();},
    [](void* p){return Text(p).NameRectAt40();},
    [](void* p){return Text(p).TextState().unreadAt9A.value();},
    [](void* p,std::uint32_t refresh,DrawTextNativeRebuildServices*,DrawTextControlledRebuildServices* services){
        auto& window=Text(p);
        if(!services)throw std::logic_error("controlled Draw Text rebuild services were not bound");
        auto layout=services->layout.ForWindow(window);
        return services->format.RebuildForWindowAt4C1650(window,services->values,refresh!=0,layout);},
    [](void* p,TextWindowLayerNativeServices*){Text(p).ResetTextAt46D170();}};
const DrawLayersViewMethods managerMethods{
    [](void* p,std::uint32_t index){return DrawLayerView(static_cast<DrawManagerLayerRuntime*>(p)->LayerAt418C10(index));},
    [](void* p){return static_cast<std::uint32_t>(static_cast<DrawManagerLayerRuntime*>(p)->OwnedLayerCount());},
    [](void* p,IEnvironmentRootMemory&){return RegisteredLayersView(static_cast<DrawManagerLayerRuntime*>(p)->RegisteredLayers());},
    [](void* p){static_cast<DrawManagerLayerRuntime*>(p)->AdvanceFramesAt46B240();},
    [](void* p,IGraphicsWindowSurfaceApi& graphics){static_cast<DrawManagerLayerRuntime*>(p)->ResetOwnedLayersAt47E940(graphics);},
    [](void* p,std::uint32_t index,std::uint32_t value,std::int32_t second){
        auto* target=dynamic_cast<ILayerPrepareTarget*>(static_cast<DrawManagerLayerRuntime*>(p)->LayerAt418C10(index));
        if(!target)throw std::logic_error("controlled owning layer does not implement request +10");
        if(value>0xFFu)throw std::invalid_argument("controlled layer request supports only its explicit BYTE domain");
        return target->SetRequestedEnabledAtVslot10(std::uint8_t(value),second);},
    [](void* p,DrawLayersNativeWindowServices*){
        // 48DCD0仅固定槽0；空槽直到48DDC7真正读取Pattern数组才被受控模型拒绝。
        return DrawWindowView(static_cast<DrawManagerLayerRuntime*>(p)->LayerAt418C10(0),&windowMethods);},
    [](void* p){return DrawTextView(static_cast<DrawManagerLayerRuntime*>(p)->LayerAt418C10(3));},
    [](void* p){auto* message=dynamic_cast<MessageWindowLayerRuntime*>(static_cast<DrawManagerLayerRuntime*>(p)->LayerAt418C10(1));
        if(!message)throw std::logic_error("controlled Draw slot1 is not CMesWin");return message->EnabledByteAt0C();},
    [](void* p,ToolWindowNativeInteractionServices*,IToolWindowInteractionServices* controlled){
        return DrawToolView(static_cast<DrawManagerLayerRuntime*>(p)->LayerAt418C10(2),controlled);},
    [](void* p,SelectWindowNativeWorkflowServices*,DrawSelectionControlledServices controlled){
        return DrawSelectionView(static_cast<DrawManagerLayerRuntime*>(p)->LayerAt418C10(StartupSelectionLayerIndex()),controlled);},
    [](void* p,IOpacityWindowInputSource*,DrawOpacityControlledServices controlled){
        return DrawOpacityView(static_cast<DrawManagerLayerRuntime*>(p)->LayerAt418C10(StartupOpacityLayerIndex()),controlled);}};
}
DrawLayerView::DrawLayerView(FrameLayerItem* body) noexcept:body_(body),methods_(&layerMethods){}
RegisteredLayersView::RegisteredLayersView(LayerList& body) noexcept
    :body_(&body),methods_(&registeredMethods),memory_(&DefaultEnvironmentRootMemory()){}
DrawLayersView::DrawLayersView(DrawManagerLayerRuntime& body) noexcept
    :body_(&body),methods_(&managerMethods),memory_(&DefaultEnvironmentRootMemory()){}
DrawLayersView::DrawLayersView(const DrawManagerLayerRuntime& body) noexcept
    :DrawLayersView(const_cast<DrawManagerLayerRuntime&>(body)){}
DrawTextView::DrawTextView(FrameLayerItem* body) noexcept:body_(body),methods_(&textMethods){}
DrawPatternView::DrawPatternView(ILayerWindowPatternLayer* body) noexcept:body_(body),methods_(&patternMethods){}
}

#include <Runtime/DrawLayersView.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/DrawManagerNative.h>
#include <Runtime/LayerListNative.h>
#include <Runtime/LayerWindowNative.h>
#include <Runtime/TextWindowLayerNative.h>
#include <Runtime/PatternLayerNative.h>
#include <Runtime/TextWindowNativeFormat.h>
#include <Runtime/TextWindowNativeText.h>
#include <Runtime/PatternLayerNativeResourceTarget.h>
#include <stdexcept>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
GraphicsSurfaceNativeBody* Target(GraphicsSurfaceDestination target)
{
    // 原+04/+1C传CSurface*；显式handle扩展不能伪造一块原正文。
    if(!target.Body()&&target.Handle()!=0)throw std::invalid_argument("native layer composite requires a CSurface receiver");
    return const_cast<GraphicsSurfaceNativeBody*>(target.Body());
}
const DrawLayerViewMethods layerMethods{
    [](void* p){return static_cast<const volatile LayerItemNativeBody14h*>(p)->enabledAt0C;},
    [](void* p,std::uint8_t v){static_cast<LayerItemNativeBody14h*>(p)->enabledAt0C=v;},
    [](void* p,GraphicsSurfaceDestination target,const RECT* clip){
        auto* body=static_cast<LayerItemNativeBody14h*>(p);body->vptrAt00->compositeAt04(body,Target(target),clip);}};
const RegisteredLayersViewMethods registeredMethods{
    [](void* p,GraphicsSurfaceDestination target,const RECT* clip){
        auto* body=static_cast<LayerListNativeBody18h*>(p);body->vptrAt00->compositeAt1C(body,Target(target),clip);},
    [](void* p,IGraphicsScreenCalls& graphics,IEnvironmentRootMemory& memory){
        LayerListNativeScope scope(memory,&graphics);auto* body=static_cast<LayerListNativeBody18h*>(p);
        body->vptrAt00->drawDirtyAt18(body);},
    [](void* p,const RECT* rect){auto* body=static_cast<LayerListNativeBody18h*>(p);body->vptrAt00->addDirtyAt20(body,rect);}};
PatternLayerNativeBody80h* PatternAt(void* p,std::uint32_t index)
{
    const auto& array=static_cast<LayerWindowNativeBody4Ch*>(p)->patternsAt30;
    // 4AD2BF / 48DDDF都保留index==count的物理Ref读取。
    return index>static_cast<std::uint32_t>(DrawLayerReferenceCount(array))?nullptr:
        reinterpret_cast<PatternLayerNativeBody80h*>(ResolveDrawLayerReference(array.begin[index]));
}
const DrawPatternViewMethods patternMethods{
    [](void* p){return static_cast<const volatile PatternLayerNativeBody80h*>(p)->layer.enabledAt0C;},
    [](void* p)->PatternLayerNativeBody80h&{return *static_cast<PatternLayerNativeBody80h*>(p);},
    [](void*)->PatternLayerRuntime&{throw std::logic_error("native Pattern cannot bind a controlled snapshot executor");},
    [](void* p){
        const auto* control=static_cast<const volatile PatternLayerNativeBody80h*>(p)->spritesAt34.controlAt08;
        return control?static_cast<const volatile EnvironmentObjectReferenceControl*>(control)->countAt10:0u;},
    [](void* p,std::uint32_t index){
        const auto& slot=static_cast<PatternLayerNativeBody80h*>(p)->spritesAt34;
        if(!slot.baseAt00)return DrawSpriteView(static_cast<SpriteNativeBodyA0h*>(nullptr));
        const auto stride=slot.controlAt08?static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C:0u;
        return DrawSpriteView(reinterpret_cast<SpriteNativeBodyA0h*>(slot.baseAt00+(slot.biasAt04+index)*stride));},
    [](void* p){InvalidatePatternLayerNativeAt443650(*static_cast<PatternLayerNativeBody80h*>(p),nullptr);},
    [](void* p,PatternLayerNativeRenderServices* services){
        if(!services)throw std::logic_error("native Draw Pattern refresh services were not bound");
        RefreshPatternProcessedNativeAt443B00(*static_cast<PatternLayerNativeBody80h*>(p),*services);}};
const DrawWindowViewMethods windowMethods{
    [](void* p,DrawLayersNativeWindowServices* services){
        if(!services)throw std::logic_error("native Draw window effect services were not bound");
        AdvanceNativeLayerWindowEffectAt4B5AC5(*static_cast<LayerWindowNativeBody4Ch*>(p),services->state,services->effects);},
    [](void* p){PropagateLayerWindowDirtyNativeAt4C4250(*static_cast<LayerWindowNativeBody4Ch*>(p));},
    [](void* p){return static_cast<const volatile LayerWindowNativeBody4Ch*>(p)->window.layer.enabledAt0C;},
    [](void* p,std::uint32_t index){
        return static_cast<const volatile PatternLayerNativeBody80h&>(*PatternAt(p,index)).layer.enabledAt0C;},
    [](void* p){return static_cast<std::uint32_t>(DrawLayerReferenceCount(static_cast<LayerWindowNativeBody4Ch*>(p)->patternsAt30));},
    [](void* p,std::uint32_t index){return DrawPatternView(PatternAt(p,index));},
    [](void* p,IGraphicsWindowSurfaceApi&,PatternLayerNativeRenderServices* services){
        if(!services)throw std::logic_error("native Draw Pattern restore services were not bound");
        RestoreLayerWindowPatternsNativeAt4C4330(*static_cast<LayerWindowNativeBody4Ch*>(p),*services);},
    [](void* p,IGraphicsWindowSurfaceApi&,PatternLayerNativeRenderServices* services){
        if(!services)throw std::logic_error("native Draw saved Pattern restore services were not bound");
        RestoreSavedLayerWindowPatternsNativeAt4C14E0(*static_cast<LayerWindowNativeBody4Ch*>(p),*services);}};
const DrawTextViewMethods textMethods{
    [](void* p){return static_cast<const volatile TextWindowLayerNativeBodyE8h*>(p)->glyphCursorAt50;},
    [](void* p){return static_cast<const volatile TextWindowLayerNativeBodyE8h*>(p)->nameAt70.baseAt00!=0;},
    [](void* p){const volatile RECT& r=static_cast<TextWindowLayerNativeBodyE8h*>(p)->nameBoundsAt40;return RECT{r.left,r.top,r.right,r.bottom};},
    [](void* p){return static_cast<const volatile TextWindowLayerNativeBodyE8h*>(p)->unreadAt9A;},
    [](void* p,std::uint32_t refresh,DrawTextNativeRebuildServices* services,DrawTextControlledRebuildServices*){
        if(!services)throw std::logic_error("native Draw Text rebuild services were not bound");
        return services->RebuildForFixedBody(*static_cast<TextWindowLayerNativeBodyE8h*>(p),refresh);},
    [](void* p,TextWindowLayerNativeServices* services){
        if(!services)throw std::logic_error("native Draw Text reset services were not bound");
        ResetTextWindowNativeAt46D170(*static_cast<TextWindowLayerNativeBodyE8h*>(p),*services);}};
const DrawLayersViewMethods managerMethods{
    [](void* p,std::uint32_t index){return DrawLayerView(DrawLayerAt418C10(*static_cast<DrawManagerNativeBody2Ch*>(p),index));},
    [](void* p){return static_cast<std::uint32_t>(DrawLayerReferenceCount(static_cast<DrawManagerNativeBody2Ch*>(p)->ownedLayersAt14));},
    [](void* p,IEnvironmentRootMemory& memory){return RegisteredLayersView(ResolveDrawLayerListAt47AE80(*static_cast<DrawManagerNativeBody2Ch*>(p)),memory);},
    [](void* p){AdvanceDrawLayersAt46B240(*static_cast<DrawManagerNativeBody2Ch*>(p));},
    [](void* p,IGraphicsWindowSurfaceApi&){ResetDrawLayersAt47E940(*static_cast<DrawManagerNativeBody2Ch*>(p));},
    [](void* p,std::uint32_t index,std::uint32_t value,std::int32_t second){
        auto* body=DrawLayerAt418C10(*static_cast<DrawManagerNativeBody2Ch*>(p),index);
        // 同一原DWORD栈协议允许不同callee读BYTE/DWORD，不用窄声明截掉WaitMark高位。
        using Request=std::uint8_t(__thiscall*)(LayerItemNativeBody14h*,std::uint32_t,std::int32_t);
        Request request;std::memcpy(&request,reinterpret_cast<const std::byte*>(body->vptrAt00)+0x10,sizeof(request));
        return request(body,value,second);},
    [](void* p,DrawLayersNativeWindowServices* services){
        return DrawWindowView(DrawLayerAt418C10(*static_cast<DrawManagerNativeBody2Ch*>(p),0),&windowMethods,services);},
    [](void* p){return DrawTextView(reinterpret_cast<TextWindowLayerNativeBodyE8h*>(DrawLayerAt418C10(*static_cast<DrawManagerNativeBody2Ch*>(p),3)));},
    [](void* p){auto* layer=DrawLayerAt418C10(*static_cast<DrawManagerNativeBody2Ch*>(p),1);return static_cast<const volatile LayerItemNativeBody14h&>(*layer).enabledAt0C;},
    [](void* p,ToolWindowNativeInteractionServices* native,IToolWindowInteractionServices*){
        return DrawToolView(reinterpret_cast<ToolWindowNativeBody40h*>(DrawLayerAt418C10(*static_cast<DrawManagerNativeBody2Ch*>(p),2)),native);},
    [](void* p,SelectWindowNativeWorkflowServices* native,DrawSelectionControlledServices){
        return DrawSelectionView(reinterpret_cast<SelectWindowNativeBody6Ch*>(DrawLayerAt418C10(*static_cast<DrawManagerNativeBody2Ch*>(p),StartupSelectionLayerIndex())),native);},
    [](void* p,IOpacityWindowInputSource* native,DrawOpacityControlledServices){
        return DrawOpacityView(reinterpret_cast<OpacityWindowNativeBody30h*>(DrawLayerAt418C10(*static_cast<DrawManagerNativeBody2Ch*>(p),StartupOpacityLayerIndex())),native);}};
}
DrawLayerView::DrawLayerView(LayerItemNativeBody14h* body) noexcept:body_(body),methods_(&layerMethods){}
RegisteredLayersView::RegisteredLayersView(LayerListNativeBody18h* body,IEnvironmentRootMemory& memory) noexcept
    :body_(body),methods_(&registeredMethods),memory_(&memory){}
DrawLayersView::DrawLayersView(DrawManagerNativeBody2Ch* body,IEnvironmentRootMemory& memory) noexcept
    :body_(body),methods_(&managerMethods),memory_(&memory){}
DrawLayersView::DrawLayersView(DrawManagerNativeBody2Ch* body,IEnvironmentRootMemory& memory,
    DrawLayersNativeWindowServices& services) noexcept:DrawLayersView(body,memory){windowServices_=&services;}
DrawTextView::DrawTextView(TextWindowLayerNativeBodyE8h* body) noexcept:body_(body),methods_(&textMethods){}
DrawWindowView::DrawWindowView(LayerWindowNativeBody4Ch* body,DrawLayersNativeWindowServices* services) noexcept
    :body_(body),methods_(&windowMethods),services_(services){}
DrawPatternView::DrawPatternView(PatternLayerNativeBody80h* body) noexcept:body_(body),methods_(&patternMethods){}
}

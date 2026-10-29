#pragma once
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/DrawToolView.h>
#include <Runtime/DrawTextRebuildServices.h>
#include <Runtime/DrawSelectionView.h>
#include <Runtime/DrawSpriteView.h>
#include <Runtime/DrawOpacityView.h>

namespace KaGuYa::Start::Runtime
{
class FrameLayerItem;
class LayerList;
class DrawManagerLayerRuntime;
struct DrawManagerNativeBody2Ch;
struct LayerItemNativeBody14h;
struct LayerListNativeBody18h;
class TextWindowLayerRuntime;
struct TextWindowLayerNativeBodyE8h;
class ILayerWindowDrawState;
class ILayerWindowEffectNativeSource;
class ILayerWindowPatternLayer;
class PatternLayerRuntime;
struct PatternLayerNativeBody80h;
struct PatternLayerNativeRenderServices;
struct LayerWindowNativeBody4Ch;
struct PostWindowNativeLayerServices;
struct LayerWindowNativeServices;
struct DrawLayersNativeWindowServices;
struct DrawGuiNativeServices;
// 按值复制的调用期借用，不取根、不增持正文，也不拥有服务或业务快照。
// layers/layerRuntime成对提供；二者均空时沿用调用方已有scope。
// 仅安装PostWindowNativeDrawScope已覆盖的服务；完整绘制仍须调用方外层提供
// OpacityDialogNativeScope、GuiControlGraphicsScope及所需GUI输入服务，不由view补齐。
struct DrawLayersNativeCallServices final
{
    PostWindowNativeLayerServices* layers = nullptr;
    LayerWindowNativeServices* layerRuntime = nullptr;
    IGraphicsScreenCalls* graphics = nullptr;
    DrawLayersNativeWindowServices* windowEffects = nullptr;
    DrawGuiNativeServices* gui = nullptr;
};
struct DrawPatternViewMethods final
{
    std::uint8_t(*enabled)(void*);
    PatternLayerNativeBody80h&(*nativeBody)(void*);
    PatternLayerRuntime&(*controlledBody)(void*);
    std::uint32_t(*spriteCount)(void*);
    DrawSpriteView(*sprite)(void*,std::uint32_t);
    void(*invalidateAll)(void*);
    void(*refresh)(void*,PatternLayerNativeRenderServices*);
};
// 只借已选中的Pattern；原生正文和显式受控模型分别绑定，不能失败后换路。
class DrawPatternView final
{
public:
    DrawPatternView(ILayerWindowPatternLayer*) noexcept;
    DrawPatternView(PatternLayerNativeBody80h*) noexcept;
    [[nodiscard]] DrawPatternView WithNativeCallServices(DrawLayersNativeCallServices services) const noexcept
    {auto result=*this;result.callServices_=services;return result;}
    explicit operator bool() const noexcept{return body_!=nullptr;}
    std::uint8_t EnabledByteAt0C() const{return methods_->enabled(body_);}
    PatternLayerNativeBody80h& NativeBody() const{return methods_->nativeBody(body_);}
    PatternLayerRuntime& ControlledBody() const{return methods_->controlledBody(body_);}
    const void* Receiver() const noexcept{return body_;}
    std::uint32_t SpriteCountAtControl3C() const{return methods_->spriteCount(body_);}
    DrawSpriteView SpriteAt4AD550(std::uint32_t index) const{return methods_->sprite(body_,index);}
    void InvalidateAllAt443650() const;
    void RefreshAt443B00(PatternLayerNativeRenderServices* services=nullptr) const;
private:
    void* body_;const DrawPatternViewMethods* methods_;
    DrawLayersNativeCallServices callServices_{};
};
struct DrawLayersNativeWindowServices final
{
    const ILayerWindowDrawState& state;
    ILayerWindowEffectNativeSource& effects;
};
struct DrawWindowViewMethods final
{
    void(*advanceEffect)(void*,DrawLayersNativeWindowServices*);
    void(*propagateDirty)(void*);
    std::uint8_t(*enabled)(void*);
    std::uint8_t(*patternEnabled)(void*,std::uint32_t);
    std::uint32_t(*patternCount)(void*);
    DrawPatternView(*pattern)(void*,std::uint32_t);
    void(*restore)(void*,IGraphicsWindowSurfaceApi&,PatternLayerNativeRenderServices*);
    void(*restoreSaved)(void*,IGraphicsWindowSurfaceApi&,PatternLayerNativeRenderServices*);
};
class DrawWindowView final
{
public:
    DrawWindowView(LayerWindowNativeBody4Ch*,DrawLayersNativeWindowServices* services=nullptr) noexcept;
    DrawWindowView(void* body,const DrawWindowViewMethods* methods,DrawLayersNativeWindowServices* services=nullptr) noexcept
        :body_(body),methods_(methods),services_(services){}
    [[nodiscard]] DrawWindowView WithNativeCallServices(DrawLayersNativeCallServices services) const noexcept
    {auto result=*this;result.callServices_=services;return result;}
    void AdvanceEffectAndQueueDirtyAt4B5AC5() const;
    void PropagateDirtyRectsAt4C4250() const;
    const void* Receiver() const noexcept{return body_;}
    std::uint8_t EnabledByteAt0C() const{return methods_->enabled(body_);}
    std::uint8_t PatternEnabledAt4AD2A0(std::uint32_t index) const{return methods_->patternEnabled(body_,index);}
    std::uint32_t PatternLayerCount() const{return methods_->patternCount(body_);}
    DrawPatternView PatternAt4AD2A0(std::uint32_t index) const;
    void RestorePatternsAt4C4330(IGraphicsWindowSurfaceApi& graphics,PatternLayerNativeRenderServices* native=nullptr) const;
    void RestoreSavedPatternsAt4C14E0(IGraphicsWindowSurfaceApi& graphics,PatternLayerNativeRenderServices* native=nullptr) const;
private:
    void* body_;const DrawWindowViewMethods* methods_;DrawLayersNativeWindowServices* services_;
    DrawLayersNativeCallServices callServices_{};
};
struct DrawTextViewMethods final
{
    std::int32_t(*glyph)(void*);
    bool(*hasName)(void*);
    RECT(*nameRect)(void*);
    std::uint8_t(*unread)(void*);
    bool(*rebuild)(void*,std::uint32_t,DrawTextNativeRebuildServices*,DrawTextControlledRebuildServices*);
    void(*reset)(void*,TextWindowLayerNativeServices*);
};
class DrawTextView final
{
public:
    DrawTextView(FrameLayerItem*) noexcept;
    DrawTextView(TextWindowLayerNativeBodyE8h*) noexcept;
    [[nodiscard]] DrawTextView WithNativeCallServices(DrawLayersNativeCallServices services) const noexcept
    {auto result=*this;result.callServices_=services;return result;}
    std::int32_t GlyphCursorAt50() const{return methods_->glyph(body_);}
    bool HasNameAt70() const{return methods_->hasName(body_);}
    RECT NameRectAt40() const{return methods_->nameRect(body_);}
    std::uint8_t UnreadAt9A() const{return methods_->unread(body_);}
    std::uint8_t(*BorrowUnreadReaderAt9A() const noexcept)(void*){return methods_->unread;}
    const void* Receiver() const noexcept{return body_;}
    bool RebuildAt4C1650(std::uint32_t refreshUnread,DrawTextNativeRebuildServices* native,
        DrawTextControlledRebuildServices* controlled) const;
    void ResetTextAt46D170(TextWindowLayerNativeServices* native=nullptr) const;
private:
    void* body_;const DrawTextViewMethods* methods_;
    DrawLayersNativeCallServices callServices_{};
};
struct DrawLayerViewMethods final
{
    std::uint8_t(*enabled)(void*);
    void(*setEnabled)(void*,std::uint8_t);
    void(*composite)(void*,GraphicsSurfaceDestination,const RECT*);
};
// 只保留已经选中的receiver；读取和提交直接作用于原正文，不增持或复制业务状态。
class DrawLayerView final
{
public:
    DrawLayerView() noexcept=default;
    DrawLayerView(std::nullptr_t) noexcept{}
    DrawLayerView(FrameLayerItem*) noexcept;
    DrawLayerView(LayerItemNativeBody14h*) noexcept;
    [[nodiscard]] DrawLayerView WithNativeCallServices(DrawLayersNativeCallServices services) const noexcept
    {auto result=*this;result.callServices_=services;return result;}
    explicit operator bool() const noexcept{return body_!=nullptr;}
    std::uint8_t EnabledByteAt0C() const{return methods_->enabled(body_);}
    void SetEnabledByteAt0C(std::uint8_t value) const{methods_->setEnabled(body_,value);}
    void CompositeToSurface(GraphicsSurfaceDestination target,const RECT* clip) const;
    const void* Receiver() const noexcept{return body_;}
private:
    void* body_=nullptr;const DrawLayerViewMethods* methods_=nullptr;
    DrawLayersNativeCallServices callServices_{};
};
struct RegisteredLayersViewMethods final
{
    void(*composite)(void*,GraphicsSurfaceDestination,const RECT*);
    void(*drawDirty)(void*,IGraphicsScreenCalls&,IEnvironmentRootMemory&);
    void(*addDirty)(void*,const RECT*);
};
class RegisteredLayersView final
{
public:
    RegisteredLayersView(LayerList&) noexcept;
    RegisteredLayersView(LayerListNativeBody18h*,IEnvironmentRootMemory&) noexcept;
    [[nodiscard]] RegisteredLayersView WithNativeCallServices(DrawLayersNativeCallServices services) const noexcept
    {auto result=*this;result.callServices_=services;return result;}
    void CompositeToSurface(GraphicsSurfaceDestination target,const RECT* clip) const;
    void RenderDirtyRects(IGraphicsScreenCalls& graphics) const;
    void AddDirtyRect(const RECT* rect) const;
    const void* Receiver() const noexcept{return body_;}
private:
    void* body_;const RegisteredLayersViewMethods* methods_;IEnvironmentRootMemory* memory_;
    DrawLayersNativeCallServices callServices_{};
};
struct DrawLayersViewMethods final
{
    DrawLayerView(*layer)(void*,std::uint32_t);
    std::uint32_t(*count)(void*);
    RegisteredLayersView(*registered)(void*,IEnvironmentRootMemory&);
    void(*advance)(void*);
    void(*reset)(void*,IGraphicsWindowSurfaceApi&);
    std::uint8_t(*request)(void*,std::uint32_t,std::uint32_t,std::int32_t);
    DrawWindowView(*window0)(void*,DrawLayersNativeWindowServices*);
    DrawTextView(*text3)(void*);
    std::uint8_t(*messageEnabled)(void*);
    DrawToolView(*tool2)(void*,ToolWindowNativeInteractionServices*,IToolWindowInteractionServices*);
    DrawSelectionView(*selection5)(void*,SelectWindowNativeWorkflowServices*,DrawSelectionControlledServices);
    DrawOpacityView(*opacity6)(void*,IOpacityWindowInputSource*,DrawOpacityControlledServices);
};
class DrawLayersView final
{
public:
    DrawLayersView() noexcept=default;
    DrawLayersView(std::nullptr_t) noexcept{}
    DrawLayersView(DrawManagerLayerRuntime&) noexcept;
    DrawLayersView(const DrawManagerLayerRuntime&) noexcept;
    DrawLayersView(DrawManagerNativeBody2Ch*,IEnvironmentRootMemory& = DefaultEnvironmentRootMemory()) noexcept;
    DrawLayersView(DrawManagerNativeBody2Ch*,IEnvironmentRootMemory&,DrawLayersNativeWindowServices&) noexcept;
    [[nodiscard]] DrawLayersView WithNativeCallServices(DrawLayersNativeCallServices services) const noexcept
    {auto result=*this;result.callServices_=services;return result;}
    explicit operator bool() const noexcept{return body_!=nullptr;}
    DrawLayerView LayerAt418C10(std::uint32_t index) const;
    std::uint32_t OwnedLayerCount() const{return methods_->count(body_);}
    RegisteredLayersView RegisteredLayers() const;
    void AdvanceFramesAt46B240() const;
    void ResetOwnedLayersAt47E940(IGraphicsWindowSurfaceApi& graphics) const;
    // 仅从原CDrawManager owning数组取得frame槽，不为普通注册层/Pattern的三槽表扩展+10。
    // 原栈槽是DWORD；WaitMark消费整值，其它原生窗口各自在callee取低BYTE。
    std::uint8_t RequestOwnedLayerAtVslot10(std::uint32_t index,std::uint32_t value,std::int32_t second) const;
    const void* Receiver() const noexcept{return body_;}
    DrawWindowView WindowZero() const;
    DrawTextView TextAtSlotThree() const;
    std::uint8_t MessageWindowEnabledAtSlotOne() const{return methods_->messageEnabled(body_);}
    DrawToolView ToolAtSlotTwo(ToolWindowNativeInteractionServices* native=nullptr,
        IToolWindowInteractionServices* controlled=nullptr) const;
    DrawSelectionView SelectionAtSlotFive(SelectWindowNativeWorkflowServices* native=nullptr,
        DrawSelectionControlledServices controlled={}) const;
    DrawOpacityView OpacityAtSlotSix(IOpacityWindowInputSource* native=nullptr,
        DrawOpacityControlledServices controlled={}) const;
private:
    void* body_=nullptr;const DrawLayersViewMethods* methods_=nullptr;IEnvironmentRootMemory* memory_=nullptr;
    DrawLayersNativeWindowServices* windowServices_=nullptr;
    DrawLayersNativeCallServices callServices_{};
};
static_assert(std::is_trivially_copyable_v<DrawLayersNativeCallServices>);
static_assert(sizeof(DrawLayersNativeCallServices) == 5 * sizeof(void*));
static_assert(std::is_trivially_copyable_v<DrawLayersView>);
class IDrawLayersViewSource
{
public:
    virtual ~IDrawLayersViewSource()=default;
    virtual DrawLayersView CaptureDrawLayersAt413BE0()=0;
};
// 每个request独立捕获根；不把七次捕获改成一个固定manager的循环。
std::uint8_t PrepareDrawLayersViewAt46A980(IDrawLayersViewSource&, std::int32_t mode = 0);
std::uint8_t PrepareDrawLayersViewAt46AD20(IDrawLayersViewSource&,std::int32_t mode);
}

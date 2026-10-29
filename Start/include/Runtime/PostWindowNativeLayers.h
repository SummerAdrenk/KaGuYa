#pragma once
#include <Runtime/LayerWindowNative.h>
#include <Runtime/MessageWindowLayerNative.h>
#include <Runtime/TextWindowLayerNative.h>
#include <Runtime/ToolWindowNative.h>
#include <Runtime/WaitMarkNative.h>
#include <Runtime/SelectWindowNative.h>
#include <Runtime/OpacityWindowNative.h>
#include <Runtime/OpacityDialogNative.h>
#include <Runtime/CountDownWindowNative.h>
#include <Runtime/DateWindowNative.h>
#include <Runtime/CommandSelectionNative.h>
#include <Runtime/TokimekiGaugeNative.h>
#include <Runtime/LoveHolicGaugesNative.h>

namespace KaGuYa::Start::Runtime
{
// 仅借用489C90各构造点的服务。没有第二个Draw owner，也不缓存父表或append接收者。
struct PostWindowNativeLayerServices final
{
    ILayerWindowConstructionSource& layer;
    IGraphicsSurfaceApi& surfaces;
    IRuntimeStringManagerSource& strings;
    IEnvironmentRootMemory& memory;
    MessageWindowLayerNativeServices& message;
    ToolWindowNativeServices& tool;
    TextWindowLayerNativeServices& text;
    WaitMarkNativeServices& wait;
    SelectWindowNativeServices& select;
    IGuiControlResourceLoadServices& opacityResources;
    CountDownWindowNativeServices& countdown;
    const OpacityDialogNativeProfile& opacityProfile = MamatomoOpacityDialogNativeProfile();
    DateWindowNativeServices* date = nullptr;
    CommandSelectionNativeServices* command = nullptr;
    TokimekiGaugeNativeServices* tokimeki = nullptr;
    LoveHolicGaugesNativeServices* loveHolic = nullptr;
};
LayerWindowNativeBody4Ch* CreateAndAppendLayerWindowNativeAt48A6FF(IDrawManagerNativeRootSource&,
    ILayerWindowConstructionSource&, IGraphicsSurfaceApi&, IRuntimeStringManagerSource&, IEnvironmentRootMemory&);
ToolWindowNativeBody40h* CreateAndAppendToolWindowNativeAt48A7F7(IDrawManagerNativeRootSource&, ToolWindowNativeServices&);
WaitMarkNativeBody34h* CreateAndAppendWaitMarkNativeAt48A8F2(IDrawManagerNativeRootSource&, WaitMarkNativeServices&);
SelectWindowNativeBody6Ch* CreateAndAppendSelectWindowNativeAt48A96E(IDrawManagerNativeRootSource&, SelectWindowNativeServices&);
DateWindowNativeBody* CreateAndAppendDateWindowNative(IDrawManagerNativeRootSource&,
    DateWindowNativeServices*, IEnvironmentRootMemory&);
CommandSelectionNativeBody* CreateAndAppendCommandSelectionNative(IDrawManagerNativeRootSource&,
    CommandSelectionNativeServices*, IEnvironmentRootMemory&);
void CreateAndAppendPrimaryGaugeNative(IDrawManagerNativeRootSource&, TokimekiGaugeNativeServices*,
    LoveHolicGaugesNativeServices*, IEnvironmentRootMemory&);
void CreateAndAppendMasoGaugeNative(IDrawManagerNativeRootSource&, LoveHolicGaugesNativeServices*, IEnvironmentRootMemory&);
CountDownWindowNativeBody44h* CreateAndAppendCountDownWindowNativeAt48AA66(IDrawManagerNativeRootSource&, CountDownWindowNativeServices&);
// 八项之间不捕获异常、不回滚已提交对象；上层仍承担489C90原catch/progress退役。
void CreatePostWindowNativeLayersAt48A6FF(IDrawManagerNativeRootSource&, PostWindowNativeLayerServices&);
}

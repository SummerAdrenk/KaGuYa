#include "CgSceneNative.Internal.h"
#include <Runtime/CgSceneNativeGlobals.h>
#include <Runtime/GuiDirtyRectNative.h>

namespace KaGuYa::Start::Runtime::CgSceneNativeDetail
{
const char* SettingName() noexcept{return "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";}
const char* SceneName() noexcept{return "\x83\x56\x81\x5B\x83\x93\x8D\xC4\x90\xB6\x83\x82\x81\x5B\x83\x68";}
const char* MusicName() noexcept{return "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x83\x82\x81\x5B\x83\x68";}
const char* TitleName() noexcept{return "\x83\x5E\x83\x43\x83\x67\x83\x8B";}
CgButtonNativeBody84h* Group(Body& body) noexcept
{return Resolve<CgButtonNativeBody84h>(*Advance(Read(body.groupsAt38.begin),Read(body.groupAt10)));}
const CgButtonNativeVtable15* GroupMethods(CgButtonNativeBody84h& body) noexcept
{return reinterpret_cast<const CgButtonNativeVtable15*>(Read(body.common.vptrAt00));}
CgDisplayWindowNativeBody60h* Display(Body& body) noexcept{return Resolve<CgDisplayWindowNativeBody60h>(body.displayWindowAt9C);}
CgOperationWindowNativeBody30h* Operation(Body& body) noexcept{return Resolve<CgOperationWindowNativeBody30h>(body.operationWindowAtA8);}
namespace
{
template<class F>F Virtual(void* p,unsigned offset) noexcept
{return Read(Field<F>(Read(Field<void*>(p,0)),offset));}
void ResetCursor(CgSceneNativeServices& services)
{
    auto* input=services.source.CaptureInputAt418210(0x4B6E9B);
    auto* cursor=Resolve<CursorNativeBody10h>(input->cursorAt0C);
    InputNativeScope scope(services.operation.inputServices);
    Virtual<std::uint8_t(__thiscall*)(void*)>(cursor,8)(cursor);
}
void Start(Body& body,CgSceneNativeServices& services)
{
    auto& source=services.source;ResetOperation(body.operationAt44);body.operationAt44.opcode=57;
    body.operationAt44.source=Resolve<GraphicsSurfaceNativeBody>(body.auxiliaryAt90);body.stepAt74=20;
    auto* configuration=Registry(source,1,0x4B7007);
    auto* parameters=Resolve<void>(Field<Slot>(configuration,0x1C));
    // 4B7034的原全局chars在Root捕获前固定；volume指针在Root捕获后取得并立即读值。
    const auto* name=ProcessCgSceneNativeBackgroundMusic().GetString();
    auto* sound=Resolve<void>(source.RootSoundSlotAt24(Root(source,0x4B7041)));
    const auto value=Read(*Virtual<const int*(__thiscall*)(void*)>(parameters,0x14)(parameters));
    auto& mapping=Field<RuntimeCString>(Registry(source,2,0x4B707D),0x2C);
    source.PlayMusicAt4BCCC0(sound,mapping,name,value,1);body.phaseAt04=1;
}
void Animate(Body& body,ICgSceneNativeSource& source)
{
    auto& operation=body.operationAt44;
    if(source.AccelerateAt4AA970(Registry(source,5,0x4B709B,true)))
    {
        operation.current=Add(Read(operation.current),256);
        if(Read(operation.current)>=Read(operation.target))operation.current=Read(operation.target);
    }
    if(Read(operation.opcode)==-1||Read(operation.current)>=Read(operation.target))++body.phaseAt04;
    auto* draw=Draw(source,0x4B711A);
    source.RenderAt449DD0(Resolve<void>(Field<Slot>(draw,0x20)),operation);
    operation.current=Add(Read(operation.current),Read(body.stepAt74));
    if(Read(operation.current)>=Read(operation.target))operation.current=Read(operation.target);
    source.InvalidateAfter4182F0(0x4B717F,nullptr);
}
void CaptureBlack(Body& body,ICgSceneNativeSource& source)
{
    body.displayAt0C=0;CaptureScreen(body,source,0x4B6B4C,true);
    Field<unsigned>(Registry(source,2,0x4B6BD7),4)=0;
    ResetOperation(body.operationAt44);body.operationAt44.opcode=0;
    body.operationAt44.source=Resolve<GraphicsSurfaceNativeBody>(body.blackAt78);
    const auto previous=Read(Resolve<GraphicsSurfaceNativeBody>(body.capturedAt84)->handleAt04);
    ++body.phaseAt04;body.operationAt44.previous=previous;body.stepAt74=20;
}
void CaptureDisplay(Body& body,CgSceneNativeServices& services)
{
    auto& source=services.source;body.displayAt0C=0;CaptureScreen(body,source,0x4B6C7A,false);
    ResetOperation(body.operationAt44);body.operationAt44.opcode=0;
    body.operationAt44.previous=Read(Resolve<GraphicsSurfaceNativeBody>(body.capturedAt84)->handleAt04);body.stepAt74=20;
    ConfigureCgDisplayWindowPanNativeAt4C17F0(*Display(body),40);
    auto* display=Display(body);auto* parent=Read(display->window.layer.parentAt10);
    auto* dirty=Read(parent->vptrAt00)->dirtyAt14(parent);
    Read(dirty->vptrAt00)->addRectAt08(dirty,&display->window.boundsAt14);
    auto* draw=Draw(source,0x4B6D8A);
    auto* list=Resolve<LayerListNativeBody18h>(Field<Slot>(draw,8));
    auto* destination=Resolve<GraphicsSurfaceNativeBody>(body.auxiliaryAt90);
    Read(list->vptrAt00)->compositeAt1C(list,destination,nullptr);
    body.operationAt44.source=Resolve<GraphicsSurfaceNativeBody>(body.auxiliaryAt90);
    source.ClearDirtyAfter4182F0(0x4B6E36);body.phaseAt04=10;
}
}
}
namespace KaGuYa::Start::Runtime
{
void DrawCgSceneNativeAt4B5970(CgSceneNativeBodyB4h& body,CgSceneNativeServices& services)
{
    using namespace CgSceneNativeDetail;CgSceneNativeScope scope(services);auto& source=services.source;
    if(!Read(body.displayAt0C))return;
    if(Read(Field<std::uint8_t>(Registry(source,1,0x4B597F),0x6D)))
    {auto* display=Display(body);reinterpret_cast<const LayerItemNativeFrameVtable*>(Read(display->window.layer.vptrAt00))->advanceAt14(&display->window.layer);}
    auto* draw=Draw(source,0x4B59C5);auto* list=Resolve<LayerListNativeBody18h>(Field<Slot>(draw,8));
    Read(list->vptrAt00)->drawDirtyAt18(list);
}
void UpdateCgSceneNativeAt4B6A60(CgSceneNativeBodyB4h& body,CgSceneNativeServices& services)
{
    using namespace CgSceneNativeDetail;CgSceneNativeScope scope(services);auto& source=services.source;
    for(;;)
    {
        switch(Read(body.phaseAt04))
        {
        case 0:Start(body,services);continue;
        case 1:case 5:case 8:case 10:case 15:Animate(body,source);return;
        case 2:Field<unsigned>(Registry(source,2,0x4B7192),4)=3;body.phaseAt04=3;[[fallthrough]];
        case 3:
            if(PollSelection(body,services)){auto* group=Group(body);GroupMethods(*group)->drawAt34(group);}return;
        case 4:
            ResetOperation(body.operationAt44);body.operationAt44.opcode=58;
            body.operationAt44.source=Resolve<GraphicsSurfaceNativeBody>(body.blackAt78);
            body.stepAt74=20;body.phaseAt04=5;continue;
        case 6:
        {
            const auto route=Read(body.routeAt14);const auto site=route==1?0x4B7878u:route==2?0x4B786Cu:0x4B7860u;
            source.QueueSceneAt47F920(Scenes(source,site,0x4B7882),route==1?SceneName():route==2?MusicName():TitleName());return;
        }
        case 7:
        {
            auto* display=Display(body);
            display->window.layer.valueAt08=Read(body.selectionAt18.positionAt08.y);
            display->window.layer.valueAt04=Read(body.selectionAt18.positionAt08.x);
            source.WriteRootByte(Root(source,0x4B6B32),0x219,0);
            CaptureBlack(body,source);continue;
        }
        case 9:
            if(!ExecuteCgDisplayResourceNativeAt4A84E0(body,services.resources))return;
            CaptureDisplay(body,services);continue;
        case 11:
            ReleaseYtlReference(body.capturedAt84);body.displayAt0C=1;body.phaseAt04=12;
            Field<unsigned>(Registry(source,2,0x4B78E5),4)=5;[[fallthrough]];
        case 12:PollDisplay(body,services);return;
        case 13:PollHidden(body,services);return;
        case 14:CaptureBlack(body,source);continue;
        case 16:
        {
            ReleaseYtlReference(body.capturedAt84);ResetCursor(services);
            ResetCgButtonNativeAt4C1260(*Group(body),0);
            auto* group=Group(body);auto* destination=Resolve<GraphicsSurfaceNativeBody>(body.auxiliaryAt90);
            GroupMethods(*group)->compositeAt38(group,destination,nullptr);
            body.phaseAt04=0;source.WriteRootByte(Root(source,0x4B6F65),0x219,1);
            // 4B6F76直接落入state0；Root重入改phase也不能在此重新分派。
            Start(body,services);Animate(body,source);return;
        }
        default:return; // 原17项之外没有业务，不制造额外状态推进。
        }
    }
}
}

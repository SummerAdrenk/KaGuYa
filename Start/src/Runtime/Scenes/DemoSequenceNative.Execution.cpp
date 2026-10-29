#include "DemoSequenceNative.Internal.h"
#include <Runtime/DrawManagerNative.h>
#include <Runtime/GraphicsSurfaceRuntime.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
using namespace DemoNativeDetail;
namespace
{
char missingDemo[]="\x83\x66\x83\x82\x83\x66\x81\x5B\x83\x5E\x82\xAA\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49\x81\x49 : [CDemo]";
char unsupportedWait[]="Wait\x83\x52\x83\x7D\x83\x93\x83\x68\x82\xAA\x96\xA2\x91\xCE\x89\x9E\x82\xC5\x82\xB7\x81\x49\x81\x49 : [CDemo]";
char missingPicture[]="\x89\xE6\x91\x9C\x83\x74\x83\x40\x83\x43\x83\x8B\x82\xAA\x82\xC8\x82\xA2\x82\xC8\x81\x49\x81\x49 : [CDemo]";
void* Command(DemoSequenceNativeBodyA4h& body,std::uint32_t offset)
{
    auto* data=ResolveParamsDemoDataAt4A52A0(body.dataAt98);
    return ResolveParamsDemoCommand(At(data->commandsAt00,offset));
}
bool Equal(const RuntimeCString& value,const char* other)
{ return _mbscmp(reinterpret_cast<const unsigned char*>(value.GetString()),
    reinterpret_cast<const unsigned char*>(other))==0; }
void AppendSprite(DemoSequenceNativeBodyA4h& body,DemoSequenceNativeServices& services)
{
    auto* sprite=NewDemoSpriteNativeAt4BF85F(services.memory,services.strings);
    YtlReferenceSlotRuntime temporary;
    ConstructDemoSpriteReferenceAt4BF8C8(temporary,sprite,services.memory);
    try
    {
        auto* end=Read(body.spritesAt38.end);
        if (end==Read(body.spritesAt38.capacity))
            InsertDemoSpriteReferenceAt453F20(body.spritesAt38,end,temporary,services.memory);
        else
        {
            end->baseAt00=temporary.baseAt00;end->biasAt04=temporary.biasAt04;
            end->controlAt08=temporary.controlAt08;
            if (auto* control=end->controlAt08) ++control->referenceCountAt04;
            body.spritesAt38.end=reinterpret_cast<YtlReferenceSlotRuntime*>(reinterpret_cast<std::uintptr_t>(end)+12u);
        }
    }
    catch (...) { ReleaseDemoSpriteReferenceAt460260(temporary);throw; }
    // 4BF945正常尾缓存control，不能以payload回调后的槽重读取代它。
    ReleaseDemoSpriteReferenceCachedAt4BF945(temporary,temporary.controlAt08);
}
void BeginTransition(DemoSequenceNativeBodyA4h& body,const ParamsDemoCmdTransit& command,
    DemoSequenceNativeServices& services)
{
    auto& operation=body.operationAt60;
    ResetPacket(operation);body.stepAt04=Signed(Read(command.stepAt08));
    operation.opcode=Equal(command.effectAt04,"BLEND_BLT")?0:
        Equal(command.effectAt04,"UNIVERSAL_BLT")?1:-1;
    if (operation.opcode==-1) {body.stepAt04=256;operation.opcode=3;}
    if (operation.opcode==1)
    {
        if (Equal(command.maskAt0C,"")) throw 1;
        const auto* name=command.maskAt0C.GetString();
        auto* draw=services.source.CaptureDrawAt413BE0();
        if (!services.source.LoadMaskAt480000(draw,body.maskAt14,name)) throw 1;
        operation.mask=Surface(body.maskAt14)->handleAt04;
        operation.current=255;operation.range=128;operation.target=-128;
        services.surfaceTransitions.BackupScreenToSurfaceIfLoaded(Surface(body.previousAt20)->handleAt04);
        operation.previous=Surface(body.previousAt20)->handleAt04;
        ComposeDemoNativeAt4A7F00(body,Surface(body.alphaAt2C),services);
        operation.source=Surface(body.alphaAt2C);
        return;
    }
    if (operation.opcode==0)
    {
        services.surfaceTransitions.BackupScreenToSurfaceIfLoaded(Surface(body.previousAt20)->handleAt04);
        operation.previous=Surface(body.previousAt20)->handleAt04;
    }
    ComposeDemoNativeAt4A7F00(body,Surface(body.canvasAt08),services);
    operation.source=Surface(body.canvasAt08);
}
}
std::int32_t ExecuteDemoCommandNativeAt4BF5A0(DemoSequenceNativeBodyA4h& body,
    DemoSequenceNativeServices& services)
{
    GraphicsSurfaceNativeScope scope(services.surfaces);
    try
    {
        if (!Read(body.dataAt98.baseAt00)) throw missingDemo;
        const auto offset=Read(body.indexAt94)*12u;
        const auto type=*static_cast<const volatile std::uint32_t*>(Command(body,offset));
        switch (type)
        {
        default:return 0;
        case 1:return 2;
        case 2:
        {
            const auto& command=*static_cast<ParamsDemoCmdWait*>(Command(body,offset));
            if (Read(command.modeAt04)) throw unsupportedWait;
            body.waitAt00=Signed(Read(command.ticksAt08));return 2;
        }
        case 3:services.source.ExecuteSoundAt4BF692(*static_cast<ParamsDemoCmdSound*>(Command(body,offset)));return 3;
        case 4:
        {
            const auto& command=*static_cast<ParamsDemoCmdLoad*>(Command(body,offset));
            while (Signed(Read(command.slotAt04))>=Signed(Count(body.spritesAt38))) AppendSprite(body,services);
            const auto* name=command.nameAt08.GetString();
            auto* draw=services.source.CaptureDrawAt413BE0();
            auto* sprite=Sprite(At(body.spritesAt38,Read(command.slotAt04)*12u));
            if (!services.source.LoadSpriteAt4814F0(draw,&sprite->spriteAt00,name,1)) throw missingPicture;
            return 3;
        }
        case 5:BeginTransition(body,*static_cast<ParamsDemoCmdTransit*>(Command(body,offset)),services);return 1;
        case 6:
        {
            const auto& command=*static_cast<ParamsDemoCmdDisp*>(Command(body,offset));
            const auto count=Count(body.spritesAt38);const auto slot=Signed(Read(command.slotAt04));
            if (slot<0)
            {
                if (!count) return 3;
                std::uint32_t index=0;
                do {Sprite(At(body.spritesAt38,index*12u))->spriteAt00.activeAt08=Read(command.visibleAt08);++index;}
                while (index<Count(body.spritesAt38));
            }
            else
            {
                if (slot>=Signed(count)) throw 1;
                Sprite(At(body.spritesAt38,std::uint32_t(slot)*12u))->spriteAt00.activeAt08=Read(command.visibleAt08);
            }
            return 3;
        }
        case 7:
        {
            const auto height=services.source.CaptureEnvironmentAt4180C0()->heightAt1C;
            const auto width=services.source.CaptureEnvironmentAt4180C0()->widthAt18;
            AddDemoDirtyAt4B4530(body,{0,0,Signed(width),Signed(height)});return 3;
        }
        case 8:
        {
            const auto& command=*static_cast<ParamsDemoCmdMove*>(Command(body,offset));
            if (Signed(Read(command.slotAt04))>=Signed(Count(body.spritesAt38))) throw 1;
            const auto previous=DemoSpriteBoundsNativeAt4AD080(*Sprite(At(body.spritesAt38,Read(command.slotAt04)*12u))).rect;
            const auto duration=Signed(Read(command.durationAt08));const auto startX=Signed(Read(command.startXAt0C));
            const auto slot=Read(command.slotAt04);const auto endX=Signed(Read(command.endXAt14));
            const auto endY=Signed(Read(command.endYAt18));const auto startY=Signed(Read(command.startYAt10));
            auto* sprite=Sprite(At(body.spritesAt38,slot*12u));
            const auto comparedY=Signed(Read(command.endYAt18));const auto comparedX=Signed(Read(command.endXAt14));
            if (startX==comparedX&&startY==comparedY) throw 1;
            CommitDemoSpriteMotionNativeAt4BFD18(*sprite,duration,startX,startY,endX,endY);
            AddDemoDirtyAt4B4530(body,previous);return 3;
        }
        case 9:
        {
            const auto& command=*static_cast<ParamsDemoCmdPos*>(Command(body,offset));
            const auto end=reinterpret_cast<std::uintptr_t>(Read(body.spritesAt38.end));
            const auto begin=reinterpret_cast<std::uintptr_t>(Read(body.spritesAt38.begin));
            const auto slot=Signed(Read(command.slotAt04));
            if (slot>=Signed(std::uint32_t(end-begin))/12) throw 1;
            auto* sprite=Sprite(*reinterpret_cast<YtlReferenceSlotRuntime*>(begin+std::uint32_t(slot)*12u));
            sprite->spriteAt00.xAt2C=Signed(Read(command.xAt08));sprite->spriteAt00.yAt30=Signed(Read(command.yAt0C));
            return 3;
        }
        }
    }
    catch (...) { return -1; }
}
std::int32_t RunDemoSequenceNativeAt4BDAE0(DemoSequenceNativeBodyA4h& body,std::uint32_t completion,
    DemoSequenceNativeServices& services)
{
    if (completion) body.waitAt00=0;
    else if (const auto wait=Read(body.waitAt00);wait!=0)
    {body.waitAt00=Signed(std::uint32_t(wait)-1u);return 2;}
    const auto count=Count(body.spritesAt38);
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(body.spritesAt38.begin));
    if (count)
        for (std::uint32_t i=0;i<count;++i)
            if (Read(Sprite(*reinterpret_cast<YtlReferenceSlotRuntime*>(begin+i*12u))->directionAtA0)) return 2;
    if (completion)
    {
        services.surfaceTransitions.BackupScreenToSurfaceIfLoaded(Surface(body.previousAt20)->handleAt04);
        auto* draw=services.source.CaptureDrawAt413BE0();
        if (!services.source.LoadMaskAt480000(draw,body.maskAt14,"bg_black.bgd")) return -1;
        ResetPacket(body.operationAt60);body.operationAt60.opcode=0;
        body.operationAt60.source=Surface(body.maskAt14);
        body.operationAt60.previous=Surface(body.previousAt20)->handleAt04;body.stepAt04=8;
        return 1;
    }
    if (count)
        for (std::uint32_t i=0,offset=0;;++i,offset+=12u)
        {
            auto& slot=*reinterpret_cast<YtlReferenceSlotRuntime*>(begin+offset);
            if (Read(Sprite(slot)->spriteAt00.activeAt08)&&Read(Sprite(slot)->directionAtA0)) return 2;
            if (i+1u>=Count(body.spritesAt38)) break;
        }
    for (;;)
    {
        const auto result=ExecuteDemoCommandNativeAt4BF5A0(body,services);
        ++body.indexAt94;
        if (result>=-1&&result<=2) return result;
    }
}
bool AdvanceDemoTransitionNativeAt4C38A0(DemoSequenceNativeBodyA4h& body,DemoSequenceNativeServices& services)
{
    auto& operation=body.operationAt60;
    const bool universal=Read(operation.opcode)==1;
    const bool completed=universal?Read(operation.current)<Read(operation.target):
        Read(operation.opcode)==-1||Read(operation.current)>=Read(operation.target);
    auto* draw=services.source.CaptureDrawAt413BE0();
    (void)RenderDrawTransitionAt4C39E0(*draw,operation,services.transit);
    const auto step=Read(body.stepAt04);
    if (universal) operation.current=Signed(std::uint32_t(Read(operation.current))-std::uint32_t(step));
    else
    {
        operation.current=Signed(std::uint32_t(Read(operation.current))+std::uint32_t(step));
        const auto target=Read(operation.target);
        if (Read(operation.current)>=target) operation.current=target;
    }
    services.screen.InvalidatedRectIfLoaded(nullptr);
    return completed;
}
}

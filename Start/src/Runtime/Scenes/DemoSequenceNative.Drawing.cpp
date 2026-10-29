#include "DemoSequenceNative.Internal.h"
#include <Runtime/SpriteNativeDrawing.h>
#include <Runtime/SpriteAnimationNativeExecution.h>
#include <Runtime/GraphicsSurfaceRuntime.h>

namespace KaGuYa::Start::Runtime
{
using namespace DemoNativeDetail;
namespace
{
std::int32_t IntegerPart(std::uint32_t value)
{ return std::bit_cast<std::int16_t>(std::uint16_t(value>>16)); }
void ReplaceHighWord(std::uint32_t& value,std::int32_t target)
{ *reinterpret_cast<volatile std::uint16_t*>(reinterpret_cast<std::byte*>(&value)+2)=std::uint16_t(target); }
void AddClipped(DemoSequenceNativeBodyA4h& sequence,const RECT& rectangle)
{
    RECT clipped{};::IntersectRect(&clipped,&rectangle,&sequence.boundsAt50);
    if (!::IsRectEmpty(&clipped)) AddDirty(sequence,clipped);
}
void MoveSprite(DemoSequenceNativeBodyA4h& sequence,DemoSpriteNativeBodyBCh& sprite)
{
    auto direction=Read(sprite.directionAtA0);
    if (!direction) return;
    if (direction&2u)
    {
        sprite.fixedXAtA4=Read(sprite.fixedXAtA4)-Read(sprite.stepXAtB4);
        if (IntegerPart(Read(sprite.fixedXAtA4))<Read(sprite.targetXAtAC)) ReplaceHighWord(sprite.fixedXAtA4,Read(sprite.targetXAtAC));
    }
    else if (direction&1u)
    {
        sprite.fixedXAtA4=Read(sprite.fixedXAtA4)+Read(sprite.stepXAtB4);
        if (IntegerPart(Read(sprite.fixedXAtA4))>Read(sprite.targetXAtAC)) ReplaceHighWord(sprite.fixedXAtA4,Read(sprite.targetXAtAC));
    }
    if (direction&8u)
    {
        sprite.fixedYAtA8=Read(sprite.fixedYAtA8)-Read(sprite.stepYAtB8);
        if (IntegerPart(Read(sprite.fixedYAtA8))<Read(sprite.targetYAtB0)) ReplaceHighWord(sprite.fixedYAtA8,Read(sprite.targetYAtB0));
    }
    else if (direction&4u)
    {
        sprite.fixedYAtA8=Read(sprite.fixedYAtA8)+Read(sprite.stepYAtB8);
        if (IntegerPart(Read(sprite.fixedYAtA8))>Read(sprite.targetYAtB0)) ReplaceHighWord(sprite.fixedYAtA8,Read(sprite.targetYAtB0));
    }
    const auto x=IntegerPart(Read(sprite.fixedXAtA4));
    if (x==Read(sprite.targetXAtAC)) {direction&=0xCu;sprite.directionAtA0=direction;}
    const auto y=IntegerPart(Read(sprite.fixedYAtA8));
    if (y==Read(sprite.targetYAtB0)) {direction&=3u;sprite.directionAtA0=direction;}
    if (x==Read(sprite.spriteAt00.xAt2C)&&y==Read(sprite.spriteAt00.yAt30)) return;
    const auto previous=DemoSpriteBoundsNativeAt4AD080(sprite).rect;
    sprite.spriteAt00.xAt2C=x;sprite.spriteAt00.yAt30=y;
    const auto current=DemoSpriteBoundsNativeAt4AD080(sprite).rect;
    AddClipped(sequence,previous);AddClipped(sequence,current);
}
}
void UpdateDemoSpritesNativeAt4A7690(DemoSequenceNativeBodyA4h& body,DemoSequenceNativeServices& services)
{
    GraphicsSurfaceNativeScope scope(services.surfaces);
    if (Count(body.spritesAt38))
        for (std::uint32_t index=0;;++index)
        {
            const auto offset=index*12u;
            if (Read(Sprite(At(body.spritesAt38,offset))->spriteAt00.activeAt08))
            {
                MoveSprite(body,*Sprite(At(body.spritesAt38,offset)));
                // 4A78ED重读当前数组与同一偏移；不能沿用运动前固定的sprite正文。
                auto* sprite=Sprite(At(body.spritesAt38,offset));
                if (sprite->spriteAt00.logicalNameAt0C.GetLength()!=0&&Read(sprite->spriteAt00.modeAt04)==2u&&
                    AdvanceSpriteAnimationNativeAt443740(sprite->spriteAt00.animationAt34)==0)
                {
                    const auto rectangle=DemoSpriteBoundsNativeAt4AD080(*Sprite(At(body.spritesAt38,offset))).rect;
                    if (!::IsRectEmpty(&rectangle)) AddDirty(body,rectangle);
                }
            }
            if (index+1u>=Count(body.spritesAt38)) break;
        }
    RedrawDemoNativeAt4C3D50(body,services);
}
void ComposeDemoNativeAt4A7F00(DemoSequenceNativeBodyA4h& body,GraphicsSurfaceNativeBody* target,
    DemoSequenceNativeServices& services)
{
    const auto handle=Read(target->handleAt04);
    if (handle) services.drawing.SurfaceClearRaw(handle,Read(target->clearColorAt20),nullptr);
    if (!Count(body.spritesAt38)) return;
    for (std::uint32_t index=0;;++index)
    {
        const auto offset=index*12u;
        if (Read(Sprite(At(body.spritesAt38,offset))->spriteAt00.activeAt08))
            (void)CompositeSpriteNativeAt46B390(Sprite(At(body.spritesAt38,offset))->spriteAt00,target,nullptr,services.drawing);
        if (index+1u>=Count(body.spritesAt38)) break;
    }
}
void RedrawDemoNativeAt4C3D50(DemoSequenceNativeBodyA4h& body,DemoSequenceNativeServices& services)
{
    auto* dirty=Dirty(body);dirty->vptrAt00->coalesceAt04(dirty);
    dirty=Dirty(body);
    const auto count=Signed(dirty->vptrAt00->countAt18(dirty));
    if (count!=0)
    {
        dirty=Dirty(body);auto* rectangles=dirty->vptrAt00->rectsAt1C(dirty);
        for (std::int32_t i=0;i<count;++i)
        {
            auto* rectangle=reinterpret_cast<RECT*>(reinterpret_cast<std::uintptr_t>(rectangles)+std::uint32_t(i)*16u);
            services.screen.ClearScreenIfLoaded(rectangle);
            if (Count(body.spritesAt38))
                for (std::uint32_t index=0;;++index)
                {
                    const auto offset=index*12u;
                    if (Read(Sprite(At(body.spritesAt38,offset))->spriteAt00.activeAt08))
                        (void)DrawSpriteNativeAt4768D0(Sprite(At(body.spritesAt38,offset))->spriteAt00,rectangle,services.drawing);
                    if (index+1u>=Count(body.spritesAt38)) break;
                }
            services.screen.InvalidatedRectIfLoaded(rectangle);
        }
    }
    // 零和负count也执行最终clear；失败则保留原dirty内容，不做finally式清除。
    dirty=Dirty(body);dirty->vptrAt00->clearAt00(dirty);
}
}

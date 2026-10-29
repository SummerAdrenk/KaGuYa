#include "CgUiButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
using namespace CgUiButtonNativeDetail;
std::int32_t ProcessCgUiButtonNativeAt4A98E0(CgUiButtonNativeBody6Ch& body,std::int32_t forced,CgUiButtonNativeServices& services)
{
    CgUiButtonNativeScope scope(services);
    if(!Count(body.surfacesAt5C))return 0;
    const auto initial=Read(body.stateAt04);
    if(initial<0||initial==3||(initial==4&&Read(body.inputModeAt1C)==2))return 0;
    const auto latched=Read(body.latchAt20);
    const auto active=body.vptrAt00->timelineAt04(&body);
    if(latched)
    {
        if(active)return 1;
        body.stateAt04=0;body.latchAt20=0;return 2;
    }
    if(active){body.stateAt04=2;body.latchAt20=1;return 1;}
    const auto state=Read(body.stateAt04);
    body.stateAt04=Read(body.inputModeAt1C)!=0&&state>=4?4:0;
    if(!forced)
    {
        POINT point;body.vptrAt00->pointerAt08(&body,&point);
        const auto height=Read(body.extentAt38.cy);const auto width=Read(body.extentAt38.cx);
        const auto* position=Read(body.useNaturalAt2C)?&body.naturalAt30:&body.positionAt24;
        RECT hit{0,0,width,height};::OffsetRect(&hit,Read(position->x),Read(position->y));
        if(!Read(body.pixelHitAt08)&&!::IsRectEmpty(&body.hitRectAt0C))hit=body.hitRectAt0C;
        if(!::PtInRect(&hit,point))return 0;
        if(Read(body.pixelHitAt08))
        {
            auto* surface=ResolveCgUiButtonSurfaceNativeAt4ADED0(body,StateSlot(body,state));
            if(!surface)surface=ResolveCgUiButtonSurfaceNativeAt4ADED0(body,0);
            if(Read(surface->alphaAt18))
            {
                const auto dx=Sub(Read(surface->originXAt10),Read(body.naturalAt30.x));
                const auto dy=Sub(Read(surface->originYAt14),Read(body.naturalAt30.y));
                position=Read(body.useNaturalAt2C)?&body.naturalAt30:&body.positionAt24;
                const auto y=Sub(point.y,Add(Read(position->y),dy));
                const auto x=Sub(Sub(point.x,dx),Read(position->x));
                const auto handle=Read(surface->handleAt04);
                if(!(services.pixels.GetSurfacePixel(handle,x,y)&0xFF000000u))return 0;
            }
        }
    }
    body.stateAt04=state;
    switch(std::uint32_t(state))
    {
    case 0:case 4:body.stateAt04=Add(state,1);return 0;
    case 1:case 5:
        if(body.vptrAt00->pressAt0C(&body))body.stateAt04=Add(Read(body.stateAt04),1);return 0;
    case 2:case 6:
        if(!body.vptrAt00->releaseAt10(&body))return 0;
        switch(Read(body.inputModeAt1C))
        {
        case 0:body.stateAt04=0;return 3;
        case 1:
            // 4A9C0B查询回调后的当前state，不用hit-test前缓存state决定分支。
            if(Read(body.stateAt04)==2){body.stateAt04=4;return 3;}
            body.stateAt04=0;return 4;
        case 2:body.stateAt04=4;return 3;
        default:return 0;
        }
    default:return 0;
    }
}
std::uint8_t DrawCgUiButtonNativeAt4B5290(CgUiButtonNativeBody6Ch& body,
    GraphicsSurfaceNativeBody* destination,const RECT* clip,CgUiButtonNativeServices& services)
{
    const auto state=Read(body.stateAt04);if(state<0)return std::uint8_t(state);
    const auto slot=StateSlot(body,state);if(slot<0)return std::uint8_t(slot);
    auto* source=ResolveCgUiButtonSurfaceNativeAt4ADED0(body,slot);if(!source)return 0;
    const auto dx=Sub(Read(source->originXAt10),Read(body.naturalAt30.x));
    const auto dy=Sub(Read(source->originYAt14),Read(body.naturalAt30.y));
    const auto* position=Read(body.useNaturalAt2C)?&body.naturalAt30:&body.positionAt24;
    const auto x=Add(dx,Read(position->x));const auto y=Add(dy,Read(position->y));
    const auto targetHandle=Read(destination->handleAt04);if(!targetHandle)return 0;
    const auto sourceHandle=Read(source->handleAt04);if(!sourceHandle)return 0;
    return std::uint8_t(services.graphics.SurfaceBlitRaw(targetHandle,sourceHandle,x,y,nullptr,nullptr,clip,0));
}
}

#include "MusicButtonNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
void SynchronizeMusicButtonNativeAt4ABB40(MusicButtonNativeBody70h& p)
{
    using namespace MusicButtonNativeDetail;auto& source=RequireMusicButtonNativeServices().source;
    auto* dirty=Dirty(p);Read(dirty->vptrAt00)->coalesceAt04(dirty);
    dirty=Dirty(p);const auto count=std::bit_cast<int>(Read(dirty->vptrAt00)->countAt18(dirty));
    if(!count)return;
    dirty=Dirty(p);auto* rects=Read(dirty->vptrAt00)->rectsAt1C(dirty);
    for(int n=0;n<count;++n)
    {
        auto* dialog=Dialog(p);Read(dialog->vptrAt00)->drawAt60(dialog,rects+n);
        for(int i=0;i<PublicCount(p);++i)
        {
            if(i==Read(p.common.skippedGuiAt40))continue;
            auto* button=PublicAt(p,static_cast<unsigned>(i));const auto state=Read(button->stateAt04);if(state<0)continue;
            // 原4ABC72按DWORD寻址，不给state添加0..6的自定钳位。
            const auto slot=Read(*reinterpret_cast<const int*>(reinterpret_cast<std::uintptr_t>(button)+0x40u+std::uint32_t(state)*4u));
            if(slot<0)continue;
            auto* surface=ResolveCgUiButtonSurfaceNativeAt4ADED0(*button,slot);if(!surface)continue;
            const auto x=Sub(Read(surface->originXAt10),Read(button->naturalAt30.x));
            const auto y=Sub(Read(surface->originYAt14),Read(button->naturalAt30.y));
            const auto position=Read(button->useNaturalAt2C)?button->naturalAt30:button->positionAt24;
            source.DrawSurfaceAt441980(surface,Add(x,position.x),Add(y,position.y),rects+n);
        }
        source.InvalidateRectAfter4182F0(rects+n);
    }
    // count为负数时原循环不执行，但仍进入Clear；不可改成count<=0直接返回。
    dirty=Dirty(p);Read(dirty->vptrAt00)->clearAt00(dirty);
}
}

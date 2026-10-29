#include "LogDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime::LogDialogNativeDetail
{
void __fastcall BeginDC(Common* p,void*)
{
    auto& b=*reinterpret_cast<DC*>(p);auto& s=RequireLogDialogNativeServices();
    Field<std::uint32_t>(Registry(2,0x50CCEA),0x1C)=2;
    Read(p->vptrAt00)->writeEnabledAt24(p,1);
    auto* root=Root(0x50CD4A);auto* draw=Resolve<void>(s.source.RootDrawSlotAt18(root));
    Fresh f;auto* returned=s.source.CaptureBackgroundAt4ACDD0(draw,&f.slot);RefLife life{f.slot};
    if(&b.backgroundAt90!=returned)
    {
        ReleaseYtlReference(b.backgroundAt90);
        CopyGuiClickedEventReference(b.backgroundAt90,*returned);
    }
    life.TerminateNormal(); // 50CCC0 state1登记541E55，不能让正常释放异常先展开外层。
    auto* scroll=Resolve<Common>(b.scrollAt84);Read(scroll->vptrAt00)->beginAt44(scroll);
    auto* text=Resolve<Common>(b.textAt78);Read(text->vptrAt00)->beginAt44(text);
    scroll=Resolve<Common>(b.scrollAt84);text=Resolve<Common>(b.textAt78);
    const auto position=ScrollPosition(scroll);UpdateRows(*reinterpret_cast<Text*>(text),position,0);
    b.phaseAt5C=0;
}
int __fastcall PollDC(Common* p,void*)
{
    auto& b=*reinterpret_cast<DC*>(p);
    switch(Read(b.phaseAt5C))
    {
    case 0:
    {auto* dirty=Read(p->vptrAt00)->dirtyAt00(p);Read(dirty->vptrAt00)->addRectAt08(dirty,&b.boundsAt68);b.phaseAt5C=3;break;}
    case 1:case 2:case 3:break;
    case 4:
    {
        Read(p->vptrAt00)->writeEnabledAt24(p,0);
        auto* c=Resolve<Common>(b.textAt78);Read(c->vptrAt00)->writeEnabledAt24(c,0);
        c=Resolve<Common>(b.scrollAt84);Read(c->vptrAt00)->writeEnabledAt24(c,0);
        auto* dirty=Read(p->vptrAt00)->dirtyAt00(p);Read(dirty->vptrAt00)->addRectAt08(dirty,&b.boundsAt68);
        b.phaseAt5C=-1;Read(p->vptrAt00)->flushDirtyAt58(p);return 1;
    }
    default:return 0;
    }
    Read(p->vptrAt00)->processInputAt54(p);Read(p->vptrAt00)->flushDirtyAt58(p);return 1;
}
int __fastcall EndDC(Common* p,void*)
{Read(p->vptrAt00)->writeEnabledAt24(p,0);Field<std::uint32_t>(Registry(2,0x508DF8),0x1C)=1;return 1;}
void __fastcall InputDC(Common* p,void*)
{
    auto& b=*reinterpret_cast<DC*>(p);auto& s=RequireLogDialogNativeServices().source;
    auto* input=s.CaptureInputAt418210(0x519569);auto* keys=Resolve<KeyboardManagerNativeBody438h>(input->keyboardAt18);
    auto* at=Read(keys->actionsAt14[0].begin);auto* end=Read(keys->actionsAt14[0].end);
    if(at!=end)
    {
        const auto buffer=Read(keys->bufferAt10);
        for(;at!=end;++at)
        {
            const auto address=reinterpret_cast<std::uintptr_t>(at);
            if(*reinterpret_cast<volatile std::uint8_t*>(address+buffer+1u)&&
                !*reinterpret_cast<volatile std::uint8_t*>(address-buffer+2u)){b.phaseAt5C=4;return;}
        }
    }
    input=s.CaptureInputAt418210(0x519662);auto* mouse=Resolve<MouseManagerNativeBody3Ch>(input->mouseAt00);
    if(!Read(mouse->previousAt30[1])&&Read(mouse->pressedAt10[1])&&!Read(mouse->suppressionAt28)){b.phaseAt5C=4;return;}
    auto* c=Resolve<Common>(b.textAt78);Read(c->vptrAt00)->pollAt48(c);
    c=Resolve<Common>(b.scrollAt84);Read(c->vptrAt00)->pollAt48(c);
    c=Resolve<Common>(b.scrollAt84);auto* text=Resolve<Text>(b.textAt78);
    const auto position=ScrollPosition(c);UpdateRows(*text,position,1);
}
void __fastcall FlushDC(Common* p,void*)
{
    auto& b=*reinterpret_cast<DC*>(p);auto& s=RequireLogDialogNativeServices();
    auto* dirty=Read(p->vptrAt00)->dirtyAt00(p);Read(dirty->vptrAt00)->coalesceAt04(dirty);
    dirty=Read(p->vptrAt00)->dirtyAt00(p);const auto count=std::bit_cast<int>(Read(dirty->vptrAt00)->countAt18(dirty));
    if(!count)return;
    dirty=Read(p->vptrAt00)->dirtyAt00(p);auto* rectangles=Read(dirty->vptrAt00)->rectsAt1C(dirty);
    for(int i=0;i<count;++i)
    {
        const auto handle=Read(Resolve<GraphicsSurfaceNativeBody>(b.backgroundAt90)->handleAt04);
        if(handle)static_cast<void>(s.drawing.SurfaceDrawRaw(handle,0,0,nullptr,nullptr,rectangles+i,0));
        Read(p->vptrAt00)->drawAt60(p,rectangles+i);
        auto* c=Resolve<Common>(b.textAt78);Read(c->vptrAt00)->drawAt60(c,rectangles+i);
        c=Resolve<Common>(b.scrollAt84);Read(c->vptrAt00)->drawAt60(c,rectangles+i);
        s.screen.InvalidateRectAt52603A(rectangles+i);
    }
    dirty=Read(p->vptrAt00)->dirtyAt00(p);Read(dirty->vptrAt00)->clearAt00(dirty);
}
}

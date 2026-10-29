#include <Runtime/TextDisplayNative.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/GraphicsSurfaceReference.h>
#include <Runtime/DrawTransitionView.h>
#include <Runtime/DrawManagerNative.h>

#include <bit>
#include <cstdlib>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=TextDisplayNativeBody90h;
using Services=TextDisplayNativeServices;
thread_local Services* current;
Services& Bound(){if(!current)throw std::logic_error("TextDisplay native services are absent");return *current;}
std::int32_t Add(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
std::int32_t Sub(std::int32_t a,std::int32_t b) noexcept
{return std::bit_cast<std::int32_t>(std::uint32_t(a)-std::uint32_t(b));}
GraphicsSurfaceNativeBody* Surface(const YtlReferenceSlotRuntime& ref) noexcept
{return ResolveEnvironmentObjectReference<GraphicsSurfaceNativeBody>(ref);}
void ReleaseMembers(Body& b) noexcept
{
    // 4A4AA5..4A4B86及构造展开：text、mask、composite、base；不补写bias。
    ReleaseYtlReference(b.textAt74);ReleaseYtlReference(b.maskAt5C);
    ReleaseYtlReference(b.compositeAt4C);ReleaseYtlReference(b.baseAt40);
}
std::uint8_t __fastcall Run(Body* self,void*,std::int32_t arg)
{return RunTextDisplayNativeAt4C3630(*self,arg,Bound());}
void __fastcall Update(Body* self,void*){UpdateTextDisplayNativeAt4BA940(*self,Bound());}
void __fastcall Draw(Body* self,void*){DrawTextDisplayNativeAt4B5BF0(*self,Bound());}
Body* __fastcall Delete(Body* self,void*,std::uint32_t flags)
{return DeleteTextDisplayNativeAt4A71B0(*self,flags);}
std::uint8_t __fastcall PureRun(Body*,void*,std::int32_t){_purecall();std::terminate();}
Body* __fastcall DeleteBase(Body* self,void*,std::uint32_t flags);
struct RetiredTaskVtable final
{
    decltype(TextDisplayNativeVtable::runAt00) run;
    decltype(TextDisplayNativeVtable::deleteAt04) destroy;
};
static_assert(sizeof(RetiredTaskVtable)==8);
const RetiredTaskVtable baseMethods{
    reinterpret_cast<decltype(RetiredTaskVtable::run)>(&PureRun),
    reinterpret_cast<decltype(RetiredTaskVtable::destroy)>(&DeleteBase)};
Body* __fastcall DeleteBase(Body* self,void*,std::uint32_t flags)
{self->vptrAt00=reinterpret_cast<const TextDisplayNativeVtable*>(&baseMethods);
    if(flags&1u)::operator delete(self,std::size_t{4});return self;}
const TextDisplayNativeVtable methods{
    reinterpret_cast<decltype(TextDisplayNativeVtable::runAt00)>(&Run),
    reinterpret_cast<decltype(TextDisplayNativeVtable::deleteAt04)>(&Delete),
    reinterpret_cast<decltype(TextDisplayNativeVtable::updateAt08)>(&Update),
    reinterpret_cast<decltype(TextDisplayNativeVtable::drawAt0C)>(&Draw)};
void Commit(Services& s)
{
    // 4B03C0先Graphics尺寸，再独立捕获根并向全局LayerList+20提交。
    const auto size=s.graphics.ReadScreenSizeAt47CC00();
    const RECT full{0,0,size.cx,size.cy};
    s.layers.CaptureDrawLayersAt413BE0().RegisteredLayers().AddDirtyRect(&full);
}
void Render(Body& b,Services& s)
{
    const auto selected=s.layers.CaptureDrawLayersAt413BE0();
    auto* manager=static_cast<DrawManagerNativeBody2Ch*>(const_cast<void*>(selected.Receiver()));
    static_cast<void>(DrawTransitionView(manager,&s.graphics).RenderTransitionAt4C39E0(b.operationAt0C));
}
void Animate(Body& b,Services& s)
{
    auto& op=b.operationAt0C;
    if(op.opcode==1)
    {
        if(s.source.AccelerateAt4AA970())op.current=op.target;
        if(op.current<op.target)b.phaseAt04=3;
        Render(b,s);op.current=Sub(op.current,b.stepAt3C);
        s.graphics.InvalidatedRectIfLoaded(nullptr);return;
    }
    if(s.source.AccelerateAt4AA970())
    {op.current=Add(op.current,256);if(op.current>=op.target)op.current=op.target;}
    if(op.opcode==-1||op.current>=op.target)b.phaseAt04=3;
    Render(b,s);op.current=Add(op.current,b.stepAt3C);
    if(op.current>=op.target)op.current=op.target;
    s.graphics.InvalidatedRectIfLoaded(nullptr);
}
void Wait(Body& b,Services& s)
{
    bool next=s.source.ReadAdvanceAt4B0550();
    if(!next)next=s.source.ReadNegativeWheelAt4AD740();
    if(!next)next=s.source.ReadHeldSixAt4198D0();
    if(!next)next=s.source.ReadFlagAt14Bit1();
    if(!next&&s.source.ReadFlagAt18Bit1())
    {if(b.waitAt70){--b.waitAt70;return;}next=true;}
    if(!next)return;
    b.phaseAt04=5;b.overlayAt6D=0;Commit(s);
}
}
TextDisplayNativeScope::TextDisplayNativeScope(Services& s) noexcept:previous_(current),surfaces_(s.graphics){current=&s;}
TextDisplayNativeScope::~TextDisplayNativeScope() noexcept{current=previous_;}
const TextDisplayNativeVtable& TextDisplayNativeMethods() noexcept{return methods;}

Body* ConstructTextDisplayNativeAt4A1F30(Body& b,Services& s)
{
    TextDisplayNativeScope scope(s);
    b.vptrAt00=&methods;::new(&b.operationAt0C) TransitOperation;
    ::new(&b.baseAt40) YtlReferenceSlotRuntime{};::new(&b.compositeAt4C) YtlReferenceSlotRuntime{};
    ::new(&b.maskAt5C) YtlReferenceSlotRuntime{};::new(&b.textAt74) YtlReferenceSlotRuntime{};b.dirtyAt80={};
    try
    {
        const auto size=s.graphics.ReadScreenSizeAt47CC00();
        AdoptGraphicsSurfaceAt407A80(b.textAt74,NewGraphicsSurfaceAt40C2E0(s.graphics,s.strings));
        if(!Surface(b.textAt74)->CreateAt415010(s.graphics,size.cx,size.cy,1,1))throw 1;
        if(!s.source.CreateFontAt4A2113(s.source.ReadNamedAt4C73A0("text_context")))throw 1;
        struct MessageGroup final
        {
            YtlReferenceSlotRuntime name{},voices{},body{};
            ~MessageGroup() noexcept
            {ReleaseYtlReference(body);ReleaseYtlReference(voices);ReleaseYtlReference(name);}
        } group;
        ClearMessageFormatRawAt4C5850({group.name,group.voices,group.body});
        const auto messageIndex=s.source.ReadNamedAt4C73A0("text_string");
        switch(s.source.InitializeMessageAt4A2239(messageIndex,{group.name,group.voices,group.body}))
        {
        case TextDisplayTextInitializationResult::ParseFailed:throw 2;
        case TextDisplayTextInitializationResult::GraphicsInitializationFailed:throw 3;
        case TextDisplayTextInitializationResult::Success:break;
        }
        auto x=s.source.ReadNamedAt4C73A0("text_pos_x");
        auto y=s.source.ReadNamedAt4C73A0("text_pos_y");
        if(s.source.ReadNamedAt4C73A0("text_pos_center"))
        {
            const auto r=s.graphics.ReadTextDrawRect();
            x=Sub(x,Sub(r.right,r.left)/2);y=Sub(y,Sub(r.bottom,r.top)/2);
        }
        s.graphics.SetTextBasePosition(x,y);
        if(!s.graphics.BlitAllTextToSurface(Surface(b.textAt74)->handleAt04,nullptr))throw 3;
        b.overlayAt6D=0;
        const auto effect=s.source.ReadNamedAt4C73A0("text_effect");
        b.skipAt58=0;
        if(effect==-2)b.skipAt58=1;
        else if(effect>=0&&!s.source.LoadMaskAt4A26E4(b.maskAt5C,effect))throw 4;
        b.savedModeAt08=s.source.ReadModeAt04();s.source.WriteModeAt04(0);
        b.drawAt6C=1;b.phaseAt04=0;b.waitAt70=0;
        if(!s.source.SuppressAt4C3430()&&s.source.ReadFlagAt18Bit1())
        {
            const auto length=s.graphics.GetTextLength();
            const auto minimum=length<30?30:length;
            b.waitAt70=std::uint32_t(minimum)*s.source.ReadPaceAt3C();
        }
        return &b;
    }
    catch(...){ReleaseMembers(b);throw;}
}
Body* NewTextDisplayNativeAt4AB7EE(Services& s,IEnvironmentRootMemory& memory)
{
    auto* raw=memory.AllocateBody(0x90);if(!raw)return nullptr;
    try{return ConstructTextDisplayNativeAt4A1F30(*::new(raw) Body,s);}
    catch(...){[&]() noexcept{memory.DeleteBody(raw,0x90);}();throw;}
}
void DestroyTextDisplayNativeAt4A4A80(Body& b) noexcept
{ReleaseMembers(b);b.vptrAt00=reinterpret_cast<const TextDisplayNativeVtable*>(&baseMethods);}
Body* DeleteTextDisplayNativeAt4A71B0(Body& b,std::uint32_t flags) noexcept
{DestroyTextDisplayNativeAt4A4A80(b);if(flags&1u)::operator delete(&b,std::size_t{0x90});return &b;}

void UpdateTextDisplayNativeAt4BA940(Body& b,Services& s)
{
    TextDisplayNativeScope scope(s);
    switch(b.phaseAt04)
    {
    case 0:
        static_cast<void>(s.layers.CaptureDrawLayersAt413BE0());
        static_cast<void>(PrepareDrawLayersViewAt46A980(s.layers));++b.phaseAt04;return;
    case 1:
        if(b.skipAt58||s.source.SuppressAt4C3430())
        {b.overlayAt6D=1;b.phaseAt04=4;Commit(s);return;}
        static_cast<void>(s.layers.CaptureDrawLayersAt413BE0());
        {
            YtlReferenceSlotRuntime temporary;
            CaptureGraphicsScreenAt4ACDD0(temporary,s.graphics,s.graphics,s.graphics);
            AssignGraphicsSurfaceReferenceAt48CA30(b.baseAt40,temporary);
        }
        b.operationAt0C=TransitOperation{};
        if(b.maskAt5C.baseAt00)
        {
            const auto selected=s.layers.CaptureDrawLayersAt413BE0();
            YtlReferenceSlotRuntime temporary;
            CompositeGraphicsFrameAt4A8340(temporary,s.graphics,s.graphics,selected,1);
            AssignGraphicsSurfaceReferenceAt48CA30(b.compositeAt4C,temporary);
            b.operationAt0C.opcode=1;b.operationAt0C.mask=Surface(b.maskAt5C)->handleAt04;
            b.operationAt0C.current=255;b.operationAt0C.range=128;b.operationAt0C.target=-128;
        }
        else
        {
            YtlReferenceSlotRuntime temporary;
            CloneGraphicsSurfaceAt444470(temporary,*Surface(b.baseAt40),1,s.graphics,s.graphics,s.strings);
            AssignGraphicsSurfaceReferenceAt48CA30(b.compositeAt4C,temporary);b.operationAt0C.opcode=0;
        }
        static_cast<void>(s.graphics.SurfaceBlitRaw(Surface(b.compositeAt4C)->handleAt04,Surface(b.textAt74)->handleAt04,
            0,0,nullptr,nullptr,nullptr,0));
        b.operationAt0C.source=Surface(b.compositeAt4C);
        b.operationAt0C.previous=Surface(b.baseAt40)->handleAt04;
        b.stepAt3C=s.source.ReadStepAt497900();b.phaseAt04=2;b.drawAt6C=0;
        Animate(b,s);return;
    case 2:Animate(b,s);return;
    case 3:
        b.drawAt6C=1;
        // 4BAC76/7E/86只释放三个Ref；operation裸指针/句柄保持原位模式。
        ReleaseYtlReference(b.baseAt40);ReleaseYtlReference(b.compositeAt4C);ReleaseYtlReference(b.maskAt5C);
        ++b.phaseAt04;b.overlayAt6D=1;
        [[fallthrough]];
    case 4:Wait(b,s);return;
    case 5:s.source.WriteModeAt04(b.savedModeAt08);s.source.AdvanceSceneAt4C1760();return;
    default:return;
    }
}
void DrawTextDisplayNativeAt4B5BF0(Body& b,Services& s)
{
    if(!b.drawAt6C)return;
    TextDisplayNativeScope scope(s);
    if(!s.source.SuppressFrameAt4C3350())s.layers.CaptureDrawLayersAt413BE0().AdvanceFramesAt46B240();
    s.layers.CaptureDrawLayersAt413BE0().WindowZero().PropagateDirtyRectsAt4C4250();
    ::SetRectEmpty(&b.dirtyAt80);
    const auto count=s.source.CaptureDirtyCountAt4B5CF9();
    if(count!=0)
    {
        const auto* rect=s.source.CaptureDirtyDataAt4B5D5C();
        for(std::int32_t i=0;i<count;++i,++rect)::UnionRect(&b.dirtyAt80,&b.dirtyAt80,rect);
    }
    s.layers.CaptureDrawLayersAt413BE0().RegisteredLayers().RenderDirtyRects(s.graphics);
    if(b.overlayAt6D&&!::IsRectEmpty(&b.dirtyAt80))
        static_cast<void>(s.graphics.SurfaceDrawRaw(Surface(b.textAt74)->handleAt04,0,0,nullptr,nullptr,&b.dirtyAt80,0));
}
std::uint8_t RunTextDisplayNativeAt4C3630(Body& b,std::int32_t,Services& s)
{
    TextDisplayNativeScope scope(s);
    b.vptrAt00->updateAt08(&b);
    // 4C3638在Update之后重新取当前表；只承诺AL=1，不扩成完整EAX=1。
    b.vptrAt00->drawAt0C(&b);return 1;
}
}

#include "DemoSequenceNative.Internal.h"
#include <Runtime/ParamsDemoLookupRuntime.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
GraphicsSurfaceNativeBody* NewSurface(DemoSequenceNativeServices& services)
{
    auto* raw=services.memory.AllocateBody(0x24);
    if (!raw) return nullptr;
    try { return ::new(raw) GraphicsSurfaceNativeBody(services.surfaces,services.strings); }
    catch (...) { services.memory.DeleteBody(raw,0x24);throw; }
}
void CreateSurface(YtlReferenceSlotRuntime& slot,int alpha,DemoSequenceNativeServices& services)
{
    AdoptGraphicsSurfaceAt407A80(slot,NewSurface(services));
    // 49B87D/888等：先高后宽，两次取源；不能与后两张画布共用SIZE。
    const auto height=services.source.CaptureEnvironmentAt4180C0()->heightAt1C;
    const auto width=services.source.CaptureEnvironmentAt4180C0()->widthAt18;
    if (!DemoNativeDetail::Surface(slot)->CreateAt415010(services.surfaces,
        DemoNativeDetail::Signed(width),DemoNativeDetail::Signed(height),alpha,1)) throw 1;
}
}
DemoSequenceNativeBodyA4h* ConstructDemoSequenceNativeAt49B6F0(DemoSequenceNativeBodyA4h& body,
    DemoSequenceNativeServices& services)
{
    GraphicsSurfaceNativeScope scope(services.surfaces);
    body.canvasAt08={};body.maskAt14={};body.previousAt20={};body.alphaAt2C={};
    body.spritesAt38={};body.dirtyAt44={};body.boundsAt50={};
    // 49B7C1..817独立写原包；不写step、index或ready尾三byte。
    auto& packet=body.operationAt60;
    packet.destination=0;packet.previous=0;packet.source=nullptr;packet.mask=0;
    packet.y=0;packet.x=0;packet.clip=nullptr;packet.opcode=-1;
    packet.targetKind=0;packet.current=0;packet.target=255;packet.range=0;
    body.readyAt90=0;body.dataAt98={};body.waitAt00=0;
    try
    {
        CreateSurface(body.canvasAt08,0,services);
        CreateSurface(body.previousAt20,0,services);
        CreateSurface(body.alphaAt2C,1,services);
        auto* raw=services.memory.AllocateBody(0x80);
        auto* dirty=raw?::new(raw) GuiDirtyRectNativeBody80h:nullptr;
        AdoptGuiDirtyRectReferenceAt498860(body.dirtyAt44,dirty,services.memory);
        // 原state11只覆盖sprite range退役；抛出不能进入普通catch-all。
        try
        {
            auto* at=body.spritesAt38.begin;
            auto* const end=body.spritesAt38.end;
            while (at!=end) {ReleaseYtlReference(*at);++at;}
            body.spritesAt38.end=body.spritesAt38.begin;
        }
        catch (...) { std::terminate(); }
        body.readyAt90=1;
        const auto height=services.source.CaptureEnvironmentAt4180C0()->heightAt1C;
        const auto width=services.source.CaptureEnvironmentAt4180C0()->widthAt18;
        body.boundsAt50={0,0,DemoNativeDetail::Signed(width),DemoNativeDetail::Signed(height)};
    }
    // 原PE5EF798：仅state7..11这段内部catch-all保留已发布Ref，返回当时ready。
    catch (...) {}
    return &body;
}
DemoSequenceNativeBodyA4h* NewDemoSequenceNativeAt49B624(DemoSequenceNativeServices& services)
{
    auto* raw=services.memory.AllocateBody(0xA4);
    if (!raw) return nullptr;
    try { return ConstructDemoSequenceNativeAt49B6F0(*::new(raw) DemoSequenceNativeBodyA4h,services); }
    catch (...) { services.memory.DeleteBody(raw,0xA4);throw; }
}
void DestroyDemoSequenceNativeAt4A3CA0(DemoSequenceNativeBodyA4h& body,DemoSequenceNativeServices& services) noexcept
{
    GraphicsSurfaceNativeScope scope(services.surfaces);
    ReleaseYtlReference(body.dataAt98);
    ReleaseYtlReference(body.dirtyAt44);
    DestroyDemoSpriteReferenceVectorAt4615F0(body.spritesAt38,services.memory);
    ReleaseYtlReference(body.alphaAt2C);ReleaseYtlReference(body.previousAt20);
    ReleaseYtlReference(body.maskAt14);ReleaseYtlReference(body.canvasAt08);
}
void DeleteDemoSequenceNativeAt4C5FB0(DemoSequenceNativeBodyA4h* body,DemoSequenceNativeServices& services) noexcept
{
    if (!body) return;
    DestroyDemoSequenceNativeAt4A3CA0(*body,services);
    body->~DemoSequenceNativeBodyA4h();
    ::operator delete(body,std::size_t{0xA4});
}
void BindDemoSequenceNativeAt4C2220(DemoSequenceNativeBodyA4h& body,const YtlReferenceSlotRuntime& source)
{
    if (&body.dataAt98!=&source)
    {
        ReleaseParamsDemoDataAt43B7D0(body.dataAt98);
        body.dataAt98.baseAt00=source.baseAt00;
        body.dataAt98.biasAt04=source.biasAt04;
        body.dataAt98.controlAt08=source.controlAt08;
        if (auto* control=body.dataAt98.controlAt08) ++control->referenceCountAt04;
    }
    body.indexAt94=0;
}
}

#include <Runtime/CgOperationWindowNative.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
using Body=CgOperationWindowNativeBody30h;
Body* ConstructCgOperationWindowNativeAt499C10(Body& body,LayerListNativeBody18h* parent,
    CgOperationWindowNativeServices& services)
{
    CgOperationDialogNativeScope scope(services.dialog);
    ConstructLayerItemNativeAt441270(body.window.layer,parent);
    body.window.boundsAt14={};body.window.layer.vptrAt00=CgOperationWindowNativeMethods().frame.AsBase();
    ::new(&body.dialogAt24) YtlReferenceSlotRuntime{};body.window.layer.enabledAt0C=0;
    try
    {
        void* raw=::operator new(std::size_t{0xA8});
        GuiDialogNativeBody5Ch* child=nullptr;
        try{if(raw)child=ConstructCgOperationDialogNativeAt4DB3D0(raw);}
        catch(...)
        {
            struct RawUnwind{void* raw;~RawUnwind() noexcept{::operator delete(raw,std::size_t{0xA8});}} cleanup{raw};
            throw;
        }
        AdoptCgOperationDialogNativeAt499CBA(body.dialogAt24,reinterpret_cast<CgOperationDialogNativeBodyA8h*>(child));
        body.window.boundsAt14=ResolveCgOperationDialogNative(body.dialogAt24)->boundsAt68;
    }
    catch(...)
    {
        // 5EEBE8 try2..3/catch4：仅清RECT，返回部分对象；Ref/base不回滚。
        body.window.boundsAt14={};
    }
    return &body;
}
void DestroyCgOperationWindowNative(Body& body) noexcept
{
    ReleaseYtlReference(body.dialogAt24);
    DestroyLayerWindowNativeAt464910(body.window);
}
Body* DeleteCgOperationWindowNativeAt4A60D0(Body& body,std::uint32_t flags) noexcept
{
    DestroyCgOperationWindowNative(body);auto* result=&body;
    if(flags&1u)::operator delete(result,std::size_t{0x30});
    return result;
}
}

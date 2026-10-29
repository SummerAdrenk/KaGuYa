#include "PropertyPageTextNative.Internal.h"
#include <Runtime/ArchiveReaderFactoryRuntime.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace PropertyPageTextDetail;
namespace
{
struct Dib;
struct DibTable final{Dib*(__thiscall* deleteAt00)(Dib*,DWORD);};
// CDIBSurface不是CSurface：+08是DC，+0C是HBITMAP，+1C只有一个byte。
struct Dib final
{
    const DibTable* vptrAt00;int handleAt04;HDC dcAt08;HBITMAP bitmapAt0C;
    int widthAt10,heightAt14,bitsAt18;BYTE clearAt1C,paddingAt1D[3];DWORD colorAt20;
};
static_assert(sizeof(Dib)==0x24&&offsetof(Dib,dcAt08)==8&&offsetof(Dib,colorAt20)==0x20);
void ResetDib(Dib& b,PropertyPageTextNativeServices& s)
{
    const int handle=Read(b.handleAt04);
    if(handle)
    {
        bool present;auto address=GraphicsExport("DIBitmapRelease",15,s,present);
        if(present)reinterpret_cast<void(__cdecl*)(int)>(address)(handle);
    }
    b.handleAt04=0;b.dcAt08=nullptr;b.bitmapAt0C=nullptr;b.widthAt10=0;b.heightAt14=0;
    b.bitsAt18=0;b.clearAt1C=0;b.colorAt20=0;
}
Dib* __fastcall DeleteDib(Dib* b,void*,DWORD flags)noexcept;
const DibTable dibTable{reinterpret_cast<decltype(DibTable::deleteAt00)>(&DeleteDib)};
Dib* __fastcall DeleteDib(Dib* b,void*,DWORD flags)noexcept
{
    // 411BD0 state0/5DF52C flags5：Reset、名称叶或sized delete异常直接terminate。
    b->vptrAt00=&dibTable;ResetDib(*b,RequirePropertyPageTextNativeServices());
    if(flags&1)::operator delete(b,std::size_t{0x24});return b;
}
template<class T>class Control final:public EnvironmentObjectReferenceControl
{
public:
    explicit Control(T* b)noexcept:EnvironmentObjectReferenceControl(b,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00()override
    {
        auto* b=static_cast<T*>(payloadAt14);if(!b)return;
        if constexpr(std::is_same_v<T,Dib>)b->vptrAt00->deleteAt00(b,1);
        else b->vptrAt00->deletingDestructor(b,1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags)override{RetireEnvironmentObjectControl(this,flags);}
};
static_assert(sizeof(Control<Dib>)==0x18&&sizeof(Control<GraphicsSurfaceNativeBody>)==0x18);
template<class T>void Adopt(YtlReferenceSlotRuntime& slot,T* b)
{
    ReleaseImageReference(slot);if(!b)return;
    // 407ABB/414A45：释放完成后的ECX=0，不给回调晚填control新增删除资格。
    slot.biasAt04=0;slot.baseAt00=reinterpret_cast<DWORD>(b);
    void* raw=::operator new(0x18);auto* c=raw?::new(raw) Control<T>(b):nullptr;slot.controlAt08=c;
    // 原18h分配返回null后的+04写入保留为故障，不回收已发布24h正文。
    c->referenceCountAt04=1;slot.controlAt08->owningAt08=1;
    Field<DWORD>(slot.controlAt08,0x0C)=0x24;Field<DWORD>(slot.controlAt08,0x10)=1;
}
Dib* NewDib(PropertyPageTextNativeServices& s)
{
    void* raw=::operator new(0x24);if(!raw)return nullptr;
    auto* b=::new(raw) Dib;b->vptrAt00=&dibTable;b->handleAt04=0;b->widthAt10=0;b->heightAt14=0;
    try{ResetDib(*b,s);return b;}
    catch(...)
    {
        struct Cleanup final{void* p;~Cleanup()noexcept{::operator delete(p,std::size_t{0x24});}}cleanup{raw};
        throw;
    }
}
bool CreateDib(Dib& b,int width,int height,PropertyPageTextNativeServices& s)
{
    ResetDib(b,s);bool present;auto address=GraphicsExport("DIBitmapCreate",14,s,present);
    const int handle=present?reinterpret_cast<int(__cdecl*)(int,int,int,int)>(address)(width,height,32,0):0;
    b.handleAt04=handle;if(!handle)return false;
    address=GraphicsExport("GetDIBitmapDC",13,s,present);
    b.dcAt08=present?reinterpret_cast<HDC(__cdecl*)(int)>(address)(handle):nullptr;
    const int currentHandle=Read(b.handleAt04);
    address=GraphicsExport("GetDIBitmapHBITMAP",18,s,present);
    b.bitmapAt0C=present?reinterpret_cast<HBITMAP(__cdecl*)(int)>(address)(currentHandle):nullptr;
    b.widthAt10=width;b.heightAt14=height;b.bitsAt18=32;b.clearAt1C=0;return true;
}
struct Resource final
{
    PropertyPageTextNativeServices& s;HGLOBAL data=nullptr;DWORD size=0;const void* bytes=nullptr;bool armed=false;
    explicit Resource(PropertyPageTextNativeServices& services):s(services){}
    HMODULE Module() const noexcept
    {
        return s.nativeBitmapResourceModuleSlot ?
            *static_cast<const volatile HMODULE*>(s.nativeBitmapResourceModuleSlot) : s.bitmapResourceModule;
    }
    void Load()
    {
        // 三处原PE均FF359CA66100：各自重读61A69C，不缓存初值或改用61A6A0。
        const HRSRC info=s.win32.findResource(Module(),MAKEINTRESOURCEA(0xCC),MAKEINTRESOURCEA(2));
        data=s.win32.loadResource(Module(),info);
        if(data)
        {
            size=s.win32.sizeofResource(Module(),info);bytes=s.win32.lockResource(data);
            if(!bytes){s.win32.freeResource(data);data=nullptr;size=0;bytes=nullptr;}
        }
        armed=true; // 只有414702之后才取得CUserRes展开资格。
    }
    void Release(){if(data)s.win32.freeResource(data);armed=false;}
    ~Resource()noexcept{if(armed&&data)s.win32.freeResource(data);}
};
struct Reader final
{
    union{ArchiveReaderReference value;};bool armed=false;
    Reader()noexcept{}~Reader()noexcept{if(armed)value.~ArchiveReaderReference();}
};
void ReleaseCached(YtlReferenceControlRuntime* c)noexcept
{
    // 414959/41497C：正常尾使用ESI/EBX缓存，不在payload回调后重选局部slot。
    if(!c||--c->referenceCountAt04!=0)return;
    if(c->owningAt08)c->DestroyPayloadAtVslot00();
    c->DeleteControlAtVslot04(1);
}
struct Buffer final
{
    union{ByteArrayReferenceRuntime value;};bool armed=false;
    Buffer(){::new(&value) ByteArrayReferenceRuntime;armed=true;}
    ~Buffer()noexcept{if(armed)value.~ByteArrayReferenceRuntime();}
};
}
void InitializePropertyPageImageStaticNativeAt414660(PropertyPageImageStaticNativeBody48h& b,PropertyPageTextNativeServices& s)
{
    const PropertyPageTextNativeScope scope(s);Resource resource(s);resource.Load();
    if(resource.data)
    {
        Buffer buffer;buffer.value.AllocateAt412700(resource.size+14u);
        auto* bytes=buffer.value.Data();auto* bufferControl=Read(buffer.value.RawSlot().controlAt08);
        const WORD magic=0x4D42;const DWORD length=bufferControl?Read(Field<DWORD>(bufferControl,0x10)):0,zero=0,offset=54;
        std::memcpy(bytes,&magic,2);std::memcpy(bytes+2,&length,4);std::memcpy(bytes+6,&zero,4);std::memcpy(bytes+10,&offset,4);
        std::memmove(buffer.value.DataAt(14),resource.bytes,resource.size);
        void* raw=::operator new(0x14);ArchiveFileReaderRuntime* reader=nullptr;
        if(raw)
        {
            // 内联Ctor没有外块删除/基类展开资格；Clear失败不能用普通reader Ctor补回收。
            reader=::new(raw) ArchiveFileReaderRuntime(ArchiveFileReaderRuntime::InlineBeforeClearTag{},s.strings);
            reader->ClearAt4327B0();SetRuntimeCStringAt437680(reader->nameAt10,"IDB_BITMAP3",11);
            reader->bytesAt04=buffer.value;static_cast<void>(reader->ReadyAt419360());
        }
        ArchiveReaderReferenceSlot slot{};ArchiveReaderReferenceControl* readerControl=nullptr;
        if(reader)
        {
            slot.baseAt00=reinterpret_cast<DWORD>(reader);slot.biasAt04=0;
            void* controlRaw=::operator new(0x18);auto* c=ConstructArchiveReaderControl(controlRaw,reader);readerControl=c;
            c->referenceCountAt04=1;c->owningAt08=1;c->strideAt0C=0x14;slot.controlAt08=c;c->extentAt10=1;
        }
        Reader held;::new(&held.value) ArchiveReaderReference(ArchiveReaderReference::Adopt(slot));held.armed=true;
        Adopt(b.backgroundAt24,NewGraphicsSurfaceAt40C2E0(s.surfaces,s.strings));
        auto* background=Resolve<GraphicsSurfaceNativeBody>(b.backgroundAt24);
        if(!LoadGraphicsSurfaceArchiveReaderAt419D70(*background,held.value,s.surfaces,0))ReleaseImageReference(b.backgroundAt24);
        // 414955 state4与414978 state5均terminate；正常释放不得进入前一层重试。
        held.armed=false;ReleaseCached(readerControl);
        buffer.armed=false;ReleaseCached(bufferControl);
    }
    GraphicsSurfaceNativeBody* background;
    if(ResolvePresent(b.backgroundAt24,background))
    {
        const int width=Read(background->widthAt08),height=Read(background->heightAt0C);
        Adopt(b.dibAt3C,NewDib(s));
        if(!CreateDib(*Resolve<Dib>(b.dibAt3C),width,height,s))ReleaseImageReference(b.dibAt3C);
        else
        {
            Resolve<Dib>(b.dibAt3C)->colorAt20=0;
            Adopt(b.colorSurfaceAt30,NewGraphicsSurfaceAt40C2E0(s.surfaces,s.strings));
            if(!Resolve<GraphicsSurfaceNativeBody>(b.colorSurfaceAt30)->CreateAt415010(s.surfaces,
                std::bit_cast<int>(DWORD(width)-16u),std::bit_cast<int>(DWORD(height)-8u),1,1))
                ReleaseImageReference(b.colorSurfaceAt30);
        }
    }
    resource.Release();
}
void UpdatePropertyPageImageStaticNativeAt438D60(PropertyPageImageStaticNativeBody48h& b,DWORD color,PropertyPageTextNativeServices& s)
{
    Dib* dib;if(!ResolvePresent(b.dibAt3C,dib))return;
    const int handle=Read(dib->handleAt04);
    if(handle)
    {
        const DWORD clear=Read(dib->colorAt20);bool present;auto address=GraphicsExport("DIBitmapClear",13,s,present);
        if(present)reinterpret_cast<void(__cdecl*)(int,DWORD,const RECT*)>(address)(handle,clear,nullptr);
    }
    GraphicsSurfaceNativeBody* background;
    if(ResolvePresent(b.backgroundAt24,background))
    {
        const int destination=Read(Resolve<Dib>(b.dibAt3C)->handleAt04);
        if(destination)
        {
            const int source=Read(background->handleAt04);
            if(source)
            {
                bool present;auto address=GraphicsExport("DIBitmapCopy",12,s,present);
                if(present)reinterpret_cast<void(__cdecl*)(int,int,int,int,const SIZE*,const RECT*,const RECT*,int)>(address)
                    (destination,source,0,0,nullptr,nullptr,nullptr,0);
            }
        }
    }
    GraphicsSurfaceNativeBody* surface;
    if(ResolvePresent(b.colorSurfaceAt30,surface))
    {
        surface->clearColorAt20=color;ClearSurface(Resolve<GraphicsSurfaceNativeBody>(b.colorSurfaceAt30),s);
        surface=Resolve<GraphicsSurfaceNativeBody>(b.colorSurfaceAt30);
        const int destination=Read(Resolve<Dib>(b.dibAt3C)->handleAt04);
        if(destination)
        {
            const int source=Read(surface->handleAt04);
            if(source)
            {
                bool present;auto address=GraphicsExport("DIBitmapDraw",12,s,present);
                if(present)reinterpret_cast<void(__cdecl*)(int,int,int,int,const SIZE*,const RECT*,const RECT*,int)>(address)
                    (destination,source,8,4,nullptr,nullptr,nullptr,0);
            }
        }
    }
}
}
namespace KaGuYa::Start::Runtime::PropertyPageTextDetail
{
void ClearSurface(GraphicsSurfaceNativeBody* b,PropertyPageTextNativeServices& s)
{
    const int handle=Read(b->handleAt04);if(!handle)return;const DWORD color=Read(b->clearColorAt20);
    bool present;auto address=GraphicsExport("SurfaceClear",12,s,present);
    if(present)reinterpret_cast<void(__cdecl*)(int,DWORD,const RECT*)>(address)(handle,color,nullptr);
}
}

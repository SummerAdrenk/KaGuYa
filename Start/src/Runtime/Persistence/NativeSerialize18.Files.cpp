#include "NativeSerialize18.Internal.h"

namespace KaGuYa::Start::Runtime::NativeSerialize18Detail
{
namespace
{
struct StreamStorage final
{
    union{FileStreamRuntime file;};StreamStorage() noexcept{::new(&file)FileStreamRuntime;}~StreamStorage() noexcept{}
};
struct StreamLife final
{
    StreamStorage& storage;IStartFileApi& api;bool armed=true;
    ~StreamLife() noexcept{if(armed)DestroyFileStreamAt40ED70(&storage.file,api);}
    void Normal() noexcept{armed=false;DestroyFileStreamAt40ED70(&storage.file,api);}
};
}
int __fastcall SaveFile(void* p,void*,const char* path)
{
    auto& services=RequireNativeSerialize18Services();auto& api=services.files;StreamStorage storage;SECURITY_ATTRIBUTES attributes{12,nullptr,TRUE};
    storage.file.OpenWithApi({path,GENERIC_WRITE,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,&attributes},api,services.transaction);
    StreamLife life{storage,api};Vector first{Buffer(p)};const U size=Address(first.End())-Address(first.Begin());
    auto* begin=Vector{Buffer(p)}.Begin();const int result=storage.file.WriteWithApi(begin,size,api);
    life.Normal();return result==0?1:0; // stream成功=1，CSerialize返回0；不SetEndOfFile，原打开方式4。
}
int __fastcall LoadFile(void* p,void*,const char* path)
{
    auto& services=RequireNativeSerialize18Services();auto& api=services.files;StreamStorage storage;SECURITY_ATTRIBUTES attributes{12,nullptr,TRUE};
    storage.file.OpenWithApi({path,GENERIC_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,&attributes},api,services.transaction);
    StreamLife life{storage,api};int result=0;
    if(storage.file.fileAt04.baseAt00)
    {
        const U count=static_cast<U>(storage.file.SizeWithApi(api)); // 原只消费EAX低DWORD，高32不构成额外拒绝门。
        if(count)
        {
            void* v=Buffer(p);ResizeNativeSerialize18ByteVectorAt4570D0(v,count,v);
            auto* begin=Vector{Buffer(p)}.Begin();result=storage.file.ReadExactWithApi(begin,count,api);
        }
    }
    life.Normal();return result==0?1:0; // 空文件/未打开返回1，保留旧buffer；不猜隐式读模式。
}
int __fastcall ReadStream(void* p,void*,IFileStreamRuntime* stream,U count)
{
    if(count==0xFFFFFFFF)return 1;void* v=Buffer(p);ResizeNativeSerialize18ByteVectorAt4570D0(v,count,v);
    auto* begin=Vector{Buffer(p)}.Begin();return stream->ReadExactAtVslot10(begin,count)==0?1:0;
}
int __fastcall ImportReference(void* p,void*,const YtlReferenceSlotRuntime* borrowed)
{
    auto* control=ReadAt<const void*>(borrowed,8);U count=control?ReadAt<U>(control,0x10):0;
    void* v=Buffer(p);ResizeNativeSerialize18ByteVectorAt4570D0(v,count,v);
    control=ReadAt<const void*>(borrowed,8);count=control?ReadAt<U>(control,0x10):0;
    const U base=ReadAt<U>(borrowed);const U stride=base&&control?ReadAt<U>(control,0xC):0;
    const B* source=base?reinterpret_cast<const B*>(base+ReadAt<U>(borrowed,4)*stride):nullptr;
    auto* destination=Vector{Buffer(p)}.Begin();std::memmove(destination,source,count);return 0;
    // +78只借Ref地址(ret4)，不增持/不消费。resize前后的count、后面的buffer获取各自独立。
}
}

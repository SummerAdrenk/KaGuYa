#include "LogDialogNative.Internal.h"
#include <limits>

namespace KaGuYa::Start::Runtime::LogDialogNativeDetail
{
namespace
{
void DestroyPrefix(RuntimeCString* p,std::uint32_t count) noexcept
{
    // 原53FB52先退步长再析构；清理再次抛出C++异常时在失败元素终止。
    // 此处不宣称覆盖原53FB96的所有SEH异常码过滤结果。
    while(count)DestroyRuntimeCStringAt40CD40(p+--count);
}
struct Prefix final
{
    RuntimeCString* p;std::uint32_t& count;bool active=true;
    ~Prefix() noexcept{if(active)DestroyPrefix(p,count);}
};
class RetiredArray final:public EnvironmentObjectReferenceControl
{
public:
    RetiredArray() noexcept:EnvironmentObjectReferenceControl(nullptr){}
    void DestroyPayloadAtVslot00() override{_purecall();}
    void DeleteControlAtVslot04(std::uint32_t f) override{if(f&1u)::operator delete(this,std::size_t{0x14});}
};
class ArrayControl final:public EnvironmentObjectReferenceControl
{
public:
    explicit ArrayControl(RuntimeCString* p) noexcept:EnvironmentObjectReferenceControl(p,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        auto* p=static_cast<RuntimeCString*>(payloadAt14);if(!p)return;
        auto* cookie=reinterpret_cast<std::uint32_t*>(p)-1;
        std::uint32_t count=*cookie;Prefix cleanup{p,count};
        // 原53FADB..53FAF5先减count；失败元素不重试，展开只处理较低前缀。
        while(count)DestroyRuntimeCStringAt40CD40(p+--count);
        cleanup.active=false;const std::uint32_t bytes=*cookie*4u+4u;::operator delete[](cookie,bytes);
    }
    void DeleteControlAtVslot04(std::uint32_t f) override
    {
        std::array<std::byte,20> fields;std::memcpy(fields.data(),reinterpret_cast<std::byte*>(this)+4,20);
        auto* p=::new(this) RetiredArray;std::memcpy(reinterpret_cast<std::byte*>(p)+4,fields.data(),20);
        if(f&1u)::operator delete(p,std::size_t{0x18});
    }
};
static_assert(sizeof(ArrayControl)==0x18&&sizeof(RetiredArray)==0x18);
}
void RebuildVoices(Slot& out,int count)
{
    // 412870直接发布到原78h槽，不能先建临时owner再交换而隐藏异常前缀。
    ReleaseYtlReference(out);if(count<=0)return;
    const auto n=static_cast<std::uint32_t>(count),maximum=(std::numeric_limits<std::uint32_t>::max)();
    const auto bytes=n>(maximum-4u)/4u?maximum:n*4u+4u;
    auto* cookie=static_cast<std::uint32_t*>(::operator new[](bytes));RuntimeCString* p=nullptr;
    if(cookie)
    {
        *cookie=n;p=reinterpret_cast<RuntimeCString*>(cookie+1);std::uint32_t completed=0;Prefix cleanup{p,completed};
        for(;completed<n;++completed)InitializeEmptyRuntimeCString(p[completed],RequireLogDialogNativeServices().strings);
        cleanup.active=false; // 原元素构造失败只销毁前缀，不补释放外层数组块。
    }
    if(auto* old=Read(out.controlAt08))old->DeleteControlAtVslot04(1);
    out.biasAt04=0;out.baseAt00=reinterpret_cast<std::uintptr_t>(p);
    void* raw=::operator new(0x18);auto* c=raw?::new(raw) ArrayControl(p):nullptr;out.controlAt08=c;
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(c)+4u)=1;
    out.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C=4;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10=n;
}
RuntimeCString* VoiceAt(const Slot& r,std::uint32_t index) noexcept
{
    const auto base=Read(r.baseAt00);if(!base)return nullptr;auto* c=Read(r.controlAt08);
    const auto stride=c?Read(static_cast<EnvironmentObjectReferenceControl*>(c)->strideAt0C):0u;
    return reinterpret_cast<RuntimeCString*>(base+(Read(r.biasAt04)+index)*stride);
}
}

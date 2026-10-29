#include <Runtime/EnvironmentPathExistsNative.h>
#include <Runtime/RuntimeCStringInputNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
union Text{RuntimeCString value;Text()noexcept{}~Text()noexcept{}};
ATL::CStringData* Header(const char* chars)noexcept{return reinterpret_cast<ATL::CStringData*>(const_cast<char*>(chars))-1;}
}
bool EnvironmentPathExistsNativeAt46CC90(const char* name,IEnvironmentPathApi& api,IRuntimeStringManagerSource& strings)
{
    Text text;ConstructRuntimeCStringInputNativeAt40B530(&text.value,name,strings);
    struct Unwind final{RuntimeCString& text;bool active=true;~Unwind()noexcept{if(active)DestroyRuntimeCStringAt40CD40(&text);}} unwind{text.value};
    auto* chars=const_cast<char*>(text.value.GetString());const int length=Header(chars)->nDataLength;
    if(length>=2&&chars[length-1]=='\\'&&!(length>=3&&chars[length-2]==':'))
    {
        // 46CDC3只在refs>1时分离；单引用或锁定串不经过现代GetBuffer的throw()边界。
        if(Header(chars)->nRefs>1)
        {
            PrepareRuntimeCStringWriteAt42D590(text.value,length);
            chars=const_cast<char*>(text.value.GetString());
        }
        const auto next=length-1;chars[next]=chars[length];
        // 46CDEF..46CDF5在移动之后重取槽并校验容量；错误不能继续查Attributes。
        auto* current=const_cast<char*>(text.value.GetString());
        if(next<0||next>Header(current)->nAllocLength)throw ATL::CAtlException(E_INVALIDARG);
        Header(current)->nDataLength=next;
        const_cast<char*>(text.value.GetString())[next]=0;
    }
    bool found=api.Attributes(text.value.GetString())!=INVALID_FILE_ATTRIBUTES;
    while(!found)
    {
        const auto error=api.LastError();
        if(error!=ERROR_NOT_READY)break;
        found=api.Attributes(text.value.GetString())!=INVALID_FILE_ATTRIBUTES;
    }
    const auto* cached=text.value.GetString();unwind.active=false;
    ReleaseRuntimeCStringHeader(Header(cached));return found;
}
}

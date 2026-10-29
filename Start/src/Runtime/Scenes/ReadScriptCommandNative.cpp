#include <Runtime/ReadScriptCommandNative.h>
#include <Runtime/SceneManagerNative.h>
#include <Runtime/RuntimeCString.h>
#include <cstring>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
void* CaptureReadScriptCommandRoot(IReadScriptCommandNativeSource& source,std::uint32_t ensure,std::uint32_t read)
{source.EnsureCommandRootAt413BE0(ensure);return source.CommandRootAt6198C4(read);}
bool CompareReadScriptCommandNative(IReadScriptCommandNativeSource& source,void* root,
    std::uint32_t site,const char* expected,ReadScriptCommandRelease release)
{
    auto* manager=source.CommandManagerAt4186D0(root,site);
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(manager->commandAt1C.GetString()))-1;
    auto* clone=CloneRuntimeCStringDataAt414540(header);
    const auto* chars=reinterpret_cast<const char*>(clone+1);
    // 原12处均用进程__mbscmp，不逐次创建/销毁locale，不改为_mbscmp_l。
    if(release==ReadScriptCommandRelease::CachedHeader)
    {
        const bool matched=::_mbscmp(reinterpret_cast<const unsigned char*>(chars),reinterpret_cast<const unsigned char*>(expected))==0;
        ReleaseRuntimeCStringHeader(clone);return matched;
    }
    union Text{RuntimeCString value;Text()noexcept{}~Text()noexcept{}} text;
    std::memcpy(&text.value,&chars,sizeof(chars)); // 4ADAD8在Clone正常返回后才写fresh out。
    const bool matched=::_mbscmp(reinterpret_cast<const unsigned char*>(text.value.GetString()),reinterpret_cast<const unsigned char*>(expected))==0;
    DestroyRuntimeCStringAt40CD40(&text.value);return matched;
}
void SetReadScriptCommandNative(IReadScriptCommandNativeSource& source,void* root,
    std::uint32_t site,const char* text,int length)
{
    auto* manager=source.CommandManagerAt4186D0(root,site);
    SetRuntimeCStringAt437680(manager->commandAt1C,text,length);
}
}

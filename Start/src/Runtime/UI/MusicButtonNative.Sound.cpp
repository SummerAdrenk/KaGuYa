#include "MusicButtonNative.Internal.h"
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/PatternResourcePlan.h>
#include <Runtime/RuntimeCStringFolderNative.h>
#include <mbstring.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace MusicButtonNativeDetail;
using ScannerBody=RuntimeCStringFolderNativeBody8h;
void ReleaseScanner(ScannerBody& p)
{
    auto* header=reinterpret_cast<ATL::CStringData*>(const_cast<char*>(p.textAt04.GetString()))-1;
    p.vptrAt00=&RuntimeStringScannerNativeMethods();ReleaseRuntimeCStringHeader(header);
}
ScannerBody* __fastcall DeleteSuffix(ScannerBody* p,void*,unsigned flags) noexcept
{ReleaseScanner(*p);if(flags&1)::operator delete(p,std::size_t{8});return p;}
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
// noexcept 实现仅向可抛槽类型放宽，不改变原异常处理。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const RuntimeCStringFolderNativeVtable suffixTable{reinterpret_cast<decltype(RuntimeCStringFolderNativeVtable::deletingDestructorAt00)>(&DeleteSuffix)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
class Scanner final
{
public:
    explicit Scanner(const char* source,IRuntimeStringManagerSource& strings)
    {
        body.vptrAt00=&RuntimeStringScannerNativeMethods();InitializeEmptyRuntimeCString(body.textAt04,strings);
        struct Prefix final
        {
            ScannerBody& body;bool active=true,derived=false;
            ~Prefix() noexcept
            {
                if(!active)return;
                // 40C760的591100只清TEXT；45DA40取得state0后才恢复scanner基表。
                if(derived)ReleaseScanner(body);else DestroyRuntimeCStringAt40CD40(&body.textAt04);
            }
        }prefix{body};
        SetRuntimeCStringAt437680(body.textAt04,"",0);body.vptrAt00=&suffixTable;prefix.derived=true;
        ScanPatternResourceSuffixAt45DA40(body.textAt04,source,strings);
        prefix.active=false;active=true; // 40C760/45DA40失败在展开中清理，不在普通catch中替换主异常。
    }
    ~Scanner() noexcept{if(active)ReleaseScanner(body);}
    void Normal(){active=false;ReleaseScanner(body);}
    ScannerBody body;bool active=false;
};
void Stop(MusicButtonNativeServices& services,unsigned rootSite,unsigned envSite)
{
    auto* sound=Sound(services.source,rootSite);
    SetRuntimeCStringAt437680(Field<RuntimeCString>(Registry(services.source,2,envSite),0x2C),"",0);
    services.source.StopSoundAt4383B0(sound,Read(Field<unsigned>(sound,0x10)));
}
}
void SelectMusicButtonSoundNativeAt4C1A20(MusicButtonNativeBody70h& p,int index)
{
    using namespace MusicButtonNativeDetail;auto& services=RequireMusicButtonNativeServices();
    if(Read(p.soundSelectionAt68)==index)
    {p.soundSelectionAt68=-1;Stop(services,0x4C1BEB,0x4C1C19);return;}
    p.soundSelectionAt68=index;
    const auto songs=services.dialogs.NativeDialogServices().resources.songs;
    if(static_cast<unsigned>(index)>=songs.size()){Stop(services,0x4C1BEB,0x4C1C19);return;}
    Scanner scanner(songs[static_cast<unsigned>(index)],services.strings);
    ArchiveRawResourceNameRuntime suffix(scanner.body.textAt04.GetString(),services.strings);
    scanner.Normal();
    if(!::_mbscmp(reinterpret_cast<const unsigned char*>(suffix.Value().GetString()),reinterpret_cast<const unsigned char*>("mpg")))
    {Stop(services,0x4C1AC2,0x4C1AF0);p.movieAt6C=1;}
    else
    {
        auto* params=Resolve<void>(Field<Slot>(Registry(services.source,1,0x4C1B20),0x1C));
        auto* sound=Sound(services.source,0x4C1B52);
        using Getter=const int*(__thiscall*)(void*);auto* table=Read(*static_cast<const std::byte**>(params));Getter getter;
        std::memcpy(&getter,table+0x14,4);const auto value=Read(*getter(params));
        const auto* name=songs[static_cast<unsigned>(index)];
        auto& mapping=Field<RuntimeCString>(Registry(services.source,2,0x4C1B92),0x2C);
        services.source.PlayMusicAt4BCCC0(sound,mapping,name,value,1);
    }
    suffix.ReleaseNormal(); // 原state2撤销后Free可抛，selected与movie前缀不回滚。
}
std::span<const char* const,18> MusicButtonNativeSongsSample18() noexcept
{
    // 保留历史审计接口；正式播放从同一个游戏profile取曲目，不再调用本函数。
    return std::span<const char* const,18>(MamatomoMusicResourceProfile().songs.data(),18);
}
}

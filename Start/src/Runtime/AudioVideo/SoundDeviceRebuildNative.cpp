#include <Runtime/SoundDeviceRebuildNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const void* object,DWORD offset=0)noexcept
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<DWORD>(object)+offset);}
void* ResolveInline(const void* ref)noexcept
{
    const auto base=Read<DWORD>(ref);if(!base)return nullptr;
    const auto* control=Read<const void*>(ref,8);const auto stride=control?Read<DWORD>(control,0x0C):0u;
    return reinterpret_cast<void*>(base+Read<DWORD>(ref,4)*stride);
}
void* InlineArchive(EnvironmentRootStorage* environment,DWORD index)noexcept
{
    const auto begin=Read<DWORD>(environment,0x30),end=Read<DWORD>(environment,0x34);
    const auto count=static_cast<DWORD>(std::bit_cast<std::int32_t>(end-begin)/12);
    return index<count?ResolveInline(reinterpret_cast<const void*>(begin+index*12u)):nullptr;
}
void* Params(void* save)noexcept
{return ResolveInline(reinterpret_cast<const void*>(reinterpret_cast<DWORD>(save)+0x1Cu));}
void* CapturedParams(ISoundDeviceRebuildNativeSource& source)
{return Params(InlineArchive(source.EnvironmentAt4180C0(),1));}
SoundManagerNativeBody50h* Manager(ISoundDeviceRebuildNativeSource& source)
{
    source.EnsureRootAt413BE0();auto* root=source.RootAt6198C4();
    return static_cast<SoundManagerNativeBody50h*>(ResolveInline(&source.SoundManagerAt24(root)));
}
const char* Music(void* unsave)noexcept{return Read<const char*>(unsave,0x2C);}
const char* Mapping(void* unsave,DWORD byteOffset)noexcept
{return Read<const char*>(reinterpret_cast<const void*>(Read<DWORD>(unsave,0x30)+byteOffset));}
std::int32_t Length(const char* characters)noexcept
{return Read<std::int32_t>(reinterpret_cast<const void*>(reinterpret_cast<DWORD>(characters)-0x0Cu));}
void* Pointer(void* parameters,DWORD slot)
{
    using Function=void*(__thiscall*)(void*);
    const auto* table=Read<const void*>(parameters);return Read<Function>(table,slot)(parameters);
}
BYTE HasLoop(void* parameters)
{
    using Function=DWORD(__thiscall*)(void*);
    const auto* table=Read<const void*>(parameters);return static_cast<BYTE>(Read<Function>(table,0x28)(parameters));
}
std::int32_t MappingCount(void* unsave)noexcept
{
    const auto begin=Read<DWORD>(unsave,0x30),end=Read<DWORD>(unsave,0x34),bits=end-begin;
    return std::bit_cast<std::int32_t>((bits>>2)|((bits&0x80000000u)?0xC0000000u:0u));
}
[[noreturn]]void ResetFailed(ISoundDeviceRebuildNativeSource& source)
{
    // 权威FBD原PE5AE184，38个SJIS字节；420C50借当前Env标题/窗口，不复制CString。
    static constexpr char text[]=
        "\x83\x54\x83\x45\x83\x93\x83\x68\x83\x89\x83\x43\x83\x75\x83\x89"
        "\x83\x8A\x82\xCC\x83\x8A\x83\x5A\x83\x62\x83\x67\x82\xC9\x8E\xB8"
        "\x94\x73\x81\x49\x81\x49";
    static_assert(sizeof(text)==39);
    auto* environment=source.EnvironmentAt4180C0();
    const auto caption=Read<const char*>(environment,0x10);const auto window=Read<HWND>(environment,0x2C);
    static_cast<void>(source.MessageBoxA(window,text,caption,MB_ICONERROR));throw 1;
}
}
void RebuildSoundDeviceNativeAt432C20(SoundManagerNativeBody50h* entry,GUID device,
    ISoundDeviceRebuildNativeSource& source,ISoundDeviceRebuildNativeCalls& calls)
{
    auto* music=Manager(source);calls.StopAbsoluteAt4383B0(music,Read<DWORD>(music,0x10));
    calls.StopEffectsAt413270(entry);
    const auto window=Read<HWND>(source.EnvironmentAt4180C0(),0x2C);
    if(!calls.ResetDeviceAt432C99(window))ResetFailed(source);
    calls.SetGlobalFocusAt432D45();calls.InitializeHandlesAt42CA60(entry);

    // BGM四个Env捕获是独立节点；固定params/name/manager后才调用volume虚槽。
    auto* environment=source.EnvironmentAt4180C0();
    auto* parameters=Params(ResolveEnvironmentArchiveAt417990(*environment,1));
    if(!Read<BYTE>(Pointer(parameters,0x10)))
    {
        environment=source.EnvironmentAt4180C0();
        if(Length(Music(ResolveEnvironmentArchiveAt417990(*environment,2)))!=0)
        {
            environment=source.EnvironmentAt4180C0();parameters=Params(ResolveEnvironmentArchiveAt417990(*environment,1));
            environment=source.EnvironmentAt4180C0();const auto* name=Music(ResolveEnvironmentArchiveAt417990(*environment,2));
            auto* selected=Manager(source);const auto volume=Read<std::int32_t>(Pointer(parameters,0x14));
            static_cast<void>(calls.PlayBgmAt42BFE0(selected,name,volume,1));
        }
    }
    DWORD index=0;
    for(;;++index)
    {
        const auto bound=Read<DWORD>(source.GameSystemAt418480(),0x50);
        if(std::bit_cast<std::int32_t>(index)>=std::bit_cast<std::int32_t>(bound))break;
        auto* unsave=InlineArchive(source.EnvironmentAt4180C0(),2);
        if(Length(Mapping(unsave,index*4u))==0)continue;
        unsave=InlineArchive(source.EnvironmentAt4180C0(),2);const auto* name=Mapping(unsave,index*4u);
        auto* selected=Manager(source);static_cast<void>(calls.RestoreFirstAt42C1B0(selected,name,index,0));
    }
    for(;;++index)
    {
        auto* unsave=InlineArchive(source.EnvironmentAt4180C0(),2);
        if(std::bit_cast<std::int32_t>(index)>=MappingCount(unsave))break;
        unsave=InlineArchive(source.EnvironmentAt4180C0(),2);const auto byteOffset=index*4u;
        if(Length(Mapping(unsave,byteOffset))==0)continue;
        const bool loop=HasLoop(CapturedParams(source))!=0;
        // 原433775/4339E4均为JZ：零才跳过，不能按mute字段名反转原行为。
        parameters=CapturedParams(source);if(!Read<BYTE>(Pointer(parameters,loop?0x2C:0x20)))continue;
        parameters=CapturedParams(source);auto* const game=source.GameSystemAt418480();
        unsave=InlineArchive(source.EnvironmentAt4180C0(),2);const auto* name=Mapping(unsave,byteOffset);
        auto* selected=Manager(source);
        const auto volume=Read<std::int32_t>(Pointer(parameters,loop?0x30:0x24));
        const auto group=index-Read<DWORD>(game,0x50);
        static_cast<void>(calls.RestoreSecondAt42C070(selected,name,group,volume));
    }
    std::memcpy(&entry->deviceAt40,&device,sizeof(device));
}
}

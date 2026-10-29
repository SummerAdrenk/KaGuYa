#include <Runtime/RootDemoSoundCommandServices.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ParamsDemoStorage.h>
#include <Runtime/RuntimeCString.h>
#include <cstdint>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const void* object,std::uint32_t offset=0)noexcept
{return *reinterpret_cast<const volatile T*>(reinterpret_cast<std::uintptr_t>(object)+offset);}
void* ResolveOriginalReference(const void* slot)noexcept
{
    // 4188C0/4188F0：先检查base，control/stride之后读bias，最后重新读base。
    if(!Read<std::uint32_t>(slot))return nullptr;
    const auto* control=Read<const void*>(slot,8);
    const auto stride=control?Read<std::uint32_t>(control,12):0u;
    const auto bias=Read<std::uint32_t>(slot,4);
    return reinterpret_cast<void*>(Read<std::uint32_t>(slot)+bias*stride);
}
RuntimeCString& MusicOutput(EnvironmentRootStorage* environment)
{
    auto* unsave=ResolveEnvironmentArchiveAt417990(*environment,2);
    // 原+2C仅计算地址，到437680才消费；不提前Require或改成int异常。
    return *reinterpret_cast<RuntimeCString*>(reinterpret_cast<std::uintptr_t>(unsave)+0x2Cu);
}
SoundManagerNativeBody50h* Manager(RootRuntimePayload* root)noexcept
{
    // payload是现代载体；空根仍按原4188F0的+24读取失败，不借最新发布根。
    const auto* slot=root?&root->SoundManagerSlotAt24():reinterpret_cast<const YtlReferenceSlotRuntime*>(0x24u);
    return static_cast<SoundManagerNativeBody50h*>(ResolveOriginalReference(slot));
}
const char* Name(const ParamsDemoCmdSound& command)noexcept
{return Read<const char*>(&command,8);}
}

void RootDemoSoundCommandServices::ExecuteDemoSoundAt4BF692(const ParamsDemoCmdSound& command)
{
    const auto mode=Read<std::uint8_t>(&command,4);
    const bool empty=_mbscmp(reinterpret_cast<const unsigned char*>(Name(command)),
        reinterpret_cast<const unsigned char*>(""))==0;
    if(mode==0)
    {
        if(empty)
        {
            root_.EnsureCreatedAt413BE0();auto* savedRoot=root_.ReadPublishedAt6198C4();
            auto* environment=environment_.CaptureAt4180C0();auto& output=MusicOutput(environment);
            root_.NativeSoundServices().Demo(Manager(savedRoot)).StopDemoMusicAt4C3500(output,archives_);
            return;
        }
        auto* first=environment_.CaptureAt4180C0();
        auto* save=ResolveEnvironmentArchiveAt417990(*first,1);
        auto* parameters=ResolveOriginalReference(reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(save)+0x1Cu));
        const auto* borrowedName=Name(command);
        root_.EnsureCreatedAt413BE0();
        const auto* table=Read<const void*>(parameters); // 原4BF738早于Root发布读取。
        auto* savedRoot=root_.ReadPublishedAt6198C4();
        using Volume=std::uint32_t*(__thiscall*)(void*);
        static_assert(sizeof(Volume)==4);
        const auto volume=Read<Volume>(table,0x14);
        const auto* field=volume(parameters);
        auto* second=environment_.CaptureAt4180C0();
        const auto value=Read<std::int32_t>(field); // 4BF74E：Env②之后、slot2解析之前。
        auto& output=MusicOutput(second);
        static_cast<void>(root_.NativeSoundServices().Demo(Manager(savedRoot))
            .PlayDemoMusicAt4BCCC0(output,borrowedName,value,0,archives_));
        return;
    }
    if(mode==1)
    {
        if(empty)
        {
            root_.EnsureCreatedAt413BE0();auto* savedRoot=root_.ReadPublishedAt6198C4();
            root_.NativeSoundServices().Demo(Manager(savedRoot)).StopAndClearAllVoiceSlots();
        }
        else
        {
            const auto* borrowedName=Name(command);
            root_.EnsureCreatedAt413BE0();auto* savedRoot=root_.ReadPublishedAt6198C4();
            static_cast<void>(root_.NativeSoundServices().Demo(Manager(savedRoot))
                .PlayDemoVoiceAt4BCE10(borrowedName,archives_));
        }
        return;
    }
    if(empty)
    {
        root_.EnsureCreatedAt413BE0();auto* savedRoot=root_.ReadPublishedAt6198C4();
        root_.NativeSoundServices().Demo(Manager(savedRoot)).StopEffectsKeepingMappingsAt413270(archives_);
    }
}
}

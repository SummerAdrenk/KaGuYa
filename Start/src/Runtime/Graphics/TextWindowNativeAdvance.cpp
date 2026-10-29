#include <Runtime/TextWindowNativeAdvance.h>
#include <Runtime/ReadScriptAdvanceCommandGateNative.h>
#include <Runtime/TextWindowNativeMaintenance.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentStateArchives.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>
#include <Runtime/RootRuntimeOwner.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value)noexcept{return *static_cast<const volatile T*>(&value);}
std::uint8_t Byte(const void* body,std::uint32_t offset)noexcept
{return *reinterpret_cast<const volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(body)+offset);}
std::uint32_t Word(const void* body,std::uint32_t offset)noexcept
{return *reinterpret_cast<const volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(body)+offset);}
IEnvironmentArchiveRuntime* Lookup(ITextWindowNativeAdvanceSource& source,std::uint32_t caller,unsigned index)
{return ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(caller),index);}
void* Inline(EnvironmentRootStorage* environment,unsigned index)
{
    const volatile auto& array=environment->archivesAt30;
    const auto end=reinterpret_cast<std::uintptr_t>(array.end);auto* const begin=array.begin;
    const auto count=std::uint32_t(std::bit_cast<std::int32_t>(std::uint32_t(end-reinterpret_cast<std::uintptr_t>(begin)))/12);
    if(count<=index)return nullptr;
    const volatile auto& slot=begin[index];const auto base=slot.baseAt00;if(!base)return nullptr;
    const auto* control=static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto stride=control?control->strideAt0C:0u;const auto bias=slot.biasAt04;
    return reinterpret_cast<void*>(base+bias*stride);
}
YtlReferenceSlotRuntime* Voices(void* unsaved)noexcept
{return reinterpret_cast<YtlReferenceSlotRuntime*>(reinterpret_cast<std::uintptr_t>(unsaved)+0x3Cu);}
}
bool AdvanceTextWindowNativeAt4BC260(TextWindowLayerNativeBodyE8h& body,std::uint32_t input,
    TextWindowNativeAdvanceServices& services)
{
    auto& source=services.source;
    const auto remaining=Read(body.autoWaitAt5C);
    if(remaining)body.autoWaitAt5C=std::bit_cast<std::int32_t>(std::uint32_t(remaining)-1u);
    if(services.playback.ReadSkipAt4AA540(body))
    {
        if(Byte(Lookup(source,0x4BC288,1),0x38)&2u)
            services.sound.CaptureRootAfter413BE0().Resolve().voice.StopAndClearAllVoiceSlots();
        if(Byte(Lookup(source,0x4BC2D2,1),0x38)&0x20u)
            services.sound.CaptureRootAfter413BE0().Resolve().effects.StopEffectsKeepingMappingsAt413270();
        services.playback.ClearRootUpdateEnabledAt1B0After413BE0();
        return true;
    }
    if(Read(body.skipAt60))
    {
        body.skipAt60=0;
        RestoreTextWindowEffectsAt4BC34B(services.restore);
        if(!source.CaptureLayersAfter413BE0(0x4BC4F4).LayerAt418C10(4).EnabledByteAt0C())
            static_cast<void>(source.CaptureLayersAfter413BE0(0x4BC518).RequestOwnedLayerAtVslot10(4,1,0));
    }
    const auto mask=std::uint8_t(input);
    if(mask&5u)return false;
    if((mask&2u)&&(Byte(Lookup(source,0x4BC54C,2),0x18)&2u))
    {
        const auto* mouse=source.CaptureMouseAt418210(0x4BC55E);
        if(Read(mouse->secondaryAt2C)==0&&Read(mouse->wheelAt34)>0)
        {
            auto* unsaved=Lookup(source,0x4BC578,2);
            auto& flags=*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(unsaved)+0x1Cu);
            flags=Read(flags)|2u;
        }
        return false;
    }
    if(!ReadScriptAdvanceCommandAllowedNativeAt4AA3E0(services.command))return false;
    // 4BC5A0是inline Env2；autoWait必须在其回调之后从同一E8h重读。
    if((Byte(Inline(source.CaptureEnvironmentAt4180C0(0x4BC5A0),2),0x18)&2u)&&Read(body.autoWaitAt5C)==0)
    {
        if(Word(Lookup(source,0x4BC601,2),0x3C)==0)return true;
        auto* save=static_cast<SaveDataEnvironmentArchive*>(Lookup(source,0x4BC619,1));
        const auto gate=TextSoundVoiceGateTableReceiver::Native(ResolveEnvironmentObjectReference<EnvironmentSoundParamsArchive>(save->soundParamsAt1C));
        const auto* names=Voices(Lookup(source,0x4BC64A,2));
        if(!gate.Allowed(names))return true;
        if(!services.sound.CaptureRootAfter413BE0().Resolve().effects.AnyVoiceSlotPlaying())return true;
    }
    const auto* first=source.CaptureMouseAt418210(0x4BC68D);
    const auto wheel=Read(first->secondaryAt2C)==0?Read(first->wheelAt34):0;
    // 4BC6A6无条件捕获一次；下面417990沿用这个Env，不能用便利getter再次捕获。
    auto* const environment=source.CaptureEnvironmentAt4180C0(0x4BC6A6);
    if(wheel>0)
    {
        auto* unsaved=ResolveEnvironmentArchiveAt417990(*environment,2);
        auto& flags=*reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(unsaved)+0x1Cu);
        flags=Read(flags)|2u;
        MaintainTextWindowVoiceNativeAt4C0A70(body,services.maintenance);
        return false;
    }
    auto* click=static_cast<ClickInfoArchive*>(ResolveEnvironmentArchiveAt417990(*environment,5));
    if(services.playback.ReadAdvanceInputAt4B0550(*click))return true;
    const auto* second=source.CaptureMouseAt418210(0x4BC6E7);
    if(Read(second->secondaryAt2C)==0&&Read(second->wheelAt34)<0)return true;
    MaintainTextWindowVoiceNativeAt4C0A70(body,services.maintenance);
    return false;
}
EnvironmentRootStorage* RootTextWindowNativeAdvanceSource::CaptureEnvironmentAt4180C0(std::uint32_t)
{return environment_.CaptureAt4180C0();}
MouseManagerNativeBody3Ch* RootTextWindowNativeAdvanceSource::CaptureMouseAt418210(std::uint32_t)
{return ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(input_.CaptureAt418210()->mouseAt00);}
DrawLayersView RootTextWindowNativeAdvanceSource::CaptureLayersAfter413BE0(std::uint32_t)
{
    root_.EnsureCreatedAt413BE0();auto* const root=root_.ReadPublishedAt6198C4();
    auto* const draw=ResolveDrawManagerReferenceAt4175A0(root->DrawManagerSlotAt18());
    return DrawLayersView(draw,memory_).WithNativeCallServices(root->PostWindow().BorrowNativeDrawCallServices());
}
}

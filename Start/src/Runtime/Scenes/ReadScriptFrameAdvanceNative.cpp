#include <Runtime/ReadScriptFrameAdvanceNative.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/InputRootNative.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>T Read(const T& value){return *static_cast<const volatile T*>(&value);}
void* Archive(EnvironmentRootOwner& environment,std::uint32_t index)
{return ResolveEnvironmentArchiveAt417990(*environment.CaptureAt4180C0(),index);}
std::uint8_t Byte(const void* body,std::uint32_t offset)
{return *reinterpret_cast<const volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(body)+offset);}
}
bool SuppressReadScriptDrawAt4B5A3A(EnvironmentRootOwner& environment,InputRootOwner& input)
{
    static_cast<void>(Archive(environment,5));
    const auto& array=environment.CaptureAt4180C0()->archivesAt30;
    const auto begin=reinterpret_cast<std::uintptr_t>(Read(array.begin));
    const auto end=reinterpret_cast<std::uintptr_t>(Read(array.end));
    const auto count=std::uint32_t(std::bit_cast<std::int32_t>(std::uint32_t(end-begin))/12);
    std::uintptr_t save=0;
    if(count>1)
    {
        const auto* slot=reinterpret_cast<const volatile YtlReferenceSlotRuntime*>(begin+12u);
        const auto base=slot->baseAt00;
        if(base)
        {
            const auto* control=static_cast<const volatile EnvironmentObjectReferenceControl*>(slot->controlAt08);
            const auto stride=control?control->strideAt0C:0u;save=base+slot->biasAt04*stride;
        }
    }
    if(Byte(reinterpret_cast<void*>(save),0x6D)==0)return true;
    if(Byte(Archive(environment,2),0x1C)&2u)return true;
    if(Byte(Archive(environment,2),0x29)!=0)return Byte(Archive(environment,1),0x6E)==0;
    auto* root=input.CaptureAt418210();
    auto* keyboard=ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(root->keyboardAt18);
    if(ReadKeyboardHeldAt4198D0(*keyboard,6))return true;
    return (Byte(Archive(environment,2),0x14)&2u)!=0;
}
}

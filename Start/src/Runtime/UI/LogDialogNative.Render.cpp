#include "LogDialogNative.Internal.h"
#include <Runtime/LogPageResourceProfile.h>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime::LogDialogNativeDetail
{
namespace
{
struct Format final
{
    Slot name{},voices{},body{};
    ~Format() noexcept{ReleaseYtlReference(body);ReleaseYtlReference(voices);ReleaseYtlReference(name);}
    MessageFormatSelectionRawTarget Target(){return {name,voices,body};}
};
bool DefaultNames(void* save)
{
    auto& s=RequireLogDialogNativeServices().source;
    auto* params=s.CaptureParamsAt418480(0x50FEA3);
    const auto* given=params->gameSystemAt04.get()->defaultGivenName.Native().GetString();
    if(!given)throw ATL::CAtlException(E_FAIL);
    if(::_mbscmp(reinterpret_cast<const unsigned char*>(Field<RuntimeCString>(save,0x58).GetString()),reinterpret_cast<const unsigned char*>(given)))return false;
    params=s.CaptureParamsAt418480(0x50FEE0);
    const auto* family=params->gameSystemAt04.get()->defaultFamilyName.Native().GetString();
    if(!family)throw ATL::CAtlException(E_FAIL);
    return !::_mbscmp(reinterpret_cast<const unsigned char*>(Field<RuntimeCString>(save,0x5C).GetString()),reinterpret_cast<const unsigned char*>(family));
}
void ReadText(Slot& slot,CStringLife& out)
{
    using Fn=RuntimeCString*(__thiscall*)(void*,RuntimeCString*);
    auto* receiver=Resolve<void>(slot);auto* table=Read(*static_cast<const std::byte**>(receiver));Fn fn;
    std::memcpy(&fn,table+4,4);fn(receiver,&out.value);out.active=true;
}
}
void RenderRow(Row& row,int format)
{
    const auto& profile=CurrentLogPageResourceProfile();
    Format selected;auto& s=RequireLogDialogNativeServices();auto* save=Registry(1,0x53DAB3);
    auto* root=Root(0x53DB03);auto* files=Resolve<void>(s.source.RootFileManagerSlotAt00(root));
    auto* message=Resolve<void>(s.source.FileManagerMessageSlotAt10(files));
    const auto variant=static_cast<unsigned>(!DefaultNames(save));
    if(s.source.ReadFormatAt417C40(message,format,selected.Target(),variant,false)<0)return;
    if(Read(selected.voices.baseAt00))
    {
        auto* cached=Read(selected.voices.controlAt08);
        for(unsigned i=0;;++i)
        {
            const auto count=cached?Read(static_cast<EnvironmentObjectReferenceControl*>(cached)->countAt10):0u;
            if(std::bit_cast<int>(i)>=std::bit_cast<int>(count))break;
            const auto stride=cached?Read(static_cast<EnvironmentObjectReferenceControl*>(cached)->strideAt0C):0u;
            const auto address=Read(selected.voices.baseAt00)+(Read(selected.voices.biasAt04)+i)*stride;
            AppendRuntimeCStringAt41D5E9(row.voicesAt18,*reinterpret_cast<RuntimeCString*>(address));
        }
    }
    // 01–08原行surface602×120，正文(8,15/30)；09–18为(12,17/34)。
    auto origin=profile.bodyWithoutName;
    if(Read(selected.name.baseAt00))
    {
        CStringLife name;ReadText(selected.name,name);CStringLife formatted;
        FormatName(&formatted.value,name.value);formatted.active=true;
        if(!CreateFont(profile.nameFont))throw 1;
        if(!s.text.InitializeAt4389D0(formatted.value.GetString(),profile.nameOrigin.x,profile.nameOrigin.y))throw 1;
        const auto handle=Read(Resolve<GraphicsSurfaceNativeBody>(row.surfaceAt24)->handleAt04);
        if(!s.text.BlitAllAt53A980(handle,nullptr))throw 1;
        formatted.Normal();name.Normal();origin=profile.bodyWithName;
    }
    CStringLife body;ReadText(selected.body,body);
    if(!CreateFont(profile.bodyFont))throw 2;
    if(!s.text.InitializeAt4389D0(body.value.GetString(),origin.x,origin.y))throw 2;
    const auto handle=Read(Resolve<GraphicsSurfaceNativeBody>(row.surfaceAt24)->handleAt04);
    if(!s.text.BlitAllAt53A980(handle,nullptr))throw 2;
    body.Normal();row.formatAt10=format;
}
}

#include <Runtime/DrawLayersView.h>
#include <Runtime/GameBuildProfile.h>

namespace KaGuYa::Start::Runtime
{
std::uint8_t PrepareDrawLayersViewAt46A980(IDrawLayersViewSource& source, std::int32_t mode)
{
    auto result=source.CaptureDrawLayersAt413BE0().RequestOwnedLayerAtVslot10(1,0,0);
    for(std::uint32_t index=2;index<=LastStartupDrawLayerIndex();++index)
    {
        const auto request=StartupPrepareRequest(index,false,mode);
        result=static_cast<std::uint8_t>(result|source.CaptureDrawLayersAt413BE0().RequestOwnedLayerAtVslot10(index,request.requested,request.argument));
    }
    return result;
}
std::uint8_t PrepareDrawLayersViewAt46AD20(IDrawLayersViewSource& source,std::int32_t mode)
{
    auto result=source.CaptureDrawLayersAt413BE0().RequestOwnedLayerAtVslot10(1,1,0);
    for(std::uint32_t index=2;index<=LastStartupDrawLayerIndex();++index)
    {
        const auto request=StartupPrepareRequest(index,true,mode);
        result=static_cast<std::uint8_t>(result|source.CaptureDrawLayersAt413BE0().RequestOwnedLayerAtVslot10(index,request.requested,request.argument));
    }
    return result;
}
}

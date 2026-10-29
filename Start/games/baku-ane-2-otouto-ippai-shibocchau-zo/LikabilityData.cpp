#include "LikabilityData.h"

namespace KaGuYa::Start::Runtime::BakuAne2OtoutoIppaiShibocchauZoLikabilityData
{
namespace
{
// 原PE 0x4E5FD0/0x4F9D30；边界和原字节见likability-profile-integration/tables/06.json。
// 独立静态资料，不读取全局profile，不替其它款补图或修改脚本值。
}
LikabilityResourceProfile Profile() noexcept
{
    return {6,"baku-ane-2-otouto-ippai-shibocchau-zo",{},{},{},
        {0,1,{},false,false,false,nullptr,LikabilityCurrentTab::Disable,-1,-1,-1,{},{},nullptr},
        LikabilityResidualData{"Likability06 residual EroZemi page has no matching own-game resources or variables; direct entry rejected","\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F"},0x4E5FD0,0x4F9D30};
}
}

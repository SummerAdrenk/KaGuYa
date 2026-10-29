#include "LikabilityData.h"

namespace KaGuYa::Start::Runtime::HaramiTamaLikabilityData
{
namespace
{
// 原PE 0x4F8880/0x4F9010；边界和原字节见likability-profile-integration/tables/04.json。
// 独立静态资料，不读取全局profile，不替其它款补图或修改脚本值。
const char* const variables[]{
    "\x90\xAF\x89\xD4\x81\x40\x90\x6C\x8A\xD4\x93\x78",
    "\x90\xAF\x89\xD4\x81\x40\x97\x64\x89\xF6\x93\x78",
    "\x83\x8B\x83\x8B\x81\x40\x90\x6C\x8A\xD4\x93\x78",
    "\x83\x8B\x83\x8B\x81\x40\x97\x64\x89\xF6\x93\x78",
    "\x97\x52\x89\xC4\x81\x40\x90\x6C\x8A\xD4\x93\x78",
    "\x97\x52\x89\xC4\x81\x40\x97\x64\x89\xF6\x93\x78",
    "\x97\xCF\x94\x4A\x81\x40\x90\x6C\x8A\xD4\x93\x78",
    "\x97\xCF\x94\x4A\x81\x40\x97\x64\x89\xF6\x93\x78",
};
const char* const icons[]{
    "\x8D\x44\x8A\xB4\x93\x78\x83\x74\x83\x89\x83\x4F\x97\xF6\x88\xA4\x83\x49\x83\x93\x2E\x76\x72\x6D",
    "\x8D\x44\x8A\xB4\x93\x78\x83\x74\x83\x89\x83\x4F\x97\x64\x89\xF6\x83\x49\x83\x93\x2E\x76\x72\x6D",
};
const POINT positions[]{
    {574,196},
    {657,196},
    {740,196},
    {389,200},
    {315,200},
    {238,200},
    {574,296},
    {657,296},
    {740,296},
    {389,298},
    {315,298},
    {238,298},
    {574,403},
    {657,403},
    {740,403},
    {389,399},
    {315,399},
    {238,399},
    {574,501},
    {657,501},
    {740,501},
    {389,497},
    {315,497},
    {238,497},
};
const LikabilityRepeatedRow rows[]{
    {variables[0],icons[0],LikabilityPoints(positions).subspan(0,3)},
    {variables[1],icons[1],LikabilityPoints(positions).subspan(3,3)},
    {variables[2],icons[0],LikabilityPoints(positions).subspan(6,3)},
    {variables[3],icons[1],LikabilityPoints(positions).subspan(9,3)},
    {variables[4],icons[0],LikabilityPoints(positions).subspan(12,3)},
    {variables[5],icons[1],LikabilityPoints(positions).subspan(15,3)},
    {variables[6],icons[0],LikabilityPoints(positions).subspan(18,3)},
    {variables[7],icons[1],LikabilityPoints(positions).subspan(21,3)},
};
const LikabilityControlProfile controls[]{
    {{{1,{"\x8D\x44\x8A\xB4\x93\x78\x94\xF1\x95\x5C\x8E\xA6\x83\x49\x83\x93\x2E\x76\x72\x6D",nullptr,nullptr,"\x8D\x44\x8A\xB4\x93\x78\x94\xF1\x95\x5C\x8E\xA6\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D","\x8D\x44\x8A\xB4\x93\x78\x94\xF1\x95\x5C\x8E\xA6\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D","\x8D\x44\x8A\xB4\x93\x78\x94\xF1\x95\x5C\x8E\xA6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D","\x8D\x44\x8A\xB4\x93\x78\x94\xF1\x95\x5C\x8E\xA6\x94\xCD\x88\xCD\x2E\x76\x72\x6D"},nullptr},6,-1},true,-1,-1,false},
};
const char* const background[]{
    "\x8D\x44\x8A\xB4\x93\x78\x98\x67\x2E\x76\x72\x6D",
};
}
LikabilityResourceProfile Profile() noexcept
{
    return {4,"harami-tama",background,{},controls,
        {1,1,{},false,false,false,nullptr,LikabilityCurrentTab::Disable,-1,-1,-1,{},{},nullptr},
        LikabilityRepeatedData{rows},0x4F8880,0x4F9010};
}
}

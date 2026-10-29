#include "LikabilityData.h"

namespace KaGuYa::Start::Runtime::ChuXPetLikabilityData
{
namespace
{
// 原PE 0x4FACA0/0x4FB430；边界和原字节见likability-profile-integration/tables/03.json。
// 独立静态资料，不读取全局profile，不替其它款补图或修改脚本值。
const char* const variables[]{
    "\x8D\xE7\x96\xEB\x81\x40\x88\xA4\x8F\xEE\x93\x78",
    "\x8D\xE7\x96\xEB\x81\x40\x8F\x5D\x8F\x87\x93\x78",
    "\x8B\xD5\x97\x9C\x81\x40\x88\xA4\x8F\xEE\x93\x78",
    "\x8B\xD5\x97\x9C\x81\x40\x8F\x5D\x8F\x87\x93\x78",
    "\x83\x56\x83\x85\x83\x8A\x81\x40\x88\xA4\x8F\xEE\x93\x78",
    "\x83\x56\x83\x85\x83\x8A\x81\x40\x8F\x5D\x8F\x87\x93\x78",
};
const char* const icons[]{
    "\x8D\x44\x8A\xB4\x93\x78\x83\x74\x83\x89\x83\x4F\x97\xF6\x88\xA4\x83\x49\x83\x93\x2E\x76\x72\x6D",
    "\x8D\x44\x8A\xB4\x93\x78\x83\x74\x83\x89\x83\x4F\x8F\x5D\x8F\x87\x83\x49\x83\x93\x2E\x76\x72\x6D",
};
const POINT positions[]{
    {565,183},
    {596,239},
    {625,183},
    {660,239},
    {689,183},
    {721,239},
    {750,183},
    {410,177},
    {374,240},
    {352,177},
    {313,240},
    {294,177},
    {254,240},
    {236,177},
    {565,319},
    {596,375},
    {625,319},
    {660,375},
    {689,319},
    {721,375},
    {750,319},
    {410,315},
    {374,378},
    {352,315},
    {313,378},
    {294,315},
    {254,378},
    {236,315},
    {565,456},
    {596,512},
    {625,456},
    {660,512},
    {689,456},
    {721,512},
    {750,456},
    {410,451},
    {374,514},
    {352,451},
    {313,514},
    {294,451},
    {254,514},
    {236,451},
};
const LikabilityRepeatedRow rows[]{
    {variables[0],icons[0],LikabilityPoints(positions).subspan(0,7)},
    {variables[1],icons[1],LikabilityPoints(positions).subspan(7,7)},
    {variables[2],icons[0],LikabilityPoints(positions).subspan(14,7)},
    {variables[3],icons[1],LikabilityPoints(positions).subspan(21,7)},
    {variables[4],icons[0],LikabilityPoints(positions).subspan(28,7)},
    {variables[5],icons[1],LikabilityPoints(positions).subspan(35,7)},
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
    return {3,"chu-x-pet",background,{},controls,
        {1,1,{},false,false,false,nullptr,LikabilityCurrentTab::Disable,-1,-1,-1,{},{},nullptr},
        LikabilityRepeatedData{rows},0x4FACA0,0x4FB430};
}
}

#include "LikabilityData.h"

namespace KaGuYa::Start::Runtime::NeeChanNoSusumeOneeChanNoItazuraSeiseikatsuLikabilityData
{
namespace
{
// 原PE 0x4E71C0/0x4F93E0；边界和原字节见likability-profile-integration/tables/09.json。
// 独立静态资料，不读取全局profile，不替其它款补图或修改脚本值。
const char* const variables[]{
    "\x97\x95\x88\xDF\x81\x40\x8D\x44\x8A\xB4\x93\x78",
    "\x97\x95\x88\xDF\x81\x40\x92\xED\x88\xA4\x93\x78",
    "\x8E\x8D\x8E\x71\x81\x40\x8D\x44\x8A\xB4\x93\x78",
    "\x8E\x8D\x8E\x71\x81\x40\x95\xEA\x90\xAB\x93\x78",
    "\x90\xC2\x97\x74\x81\x40\x8D\x44\x8A\xB4\x93\x78",
    "\x90\xC2\x97\x74\x81\x40\x92\xED\x88\xA4\x93\x78",
    "\x8E\x9E\x89\x4A\x81\x40\x8D\x44\x8A\xB4\x93\x78",
};
const char* const icons[]{
    "\x8D\x44\x8A\xB4\x93\x78\x83\x6E\x81\x5B\x83\x67\x83\x7D\x81\x5B\x83\x4E\x2E\x76\x72\x6D",
};
const POINT positions[]{
    {410,109},
    {451,109},
    {492,109},
    {533,109},
    {574,109},
    {410,148},
    {451,148},
    {492,148},
    {533,148},
    {574,148},
    {410,228},
    {451,228},
    {492,228},
    {533,228},
    {574,228},
    {410,267},
    {451,267},
    {492,267},
    {533,267},
    {574,267},
    {919,109},
    {960,109},
    {1001,109},
    {1042,109},
    {1083,109},
    {919,148},
    {960,148},
    {1001,148},
    {1042,148},
    {1083,148},
    {919,228},
    {960,228},
    {1001,228},
    {1042,228},
    {1083,228},
    {919,267},
    {960,267},
    {1001,267},
    {1042,267},
    {1083,267},
    {410,445},
    {451,445},
    {492,445},
    {533,445},
    {574,445},
    {410,484},
    {451,484},
    {492,484},
    {533,484},
    {574,484},
    {410,564},
    {451,564},
    {492,564},
    {533,564},
    {574,564},
    {410,603},
    {451,603},
    {492,603},
    {533,603},
    {574,603},
    {919,445},
    {960,445},
    {1001,445},
    {1042,445},
    {1083,445},
    {919,484},
    {960,484},
    {1001,484},
    {1042,484},
    {1083,484},
};
const LikabilityRepeatedRow rows[]{
    {variables[0],icons[0],LikabilityPoints(positions).subspan(0,10)},
    {variables[1],icons[0],LikabilityPoints(positions).subspan(10,10)},
    {variables[2],icons[0],LikabilityPoints(positions).subspan(20,10)},
    {variables[3],icons[0],LikabilityPoints(positions).subspan(30,10)},
    {variables[4],icons[0],LikabilityPoints(positions).subspan(40,10)},
    {variables[5],icons[0],LikabilityPoints(positions).subspan(50,10)},
    {variables[6],icons[0],LikabilityPoints(positions).subspan(60,10)},
};
const LikabilityControlProfile controls[]{
    {{{1,{nullptr,nullptr,nullptr,"\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA\x5F\x45\x58\x49\x54\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D","\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA\x5F\x45\x58\x49\x54\x74\x61\x72\x67\x65\x74\x2E\x76\x72\x6D","\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA\x5F\x45\x58\x49\x54\x63\x6C\x69\x63\x6B\x2E\x76\x72\x6D",nullptr},nullptr},7,-1},true,-1,-1,false},
};
const char* const background[]{
    "\x8D\x44\x8A\xB4\x93\x78\x89\xE6\x96\xCA\x5F\x83\x78\x81\x5B\x83\x58\x2E\x76\x72\x6D",
};
}
LikabilityResourceProfile Profile() noexcept
{
    return {9,"nee-chan-no-susume-onee-chan-no-itazura-seiseikatsu",background,{},controls,
        {1,1,{},true,false,false,nullptr,LikabilityCurrentTab::Disable,-1,-1,-1,{},{},nullptr},
        LikabilityRepeatedData{rows},0x4E71C0,0x4F93E0};
}
}

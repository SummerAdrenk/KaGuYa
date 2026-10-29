#include "ProgramData.h"
#include <Runtime/ProgramDialogResourceData.h>
#include <Runtime/ProgramSystemDialogNative.h>

namespace KaGuYa::Start::Runtime::ProgramMain00NativeDetail::Sample18
{
extern const GuiStaticDescriptorNative24h initial[3], statics[31];
extern const GuiButtonDescriptorNative2Ch buttons[8];
extern const char* const params[18];
extern const char* const other[6];
extern const char* const resources[20];
extern const char* const markers[4];
extern const char* const digits[10];
extern const char* const time[2];
extern const POINT numberPoints[2], datePoints[3];
extern const std::int32_t events[36], extraEvents[3];
extern const GuiDialogNavigationMapping navigation0[9], navigation1[9];
}
namespace KaGuYa::Start::Runtime::MamatomoProgramData
{
namespace
{
const char* const bgm[]{"BGM16.bgm", "BGM16.bgm", "BGM16.bgm"};
template<class Source> auto Groups(Source source) noexcept
{
    std::array<ProgramMain01DialogNativeControlGroup, 4> out{};
    for (unsigned i=0;i<4;++i) out[i]={source[i].kindAt00,source[i].countAt04,source[i].descriptorsAt08};
    return out;
}
template<class Source> auto Routes(Source source) noexcept
{
    std::array<ProgramSystemRoute,6> out{};
    for (unsigned i=0;i<6;++i) out[i]={source[i].eventAt00,source[i].nameAt04};
    return out;
}
}
const ProgramResourceProfile& Profile() noexcept
{
    namespace D = ProgramMain00NativeDetail::Sample18;
    static const auto system0=Groups(ProgramSystem00DialogNativeGroupsSample18());
    static const auto system1=Groups(ProgramSystem01DialogNativeGroupsSample18());
    static const auto routes0=Routes(ProgramSystem00DialogNativeRoutesSample18());
    static const auto routes1=Routes(ProgramSystem01DialogNativeRoutesSample18());
    static const ProgramMain01DialogNativeControlGroup trainingGroups[]{{0,3,D::initial},{1,8,D::buttons},{0,31,D::statics}};
    static const ProgramMain01DialogNativeNavigationGroup trainingNavigation[]{{3,D::navigation0},{4,D::navigation1}};
    static const ProgramTrainingProfileView training{D::initial,D::buttons,D::statics,D::params,D::other,
        D::resources,D::markers,D::numberPoints,D::events,D::extraEvents,ProgramMain00DialogNativeRecordsSample18()};
    static const auto availability=[]
    {
        std::array<ProgramControlRule,19> out{};std::size_t i=0;
        for (const auto& r:ProgramMain01DialogNativeAvailabilitySample18())
        {
            out[i++]={r.nameAt00,nullptr,r.buttonAt04,ProgramRuleTest::Nonnegative,0,ProgramRuleTarget::State};
            for(int j=0;j<3;++j)out[i++]={r.nameAt00,nullptr,r.symbolsAt08[j],ProgramRuleTest::Equal,j,ProgramRuleTarget::State};
        }
        const auto& r=ProgramMain01DialogNativeLastAvailabilitySample18();
        out[i++]={r.nameAt00,nullptr,r.buttonAt04,ProgramRuleTest::Nonnegative,0,ProgramRuleTarget::State};
        for(int j=0;j<2;++j)out[i++]={r.nameAt00,nullptr,r.symbolsAt08[j],ProgramRuleTest::Equal,j,ProgramRuleTarget::State};
        return out;
    }();
    static const ProgramChoiceProfile tripChoices[]{{17,0,false,{},{}},{18,1,false,{},{}},{19,2,false,{},{}},
        {20,3,false,{},{}},{21,4,false,{},{}},{22,5,false,{},{}}};
    static const ProgramModeProfile modes[]{
        {0,ProgramDialogImplementation::MamatomoTraining,
            {nullptr,trainingGroups,trainingNavigation,{},nullptr,{}},
            {{},D::time,D::digits,true,{{D::datePoints[0],{0,0},D::datePoints[1],D::datePoints[2]}},true,true},
            {{nullptr,system0,{},{},nullptr,{}},{49,51},54,55,routes0},{},{},bgm,&training},
        {1,ProgramDialogImplementation::MamatomoTrip,
            {ProgramMain01DialogNativeBackgroundSample18(),ProgramMain01DialogNativeGroupsSample18(),
             ProgramMain01DialogNativeNavigationSample18(),availability,nullptr,{}},
            {{},{ProgramMain01DialogNativeTimeNamesSample18(),2},ProgramMain01DialogNativeDigitsSample18(),true,
             {{{11,27},{0,0},{79,27},{55,27}}},true,true},
            {{nullptr,system1,{},{},nullptr,{}},{24,26},29,30,routes1},{},tripChoices,bgm,nullptr}};
    static const ProgramResourceProfile profile{ProgramModeSelection::NamedParameter,"prog_param00",modes};
    return profile;
}
}

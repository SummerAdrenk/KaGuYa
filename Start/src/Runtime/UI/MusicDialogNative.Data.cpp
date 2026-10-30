#include "MusicDialogNative.Internal.h"
#include <Runtime/GameBuildProfile.h>
#include <limits>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::MusicDialogNativeDetail
{
namespace
{
#define MUSIC_PREFIX "\x89\xB9\x8A\x79\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA" "_"
constexpr char base[] = MUSIC_PREFIX "\x83\x78\x81\x5B\x83\x58.vrm";
constexpr char frame[] = MUSIC_PREFIX "\x98\x67.vrm";
constexpr char title[] = MUSIC_PREFIX "\x83\x5E\x83\x43\x83\x67\x83\x8B.vrm";
constexpr char off[] = MUSIC_PREFIX "\x83\x49\x83\x74.vrm";
constexpr char offTarget[] = MUSIC_PREFIX "\x83\x49\x83\x74\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char offClick[] = MUSIC_PREFIX "\x83\x49\x83\x74\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr char selection[] = MUSIC_PREFIX "\x91\x49\x91\xF0\x94\xCD\x88\xCD.vrm";
constexpr char on[] = MUSIC_PREFIX "\x83\x49\x83\x93.vrm";
constexpr char onTarget[] = MUSIC_PREFIX "\x83\x49\x83\x93\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char onClick[] = MUSIC_PREFIX "\x83\x49\x83\x93\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
#undef MUSIC_PREFIX
constexpr const char* backgrounds[]{base, frame, title};
constexpr POINT positions[]{
    {162,139},{685,139},{148,194},{671,194},{134,249},{657,249},
    {120,305},{643,305},{106,361},{629,361},{92,416},{615,416},
    {78,472},{601,472},{64,528},{587,528},{50,583},{573,583}};
constexpr Descriptor checkbox{2,
    {off,nullptr,nullptr,offTarget,offTarget,offClick,selection},
    {on,nullptr,nullptr,onTarget,onTarget,onClick,selection},nullptr,6,-1};
constexpr Pair horizontal[]{
    {2,3},{3,2},{4,5},{5,4},{6,7},{7,6},{8,9},{9,8},{10,11},
    {11,10},{12,13},{13,12},{14,15},{15,14},{16,17},{17,16},{18,19},{19,18},{-1,0}};
constexpr Pair up[]{
    {2,18},{3,19},{4,2},{5,3},{6,4},{7,5},{8,6},{9,7},{10,8},
    {11,9},{12,10},{13,11},{14,12},{15,13},{16,14},{17,15},{18,16},{19,17},{-1,0}};
constexpr Pair down[]{
    {2,4},{3,5},{4,6},{5,7},{6,8},{7,9},{8,10},{9,11},{10,12},
    {11,13},{12,14},{13,15},{14,16},{15,17},{16,18},{17,19},{18,2},{19,3},{-1,0}};
constexpr OperationRecord operations[]{
    {3,horizontal,-1,0},{4,horizontal,-1,0},{1,up,15,2},{2,down,15,2}};
constexpr const char* songs[]{"BGM01.bgm","BGM02.bgm","BGM03.bgm","BGM04.bgm","BGM05.bgm","BGM06.bgm",
    "BGM07.bgm","BGM08.bgm","BGM09.bgm","BGM10.bgm","BGM11.bgm","BGM12.bgm",
    "BGM13.bgm","BGM14.bgm","BGM15.bgm","BGM16.bgm","BGM17.bgm","BGM18.bgm"};
const MusicResourceProfile mamatomo{backgrounds,checkbox,positions,operations,songs,
    nullptr,MusicResourceProfile::Presentation::TiledCheckboxes,{}};
}
const POINT* Positions() { return RequireMusicDialogNativeServices().resources.positions.data(); }
const char* BackgroundName(std::uint32_t i) { return RequireMusicDialogNativeServices().resources.backgrounds[i]; }
const Descriptor& CheckTemplate() { return RequireMusicDialogNativeServices().resources.checkbox; }
std::span<const OperationRecord> Operations() { return RequireMusicDialogNativeServices().resources.navigation; }
std::int32_t ItemCount()
{
    const auto& data=RequireMusicDialogNativeServices().resources;
    const bool individual=data.presentation==MusicResourceProfile::Presentation::IndividualStateImages;
    const bool validLayout=individual
        ? data.backgrounds.size()==1&&data.positions.empty()&&data.trackButtons.size()==data.songs.size()
        : data.backgrounds.size()==3&&data.positions.size()==data.songs.size()&&data.trackButtons.empty();
    if(!validLayout || data.checkbox.eventAt00<0
        || data.songs.size()>static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max()-data.checkbox.eventAt00))
        throw std::invalid_argument("Music profile positions/songs/control IDs are inconsistent");
    return static_cast<std::int32_t>(data.songs.size());
}
}
namespace KaGuYa::Start::Runtime
{
const MusicResourceProfile& MamatomoMusicResourceProfile() noexcept
{ return MusicDialogNativeDetail::mamatomo; }
const MusicResourceProfile& GameMusicResourceProfile() noexcept
{
    const auto* configured=CompiledGameProfile().musicResources;
    return configured?*configured:MamatomoMusicResourceProfile();
}
}

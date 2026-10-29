#include "MusicSceneNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::uint32_t table[]{0x4C3630,0x4A6DA0,0x4B8C90,0x4B5E20};
constexpr std::uint32_t ctorUnwind[]{0xFFFFFFFF,0x59CB70,0,0x59CB78,1,0x59CB83,2,0x59CB8E,3,0x59CB99,3,0x59CBA8,3,0x541E55};
constexpr std::uint32_t updateUnwind[]{0xFFFFFFFF,0x59E6A0,0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x59E6A8};
constexpr std::uint32_t deleteUnwind[]{0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x541E55};
const MusicSceneNativeEhEvidence eh[]{
    {0x4A0730,0x5EFD5C,{0x19930522,7,0x5EFD80,0,0,0,0,0,1},ctorUnwind},
    {0x4B8C90,0x5EFDD0,{0x19930522,3,0x5EFDB8,0,0,0,0,0,1},updateUnwind},
    {0x4A6DA0,0x5EFE0C,{0x19930522,3,0x5EFDF4,0,0,0,0,0,5},deleteUnwind}};
}
std::span<const std::uint32_t,4> MusicSceneNativeOriginalTableSample18() noexcept{return table;}
std::span<const MusicSceneNativeEhEvidence,3> MusicSceneNativeEhSample18() noexcept{return eh;}
namespace MusicSceneNativeDetail
{
const char* BlackName() noexcept{return "bg_black.bgd";}
const char* SettingName() noexcept{return "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";}
const char* CgName() noexcept{return "\x82\x62\x82\x66\x95\x5C\x8E\xA6\x83\x82\x81\x5B\x83\x68";}
const char* SceneName() noexcept{return "\x83\x56\x81\x5B\x83\x93\x8D\xC4\x90\xB6\x83\x82\x81\x5B\x83\x68";}
const char* TitleName() noexcept{return "\x83\x5E\x83\x43\x83\x67\x83\x8B";}
}
}

#include <Runtime/NativeSerialize18.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::array<unsigned,29> baseTable{
    0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,
    0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,
    0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x541C5F,
    0x541C5F,0x541C5F,0x541C5F,0x541C5F,0x469A00};
constexpr std::array<unsigned,31> concreteTable{
    0x46D150,0x47F910,0x47D2D0,0x48D060,0x47F0A0,0x479C80,0x465E80,0x466110,
    0x4661B0,0x466000,0x466250,0x465F90,0x4662C0,0x4660A0,0x465D00,0x465E00,
    0x465C00,0x465D80,0x465B80,0x465ED0,0x465B20,0x4663A0,0x465C80,0x466380,
    0x4659F0,0x488C10,0x480550,0x47CA00,0x469420,0x480500,0x480490};
constexpr std::array<unsigned,9> fiSave{0x19930522,1,0x5E4E68,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiLoad{0x19930522,1,0x5E4E94,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiStrings{0x19930522,2,0x5EB77C,1,0x5EB78C,0,0,0,1};
constexpr std::array<unsigned,9> fiStringCopy{0x19930522,1,0x5E23F0,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiSound{0x19930522,2,0x5E7DE0,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiCharacter{0x19930522,5,0x5E76D0,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiCharacters{0x19930522,2,0x5E7790,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiTypes{0x19930522,2,0x5E7278,0,0,0,0,0,1};
constexpr unsigned uSave[]{0xFFFFFFFF,0x59A980},uLoad[]{0xFFFFFFFF,0x59A100};
constexpr unsigned uStrings[]{0xFFFFFFFF,0,0xFFFFFFFF,0},tStrings[]{0,0,1,1,0x5EB76C},hStrings[]{0x40,0,0,0x456FB7};
constexpr unsigned uStringCopy[]{0xFFFFFFFF,0x590E00};
constexpr unsigned uSound[]{0xFFFFFFFF,0x59ACA0,0xFFFFFFFF,0x541E55};
constexpr unsigned uCharacter[]{0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x59AD60,1,0x59AD68,1,0x541E55,0xFFFFFFFF,0x541E55};
constexpr unsigned uCharacters[]{0xFFFFFFFF,0x59AD90,0xFFFFFFFF,0x541E55},uTypes[]{0xFFFFFFFF,0x59ADC0,0xFFFFFFFF,0x541E55};
const NativeSerialize18EhEvidence evidence[]{
    {0x488C10,0x5E4E70,fiSave,uSave,{},{}},{0x480550,0x5E4E9C,fiLoad,uLoad,{},{}},
    {0x456E80,0x5EB7A0,fiStrings,uStrings,tStrings,hStrings},{0x40AE60,0x5E23F8,fiStringCopy,uStringCopy,{},{}},
    {0x48B750,0x5E7DF0,fiSound,uSound,{},{}},{0x48C200,0x5E76AC,fiCharacter,uCharacter,{},{}},
    {0x48C4A0,0x5E77A0,fiCharacters,uCharacters,{},{}},{0x48C6F0,0x5E7288,fiTypes,uTypes,{},{}}};
}
std::span<const unsigned,29> NativeISerializeOriginalTableSample18() noexcept{return baseTable;}
std::span<const unsigned,31> NativeSerialize18OriginalTableSample18() noexcept{return concreteTable;}
std::span<const NativeSerialize18EhEvidence> NativeSerialize18EhSample18() noexcept{return evidence;}
}

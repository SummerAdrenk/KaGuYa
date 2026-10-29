#include <Runtime/SettingDialogNativeConfirm.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
namespace
{
using W=SettingDialogNativeConfirmDefaultWrite;using K=SettingDialogNativeConfirmWriteKind;
// 相邻相同site才是原同一次capture的连续写，其余不能折成最终值矩阵。
constexpr W prefix[]{
    {0x538BDA,1,0x60,1,K::Byte},{0x538C29,1,0x61,1,K::Byte},
    {0x538C78,1,0x38,0xFFFFFFEF,K::And},{0x538CC7,1,0x38,0xFFFFFFFB,K::And},
    {0x538D16,1,0x6C,1,K::Byte},{0x538D65,1,0x38,0xFFFFFFFE,K::And},
    {0x538DB4,1,0x38,1,K::Or},{0x538E03,1,0x38,0xFFFFFFF7,K::And},
    {0x538E52,1,0x38,0xFFFFFFFD,K::And},{0x538EA1,1,0x38,0xFFFFFFDF,K::And},
    {0x538EF0,1,0x34,2,K::Dword},{0x538F42,1,0x3C,1,K::Dword},
    {0x5390C4,1,0x7A,0x100,K::Word},{0x5390C4,1,0x77,0x101,K::Word},
    {0x5390C4,1,0x79,0,K::Byte},{0x5390C4,1,0x7C,0x1010101,K::Dword},
    {0x5390C4,1,0x80,1,K::Byte},{0x53912D,1,0x8C,1,K::Dword},
    {0x539182,1,0x74,0x101,K::Word},{0x539182,1,0x76,1,K::Byte}};
constexpr W tail[]{
    {0x5392C4,1,4,0,K::Byte},{0x539313,1,0x54,0,K::Byte},
    {0x53945C,1,0x6F,1,K::Byte},{0x5394AB,1,0x70,2,K::Dword},
    {0x5394FD,7,0x10,0xFFFF,K::Word},{0x539551,7,0x12,0xFFFF,K::Word},
    {0x5395A0,7,0x16,0,K::Word},{0x5395F1,7,0x18,1,K::Word},
    {0x539645,7,0x1A,0,K::Word},{0x539696,7,0x1C,0,K::Word}};

constexpr std::array<unsigned,9> fiMode{0x19930522,3,0x5F759C,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiDefaults{0x19930522,3,0x5F75D8,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiName{0x19930522,7,0x5F7638,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiReset{0x19930522,9,0x5F7694,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiModal{0x19930522,5,0x5FD2F0,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiGeneral{0x19930522,15,0x5F49E4,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiNameCtor{0x19930522,11,0x5FCD0C,0,0,0,0,0,1};
constexpr std::array<unsigned,9> fiNameText{0x19930522,8,0x5F99D4,1,0x5F999C,0,0,0,1};
constexpr unsigned uMode[]{0xFFFFFFFF,0x5A74E0,0xFFFFFFFF,0x5A74F2,0xFFFFFFFF,0x541E55};
constexpr unsigned uDefaults[]{0xFFFFFFFF,0x5A73A0,0xFFFFFFFF,0x5A73B2,0xFFFFFFFF,0x541E55};
constexpr unsigned uName[]{0xFFFFFFFF,0x5A7910,0xFFFFFFFF,0x5A7922,1,0x5A792A,2,0x5A7932,2,0x541E55,1,0x541E55,0xFFFFFFFF,0x541E55};
constexpr unsigned uReset[]{0xFFFFFFFF,0x5A9490,0xFFFFFFFF,0x5A9498,0xFFFFFFFF,0x541E55,
    0xFFFFFFFF,0x5A94A0,0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x5A94A8,0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x5A94B0,0xFFFFFFFF,0x541E55};
constexpr unsigned uModal[]{0xFFFFFFFF,0x5A5A70,0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x5A5A78,0xFFFFFFFF,0x5A5A80,0xFFFFFFFF,0x541E55};
constexpr unsigned uGeneral[]{0xFFFFFFFF,0x5A18A0,0,0x5A18A8,1,0x5A18B0,1,0x541E55,0,0x541E55,
    0,0x5A18B8,5,0x5A18C0,6,0x5A18C8,6,0x541E55,6,0x5A18D0,6,0x541E55,5,0x541E55,0,0x541E55,0,0x5A18D8,0,0x541E55};
constexpr unsigned uNameCtor[]{0xFFFFFFFF,0x5A19C0,0,0x5A19C8,1,0x5A19D0,2,0x5A19D8,2,0x541E55,
    2,0x5A19E0,2,0x541E55,1,0x541E55,0,0x541E55,0,0x5A19E8,0,0x541E55};
constexpr unsigned uNameText[]{0xFFFFFFFF,0x5A68F0,0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x5A68FB,0xFFFFFFFF,0x541E55,
    0xFFFFFFFF,0,4,0x5A6906,5,0x5A690E,0xFFFFFFFF,0};
constexpr unsigned tNameText[]{4,6,7,1,0x5F998C},hNameText[]{0x40,0,0,0x51252F};
const SettingDialogNativeConfirmEhEvidence evidence[]{
    {0x51E4B0,0x5F75B4,fiMode,uMode,{},{}},{0x51B730,0x5F75F0,fiDefaults,uDefaults,{},{}},
    {0x521AD0,0x5F7614,fiName,uName,{},{}},{0x538BB0,0x5F7670,fiReset,uReset,{},{}},
    {0x5071D0,0x5FD2CC,fiModal,uModal,{},{}},{0x4E3330,0x5F49C0,fiGeneral,uGeneral,{},{}},
    {0x4E4340,0x5FCCE8,fiNameCtor,uNameCtor,{},{}},{0x512200,0x5F99B0,fiNameText,uNameText,tNameText,hNameText}};
}
std::span<const SettingDialogNativeConfirmDefaultWrite> SettingDialogNativeConfirmDefaultPrefixSample18() noexcept{return prefix;}
std::span<const SettingDialogNativeConfirmDefaultWrite> SettingDialogNativeConfirmDefaultTailSample18() noexcept{return tail;}
std::span<const SettingDialogNativeConfirmEhEvidence> SettingDialogNativeConfirmEhSample18() noexcept{return evidence;}
}

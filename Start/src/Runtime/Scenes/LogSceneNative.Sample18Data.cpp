#include <Runtime/LogSceneNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::uint32_t table[]{0x4C3630,0x4A6BA0,0x4B6900,0x4B5E20};
constexpr std::uint32_t control[]{0x4C60D0,0x4A5EE0};
constexpr std::array<std::uint32_t,9> ctor{0x19930522,3,0x5EF490,0,0,0,0,0,1};
constexpr std::array<std::uint32_t,9> destroy{0x19930522,1,0x5EF4CC,0,0,0,0,0,5};
constexpr std::array<std::uint32_t,9> reference{0x19930522,0,0,0,0,0,0,0,5};
constexpr std::uint32_t ctorUnwind[]{0xFFFFFFFF,0x59C890,0,0x59C898,1,0x59C8A3};
constexpr std::uint32_t destroyUnwind[]{0xFFFFFFFF,0x541E55};
const LogSceneNativeEhEvidence eh[]{
    {0x49E180,0x5EF4A8,ctor,ctorUnwind},
    {0x4A6BA0,0x5EF4D4,destroy,destroyUnwind},
    {0x4A3540,0x5EF46C,reference,{}}};
}
std::span<const std::uint32_t,4> LogSceneNativeOriginalTableSample18() noexcept{return table;}
std::span<const std::uint32_t,2> LogSceneNativeControlOriginalTableSample18() noexcept{return control;}
std::span<const LogSceneNativeEhEvidence,3> LogSceneNativeEhSample18() noexcept{return eh;}
}

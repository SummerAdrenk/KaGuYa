#pragma once
#include <Runtime/SecretCgNativeArchive.h>
#include <Windows.h>
#include <cstddef>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct CgSelectionPairNative8h final
{
    std::int32_t ordinalAt00;
    std::int32_t resourceIdAt04;
};
using CgSelectionPairVector = EnvironmentVectorStorage<CgSelectionPairNative8h>;
// CModeCGdisp+18..37：五个DWORD后跟唯一pair vector，不附availability影子表。
struct CgSelectionNativeBody20h final
{
    std::int32_t countAt00;
    // 4B745E/4AD780读入场景+1C；不是已证差分序号，当前项另在+28。
    std::int32_t valueAt04;
    POINT positionAt08;
    std::int32_t selectedPairAt10;
    CgSelectionPairVector pairsAt14;
};
static_assert(sizeof(CgSelectionPairNative8h)==8&&sizeof(CgSelectionPairVector)==12);
static_assert(sizeof(CgSelectionNativeBody20h)==0x20&&alignof(CgSelectionNativeBody20h)==4);
static_assert(offsetof(CgSelectionNativeBody20h,positionAt08)==8);
static_assert(offsetof(CgSelectionNativeBody20h,selectedPairAt10)==0x10);
static_assert(offsetof(CgSelectionNativeBody20h,pairsAt14)==0x14);
static_assert(std::is_standard_layout_v<CgSelectionNativeBody20h>);

// 4AD780只写count/POINT/value；拒绝时所有输出原样不动，不分配或增持。
// category比较有符号，item比较无符号；两个Params捕获必须保持独立。
bool ReadCgSelectionNativeAt4AD780(const SecretCgNativeArchive&,
    ISecretCgParamsRootSource&,std::int32_t category,std::uint32_t item,
    std::int32_t* count,POINT* position,std::int32_t* value);
void ResetCgSelectionNativeAt4B7433(CgSelectionNativeBody20h&) noexcept;
CgSelectionPairNative8h* InsertCgSelectionPairNativeAt499780(CgSelectionPairVector&,
    CgSelectionPairNative8h* position,const CgSelectionPairNative8h&,
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
void AppendCgSelectionPairNativeAt4B7584(CgSelectionPairVector&,
    const CgSelectionPairNative8h&,IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
// 普通叶不额外加noexcept；析构/EH调用方自行保持原终止资格。
void DestroyCgSelectionNativeAt4A4F40(CgSelectionNativeBody20h&,
    IEnvironmentRootMemory& = DefaultEnvironmentRootMemory());
}

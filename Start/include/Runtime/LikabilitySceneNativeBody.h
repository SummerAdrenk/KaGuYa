#pragma once
#include <Runtime/EnvironmentObjectReference.h>

namespace KaGuYa::Start::Runtime
{
struct LikabilitySceneNativeBody20h;
struct LikabilitySceneNativeVtable4 final
{
    std::uint8_t(__thiscall* runAt00)(LikabilitySceneNativeBody20h*,std::int32_t);
    LikabilitySceneNativeBody20h*(__thiscall* deleteAt04)(LikabilitySceneNativeBody20h*,std::uint32_t);
    void(__thiscall* updateAt08)(LikabilitySceneNativeBody20h*);
    void(__thiscall* drawAt0C)(LikabilitySceneNativeBody20h*);
};
// 原49DFB0/5B5810；+0C/+1C构造未写，不以现代shared_ptr/phase副本替代。
struct LikabilitySceneNativeBody20h final
{
    const LikabilitySceneNativeVtable4* vptrAt00;
    std::uint32_t phaseAt04;
    std::uint32_t savedModeAt08;
    std::uint32_t unwrittenAt0C;
    union{YtlReferenceSlotRuntime dialogAt10;};
    std::uint32_t unwrittenAt1C;
    LikabilitySceneNativeBody20h()noexcept{}
    ~LikabilitySceneNativeBody20h()noexcept{}
};
static_assert(sizeof(LikabilitySceneNativeVtable4)==0x10);
static_assert(sizeof(LikabilitySceneNativeBody20h)==0x20&&alignof(LikabilitySceneNativeBody20h)==4);
static_assert(std::is_standard_layout_v<LikabilitySceneNativeBody20h>);
static_assert(offsetof(LikabilitySceneNativeBody20h,phaseAt04)==4);
static_assert(offsetof(LikabilitySceneNativeBody20h,savedModeAt08)==8);
static_assert(offsetof(LikabilitySceneNativeBody20h,unwrittenAt0C)==0x0C);
static_assert(offsetof(LikabilitySceneNativeBody20h,dialogAt10)==0x10);
static_assert(offsetof(LikabilitySceneNativeBody20h,unwrittenAt1C)==0x1C);
}

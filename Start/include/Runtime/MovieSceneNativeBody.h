#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct MovieSceneNativeBody0Ch;
struct MovieSceneNativeVtable4 final
{
    std::uint8_t(__thiscall* runAt00)(MovieSceneNativeBody0Ch*,std::int32_t);
    MovieSceneNativeBody0Ch*(__thiscall* deleteAt04)(MovieSceneNativeBody0Ch*,std::uint32_t);
    void(__thiscall* updateAt08)(MovieSceneNativeBody0Ch*);
    void(__thiscall* drawAt0C)(MovieSceneNativeBody0Ch*);
};

// 4A0B00只写虚表和savedModeAt08；+04不是服务指针或播放阶段，不得初始化。
struct MovieSceneNativeBody0Ch final
{
    const MovieSceneNativeVtable4* vptrAt00;
    std::uint32_t unwrittenAt04;
    std::uint32_t savedModeAt08;
    MovieSceneNativeBody0Ch()noexcept{}
    ~MovieSceneNativeBody0Ch()noexcept{}
};
static_assert(sizeof(void*)==4&&sizeof(MovieSceneNativeBody0Ch)==0x0C);
static_assert(sizeof(MovieSceneNativeVtable4)==0x10&&alignof(MovieSceneNativeBody0Ch)==4);
static_assert(offsetof(MovieSceneNativeVtable4,runAt00)==0);
static_assert(offsetof(MovieSceneNativeVtable4,deleteAt04)==4);
static_assert(offsetof(MovieSceneNativeVtable4,updateAt08)==8);
static_assert(offsetof(MovieSceneNativeVtable4,drawAt0C)==0x0C);
static_assert(std::is_standard_layout_v<MovieSceneNativeBody0Ch>);
static_assert(offsetof(MovieSceneNativeBody0Ch,vptrAt00)==0);
static_assert(offsetof(MovieSceneNativeBody0Ch,unwrittenAt04)==4);
static_assert(offsetof(MovieSceneNativeBody0Ch,savedModeAt08)==8);
}

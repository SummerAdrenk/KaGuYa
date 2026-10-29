#pragma once
#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct ReplaySceneNativeBody80h;
struct ReplaySceneNativeVtable4 final
{
    std::uint8_t(__thiscall* runAt00)(ReplaySceneNativeBody80h*,std::int32_t);
    ReplaySceneNativeBody80h*(__thiscall* deleteAt04)(ReplaySceneNativeBody80h*,std::uint32_t);
    void(__thiscall* updateAt08)(ReplaySceneNativeBody80h*);
    void(__thiscall* drawAt0C)(ReplaySceneNativeBody80h*);
};
// 49EC90/5B52DC；阶段、保存mode、route/item/segment按原站点写，不使用optional或宿主状态副本。
struct ReplaySceneNativeBody80h final
{
    const ReplaySceneNativeVtable4* vptrAt00;
    std::uint32_t phaseAt04;
    std::uint32_t savedModeAt08;
    std::int32_t groupAt0C;
    std::int32_t routeAt10;
    std::int32_t itemAt14;
    std::int32_t segmentAt18;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> groupsAt1C;
    union { TransitOperation operationAt28; };
    std::int32_t stepAt58;
    union { YtlReferenceSlotRuntime blackAt5C; };
    union { YtlReferenceSlotRuntime capturedAt68; };
    union { YtlReferenceSlotRuntime auxiliaryAt74; };
    ReplaySceneNativeBody80h()noexcept{}
    ~ReplaySceneNativeBody80h()noexcept{}
};
static_assert(sizeof(ReplaySceneNativeBody80h)==0x80&&alignof(ReplaySceneNativeBody80h)==4);
static_assert(std::is_standard_layout_v<ReplaySceneNativeBody80h>);
static_assert(sizeof(ReplaySceneNativeVtable4)==0x10);
static_assert(offsetof(ReplaySceneNativeBody80h,phaseAt04)==4);
static_assert(offsetof(ReplaySceneNativeBody80h,savedModeAt08)==8);
static_assert(offsetof(ReplaySceneNativeBody80h,groupAt0C)==0x0C);
static_assert(offsetof(ReplaySceneNativeBody80h,routeAt10)==0x10);
static_assert(offsetof(ReplaySceneNativeBody80h,itemAt14)==0x14);
static_assert(offsetof(ReplaySceneNativeBody80h,segmentAt18)==0x18);
static_assert(offsetof(ReplaySceneNativeBody80h,groupsAt1C)==0x1C);
static_assert(offsetof(ReplaySceneNativeBody80h,operationAt28)==0x28);
static_assert(offsetof(ReplaySceneNativeBody80h,stepAt58)==0x58);
static_assert(offsetof(ReplaySceneNativeBody80h,blackAt5C)==0x5C);
static_assert(offsetof(ReplaySceneNativeBody80h,capturedAt68)==0x68);
static_assert(offsetof(ReplaySceneNativeBody80h,auxiliaryAt74)==0x74);
}

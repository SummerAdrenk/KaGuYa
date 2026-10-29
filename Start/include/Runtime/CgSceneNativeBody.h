#pragma once
#include <Runtime/CgSelectionNative.h>
#include <Runtime/TransitRuntime.h>

namespace KaGuYa::Start::Runtime
{
struct CgSceneNativeBodyB4h;
struct CgSceneNativeVtable4 final
{
    std::uint8_t(__thiscall* runAt00)(CgSceneNativeBodyB4h*,std::int32_t);
    CgSceneNativeBodyB4h*(__thiscall* deleteAt04)(CgSceneNativeBodyB4h*,std::uint32_t);
    void(__thiscall* updateAt08)(CgSceneNativeBodyB4h*);
    void(__thiscall* drawAt0C)(CgSceneNativeBodyB4h*);
};
// 49E3B0/5B5224：对象的唯一原生正文，08和padding没有已证明的构造写入。
struct CgSceneNativeBodyB4h final
{
    const CgSceneNativeVtable4* vptrAt00;
    std::uint32_t phaseAt04;
    std::uint32_t unwrittenAt08;
    std::uint8_t displayAt0C;
    std::uint8_t unwrittenAt0D[3];
    std::int32_t groupAt10;
    std::int32_t routeAt14;
    CgSelectionNativeBody20h selectionAt18;
    EnvironmentVectorStorage<YtlReferenceSlotRuntime> groupsAt38;
    union{TransitOperation operationAt44;};
    std::int32_t stepAt74;
    union{YtlReferenceSlotRuntime blackAt78;};
    union{YtlReferenceSlotRuntime capturedAt84;};
    union{YtlReferenceSlotRuntime auxiliaryAt90;};
    union{YtlReferenceSlotRuntime displayWindowAt9C;};
    union{YtlReferenceSlotRuntime operationWindowAtA8;};
    CgSceneNativeBodyB4h() noexcept{}
    ~CgSceneNativeBodyB4h() noexcept{}
};
static_assert(sizeof(CgSceneNativeVtable4)==0x10);
static_assert(sizeof(CgSceneNativeBodyB4h)==0xB4&&alignof(CgSceneNativeBodyB4h)==4);
static_assert(std::is_standard_layout_v<CgSceneNativeBodyB4h>);
static_assert(offsetof(CgSceneNativeBodyB4h,unwrittenAt08)==8);
static_assert(offsetof(CgSceneNativeBodyB4h,displayAt0C)==0x0C);
static_assert(offsetof(CgSceneNativeBodyB4h,groupAt10)==0x10);
static_assert(offsetof(CgSceneNativeBodyB4h,routeAt14)==0x14);
static_assert(offsetof(CgSceneNativeBodyB4h,selectionAt18)==0x18);
static_assert(offsetof(CgSceneNativeBodyB4h,groupsAt38)==0x38);
static_assert(offsetof(CgSceneNativeBodyB4h,operationAt44)==0x44);
static_assert(offsetof(CgSceneNativeBodyB4h,stepAt74)==0x74);
static_assert(offsetof(CgSceneNativeBodyB4h,blackAt78)==0x78);
static_assert(offsetof(CgSceneNativeBodyB4h,capturedAt84)==0x84);
static_assert(offsetof(CgSceneNativeBodyB4h,auxiliaryAt90)==0x90);
static_assert(offsetof(CgSceneNativeBodyB4h,displayWindowAt9C)==0x9C);
static_assert(offsetof(CgSceneNativeBodyB4h,operationWindowAtA8)==0xA8);
}

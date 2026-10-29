#pragma once

#include <Runtime/DrawLayerReferenceArray.h>

namespace KaGuYa::Start::Runtime
{
struct PatternLayerNativeBody80h;
struct PatternLayerNativeRenderServices;
class IDrawManagerNativeRootSource;
class IGraphicsScreenCalls;
struct MoveEffectNativeBody24h;

struct MoveEffectNativeVtable final
{
    std::uint8_t(__thiscall* tickAt00)(MoveEffectNativeBody24h*);
    void(__thiscall* drawAt04)(MoveEffectNativeBody24h*);
    std::uint8_t(__thiscall* readyAt08)(MoveEffectNativeBody24h*);
    MoveEffectNativeBody24h*(__thiscall* deleteAt0C)(MoveEffectNativeBody24h*, std::uint32_t);
};

// 49C12C..49C16E/5B4D50：四槽；+05..07不写，vector元素是Ref12。
struct MoveEffectNativeBody24h final
{
    const MoveEffectNativeVtable* vptrAt00;
    std::uint8_t readyAt04;
    std::uint8_t unwrittenAt05[3];
    DrawLayerReferenceArray itemsAt08;
    RECT viewportAt14;
};
static_assert(sizeof(MoveEffectNativeVtable) == 0x10);
static_assert(sizeof(MoveEffectNativeBody24h) == 0x24);
static_assert(offsetof(MoveEffectNativeBody24h, itemsAt08) == 8);
static_assert(offsetof(MoveEffectNativeBody24h, viewportAt14) == 0x14);

// 49C33B：0/4/18/1C/20/24在后续构造步骤才写，不能默认清零整块。
struct MoveEffectItemNativeBody3Ch final
{
    float currentXAt00;
    float currentYAt04;
    std::int32_t targetXAt08;
    std::int32_t targetYAt0C;
    std::int32_t finalXAt10;
    std::int32_t finalYAt14;
    float stepXAt18;
    float stepYAt1C;
    std::uint32_t directionsAt20;
    PatternLayerNativeBody80h* patternAt24;
    union { YtlReferenceSlotRuntime snapshotAt28; };
    std::int32_t offsetXAt34;
    std::int32_t offsetYAt38;
    MoveEffectItemNativeBody3Ch() noexcept {}
    ~MoveEffectItemNativeBody3Ch() noexcept {}
    MoveEffectItemNativeBody3Ch(const MoveEffectItemNativeBody3Ch&) = delete;
    MoveEffectItemNativeBody3Ch& operator=(const MoveEffectItemNativeBody3Ch&) = delete;
};
static_assert(sizeof(MoveEffectItemNativeBody3Ch) == 0x3C);
static_assert(offsetof(MoveEffectItemNativeBody3Ch, targetXAt08) == 8);
static_assert(offsetof(MoveEffectItemNativeBody3Ch, finalXAt10) == 0x10);
static_assert(offsetof(MoveEffectItemNativeBody3Ch, stepXAt18) == 0x18);
static_assert(offsetof(MoveEffectItemNativeBody3Ch, directionsAt20) == 0x20);
static_assert(offsetof(MoveEffectItemNativeBody3Ch, patternAt24) == 0x24);
static_assert(offsetof(MoveEffectItemNativeBody3Ch, snapshotAt28) == 0x28);
static_assert(offsetof(MoveEffectItemNativeBody3Ch, offsetXAt34) == 0x34);

struct MoveEffectNativeServices final
{
    IDrawManagerNativeRootSource& draw;
    PatternLayerNativeRenderServices& pattern;
    IGraphicsScreenCalls& screen;
};
const MoveEffectNativeVtable& MoveEffectNativeMethods() noexcept;
const MoveEffectNativeVtable& MoveEffectBaseNativeMethods() noexcept;
std::uint8_t TickMoveEffectNativeAt4BB960(MoveEffectNativeBody24h& body);
void DrawMoveEffectNativeAt4B5550(MoveEffectNativeBody24h& body, MoveEffectNativeServices& services);
void DestroyMoveEffectNativeAt4A4280(MoveEffectNativeBody24h& body, MoveEffectNativeServices& services) noexcept;
MoveEffectNativeBody24h* DeleteMoveEffectNativeAt4A65C0(MoveEffectNativeBody24h& body,
    std::uint32_t flags, MoveEffectNativeServices& services) noexcept;

// 供原四槽调用借用外部服务，不向24h正文增加拥有者或状态副本。
class MoveEffectNativeScope final
{
public:
    explicit MoveEffectNativeScope(MoveEffectNativeServices&) noexcept;
    ~MoveEffectNativeScope() noexcept;
    MoveEffectNativeScope(const MoveEffectNativeScope&) = delete;
    MoveEffectNativeScope& operator=(const MoveEffectNativeScope&) = delete;
private:
    MoveEffectNativeServices* previous_;
};
}

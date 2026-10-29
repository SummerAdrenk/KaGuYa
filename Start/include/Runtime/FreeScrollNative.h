#pragma once

#include <Runtime/DrawLayerReferenceArray.h>

namespace KaGuYa::Start::Runtime
{
struct PatternLayerNativeBody80h;
class IGraphicsScreenSizeApi;
class IDrawManagerNativeRootSource;
class IStartupConfigGameSystemSource;
struct PatternLayerNativeRenderServices;
struct FreeScrollNativeDrawServices;
struct FreeScrollNativeInputServices;

// 4A1559..4A1618：嵌入 CReadScript+60h，无虚表，也不保存现代服务/状态副本。
struct FreeScrollNativeBody50h final
{
    DrawLayerReferenceArray entriesAt00;
    std::uint32_t axesAt0C;
    std::int32_t stepAt10;
    std::int32_t xAt14;
    std::int32_t yAt18;
    RECT viewportAt1C;
    RECT limitsAt2C;
    std::int32_t repeatWaitAt3C;
    std::int32_t repeatCountdownAt40;
    std::uint8_t draggingAt44;
    std::uint8_t unwrittenAt45[3];
    POINT lastPointerAt48;
};
static_assert(sizeof(FreeScrollNativeBody50h) == 0x50);
static_assert(offsetof(FreeScrollNativeBody50h, axesAt0C) == 0x0C);
static_assert(offsetof(FreeScrollNativeBody50h, stepAt10) == 0x10);
static_assert(offsetof(FreeScrollNativeBody50h, xAt14) == 0x14);
static_assert(offsetof(FreeScrollNativeBody50h, yAt18) == 0x18);
static_assert(offsetof(FreeScrollNativeBody50h, viewportAt1C) == 0x1C);
static_assert(offsetof(FreeScrollNativeBody50h, limitsAt2C) == 0x2C);
static_assert(offsetof(FreeScrollNativeBody50h, repeatWaitAt3C) == 0x3C);
static_assert(offsetof(FreeScrollNativeBody50h, repeatCountdownAt40) == 0x40);
static_assert(offsetof(FreeScrollNativeBody50h, draggingAt44) == 0x44);
static_assert(offsetof(FreeScrollNativeBody50h, lastPointerAt48) == 0x48);

// 4AE185：Pattern 仅借用；processed/main 各有原 Ref12，析构依次释放。
struct FreeScrollEntryNativeBody24h final
{
    PatternLayerNativeBody80h* patternAt00;
    POINT unionOriginAt04;
    union { YtlReferenceSlotRuntime mainAt0C; };
    union { YtlReferenceSlotRuntime processedAt18; };
    FreeScrollEntryNativeBody24h() noexcept {}
    ~FreeScrollEntryNativeBody24h() noexcept {}
    FreeScrollEntryNativeBody24h(const FreeScrollEntryNativeBody24h&) = delete;
    FreeScrollEntryNativeBody24h& operator=(const FreeScrollEntryNativeBody24h&) = delete;
};
static_assert(sizeof(FreeScrollEntryNativeBody24h) == 0x24);
static_assert(offsetof(FreeScrollEntryNativeBody24h, unionOriginAt04) == 4);
static_assert(offsetof(FreeScrollEntryNativeBody24h, mainAt0C) == 0x0C);
static_assert(offsetof(FreeScrollEntryNativeBody24h, processedAt18) == 0x18);

void ConstructFreeScrollNativeAt4A1559(FreeScrollNativeBody50h& body,
    IGraphicsScreenSizeApi& screenSize);
void DestroyFreeScrollNativeAt4A3B60(FreeScrollNativeBody50h& body) noexcept;
void DestroyFreeScrollEntryNativeAt4A4E90(FreeScrollEntryNativeBody24h& body) noexcept;
void AppendFreeScrollEntryNativeAt4AE185(FreeScrollNativeBody50h& body,
    PatternLayerNativeBody80h* pattern,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
RECT ReadFreeScrollLocalUnionAt4ADDE0(PatternLayerNativeBody80h& pattern);
bool BeginFreeScrollNativeAt4ADF70(FreeScrollNativeBody50h& body,
    IDrawManagerNativeRootSource& draw, const IStartupConfigGameSystemSource& params,
    FreeScrollNativeDrawServices& graphics,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void EndFreeScrollNativeAt4AC970(FreeScrollNativeBody50h& body,
    IDrawManagerNativeRootSource& draw, const IStartupConfigGameSystemSource& params,
    PatternLayerNativeRenderServices& graphics, FreeScrollNativeInputServices& input);
}

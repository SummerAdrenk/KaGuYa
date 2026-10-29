#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/GraphicsSurfaceNativeBody.h>
#include <Runtime/GuiControlResourceNative.h>
#include <Runtime/RuntimeCStringVector.h>

namespace KaGuYa::Start::Runtime
{
// 444920：命令不是三整数快照；参数是唯一的原int vector。
struct SpriteCommandNativeBody10h final
{
    std::uint32_t opcodeAt00;
    EnvironmentVectorStorage<std::int32_t> argumentsAt04;
};
struct SpriteBranchNativeBody8h final
{
    std::int32_t firstAt00;
    std::int32_t secondAt04;
};
struct SpriteAnimationNativeBody54h final
{
    std::int32_t flagsAt00;
    std::int32_t waitAt04;
    std::int32_t indexAt08;
    std::int32_t resultAt0C;
    std::uint8_t repeatAt10;
    std::uint8_t unwrittenAt11[3];
    std::int32_t mappingCountAt14;
    union { YtlReferenceSlotRuntime currentAt18; };
    union { YtlReferenceSlotRuntime finalAt24; };
    union { YtlReferenceSlotRuntime countersAt30; };
    union { YtlReferenceSlotRuntime commandsAt3C; };
    union { YtlReferenceSlotRuntime branchesAt48; };
    SpriteAnimationNativeBody54h() noexcept {}
};
static_assert(sizeof(SpriteCommandNativeBody10h) == 0x10);
static_assert(offsetof(SpriteCommandNativeBody10h, argumentsAt04) == 4);
static_assert(sizeof(SpriteBranchNativeBody8h) == 8);
static_assert(sizeof(SpriteAnimationNativeBody54h) == 0x54);
static_assert(offsetof(SpriteAnimationNativeBody54h, repeatAt10) == 0x10);
static_assert(offsetof(SpriteAnimationNativeBody54h, mappingCountAt14) == 0x14);
static_assert(offsetof(SpriteAnimationNativeBody54h, currentAt18) == 0x18);
static_assert(offsetof(SpriteAnimationNativeBody54h, finalAt24) == 0x24);
static_assert(offsetof(SpriteAnimationNativeBody54h, countersAt30) == 0x30);
static_assert(offsetof(SpriteAnimationNativeBody54h, commandsAt3C) == 0x3C);
static_assert(offsetof(SpriteAnimationNativeBody54h, branchesAt48) == 0x48);

void ClearSpriteNameVectorAt43F240(EnvironmentVectorStorage<RuntimeCString>& names) noexcept;
void ClearSpriteAnimationAt43EF10(SpriteAnimationNativeBody54h& body);
void DestroySpriteAnimationAt43EB50(SpriteAnimationNativeBody54h& body) noexcept;
void ConstructSpriteCommandAt444920(SpriteCommandNativeBody10h& body) noexcept;
void DestroySpriteCommandAt4449C0(SpriteCommandNativeBody10h& body) noexcept;
void DeleteSpriteCommandArrayAt4487F0(SpriteCommandNativeBody10h* body) noexcept;

// 只提供原生存储工厂；不执行AN/PL解析、帧推进或装载业务。
// 原分配后第二次control检查保留；control失败留下已发布base，不补回收。
void ReplaceSpriteIntArrayAt4127B0(YtlReferenceSlotRuntime& slot, std::int32_t count);
void ReplaceSpriteCommandArrayAt444EB0(YtlReferenceSlotRuntime& slot, std::int32_t count);
void ReplaceSpriteSurfaceArrayAt444FE0(YtlReferenceSlotRuntime& slot, std::int32_t count,
    IGraphicsSurfaceApi& surfaces,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
// 446327分支已经选定；原计数来自WORD，不能任意扩成有符号int分支。
void ReplaceSpriteBranchArrayAt446327(YtlReferenceSlotRuntime& slot, std::uint16_t count);
}

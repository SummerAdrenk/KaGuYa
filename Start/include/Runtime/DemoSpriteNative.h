#pragma once

#include <Runtime/SpriteNativeResourceState.h>

namespace KaGuYa::Start::Runtime
{
// 4BF87B：前A0h是真实CSprite正文；服务与分配器不进入BCh布局。
struct DemoSpriteNativeBodyBCh final
{
    SpriteNativeBodyA0h spriteAt00;
    std::uint32_t directionAtA0;
    std::uint32_t fixedXAtA4;
    std::uint32_t fixedYAtA8;
    std::int32_t targetXAtAC;
    std::int32_t targetYAtB0;
    std::uint32_t stepXAtB4;
    std::uint32_t stepYAtB8;
    // A4/A8/B4/B8原构造未写，不使用整块值初始化。
    DemoSpriteNativeBodyBCh() noexcept {}
    ~DemoSpriteNativeBodyBCh() noexcept {}
    DemoSpriteNativeBodyBCh(const DemoSpriteNativeBodyBCh&) = delete;
    DemoSpriteNativeBodyBCh& operator=(const DemoSpriteNativeBodyBCh&) = delete;
};
static_assert(sizeof(DemoSpriteNativeBodyBCh) == 0xBC);
static_assert(alignof(DemoSpriteNativeBodyBCh) == 4);
static_assert(std::is_standard_layout_v<DemoSpriteNativeBodyBCh>);
static_assert(offsetof(DemoSpriteNativeBodyBCh, spriteAt00) == 0);
static_assert(offsetof(DemoSpriteNativeBodyBCh, directionAtA0) == 0xA0);
static_assert(offsetof(DemoSpriteNativeBodyBCh, fixedXAtA4) == 0xA4);
static_assert(offsetof(DemoSpriteNativeBodyBCh, fixedYAtA8) == 0xA8);
static_assert(offsetof(DemoSpriteNativeBodyBCh, targetXAtAC) == 0xAC);
static_assert(offsetof(DemoSpriteNativeBodyBCh, targetYAtB0) == 0xB0);
static_assert(offsetof(DemoSpriteNativeBodyBCh, stepXAtB4) == 0xB4);
static_assert(offsetof(DemoSpriteNativeBodyBCh, stepYAtB8) == 0xB8);

const SpriteNativeVtable& DemoSpriteNativeMethods() noexcept;
// memory须提供可由普通sized operator delete回收的存储；不保存memory指针。
DemoSpriteNativeBodyBCh* NewDemoSpriteNativeAt4BF85F(
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings);
// 非空Surface的清理由调用方GraphicsSurfaceNativeScope覆盖；不在正文存服务。
DemoSpriteNativeBodyBCh* DeleteDemoSpriteNativeAt4A6230(
    DemoSpriteNativeBodyBCh* body, std::uint32_t flags) noexcept;

// 仅新局部Ref安装：先清12字节，不释放旧control；失败保留已写前缀。
void ConstructDemoSpriteReferenceAt4BF8C8(YtlReferenceSlotRuntime& out,
    DemoSpriteNativeBodyBCh* body, IEnvironmentRootMemory& memory);
PatternSpriteRectResult DemoSpriteBoundsNativeAt4AD080(const DemoSpriteNativeBodyBCh& body) noexcept;
// 从4BFD18进入，不包含caller的同点throw 1、旧RECT、dirty或整段4A7690更新。
void CommitDemoSpriteMotionNativeAt4BFD18(DemoSpriteNativeBodyBCh& body,
    std::int32_t duration, std::int32_t startX, std::int32_t startY,
    std::int32_t endX, std::int32_t endY);
}

#pragma once

#include <Runtime/DrawLayerReferenceArray.h>

#include <type_traits>

namespace KaGuYa::Start::Runtime
{
class ICrtRandomSource;
class IGraphicsTransitApi;
class TransitRuntime;
class RootRuntimeOwner;
struct TransitOperation;
struct LayerListNativeBody18h;
struct DrawManagerNativeBody2Ch;

struct DrawManagerNativeVtable final
{
    DrawManagerNativeBody2Ch* (__thiscall* deletingDestructorAt00)(
        DrawManagerNativeBody2Ch*, std::uint32_t);
};
static_assert(sizeof(DrawManagerNativeVtable) == 4);

// 458770：+04从未由构造器写入；服务与现代图层不放进此正文。
struct DrawManagerNativeBody2Ch final
{
    const DrawManagerNativeVtable* vptrAt00;
    std::uint8_t unwrittenAt04[4];
    YtlReferenceSlotRuntime registeredLayersAt08;
    DrawLayerReferenceArray ownedLayersAt14;
    YtlReferenceSlotRuntime transitAt20;

    DrawManagerNativeBody2Ch(ICrtRandomSource& random, IEnvironmentRootMemory& memory);
    DrawManagerNativeBody2Ch(const DrawManagerNativeBody2Ch&) = delete;
    DrawManagerNativeBody2Ch& operator=(const DrawManagerNativeBody2Ch&) = delete;
    // 显式八阶段析构，Ref/数组无自动owner，最后阶段之后的晚填不再多扫一遍。
    void DestroyAt462BB0() noexcept;
};
static_assert(std::is_standard_layout_v<DrawManagerNativeBody2Ch>);
static_assert(std::is_trivially_destructible_v<DrawManagerNativeBody2Ch>);
static_assert(sizeof(DrawManagerNativeBody2Ch) == 0x2C);
static_assert(alignof(DrawManagerNativeBody2Ch) == 4);
static_assert(offsetof(DrawManagerNativeBody2Ch, unwrittenAt04) == 4);
static_assert(offsetof(DrawManagerNativeBody2Ch, registeredLayersAt08) == 8);
static_assert(offsetof(DrawManagerNativeBody2Ch, ownedLayersAt14) == 0x14);
static_assert(offsetof(DrawManagerNativeBody2Ch, transitAt20) == 0x20);

[[nodiscard]] DrawManagerNativeBody2Ch* NewDrawManagerNativeAt48A593(ICrtRandomSource& random,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void InstallDrawManagerReferenceAt48A5B3(YtlReferenceSlotRuntime& slot,
    DrawManagerNativeBody2Ch* created, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
[[nodiscard]] DrawManagerNativeBody2Ch* ResolveDrawManagerReferenceAt4175A0(
    const YtlReferenceSlotRuntime& slot) noexcept;
[[nodiscard]] LayerListNativeBody18h* ResolveDrawLayerListAt47AE80(
    const DrawManagerNativeBody2Ch& manager) noexcept;
[[nodiscard]] TransitRuntime* ResolveDrawTransit(const DrawManagerNativeBody2Ch& manager) noexcept;
[[nodiscard]] std::uint8_t RenderDrawTransitionAt4C39E0(const DrawManagerNativeBody2Ch& manager,
    const TransitOperation& operation, IGraphicsTransitApi& graphics);

[[nodiscard]] LayerItemNativeBody14h* DrawLayerAt418C10(const DrawManagerNativeBody2Ch& manager,
    std::uint32_t index);
void AdvanceDrawLayersAt46B240(DrawManagerNativeBody2Ch& manager);
void ResetDrawLayersAt47E940(DrawManagerNativeBody2Ch& manager);

// 46A980/46AD20每一槽都各自413BE0后重读根，不是固定manager上的七次调用。
class IDrawManagerNativeRootSource
{
public:
    virtual ~IDrawManagerNativeRootSource() = default;
    [[nodiscard]] virtual DrawManagerNativeBody2Ch* CaptureDrawManagerAfter413BE0() = 0;
};
class RootDrawManagerNativeSource final : public IDrawManagerNativeRootSource
{
public:
    explicit RootDrawManagerNativeSource(RootRuntimeOwner& root) noexcept : root_(root) {}
    DrawManagerNativeBody2Ch* CaptureDrawManagerAfter413BE0() override;
private:
    RootRuntimeOwner& root_;
};
[[nodiscard]] std::uint8_t PrepareDrawLayersAt46A980(IDrawManagerNativeRootSource& source, std::int32_t mode = 0);
[[nodiscard]] std::uint8_t PrepareDrawLayersAt46AD20(IDrawManagerNativeRootSource& source,
    std::int32_t mode);

// 普通477736入口允许动态control异常外传；根EH/最终析构是终止边界。
void ReleaseDrawManagerReferenceAt477736(YtlReferenceSlotRuntime& slot);
void ReleaseDrawManagerReferenceAt40D050(YtlReferenceSlotRuntime& slot) noexcept;
void ReleaseDrawManagerReferenceAt4643DB(YtlReferenceSlotRuntime& slot) noexcept;
}

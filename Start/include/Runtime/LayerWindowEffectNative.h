#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/LayerItemNative.h>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct LayerWindowEffectNativeBody2Ch;
struct LayerListNativeBody18h;

// 5B6250/5B6260：方向是两个DWORD；正文只拥有三指针动态vector。
struct LayerWindowEffectDirection8h final
{
    std::int32_t xAt00;
    std::int32_t yAt04;
};
using LayerWindowEffectDirections = EnvironmentVectorStorage<LayerWindowEffectDirection8h>;
using LayerWindowEffectLayerReferences = EnvironmentVectorStorage<YtlReferenceSlotRuntime>;

// 5B5C28 Roll / 5B5C0C Toss；空槽的未使用参数仅标明原栈宽度，不猜业务类型。
struct LayerWindowEffectNativeVtable6 final
{
    void (__thiscall* drawAt00)(LayerWindowEffectNativeBody2Ch*, const RECT*, const LayerWindowEffectLayerReferences*);
    void (__thiscall* nullAt04)(LayerWindowEffectNativeBody2Ch*, std::uintptr_t, std::uintptr_t, std::uintptr_t);
    RECT* (__thiscall* motionAt08)(LayerWindowEffectNativeBody2Ch*, RECT*, std::uint8_t);
    void (__thiscall* resetAt0C)(LayerWindowEffectNativeBody2Ch*);
    RECT* (__thiscall* emptyRectAt10)(LayerWindowEffectNativeBody2Ch*, RECT*, std::uintptr_t);
    LayerWindowEffectNativeBody2Ch* (__thiscall* deletingDestructorAt14)(LayerWindowEffectNativeBody2Ch*, std::uint32_t);
};

struct LayerWindowEffectNativeBody2Ch final
{
    const LayerWindowEffectNativeVtable6* vptrAt00;
    std::int32_t frameAt04;
    std::int32_t periodAt08;
    std::int32_t amplitudeAt0C;
    std::int32_t offsetXAt10;
    std::int32_t offsetYAt14;
    std::int32_t directionAt18;
    LayerWindowEffectDirections directionsAt1C;
    std::uint8_t activeAt28;
    std::uint8_t unwrittenAt29[3];
};

// 4D42B1要求Pattern的真实第四槽；不是LayerItemFrame的Bounds槽，不借现代Pattern对象。
struct LayerWindowEffectPatternVtable4 final
{
    LayerItemNativeVtable base;
    void (__thiscall* drawAtOffsetAt0C)(LayerItemNativeBody14h*, std::int32_t, std::int32_t, const RECT*);
};

static_assert(sizeof(LayerWindowEffectDirection8h) == 8);
static_assert(sizeof(LayerWindowEffectDirections) == 12);
static_assert(sizeof(LayerWindowEffectNativeVtable6) == 0x18);
static_assert(offsetof(LayerWindowEffectNativeVtable6, motionAt08) == 8);
static_assert(offsetof(LayerWindowEffectNativeVtable6, deletingDestructorAt14) == 0x14);
static_assert(sizeof(LayerWindowEffectNativeBody2Ch) == 0x2C);
static_assert(alignof(LayerWindowEffectNativeBody2Ch) == 4);
static_assert(offsetof(LayerWindowEffectNativeBody2Ch, frameAt04) == 4);
static_assert(offsetof(LayerWindowEffectNativeBody2Ch, periodAt08) == 8);
static_assert(offsetof(LayerWindowEffectNativeBody2Ch, amplitudeAt0C) == 0x0C);
static_assert(offsetof(LayerWindowEffectNativeBody2Ch, offsetXAt10) == 0x10);
static_assert(offsetof(LayerWindowEffectNativeBody2Ch, offsetYAt14) == 0x14);
static_assert(offsetof(LayerWindowEffectNativeBody2Ch, directionAt18) == 0x18);
static_assert(offsetof(LayerWindowEffectNativeBody2Ch, directionsAt1C) == 0x1C);
static_assert(offsetof(LayerWindowEffectNativeBody2Ch, activeAt28) == 0x28);
static_assert(offsetof(LayerWindowEffectNativeBody2Ch, unwrittenAt29) == 0x29);
static_assert(std::is_trivial_v<LayerWindowEffectNativeBody2Ch>);
static_assert(std::is_standard_layout_v<LayerWindowEffectNativeBody2Ch>);
static_assert(sizeof(LayerWindowEffectPatternVtable4) == 0x10);

// 调用期借用，不能缓存Program尺寸或创建第二个Graphics owner。
// Clear叶负责4182F0/模块存在判断及原ClearScreen调用；两次4180C0须分别现场取源。
class ILayerWindowEffectNativeSource
{
public:
    virtual ~ILayerWindowEffectNativeSource() = default;
    virtual void ClearScreenIfLoadedAt4182F0() = 0;
    virtual std::int32_t ReadProgramHeightAt4180C0() = 0;
    virtual std::int32_t ReadProgramWidthAt4180C0() = 0;
};
class LayerWindowEffectNativeScope final
{
public:
    explicit LayerWindowEffectNativeScope(ILayerWindowEffectNativeSource& source) noexcept;
    ~LayerWindowEffectNativeScope() noexcept;
    LayerWindowEffectNativeScope(const LayerWindowEffectNativeScope&) = delete;
    LayerWindowEffectNativeScope& operator=(const LayerWindowEffectNativeScope&) = delete;
private:
    ILayerWindowEffectNativeSource* previous_;
};

// 4D6860的Window宿主边界；实现者指向实际槽，本模块不宣称已装入4Ch。
class ILayerWindowEffectSelectionSource : public ILayerWindowEffectNativeSource
{
public:
    virtual std::int32_t ReadSelectorAt3C() = 0;
    virtual void WriteSelectorAt3C(std::int32_t value) = 0;
    virtual const YtlReferenceSlotRuntime& CaptureLayerListAt24() = 0;
};

const LayerWindowEffectNativeVtable6& LayerWindowEffectBaseMethods() noexcept;
const LayerWindowEffectNativeVtable6& LayerWindowRollNativeMethods() noexcept;
const LayerWindowEffectNativeVtable6& LayerWindowTossNativeMethods() noexcept;

std::int32_t LayerWindowEffectDirectionCount(const LayerWindowEffectDirections& directions) noexcept;
LayerWindowEffectDirection8h* GrowLayerWindowEffectDirectionsAt4C8690(LayerWindowEffectDirections& directions,
    LayerWindowEffectDirection8h* position, const LayerWindowEffectDirection8h& source);
void AppendLayerWindowEffectDirection(LayerWindowEffectDirections& directions,
    const LayerWindowEffectDirection8h& source);

LayerWindowEffectNativeBody2Ch* ConstructLayerWindowRollNativeAt4D68FE(LayerWindowEffectNativeBody2Ch* body);
LayerWindowEffectNativeBody2Ch* ConstructLayerWindowTossNativeAt4D6A64(LayerWindowEffectNativeBody2Ch* body);
LayerWindowEffectNativeBody2Ch* CreateLayerWindowRollNativeAt4D68E0();
LayerWindowEffectNativeBody2Ch* CreateLayerWindowTossNativeAt4D6A46();
void DestroyLayerWindowEffectNativeAt4C9DD0(LayerWindowEffectNativeBody2Ch* body) noexcept;
LayerWindowEffectNativeBody2Ch* DeleteLayerWindowRollNativeAt4CAD40(LayerWindowEffectNativeBody2Ch* body,
    std::uint32_t flags) noexcept;
LayerWindowEffectNativeBody2Ch* DeleteLayerWindowTossNativeAt4CADA0(LayerWindowEffectNativeBody2Ch* body,
    std::uint32_t flags) noexcept;

// 4D69A5/4D6B09再次释放旧槽；完成raw正文不受此阶段EH保护，control失败保留base。
void InstallLayerWindowRollReferenceAt4D69A5(YtlReferenceSlotRuntime& target, LayerWindowEffectNativeBody2Ch* body);
void InstallLayerWindowTossReferenceAt4D6B09(YtlReferenceSlotRuntime& target, LayerWindowEffectNativeBody2Ch* body);
LayerWindowEffectNativeBody2Ch* ResolveLayerWindowEffectReference(const YtlReferenceSlotRuntime& reference) noexcept;
void ReleaseLayerWindowEffectReferenceAt4607A0(YtlReferenceSlotRuntime& reference) noexcept;

// 不含4D6C10的同selector跳过判断：先普通可抛Release，最后才提交selector。
void ReplaceLayerWindowEffectAt4D6860(YtlReferenceSlotRuntime& target, std::int32_t selector,
    ILayerWindowEffectSelectionSource& source);
}

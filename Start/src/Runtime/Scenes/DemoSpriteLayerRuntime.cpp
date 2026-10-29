#include <Runtime/DemoSpriteLayerRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumReferences = 0x15555555u;
static_assert(sizeof(std::uintptr_t) == 4);

Slot* Advance(Slot* pointer, std::uint32_t count) noexcept
{
    return reinterpret_cast<Slot*>(reinterpret_cast<std::uintptr_t>(pointer) + count * 12u);
}

std::uint32_t Distance(const Slot* begin, const Slot* end) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(begin));
    return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(bytes) / 12);
}

DemoSpriteRuntime* Resolve(const Slot& slot) noexcept
{
    if (!slot.baseAt00) return nullptr;
    const auto stride = slot.controlAt08 ?
        static_cast<const EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C : 0u;
    const auto offset = slot.biasAt04 * stride;
    return reinterpret_cast<DemoSpriteRuntime*>(slot.baseAt00 + offset);
}

void CopyReference(Slot& target, const Slot& source) noexcept
{
    target.baseAt00 = source.baseAt00;
    target.biasAt04 = source.biasAt04;
    target.controlAt08 = source.controlAt08;
    if (target.controlAt08) ++target.controlAt08->referenceCountAt04;
}

void ReleaseRangeAt452320(Slot* begin, Slot* end) noexcept
{
    // 5EB824 state0 -> 541E55：逐项释放失败即终止；payload 后重读该项 control。
    for (; begin != end; begin = Advance(begin, 1)) ReleaseYtlReference(*begin);
}

void CopyRangeAt490180(Slot* begin, Slot* end, Slot* target)
{
    // 490D50/490180 都是 Ref 增持复制，不是移动；只有末尾空范围清理在 terminate state。
    for (; begin != end; begin = Advance(begin, 1), target = Advance(target, 1))
        CopyReference(*target, *begin);
    ReleaseRangeAt452320(target, target);
}

Slot* AllocateVectorAt491190(std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    const auto bytes = capacity <= MaximumReferences ? capacity * 12u : 0xFFFFFFFFu;
    if (bytes < 0x1000u)
        return bytes ? static_cast<Slot*>(memory.AllocateBody(bytes)) : nullptr;
    const auto request = bytes > 0xFFFFFFFFu - 0x23u ? 0xFFFFFFFFu : bytes + 0x23u;
    void* const raw = memory.AllocateBody(request);
    if (!raw) _invalid_parameter_noinfo_noreturn();
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23u) & ~std::uintptr_t{31};
    std::memcpy(reinterpret_cast<void*>(aligned - 4u), &raw, sizeof(raw));
    return reinterpret_cast<Slot*>(aligned);
}

void DeleteVectorAt491500(Slot* begin, std::uint32_t capacity, IEnvironmentRootMemory& memory)
{
    auto bytes = capacity * 12u;
    void* raw = begin;
    if (bytes >= 0x1000u)
    {
        const auto address = reinterpret_cast<std::uintptr_t>(begin);
        std::uintptr_t allocation;
        std::memcpy(&allocation, reinterpret_cast<const void*>(address - 4u), sizeof(allocation));
        bytes += 0x23u;
        if (address - allocation - 4u > 0x1Fu) _invalid_parameter_noinfo_noreturn();
        raw = reinterpret_cast<void*>(allocation);
    }
    memory.DeleteVector(raw, bytes);
}

class RetiredSpriteControl final : public EnvironmentObjectReferenceControl
{
public:
    RetiredSpriteControl() noexcept : EnvironmentObjectReferenceControl(nullptr, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 5ADD18 -> 412460：退役 control 后续删除为 14h。
        if (flags & 1u) ::operator delete(this, std::size_t{0x14});
    }
};
static_assert(sizeof(RetiredSpriteControl) == 0x18);

void RetireSpriteControl(EnvironmentObjectReferenceControl* control, std::uint32_t flags)
{
    std::array<std::byte, 20> fields;
    std::memcpy(fields.data(), reinterpret_cast<const std::byte*>(control) + 4, fields.size());
    auto* const retired = ::new (static_cast<void*>(control)) RetiredSpriteControl;
    std::memcpy(reinterpret_cast<std::byte*>(retired) + 4, fields.data(), fields.size());
    // 4A5BE0：本次退役后的删除仍为 18h；flags=0 只改 vptr。
    if (flags & 1u) ::operator delete(retired, std::size_t{0x18});
}

void DestroySpriteBodyAt4A6230(DemoSpriteRuntime* body) noexcept
{
    // 4A6230/43ECB0 的 FuncInfo 均 flags5，删除本身也在终止边界内。
    // 两次 Clear 对应派生析构和基类析构；其内部仍是现代 string/surface 实现。
    body->Sprite().Clear();
    body->Sprite().Clear();
    body->~DemoSpriteRuntime();
    ::operator delete(body, sizeof(DemoSpriteRuntime));
}

class SpriteControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SpriteControl(DemoSpriteRuntime* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 5B5A90 -> 4C5FD0：payload 为空则返回，否则进入 flags5 的正文删除入口。
        if (payloadAt14) DestroySpriteBodyAt4A6230(static_cast<DemoSpriteRuntime*>(payloadAt14));
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireSpriteControl(this, flags); }
};
static_assert(sizeof(SpriteControl) == 0x18);

struct BodyConstructionCleanup final
{
    void* raw;
    IEnvironmentRootMemory& memory;
    DemoSpriteRuntime* constructed = nullptr;
    bool active = true;
    ~BodyConstructionCleanup() noexcept
    {
        if (!active) return;
        // 4BF5A0 state2 -> 43ECB0，继而 state1 -> raw delete；不重试失败清理。
        if (constructed)
        {
            constructed->Sprite().Clear();
            constructed->~DemoSpriteRuntime();
        }
        memory.DeleteBody(raw, sizeof(DemoSpriteRuntime));
    }
};

DemoSpriteRuntime* CreateSpriteBody(IGraphicsSurfaceApi& graphics, IEnvironmentRootMemory& memory)
{
    // 原申请/stride 为 BCh；现代 SpriteRuntime 不是 A0h，必须用真实大小，不能伪造原 ABI。
    // 45D7D0 的两次 GetNil 和六级成员 EH 尚不由现代 noexcept 构造器模拟。
    void* const raw = memory.AllocateBody(sizeof(DemoSpriteRuntime));
    BodyConstructionCleanup cleanup{raw, memory};
    auto* const body = raw ? ::new (raw) DemoSpriteRuntime(graphics) : nullptr;
    cleanup.constructed = body;
    if (body) body->Sprite().Clear(); // 4BF8AA：基类构造完成之后的 Clear。
    cleanup.active = false; // 4BF8B5：control 分配失败不能回收已完成 body。
    return body;
}

struct LocalReferenceCleanup final
{
    Slot& reference;
    bool active = true;
    ~LocalReferenceCleanup() noexcept
    {
        if (active) ReleaseDemoSpriteReferenceAt460260(reference);
    }
};

std::int32_t AddCoordinate(const std::int32_t a, const std::int32_t b) noexcept
{
    return std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(a) + static_cast<std::uint32_t>(b));
}

// 443580：索引依次跨越主帧数组和各辅助组，不能只把它当作主数组下标。
const RuntimeGraphicsSurface& FrameAt(const SpriteRuntime& sprite, std::size_t index)
{
    if (index < sprite.PrimarySurfaceCount())
        return sprite.PrimarySurfaceAt(index);
    index -= sprite.PrimarySurfaceCount();
    for (std::size_t group = 0; group < sprite.AuxiliarySurfaceGroupCount(); ++group)
    {
        if (index < sprite.AuxiliarySurfaceCount(group))
            return sprite.AuxiliarySurfaceAt(group, index);
        index -= sprite.AuxiliarySurfaceCount(group);
    }
    // 原 getter 返回空指针后会被调用者解引用；明确拒绝，不绘制替代帧。
    throw std::out_of_range("CDemoSeq frame index exceeds sprite surfaces");
}

void DrawFrame(const RuntimeGraphicsSurface& frame, const std::int32_t x,
    const std::int32_t y, const RECT& clip, IGraphicsSurfaceCompositionApi& composition,
    IGraphicsScreenUpdateApi& screen)
{
    // 441980：句柄为零或 Graphics 引用为空时返回 false；4C3D50 忽略返回值。
    if (frame.Handle() != 0 && screen.ModuleLoaded())
        static_cast<void>(composition.DrawSurface(
            frame.Handle(), x, y, nullptr, nullptr, &clip, 0));
}

// 4C3D50 与 4A7F00 的精灵/映射遍历相同，只在目标和清屏/脏区副作用上不同。
template<class Submit>
void SubmitVisibleFrames(const DemoSpriteLayerRuntime& layer, Submit&& submit)
{
    for (std::uint32_t index = 0; index < layer.Count(); ++index)
    {
        const auto& demo = layer.At(index);
        const auto& sprite = demo.Sprite();
        if (!sprite.Active() || sprite.LogicalName().empty())
            continue;
        switch (sprite.LoadMode())
        {
        case SpriteLoadMode::StaticWithoutAlpha:
        case SpriteLoadMode::StaticWithAlpha:
            submit(FrameAt(sprite, 0), demo.PositionX(), demo.PositionY());
            break;
        case SpriteLoadMode::Animation:
            for (const auto mapping : sprite.CurrentFrameMappings())
            {
                if (mapping < 0)
                    continue;
                const auto& frame = FrameAt(sprite, static_cast<std::size_t>(mapping));
                submit(frame, AddCoordinate(demo.PositionX(), frame.OriginX()),
                    AddCoordinate(demo.PositionY(), frame.OriginY()));
            }
            break;
        }
    }
}
}

Slot* InsertDemoSpriteReferenceAt453F20(DemoSpriteReferenceVector& storage, Slot* position,
    const Slot& source, IEnvironmentRootMemory& memory)
{
    const auto index = Distance(storage.begin, position);
    const auto size = Distance(storage.begin, storage.end);
    if (size == MaximumReferences) throw std::length_error("vector<T> too long");
    const auto required = size + 1u;
    const auto capacity = Distance(storage.begin, storage.capacity);
    const auto grown = capacity > MaximumReferences - capacity / 2u ? required : capacity + capacity / 2u;
    const auto nextCapacity = (std::max)(required, grown);
    auto* const next = AllocateVectorAt491190(nextCapacity, memory);
    auto* const inserted = Advance(next, index);
    auto* cleanupBegin = inserted;
    auto* const cleanupEnd = Advance(inserted, 1);
    try
    {
        // 453FF2..454009：分配回调后读 source，增持完成后才重取旧 end/begin。
        // 原小块分配返回空会直接写空指针；此处显式硬失败，不补异常回滚资格。
        if (!next) _invalid_parameter_noinfo_noreturn();
        CopyReference(*inserted, source);
        if (position == storage.end)
            CopyRangeAt490180(storage.begin, storage.end, next);
        else
        {
            CopyRangeAt490180(storage.begin, position, next);
            cleanupBegin = next;
            CopyRangeAt490180(position, storage.end, cleanupEnd);
        }
    }
    catch (...)
    {
        // 45406E/5EBD44：普通 catch，不给退新块套 noexcept；删除异常替换原异常。
        ReleaseRangeAt452320(cleanupBegin, cleanupEnd);
        DeleteVectorAt491500(next, nextCapacity, memory);
        throw;
    }
    // 454047 撤回滚资格后才进 48F720；释放旧项后重新读 capacity/begin。
    if (storage.begin)
    {
        ReleaseRangeAt452320(storage.begin, storage.end);
        auto* const currentCapacity = storage.capacity;
        auto* const currentBegin = storage.begin;
        DeleteVectorAt491500(currentBegin, Distance(currentBegin, currentCapacity), memory);
    }
    storage.begin = next;
    storage.end = Advance(next, required);
    storage.capacity = Advance(next, nextCapacity);
    return Advance(storage.begin, index);
}

void ReleaseDemoSpriteReferenceAt460260(Slot& reference) noexcept
{
    // 5E54C4 flags5；此路径 payload 后重取当前 control，不使用正常出口的缓存。
    ReleaseYtlReference(reference);
}

void ReleaseDemoSpriteReferenceCachedAt4BF945(Slot& reference,
    YtlReferenceControlRuntime* cachedControl) noexcept
{
    // 4BF5A0 state4 -> 541E55：失败终止，不能再进入 state3 清理并重试。
    if (!cachedControl) return;
    if (--cachedControl->referenceCountAt04 == 0)
    {
        if (cachedControl->owningAt08) cachedControl->DestroyPayloadAtVslot00();
        cachedControl->DeleteControlAtVslot04(1);
    }
    reference.controlAt08 = nullptr;
    reference.baseAt00 = 0;
}

void DestroyDemoSpriteReferenceVectorAt4615F0(DemoSpriteReferenceVector& storage,
    IEnvironmentRootMemory& memory) noexcept
{
    // 5EB090 flags5 包含最终退块；空 begin 时不补写另两个字段。
    if (!storage.begin) return;
    ReleaseRangeAt452320(storage.begin, storage.end);
    auto* const currentCapacity = storage.capacity;
    auto* const currentBegin = storage.begin;
    DeleteVectorAt491500(currentBegin, Distance(currentBegin, currentCapacity), memory);
    storage.begin = nullptr;
    storage.end = nullptr;
    storage.capacity = nullptr;
}

DemoSpriteLayerRuntime::DemoSpriteLayerRuntime(
    IGraphicsSurfaceApi& graphics, const RECT& bounds, IEnvironmentRootMemory& memory) noexcept
    : graphics_(graphics), bounds_(bounds), memory_(memory)
{
}

DemoSpriteLayerRuntime::~DemoSpriteLayerRuntime() noexcept
{
    // 4A3CA0 -> 4615F0 -> 452320 从 begin 向 end 逐个释放引用。
    // 这里只闭合 sprite vector；现代 dirty/surface 成员不是完整 A4h sequence 的 Ref 布局。
    DestroyDemoSpriteReferenceVectorAt4615F0(sprites_, memory_);
}

void DemoSpriteLayerRuntime::AppendOneDemoSpriteAt4BF864()
{
    auto* const body = CreateSpriteBody(graphics_, memory_);
    Slot local{};
    YtlReferenceControlRuntime* cachedControl = nullptr;
    if (body)
    {
        local.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
        void* const raw = memory_.AllocateBody(0x18);
        auto* const control = raw ? ::new (raw) SpriteControl(body) : nullptr;
        // 4BF8F0 原空 control 直接写失败；不把它转换成可恢复 bad_alloc。
        if (!control) _invalid_parameter_noinfo_noreturn();
        control->referenceCountAt04 = 1;
        control->owningAt08 = 1;
        control->strideAt0C = static_cast<std::uint32_t>(sizeof(DemoSpriteRuntime));
        local.controlAt08 = control;
        control->countAt10 = 1;
        cachedControl = control;
    }
    const auto cachedBase = local.baseAt00;
    auto* const end = sprites_.end;
    LocalReferenceCleanup cleanup{local}; // state3，仅 append 失败释放当前 local Ref。
    if (end != sprites_.capacity)
    {
        end->baseAt00 = cachedBase;
        end->biasAt04 = 0;
        end->controlAt08 = cachedControl;
        if (cachedControl) ++cachedControl->referenceCountAt04;
        sprites_.end = Advance(sprites_.end, 1);
    }
    else
    {
        static_cast<void>(InsertDemoSpriteReferenceAt453F20(sprites_, end, local, memory_));
        cachedControl = local.controlAt08; // 4BF942 慢路径返回后重取。
    }
    cleanup.active = false;
    ReleaseDemoSpriteReferenceCachedAt4BF945(local, cachedControl);
}

DemoSpriteRuntime* DemoSpriteLayerRuntime::ResolveAt4AD170(const std::uint32_t index) noexcept
{
    return Resolve(*Advance(sprites_.begin, index));
}

const DemoSpriteRuntime* DemoSpriteLayerRuntime::ResolveAt4AD170(const std::uint32_t index) const noexcept
{
    return Resolve(*Advance(sprites_.begin, index));
}

DemoSpriteRuntime& DemoSpriteLayerRuntime::EnsureSlot(const std::size_t index)
{
    while (index >= Count()) AppendOneDemoSpriteAt4BF864();
    return At(index);
}

DemoSpriteRuntime& DemoSpriteLayerRuntime::At(const std::size_t index)
{
    if (index >= Count()) throw std::out_of_range("CDemoSeq sprite index exceeds references");
    auto* const body = ResolveAt4AD170(static_cast<std::uint32_t>(index));
    if (!body) throw std::out_of_range("CDemoSeq sprite reference is null");
    return *body;
}

const DemoSpriteRuntime& DemoSpriteLayerRuntime::At(const std::size_t index) const
{
    if (index >= Count()) throw std::out_of_range("CDemoSeq sprite index exceeds references");
    const auto* const body = ResolveAt4AD170(static_cast<std::uint32_t>(index));
    if (!body) throw std::out_of_range("CDemoSeq sprite reference is null");
    return *body;
}

std::size_t DemoSpriteLayerRuntime::Count() const noexcept { return Distance(sprites_.begin, sprites_.end); }
DirtyRectCollection& DemoSpriteLayerRuntime::DirtyRects() noexcept { return dirty_; }

bool DemoSpriteLayerRuntime::AnyMoving() const noexcept
{
    // 4BDAE0 的阻塞判断不跳过隐藏 Sprite，与 4A7690 的更新 gate 不同。
    for (std::uint32_t index = 0; index < Count(); ++index)
        if (At(index).Motion().direction != 0)
            return true;
    return false;
}

void DemoSpriteLayerRuntime::UpdateSpritesAndRedraw(
    IGraphicsSurfaceCompositionApi& composition, IGraphicsScreenUpdateApi& screen)
{
    for (std::uint32_t index = 0; index < Count(); ++index)
        At(index).Update(bounds_, dirty_);
    Redraw(composition, screen);
}

void DemoSpriteLayerRuntime::Redraw(
    IGraphicsSurfaceCompositionApi& composition, IGraphicsScreenUpdateApi& screen)
{
    // 4C3D50：先合并，再逐矩形清屏/重画/登记；异常时不提前清空 dirty。
    dirty_.Coalesce();
    for (const RECT& rect : dirty_.Rects())
    {
        screen.ClearScreenIfLoaded(&rect);
        SubmitVisibleFrames(*this, [&](const RuntimeGraphicsSurface& frame,
            const std::int32_t x, const std::int32_t y)
            { DrawFrame(frame, x, y, rect, composition, screen); });
        screen.InvalidatedRectIfLoaded(&rect);
    }
    dirty_.Clear();
}

void DemoSpriteLayerRuntime::ComposeInto(RuntimeGraphicsSurface& target,
    IGraphicsSurfaceCompositionApi& composition, IGraphicsScreenUpdateApi& screen)
{
    if (target.Handle() != 0 && screen.ModuleLoaded())
        composition.ClearSurface(target.Handle(), target.ClearColor(), nullptr);
    SubmitVisibleFrames(*this, [&](const RuntimeGraphicsSurface& frame,
        const std::int32_t x, const std::int32_t y)
        {
            // 4425C0：目标句柄、源句柄和模块分别判空；4A7F00 忽略 false。
            if (target.Handle() != 0 && frame.Handle() != 0 && screen.ModuleLoaded())
                static_cast<void>(composition.BlitSurface(
                    target.Handle(), frame.Handle(), x, y, nullptr, nullptr, nullptr, 0));
        });
}
}

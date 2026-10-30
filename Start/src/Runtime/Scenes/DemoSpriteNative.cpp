#include <Runtime/DemoSpriteNative.h>

#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = DemoSpriteNativeBodyBCh;
SpriteNativeBodyA0h* __fastcall Delete(SpriteNativeBodyA0h* sprite, void*, std::uint32_t flags) noexcept
{
    return reinterpret_cast<SpriteNativeBodyA0h*>(
        DeleteDemoSpriteNativeAt4A6230(reinterpret_cast<Body*>(sprite), flags));
}
// 原生x86虚槽桥接：对象走ECX，额外void*只占EDX且不消费，其余参数/返回与原槽对应。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const SpriteNativeVtable methods{
    reinterpret_cast<decltype(SpriteNativeVtable::deletingDestructorAt00)>(&Delete)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

struct ConstructionUnwind final
{
    IEnvironmentRootMemory& memory;
    void* raw;
    Body* body;
    bool baseComplete = false;
    bool armed = true;
    ~ConstructionUnwind() noexcept
    {
        if (!armed) return;
        // PE 5EF744：state2->1为59E9F2(43ECB0)，state1->0为59E9E0(delete BC)。
        if (baseComplete) DestroySpriteNativeAt43ECB0(body->spriteAt00);
        memory.DeleteBody(raw, 0xBC);
    }
};

class DemoSpriteControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit DemoSpriteControl(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 4C5FD0读取当前正文vptr；不能静态调用BC删除或改用基类A0h删除。
        auto* const body = static_cast<Body*>(payloadAt14);
        if (body) body->spriteAt00.vptrAt00->deletingDestructorAt00(&body->spriteAt00, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 4A5BE0退到5ADD18：首次18h，退休表再次删除14h。
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(DemoSpriteControl) == 0x18);

std::uint32_t MotionStep(std::int32_t larger, std::int32_t smaller, std::int32_t duration)
{
    const auto distance = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(larger) - static_cast<std::uint32_t>(smaller));
    // 4BFD8C/4BFDDA先符号扩展DWORD差，再左移16位；__alldiv只提交商低DWORD。
    if (duration == 0)
    {
        // 原__alldiv产生硬件除零，不改成C++异常或让优化器消费除零UB。
        __asm
        {
            xor eax, eax
            xor edx, edx
            xor ecx, ecx
            div ecx
        }
    }
    return static_cast<std::uint32_t>((static_cast<std::int64_t>(distance) * 65536) / duration);
}
}

const SpriteNativeVtable& DemoSpriteNativeMethods() noexcept { return methods; }

DemoSpriteNativeBodyBCh* NewDemoSpriteNativeAt4BF85F(
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    void* const raw = memory.AllocateBody(0xBC);
    if (!raw) return nullptr;
    auto* const body = ::new (raw) Body;
    ConstructionUnwind unwind{memory, raw, body};
    ConstructSpriteNativeAt45D7D0(body->spriteAt00, strings);
    body->spriteAt00.vptrAt00 = &methods;
    body->targetXAtAC = 0;
    body->targetYAtB0 = 0;
    unwind.baseComplete = true; // 4BF89C才获得state2的基类展开资格。
    body->directionAtA0 = 0;
    ClearSpriteNativeAt43F140(body->spriteAt00);
    unwind.armed = false; // 4BF8B5撤raw资格；control申请失败不回收已完成正文。
    return body;
}

DemoSpriteNativeBodyBCh* DeleteDemoSpriteNativeAt4A6230(Body* body, std::uint32_t flags) noexcept
{
    // PE 5EF684 flags5，唯一state0动作541E55：两轮清理任一异常均terminate。
    body->spriteAt00.vptrAt00 = &methods;
    ClearSpriteNativeAt43F140(body->spriteAt00);
    DestroySpriteNativeAt43ECB0(body->spriteAt00);
    // 原仅test bit0；bit1不是数组cookie协议。
    if (flags & 1u) ::operator delete(body, std::size_t{0xBC});
    return body;
}

void ConstructDemoSpriteReferenceAt4BF8C8(YtlReferenceSlotRuntime& out,
    Body* body, IEnvironmentRootMemory& memory)
{
    out = {};
    if (!body) return;
    out.biasAt04 = 0;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    void* const raw = memory.AllocateBody(0x18);
    auto* const control = raw ? ::new (raw) DemoSpriteControl(body) : nullptr;
    // 4BF8F0先写新control+4；原null写故障前不会发布out.controlAt08。
    volatile auto* const fields = static_cast<EnvironmentObjectReferenceControl*>(control);
    fields->referenceCountAt04 = 1;
    fields->owningAt08 = 1;
    fields->strideAt0C = 0xBC;
    out.controlAt08 = control;
    fields->countAt10 = 1;
}

PatternSpriteRectResult DemoSpriteBoundsNativeAt4AD080(const Body& body) noexcept
{
    return ReadSpriteDrawableRectNativeAt4AD080(body.spriteAt00);
}

void CommitDemoSpriteMotionNativeAt4BFD18(Body& body, std::int32_t duration,
    std::int32_t startX, std::int32_t startY, std::int32_t endX, std::int32_t endY)
{
    // volatile保留原ASM对同一正文的写入顺序，包括除零前可见前缀。
    volatile auto& motion = body;
    motion.directionAtA0 = 0;
    motion.fixedYAtA8 = 0;
    motion.fixedXAtA4 = 0;
    motion.stepYAtB8 = 0;
    motion.stepXAtB4 = 0;
    std::uint32_t direction = 0;
    if (startX != endX)
    {
        direction = startX < endX ? 1u : 2u;
        motion.directionAtA0 = direction;
        motion.stepXAtB4 = startX < endX ? MotionStep(endX, startX, duration)
            : MotionStep(startX, endX, duration);
    }
    if (startY != endY)
    {
        direction |= startY < endY ? 4u : 8u;
        motion.directionAtA0 = direction;
        motion.stepYAtB8 = startY < endY ? MotionStep(endY, startY, duration)
            : MotionStep(startY, endY, duration);
    }
    // 4BFDF8/4BFE0E仅写高WORD，随后各写基类坐标；不是两次整DWORD覆盖。
    *reinterpret_cast<volatile std::uint16_t*>(reinterpret_cast<std::byte*>(&body) + 0xAA) =
        static_cast<std::uint16_t>(startY);
    motion.spriteAt00.yAt30 = startY;
    motion.targetXAtAC = endX;
    *reinterpret_cast<volatile std::uint16_t*>(reinterpret_cast<std::byte*>(&body) + 0xA6) =
        static_cast<std::uint16_t>(startX);
    motion.spriteAt00.xAt2C = startX;
    motion.targetYAtB0 = endY;
}
}

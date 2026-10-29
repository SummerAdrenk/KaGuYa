#include <Runtime/AnimationFormatNative.h>

#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = AnimationFormatNativeBody44h;
Body* __fastcall Delete(Body* body, void*, std::uint32_t flags) noexcept
{ return DeleteAnimationFormatNativeAt4685B0(body, flags); }
const AnimationFormatNativeVtable methods{
    reinterpret_cast<decltype(AnimationFormatNativeVtable::deletingDestructorAt00)>(&Delete)};

struct ConstructionUnwind final
{
    Body& body;
    bool stringConstructed = false;
    bool referencesConstructed = false;
    ~ConstructionUnwind() noexcept
    {
        // 596291 -> 596286 -> 59627B -> 596270；清理发生在实际展开中。
        if (referencesConstructed)
        {
            ReleaseYtlReference(body.surfacesAt38);
            ReleaseYtlReference(body.branchesAt2C);
            ReleaseYtlReference(body.commandsAt20);
        }
        if (stringConstructed) DestroyRuntimeCStringAt40CD40(&body.nameAt04);
    }
};

class AnimationFormatControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit AnimationFormatControl(Body* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        // 492BD0：使用control当前payload和正文当前虚表，不能固定调默认析构。
        auto* const body = static_cast<Body*>(payloadAt14);
        if (body) body->vptrAt00->deletingDestructorAt00(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(AnimationFormatControl) == 0x18);

AnimationFormatControl* NewControl(Body* body)
{
    auto* const raw = ::operator new(std::size_t{0x18});
    return raw ? ::new (raw) AnimationFormatControl(body) : nullptr;
}

std::uint16_t ReadWord(const std::uint8_t* input) noexcept
{
    std::uint16_t value;
    std::memcpy(&value, input, sizeof(value));
    return value;
}
}

const AnimationFormatNativeVtable& AnimationFormatNativeMethods() noexcept { return methods; }

AnimationFormatNativeBody44h* ConstructAnimationFormatNativeAt4580A0(Body& body,
    IRuntimeStringManagerSource& strings)
{
    body.vptrAt00 = &methods;
    ConstructionUnwind unwind{body};
    InitializeEmptyRuntimeCString(body.nameAt04, strings);
    unwind.stringConstructed = true;
    // 4580F0..458128：GetNil成功后才开始三Ref的生命期及九个DWORD零写。
    ::new (static_cast<void*>(&body.commandsAt20)) YtlReferenceSlotRuntime{};
    ::new (static_cast<void*>(&body.branchesAt2C)) YtlReferenceSlotRuntime{};
    ::new (static_cast<void*>(&body.surfacesAt38)) YtlReferenceSlotRuntime{};
    body.heightAt14 = 0;
    body.widthAt10 = 0;
    body.yAt0C = 0;
    body.xAt08 = 0;
    body.repeatAt18 = 0;
    body.mappingCountAt1C = 0;
    unwind.referencesConstructed = true;
    ReleaseYtlReference(body.commandsAt20);
    ReleaseYtlReference(body.branchesAt2C);
    ReleaseYtlReference(body.surfacesAt38);
    unwind.referencesConstructed = false;
    unwind.stringConstructed = false;
    return &body;
}

void DestroyAnimationFormatNativeAt4628B0(Body& body) noexcept
{
    // FuncInfo 5E525C flags5：释放抛出即终止，不能继续清下一成员。
    body.vptrAt00 = &methods;
    ReleaseYtlReference(body.surfacesAt38);
    ReleaseYtlReference(body.branchesAt2C);
    ReleaseYtlReference(body.commandsAt20);
    DestroyRuntimeCStringAt40CD40(&body.nameAt04);
}

AnimationFormatNativeBody44h* DeleteAnimationFormatNativeAt4685B0(Body* body, std::uint32_t flags) noexcept
{
    DestroyAnimationFormatNativeAt4628B0(*body);
    // 4686AB仅检查bit0；本类型无vector deleting destructor的bit1分支。
    if (flags & 1u) ::operator delete(body, std::size_t{0x44});
    return body;
}

AnimationFormatNativeBody44h* NewAnimationFormatNativeAt47EECB(IRuntimeStringManagerSource& strings)
{
    void* const raw = ::operator new(std::size_t{0x44});
    if (!raw) return nullptr;
    try { return ConstructAnimationFormatNativeAt4580A0(*::new (raw) Body, strings); }
    catch (...)
    {
        // 599EF0裸44h删除仍在展开期；不能在catch内提前delete再重新抛出。
        struct Unwind final
        {
            void* raw;
            ~Unwind() noexcept { ::operator delete(raw, std::size_t{0x44}); }
        } unwind{raw};
        throw;
    }
}

void ConstructAnimationFormatReferenceAt47EF04(YtlReferenceSlotRuntime& out, Body* body)
{
    out.baseAt00 = 0;
    out.biasAt04 = 0;
    out.controlAt08 = nullptr;
    if (!body) return;
    out.biasAt04 = 0;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* const control = NewControl(body);
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x44;
    out.controlAt08 = control;
    control->countAt10 = 1;
}

void ConstructAnimationFormatReferenceAt4987F0(YtlReferenceSlotRuntime& out, Body* body, std::uint8_t owning)
{
    out.baseAt00 = 0;
    out.biasAt04 = 0;
    out.controlAt08 = nullptr;
    if (!body) return;
    out.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* const control = NewControl(body);
    out.controlAt08 = control;
    control->referenceCountAt04 = 1;
    out.controlAt08->owningAt08 = owning;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->strideAt0C = 0x44;
    static_cast<EnvironmentObjectReferenceControl*>(out.controlAt08)->countAt10 = 1;
}

AnimationFormatNativeBody44h* ResolveAnimationFormatReference(const YtlReferenceSlotRuntime& slot) noexcept
{ return ResolveEnvironmentObjectReference<Body>(slot); }

std::int32_t LoadAnimationFormatNativeAt448010(Body& body,
    const YtlReferenceSlotRuntime& input, IAnimationFormatNativePictures& pictures)
{
    try
    {
        const auto* const bytes = ResolveEnvironmentObjectReference<const std::uint8_t>(input);
        if (ReadWord(bytes) != 0x4E41u) throw 3;
        const auto version = ReadWord(bytes + 2);
        if (version == 0x3030u || version == 0x3031u)
        {
            const auto* cursor = bytes + 4;
            body.heightAt14 = 0;
            body.widthAt10 = 0;
            body.yAt0C = 0;
            body.xAt08 = 0;
            body.repeatAt18 = 0;
            body.mappingCountAt1C = 0;
            // 4480E7..44817D：先选定输入cursor，再依次释放旧数组，期间不重新解析input Ref。
            ReleaseYtlReference(body.commandsAt20);
            ReleaseYtlReference(body.branchesAt2C);
            ReleaseYtlReference(body.surfacesAt38);
            LoadLegacyAnimationCommandsNativeAt445B90(body, cursor,
                static_cast<std::uint8_t>(version == 0x3031u), pictures);
        }
        else
        {
            float nativeVersion;
            switch (version)
            {
            case 0x3032u: nativeVersion = 2.0f; break;
            case 0x3132u: nativeVersion = 2.1f; break;
            case 0x3033u: nativeVersion = 3.0f; break;
            default: throw 4;
            }
            const auto* cursor = bytes + 4;
            // 44820A无条件返回0，PIC返回AL=0不代表本入口失败。
            static_cast<void>(LoadNewAnimationCommandsNativeAt4460D0(body, cursor, nativeVersion, pictures));
        }
        return 0;
    }
    catch (int value) { return value; }
    catch (...) { return -1; }
}
}

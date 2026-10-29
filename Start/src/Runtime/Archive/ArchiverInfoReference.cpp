#include <Runtime/ArchiverInfoReference.h>
#include <Runtime/ArchiverInfoRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <new>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原5B30C0：slot00=492BE0，slot04=467950；不在控制块中追加现代owner。
class ArchiverInfoControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ArchiverInfoControl(ArchiverInfoRuntime* payload) noexcept
        : EnvironmentObjectReferenceControl(payload) {}

    void DestroyPayloadAtVslot00() override
    {
        auto* payload = static_cast<ArchiverInfoRuntime*>(payloadAt14);
        if (!payload) return;
        payload->~ArchiverInfoRuntime();
        ::operator delete(payload);
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 46795A：flags=0也先退化虚表，不重写其余字段或再次销毁正文。
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(ArchiverInfoControl) == 0x18);
static_assert(sizeof(ArchiverInfoRuntime) == 0x68);
}

ArchiverInfoReference::ArchiverInfoReference(const ArchiverInfoReference& source) noexcept
    : reference(source.reference)
{
    if (reference.controlAt08) ++reference.controlAt08->referenceCountAt04;
}

ArchiverInfoReference::ArchiverInfoReference(ArchiverInfoReference&& source) noexcept
    : reference(std::exchange(source.reference, {}))
{
}

ArchiverInfoReference& ArchiverInfoReference::operator=(const ArchiverInfoReference& source)
{
    if (this != &source)
    {
        ReleaseYtlReference(reference);
        reference = source.reference;
        if (reference.controlAt08) ++reference.controlAt08->referenceCountAt04;
    }
    return *this;
}

ArchiverInfoReference& ArchiverInfoReference::operator=(ArchiverInfoReference&& source)
{
    if (this != &source)
    {
        ReleaseYtlReference(reference);
        reference = std::exchange(source.reference, {});
    }
    return *this;
}

ArchiverInfoReference::~ArchiverInfoReference() noexcept
{
    ReleaseYtlReference(reference);
}

void ArchiverInfoReference::Reset()
{
    ReleaseYtlReference(reference);
}

ArchiverInfoRuntime* ArchiverInfoReference::get() const noexcept
{
    return ResolveEnvironmentObjectReference<ArchiverInfoRuntime>(reference);
}

ArchiverInfoReference ArchiverInfoReference::AdoptAt4590DA(ArchiverInfoRuntime* created)
{
    ArchiverInfoReference result;
    if (!created) return result;
    result.reference.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    // 4590F5..45912A：保留+09..+0B；分离分配与placement，避免/sdl预先整块清零。
    void* storage = ::operator new(sizeof(ArchiverInfoControl));
    auto* control = storage ? ::new (storage) ArchiverInfoControl(created) : nullptr;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x68;
    result.reference.controlAt08 = control;
    control->countAt10 = 1;
    return result;
}
}

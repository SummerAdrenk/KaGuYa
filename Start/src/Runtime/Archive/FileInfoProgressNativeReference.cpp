#include <Runtime/FileInfoProgressNativeReference.h>
#include <Runtime/EnvironmentRootRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <array>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
class RetiredControl final : public YtlReferenceControlRuntime
{
public:
    RetiredControl() noexcept : YtlReferenceControlRuntime(UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { _purecall(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { if (flags & 1u) RequireFileInfoProgressNativeServices().endpoint.memory.DeleteBody(this, 0x14); }
    std::uint32_t strideAt0C;
    std::uint32_t countAt10;
};
static_assert(sizeof(RetiredControl) == 0x14);
class Control final : public EnvironmentObjectReferenceControl
{
public:
    explicit Control(FileInfoProgressNativeBody10h* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<FileInfoProgressNativeBody10h*>(payloadAt14);
        if (!body) return; // 原492C23。
        const auto* methods = *static_cast<const FileInfoProgressNativeVtable5* const volatile*>(&body->vptrAt00);
        static_cast<void>(methods->deleteAt10(body, 1));
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        auto& memory = RequireFileInfoProgressNativeServices().endpoint.memory;
        std::array<std::byte, 20> fields;
        auto* storage = reinterpret_cast<std::byte*>(this);
        std::memcpy(fields.data(), storage + 4, fields.size());
        ::new(static_cast<void*>(this)) RetiredControl;
        std::memcpy(storage + 4, fields.data(), fields.size());
        if (flags & 1u) memory.DeleteBody(storage, 0x18); // 原4679F2；退役后的第二次删除才传14h。
    }
};
static_assert(sizeof(Control) == 0x18);
}
FileInfoProgressNativeBody10h* NewFileInfoProgressNativeAt489D70(FileInfoProgressNativeServices& services)
{
    auto* storage = services.endpoint.memory.AllocateBody(0x10);
    if (!storage) return nullptr;
    struct AllocationUnwind final
    {
        void* storage;
        IEnvironmentRootMemory& memory;
        ~AllocationUnwind() noexcept { if (storage) memory.DeleteBody(storage, 0x10); }
    } unwind{storage, services.endpoint.memory};
    auto* result = ConstructFileInfoProgressNativeAt4588F0(static_cast<FileInfoProgressNativeBody10h*>(storage), services);
    unwind.storage = nullptr;
    return result;
}
void ConstructFileInfoProgressNativeReferenceAt489DF5(YtlReferenceSlotRuntime& reference,
    FileInfoProgressNativeBody10h* body, IEnvironmentRootMemory& memory)
{
    reference = {};
    if (!body) return;
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    auto* storage = memory.AllocateBody(0x18);
    auto* control = storage ? ::new(storage) Control(body) : nullptr;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x10;
    reference.controlAt08 = control;
    control->countAt10 = 1;
}
void ReleaseFileInfoProgressNativeReferenceAt460730(YtlReferenceSlotRuntime& reference) noexcept
{ ReleaseYtlReference(reference); }
FileInfoProgressNativeReferenceView::FileInfoProgressNativeReferenceView(
    const YtlReferenceSlotRuntime& reference, FileInfoProgressNativeServices& services) noexcept
    : reference_(reference), services_(services) {}
bool FileInfoProgressNativeReferenceView::PollCancelAt47F0B0()
{
    FileInfoProgressNativeScope scope(services_);
    auto* body = ResolveEnvironmentObjectReference<FileInfoProgressNativeBody10h>(reference_);
    return body->vptrAt00->pollAt00(body) != 0;
}
LRESULT FileInfoProgressNativeReferenceView::StepAt46E820()
{
    FileInfoProgressNativeScope scope(services_);
    auto* body = ResolveEnvironmentObjectReference<FileInfoProgressNativeBody10h>(reference_);
    return body->vptrAt00->stepAt04(body);
}
LRESULT FileInfoProgressNativeReferenceView::AddRangeAt46A5A0(std::uint32_t delta)
{
    FileInfoProgressNativeScope scope(services_);
    auto* body = ResolveEnvironmentObjectReference<FileInfoProgressNativeBody10h>(reference_);
    return body->vptrAt00->rangeAt08(body, delta);
}
HWND FileInfoProgressNativeReferenceView::WindowAt47ADF0() const noexcept
{
    auto* body = ResolveEnvironmentObjectReference<FileInfoProgressNativeBody10h>(reference_);
    return body->vptrAt00->windowAt0C(body);
}
}

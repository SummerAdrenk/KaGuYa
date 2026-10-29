#include "ProgramSystemDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace ProgramSystemNativeDetail
{
void Destroy(Body& body, bool mode01) noexcept
{
    Leaves leaves;
    body.common.vptrAt00 = mode01 ? &ProgramSystem01DialogNativeVtable() : &ProgramSystem00DialogNativeVtable();
    // 4FC2DB/4FC37B：manager先释放，payload之后重取槽control，bias不清。
    ReleaseCurrent(body.clickedManagerAt5C, true);
    DestroyGuiDialogAt4F7AC0(&body.common);
}
}
void DestroyProgramSystem00DialogNative(ProgramSystemDialogNativeBody68h& body) noexcept
{ ProgramSystemNativeDetail::Destroy(body, false); }
void DestroyProgramSystem01DialogNative(ProgramSystemDialogNativeBody68h& body) noexcept
{ ProgramSystemNativeDetail::Destroy(body, true); }
ProgramSystemDialogNativeBody68h* DeleteProgramSystem00DialogNativeAt4FC2B0(
    ProgramSystemDialogNativeBody68h* body, std::uint32_t flags) noexcept
{
    ProgramSystemNativeDetail::Leaves leaves;
    DestroyProgramSystem00DialogNative(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{0x68});
    return body;
}
ProgramSystemDialogNativeBody68h* DeleteProgramSystem01DialogNativeAt4FC350(
    ProgramSystemDialogNativeBody68h* body, std::uint32_t flags) noexcept
{
    ProgramSystemNativeDetail::Leaves leaves;
    DestroyProgramSystem01DialogNative(*body);
    if (flags & 1u) ::operator delete(body, std::size_t{0x68});
    return body;
}
namespace
{
template<bool Mode01> class SystemControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit SystemControl(ProgramSystemDialogNativeBody68h* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        using namespace ProgramSystemNativeDetail;
        if (auto* body = static_cast<Common*>(Read(payloadAt14)))
            Read(body->vptrAt00)->deletingDestructorAt04(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(SystemControl<false>) == 0x18);
static_assert(sizeof(SystemControl<true>) == 0x18);
template<bool Mode01> EnvironmentObjectReferenceControl* NewControl(ProgramSystemDialogNativeBody68h* body)
{
    auto* raw = ::operator new(0x18);
    return raw ? ::new(raw) SystemControl<Mode01>(body) : nullptr;
}
}
EnvironmentObjectReferenceControl* NewProgramSystem00DialogNativeControl(ProgramSystemDialogNativeBody68h* body)
{ return NewControl<false>(body); }
EnvironmentObjectReferenceControl* NewProgramSystem01DialogNativeControl(ProgramSystemDialogNativeBody68h* body)
{ return NewControl<true>(body); }
}

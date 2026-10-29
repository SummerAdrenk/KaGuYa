#include <Runtime/SceneManagerPublicNativeReferences.h>

#include <bit>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
template<class T> T Read(const T& value) noexcept
{ return *reinterpret_cast<const volatile T*>(&value); }

void CopyBorrowed(Slot& destination, const Slot* source) noexcept
{
    // 4A50A0..B4/4A5100..14：释放旧槽之后才读取来源，无base/count门控。
    destination.baseAt00 = Read(source->baseAt00);
    destination.biasAt04 = Read(source->biasAt04);
    destination.controlAt08 = Read(source->controlAt08);
    if (destination.controlAt08) ++destination.controlAt08->referenceCountAt04;
}
std::uint32_t Stride(const Slot& reference) noexcept
{
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(reference.controlAt08));
    return control ? Read(control->strideAt0C) : 0u;
}
CgUiButtonNativeBody6Ch* Button(const Slot& reference, std::int32_t index) noexcept
{
    const auto base = Read(reference.baseAt00);
    if (!base) return nullptr;
    const auto stride = Stride(reference);
    const auto offset = (Read(reference.biasAt04) + static_cast<std::uint32_t>(index)) * stride;
    return reinterpret_cast<CgUiButtonNativeBody6Ch*>(base + offset);
}
OptionCharSelectDialogNativeBody8Ch* Dialog(const Slot& reference) noexcept
{
    const auto base = Read(reference.baseAt00);
    if (!base) return nullptr;
    const auto stride = Stride(reference);
    return reinterpret_cast<OptionCharSelectDialogNativeBody8Ch*>(base + Read(reference.biasAt04) * stride);
}
}

SceneManagerModeButtonNativeView::SceneManagerModeButtonNativeView(const Slot* array,
    std::int32_t index, ModeSelectButtonNativeServices& services) noexcept
    : array_(array), index_(index), services_(&services), binding_(nullptr) {}
SceneManagerModeButtonNativeView::SceneManagerModeButtonNativeView(const Slot* array,
    std::int32_t index, ModeSelectButtonNativeServices* const* binding) noexcept
    : array_(array), index_(index), services_(nullptr), binding_(binding) {}
ModeSelectButtonNativeServices& SceneManagerModeButtonNativeView::Services() const noexcept
{ return *(binding_ ? *binding_ : services_); }
CgUiButtonNativeBody6Ch* SceneManagerModeButtonNativeView::Get() const noexcept
{ return Button(*array_, index_); }
std::int32_t SceneManagerModeButtonNativeView::ProcessAt4A98E0(std::int32_t forced) const
{
    auto& services = Services();
    ModeSelectButtonNativeScope scope(services);
    return ProcessCgUiButtonNativeAt4A98E0(*Get(), forced, services.button);
}
std::uint8_t SceneManagerModeButtonNativeView::DrawIntoAt4B5290(
    GraphicsSurfaceNativeBody* destination, const RECT* clip) const
{
    auto& services = Services();
    ModeSelectButtonNativeScope scope(services);
    return DrawCgUiButtonNativeAt4B5290(*Get(), destination, clip, services.button);
}

SceneManagerModeButtonsNativeReference::SceneManagerModeButtonsNativeReference(
    const Slot* borrowed, ModeSelectButtonNativeServices& services) noexcept : services_(&services)
{ CopyBorrowed(reference_, borrowed); }
SceneManagerModeButtonsNativeReference::SceneManagerModeButtonsNativeReference(
    const SceneManagerModeButtonsNativeReference& other) noexcept : services_(other.services_)
{ CopyBorrowed(reference_, &other.reference_); }
SceneManagerModeButtonsNativeReference::SceneManagerModeButtonsNativeReference(
    SceneManagerModeButtonsNativeReference&& other) noexcept
    : reference_(std::exchange(other.reference_, Slot{})), services_(other.services_) {}
SceneManagerModeButtonsNativeReference& SceneManagerModeButtonsNativeReference::operator=(
    const SceneManagerModeButtonsNativeReference& other)
{
    if (this != &other)
    {
        Reset();
        CopyBorrowed(reference_, &other.reference_);
        services_ = other.services_;
    }
    return *this;
}
SceneManagerModeButtonsNativeReference& SceneManagerModeButtonsNativeReference::operator=(
    SceneManagerModeButtonsNativeReference&& other)
{
    if (this != &other)
    {
        Reset();
        reference_ = std::exchange(other.reference_, Slot{});
        services_ = other.services_;
    }
    return *this;
}
SceneManagerModeButtonsNativeReference::~SceneManagerModeButtonsNativeReference() noexcept { Reset(); }
void SceneManagerModeButtonsNativeReference::Reset()
{
    // 无control不启动来源、不清孤立base/bias；非空GraphicRefs始终在同源scope退役。
    if (!Read(reference_.controlAt08)) return;
    ModeSelectButtonNativeScope scope(*services_);
    ReleaseYtlReference(reference_);
}
void SceneManagerModeButtonsNativeReference::Reset(const Slot* borrowed,
    ModeSelectButtonNativeServices& services)
{
    if (borrowed == &reference_) return;
    Reset();
    CopyBorrowed(reference_, borrowed);
    services_ = &services;
}
std::int32_t SceneManagerModeButtonsNativeReference::Count() const noexcept
{
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(reference_.controlAt08));
    return control ? std::bit_cast<std::int32_t>(Read(control->countAt10)) : 0;
}
SceneManagerModeButtonNativeView SceneManagerModeButtonsNativeReference::BorrowAt(std::int32_t index) const & noexcept
{ return SceneManagerModeButtonNativeView(&reference_, index, &services_); }

SceneManagerOptionCharNativeView::SceneManagerOptionCharNativeView(const Slot* borrowed,
    OptionCharSelectDialogNativeDependencies& services) noexcept
    : reference_(borrowed), services_(&services), binding_(nullptr) {}
SceneManagerOptionCharNativeView::SceneManagerOptionCharNativeView(const Slot* borrowed,
    OptionCharSelectDialogNativeDependencies* const* binding) noexcept
    : reference_(borrowed), services_(nullptr), binding_(binding) {}
OptionCharSelectDialogNativeDependencies& SceneManagerOptionCharNativeView::Services() const noexcept
{ return *(binding_ ? *binding_ : services_); }
OptionCharSelectDialogNativeBody8Ch* SceneManagerOptionCharNativeView::Get() const noexcept
{ return Dialog(*reference_); }
std::int32_t SceneManagerOptionCharNativeView::ProcessAtVslot48() const
{
    OptionCharSelectDialogNativeScope scope(Services());
    auto* body = &Get()->common;
    return body->vptrAt00->pollAt48(body);
}
std::int32_t SceneManagerOptionCharNativeView::SelectionAtVslot50() const
{
    OptionCharSelectDialogNativeScope scope(Services());
    auto* body = &Get()->common;
    return body->vptrAt00->resultAt50(body);
}
void SceneManagerOptionCharNativeView::ResetAtVslot90() const
{
    OptionCharSelectDialogNativeScope scope(Services());
    auto* body = &Get()->common;
    auto* table = reinterpret_cast<const OptionCharSelectDialogNativeVtable38*>(body->vptrAt00);
    table->resetAt90(body);
}
void SceneManagerOptionCharNativeView::AttachDirtyRectsAtVslot94(const Slot* borrowedDirty) const
{
    OptionCharSelectDialogNativeScope scope(Services());
    auto* body = &Get()->common;
    auto* table = reinterpret_cast<const OptionCharSelectDialogNativeVtable38*>(body->vptrAt00);
    table->attachDirtyAt94(body, borrowedDirty);
}
void SceneManagerOptionCharNativeView::DrawAtVslot60(const RECT* clip) const
{
    OptionCharSelectDialogNativeScope scope(Services());
    auto* body = &Get()->common;
    body->vptrAt00->drawAt60(body, clip);
}
void SceneManagerOptionCharNativeView::CompositeOwnedAtVslot5C(Slot ownedTarget, const RECT* clip) const
{
    OptionCharSelectDialogNativeScope scope(Services());
    auto* body = &Get()->common;
    body->vptrAt00->compositeAt5C(body, ownedTarget, clip);
}
void SceneManagerOptionCharNativeView::CompositeBorrowedAtVslot5C(const Slot& target, const RECT* clip) const
{
    OptionCharSelectDialogNativeScope scope(Services());
    auto* body = &Get()->common;
    Slot argument;
    CopyBorrowed(argument, &target);
    body->vptrAt00->compositeAt5C(body, argument, clip);
}
void SceneManagerOptionCharNativeView::CompositeSurfaceAtVslot5C(
    GraphicsSurfaceNativeBody* target, const RECT* clip) const
{
    OptionCharSelectDialogNativeScope scope(Services());
    auto* body = &Get()->common;
    Slot argument;
    ConstructGraphicsSurfaceReferenceAt451500(argument, target, 0);
    body->vptrAt00->compositeAt5C(body, argument, clip);
}

SceneManagerOptionCharNativeReference::SceneManagerOptionCharNativeReference(
    const Slot* borrowed, OptionCharSelectDialogNativeDependencies& services) noexcept : services_(&services)
{ CopyBorrowed(reference_, borrowed); }
SceneManagerOptionCharNativeReference::SceneManagerOptionCharNativeReference(
    const SceneManagerOptionCharNativeReference& other) noexcept : services_(other.services_)
{ CopyBorrowed(reference_, &other.reference_); }
SceneManagerOptionCharNativeReference::SceneManagerOptionCharNativeReference(
    SceneManagerOptionCharNativeReference&& other) noexcept
    : reference_(std::exchange(other.reference_, Slot{})), services_(other.services_) {}
SceneManagerOptionCharNativeReference& SceneManagerOptionCharNativeReference::operator=(
    const SceneManagerOptionCharNativeReference& other)
{
    if (this != &other)
    {
        Reset();
        CopyBorrowed(reference_, &other.reference_);
        services_ = other.services_;
    }
    return *this;
}
SceneManagerOptionCharNativeReference& SceneManagerOptionCharNativeReference::operator=(
    SceneManagerOptionCharNativeReference&& other)
{
    if (this != &other)
    {
        Reset();
        reference_ = std::exchange(other.reference_, Slot{});
        services_ = other.services_;
    }
    return *this;
}
SceneManagerOptionCharNativeReference::~SceneManagerOptionCharNativeReference() noexcept { Reset(); }
void SceneManagerOptionCharNativeReference::Reset()
{
    if (!Read(reference_.controlAt08)) return;
    OptionCharSelectDialogNativeScope scope(*services_);
    ReleaseYtlReference(reference_);
}
void SceneManagerOptionCharNativeReference::Reset(const Slot* borrowed,
    OptionCharSelectDialogNativeDependencies& services)
{
    if (borrowed == &reference_) return;
    Reset();
    CopyBorrowed(reference_, borrowed);
    services_ = &services;
}
SceneManagerOptionCharNativeView SceneManagerOptionCharNativeReference::Borrow() const & noexcept
{ return SceneManagerOptionCharNativeView(&reference_, &services_); }
}

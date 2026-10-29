#include <Runtime/NativeEditWindowCollectionRuntime.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/NativeEditWindowRuntime.h>
#include <Runtime/WindowEnvironmentRuntime.h>
#include <Runtime/WindowModeManagerRuntime.h>

#include <algorithm>
#include <bit>
#include <cstdlib>
#include <limits>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = NativeEditWindowReferenceSlotRuntime;
using Allocation = NativeEditCollectionAllocation;
constexpr std::size_t MaximumReferences = 0x15555555;

NativeEditWindowRuntime* Resolve(const Slot& slot) noexcept
{
    if (!slot.baseAt00) return nullptr;
    const auto stride = slot.controlAt08 ? slot.controlAt08->strideAt0C : 0u;
    return reinterpret_cast<NativeEditWindowRuntime*>(
        slot.baseAt00 + slot.biasAt04 * stride);
}

void Copy(Slot& destination, const Slot& source) noexcept
{
    destination.baseAt00 = source.baseAt00;
    destination.biasAt04 = source.biasAt04;
    destination.controlAt08 = source.controlAt08;
    if (destination.controlAt08) ++destination.controlAt08->referenceCountAt04;
}

// 486B60/452290：正文回调之后重读control；异常不得补清槽或恢复计数。
void Release(Slot& slot)
{
    auto* control = slot.controlAt08;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control = slot.controlAt08;
        if (control) control->DeleteControlAtVslot04(1);
    }
    slot.controlAt08 = nullptr;
    slot.baseAt00 = 0;
}

void Assign(Slot& destination, const Slot& source)
{
    if (&destination == &source) return;
    Release(destination);
    Copy(destination, source);
}

void* Allocate(INativeEditWindowCollectionMemoryApi& memory, Allocation kind, std::size_t size)
{
    auto* result = memory.Allocate(kind, size);
    if (!result) throw std::bad_alloc();
    return result;
}

class WindowOwnerControl final : public NativeEditWindowReferenceControlRuntime
{
public:
    WindowOwnerControl(std::shared_ptr<NativeEditWindowRuntime>&& window,
        INativeEditWindowCollectionMemoryApi& memory) noexcept
        : window_(std::move(window)), memory_(memory)
    {
        strideAt0C = 0x98;
    }
    void DestroyPayloadAtVslot00() override { window_.reset(); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 仅支持工厂唯一owner移交；禁止owning被改零后暗中以shared_ptr析构正文。
        if (window_) throw std::logic_error("Native edit bridge requires owning payload release");
        auto& memory = memory_;
        this->~WindowOwnerControl();
        if (flags & 1) memory.Deallocate(Allocation::WindowOwnerBridge, this, sizeof(WindowOwnerControl));
    }
private:
    std::shared_ptr<NativeEditWindowRuntime> window_;
    INativeEditWindowCollectionMemoryApi& memory_;
};

class LayoutControl final : public NativeEditWindowReferenceControlRuntime
{
public:
    LayoutControl(RECT* rectangle, INativeEditWindowCollectionMemoryApi& memory) noexcept
        : rectangle_(rectangle), memory_(memory) { strideAt0C = sizeof(RECT); }
    void DestroyPayloadAtVslot00() override
    {
        memory_.Deallocate(Allocation::LayoutRectangle, rectangle_, sizeof(RECT));
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        auto& memory = memory_;
        this->~LayoutControl();
        if (flags & 1) memory.Deallocate(Allocation::LayoutControl, this, sizeof(LayoutControl));
    }
private:
    RECT* rectangle_;
    INativeEditWindowCollectionMemoryApi& memory_;
};

LONG Add(LONG left, LONG right) noexcept
{
    return std::bit_cast<LONG>(std::bit_cast<std::uint32_t>(left) + std::bit_cast<std::uint32_t>(right));
}

LONG Subtract(LONG left, LONG right) noexcept
{
    return std::bit_cast<LONG>(std::bit_cast<std::uint32_t>(left) - std::bit_cast<std::uint32_t>(right));
}

// 4878A8等：保留IMUL低DWORD，EDX清零后DIV；零分母仍为真实x86除法故障。
LONG Scale(LONG factor, LONG value, int denominator)
{
    LONG result;
    __asm
    {
        mov eax, factor
        imul eax, value
        xor edx, edx
        div denominator
        mov result, eax
    }
    return result;
}
}

void* NativeEditWindowCollectionMemoryApi::Allocate(Allocation kind, std::size_t size)
{
    if (kind != Allocation::ReferenceStorage || size < 0x1000) return ::operator new(size);
    const auto maximum = (std::numeric_limits<std::size_t>::max)();
    const auto allocationSize = size > maximum - 0x23 ? maximum : size + 0x23;
    auto* raw = ::operator new(allocationSize);
    const auto aligned = (reinterpret_cast<std::uintptr_t>(raw) + 0x23) & ~std::uintptr_t{31};
    reinterpret_cast<void**>(aligned)[-1] = raw;
    return reinterpret_cast<void*>(aligned);
}

void NativeEditWindowCollectionMemoryApi::Deallocate(Allocation kind, void* memory, std::size_t size) noexcept
{
    if (!memory) return;
    if (kind == Allocation::ReferenceStorage && size >= 0x1000)
    {
        auto* raw = static_cast<void**>(memory)[-1];
        const auto distance = reinterpret_cast<std::uintptr_t>(memory) - reinterpret_cast<std::uintptr_t>(raw);
        if (distance - sizeof(void*) > 0x1F) _invalid_parameter_noinfo_noreturn();
        memory = raw;
    }
    ::operator delete(memory);
}

BOOL Win32NativeEditWindowCollectionPlatformApi::IsWindowVisible(HWND window) { return ::IsWindowVisible(window); }
BOOL Win32NativeEditWindowCollectionPlatformApi::OffsetRect(RECT& rectangle, int x, int y)
    { return ::OffsetRect(&rectangle, x, y); }

NativeEditWindowReferenceRuntime::NativeEditWindowReferenceRuntime(const NativeEditWindowReferenceRuntime& source) noexcept
    { Copy(slot_, source.slot_); }
NativeEditWindowReferenceRuntime::NativeEditWindowReferenceRuntime(NativeEditWindowReferenceRuntime&& source) noexcept
    : slot_(std::exchange(source.slot_, {})) {}
NativeEditWindowReferenceRuntime& NativeEditWindowReferenceRuntime::operator=(const NativeEditWindowReferenceRuntime& source)
{
    Assign(slot_, source.slot_);
    return *this;
}
NativeEditWindowReferenceRuntime& NativeEditWindowReferenceRuntime::operator=(NativeEditWindowReferenceRuntime&& source)
{
    if (this != &source)
    {
        Release(slot_);
        slot_ = std::exchange(source.slot_, {});
    }
    return *this;
}
NativeEditWindowReferenceRuntime::~NativeEditWindowReferenceRuntime() noexcept { Release(slot_); }
void NativeEditWindowReferenceRuntime::Reset() { Release(slot_); }
NativeEditWindowRuntime* NativeEditWindowReferenceRuntime::Get() const noexcept { return Resolve(slot_); }
NativeEditWindowReferenceRuntime NativeEditWindowReferenceRuntime::AdoptSlot(
    NativeEditWindowReferenceSlotRuntime& slot) noexcept
{
    NativeEditWindowReferenceRuntime result;
    result.slot_ = std::exchange(slot, {});
    return result;
}
NativeEditWindowReferenceRuntime NativeEditWindowReferenceRuntime::AdoptCreatedWindow(
    std::shared_ptr<NativeEditWindowRuntime>&& window, INativeEditWindowCollectionMemoryApi& memory)
{
    if (!window || window.use_count() != 1)
        throw std::invalid_argument("Native edit bridge requires a nonempty unique shared owner");
    auto* storage = Allocate(memory, Allocation::WindowOwnerBridge, sizeof(WindowOwnerControl));
    const auto base = reinterpret_cast<std::uintptr_t>(window.get());
    auto* control = new (storage) WindowOwnerControl(std::move(window), memory);
    NativeEditWindowReferenceSlotRuntime slot{base, 0, control};
    return AdoptSlot(slot);
}

NativeEditWindowCollectionRuntime::NativeEditWindowCollectionRuntime(const WindowEnvironmentRuntime& environment,
    const std::shared_ptr<WindowModeManagerRuntime>& modeSlot,
    INativeEditWindowCollectionPlatformApi& platform, INativeEditWindowCollectionMemoryApi& memory) noexcept
    : environment_(&environment), modeSlot_(&modeSlot), platform_(platform), memory_(memory) {}

NativeEditWindowCollectionRuntime::NativeEditWindowCollectionRuntime(const EnvironmentStartupRuntime& environment,
    RootRuntimeOwner& root, INativeEditWindowCollectionPlatformApi& platform,
    INativeEditWindowCollectionMemoryApi& memory) noexcept
    : startupEnvironment_(&environment), root_(&root), platform_(platform), memory_(memory) {}

NativeEditWindowCollectionRuntime::~NativeEditWindowCollectionRuntime() noexcept
{
    RemoveAt486B60(nullptr);
    memory_.Deallocate(Allocation::ReferenceStorage, storage_, capacity_ * sizeof(Slot));
}

NativeEditWindowRuntime* NativeEditWindowCollectionRuntime::CreateAt46FA30(
    std::uint32_t disable, std::uint32_t alignment, std::uint32_t border)
{
    // 46FA5F/46FB82/46FCFF：对象、Ref控制块、HWND，桥接不得移到CreateWindows之后。
    auto created = std::shared_ptr<NativeEditWindowRuntime>(new NativeEditWindowRuntime());
    auto reference = NativeEditWindowReferenceRuntime::AdoptCreatedWindow(std::move(created), memory_);
    reference.Get()->CreateWindows(disable, alignment, border, CurrentWindow());
    static_cast<void>(RegisterAt46FC43(reference));
    auto* borrowed = reference.Get();
    reference.Reset();
    return borrowed;
}

NativeEditWindowRuntime* NativeEditWindowCollectionRuntime::RegisterAt46FC43(const NativeEditWindowReferenceRuntime& source)
{
    if (size_ == MaximumReferences) throw std::length_error("Native edit reference array too long");
    if (size_ < capacity_)
    {
        Copy(storage_[size_], source.slot_);
        ++size_;
        return source.Get();
    }
    const auto required = size_ + 1;
    const auto grown = capacity_ > MaximumReferences - capacity_ / 2 ? required : capacity_ + capacity_ / 2;
    const auto capacity = (std::max)(required, grown);
    auto* storage = static_cast<Slot*>(Allocate(memory_, Allocation::ReferenceStorage, capacity * sizeof(Slot)));
    for (std::size_t index = 0; index < capacity; ++index) new (storage + index) Slot{};
    const auto oldSize = size_;
    auto* const oldStorage = storage_;
    // 452E60：先插入新项，再复制旧项；Copy只含DWORD读写/增计数，不调用用户代码。
    Copy(storage[oldSize], source.slot_);
    for (std::size_t index = 0; index < oldSize; ++index) Copy(storage[index], oldStorage[index]);
    // 原释放旧范围不在构造catch内；释放异常不回滚、不发布新存储。
    for (std::size_t index = 0; index < oldSize; ++index)
    {
        RequireCursor(oldStorage, index);
        Release(oldStorage[index]);
    }
    memory_.Deallocate(Allocation::ReferenceStorage, oldStorage, capacity_ * sizeof(Slot));
    storage_ = storage;
    size_ = required;
    capacity_ = capacity;
    return source.Get();
}

NativeEditWindowReferenceRuntime NativeEditWindowCollectionRuntime::FindAt479DF0(NativeEditWindowRuntime* target) const noexcept
{
    NativeEditWindowReferenceRuntime result;
    for (std::size_t index = 0; index < size_; ++index)
    {
        if (Resolve(storage_[index]) == target)
        {
            Copy(result.slot_, storage_[index]);
            break;
        }
    }
    return result;
}

void NativeEditWindowCollectionRuntime::RemoveAt486B60(NativeEditWindowRuntime* target)
{
    auto* const storage = storage_;
    const auto end = size_;
    if (!target)
    {
        for (std::size_t index = 0; index < end; ++index)
        {
            RequireCursor(storage, index);
            Release(storage[index]);
        }
        size_ = 0;
        return;
    }
    for (std::size_t index = 0; index < end; ++index)
    {
        if (Resolve(storage[index]) != target) continue;
        for (auto source = index + 1; source < end; ++source)
        {
            RequireCursor(storage, source);
            Assign(storage[source - 1], storage[source]);
        }
        if (!size_) throw std::logic_error("Native edit removal invalidated its tail");
        RequireCursor(storage, size_ - 1);
        Release(storage[size_ - 1]);
        --size_;
        return;
    }
}

WindowModeView NativeEditWindowCollectionRuntime::CurrentMode() const
{
    // 487627/487663以及每个可处理项的487750分别同步；不跨这些位置缓存mode正文。
    if (root_) root_->EnsureCreatedAt413BE0();
    if (root_) return root_->RequirePublished().WindowModeAt0C();
    const auto* mode = modeSlot_->get();
    if (!mode) throw std::logic_error("Native edit layout requires the current mode owner");
    return mode->Borrow();
}

HWND NativeEditWindowCollectionRuntime::CurrentWindow() const
{ return startupEnvironment_ ? reinterpret_cast<HWND>(startupEnvironment_->CaptureAt4180C0().windowAt2C) : environment_->window; }

std::int32_t NativeEditWindowCollectionRuntime::CurrentWidth() const
{
    return startupEnvironment_ ? std::bit_cast<std::int32_t>(startupEnvironment_->WindowFields().widthAt18)
        : environment_->gameProfile.logicalWidth;
}

std::int32_t NativeEditWindowCollectionRuntime::CurrentHeight() const
{
    return startupEnvironment_ ? std::bit_cast<std::int32_t>(startupEnvironment_->WindowFields().heightAt1C)
        : environment_->gameProfile.logicalHeight;
}

void NativeEditWindowCollectionRuntime::RequireCursor(const Slot* storage, std::size_t index) const
{
    // 现代越界保护，不把原失效指针重新定位，也不恢复此前副作用。
    if (storage != storage_ || index >= size_)
        throw std::logic_error("Native edit collection cursor invalidated by reentrancy");
}

void NativeEditWindowCollectionRuntime::UpdateLayoutAt4875F0(NativeEditWindowRuntime* target)
{
    const auto& origin = CurrentMode().RequireViewportAt0C();
    const LONG y = origin[1];
    const LONG x = origin[0];
    const auto& dimensions = CurrentMode().RequireViewportAt0C();
    const LONG height = dimensions[3];
    const LONG width = dimensions[2];
    auto* const storage = storage_;
    for (std::size_t index = 0; index < size_; ++index)
    {
        RequireCursor(storage, index);
        auto* object = Resolve(storage[index]);
        if (target)
        {
            if (object != target) continue;
        }
        else
        {
            if (!object) throw std::logic_error("Native edit layout resolved a null object");
            const BOOL visible = platform_.IsWindowVisible(object->Window());
            RequireCursor(storage, index);
            if (!visible) continue;
        }
        object = Resolve(storage[index]);
        if (!object) throw std::logic_error("Native edit layout resolved a null object");
        RECT rectangle = object->LogicalRectAt4C();
        NativeEditWindowReferenceRuntime marginsOwner;
        RECT* margins = nullptr;
        if (CurrentMode().ModeAt04() != 0)
        {
            const LONG logicalWidth = Subtract(rectangle.right, rectangle.left);
            const LONG logicalHeight = Subtract(rectangle.bottom, rectangle.top);
            rectangle.left = Scale(width, rectangle.left, CurrentWidth());
            rectangle.top = Scale(height, rectangle.top, CurrentHeight());
            rectangle.right = Add(Scale(width, logicalWidth, CurrentWidth()), rectangle.left);
            rectangle.bottom = Add(Scale(height, logicalHeight, CurrentHeight()), rectangle.top);
            static_cast<void>(platform_.OffsetRect(rectangle, x, y));
            RequireCursor(storage, index);
            object = Resolve(storage[index]);
            if (!object) throw std::logic_error("Native edit layout resolved a null object");
            const RECT defaults = object->DefaultMarginsAt5C();
            margins = new (Allocate(memory_, Allocation::LayoutRectangle, sizeof(RECT))) RECT{};
            // 控制块失败时原base已存在但不受控；不使用unique_ptr补回滚这个前缀。
            marginsOwner.slot_.baseAt00 = reinterpret_cast<std::uintptr_t>(margins);
            auto* control = Allocate(memory_, Allocation::LayoutControl, sizeof(LayoutControl));
            marginsOwner.slot_.controlAt08 = new (control) LayoutControl(margins, memory_);
            // 487D71..48802C：OffsetRect和两次分配可能重入，边距必须重新读环境尺寸。
            margins->left = Scale(width, defaults.left, CurrentWidth());
            margins->top = Scale(height, defaults.top, CurrentHeight());
            margins->right = Scale(width, defaults.right, CurrentWidth());
            margins->bottom = Scale(height, defaults.bottom, CurrentHeight());
        }
        RequireCursor(storage, index);
        object = Resolve(storage[index]);
        if (!object) throw std::logic_error("Native edit layout resolved a null object");
        object->ApplyLayoutAt4880F0(rectangle, margins);
    }
}
}

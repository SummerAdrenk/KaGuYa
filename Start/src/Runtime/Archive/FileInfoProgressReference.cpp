#include <Runtime/FileInfoProgressReference.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Logging/Logger.h>

#include <exception>
#include <cstring>
#include <new>
#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 5B3768={492C20,4679E0}：共用既有18h非数组control布局，不附加现代owner或服务。
class ProgressControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ProgressControl(IFileInfoProgress* payload) noexcept
        : EnvironmentObjectReferenceControl(payload) {}

    void DestroyPayloadAtVslot00() override
    {
        // 492C20由payload+10删除；正文仍为现代动态类型，不硬编码10h释放尺寸。
        delete static_cast<IFileInfoProgress*>(payloadAt14);
    }

    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        // 4679E0先退化control虚表，再按bit0释放18h；此void接口不复刻EAX=this。
        RetireEnvironmentObjectControl(this, flags);
    }
};
static_assert(sizeof(ProgressControl) == 0x18);

[[noreturn]] void MissingProgress()
{
    KaGuYa::Logging::Write(KaGuYa::Logging::Level::Error,
        "FileInfoProgress Ref 466520: null payload");
    throw std::logic_error("FileInfoProgress reference has no payload");
}
IFileInfoProgress& RequireProgress(const YtlReferenceSlotRuntime& reference)
{
    auto* progress = ResolveFileInfoProgressReference(reference);
    if (!progress)
    {
        MissingProgress();
    }
    return *progress;
}
// 原5B2A28的四业务槽；不把真正10h正文强转为现代IFileInfoProgress对象。
// 既有受控对象的四槽ABI已独立验证相同，删除槽仍由各自owning control负责。
struct BusinessMethods final
{
    BYTE(__thiscall* poll)(void*);
    LRESULT(__thiscall* step)(void*);
    LRESULT(__thiscall* range)(void*, std::uint32_t);
    HWND(__thiscall* window)(const void*);
};
const BusinessMethods* Methods(void* body) noexcept
{
    const BusinessMethods* methods;
    std::memcpy(&methods, body, sizeof(methods));
    return methods;
}
void* RequireBusinessProgress(const YtlReferenceSlotRuntime& reference)
{
    // 旧空正文安全边界暂保留，不把它宣称为原硬件故障语义。
    auto* body = ResolveEnvironmentObjectReference<void>(reference);
    if (!body) MissingProgress();
    return body;
}
}

IFileInfoProgress* ResolveFileInfoProgressReference(const YtlReferenceSlotRuntime& reference) noexcept
{
    return ResolveEnvironmentObjectReference<IFileInfoProgress>(reference);
}

FileInfoProgressReference::FileInfoProgressReference(const YtlReferenceSlotRuntime& source) noexcept
    : reference(source)
{
    if (reference.controlAt08) ++reference.controlAt08->referenceCountAt04;
}

FileInfoProgressReference::FileInfoProgressReference(const FileInfoProgressReference& source) noexcept
    : FileInfoProgressReference(source.reference) {}

FileInfoProgressReference::FileInfoProgressReference(FileInfoProgressReference&& source) noexcept
    : reference(std::exchange(source.reference, {})) {}

FileInfoProgressReference& FileInfoProgressReference::operator=(const FileInfoProgressReference& source) noexcept
{
    if (this != &source)
    {
        reset();
        reference = source.reference;
        if (reference.controlAt08) ++reference.controlAt08->referenceCountAt04;
    }
    return *this;
}

FileInfoProgressReference& FileInfoProgressReference::operator=(FileInfoProgressReference&& source) noexcept
{
    if (this != &source)
    {
        reset();
        reference = std::exchange(source.reference, {});
    }
    return *this;
}

FileInfoProgressReference::~FileInfoProgressReference() noexcept { reset(); }

void FileInfoProgressReference::AdoptAt489DF5(IFileInfoProgress* created)
{
    reset();
    reference = {};
    if (!created) return;
    // 489E1B先发布base；489E1E失败时没有正文清理态，不加任何RAII回收。
    reference.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    // 原控制块没有整块初始化；普通new在/sdl下会额外清零未知字节。
    void* storage = ::operator new(sizeof(ProgressControl));
    auto* control = storage ? ::new (storage) ProgressControl(created) : nullptr;
    control->referenceCountAt04 = 1;
    control->owningAt08 = 1;
    control->strideAt0C = 0x10;
    reference.controlAt08 = control;
    control->countAt10 = 1;
}

IFileInfoProgress* FileInfoProgressReference::get() const noexcept
{
    return ResolveFileInfoProgressReference(reference);
}

IFileInfoProgress* FileInfoProgressReference::operator->() const { return &RequireProgress(reference); }
FileInfoProgressReference::operator bool() const noexcept { return get() != nullptr; }

void FileInfoProgressReference::reset() noexcept
{
    // 46075C..460781与公共释放协议同序：回调可改control，不能提前清槽或清bias。
    ReleaseYtlReference(reference);
}

FileInfoProgressReferenceView::FileInfoProgressReferenceView(
    const YtlReferenceSlotRuntime& reference) noexcept : reference_(&reference) {}

bool FileInfoProgressReferenceView::PollCancelAt47F0B0()
{
    auto* body = RequireBusinessProgress(*reference_);
    return Methods(body)->poll(body) != 0;
}

LRESULT FileInfoProgressReferenceView::StepAt46E820()
{
    auto* body = RequireBusinessProgress(*reference_);
    return Methods(body)->step(body);
}

LRESULT FileInfoProgressReferenceView::AddRangeAt46A5A0(std::uint32_t delta)
{
    auto* body = RequireBusinessProgress(*reference_);
    return Methods(body)->range(body, delta);
}

HWND FileInfoProgressReferenceView::WindowAt47ADF0() const noexcept
{
    auto* progress = ResolveEnvironmentObjectReference<void>(*reference_);
    if (!progress)
    {
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Error,
            "FileInfoProgress Ref 47ADF0: null payload in noexcept window access");
        std::terminate();
    }
    return Methods(progress)->window(progress);
}
}

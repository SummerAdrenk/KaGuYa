#include "SaveLoadDialogNative.Persistence.Internal.h"
#include <Runtime/SaveFileCatalog.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace SaveLoadPersistenceDetail;
namespace
{
template<unsigned Kind>
class FactoryControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit FactoryControl(void* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override
    {
        auto* body = static_cast<SaveFileDeleteFactoryNativeBody0Ch*>(payloadAt14);
        // 原4D79B0/4D7A50/4D7AF0持不同控制表，payload槽都调用实际factory虚析构(flags1)。
        // 53D633明确检查payload为空；这是原分支，不是现代兜底。
        if (body) body->vptrAt00->deletingDestructorAt00(body, 1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }
};
template<unsigned Kind>
void Adopt(Slot& destination, void* body, IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(destination);
    if (!body) return;
    if (destination.controlAt08) destination.controlAt08->DeleteControlAtVslot04(1);
    destination.biasAt04 = 0; destination.baseAt00 = reinterpret_cast<std::uintptr_t>(body);
    static_assert(sizeof(FactoryControl<Kind>) == 0x18);
    auto* raw = memory.AllocateBody(0x18);
    auto* control = raw ? ::new (raw) FactoryControl<Kind>(body) : nullptr;
    destination.controlAt08 = control;
    control->referenceCountAt04 = 1;
    destination.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(destination.controlAt08)->strideAt0C = Kind == 0 ? 12 : 16;
    static_cast<EnvironmentObjectReferenceControl*>(destination.controlAt08)->countAt10 = 1;
}
}
void CreateSaveLoadFileFactoryNativeForSelection(Slot& destination, const std::int32_t& sourceIndex,
    ISaveFileFactoryNativeIoSource& source, IEnvironmentRootMemory& memory)
{
    CreateSaveLoadFileFactoryNativeForSelectionBranch(destination, sourceIndex, sourceIndex, source, memory);
}
void CreateSaveLoadFileFactoryNativeForSelectionBranch(Slot& destination, std::int32_t selected,
    const std::int32_t& sourceIndex, ISaveFileFactoryNativeIoSource& source, IEnvironmentRootMemory& memory)
{
    const auto bytes = selected == -1 || selected == -2 ? std::size_t{0x10} : std::size_t{0x0C};
    auto* raw = memory.AllocateBody(bytes);
    void* created = nullptr;
    if (raw)
    {
        try
        {
            if (selected == -1 || selected == -2)
            {
                auto* body = ::new (raw) SaveFileDeleteFactoryNativeRollingBody10h;
                ConstructRollingSaveFileDeleteFactoryNativeAt45B800(*body, selected == -2, source);
                created = &body->base;
            }
            else
            {
                auto* body = ::new (raw) SaveFileDeleteFactoryNativeBody0Ch;
                const auto* catalog = static_cast<SaveFileStateNativeArchive*>(
                    ResolveEnvironmentArchiveAt417990(*source.CaptureEnvironmentAt4180C0(), 6));
                ConstructRegularSaveFileDeleteFactoryNativeAt4DCDB0(*body,
                    SaveFilePhysicalPageNumber(*catalog, sourceIndex), source);
                created = body;
            }
        }
        catch (...) { [&]() noexcept { memory.DeleteBody(raw, bytes); }(); throw; }
    }
    // 完整正文构造之后不再有raw回收资格；控制块分配失败保持已发布孤儿正文。
    if (selected == -2) Adopt<2>(destination, created, memory);
    else if (selected == -1) Adopt<1>(destination, created, memory);
    else Adopt<0>(destination, created, memory);
}
std::int32_t ExecuteSaveLoadLoadNativeAt511FB0(SaveLoadDialogNativeBody184h& dialog,
    ISaveLoadDialogNativePersistenceSource& source, IEnvironmentRootMemory& memory)
{
    Slot factory;
    ReferenceUnwind unwind{factory};
    CreateSaveLoadFileFactoryNativeForSelection(factory, dialog.sourceIndexAt160, source, memory);
    auto* cached = factory.controlAt08;
    const auto error = LoadFileFactoryNativeAt480830(
        *ResolveEnvironmentObjectReference<SaveFileDeleteFactoryNativeBody0Ch>(factory),
        Increment(dialog.slotIndexAt164), source);
    std::int32_t result;
    if (error)
    {
        auto* env = source.CaptureEnvironmentAt4180C0();
        (void)source.MessageBoxA(reinterpret_cast<HWND>(env->windowAt2C), loadFailure, env->titleAt10.GetString(), MB_ICONERROR);
        result = 0;
    }
    else { dialog.resultAt68 = 1; result = 1; }
    unwind.FinishCached(cached);
    return result;
}
}

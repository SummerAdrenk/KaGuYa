#include "SaveLoadSceneNative.Internal.h"
#include <bit>
#include <new>

namespace KaGuYa::Start::Runtime
{
using namespace SaveLoadSceneNativeDetail;
namespace
{
struct TaskUnwind final
{
    Body& body; bool armed = true;
    ~TaskUnwind() noexcept { if (armed) RetireTask(body); }
};
struct RefUnwind final
{
    YtlReferenceSlotRuntime& slot; bool armed = true;
    ~RefUnwind() noexcept { if (armed) ReleaseYtlReference(slot); }
};
struct RawUnwind final
{
    void* raw; std::size_t size; IEnvironmentRootMemory& memory; bool armed = true;
    ~RawUnwind() noexcept { if (armed) memory.DeleteBody(raw, size); }
};
std::uint32_t& Registry(ISaveLoadSceneNativeSource& source, std::uint32_t site)
{
    auto* environment = source.CaptureEnvironmentAt4180C0(site);
    // ctor内联slot2先读end再begin，先stride再bias；Update调417990，读序不同，不合并。
    const auto end = reinterpret_cast<std::uintptr_t>(Read(environment->archivesAt30.end));
    const auto begin = reinterpret_cast<std::uintptr_t>(Read(environment->archivesAt30.begin));
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(std::uint32_t(end - begin)) / 12);
    std::uintptr_t archive = 0;
    if (count > 2)
    {
        const auto* slot = reinterpret_cast<const YtlReferenceSlotRuntime*>(begin + 0x18u);
        const auto base = Read(slot->baseAt00);
        if (base)
        {
            auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot->controlAt08));
            const auto stride = control ? Read(control->strideAt0C) : 0u;
            archive = base + Read(slot->biasAt04) * stride;
        }
    }
    return *reinterpret_cast<std::uint32_t*>(archive + 4u);
}
Body* NewScene(std::uint32_t mode, SaveLoadSceneNativeServices& services)
{
    SaveLoadSceneNativeScope scope(services);
    auto* raw = services.memory.AllocateBody(0x24);
    if (!raw) return nullptr;
    RawUnwind cleanup{raw, 0x24, services.memory};
    auto* result = ConstructSaveLoadSceneNativeAt4A17B0(*::new(raw) Body, mode, services);
    cleanup.armed = false;
    return result;
}
}
Body* ConstructSaveLoadSceneNativeAt4A17B0(Body& body, std::uint32_t modeArgument,
    SaveLoadSceneNativeServices& services)
{
    SaveLoadSceneNativeScope scope(services);
    body.vptrAt00 = &SaveLoadSceneNativeMethods();
    TaskUnwind task{body};
    body.dialogAt08.baseAt00 = 0;
    body.dialogAt08.biasAt04 = 0;
    body.dialogAt08.controlAt08 = nullptr;
    RefUnwind reference{body.dialogAt08};
    const auto mode = static_cast<std::uint8_t>(modeArgument);
    body.modeAt18 = mode;
    // PE 5EF380仅捕获try-state2..3；catch恢复成功后，原Ref保留在当前槽中。
    try
    {
        auto* raw = services.memory.AllocateBody(0x184);
        GuiDialogNativeBody5Ch* created = nullptr;
        if (raw)
        {
            RawUnwind dialog{raw, 0x184, services.memory};
            const auto rootByte = services.source.ReadRootByteAt1AF();
            created = services.dialog.ConstructAt4EB950(raw, mode, rootByte);
            // 原正常NULL返回不触发raw delete184。
            dialog.armed = false;
        }
        AdoptDialogAt4A1850(body.dialogAt08, created, services.memory);
        body.savedRegistryAt14 = Read(Registry(services.source, 0x4A18E2));
        *static_cast<volatile std::uint32_t*>(&Registry(services.source, 0x4A1933)) = 1;
        body.savedRootByteAt21 = services.source.ReadRootByteAt219();
        services.source.WriteRootByteAt219(0, 0x4A19A2);
        body.phaseAt04 = 0;
    }
    catch (...)
    {
        // 4A19BC重新捕获root -> 4186D0 -> 47E180；仅恢复再抛出才进入
        // state4 -> state1释放Ref，随后state0退役ITask，不做全局字段回滚。
        services.source.ResetAndQueueTitleAt47E180();
    }
    reference.armed = task.armed = false;
    return &body;
}
Body* NewSaveSceneNativeAt4AB3A2(SaveLoadSceneNativeServices& services) { return NewScene(1, services); }
Body* NewLoadSceneNativeAt4AB40B(SaveLoadSceneNativeServices& services) { return NewScene(0, services); }
}

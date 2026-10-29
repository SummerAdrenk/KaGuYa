#include <Runtime/RestoredScriptNative.h>

#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/ScrVmScriptCommandState.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
static_assert(offsetof(ScriptNativeBody, commandAt24) + 2 == 0x26);
static_assert(offsetof(ScriptNativeBody, parserAtA0) + offsetof(ScrContainer, bytesAt04) == 0xA4);
static_assert(offsetof(ScriptNativeBody, parserAtA0) + offsetof(ScrContainer, saveCountAt20) == 0xC0);
static_assert(offsetof(ScriptNativeBody, parserAtA0) + offsetof(ScrContainer, saveOffsetAt24) == 0xC4);
namespace
{
// 使用volatile逐原点读取，不把raw故障、control空或32位回绕改成现代边界异常。
std::uintptr_t SaveParams2Address(const ScriptParamsNativeBody* params) noexcept
{
    const volatile auto& slot = params->saveParams2At30;
    const auto base = slot.baseAt00;
    if (!base) return 0;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto stride = control ? control->strideAt0C : 0u;
    const auto bias = slot.biasAt04;
    return base + bias * stride;
}

IEnvironmentArchiveRuntime* Archive(IRestoredScriptNativeServices& services, std::uint32_t slot)
{
    auto* const environment = services.CaptureEnvironmentAt4180C0();
    return ResolveEnvironmentArchiveAt417990(*environment, slot);
}

const char* NameAt(IEnvironmentArchiveRuntime* archive, std::uint32_t offset) noexcept
{
    const auto address = reinterpret_cast<std::uintptr_t>(archive) + offset;
    return *reinterpret_cast<const char* const volatile*>(address);
}

void WriteByte(IEnvironmentArchiveRuntime* archive, std::uint32_t offset, std::uint8_t value) noexcept
{
    *reinterpret_cast<volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(archive) + offset) = value;
}

void WriteDword(IEnvironmentArchiveRuntime* archive, std::uint32_t offset, std::uint32_t value) noexcept
{
    *reinterpret_cast<volatile std::uint32_t*>(reinterpret_cast<std::uintptr_t>(archive) + offset) = value;
}

std::uint32_t ReadSaveOffset(const ScriptNativeBody& receiver, std::int32_t index) noexcept
{
    const volatile auto& slot = receiver.parserAtA0.bytesAt04.RawSlot();
    const auto base = slot.baseAt00;
    std::uintptr_t address = 0;
    if (base)
    {
        const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
        const auto stride = control ? control->strideAt0C : 0u;
        const auto bias = slot.biasAt04;
        const auto table = static_cast<const volatile std::uint32_t&>(receiver.parserAtA0.saveOffsetAt24);
        // 4D4ED8..4D4EE4：(bias + index*4 + SAVEoffset)*stride，全程DWORD回绕。
        address = base + (bias + std::bit_cast<std::uint32_t>(index) * 4u + table) * stride;
    }
    // base空仍执行4D4EE6的[0]读；负index合法通过signed比较后亦直接读原地址。
    return *reinterpret_cast<const volatile std::uint32_t*>(address);
}
}

std::int32_t RestoreScriptNativeAt4D4CC0(ScriptNativeBody& receiver,
    IRestoredScriptNativeServices& services)
{
    // 4D4CC3..4D4D0D：先当前根/Params/SaveParams2+04，再给选定CScript加载。
    const auto loadRoot = services.CaptureAfterEnsureAt413BE0();
    auto* const loadParams = loadRoot.paramsAt47BDB0(loadRoot.object);
    const auto id = *reinterpret_cast<const volatile std::int32_t*>(SaveParams2Address(loadParams) + 4u);
    if (services.LoadScriptAt4D3C90(receiver, id) == 0) return 2;

    // 4D4D0E..4D4D7B：重新取根，选+18，然后47E940；两次人名各自取Env。
    const auto drawRoot = services.CaptureAfterEnsureAt413BE0();
    const auto draw = drawRoot.drawLayersAt18(drawRoot.object);
    services.ResetDrawOwnersAt47E940(draw);
    const char* const firstName = NameAt(Archive(services, 1), 0x58);
    services.SetGraphicsTextFirstNameAt4D6CD0(firstName);
    const char* const secondName = NameAt(Archive(services, 1), 0x5C);
    services.SetGraphicsTextSecondNameAt4D6DB0(secondName);

    // 4D4D80..4D4E2F：每个字段独立4180C0/417990，不能合并Env捕获或写宽度。
    WriteByte(Archive(services, 2), 0x0C, 1);
    WriteDword(Archive(services, 2), 0x18, 1);
    WriteDword(Archive(services, 2), 0x14, 1);
    WriteDword(Archive(services, 2), 0x1C, 1);
    WriteDword(Archive(services, 2), 0x20, 1);
    WriteByte(Archive(services, 2), 0x08, 1);
    WriteByte(Archive(services, 2), 0x09, 1);
    WriteByte(Archive(services, 2), 0x28, 1);
    WriteByte(Archive(services, 2), 0x29, 0);

    // 4D4E33..4D4E64：ESI先固定根；slot0/name查询之后才在该根上GetParams。
    const auto replayRoot = services.CaptureAfterEnsureAt413BE0();
    auto* const namedReceiver = Archive(services, 0);
    const auto replay = services.ResolveNamedValueAt498020(namedReceiver, "replay");
    auto* const replayParams = replayRoot.paramsAt47BDB0(replayRoot.object);
    services.WriteValueAt4C8220(*replayParams, replay, 0);
    static_cast<volatile std::uint8_t&>(receiver.commandAt24.bytes[2]) = 1;
    // 非原业务写：只发布刚写入字段的合法性，不复制正文或重新取当前根。
    services.OnPublishCommandFields(receiver, ScrVmScriptCommandState::StateByteAt26Mask);

    // 4D4E6D..4D4EE6：+26已写1；在新当前根读取存档索引，signed index<count。
    const auto indexRoot = services.CaptureAfterEnsureAt413BE0();
    auto* const indexParams = indexRoot.paramsAt47BDB0(indexRoot.object);
    const auto index = *reinterpret_cast<const volatile std::int32_t*>(SaveParams2Address(indexParams) + 8u);
    std::uint32_t pc = 0xFFFFFFFFu;
    const auto count = static_cast<const volatile std::uint32_t&>(receiver.parserAtA0.saveCountAt20);
    if (index < std::bit_cast<std::int32_t>(count)) pc = ReadSaveOffset(receiver, index);

    // 4D4EE8..4D4F15：PC再取当前根；写完PC才flags=3；异常不自动清+26。
    const auto pcRoot = services.CaptureAfterEnsureAt413BE0();
    auto* const pcParams = pcRoot.paramsAt47BDB0(pcRoot.object);
    static_cast<volatile std::uint32_t&>(pcParams->programCounter) = pc;
    static_cast<volatile std::uint32_t&>(receiver.commandAt04.flags) = 3;
    services.OnPublishCommandFields(receiver, ScrVmScriptCommandState::FlagsAt04Mask);
    const auto intRoute = services.DispatchAt4D3FF0(receiver);
    static_cast<volatile std::uint8_t&>(receiver.commandAt24.bytes[2]) = 0;
    services.OnPublishCommandFields(receiver, ScrVmScriptCommandState::StateByteAt26Mask);
    return intRoute;
}
}

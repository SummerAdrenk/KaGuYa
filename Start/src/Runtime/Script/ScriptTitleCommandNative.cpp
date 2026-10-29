#include <Runtime/ScriptTitleCommandNative.h>
#include <Runtime/ScriptControlCommandNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/RootRuntimeOwner.h>

#include <bit>
#include <cstring>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
union TextSlot
{
    RuntimeCString value;
    TextSlot() noexcept {}
    ~TextSlot() noexcept {}
};
struct CommandCleanup
{
    YtlReferenceSlotRuntime* slot;
    ~CommandCleanup() noexcept { if (slot) UnwindScriptControlCommand(*slot); }
};
struct TextCleanup
{
    RuntimeCString* text;
    ~TextCleanup() noexcept { if (text) DestroyRuntimeCStringAt40CD40(text); }
};
void DeleteConstructing(IEnvironmentRootMemory& memory, void* body) noexcept
{ memory.DeleteBody(body, 4); }
}

std::int32_t ExecuteScriptTitleAt4D25A0(ScrContainer& parser,
    IScriptTitleCommandSource& source, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)
{
    void* allocation = memory.AllocateBody(4);
    auto* body = allocation ? ::new (allocation) ScriptControlCommandNative : nullptr;
    std::uintptr_t allocationBase = 0;
    std::uint64_t extent = 0;
    bool knownExtent = false;
    if (body)
    {
        try
        {
            const auto root = source.CaptureAfterEnsureAt413BE0();
            const auto pc = root.readPcAt47BDB0(root.object);
            body->source = reinterpret_cast<const std::byte*>(parser.bytesAt04.DataAt(pc));
            const auto& bytes = parser.bytesAt04.RawSlot();
            allocationBase = bytes.baseAt00;
            if (bytes.controlAt08)
            {
                const auto* control = static_cast<const EnvironmentObjectReferenceControl*>(bytes.controlAt08);
                extent = std::uint64_t{control->countAt10} * control->strideAt0C;
                knownExtent = true;
            }
        }
        catch (...) { DeleteConstructing(memory, body); throw; }
    }
    YtlReferenceSlotRuntime reference;
    // 4D2648先撤构造清理资格；control分配失败不会自动回收body。
    CreateScriptControlCommandReference(reference, body, ScriptControlCommandKind::Title, memory);
    auto* const capturedControl = reference.controlAt08;
    CommandCleanup commandCleanup{&reference};
    const auto readSource = [&](std::uint32_t offset, std::uint32_t size)
    {
        if (!body || !body->source)
            throw std::logic_error("TITLE null command/source (modern safety boundary)");
        const auto pointer = reinterpret_cast<std::uintptr_t>(body->source);
        const auto relative = static_cast<std::uint32_t>(pointer - allocationBase);
        if (knownExtent && std::uint64_t{relative} + offset + size > extent)
            throw std::out_of_range("TITLE source outside allocation (modern safety boundary)");
        return reinterpret_cast<const void*>(pointer + offset);
    };
    const auto length = *static_cast<const std::uint8_t*>(readSource(4, 1));
    TextSlot text;
    InitializeEmptyRuntimeCString(text.value, strings);
    TextCleanup textCleanup{&text.value};
    auto* chars = const_cast<char*>(text.value.GetString());
    auto* header = reinterpret_cast<ATL::CStringData*>(chars) - 1;
    const auto capacityDelta = static_cast<std::uint32_t>(header->nAllocLength) - length;
    const auto refsDelta = 1u - static_cast<std::uint32_t>(header->nRefs);
    if (std::bit_cast<std::int32_t>(capacityDelta | refsDelta) < 0)
    {
        PrepareRuntimeCStringWriteAt42D590(text.value, length);
        chars = const_cast<char*>(text.value.GetString());
        header = reinterpret_cast<ATL::CStringData*>(chars) - 1;
    }
    if (length > header->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength = length;
    chars[length] = 0;
    // 4D2715在manager回调之后重读正文source，不提前缓存payload或扫描NUL。
    std::memmove(chars, readSource(5, length), length);
    const auto capacity = static_cast<std::size_t>(header->nAllocLength);
    const auto* nul = static_cast<const char*>(std::memchr(chars, 0, capacity));
    const auto measured = nul ? static_cast<int>(nul - chars) : header->nAllocLength;
    if (measured < 0 || measured > header->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength = measured;
    chars[measured] = 0;
    const auto target = source.CaptureAfterEnsureAt413BE0();
    auto& destination = target.titleAt47BDB0(target.object);
    SetRuntimeCStringAt437680(destination, chars, static_cast<int>(std::strlen(chars)));

    // EH5F23D8：正常Free前state2->1，Free失败只展开Ref，不重试CString减持。
    textCleanup.text = nullptr;
    ReleaseRuntimeCStringHeader(header);
    commandCleanup.slot = nullptr;
    YtlReferenceSlotRuntime captured{0, 0, capturedControl};
    ReleaseScriptDispatchCommandAt4D40C6(captured);
    return 0;
}

ScriptTitleRootReceiver RootScriptTitleCommandSource::CaptureAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return {&root_.RequirePublished(), [](void* p)
    { return static_cast<RootRuntimePayload*>(p)->ParamsAt47BDB0().programCounter; },
    [](void* p) -> RuntimeCString&
    { return static_cast<RootRuntimePayload*>(p)->ParamsAt47BDB0().SaveParamsAt08().saveTitleAt08; }};
}
}

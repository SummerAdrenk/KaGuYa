#include <Runtime/ScriptInvalidCommandNative.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/EnvironmentStartupRuntime.h>

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
class CommandUnwind final
{
public:
    explicit CommandUnwind(YtlReferenceSlotRuntime& slot) noexcept : slot_(&slot) {}
    ~CommandUnwind() noexcept { if (slot_) UnwindScriptInvalidCommandAt4C98E0(*slot_); }
    void Disarm() noexcept { slot_ = nullptr; }
private:
    YtlReferenceSlotRuntime* slot_;
};
class TextUnwind final
{
public:
    explicit TextUnwind(RuntimeCString& text) noexcept : text_(&text) {}
    ~TextUnwind() noexcept { if (text_) DestroyRuntimeCStringAt40CD40(text_); }
    void Disarm() noexcept { text_ = nullptr; }
private:
    RuntimeCString* text_;
};
void DeleteConstructingCommand(IEnvironmentRootMemory& memory, void* body) noexcept
{ memory.DeleteBody(body, 4); }
}

void UnwindScriptInvalidCommandAt4C98E0(YtlReferenceSlotRuntime& slot) noexcept
{ ReleaseYtlReference(slot); }

std::int32_t ExecuteScriptInvalidAt4CDC00(ScrContainer& parser,
    IScriptExecutionRootSource& root, IScrVmCommandErrorSink& errors,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    void* storage = memory.AllocateBody(4);
    auto* body = storage ? ::new (storage) ScriptSingleCommandNative : nullptr;
    std::uintptr_t allocationBase = 0;
    std::uint64_t extent = 0;
    bool knownExtent = false;
    if (body)
    {
        try
        {
            auto receiver = root.CaptureAfterEnsureAt413BE0();
            const auto pc = receiver.programCounterAt47BDB0(receiver.object);
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
        catch (...)
        {
            DeleteConstructingCommand(memory, body);
            throw;
        }
    }
    YtlReferenceSlotRuntime reference;
    // control分配失败时，原state0已经撤销；不补回收孤立正文。
    CreateScriptSingleCommandReferenceAt4C8490(reference, body, 1, memory);
    CommandUnwind commandUnwind(reference);
    TextSlot text;
    InitializeEmptyRuntimeCString(text.value, strings);
    auto* const capturedControl = reference.controlAt08;
    TextUnwind textUnwind(text.value);
    const auto* command = ResolveEnvironmentObjectReference<ScriptSingleCommandNative>(reference);
    if (!command || !command->source)
        throw std::logic_error("CScript invalid null command read (modern safety boundary)");
    const auto pointer = reinterpret_cast<std::uintptr_t>(command->source);
    const auto offset = static_cast<std::uint32_t>(pointer - allocationBase);
    if (knownExtent && std::uint64_t{offset} + 2 > extent)
        throw std::out_of_range("CScript invalid opcode outside allocation (modern safety boundary)");
    std::uint16_t opcode;
    std::memcpy(&opcode, reinterpret_cast<const void*>(pointer), 2);
    // 原PE 5B5DCC，CP932；不替换为UTF-8或本地化后的现代错误模板。
    constexpr char format[] = "\x83\x58\x83\x4E\x83\x8A\x83\x76\x83\x67"
        "\x83\x52\x83\x7D\x83\x93\x83\x68\x8F\x88\x97\x9D\x82\xAA"
        "\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD\x81\x49 command : %04X";
    static_assert(sizeof(format) == 54);
    FormatRuntimeCStringAt417130(text.value, format, static_cast<unsigned>(opcode));
    const auto& environment = errors.CaptureAt4180C0();
    const char* const cachedText = text.value.GetString();
    const char* const caption = environment.titleAt10.GetString();
    const HWND window = reinterpret_cast<HWND>(environment.windowAt2C);
    static_cast<void>(errors.ShowMessageBoxA(window, cachedText, caption, MB_ICONERROR));

    // 4CDD23先撤CString清理资格；Free异常只展开Ref，不能再次减持这个header。
    textUnwind.Disarm();
    ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(cachedText) - 16u));
    commandUnwind.Disarm();
    YtlReferenceSlotRuntime captured{0, 0, capturedControl};
    ReleaseScriptDispatchCommandAt4D40C6(captured);
    return 2;
}

EnvironmentInvalidOpcodeRuntime::EnvironmentInvalidOpcodeRuntime(EnvironmentStartupRuntime& environment,
    decltype(&::MessageBoxA) messageBox) noexcept : environment_(environment), messageBox_(messageBox) {}
EnvironmentRootStorage& EnvironmentInvalidOpcodeRuntime::CaptureAt4180C0()
{ return environment_.CaptureAt4180C0(); }
int EnvironmentInvalidOpcodeRuntime::ShowMessageBoxA(HWND window, const char* text,
    const char* caption, UINT type)
{ return messageBox_(window, text, caption, type); }
}

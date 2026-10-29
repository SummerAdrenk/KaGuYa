#include <Runtime/EnvironmentErrorRuntime.h>
#include <Runtime/RuntimeCStringVector.h>

#include <cstdint>

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

class TextUnwind final
{
public:
    explicit TextUnwind(RuntimeCString& value) noexcept : value_(&value) {}
    ~TextUnwind() noexcept { if (value_) DestroyRuntimeCStringAt40CD40(value_); }
    void Disarm() noexcept { value_ = nullptr; }
private:
    RuntimeCString* value_;
};

ATL::CStringData* Header(const char* characters) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(reinterpret_cast<std::uintptr_t>(characters) - 16u);
}

std::uint32_t ErrorCount(const EnvironmentRootStorage& receiver) noexcept
{
    const auto bytes = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(receiver.errorsAt3C.end) -
        reinterpret_cast<std::uintptr_t>(receiver.errorsAt3C.begin));
    // 479F29/479FA4为DWORD相减后SAR；不对空指针作C++指针相减。
    return (bytes >> 2u) | ((bytes & 0x80000000u) ? 0xC0000000u : 0u);
}
}

void AppendEnvironmentErrorVAt4769B0(EnvironmentRootStorage& receiver,
    const char* format, va_list args, IRuntimeStringManagerSource& strings,
    IEnvironmentRootMemory& memory)
{
    TextSlot temporary;
    InitializeEmptyRuntimeCString(temporary.value, strings);
    TextUnwind unwind(temporary.value);
    FormatRuntimeCStringVAt417150(temporary.value, format, args);
    const bool growth = receiver.errorsAt3C.end == receiver.errorsAt3C.capacity;
    const char* cached = temporary.value.GetString();
    // 476A11与41D5E9快路同构；4085C0增长后476A38才重取临时chars。
    AppendRuntimeCStringAt41D5E9(receiver.errorsAt3C, temporary.value, memory);
    if (growth) cached = temporary.value.GetString();
    // 476A3B先撤资格；正常Free可外传，但不得再次展开临时串。
    unwind.Disarm();
    ReleaseRuntimeCStringHeader(Header(cached));
}

void __cdecl AppendEnvironmentErrorAt4769B0(EnvironmentRootStorage& receiver,
    const char* format, ...)
{
    va_list args;
    va_start(args, format);
    try { AppendEnvironmentErrorVAt4769B0(receiver, format, args); }
    catch (...) { va_end(args); throw; }
    va_end(args);
}

RuntimeCString& BuildEnvironmentErrorTextAt479EA0(EnvironmentRootStorage& receiver,
    RuntimeCString& outRaw, IRuntimeStringManagerSource& strings)
{
    // 5E6F88：GetNil失败不析构out；Set空失败state1，循环失败state0/bit0均清当前槽。
    InitializeEmptyRuntimeCString(outRaw, strings);
    TextUnwind unwind(outRaw);
    SetRuntimeCStringAt437680(outRaw, "", 0);
    if (ErrorCount(receiver) != 0)
    {
        std::uint32_t index = 0;
        do
        {
            if (index != 0) AppendRuntimeCStringLineFeedAt479F48(outRaw);
            const auto* element = reinterpret_cast<const RuntimeCString*>(
                reinterpret_cast<std::uintptr_t>(receiver.errorsAt3C.begin) + index * 4u);
            const char* characters = element->GetString();
            AppendRuntimeCStringAt413550(outRaw, characters, Header(characters)->nDataLength);
            ++index;
        } while (index < ErrorCount(receiver));
    }
    unwind.Disarm();
    return outRaw;
}
}

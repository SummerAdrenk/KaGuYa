#include <Runtime/SoundModuleNativeApi.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
struct NameStorage final
{
    union { RuntimeCString text; };
    NameStorage() noexcept {}
    ~NameStorage() noexcept {}
};
struct NameUnwind final
{
    RuntimeCString* text;
    ~NameUnwind() noexcept { if (text) DestroyRuntimeCStringAt40CD40(text); }
};
}
SoundModuleNativeApi::SoundModuleNativeApi(ISoundModuleNativeSource& modules,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32) noexcept
    : modules_(modules), strings_(strings), win32_(win32) {}

FARPROC SoundModuleNativeApi::ResolveExport(const YtlReferenceSlotRuntime& input,
    std::uintptr_t base, const char* name, std::int32_t length)
{
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(input);
    auto* const control = ref.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
    const auto bias = ref.biasAt04;
    const auto moduleAddress = base + bias * stride;
    NameStorage storage;
    InitializeEmptyRuntimeCString(storage.text, strings_);
    NameUnwind unwind{&storage.text};
    SetRuntimeCStringAt437680(storage.text, name, length);
    const char* const capturedName = storage.text.GetString();
    const auto module = *reinterpret_cast<const volatile HMODULE*>(moduleAddress + 4U);
    // 43820A/43866B/42C8FA前撤EH资格；GetProcAddress或正常Free失败不再减持。
    unwind.text = nullptr;
    const auto function = win32_.GetProcAddress(module, capturedName);
    ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(capturedName) - 0x10U));
    return function;
}
bool SoundModuleNativeApi::IsPlaying(std::int32_t handle)
{
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if (!base) return false;
    using Function = std::uint8_t (__cdecl*)(std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveExport(*slot, base, "SoundIsPlay", 11));
    return function(handle) != 0;
}
void SoundModuleNativeApi::ActivateAppRestoreAt42103E(WPARAM active)
{
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if (!base) return;
    using Function = void (__cdecl*)(std::int32_t);
    // 42103E按Ref base门控，不按HMODULE/initialized；4210E2保留完整DWORD。
    reinterpret_cast<Function>(ResolveExport(*slot, base, "ActivateAppRestore", 18))(
        std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(active)));
}
long SoundModuleNativeApi::CalcValueToVolumeAt4A0E3A(std::int32_t maximum, std::int32_t value)
{
    static_assert(sizeof(long) == 4);
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if (!base) return -10000; // 4A0E3F/4A0E48：空Ref才使用原初值，缺导出仍非法调用。
    using Function = long (__cdecl*)(std::int32_t, std::int32_t);
    // 4A0ED3..4A0EDB原样返回long；4A0F0D裁剪属于movie caller，不能放在包装里。
    return reinterpret_cast<Function>(ResolveExport(*slot, base, "SoundCalcValueToVolume", 22))(maximum, value);
}
void SoundModuleNativeApi::SetVolume(std::int32_t handle, std::int32_t maximum, std::int32_t value)
{
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if (!base) return;
    using Function = void (__cdecl*)(std::int32_t, std::int32_t, std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveExport(*slot, base, "SoundSetVolume", 14));
    function(handle, maximum, value);
}
void SoundModuleNativeApi::StopAt4385F1(std::int32_t handle)
{
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if (!base) return;
    using Function = void (__cdecl*)(std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveExport(*slot, base, "SoundStop", 9));
    function(handle);
}
void SoundModuleNativeApi::PlaySlotAt42C87F(const std::int32_t* capturedSlot)
{
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    const auto handle = *static_cast<const volatile std::int32_t*>(capturedSlot);
    if (!base) return;
    using Function = void (__cdecl*)(std::int32_t);
    const auto function = reinterpret_cast<Function>(ResolveExport(*slot, base, "SoundPlay", 9));
    function(handle);
}
void SoundModuleNativeApi::PlayHandleAt438090(std::int32_t handle)
{
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if (!base) return;
    using Function = void (__cdecl*)(std::int32_t);
    reinterpret_cast<Function>(ResolveExport(*slot, base, "SoundPlay", 9))(handle);
}
void SoundModuleNativeApi::SetLoopAt42BEBF(std::int32_t handle, std::int32_t loop)
{
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if (!base) return;
    using Function = void (__cdecl*)(std::int32_t, std::int32_t);
    reinterpret_cast<Function>(ResolveExport(*slot, base, "SoundSetLoop", 12))(handle, loop);
}
bool SoundModuleNativeApi::OpenCapturedAt42BD8F(const YtlReferenceSlotRuntime& slot, std::int32_t handle,
    const wchar_t* logical, const wchar_t* physical, std::int32_t offset, std::int32_t length)
{
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(slot).baseAt00;
    if (!base) return false; // 捕获早于两次wide构造，base资格检查晚于它们。
    using Function = std::uint8_t (__cdecl*)(std::int32_t, const wchar_t*, const wchar_t*, std::int32_t, std::int32_t);
    return reinterpret_cast<Function>(ResolveExport(slot, base, "SoundOpen", 9))(
        handle, logical, physical, offset, length) != 0;
}
void SoundModuleNativeApi::SetGlobalFocusAt45D1F4(std::int32_t enabled)
{
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if (!base) return;
    using Function = void (__cdecl*)(std::int32_t);
    reinterpret_cast<Function>(ResolveExport(*slot, base, "SetGlobalFocus", 14))(enabled);
}
std::int32_t SoundModuleNativeApi::AddAt412AB0(std::int32_t mode, std::int32_t reserved)
{
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if (!base) return 0;
    // Start412B6C压两个DWORD；原DLL10002F03只读首参数低byte，不改DLL声明。
    using Function = std::int32_t (__cdecl*)(std::int32_t, std::int32_t);
    return reinterpret_cast<Function>(ResolveExport(*slot, base, "AddSound", 8))(mode, reserved);
}
const GUID* SoundModuleNativeApi::DefaultDeviceAt45D4E2()
{
    const auto* const slot = modules_.CaptureSoundModuleReference();
    const auto base = static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if (!base) return nullptr; // caller45D581仍读16字节，不补默认GUID或安全异常。
    using Function = const GUID* (__cdecl*)();
    return reinterpret_cast<Function>(ResolveExport(*slot, base, "GetDefaultDeviceID", 18))();
}
std::uint8_t SoundModuleNativeApi::ResetDeviceAt432C99(HWND window)
{
    const auto* slot=modules_.CaptureSoundModuleReference();
    const auto base=static_cast<const volatile YtlReferenceSlotRuntime&>(*slot).baseAt00;
    if(!base)return 0; // 原432CA2由caller进入当前Env错误尾，不构造导出名。
    using Function=std::uint8_t(__cdecl*)(HWND);
    return reinterpret_cast<Function>(ResolveExport(*slot,base,"ResetDevice",11))(window);
}
}

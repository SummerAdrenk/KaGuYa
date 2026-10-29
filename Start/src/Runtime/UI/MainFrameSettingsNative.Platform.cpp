#include <Runtime/MainFrameSettingsNative.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScriptArithmeticCommandNative.h>

// 原PE的USER32导入，4295D6/429710为两个DWORD栈参数；返回值均未消费。
extern "C" __declspec(dllimport) BOOL WINAPI WINNLSEnableIME(HWND, BOOL);

namespace KaGuYa::Start::Runtime
{
namespace
{
class Api final : public IMainFrameSettingsNativeApi
{
public:
    BOOL WinNlsEnableIME(HWND window, BOOL enabled) override
    { return ::WINNLSEnableIME(window, enabled); }
};
}
IMainFrameSettingsNativeApi& DefaultMainFrameSettingsNativeApi() noexcept
{ static Api api; return api; }
RootMainFrameSettingsNativeAccess::RootMainFrameSettingsNativeAccess(IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings, ScriptNamedValueNativeTables names) noexcept
    : errors_(errors), strings_(strings), names_(names) {}
void RootMainFrameSettingsNativeAccess::WriteMinimizeAt1B2(void* fixed, BYTE value)
{
    if (!fixed) { *reinterpret_cast<volatile BYTE*>(0x1B2u) = value; return; }
    static_cast<RootRuntimePayload*>(fixed)->Frame().WriteMinimizeByteAt1B2(value);
}
ScriptParamsNativeBody* RootMainFrameSettingsNativeAccess::ParamsAt47BDB0(void* fixed)
{
    if (!fixed) return GetParamsAt47BDB0(*reinterpret_cast<YtlReferenceSlotRuntime*>(0x1C0u));
    return &static_cast<RootRuntimePayload*>(fixed)->ParamsAt47BDB0();
}
std::int32_t RootMainFrameSettingsNativeAccess::ResolveNamedAt498020(void*, const char* name)
{ return ResolveScriptNamedValueNativeAt498020(name, names_, strings_); }
std::int32_t RootMainFrameSettingsNativeAccess::ReadValueAt4C73A0(ScriptParamsNativeBody* parameters, std::int32_t index)
{ return ReadScriptValueAt4C73A0(*parameters, index, errors_); }
}

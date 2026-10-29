#pragma once
#include <Runtime/SettingPage01PreviewBlockNative.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
enum class SettingDialogGlobalId : std::uint8_t { PreviewFace,PreviewSample,NameFace,SystemFace };
using SettingDialogGlobalExit = void(__cdecl*)();
class ISettingDialogGlobalExitRegistrar
{
public:
    virtual int RegisterAtExit(SettingDialogGlobalExit)=0;
protected:
    ~ISettingDialogGlobalExitRegistrar()=default;
};
// 四个原字符指针槽61A644/648/64C/650；getter不复制、不重新构造。
RuntimeCString& ProcessSettingDialogNativeGlobal(SettingDialogGlobalId) noexcept;
ISettingPage01PreviewBlockNativeGlobals& ProcessSettingDialogPreviewGlobals() noexcept;
// 原5AD718..724顺序。GetNil/Set失败或注册异常保留各自原EH资格，忽略注册返回码。
void InitializeSettingDialogNativeGlobal(SettingDialogGlobalId,RuntimeCString&,SettingDialogGlobalExit,
    ISettingDialogGlobalExitRegistrar&,IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());
struct SettingDialogGlobalEvidence final
{
    std::uint32_t initializerPointer,constructor,slot,release;
};
std::span<const SettingDialogGlobalEvidence,4> SettingDialogNativeGlobalsSample18() noexcept;
}

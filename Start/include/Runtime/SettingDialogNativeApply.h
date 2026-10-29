#pragma once
#include <Runtime/SettingDialogNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/GraphicsRootNative.h>
#include <Runtime/SoundManagerNativeMusic.h>
#include <Runtime/SoundManagerNativeEffects.h>
#include <Runtime/MessageWindowLayerNative.h>
#include <Runtime/TextWindowNativeText.h>
#include <Runtime/SettingDialogNativeApplyTextServices.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// site是审计标签，不是原函数实参，更不是可执行VA。
class ISettingDialogNativeApplySource
{
public:
    virtual ~ISettingDialogNativeApplySource() = default;
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t site) = 0;
    // 原601160锁/6198C8获取与601118锁/6198C0获取分别履行各自构造、normal/EH解锁契约。
    // 只允许真实singleton获取；不能用配置快照或恒定根掩盖未实现构造。
    virtual EnvironmentRootStorage* CaptureEnvironmentInlineAt6198C8(std::uint32_t site) = 0;
    virtual ParamsRuntimeRoot* CaptureParamsInlineAt6198C0(std::uint32_t site) = 0;
    virtual ParamsRuntimeRoot* CaptureParamsAt418480(std::uint32_t site) = 0;
    virtual void EnsureRootAt413BE0(std::uint32_t site) = 0;
    virtual void* RootAt6198C4(std::uint32_t site) = 0;
    virtual YtlReferenceSlotRuntime& RootSoundSlotAt24(void* fixedRoot) = 0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void* fixedRoot) = 0;
    virtual YtlReferenceSlotRuntime& RootFileSlotAt00(void* fixedRoot) = 0;
    virtual GraphicsRootNativeBody1Ch* CaptureGraphicsAt4182F0(std::uint32_t site) = 0;
    // 仅433C50 started分支原错误处理；不是三条确认框。
    virtual int MessageBoxA(HWND,const char* text,const char* caption,UINT) = 0;
};
class ISettingDialogNativeApplyTextBindings
{
public:
    virtual ~ISettingDialogNativeApplyTextBindings() = default;
    // 纯服务借用，不捕获Root、不执行433C50/4C1650、不返回modern文字镜像。
    // 必须绑定传入的固定E8h receiver，不能回调后改选当前Root里的另一个TextWin。
    virtual SettingDialogNativeApplyTextServices ForFixedBody(TextWindowLayerNativeBodyE8h&) = 0;
};
struct SettingDialogNativeApplyServices final
{
    ISettingDialogNativeApplySource& source;
    SoundModuleNativeApi& sound;
    SoundManagerNativePlayback& playback;
    SoundManagerNativeMusic& music;
    SoundManagerNativeEffects& effects;
    ISoundManagerNativeOpen& open;
    MessageWindowLayerNativeServices& messageWindow;
    ISettingDialogNativeApplyTextBindings& textBindings;
    IGraphicsModuleNativeWin32Api& graphicsWin32;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
// 仅借调用期服务；不向AC/E8/50h正文添加字段，不接管或复制任何Snapshot。
class SettingDialogNativeApplyScope final
{
public:
    explicit SettingDialogNativeApplyScope(SettingDialogNativeApplyServices&) noexcept;
    ~SettingDialogNativeApplyScope() noexcept;
    SettingDialogNativeApplyScope(const SettingDialogNativeApplyScope&) = delete;
    SettingDialogNativeApplyScope& operator=(const SettingDialogNativeApplyScope&) = delete;
private:
    SettingDialogNativeApplyServices* previous_;
    MessageWindowLayerNativeScope messageWindow_;
};
SettingDialogNativeApplyServices& RequireSettingDialogNativeApplyServices();
void ApplySettingDialogNativeAt51CFC0(SettingDialogNativeBodyACh&);
// 同一次业务使用固定receiver的433C50原核；不冒充4C1650或默认成功。
void RestoreSettingDialogTextNativeAt433C50(TextWindowLayerNativeBodyE8h&);
struct SettingDialogNativeApplyWrite final { std::uint32_t offset,value; };
std::span<const SettingDialogNativeApplyWrite> SettingDialogNativeApplyPolicyWritesSample18(std::uint32_t choice) noexcept;
std::span<const std::uint32_t,28> SettingDialogNativeApplyOriginalUnwindSample18() noexcept;
std::span<const std::uint32_t,6> SettingDialogNativeApplyTextOriginalUnwindSample18() noexcept;
std::span<const std::uint32_t,9> SettingDialogNativeApplyOriginalFuncInfoSample18() noexcept;
std::span<const std::uint32_t,9> SettingDialogNativeApplyTextOriginalFuncInfoSample18() noexcept;
std::span<const std::uint32_t,5> SettingDialogNativeApplyTextOriginalTryBlockSample18() noexcept;
std::span<const std::uint32_t,4> SettingDialogNativeApplyTextOriginalHandlerSample18() noexcept;
const char* SettingDialogNativeApplyTextErrorSample18() noexcept;
}

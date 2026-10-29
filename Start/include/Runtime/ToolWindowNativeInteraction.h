#pragma once
#include <Runtime/ToolWindowNative.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/DrawManagerNative.h>

namespace KaGuYa::Start::Runtime
{
class UnSaveDataRuntime;
enum class ToolUnSaveNativeRead { Inline, Lookup417990 };
class IToolWindowNativeInteractionSource
{
public:
    virtual ~IToolWindowNativeInteractionSource() = default;
    virtual UnSaveDataRuntime* CaptureUnSave(ToolUnSaveNativeRead) = 0;
    virtual KeyboardManagerNativeBody438h* CaptureKeyboardAt418210() = 0;
    virtual std::int32_t ReadTextCursorAfter413BE0() = 0;
    virtual std::uint8_t ReadMessageEnabledAfter413BE0() = 0;
    virtual std::int32_t ReadNamedAfterRoot(const char* name) = 0;
};
// draws是原生根捕获边界，不将现代DrawManager/FrameLayerItem强转为原布局。
class EnvironmentToolWindowNativeInteractionSource final : public IToolWindowNativeInteractionSource
{
public:
    EnvironmentToolWindowNativeInteractionSource(EnvironmentRootOwner&, InputRootOwner&, RootRuntimeOwner&,
        IDrawManagerNativeRootSource&, ScriptNamedValueNativeTables, IScrVmCommandErrorSink&,
        IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource()) noexcept;
    UnSaveDataRuntime* CaptureUnSave(ToolUnSaveNativeRead) override;
    KeyboardManagerNativeBody438h* CaptureKeyboardAt418210() override;
    std::int32_t ReadTextCursorAfter413BE0() override;
    std::uint8_t ReadMessageEnabledAfter413BE0() override;
    std::int32_t ReadNamedAfterRoot(const char*) override;
private:
    EnvironmentRootOwner& environment_;
    InputRootOwner& input_;
    RootRuntimeOwner& root_;
    IDrawManagerNativeRootSource& draws_;
    ScriptNamedValueNativeTables names_;
    IScrVmCommandErrorSink& errors_;
    IRuntimeStringManagerSource& strings_;
};
struct ToolWindowNativeInteractionServices final
{
    ToolWindowNativeServices& window;
    IToolWindowNativeInteractionSource& state;
};
void SetToolModeNativeAt434FB0(ToolWindowNativeBody40h&, std::int32_t mode, ToolWindowNativeInteractionServices&);
std::uint32_t TickToolWindowNativeAt4BE200(ToolWindowNativeBody40h&, ToolWindowNativeInteractionServices&);
void CaptureToolWindowNativeAt4C01F0(ToolWindowNativeBody40h&, ToolWindowNativeInteractionServices&);
void RestoreToolWindowNativeAt4BF4E0(ToolWindowNativeBody40h&, ToolWindowNativeInteractionServices&);
void SetToolCaptureModeNativeAt4C2300(ToolWindowNativeBody40h&, std::uint8_t, ToolWindowNativeInteractionServices&);
void PropagateToolDirtyNativeAt415910(ToolWindowNativeBody40h&, ToolWindowNativeInteractionServices&);
void ResetToolSkipLatchNativeAt4AB910(ToolWindowNativeBody40h&, std::uint8_t, ToolWindowNativeInteractionServices&);
// 4B0760/4B0470不消费ECX；同一核既供Tool模式更新，也供ReadScript直接查询。
std::int32_t ReadToolSkipStatusNativeAt4B0760(IToolWindowNativeInteractionSource&, std::uint32_t checkHeld);
std::int32_t ReadToolAutoStatusNativeAt4B0470(IToolWindowNativeInteractionSource&, std::uint32_t checkHeld);
}

#pragma once

#include <Runtime/DrawToolView.h>

#include <cstdint>

namespace KaGuYa::Start::Runtime
{
struct ReadScriptSceneState;
class RootRuntimeOwner;
class EnvironmentRootOwner;
class IDrawLayersViewSource;
struct ToolWindowNativeInteractionServices;

class IReadScriptSceneRestoreSource
{
public:
    virtual ~IReadScriptSceneRestoreSource() = default;
    virtual std::uint32_t ReadScriptFlagsAfter413BE0() = 0;
    virtual std::int32_t ReadSceneMarkerAfter413BE0() = 0;
    virtual DrawToolView CaptureToolAfter413BE0() = 0;
    virtual std::uint32_t ReadUnSaveModeAt4180C0() = 0;
    // 4A1777..4A178E：重新捕获一次Script，依次清+04 DWORD、+08低BYTE。
    virtual void ClearScriptFlagsAndReloadAfter413BE0() = 0;
};

// 只借当前发布来源；draws自行完成413BE0捕获，且必须与root同根。
// 不将现代root+18正文强转为原生2Ch。
class RootReadScriptSceneRestoreSource final : public IReadScriptSceneRestoreSource
{
public:
    RootReadScriptSceneRestoreSource(RootRuntimeOwner& root, EnvironmentRootOwner& environment,
        IDrawLayersViewSource& draws, ToolWindowNativeInteractionServices& toolServices) noexcept;
    std::uint32_t ReadScriptFlagsAfter413BE0() override;
    std::int32_t ReadSceneMarkerAfter413BE0() override;
    DrawToolView CaptureToolAfter413BE0() override;
    std::uint32_t ReadUnSaveModeAt4180C0() override;
    void ClearScriptFlagsAndReloadAfter413BE0() override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    IDrawLayersViewSource& draws_;
    ToolWindowNativeInteractionServices& toolServices_;
};

// 仅4A1646..4A178E，在owner完成4A163C资源释放后调用。
// 调用方拥有唯一scene状态，并负责Ref/FreeScroll构造及异常展开。
void RestoreReadScriptSceneNativeAt4A14D0(ReadScriptSceneState& state,
    IReadScriptSceneRestoreSource& source);
}

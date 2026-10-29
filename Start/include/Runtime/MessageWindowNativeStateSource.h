#pragma once

#include <Runtime/MessageWindowLayerNative.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
// 只借实际root接收者、环境唯一owner和原名称映射；没有第二份Draw/TextWin或Save快照。
// E8 TextWin的构造/拥有仍由宿主负责，本类仅实现已核实的+40/+70读取叶。
class MessageWindowNativeStateSource final : public IMessageWindowNativeStateSource
{
public:
    MessageWindowNativeStateSource(IMessageWindowNativeRootSource& roots,
        EnvironmentRootOwner& environment,
        ScriptNamedValueNativeTables names = GameScriptNamedValueTables(),
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept;
    MessageWindowNativeRootReceiver CaptureAfterEnsureAt413BE0() override;
    std::uint8_t MessageWindowAllowedAt419290(DrawManagerNativeBody2Ch*) override;
    const IEnvironmentArchiveRuntime* CaptureSlot0At4180C0() override;
    std::int32_t ResolveAt498020(const IEnvironmentArchiveRuntime*, const char*) override;
    std::uintptr_t CaptureTextWindowNameBaseAt70() override;
    const RECT* CaptureTextWindowNameBoundsAt46B558() override;
private:
    LayerItemNativeBody14h* CaptureTextWindow();
    IMessageWindowNativeRootSource& roots_;
    EnvironmentRootOwner& environment_;
    ScriptNamedValueNativeTables names_;
    IRuntimeStringManagerSource& strings_;
};
}

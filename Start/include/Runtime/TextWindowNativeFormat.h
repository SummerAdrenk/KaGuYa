#pragma once

#include <Runtime/TextWindowNativeText.h>
#include <Runtime/MessageFormatSelectionRuntime.h>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRootOwner;
class RootRuntimeOwner;
class IScrVmCommandErrorSink;

// 每个入口各自Ensure并取当前对象；服务不占E8h，也不保留参数或消息快照。
class ITextWindowNativeFormatSource
{
public:
    virtual ~ITextWindowNativeFormatSource() = default;
    virtual ParamsGameSystem* CaptureGameSystemAt418480() = 0;
    virtual std::int32_t ReadFormatAfterEnsureAt413BE0(std::int32_t index,
        MessageFormatSelectionRawTarget output, std::uint32_t variant) = 0;
    virtual ScriptParamsNativeBody* CaptureParamsAfterEnsureAt413BE0() = 0;
};

class RootTextWindowNativeFormatSource final : public ITextWindowNativeFormatSource
{
public:
    RootTextWindowNativeFormatSource(RootRuntimeOwner& root, ParamsRuntimeRootOwner& params) noexcept;
    ParamsGameSystem* CaptureGameSystemAt418480() override;
    std::int32_t ReadFormatAfterEnsureAt413BE0(std::int32_t,
        MessageFormatSelectionRawTarget, std::uint32_t) override;
    ScriptParamsNativeBody* CaptureParamsAfterEnsureAt413BE0() override;
private:
    RootRuntimeOwner& root_;
    ParamsRuntimeRootOwner& params_;
};

class TextWindowNativeFormat final
{
public:
    TextWindowNativeFormat(TextWindowLayerNativeBodyE8h&, ITextWindowNativeFormatSource&,
        TextWindowLayerNativeServices&, TextWindowNativeText&, IGraphicsTextApi&,
        IScrVmCommandErrorSink&) noexcept;
    // 原4C2974/4C16BD测试完整DWORD，不提前缩成bool。
    [[nodiscard]] bool SetFormatAt4C2740(std::int32_t index, std::uint32_t initialize);
    [[nodiscard]] bool RebuildAt4C1650(std::uint32_t refreshUnread);
private:
    TextWindowLayerNativeBodyE8h& body_;
    ITextWindowNativeFormatSource& source_;
    TextWindowLayerNativeServices& window_;
    TextWindowNativeText& layout_;
    IGraphicsTextApi& text_;
    IScrVmCommandErrorSink& errors_;
};
}

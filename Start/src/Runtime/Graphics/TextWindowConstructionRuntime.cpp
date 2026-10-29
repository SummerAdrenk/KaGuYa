#include <Runtime/TextWindowConstructionRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

#include <array>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 18号原 CP932 字节，按构造顺序：テキスト文字色、既読テキスト色、名前文字色、名前飾り色。
// 准确起点依次为 5B2CB0、5B2CC0、5B2CD0、5B2CDC。
constexpr std::array<const char*, 4> ColorNames{
    "\x83\x65\x83\x4C\x83\x58\x83\x67\x95\xB6\x8E\x9A\x90\x46",
    "\x8A\xF9\x93\xC7\x83\x65\x83\x4C\x83\x58\x83\x67\x90\x46",
    "\x96\xBC\x91\x4F\x95\xB6\x8E\x9A\x90\x46",
    "\x96\xBC\x91\x4F\x8F\xFC\x82\xE8\x90\x46",
};
}

TextWindowConstructionRuntime::TextWindowConstructionRuntime(
    const ParamsGameSystem& params, IMessageWindowConfigExpressionParser& parser,
    IGraphicsSurfaceApi& surfaces, ProgramDwordReference programWidthAt18,
    ProgramDwordReference programHeightAt1C, ISettingTagErrorSource& errors) noexcept
    : fixedParams_(&params), parser_(parser), surfaces_(surfaces),
      programWidthAt18_(programWidthAt18), programHeightAt1C_(programHeightAt1C), errors_(errors)
{
}

TextWindowConstructionRuntime::TextWindowConstructionRuntime(
    ParamsRuntimeRootOwner& paramsRoot, IMessageWindowConfigExpressionParser& parser,
    IGraphicsSurfaceApi& surfaces, ProgramDwordReference programWidthAt18,
    ProgramDwordReference programHeightAt1C, ISettingTagErrorSource& errors) noexcept
    : paramsRoot_(&paramsRoot), parser_(parser), surfaces_(surfaces),
      programWidthAt18_(programWidthAt18), programHeightAt1C_(programHeightAt1C), errors_(errors)
{
}

const ParamsGameSystem& TextWindowConstructionRuntime::ReadGameSystem() const
{
    if (!paramsRoot_) return *fixedParams_;
    const auto& current = paramsRoot_->GetOrCreateAt418480().GameSystem();
    if (!current) throw std::logic_error("TextWindow requires the current params GameSystem");
    return *current;
}

std::shared_ptr<const std::uint32_t> TextWindowConstructionRuntime::ReadColorOverride(
    std::uint32_t index)
{
    const auto* name = ColorNames.at(index);
    const auto& params = ReadGameSystem();
    auto* errorCString = errors_.CaptureAt47BCF0(); // 45DD1F/45DDFB/45DED7/45DFB3
    std::uint32_t color;
    {
        const auto definition = CaptureSettingChildAt477E90(errorCString, params.settingTags[1].Native(), name);
        color = ReadSettingNodeRgbColorAt4797C0(errorCString, definition.Native(), "Value", parser_);
    }
    // 469EC0 在解析成功后才分配；现代共享所有权不复现原部分 Ref12 的泄漏状态。
    return std::make_shared<const std::uint32_t>(color);
}

std::unique_ptr<RuntimeGraphicsSurface> TextWindowConstructionRuntime::AllocateSurface()
{
    // owner 先提交空 Surface，再读高、宽并调用 Create(width,height,true,true)。
    return std::make_unique<RuntimeGraphicsSurface>(surfaces_);
}

std::int32_t TextWindowConstructionRuntime::ReadProgramHeightAt1C() const
{
    return programHeightAt1C_.Read();
}

std::int32_t TextWindowConstructionRuntime::ReadProgramWidthAt18() const
{
    return programWidthAt18_.Read();
}
}

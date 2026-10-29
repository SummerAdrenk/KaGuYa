#pragma once

#include <Runtime/GraphicsSurfaceRuntime.h>
#include <Runtime/MessageWindowConfigRuntime.h>
#include <Runtime/ProgramWindowFieldReferences.h>

#include <cstdint>
#include <memory>

namespace KaGuYa::Start::Runtime
{
class ParamsRuntimeRootOwner;

// 45DB30：owner 对每次颜色尝试分别捕获异常，四项结束后才分配 Surface。
class ITextWindowConstructionServices
{
public:
    virtual ~ITextWindowConstructionServices() = default;
    [[nodiscard]] virtual std::shared_ptr<const std::uint32_t> ReadColorOverride(
        std::uint32_t index) = 0;
    [[nodiscard]] virtual std::unique_ptr<RuntimeGraphicsSurface> AllocateSurface() = 0;
    [[nodiscard]] virtual std::int32_t ReadProgramHeightAt1C() const = 0;
    [[nodiscard]] virtual std::int32_t ReadProgramWidthAt18() const = 0;
};

// 仅用于构造期的借用服务，不保留生成的颜色或 Surface 所有权。
// Program 宽高实时读取 4180C0 的 +18/+1C 字段，不查询 DLL 的屏幕尺寸。
// 所有借用对象必须比服务长寿；Surface API 还必须比生成的 Surface 长寿。
class TextWindowConstructionRuntime final : public ITextWindowConstructionServices
{
public:
    // 固定输入入口：每次读取同一个 params；调用方负责其寿命，不跟随根换代。
    TextWindowConstructionRuntime(const ParamsGameSystem& params,
        IMessageWindowConfigExpressionParser& parser, IGraphicsSurfaceApi& surfaces,
        ProgramDwordReference programWidthAt18, ProgramDwordReference programHeightAt1C,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner()) noexcept;
    // 45DCF9/45DDD5/45DEB1/45DF8D：每项颜色才取当前 Params+4，不缓存或保活 child。
    // 构造只借用稳定 root；root 必须长于本服务，缺失 GameSystem 在读取时明确失败。
    TextWindowConstructionRuntime(ParamsRuntimeRootOwner& paramsRoot,
        IMessageWindowConfigExpressionParser& parser, IGraphicsSurfaceApi& surfaces,
        ProgramDwordReference programWidthAt18, ProgramDwordReference programHeightAt1C,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner()) noexcept;

    [[nodiscard]] std::shared_ptr<const std::uint32_t> ReadColorOverride(
        std::uint32_t index) override;
    [[nodiscard]] std::unique_ptr<RuntimeGraphicsSurface> AllocateSurface() override;
    [[nodiscard]] std::int32_t ReadProgramHeightAt1C() const override;
    [[nodiscard]] std::int32_t ReadProgramWidthAt18() const override;

private:
    [[nodiscard]] const ParamsGameSystem& ReadGameSystem() const;
    const ParamsGameSystem* fixedParams_ = nullptr;
    ParamsRuntimeRootOwner* paramsRoot_ = nullptr;
    IMessageWindowConfigExpressionParser& parser_;
    IGraphicsSurfaceApi& surfaces_;
    ProgramDwordReference programWidthAt18_;
    ProgramDwordReference programHeightAt1C_;
    ISettingTagErrorSource& errors_;
};
}

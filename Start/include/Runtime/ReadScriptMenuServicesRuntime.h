#pragma once

#include <cstdint>
#include <Runtime/DrawLayersView.h>

namespace KaGuYa::Start::Runtime
{
class DrawManagerLayerRuntime;
class SelectWindowLayerRuntime;
class ISelectWindowStateServices;
class IGraphicsTextApi;
class IGraphicsTextGlyphApi;
class ScrVmValueTable;
class ScrVmExecutableProfile;
class ScrVmScriptCommandState;

class IReadScriptMenuServices
{
public:
    virtual ~IReadScriptMenuServices() = default;
    virtual void RebuildChoicesAt4369E0() = 0;
    virtual void ClearToolByteAt38() = 0;
    [[nodiscard]] virtual std::uint8_t ReadScriptByteAt24() = 0;
    [[nodiscard]] virtual std::int32_t TickChoicesAt4BBFD0(std::uint8_t blocked) = 0;
    virtual void WriteMenuResult(std::int32_t ordinal) = 0;
};

// 每次从同一 manager 的 owning 槽取对象；不保留替代窗口或复制 CScript 状态。
class ReadScriptMenuServicesRuntime final : public IReadScriptMenuServices
{
public:
    ReadScriptMenuServicesRuntime(DrawManagerLayerRuntime& layers, ISelectWindowStateServices& choices,
        IGraphicsTextApi& text, IGraphicsTextGlyphApi& glyphs, const ScrVmScriptCommandState& script,
        ScrVmValueTable& values, const ScrVmExecutableProfile& profile) noexcept;
    ReadScriptMenuServicesRuntime(DrawLayersView layers,SelectWindowNativeWorkflowServices& choices,
        const ScrVmScriptCommandState& script,ScrVmValueTable& values,const ScrVmExecutableProfile& profile) noexcept;
    ReadScriptMenuServicesRuntime(IDrawLayersViewSource& source,SelectWindowNativeWorkflowServices& choices,
        const ScrVmScriptCommandState& script,ScrVmValueTable& values,const ScrVmExecutableProfile& profile) noexcept;
    void RebuildChoicesAt4369E0() override;
    void ClearToolByteAt38() override;
    std::uint8_t ReadScriptByteAt24() override;
    std::int32_t TickChoicesAt4BBFD0(std::uint8_t blocked) override;
    void WriteMenuResult(std::int32_t ordinal) override;
private:
    [[nodiscard]] DrawSelectionView Window() const;
    DrawLayersView CaptureLayers() const;
    DrawLayersView layers_;
    IDrawLayersViewSource* source_=nullptr;
    DrawSelectionControlledServices controlled_;
    SelectWindowNativeWorkflowServices* native_=nullptr;
    const ScrVmScriptCommandState& script_;
    ScrVmValueTable& values_;
    const ScrVmExecutableProfile& profile_;
};
}

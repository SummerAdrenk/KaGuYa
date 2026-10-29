#include "MainFrameSettingsNative.Internal.h"
#include <Runtime/GameBuildProfile.h>

namespace KaGuYa::Start::Runtime::MainFrameSettingsNativeDetail
{
namespace
{
void* Saved(MainFrameSettingsNativeServices& services, unsigned site)
{ return Registry(services, 1, site, RegistryOrder::Function417990); }
bool NameChanged(const RuntimeCString& old, const RuntimeCString& current)
{
    const auto chars = current.GetString();
    if (!chars) throw ATL::CAtlException(E_FAIL);
    return _mbsicmp(reinterpret_cast<const unsigned char*>(old.GetString()),
        reinterpret_cast<const unsigned char*>(chars)) != 0;
}
void RefreshPane(const Slot& slot)
{
    const auto base = Read(slot.baseAt00);
    if (!base) return;
    auto* const control = Read(slot.controlAt08);
    const auto stride = control ? *reinterpret_cast<const volatile unsigned*>(reinterpret_cast<std::uintptr_t>(control) + 0x0C) : 0u;
    const auto bias = Read(slot.biasAt04);
    auto* const pane = reinterpret_cast<MessageWindowPaneNativeBody14h*>(base + bias * stride);
    pane->vptrAt00->refreshAt08(pane);
}
void RefreshStyles(MessageWindowLayerNativeBody50h& layer)
{
    auto* slot = Read(layer.stylesAt24.begin);
    while (slot != Read(layer.stylesAt24.end))
    {
        auto* const style = Resolve<MessageWindowStyleNativeBody34h>(*slot);
        RefreshPane(style->bodyAt00);
        RefreshPane(style->nameAt0C);
        slot = reinterpret_cast<Slot*>(reinterpret_cast<std::uintptr_t>(slot) + 12u);
    }
    if (Read(layer.layer.enabledAt0C))
    {
        auto* const parent = Read(layer.layer.parentAt10);
        parent->vptrAt00->addDirtyAt20(parent, &layer.boundsAt14);
    }
}
void ReconsiderMessage(MainFrameSettingsNativeServices& services)
{
    auto* const layer = reinterpret_cast<MessageWindowLayerNativeBody50h*>(DrawLayerAt418C10(*RootDraw(0x42A79A), 1));
    auto* const root = Root(services, 0x42A7DA);
    if (Read(services.access.ParamsAt47BDB0(root)->messageFormatIndexAt60) == -1) return;
    const auto enabled = Read(layer->layer.enabledAt0C);
    auto* const draw = RootDraw(enabled ? 0x42A7FF : 0x42A840);
    const auto allowed = services.apply.messageWindow.state.MessageWindowAllowedAt419290(draw);
    if (enabled)
    {
        if (allowed) return;
        layer->layer.enabledAt0C = 0;
    }
    else
    {
        if (!allowed) return;
        auto* const fixed = Root(services, 0x42A87A);
        auto* const archive = Registry(services, 0, 0x42A88C, RegistryOrder::Function417990);
        const auto index = services.access.ResolveNamedAt498020(archive, "mess_win_type");
        auto* const parameters = services.access.ParamsAt47BDB0(fixed);
        layer->selectedAt30 = services.access.ReadValueAt4C73A0(parameters, index);
        layer->layer.enabledAt0C = 1;
    }
    // 42A8BB无条件提交bounds，不能套487030对空区域的现代门控。
    auto* const parent = Read(layer->layer.parentAt10);
    parent->vptrAt00->addDirtyAt20(parent, &layer->boundsAt14);
}
}
void ApplyPolicyAndLayers(Locals& values, MainFrameSettingsNativeServices& services)
{
    const auto mode = Read(Field<unsigned>(Saved(services, 0x42A2BA), 0x8C));
    auto* const policy = Registry(services, 5, 0x42A2CE, RegistryOrder::Function417990);
    for (const auto& write : SettingDialogNativeApplyPolicyWritesSample18(mode))
        *static_cast<volatile unsigned*>(&Field<unsigned>(policy, write.offset)) = write.value;

    bool changed = NameChanged(values.first, Field<RuntimeCString>(Saved(services, 0x42A3B4), 0x58));
    if (!changed) changed = NameChanged(values.second, Field<RuntimeCString>(Saved(services, 0x42A3DB), 0x5C));
    if (changed)
    {
        const auto first = Field<RuntimeCString>(Saved(services, 0x42A406), 0x58).GetString();
        SettingDialogNativeApplyDetail::SetDefaultName(first, true, 0x42A41B);
        const auto second = Field<RuntimeCString>(Saved(services, 0x42A453), 0x5C).GetString();
        SettingDialogNativeApplyDetail::SetDefaultName(second, false, 0x42A468);
    }
    bool colors = values.color44 != Read(Field<DWORD>(Saved(services, 0x42A4A2), 0x44));
    if (!colors) colors = values.color40 != Read(Field<DWORD>(Saved(services, 0x42A4BB), 0x40));
    if (colors)
    {
        auto* const message = reinterpret_cast<MessageWindowLayerNativeBody50h*>(DrawLayerAt418C10(*RootDraw(0x42A4DB), 1));
        RefreshStyles(*message);
        changed = true;
    }
    // 原42A5B1..42A615按BYTE/BYTE/DWORD/DWORD短路；名字/颜色最后才补触发。
    bool text = values.text60 != Read(Field<BYTE>(Saved(services, 0x42A5B1), 0x60));
    if (!text) text = values.text61 != Read(Field<BYTE>(Saved(services, 0x42A5CA), 0x61));
    if (!text) text = values.text64 != Read(Field<DWORD>(Saved(services, 0x42A5E3), 0x64));
    if (!text) text = values.text68 != Read(Field<DWORD>(Saved(services, 0x42A5FC), 0x68));
    if (text || changed)
    {
        auto* probe = DrawLayerAt418C10(*RootDraw(0x42A620), 3);
        if (Read(probe->enabledAt0C))
        {
            auto* const current = reinterpret_cast<TextWindowLayerNativeBodyE8h*>(DrawLayerAt418C10(*RootDraw(0x42A65F), 3));
            RestoreSettingDialogTextNativeAt433C50(*current);
        }
        probe = DrawLayerAt418C10(*RootDraw(0x42A69E), StartupSelectionLayerIndex());
        if (Read(probe->enabledAt0C))
        {
            auto* const current = reinterpret_cast<SelectWindowNativeBody6Ch*>(DrawLayerAt418C10(*RootDraw(0x42A6DD), StartupSelectionLayerIndex()));
            RebuildSelectWindowNativeAt4369E0(*current, services.selectWorkflow);
        }
    }
    auto* const patterns = reinterpret_cast<LayerWindowNativeBody4Ch*>(DrawLayerAt418C10(*RootDraw(0x42A71C), 0));
    ApplySettingsAnimationFlagNativeAt431000(*patterns, services.environment);
    auto* const window = reinterpret_cast<TextWindowLayerNativeBodyE8h*>(DrawLayerAt418C10(*RootDraw(0x42A75B), 3));
    RestoreSettingsTextVisibilityNativeAt431260(*window, services.apply);
    ReconsiderMessage(services);
    auto* const tools = reinterpret_cast<ToolWindowNativeBody40h*>(DrawLayerAt418C10(*RootDraw(0x42A8CC), 2));
    EnableToolWindowNativeAt418F70(*tools, 1, services.tools.replay);
}
}

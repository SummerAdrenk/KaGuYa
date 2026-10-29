#include <Runtime/SettingsLayerRefreshNative.h>
#include <Runtime/TextWindowNativeText.h>
#include <bit>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> T Read(const T& value) noexcept
{ return *static_cast<const volatile T*>(&value); }
template<class T> T* Element(const YtlReferenceSlotRuntime& slot, std::uint32_t index = 0) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* const control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride = control ? Read(control->strideAt0C) : 0u;
    const auto bias = Read(slot.biasAt04);
    return reinterpret_cast<T*>(base + (bias + index) * stride);
}
std::uint32_t Count(const YtlReferenceSlotRuntime& slot) noexcept
{
    auto* const control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    return control ? Read(control->countAt10) : 0u; // 原4172B0不先检查base。
}
bool HasContent(const TextWindowLayerNativeBodyE8h& body) noexcept
{
    return Read(body.contentAt30.left) != 0 || Read(body.contentAt30.right) != 0 ||
        Read(body.contentAt30.top) != 0 || Read(body.contentAt30.bottom) != 0;
}
void Dirty(TextWindowLayerNativeBodyE8h& body)
{
    auto* const parent = Read(body.layer.parentAt10);
    parent->vptrAt00->addDirtyAt20(parent, &body.contentAt30);
}
}
void ApplySettingsAnimationFlagNativeAt431000(LayerWindowNativeBody4Ch& body,
    EnvironmentRootOwner& environment)
{
    if (!Read(body.window.layer.enabledAt0C)) return;
    auto* slot = Read(body.patternsAt30.begin);
    while (slot != Read(body.patternsAt30.end))
    {
        auto* const pattern = Element<PatternLayerNativeBody80h>(*slot);
        if (Read(pattern->layer.enabledAt0C))
        {
            // 431083..431169的原内联捕获与owner使用同一锁/发布核；回调后再读注册表。
            auto* const root = environment.CaptureAt4180C0();
            const auto end = reinterpret_cast<std::uintptr_t>(Read(root->archivesAt30.end));
            auto* const begin = Read(root->archivesAt30.begin);
            const auto count = static_cast<unsigned>(std::bit_cast<int>(end - reinterpret_cast<std::uintptr_t>(begin)) / 12);
            auto* const saved = count > 1u ? Element<const BYTE>(begin[1]) : nullptr;
            const auto enabled = *reinterpret_cast<const volatile BYTE*>(reinterpret_cast<std::uintptr_t>(saved) + 0x6Du);
            for (std::uint32_t i = 0; i < Count(pattern->spritesAt34); ++i)
            {
                auto* const sprite = Element<SpriteNativeBodyA0h>(pattern->spritesAt34, i);
                if (sprite->logicalNameAt0C.GetLength() && Read(sprite->modeAt04) == 2u)
                {
                    const auto flags = Read(sprite->animationAt34.flagsAt00);
                    if (flags & 1u) sprite->animationAt34.flagsAt00 = enabled ? flags & ~2u : flags | 2u;
                }
            }
        }
        // 431227..431230保持旧迭代槽递增，但终点从原receiver重新读取。
        slot = reinterpret_cast<YtlReferenceSlotRuntime*>(reinterpret_cast<std::uintptr_t>(slot) + 12u);
    }
}
const char* SettingsTextVisibilityErrorSample18() noexcept
{
    // 原PE5AE1EC：メッセージウィンドウの再描画に失敗しました。
    return "\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45"
        "\x82\xCC\x8D\xC4\x95\x60\x89\xE6\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD";
}
void RestoreSettingsTextVisibilityNativeAt431260(TextWindowLayerNativeBodyE8h& body,
    SettingDialogNativeApplyServices& services)
{
    auto& state = services.messageWindow.state;
    const auto paramsRoot = state.CaptureAfterEnsureAt413BE0();
    if (Read(paramsRoot.paramsAt47BDB0(paramsRoot.object)->messageFormatIndexAt60) == -1) return;
    const auto enabled = Read(body.layer.enabledAt0C);
    const auto drawRoot = state.CaptureAfterEnsureAt413BE0();
    const auto allowed = state.MessageWindowAllowedAt419290(drawRoot.drawManagerAt4175A0(drawRoot.object));
    if (enabled)
    {
        if (allowed) return;
        body.layer.enabledAt0C = 0;
    }
    else
    {
        if (!allowed) return;
        auto bindings = services.textBindings.ForFixedBody(body);
        ClearTextWindowPreviousRegionNativeAt418EE0(body, bindings.window);
        try
        {
            TextWindowNativeText layout(body, bindings.text, bindings.glyphs, bindings.layout, services.strings);
            static_cast<void>(layout.PrepareTextAt415150());
            const auto end = static_cast<std::uint32_t>(bindings.text.GetTextLength());
            static_cast<void>(layout.BlitRangeAt419090(0, end));
        }
        catch (...)
        {
            auto* const environment = services.source.CaptureEnvironmentAt4180C0(0x43135D);
            services.source.MessageBoxA(reinterpret_cast<HWND>(Read(environment->windowAt2C)),
                SettingsTextVisibilityErrorSample18(), environment->titleAt10.GetString(), MB_ICONERROR);
        }
        body.layer.enabledAt0C = 1;
    }
    if (HasContent(body)) Dirty(body);
}
}

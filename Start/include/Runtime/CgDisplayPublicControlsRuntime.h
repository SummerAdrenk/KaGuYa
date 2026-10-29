#pragma once

#include <Runtime/CgUiButtonRuntime.h>
#include <Runtime/SceneManagerPublicNativeReferences.h>

#include <type_traits>
#include <utility>
#include <variant>

namespace KaGuYa::Start::Runtime
{
class ICgDisplayButtonGroupDirtyRects;
class ICgDisplayButtonGroupSelector;
class ILoadAfterTransitionSource;

// 只拥有一份原数组Ref，不建立四个shared_ptr别名；默认和失败引用始终留在原生域。
class CgDisplayPublicModeButtonsRuntime final
{
public:
    using ControlledButtons = std::vector<std::shared_ptr<CgUiButtonRuntime>>;
    CgDisplayPublicModeButtonsRuntime() noexcept = default;
    CgDisplayPublicModeButtonsRuntime(const YtlReferenceSlotRuntime* borrowed,
        ModeSelectButtonNativeServices& services) noexcept;
    // 兼容既有明确使用现代模型的工厂结果，不作为原生失败后的替代入口。
    CgDisplayPublicModeButtonsRuntime(ControlledButtons buttons) noexcept;

    [[nodiscard]] bool IsNative() const noexcept;
    [[nodiscard]] const SceneManagerModeButtonsNativeReference* NativeReference() const noexcept;
    [[nodiscard]] std::int32_t Count() const noexcept;
    [[nodiscard]] bool HasButton(std::int32_t index) const;
    [[nodiscard]] std::int32_t StateAt04(std::int32_t index) const;
    [[nodiscard]] RECT VisibleRect(std::int32_t index) const;
    [[nodiscard]] std::int32_t ProcessAt4A98E0(std::int32_t index, bool forced);
    [[nodiscard]] std::uint8_t DrawToScreenAt441980(std::int32_t index, const RECT* clip);
    [[nodiscard]] std::uint8_t DrawIntoAt4B5290(std::int32_t index,
        GraphicsSurfaceDestination destination, const RECT* clip);
    void Release();

private:
    struct Native final
    {
        Native() noexcept : services(nullptr) {}
        Native(const YtlReferenceSlotRuntime* borrowed, ModeSelectButtonNativeServices& source) noexcept
            : reference(borrowed, source), services(&source) {}
        SceneManagerModeButtonsNativeReference reference;
        ModeSelectButtonNativeServices* services;
    };
    std::variant<Native, ControlledButtons> storage_{std::in_place_index<0>};
};

// +78互斥拥有一份原选择器Ref或既有受控接口，不复制正文状态。
class CgDisplayPublicSelectorRuntime final
{
public:
    CgDisplayPublicSelectorRuntime() noexcept = default;
    CgDisplayPublicSelectorRuntime(const YtlReferenceSlotRuntime* borrowed,
        OptionCharSelectDialogNativeDependencies& services) noexcept;
    CgDisplayPublicSelectorRuntime(std::shared_ptr<ICgDisplayButtonGroupSelector> selector) noexcept;
    template<class T> requires std::is_convertible_v<T*, ICgDisplayButtonGroupSelector*>
    CgDisplayPublicSelectorRuntime(std::shared_ptr<T> selector) noexcept
        : CgDisplayPublicSelectorRuntime(
              std::shared_ptr<ICgDisplayButtonGroupSelector>(std::move(selector))) {}

    [[nodiscard]] bool IsNative() const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept;
    [[nodiscard]] const SceneManagerOptionCharNativeReference* NativeReference() const noexcept;
    [[nodiscard]] std::int32_t ProcessAtVslot48();
    [[nodiscard]] std::int32_t SelectionAtVslot50() const;
    void AttachDirtyRectsAtVslot94(ICgDisplayButtonGroupDirtyRects& dirty);
    void ResetAtVslot90();
    void DrawAtVslot60(const RECT& clip);
    void CompositeAtVslot5C(const std::shared_ptr<ILoadAfterTransitionSource>& target,
        GraphicsSurfaceDestination destination, const RECT* clip);
    void Release();

private:
    std::variant<SceneManagerOptionCharNativeReference,
        std::shared_ptr<ICgDisplayButtonGroupSelector>> storage_{std::in_place_index<0>};
};
}

#pragma once

#include <Runtime/SaveLoadPageObjectGraphRuntime.h>
#include <Runtime/SaveLoadPopupObjectGraphRuntime.h>
#include <Runtime/SaveLoadSystemRuntime.h>

#include <array>
#include <cstdint>
#include <vector>

namespace KaGuYa::Start::Runtime
{
class SaveLoadDialogProductionRuntime;

class ISaveLoadPageCoordinatorHost
{
public:
    virtual ~ISaveLoadPageCoordinatorHost() = default;

    [[nodiscard]] virtual SaveLoadDialogChildRef ResolveChildAtVslot90(
        std::int32_t slot) = 0;
    virtual void InsertChildAtVslot8C(
        const SaveLoadDialogChildRef& child) = 0;
};

class ISaveLoadPageCoordinatorFactory
{
public:
    virtual ~ISaveLoadPageCoordinatorFactory() = default;

    [[nodiscard]] virtual SaveLoadDialogChildRef CreatePage00At4ED190(
        std::int32_t slot) = 0;
    [[nodiscard]] virtual SaveLoadDialogChildRef CreatePage01At4EE390(
        std::int32_t slot) = 0;
    [[nodiscard]] virtual SaveLoadPageModeSystemRef CreateSystemAt4F0C80(
        bool saveMode) = 0;
    [[nodiscard]] virtual SaveLoadDialogChildRef CreatePopupAt4EF660(
        std::int32_t slot) = 0;
};

// SaveLoadDialog::0x5330A0 的八槽页面装配器。它只负责原函数已证明的
// 创建、插入、启停和 0x535160 同步，不替代 Page/Popup 自身的状态机。
class SaveLoadPageCoordinatorRuntime final
{
public:
    SaveLoadPageCoordinatorRuntime(
        ISaveLoadPageCoordinatorHost& host,
        ISaveLoadPageCoordinatorFactory& factory) noexcept;

    void InitializePageAt5330A0(std::int32_t mode);

private:
    [[nodiscard]] SaveLoadDialogChildRef Resolve(std::int32_t slot);
    void WriteEnabledIfPresent(std::int32_t slot, std::int32_t value);
    [[nodiscard]] SaveLoadDialogChildRef InsertCreated(
        const SaveLoadDialogChildRef& child);

    ISaveLoadPageCoordinatorHost* host_ = nullptr;
    ISaveLoadPageCoordinatorFactory* factory_ = nullptr;
};

struct SaveLoadDialogChildObjectGraphDependencies final
{
    ICgOperationSurfaceFactory& surfaceFactory;
    IGraphicsSurfaceCompositionApi& graphics;
    ICgOperationButtonInputServices& buttonInputServices;
    ICgOperationSliderInputServices& sliderInputServices;
    ISaveLoadPageInheritedServices& pageServices;
    ISaveLoadSystemInheritedServices& systemServices;
    ISaveLoadPopupInheritedServices& popupServices;
    RECT dialogBounds{};
};

// 0x4EB950 先装载共享 SaveLoad 视觉资源，0x5330A0 再按需填充八个
// child 槽。本 owner 同时承担原父对象的槽查询、插入与具体工厂边界；
// pendingSlot 只由刚完成的确定创建入口写入，不根据对象类型反推槽位。
class SaveLoadDialogChildObjectGraphRuntime final
    : public ISaveLoadDialogObjectGraph,
      private ISaveLoadPageCoordinatorHost,
      private ISaveLoadPageCoordinatorFactory
{
public:
    explicit SaveLoadDialogChildObjectGraphRuntime(
        SaveLoadDialogChildObjectGraphDependencies dependencies);

    void InitializePageAt5330A0(std::int32_t mode) override;
    void OpenChildrenAt532560() override;

    [[nodiscard]] const SaveLoadDialogVisualResourcesRuntime&
        VisualResources() const noexcept;
    [[nodiscard]] const std::array<SaveLoadDialogChildRef, 8>&
        Slots() const noexcept;
    [[nodiscard]] const std::vector<SaveLoadDialogChildRef>&
        ChildrenInInsertionOrder() const noexcept override;
    [[nodiscard]] SaveLoadDialogControlRef
        ResolveDialogControlAtVslot90(std::int32_t slot) override;

private:
    friend class SaveLoadDialogProductionRuntime;

    void ReleaseVisualResourcesAt4F9030() noexcept;
    void ReleaseChildrenAt4F6DC0() noexcept;

    [[nodiscard]] SaveLoadDialogChildRef ResolveChildAtVslot90(
        std::int32_t slot) override;
    void InsertChildAtVslot8C(
        const SaveLoadDialogChildRef& child) override;
    [[nodiscard]] SaveLoadDialogChildRef CreatePage00At4ED190(
        std::int32_t slot) override;
    [[nodiscard]] SaveLoadDialogChildRef CreatePage01At4EE390(
        std::int32_t slot) override;
    [[nodiscard]] SaveLoadPageModeSystemRef CreateSystemAt4F0C80(
        bool saveMode) override;
    [[nodiscard]] SaveLoadDialogChildRef CreatePopupAt4EF660(
        std::int32_t slot) override;

    void RememberCreatedSlot(std::int32_t slot);

    SaveLoadDialogChildObjectGraphDependencies dependencies_;
    SaveLoadDialogVisualResourcesRuntime visualResources_;
    SaveLoadPageCoordinatorRuntime coordinator_;
    std::array<SaveLoadDialogChildRef, 8> slots_{};
    std::vector<SaveLoadDialogChildRef> childrenInInsertionOrder_;
    std::int32_t pendingCreatedSlot_ = -1;
};
}

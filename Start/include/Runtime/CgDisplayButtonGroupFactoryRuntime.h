#pragma once

#include <Runtime/CgDisplayButtonGroupRuntime.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace KaGuYa::Start::Runtime
{
// 0x499DC0 使用的资源名既可能来自静态指针槽，也可能来自全局对象的
// vslot +0x1C。这里保留不透明标识，禁止 factory 猜测或复制样本资源表内容。
class ICgDisplayButtonGroupResourceIdentifierAt499DC0
{
public:
    virtual ~ICgDisplayButtonGroupResourceIdentifierAt499DC0() = default;
};

using CgDisplayButtonGroupResourceIdentifierRefAt499DC0 =
    std::shared_ptr<const ICgDisplayButtonGroupResourceIdentifierAt499DC0>;

struct CgDisplayButtonGroupCategoryItemAt499DC0 final
{
    // 49A79A..49A817：条目 ref_object+0x10 给出的元素总数。
    std::int32_t refObjectCountAtPlus10 = 0;
    // 49A7B2..49A814：上述元素索引到全局 12-byte 表后，+0x04 非零的数量。
    std::int32_t referencedGlobalEntryByteAtPlus04NonzeroCount = 0;
};

class ICgDisplayButtonGroupCategoryContainerAt499DC0
{
public:
    virtual ~ICgDisplayButtonGroupCategoryContainerAt499DC0() = default;

    // 499E67..499EE5：运行时 category 容器 +0x18/+0x1C 中的 16-byte
    // category 记录，其 +0x04/+0x08 是 12-byte item 引用区间。
    [[nodiscard]] virtual std::int32_t ItemCountFromOffsets18And1C(
        std::int32_t categoryIndex) const = 0;
    [[nodiscard]] virtual std::optional<CgDisplayButtonGroupCategoryItemAt499DC0>
        TryReadItemFromOffsets18And1C(
            std::int32_t categoryIndex,
            std::int32_t itemIndex) const = 0;

    // 499F10..499F60：仅 category 0..3 使用 slot 1 首元素的 vslot +0x84。
    [[nodiscard]] virtual std::uint8_t
        ReadByteFromRuntimeSlot1Element0Vslot84(
            std::int32_t categoryIndex) const = 0;
};

class ICgDisplayButtonGroupCategoryVslot24At499DC0
{
public:
    virtual ~ICgDisplayButtonGroupCategoryVslot24At499DC0() = default;

    // 49A308..49A35D：category 对象 vslot +0x24 的返回值随后交给 0x4BC950。
    [[nodiscard]] virtual std::int32_t ReadAtVslot24(
        std::int32_t categoryIndex) = 0;
};

class ICgDisplayButtonGroupGlobalVslot1CAt499DC0
{
public:
    virtual ~ICgDisplayButtonGroupGlobalVslot1CAt499DC0() = default;

    // 49AAA2..49AAF3：全局容器第四个 12-byte 项的 vslot +0x1C。
    // 空 optional 对应原函数返回空指针的固定占位分支。
    [[nodiscard]] virtual std::optional<
        CgDisplayButtonGroupResourceIdentifierRefAt499DC0>
        ReadAtGlobalVslot1C(
            std::int32_t categoryIndex,
            std::int32_t itemIndex) = 0;
};

class ICgDisplayButtonGroupStaticDataAt499DC0
{
public:
    virtual ~ICgDisplayButtonGroupStaticDataAt499DC0() = default;

    // pointerAddress 必须是样本 18 映像中的原始指针槽地址；返回空引用即原槽为 0。
    [[nodiscard]] virtual CgDisplayButtonGroupResourceIdentifierRefAt499DC0
        ReadResourcePointerAtAddress(std::uintptr_t pointerAddress) const = 0;
    [[nodiscard]] virtual std::int32_t ReadDwordAtAddress(
        std::uintptr_t address) const = 0;

    // 0x417130 对三个固定格式串的调用保留在注入层，不在 factory 内重写格式内容。
    [[nodiscard]] virtual CgDisplayButtonGroupResourceIdentifierRefAt499DC0
        FormatResourceIdentifierAt417130(
            std::uintptr_t formatAddress,
            std::int32_t categoryIndex,
            std::int32_t itemIndex) const = 0;

    // 49B331：+0x20 最终直接指向 0x6021E8 的完整 27x4 表。
    [[nodiscard]] virtual std::vector<std::array<std::int32_t, 4>>
        ReadNavigationTableAt6021E8() const = 0;
};

class ICgDisplayButtonGroupGraphicsProviderAt499DC0
{
public:
    virtual ~ICgDisplayButtonGroupGraphicsProviderAt499DC0() = default;

    [[nodiscard]] virtual IGraphicsSurfaceCompositionApi& CompositionApi() = 0;
    [[nodiscard]] virtual std::shared_ptr<RuntimeGraphicsSurface>
        AllocateSurfaceAt40C2E0() = 0;
    [[nodiscard]] virtual bool LoadSurfaceAt4800A0(
        RuntimeGraphicsSurface& destination,
        const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& resource) = 0;
    [[nodiscard]] virtual bool CreateSurfaceAt415010(
        RuntimeGraphicsSurface& destination,
        std::int32_t width,
        std::int32_t height,
        std::int32_t alphaArgument,
        std::int32_t clearArgument) = 0;
    [[nodiscard]] virtual std::shared_ptr<RuntimeGraphicsSurface>
        CopySurfaceAt444470(
            const RuntimeGraphicsSurface& source,
            std::int32_t copyPixelsArgument) = 0;

    // 0x4425C0 的返回值在 0x499DC0 中从不参与分支。
    virtual void BlitSurfaceAt4425C0(
        RuntimeGraphicsSurface& destination,
        const RuntimeGraphicsSurface& source,
        std::int32_t destinationX,
        std::int32_t destinationY) = 0;

    [[nodiscard]] virtual bool DrawValueAt4BC950(
        std::int32_t value,
        RuntimeGraphicsSurface& destination,
        std::uintptr_t resourcePointerTableAddress,
        std::uintptr_t coordinateTableAddress) = 0;
    [[nodiscard]] virtual bool DrawValueAt4B4D20(
        std::int32_t value,
        RuntimeGraphicsSurface& destination,
        std::uintptr_t resourcePointerTableAddress,
        std::uintptr_t xCoordinateDwordArrayAddress,
        std::int32_t y) = 0;
};

class ICgDisplayButtonGroupButtonFactoryAt499DC0
{
public:
    virtual ~ICgDisplayButtonGroupButtonFactoryAt499DC0() = default;

    [[nodiscard]] virtual std::shared_ptr<CgUiButtonRuntime>
        AllocateButtonAt459530() = 0;

    // 0x4C1F20 会维护 CGUIButton 的命名 Surface 表、state slot 与联合包围矩形；
    // 这些字段不能仅靠现有公开 setter 完整复刻，因此必须由生产注入实现。
    [[nodiscard]] virtual bool AttachSurfaceAt4C1F20(
        CgUiButtonRuntime& button,
        std::uint32_t stateSurfaceSlot,
        const CgDisplayButtonGroupResourceIdentifierRefAt499DC0& identifier,
        const std::shared_ptr<RuntimeGraphicsSurface>& surface) = 0;

    // 49B213..49B24D：recordAddress 依次为 0x5B3FC0 与 0x5B3FE8，
    // finalArgument 固定为 0。接口实现不得补造 40-byte 记录内容。
    [[nodiscard]] virtual bool LoadButtonAt480930(
        CgUiButtonRuntime& button,
        std::uintptr_t recordAddress,
        std::int32_t finalArgument) = 0;
};

class ICgDisplayPublicControlsSourceAt499DC0
{
public:
    virtual ~ICgDisplayPublicControlsSourceAt499DC0()=default;
    virtual CgDisplayPublicSelectorRuntime CaptureSelectorAt478A30()=0;
    virtual CgDisplayPublicModeButtonsRuntime CaptureModesAt47B7B0()=0;
};

// 既有明确受控生产者的适配域；不是原生失败后的替代路径。
class ICgDisplayButtonGroupSharedControlsAt499DC0:public ICgDisplayPublicControlsSourceAt499DC0
{
public:
    virtual ~ICgDisplayButtonGroupSharedControlsAt499DC0() = default;

    // 0x49B2DD 的 0x478A30 先写入 CCGdispBtn+0x78。
    [[nodiscard]] virtual std::shared_ptr<ICgDisplayButtonGroupSelector>
        AcquireSelectorAt478A30() = 0;
    // 0x49B323 的 0x47B7B0 随后写入 CCGdispBtn+0x44。
    [[nodiscard]] virtual std::vector<std::shared_ptr<CgUiButtonRuntime>>
        AcquireModeButtonsAt47B7B0() = 0;
    CgDisplayPublicSelectorRuntime CaptureSelectorAt478A30() override{return AcquireSelectorAt478A30();}
    CgDisplayPublicModeButtonsRuntime CaptureModesAt47B7B0() override{return AcquireModeButtonsAt47B7B0();}
};

struct CgDisplayButtonGroupFactoryDependenciesAt499DC0 final
{
    ICgDisplayButtonGroupRuntimeServices& runtimeServices;
    ICgDisplayButtonGroupCategoryContainerAt499DC0& categoryContainer;
    ICgDisplayButtonGroupCategoryVslot24At499DC0& categoryVslot24;
    ICgDisplayButtonGroupGlobalVslot1CAt499DC0& globalVslot1C;
    ICgDisplayButtonGroupStaticDataAt499DC0& staticData;
    ICgDisplayButtonGroupGraphicsProviderAt499DC0& graphicsProvider;
    ICgDisplayButtonGroupButtonFactoryAt499DC0& buttonFactory;
    ICgDisplayPublicControlsSourceAt499DC0& sharedControls;
};

// 精确对应样本 18 ModeCGdispScene::CCGdispBtn::0x499DC0。
// 构造阶段失败时按原函数直接抛出 int：1/2/4/5/6/7/8/9/10。
[[nodiscard]] std::unique_ptr<CgDisplayButtonGroupRuntime>
CreateCgDisplayButtonGroupAt499DC0(
    std::int32_t categoryIndex,
    CgDisplayButtonGroupFactoryDependenciesAt499DC0 dependencies);
}

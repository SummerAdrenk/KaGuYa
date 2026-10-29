#pragma once

#include <Runtime/SaveLoadMessageBoxNative.h>
#include <Runtime/GuiControlFactoryNative.h>

namespace KaGuYa::Start::Runtime
{
struct SaveLoadMessageBoxCheckBoxDescriptorNative48h final
{
    std::int32_t eventAt00;
    std::array<const char*, 7> normalNamesAt04;
    std::array<const char*, 7> checkedNamesAt20;
    const POINT* positionAt3C;
    std::int32_t hitSlotAt40;
    std::int32_t childIdAt44;
};
struct SaveLoadMessageBoxMultiCheckItemNative08h final
{
    const char* nameAt00;
    std::int32_t hitSlotAt04;
};
struct SaveLoadMessageBoxMultiCheckDescriptorNative44h final
{
    std::int32_t eventAt00;
    std::array<SaveLoadMessageBoxMultiCheckItemNative08h, 7> itemsAt04;
    const POINT* positionAt3C;
    std::int32_t childIdAt40;
};
struct SaveLoadMessageBoxMultiHotkeyDescriptorNative2Ch final
{
    GuiStaticDescriptorNative24h common;
    std::int32_t hitSlotAt24;
    const std::int32_t* terminatedHotkeysAt28;
};
using SaveLoadMessageBoxRadioDescriptorNative2Ch = GuiButtonDescriptorNative2Ch;
static_assert(sizeof(SaveLoadMessageBoxCheckBoxDescriptorNative48h) == 0x48);
static_assert(offsetof(SaveLoadMessageBoxCheckBoxDescriptorNative48h, positionAt3C) == 0x3C);
static_assert(offsetof(SaveLoadMessageBoxCheckBoxDescriptorNative48h, childIdAt44) == 0x44);
static_assert(sizeof(SaveLoadMessageBoxMultiCheckItemNative08h) == 8);
static_assert(sizeof(SaveLoadMessageBoxMultiCheckDescriptorNative44h) == 0x44);
static_assert(offsetof(SaveLoadMessageBoxMultiCheckDescriptorNative44h, positionAt3C) == 0x3C);
static_assert(offsetof(SaveLoadMessageBoxMultiCheckDescriptorNative44h, childIdAt40) == 0x40);
static_assert(sizeof(SaveLoadMessageBoxMultiHotkeyDescriptorNative2Ch) == 0x2C);
static_assert(offsetof(SaveLoadMessageBoxMultiHotkeyDescriptorNative2Ch, terminatedHotkeysAt28) == 0x28);

// 工厂只借用显式叶依赖；不捕获Root，不委托整段算法，不读取PE。
struct SaveLoadMessageBoxNativeChildFactoriesDependencies final
{
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IRuntimeStringManagerSource& strings;
};
class SaveLoadMessageBoxNativeChildFactoriesRuntime final
    : public ISaveLoadMessageBoxNativeChildFactories
{
public:
    explicit SaveLoadMessageBoxNativeChildFactoriesRuntime(
        SaveLoadMessageBoxNativeChildFactoriesDependencies dependencies) noexcept;
    // freshOutput不是旧Ref赋值目标。scope须覆盖构造/EH、虚调用及最后一份Ref释放。
    YtlReferenceSlotRuntime* CreateAt5043B0(YtlReferenceSlotRuntime* freshOutput,
        GuiDialogNativeBody5Ch* parent, const void* descriptor48h) override;
    YtlReferenceSlotRuntime* CreateAt503BB0(YtlReferenceSlotRuntime* freshOutput,
        GuiDialogNativeBody5Ch* parent, const void* descriptor44h) override;
    YtlReferenceSlotRuntime* CreateAt503E60(YtlReferenceSlotRuntime* freshOutput,
        GuiDialogNativeBody5Ch* parent, const void* descriptor2Ch) override;
    YtlReferenceSlotRuntime* CreateAt505090(YtlReferenceSlotRuntime* freshOutput,
        GuiDialogNativeBody5Ch* parent, const void* descriptor2Ch) override;
private:
    SaveLoadMessageBoxNativeChildFactoriesDependencies dependencies_;
};

struct SaveLoadMessageBoxMultiCheckNativeBody78h final
{
    union { GuiButtonNativeBody5Ch button; };
    std::int32_t hitSlotsAt5C[7];
    SaveLoadMessageBoxMultiCheckNativeBody78h() noexcept {}
};
struct SaveLoadMessageBoxMultiHotkeyNativeBody68h final
{
    union { GuiButtonNativeBody5Ch button; };
    union { EnvironmentVectorStorage<YtlReferenceSlotRuntime> hotkeysAt5C; };
    SaveLoadMessageBoxMultiHotkeyNativeBody68h() noexcept {}
};
struct SaveLoadMessageBoxHotkeyAndStateNativeBody08h final
{
    std::int32_t hotkeyAt00;
    std::int32_t stateAt04;
};
struct SaveLoadMessageBoxMultiCheckNativeVtable31 final
{
    GuiButtonNativeVtable29 button;
    void (__thiscall* resetMappedHitSlotsAt74)(GuiControlNativeBody50h*);
    std::int32_t (__thiscall* setMappedHitSlotAt78)(GuiControlNativeBody50h*, std::int32_t, std::int32_t);
};
static_assert(sizeof(SaveLoadMessageBoxMultiCheckNativeBody78h) == 0x78);
static_assert(offsetof(SaveLoadMessageBoxMultiCheckNativeBody78h, hitSlotsAt5C) == 0x5C);
static_assert(sizeof(SaveLoadMessageBoxMultiHotkeyNativeBody68h) == 0x68);
static_assert(offsetof(SaveLoadMessageBoxMultiHotkeyNativeBody68h, hotkeysAt5C) == 0x5C);
static_assert(sizeof(SaveLoadMessageBoxHotkeyAndStateNativeBody08h) == 8);
static_assert(sizeof(SaveLoadMessageBoxMultiCheckNativeVtable31) == 0x7C);
static_assert(offsetof(SaveLoadMessageBoxMultiCheckNativeVtable31, setMappedHitSlotAt78) == 0x78);
static_assert(std::is_standard_layout_v<SaveLoadMessageBoxMultiCheckNativeBody78h>);
static_assert(std::is_standard_layout_v<SaveLoadMessageBoxMultiHotkeyNativeBody68h>);
static_assert(std::is_trivially_destructible_v<SaveLoadMessageBoxMultiHotkeyNativeBody68h>);
const SaveLoadMessageBoxMultiCheckNativeVtable31& SaveLoadMessageBoxMultiCheckNativeVtable() noexcept;
const GuiButtonNativeVtable29& SaveLoadMessageBoxMultiHotkeyNativeVtable() noexcept;
const GuiButtonNativeVtable29& SaveLoadMessageBoxRadioNativeVtable() noexcept;

// 原地址只作静态审计元数据；运行时绝不将这些VA当作函数/资源指针。
struct SaveLoadMessageBoxChildFactoryNativeMetadata final
{
    std::uint32_t factoryAddress, descriptorSize, bodySize, vtableAddress;
    std::uint32_t rttiLocatorAddress, typeDescriptorAddress;
    const char* typeName;
    std::span<const std::uint32_t> originalSlots;
    std::uint32_t referenceVtableAddress, referenceRttiLocatorAddress;
    std::uint32_t referenceDestroyAddress, referenceDeleteAddress;
    std::uint32_t ehInfoAddress;
};
std::span<const SaveLoadMessageBoxChildFactoryNativeMetadata, 4>
SaveLoadMessageBoxNativeChildFactoriesMetadata() noexcept;
}

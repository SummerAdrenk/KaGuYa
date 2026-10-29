#pragma once
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 4F3740 原7Ch，60为UnionRect结果；5C与60..6F直到各自业务时点才写。
struct SettingSystemPageNativeBody7Ch final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t resultAt5C;
    RECT boundsAt60;
    union { YtlReferenceSlotRuntime managerAt70; };
    SettingSystemPageNativeBody7Ch() noexcept {}
};
static_assert(sizeof(SettingSystemPageNativeBody7Ch) == 0x7C);
static_assert(alignof(SettingSystemPageNativeBody7Ch) == 4);
static_assert(std::is_standard_layout_v<SettingSystemPageNativeBody7Ch>);
static_assert(std::is_trivially_destructible_v<SettingSystemPageNativeBody7Ch>);
static_assert(offsetof(SettingSystemPageNativeBody7Ch, resultAt5C) == 0x5C);
static_assert(offsetof(SettingSystemPageNativeBody7Ch, boundsAt60) == 0x60);
static_assert(offsetof(SettingSystemPageNativeBody7Ch, managerAt70) == 0x70);
class ISettingSystemPageNativeSource
{
public:
    virtual ~ISettingSystemPageNativeSource() = default;
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    // 固定Root借用，不重新Ensure/选根。原只在Find20h非空时读取。
    virtual std::uint8_t RootByteAt218(void* fixedRoot) = 0;
};
struct SettingSystemPageNativeServices final
{
    ISettingSystemPageNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
class SettingSystemPageNativeScope final
{
public:
    explicit SettingSystemPageNativeScope(SettingSystemPageNativeServices&) noexcept;
    explicit SettingSystemPageNativeScope(SettingSystemPageNativeServices*) noexcept;
    ~SettingSystemPageNativeScope() noexcept;
    SettingSystemPageNativeScope(const SettingSystemPageNativeScope&) = delete;
    SettingSystemPageNativeScope& operator=(const SettingSystemPageNativeScope&) = delete;
private:
    SettingSystemPageNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
SettingSystemPageNativeServices& RequireSettingSystemPageNativeServices();
const GuiDialogNativeVtable35& SettingSystemPageNativeVtable() noexcept;
SettingSystemPageNativeBody7Ch* ConstructSettingSystemPageNativeAt4F3740(
    void* storage7Ch, GuiDialogNativeBody5Ch* parent, std::int32_t id);
void DestroySettingSystemPageNativeAt4FE410(SettingSystemPageNativeBody7Ch*) noexcept;
SettingSystemPageNativeBody7Ch* DeleteSettingSystemPageNativeAt4FE410(
    SettingSystemPageNativeBody7Ch*, std::uint32_t flags) noexcept;
const GuiStaticDescriptorNative24h& SettingSystemPageNativeStaticSample18() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 7> SettingSystemPageNativeButtonsSample18() noexcept;
std::span<const std::uint32_t, 35> SettingSystemPageNativeOriginalVtableSample18() noexcept;
std::span<const std::uint32_t, 9> SettingSystemPageNativeRawStaticSample18() noexcept;
std::span<const std::array<std::uint32_t, 11>, 7> SettingSystemPageNativeRawButtonsSample18() noexcept;
std::span<const std::array<std::uint32_t, 2>, 14> SettingSystemPageNativeUnwindSample18() noexcept;
}

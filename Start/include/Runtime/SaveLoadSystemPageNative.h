#pragma once

#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/InputRootNative.h>
#include <array>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 4F0C80的80h正文；+70是Ref12，不是内嵌vector。构造不写+5C/+7C。
struct SaveLoadSystemPageNativeBody80h final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t resultAt5C;
    RECT boundsAt60;
    union { YtlReferenceSlotRuntime managerAt70; };
    std::int32_t keyboardEventAt7C;
    SaveLoadSystemPageNativeBody80h() noexcept {}
};
static_assert(sizeof(SaveLoadSystemPageNativeBody80h) == 0x80);
static_assert(alignof(SaveLoadSystemPageNativeBody80h) == 4);
static_assert(std::is_standard_layout_v<SaveLoadSystemPageNativeBody80h>);
static_assert(std::is_trivially_destructible_v<SaveLoadSystemPageNativeBody80h>);
static_assert(offsetof(SaveLoadSystemPageNativeBody80h, common) == 0);
static_assert(offsetof(SaveLoadSystemPageNativeBody80h, resultAt5C) == 0x5C);
static_assert(offsetof(SaveLoadSystemPageNativeBody80h, boundsAt60) == 0x60);
static_assert(offsetof(SaveLoadSystemPageNativeBody80h, managerAt70) == 0x70);
static_assert(offsetof(SaveLoadSystemPageNativeBody80h, keyboardEventAt7C) == 0x7C);

// 只暴露singleton/字段来源；不委托Begin/Poll/输入/整页构造。
class ISaveLoadSystemPageNativeSource
{
public:
    virtual ~ISaveLoadSystemPageNativeSource() = default;
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    virtual std::uint8_t RootByteAt218(void* capturedRoot) = 0;
    virtual InputRootNativeBody24h* InputAt418210() = 0;
};
class RootRuntimeOwner;
// 生产peer借同Root/Input owner，不复制根状态，不在构造期Capture。
class RootSaveLoadSystemPageNativeSource final : public ISaveLoadSystemPageNativeSource
{
public:
    RootSaveLoadSystemPageNativeSource(RootRuntimeOwner& root, InputRootOwner& input) noexcept
        : root_(root), input_(input) {}
    void EnsureRootAt413BE0() override;
    void* RootAt6198C4() override;
    std::uint8_t RootByteAt218(void*) override;
    InputRootNativeBody24h* InputAt418210() override;
private:
    RootRuntimeOwner& root_;
    InputRootOwner& input_;
};
struct SaveLoadSystemPageNativeServices final
{
    ISaveLoadSystemPageNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
// scope必须覆盖Begin/Poll/End及逃逸的child/page最后一次Ref释放；不是构造期owner。
class SaveLoadSystemPageNativeScope final
{
public:
    explicit SaveLoadSystemPageNativeScope(SaveLoadSystemPageNativeServices&) noexcept;
    explicit SaveLoadSystemPageNativeScope(SaveLoadSystemPageNativeServices*) noexcept;
    ~SaveLoadSystemPageNativeScope() noexcept;
    SaveLoadSystemPageNativeScope(const SaveLoadSystemPageNativeScope&) = delete;
    SaveLoadSystemPageNativeScope& operator=(const SaveLoadSystemPageNativeScope&) = delete;
private:
    SaveLoadSystemPageNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
SaveLoadSystemPageNativeServices& RequireSaveLoadSystemPageNativeServices();
const GuiDialogNativeVtable35& SaveLoadSystemPageNativeVtable() noexcept;
GuiDialogNativeBody5Ch* ConstructSaveLoadSystemPageNativeAt4F0C80(void* raw80,
    GuiDialogNativeBody5Ch* parent, std::int32_t id, SaveLoadSystemPageNativeServices&);
void DestroySaveLoadSystemPageNativeAt4F93F0(SaveLoadSystemPageNativeBody80h*) noexcept;
SaveLoadSystemPageNativeBody80h* DeleteSaveLoadSystemPageNativeAt4FE0A0(
    SaveLoadSystemPageNativeBody80h*, std::uint32_t flags) noexcept;

// 正文外compiled evidence；地址只用于审计，Runtime从不打开或装载原EXE。
struct SaveLoadSystemPageNativeGroup final
{
    std::uint32_t headerVa, descriptorVa;
    std::int32_t kind, count;
    const GuiButtonDescriptorNative2Ch* descriptors;
    const std::array<std::uint32_t, 11>* originalRows;
};
const SaveLoadSystemPageNativeGroup& SaveLoadSystemPageNativeGroupForId(std::int32_t id) noexcept;
std::span<const std::uint32_t, 35> SaveLoadSystemPageNativeOriginalSlots() noexcept;
std::span<const std::int32_t, 6> SaveLoadSystemPageNativeShortcuts() noexcept;
}

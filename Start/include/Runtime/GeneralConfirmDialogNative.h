#pragma once

#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiClickedEventNative.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GeneralConfirmResourceProfile.h>
#include <Runtime/GuiModalNativeAdapters.h>
#include <Runtime/RootTransitionFrameRuntime.h>
#include <Runtime/ModalDialogRunnerRuntime.h>
#include <array>
#include <span>

namespace KaGuYa::Start::Platform { class IInputRuntime; }
namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;

// 4E3330：原84h正文；不嵌入现代确认框，不提前初始化68/6C/70。
struct GeneralConfirmDialogNativeBody84h final
{
    union { GuiDialogNativeBody5Ch common; };
    union { YtlReferenceSlotRuntime capturedAt5C; };
    std::int32_t resultAt68;
    std::int32_t phaseAt6C;
    std::int32_t actionFiveAt70;
    union { GuiClickedEventReferences eventsAt74; };
    std::int32_t modeAt80;
    GeneralConfirmDialogNativeBody84h() noexcept {}
};
static_assert(sizeof(GeneralConfirmDialogNativeBody84h) == 0x84);
static_assert(alignof(GeneralConfirmDialogNativeBody84h) == 4);
static_assert(std::is_standard_layout_v<GeneralConfirmDialogNativeBody84h>);
static_assert(std::is_trivially_destructible_v<GeneralConfirmDialogNativeBody84h>);
#define GENERAL_CONFIRM_OFFSET(field, offset) static_assert(offsetof(GeneralConfirmDialogNativeBody84h, field) == offset)
GENERAL_CONFIRM_OFFSET(common, 0); GENERAL_CONFIRM_OFFSET(capturedAt5C, 0x5C);
GENERAL_CONFIRM_OFFSET(resultAt68, 0x68); GENERAL_CONFIRM_OFFSET(phaseAt6C, 0x6C);
GENERAL_CONFIRM_OFFSET(actionFiveAt70, 0x70); GENERAL_CONFIRM_OFFSET(eventsAt74, 0x74);
GENERAL_CONFIRM_OFFSET(modeAt80, 0x80);
#undef GENERAL_CONFIRM_OFFSET

// 5BE28C..5BE320恰38槽；5BE324是下一类COL，不是第39槽。
struct GeneralConfirmDialogNativeVtable38 final
{
    GuiDialogNativeVtable35 common;
    void (__thiscall* beginHookAt8C)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* readEventsAt90)(GuiDialogNativeBody5Ch*);
    void (__thiscall* endHookAt94)(GuiDialogNativeBody5Ch*);
};
static_assert(sizeof(GeneralConfirmDialogNativeVtable38) == 0x98);
static_assert(alignof(GeneralConfirmDialogNativeVtable38) == 4);
static_assert(std::is_standard_layout_v<GeneralConfirmDialogNativeVtable38>);
static_assert(offsetof(GeneralConfirmDialogNativeVtable38, common) == 0);
static_assert(offsetof(GeneralConfirmDialogNativeVtable38, beginHookAt8C) == 0x8C);
static_assert(offsetof(GeneralConfirmDialogNativeVtable38, readEventsAt90) == 0x90);
static_assert(offsetof(GeneralConfirmDialogNativeVtable38, endHookAt94) == 0x94);

struct GeneralConfirmDialogNativeServices final
{
    IGuiControlResourceLoadServices& pictures;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IGuiModalRootSource& root;
    IModalDialogRunnerServices& modal;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
// 同线程LIFO作用域：必须覆盖构造、所有回调及最后一个Ref释放，不能仅包Ctor。
class GeneralConfirmDialogNativeScope final
{
public:
    explicit GeneralConfirmDialogNativeScope(GeneralConfirmDialogNativeServices&) noexcept;
    explicit GeneralConfirmDialogNativeScope(GeneralConfirmDialogNativeServices*) noexcept;
    ~GeneralConfirmDialogNativeScope() noexcept;
    GeneralConfirmDialogNativeScope(const GeneralConfirmDialogNativeScope&) = delete;
    GeneralConfirmDialogNativeScope& operator=(const GeneralConfirmDialogNativeScope&) = delete;
private:
    GeneralConfirmDialogNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
GeneralConfirmDialogNativeServices& RequireGeneralConfirmDialogNativeServices();
const GeneralConfirmDialogNativeVtable38& GeneralConfirmDialogNativeVtable() noexcept;
std::span<const std::uint32_t, 38> GeneralConfirmDialogNativeOriginalSlots() noexcept;
std::span<const GuiStaticDescriptorNative24h> GeneralConfirmDialogNativeStaticDescriptors();
std::span<const GuiButtonDescriptorNative2Ch> GeneralConfirmDialogNativeButtonDescriptors();
std::span<const GeneralConfirmModeRow> GeneralConfirmDialogNativeModeRows();
std::span<const std::array<std::uint32_t, 2>, 15> GeneralConfirmDialogNativeCtorUnwindRows() noexcept;

GuiDialogNativeBody5Ch* ConstructGeneralConfirmDialogNativeAt4E3330(void* storage, std::int32_t mode);
void DestroyGeneralConfirmDialogNativeAt4F80D0(GuiDialogNativeBody5Ch*) noexcept;
GuiDialogNativeBody5Ch* DeleteGeneralConfirmDialogNativeAt4FCAC0(
    GuiDialogNativeBody5Ch*, std::uint32_t flags) noexcept;
// 4515E0 fresh Ref：非空正文先发布base，new18失败保留孤儿，不补删正文。
YtlReferenceSlotRuntime* ConstructGeneralConfirmDialogNativeReferenceAt4515E0(
    YtlReferenceSlotRuntime& fresh, GuiDialogNativeBody5Ch*, std::uint8_t owning);
YtlReferenceSlotRuntime* CreateGeneralConfirmDialogNativeReference(
    YtlReferenceSlotRuntime& fresh, std::int32_t mode);
// ownedArgument由调用方转交；正常/异常均消费，不得由调用方再次析构。
void AssignGeneralConfirmDialogNativeCaptureAt48CA30(
    GuiDialogNativeBody5Ch*, YtlReferenceSlotRuntime& ownedArgument);
void ConsumeRootGeneralConfirmDialogNativeCaptureAt477C60(GuiDialogNativeBody5Ch*);
std::int32_t RunGeneralConfirmDialogNativeAt5071D0(YtlReferenceSlotRuntime&);

class GeneralConfirmDialogNativeModalTarget final : public IModalDialogRuntimeTargetReference,
    private IModalDialogRuntimeTarget
{
public:
    explicit GeneralConfirmDialogNativeModalTarget(YtlReferenceSlotRuntime& ref) noexcept : reference_(ref) {}
    IModalDialogRuntimeTarget* ResolveAt4A5330() override;
private:
    GuiDialogNativeBody5Ch& Resolve() const;
    void BeginAtVslot44() override;
    bool PollAtVslot48() override;
    bool EndAtVslot4C() override;
    std::int32_t ResultAtVslot50() const override;
    YtlReferenceSlotRuntime& reference_;
};

// 拥有一次4515E0 Ref；析构体先退役Ref，成员scope随后才退出。不得导出长寿拥有Ref。
class GeneralConfirmDialogNativeSession final
{
public:
    GeneralConfirmDialogNativeSession(GeneralConfirmDialogNativeServices&, std::int32_t mode);
    ~GeneralConfirmDialogNativeSession() noexcept;
    GeneralConfirmDialogNativeSession(const GeneralConfirmDialogNativeSession&) = delete;
    GeneralConfirmDialogNativeSession& operator=(const GeneralConfirmDialogNativeSession&) = delete;
    GuiDialogNativeBody5Ch& Body() const;
    void ConsumeRootCaptureAt477C60();
    std::int32_t RunAt5071D0();
private:
    GeneralConfirmDialogNativeScope scope_;
    YtlReferenceSlotRuntime reference_;
};

// 只借已完成的资源加载、图形、24h输入、Root截图/gate及消息叶；构造不Ensure根。
// 同一Root/Graphics/Input装配由caller保证，绝不转调现代GeneralConfirmation业务。
class RootGeneralConfirmDialogNativeSource final
{
public:
    RootGeneralConfirmDialogNativeSource(RootRuntimeOwner&, NativeGraphicsRuntimeLibrary&,
        const Platform::IInputRuntime&, IGuiControlResourceLoadServices&,
        INativeScreenCaptureServices&, IModalDialogRunnerServices&,
        IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource()) noexcept;
    GeneralConfirmDialogNativeServices& Services() noexcept { return services_; }
private:
    class Screen final : public IGuiDialogNativeScreenSource
    {
    public:
        explicit Screen(NativeGraphicsRuntimeLibrary& graphics) noexcept : graphics_(graphics) {}
        void InvalidateRectAt52603A(const RECT*) override;
    private:
        NativeGraphicsRuntimeLibrary& graphics_;
    } screen_;
    GuiModalInputSource input_;
    RootGuiModalSource root_;
    GeneralConfirmDialogNativeServices services_;
};
}

#pragma once

#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/TransitRuntime.h>
#include <Runtime/NameRegistrationDialogNative.h>
#include <Runtime/GameBuildProfile.h>
#include <Runtime/TitleResourceProfile.h>
#include <optional>
#include <span>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
// Sample18 4F4530/4FE690. No service, mirror, or hidden owning task in the body.
struct TitleDialogNativeBodyD8h final
{
    union { GuiDialogNativeBody5Ch common; };
    std::int32_t phaseAt5C;
    std::int32_t transitionPhaseAt60;
    std::int32_t nextPhaseAt64;
    RECT boundsAt68;
    union { YtlReferenceSlotRuntime blackAt78; };
    union { YtlReferenceSlotRuntime canvasAt84; };
    union { TransitOperation operationAt90; };
    std::int32_t stepAtC0;
    std::int32_t keyboardLatchAtC4;
    union { YtlReferenceSlotRuntime clickedManagerAtC8; };
    std::uint32_t selectionAtD4;
    // Establish storage only; even T{} must not initialize original unwritten bytes.
    TitleDialogNativeBodyD8h() noexcept {}
};
static_assert(sizeof(TitleDialogNativeBodyD8h) == 0xD8);
static_assert(alignof(TitleDialogNativeBodyD8h) == 4);
static_assert(std::is_standard_layout_v<TitleDialogNativeBodyD8h>);
static_assert(std::is_trivially_destructible_v<TitleDialogNativeBodyD8h>);
#define TITLE_OFFSET(field, offset) static_assert(offsetof(TitleDialogNativeBodyD8h, field) == offset)
TITLE_OFFSET(common, 0x00);
TITLE_OFFSET(phaseAt5C, 0x5C);
TITLE_OFFSET(transitionPhaseAt60, 0x60);
TITLE_OFFSET(nextPhaseAt64, 0x64);
TITLE_OFFSET(boundsAt68, 0x68);
TITLE_OFFSET(blackAt78, 0x78);
TITLE_OFFSET(canvasAt84, 0x84);
TITLE_OFFSET(operationAt90, 0x90);
TITLE_OFFSET(stepAtC0, 0xC0);
TITLE_OFFSET(keyboardLatchAtC4, 0xC4);
TITLE_OFFSET(clickedManagerAtC8, 0xC8);
TITLE_OFFSET(selectionAtD4, 0xD4);
#undef TITLE_OFFSET

// PE 5BFE38..5BFEC7: 36 entries, followed by the next RTTI locator at 5BFEC8.
struct TitleDialogNativeVtable36 final
{
    GuiDialogNativeVtable35 common;
    void (__thiscall* redrawAt8C)(GuiDialogNativeBody5Ch*);
};
static_assert(sizeof(TitleDialogNativeVtable36) == 0x90);
static_assert(offsetof(TitleDialogNativeVtable36, redrawAt8C) == 0x8C);

// Non-owning leaf boundary. Root is an opaque, already-captured receiver identity,
// NOT a promise of the original 21Ch layout. Other body/storage ports remain native.
// Never cast a modern Title owner to D8h or synthesize a 21Ch root mirror.
// No Begin/Run/Input/Transition/Populate/Destroy business delegation is offered.
class ITitleDialogNativeSource
{
public:
    virtual ~ITitleDialogNativeSource() = default;
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    // Borrow the selected receiver's unique Ref slot. No Ensure, recapture, copy,
    // AddRef or mirror. Suffixes name original offsets, not C++ root field offsets.
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void* capturedRoot) = 0;
    virtual YtlReferenceSlotRuntime& RootSoundSlotAt24(void* capturedRoot) = 0;
    virtual YtlReferenceSlotRuntime& RootScenesSlotAt30(void* capturedRoot) = 0;
    virtual void* SystemAt4180C0() = 0;
    virtual void* RegistryAt417990(void* system, std::int32_t index) = 0;
    virtual void* SoundParamsAt4188C0(void* registry) = 0;
    virtual const std::int32_t* BgmVolumeAtVslot14(void* params) = 0;
    virtual const std::int32_t* EffectVolumeAtVslot24(void* params) = 0;
    virtual const char* TitleBgmAt61A654() = 0;
    virtual const char* ConfirmSoundAt61A658() = 0;
    virtual const char* HoverSoundAt61A65C() = 0;
    virtual std::uint8_t LoadSurfaceAt480000(void* graphics,
        YtlReferenceSlotRuntime* output, const char* name) = 0;
    virtual std::uint8_t InputAt4C3430(void* registry) = 0;
    virtual std::uint8_t InputAt4AA970(void* registry) = 0;
    virtual std::uint8_t InputAt47F6D0(void* capturedRoot) = 0;
    virtual std::uint8_t LoadAvailableAt485CE0(void* sceneManager) = 0;
    virtual std::uint8_t PlayBgmAt42BFE0(void* soundManager, const char* name,
        std::int32_t volume, std::int32_t loop) = 0;
    virtual std::uint8_t ClearSoundAt4384D0(void* soundManager) = 0;
    virtual void StopSoundAt413270(void* soundManager,
        EnvironmentVectorStorage<RuntimeCString>* names) = 0;
    virtual void* SoundAt4188F0(void* capturedRoot) = 0;
    virtual std::uint8_t StartSoundAt4BCD70(void* soundManager, const char* name,
        std::int32_t channel, std::int32_t volume, std::uint8_t flag) = 0;
    virtual void* ObjectAt50A620(void* system) = 0;
    virtual void* CurrentSceneAt4186D0(void* capturedRoot) = 0;
    // Separate, as-yet externally supplied 94h confirmation class, not a shared target.
    virtual void* ConstructConfirmationAt4E4340(void* storage) = 0;
    // 4AD230没有栈参数；target Ref地址只交5071D0，其ECX不被原函数消费。
    virtual void* PrepareModalAt4AD230(void* capturedRoot) = 0;
    virtual std::int32_t RunModalAt5071D0(void* unusedThis,
        const YtlReferenceSlotRuntime* target) = 0;
    virtual std::int32_t PostMessageAt522D2F(void* window,
        std::uint32_t message, std::uint32_t wparam, std::int32_t lparam) = 0;
    virtual void* GraphicsAt4182F0() = 0;
    virtual void CommitAt4B02E0(void* graphics, std::int32_t argument) = 0;
    // 非原业务元数据，只声明同一94h服务；raw叶夹具可无此绑定。
    virtual NameRegistrationDialogNativeServices* NameRegistrationServices() const noexcept{return nullptr;}
    virtual std::uint8_t NameEditFlags()
    { throw std::logic_error("Title name flags source has not been connected"); }
    // 旧游戏使用自身原生姓名资源，返回1确认、0取消、-1失败；不另造旧标题调度。
    virtual std::int32_t RunResourceNamePrompt()
    { throw std::logic_error("Resource name prompt has not been connected"); }
};

struct TitleDialogNativeServices final
{
    ITitleDialogNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IGraphicsTransitApi& transit;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    // 既有独立夹具默认使用Mamatomo合同；生产owner明确传入编译项的配置。
    TitleDialogProfile profile{TitleDialogFamily::GuiTitleDialog,
        TitleNamePromptGate::PathSwitchAndSavedPreference, TitleButtonResources::Mamatomo,
        TitlePhase5Recovery::ResetChildrenAndParentInput};
};

// Bind for ALL raw ctor/run/delete calls, including the outer Ref's final release.
// Nested scopes restore previous bindings. Neither scope nor services owns a body.
class TitleDialogNativeScope final
{
public:
    explicit TitleDialogNativeScope(TitleDialogNativeServices&) noexcept;
    ~TitleDialogNativeScope() noexcept;
    TitleDialogNativeScope(const TitleDialogNativeScope&) = delete;
    TitleDialogNativeScope& operator=(const TitleDialogNativeScope&) = delete;
private:
    TitleDialogNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
    TransitGraphicsScope transit_;
    std::optional<NameRegistrationDialogNativeScope> name_;
};
TitleDialogNativeServices& RequireTitleDialogNativeServices();
const TitleDialogNativeVtable36& TitleDialogNativeVtable() noexcept;
TitleDialogNativeBodyD8h* ConstructTitleDialogNativeAt4F4530(void* storage,
    TitleDialogNativeServices& services);
// Same constructor for a factory already inside TitleDialogNativeScope.
TitleDialogNativeBodyD8h* ConstructTitleDialogNativeAt4F4530(void* storage);
TitleDialogNativeBodyD8h* DeleteTitleDialogNativeAt4FE690(
    TitleDialogNativeBodyD8h* body, std::uint32_t flags) noexcept;
void AdvanceTitleDialogNativeAt51CC70(TitleDialogNativeBodyD8h& body);
void ProcessTitleSelectionNativeAt522BD0(TitleDialogNativeBodyD8h& body);
bool ShouldPromptForTitleName(ITitleDialogNativeSource& source, TitleNamePromptGate gate);

const GuiStaticDescriptorNative24h& ModernTitleDialogStatic() noexcept;
const GuiStaticDescriptorNative24h& ModernTitleDialogStatic(TitleButtonResources resources);
const GuiStaticDescriptorNative24h& ModernTitleDialogStatic(const TitleDialogProfile& profile);
std::span<const GuiButtonDescriptorNative2Ch, 6> ModernTitleDialogButtons() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 6> ModernTitleDialogButtons(TitleButtonResources resources);
std::span<const GuiButtonDescriptorNative2Ch, 6> ModernTitleDialogButtons(const TitleDialogProfile& profile);
const GuiDialogNavigationMapping* ModernTitleDialogNavigation(std::int32_t index) noexcept;
const char* ModernTitleDialogBlackName() noexcept;
const char* ModernTitleDialogBlackName(const TitleDialogProfile& profile);
const TitleResourceProfile& RequireConfiguredTitleResources(const TitleResourceProfile* profile);
const char* ModernTitleDialogLoadSceneName() noexcept;
}

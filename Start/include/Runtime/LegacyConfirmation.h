#pragma once

#include <Runtime/LegacyTitleQuickLoad.h>
#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/GuiModalNativeAdapters.h>
#include <Runtime/GraphicsSurfaceCaptureRuntime.h>
#include <Runtime/ModalDialogRunnerRuntime.h>
#include <Runtime/SystemDialogNative.h>
#include <Runtime/GraphicsTextNativeApi.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
// Baku 56DD38/56DEC8/56DF10/56DF80：业务描述符，不含原对象偏移或函数地址。
struct LegacyConfirmationData final
{
    std::array<GuiStaticDescriptorNative24h, 11> statics;
    std::array<std::array<std::int32_t, 2>, 9> modes;
    std::array<GuiButtonDescriptorNative2Ch, 2> buttons;
    std::array<std::int32_t, 2> directions;
    std::array<GuiDialogNavigationMapping, 3> navigation;
    GuiStaticDescriptorNative24h systemBackground;
    std::array<GuiButtonDescriptorNative2Ch, 4> systemButtons;
    const char* settingsCommand;
    std::array<GuiStaticDescriptorNative24h, 3> saveStatics;
    std::array<GuiButtonDescriptorNative2Ch, 4> saveButtons;
    std::array<std::array<std::int32_t, 4>, 2> saveDisabled;
    std::array<GuiDialogNavigationMapping, 5> saveNavigation;
    std::array<POINT, 4> labelPositions;
    POINT previewCenter;
    std::array<COLORREF, 2> labelColors;
    const char* fontFace;
};

struct LegacyConfirmationServices final
{
    // 须绑定旧44EB80资源叶；现代GuiControlResourceLoadRuntime的异常时序尚不等价。
    // 当前交付为可测核心，不能仅填现代loader就宣称整个旧profile可投入运行。
    IGuiControlResourceLoadServices& pictures;
    IGraphicsSurfaceApi& surfaces;
    IGraphicsWindowSurfaceApi& drawing;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    INativeScreenCaptureServices& capture;
    IQuickOperationsNativeSource& shared;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    decltype(&::PostMessageA) postMessage = &::PostMessageA;
    ISystemDialogNativeSource* system = nullptr;
    IGraphicsTextApi* text = nullptr;
};

// 4DD0B0/53A42C：具体五叶；统一GuiDialog/控件模型，不复制原C0h正文。
// services必须长于对象及任何逃逸的Surface Ref；不使用GeneralConfirm的Run/End。
class LegacyConfirmation : public ILegacyTitleQuickLoadMessage
{
public:
    LegacyConfirmation(const LegacyConfirmationData&, std::int32_t mode, LegacyConfirmationServices&);
    ~LegacyConfirmation() override;
    LegacyConfirmation(const LegacyConfirmation&) = delete;
    LegacyConfirmation& operator=(const LegacyConfirmation&) = delete;
    void Begin() override;
    bool Poll() override;
    std::int32_t Result() override;
    bool End() override;
    YtlReferenceSlotRuntime& BackgroundSlot() override { return background_; }
    // 原普通退役可能抛；显式释放用于正常路径，noexcept析构只负责展开/最终清理。
    virtual void ReleaseResources();
    const GuiDialogNativeBody5Ch& Controls() const noexcept { return state_.common; }

protected:
    // 共用已核的控件/背景拥有语义；不同真实类型分别覆盖业务，不复刻旧对象布局。
    explicit LegacyConfirmation(LegacyConfirmationServices&);
    struct State final { GuiDialogNativeBody5Ch common; LegacyConfirmation* owner; } state_;
    static LegacyConfirmation& From(GuiDialogNativeBody5Ch*) noexcept;
    static const GuiDialogNativeVtable35& Table();
    static void __fastcall ResetInput(GuiDialogNativeBody5Ch*, void*);
    static std::int32_t __fastcall ActiveInput(GuiDialogNativeBody5Ch*, void*);
    static void __fastcall BeginEntry(GuiDialogNativeBody5Ch*, void*);
    static std::int32_t __fastcall PollEntry(GuiDialogNativeBody5Ch*, void*);
    static std::int32_t __fastcall EndEntry(GuiDialogNativeBody5Ch*, void*);
    static std::int32_t __fastcall ResultEntry(GuiDialogNativeBody5Ch*, void*);
    static void __fastcall Flush(GuiDialogNativeBody5Ch*, void*);
    static void __fastcall ProcessEntry(GuiDialogNativeBody5Ch*, void*);
    static std::int32_t __fastcall ReadStateEntry(GuiDialogNativeBody5Ch*, void*);
    static void __fastcall WriteStateEntry(GuiDialogNativeBody5Ch*, void*, std::int32_t);
    static GuiDialogNativeBody5Ch* __fastcall RejectBaseDelete(GuiDialogNativeBody5Ch*, void*, std::uint32_t);
    void AddBackgroundBounds();
    void AppendStatic(const GuiStaticDescriptorNative24h&);
    void AppendButton(const GuiButtonDescriptorNative2Ch&);
    void AttachEvent(YtlReferenceSlotRuntime& child);
    void CaptureBackground();
    virtual void ProcessInput();
    virtual std::int32_t ReadInteractionState();
    virtual void WriteInteractionState(std::int32_t);
    virtual bool UsesActionFive() const noexcept { return true; }
    virtual void DrawExtras(const RECT*) {}
    virtual GuiClickedEventReferences& Events() { return events_; }
    void DestroyMembers();
    LegacyConfirmationServices& services_;
    YtlReferenceSlotRuntime background_{};
    GuiClickedEventReferences events_{};
    std::int32_t mode_;
    std::int32_t result_;
    std::int32_t phase_;
    std::int32_t actionFive_;
    bool resultWritten_ = false;
    bool begun_ = false;
    bool released_ = false;
};

class LegacyConfirmationSystem final : public LegacyConfirmation
{
public:
    LegacyConfirmationSystem(const LegacyConfirmationData&, LegacyConfirmationServices&);
    void Begin() override;
    bool Poll() override;
    bool End() override;
    std::int32_t Result() override; // 4C9660原基类恒-1，实际命令结果是本类内部状态。
    bool CanOpen();
private:
    void ProcessInput() override;
    std::int32_t ReadInteractionState() override { return interactionEnabled_; }
    void WriteInteractionState(std::int32_t v) override { interactionEnabled_ = v; }
    bool UsesActionFive() const noexcept override { return false; }
    const LegacyConfirmationData& data_;
    std::int32_t interactionEnabled_ = 1;
    bool suppressOnce_ = false;
};

class LegacyConfirmationSaveLoad final : public LegacyConfirmation
{
public:
    LegacyConfirmationSaveLoad(const LegacyConfirmationData&, LegacyConfirmationServices&);
    ~LegacyConfirmationSaveLoad() override;
    void SetMode(std::int32_t); // 4FFF30，4DC1B0 kind2先写mode再Begin。
    void Begin() override;
    bool Poll() override;
    bool End() override;
    void ReleaseResources() override;
    void ConfigureBorrowed(const RuntimeCString& page, const RuntimeCString& slot,
        const RuntimeCString& timestamp, const RuntimeCString& scene, std::uint8_t saveMode);
    void ConfigureOwned(RuntimeCString& page, RuntimeCString& slot,
        RuntimeCString& timestamp, RuntimeCString& scene, std::uint8_t saveMode);
    void AssignPreview(YtlReferenceSlotRuntime& owned);
    const YtlReferenceSlotRuntime& Labels() const noexcept { return labels_; }
private:
    void DrawExtras(const RECT*) override;
    GuiClickedEventReferences& Events() override;
    void DrawText(const char*, POINT, std::int32_t height, std::uint8_t spacing, COLORREF);
    const LegacyConfirmationData& data_;
    YtlReferenceSlotRuntime manager_{};
    YtlReferenceSlotRuntime labels_{};
    YtlReferenceSlotRuntime preview_{};
    bool configuredMode_ = false;
    bool extraReleased_ = false;
};

// 4DBFE0：九个MessageDialog、一个SystemDialog、一个SaveLoadConfirmDialog。
// root只持一份此owner；quickload和帧策略同时借Modal()，不得再造modal镜像。
class LegacyConfirmationOwner final
{
public:
    LegacyConfirmationOwner(const LegacyConfirmationData&, LegacyConfirmationServices&);
    ~LegacyConfirmationOwner() noexcept;
    LegacyConfirmationOwner(const LegacyConfirmationOwner&) = delete;
    LegacyConfirmationOwner& operator=(const LegacyConfirmationOwner&) = delete;
    LegacyTitleQuickLoadModalState& Modal() noexcept { return modal_; }
    ILegacyTitleQuickLoadMessage* FindMessage(const LegacyTitleQuickLoadModalState&);
    bool Begin(std::int32_t kind, std::int32_t mode);
    bool CanOpenSystem();
    LegacyConfirmationSaveLoad& SaveLoad();
    void SuppressSystem(std::int32_t value) noexcept { suppressSystem_ = value; }
    void ReleaseResources();
private:
    LegacyTitleQuickLoadModalState modal_;
    GuiControlChildrenStorage messages_{};
    std::int32_t suppressSystem_ = 0;
};

class RootRuntimeOwner;
class NativeGraphicsRuntimeLibrary;
// 真正截图叶：4C9190先临时Surface/Ref，再size/Create/Backup，最后复制返回槽。
void CaptureLegacyConfirmationScreen(YtlReferenceSlotRuntime& fresh, IGraphicsSurfaceApi&,
    IGraphicsScreenSizeApi&, IGraphicsSurfaceTransitionApi&,
    IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource());

// 真实生产根/输入/图形适配；不创建owner，不缓存root payload，不借现代modal gate。
class LegacyConfirmationRootSource final : private INativeScreenCaptureServices,
    private IGuiDialogNativeScreenSource
{
public:
    LegacyConfirmationRootSource(RootRuntimeOwner&, NativeGraphicsRuntimeLibrary&,
        const Platform::IInputRuntime&, IGuiControlResourceLoadServices&, IQuickOperationsNativeSource&,
        IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource()) noexcept;
    LegacyConfirmationServices& Services() noexcept { return services_; }
    void BindSystem(ISystemDialogNativeSource& system) noexcept { services_.system = &system; }
private:
    GraphicsSurfaceReference CaptureScreenNativeAt4ACDD0() override;
    void InvalidateRectAt52603A(const RECT*) override;
    RootRuntimeOwner& root_;
    NativeGraphicsRuntimeLibrary& graphics_;
    GuiModalInputSource input_;
    LegacyConfirmationServices services_;
};

// 44CB3A..44CBB2独占忙帧。只调用两个现成叶，不运行同步模态循环。
// true即本帧已消费，即便Tick刚清busy，也不得继续标题/场景。
bool TryTickLegacyConfirmationFrame(ILegacyTitleQuickLoadSource&, IModalDialogRunnerServices&);
}

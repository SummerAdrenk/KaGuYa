#pragma once

#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/ModalDialogRunnerRuntime.h>
#include <Runtime/EditWindowNativeLifetime.h>
#include <optional>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 原4E43A9真实类型：GuiControl::GuiModalNameRegist::NameRegistDialog。
// union只建立存储，不能提前清原构造未写的+68..73、+84..93。
struct NameRegistrationDialogNativeBody94h final
{
    union { GuiDialogNativeBody5Ch common; };
    union { YtlReferenceSlotRuntime backgroundAt5C; };
    std::int32_t resultAt68;
    std::int32_t phaseAt6C;
    std::int32_t keyboardLatchAt70;
    union { GuiClickedEventReferences eventsAt74; };
    std::int32_t radioEnabledAt80;
    std::int32_t cancelAt84;
    std::int32_t outsideAt88;
    void* editsAt8C[2]; // 原98h CEditWindow身份，不是HWND或现代owner。
    NameRegistrationDialogNativeBody94h() noexcept {}
};
static_assert(sizeof(NameRegistrationDialogNativeBody94h) == 0x94);
static_assert(alignof(NameRegistrationDialogNativeBody94h) == 4);
static_assert(std::is_standard_layout_v<NameRegistrationDialogNativeBody94h>);
static_assert(std::is_trivially_destructible_v<NameRegistrationDialogNativeBody94h>);
#define NAME_REGISTRATION_OFFSET(f, o) static_assert(offsetof(NameRegistrationDialogNativeBody94h, f) == o)
NAME_REGISTRATION_OFFSET(common, 0x00);
NAME_REGISTRATION_OFFSET(backgroundAt5C, 0x5C);
NAME_REGISTRATION_OFFSET(resultAt68, 0x68);
NAME_REGISTRATION_OFFSET(phaseAt6C, 0x6C);
NAME_REGISTRATION_OFFSET(keyboardLatchAt70, 0x70);
NAME_REGISTRATION_OFFSET(eventsAt74, 0x74);
NAME_REGISTRATION_OFFSET(radioEnabledAt80, 0x80);
NAME_REGISTRATION_OFFSET(cancelAt84, 0x84);
NAME_REGISTRATION_OFFSET(outsideAt88, 0x88);
NAME_REGISTRATION_OFFSET(editsAt8C, 0x8C);
#undef NAME_REGISTRATION_OFFSET

// PE实读5C084C..5C08E0共38槽；5C08E4是下一类的RTTI locator。
struct NameRegistrationDialogNativeVtable38 final
{
    GuiDialogNativeVtable35 common;
    void (__thiscall* beginExtraAt8C)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* pollExtraAt90)(GuiDialogNativeBody5Ch*);
    void (__thiscall* endExtraAt94)(GuiDialogNativeBody5Ch*);
};
static_assert(sizeof(NameRegistrationDialogNativeVtable38) == 0x98);
static_assert(offsetof(NameRegistrationDialogNativeVtable38, beginExtraAt8C) == 0x8C);
static_assert(offsetof(NameRegistrationDialogNativeVtable38, pollExtraAt90) == 0x90);
static_assert(offsetof(NameRegistrationDialogNativeVtable38, endExtraAt94) == 0x94);

// 只叶Source。Root身份opaque，getter只借用所传固定receiver：不得Ensure、
// 重捕获、复制/增持Ref、构造21Ch镜像或把RootRuntimePayload当原偏移正文。
// 其余明确标native的正文必须是真实布局，不能强转现代对象充数。
class INameRegistrationDialogNativeSource
{
public:
    virtual ~INameRegistrationDialogNativeSource() = default;
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void* fixedRoot) = 0;
    virtual std::int32_t& RootModalFlagAt1B8(void* fixedRoot) = 0;
    virtual YtlReferenceSlotRuntime* CaptureBackgroundAt4ACDD0(void* nativeGraphics,
        YtlReferenceSlotRuntime* freshOutput) = 0;
    // 缺少的Panel具体叶显式外供：504E90的原6Ch/26槽及失败资格，绝不modern fallback。
    virtual YtlReferenceSlotRuntime* CreatePanelAt504E90(YtlReferenceSlotRuntime* freshOutput,
        GuiDialogNativeBody5Ch* parent, const GuiStaticDescriptorNative24h* descriptor) = 0;
    virtual void* NewEditAt46FA30(void* fixedRoot, std::int32_t disable,
        std::int32_t alignment, HWND borderArgument) = 0; // 此项只决定border，实际parent来自Env+2C。
    virtual YtlReferenceSlotRuntime* FindEditAt479DF0(void* fixedRoot,
        YtlReferenceSlotRuntime* freshOutput, void* nativeEditIdentity) = 0;
    virtual void ApplyEditAt4875F0(void* fixedRoot, void* nativeEditIdentity) = 0;
    virtual void RemoveEditAt486B60(void* fixedRoot, void* nativeEditIdentity) = 0;
    // 50B000：在未构造TEXT槽返回字符串；清理资格由调用点EH决定。
    virtual RuntimeCString* ReadEditTextAt50B000(void* nativeEdit98h, RuntimeCString* freshOutput) = 0;
    virtual void* ParamsAt418480() = 0;
    virtual YtlReferenceSlotRuntime& ParamsGameSystemSlotAt04(void* fixedParams) = 0;
    virtual std::uint8_t GameSystemFlagsAt2C(void* nativeGameSystem) = 0;
    virtual const char* GameSystemDefaultNameAt30(void* nativeGameSystem) = 0;
    virtual const char* GameSystemDefaultNameAt34(void* nativeGameSystem) = 0;
    virtual void* SystemAt4180C0() = 0;
    virtual void* RegistryAt417990(void* fixedSystem, std::int32_t index) = 0;
    virtual RuntimeCString& RegistryNameAt5C(void* fixedRegistry1) = 0;
    virtual RuntimeCString& RegistryNameAt58(void* fixedRegistry1) = 0;
    virtual std::uint8_t& RegistryNameChangedAt54(void* fixedRegistry1) = 0;
    virtual HWND SystemWindowAt2C(void* fixedSystem) = 0;
    virtual HWND MainWindowAt6198C8() = 0; // 含原60h singleton/critical叶，不提前获取。
    virtual InputRootNativeBody24h* InputAt418210() = 0;
    virtual BOOL IsWindowEnabled(HWND) = 0;
    virtual HWND SetFocus(HWND) = 0;
    virtual BOOL ShowWindow(HWND, int command) = 0;
    virtual BOOL SetWindowTextA(HWND, const char*) = 0;
    virtual LRESULT SendMessageA(HWND, UINT, WPARAM, LPARAM) = 0;
    virtual BOOL DeleteObject(HGDIOBJ) = 0;
    virtual HBRUSH CreateSolidBrush(COLORREF) = 0;
    // 非原业务元数据：仅声明同一套Edit服务，实际生产Source必须提供。
    virtual EditWindowNativeMapServices* NativeEditMaps() const noexcept{return nullptr;}
    virtual WindowThunkNativeServices* NativeEditThunks() const noexcept{return nullptr;}
};

struct NameRegistrationDialogNativeServices final
{
    INameRegistrationDialogNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
// 必须覆盖raw ctor/EH、全部虚调用、delete以及外层Ref最后减持；服务不进入94h。
class NameRegistrationDialogNativeScope final
{
public:
    explicit NameRegistrationDialogNativeScope(NameRegistrationDialogNativeServices&) noexcept;
    ~NameRegistrationDialogNativeScope() noexcept;
    NameRegistrationDialogNativeScope(const NameRegistrationDialogNativeScope&) = delete;
    NameRegistrationDialogNativeScope& operator=(const NameRegistrationDialogNativeScope&) = delete;
private:
    NameRegistrationDialogNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
    std::optional<EditWindowNativeScope> edits_;
};
NameRegistrationDialogNativeServices& RequireNameRegistrationDialogNativeServices();
const NameRegistrationDialogNativeVtable38& NameRegistrationDialogNativeVtable() noexcept;
NameRegistrationDialogNativeBody94h* ConstructNameRegistrationDialogNativeAt4E4340(void* storage);
NameRegistrationDialogNativeBody94h* ConstructNameRegistrationDialogNativeAt4E4340(
    void* storage, NameRegistrationDialogNativeServices& services);
void DestroyNameRegistrationDialogNativeAt4F80D0(NameRegistrationDialogNativeBody94h*) noexcept;
NameRegistrationDialogNativeBody94h* DeleteNameRegistrationDialogNativeAt4FCDE0(
    NameRegistrationDialogNativeBody94h*, std::uint32_t flags) noexcept;
// 复用原生目标解析与既有Modal算法。unusedThis不消费；唯一参数是借用Ref地址。
std::int32_t RunNameRegistrationDialogNativeAt5071D0(void* unusedThis,
    const YtlReferenceSlotRuntime* target, IModalDialogRunnerServices& modal);

std::span<const GuiStaticDescriptorNative24h, 4> NameRegistrationDialogNativeStaticsSample18() noexcept;
const GuiStaticDescriptorNative24h& NameRegistrationDialogNativePanelSample18() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 2> NameRegistrationDialogNativeRadiosSample18() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 3> NameRegistrationDialogNativeButtonsSample18() noexcept;
const GuiDialogNavigationMapping* NameRegistrationDialogNativeRadioNavigationSample18() noexcept;
std::span<const RECT, 2> NameRegistrationDialogNativeEditRectanglesSample18() noexcept;
std::span<const std::uint32_t, 38> NameRegistrationDialogNativeOriginalVtableSample18() noexcept;
}

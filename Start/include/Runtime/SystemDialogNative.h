#pragma once

#include <Runtime/GuiControlFactoryNative.h>
#include <Runtime/GuiDialogNative.h>
#include <Runtime/WindowModeNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
// 原4F41A0：只建立存储，不提前清构造未写的68..73。无隐藏业务owner。
struct SystemDialogNativeBody88h final
{
    union { GuiDialogNativeBody5Ch common; };
    union { YtlReferenceSlotRuntime backgroundAt5C; };
    std::int32_t resultAt68;
    std::int32_t phaseAt6C;
    std::int32_t keyboardLatchAt70;
    union { GuiClickedEventReferences eventsAt74; };
    std::int32_t fieldAt80;
    std::int32_t suppressOnceAt84;
    SystemDialogNativeBody88h() noexcept {}
};
static_assert(sizeof(SystemDialogNativeBody88h) == 0x88);
static_assert(alignof(SystemDialogNativeBody88h) == 4);
static_assert(std::is_standard_layout_v<SystemDialogNativeBody88h>);
static_assert(std::is_trivially_destructible_v<SystemDialogNativeBody88h>);
#define SYSTEM_DIALOG_OFFSET(f, o) static_assert(offsetof(SystemDialogNativeBody88h, f) == o)
SYSTEM_DIALOG_OFFSET(common, 0x00);
SYSTEM_DIALOG_OFFSET(backgroundAt5C, 0x5C);
SYSTEM_DIALOG_OFFSET(resultAt68, 0x68);
SYSTEM_DIALOG_OFFSET(phaseAt6C, 0x6C);
SYSTEM_DIALOG_OFFSET(keyboardLatchAt70, 0x70);
SYSTEM_DIALOG_OFFSET(eventsAt74, 0x74);
SYSTEM_DIALOG_OFFSET(fieldAt80, 0x80);
SYSTEM_DIALOG_OFFSET(suppressOnceAt84, 0x84);
#undef SYSTEM_DIALOG_OFFSET

// PE 5BE868..5BE8FC；5BE900为下一类RTTI，不是第39槽。
struct SystemDialogNativeVtable38 final
{
    GuiDialogNativeVtable35 common;
    void (__thiscall* beginExtraAt8C)(GuiDialogNativeBody5Ch*);
    std::int32_t (__thiscall* pollExtraAt90)(GuiDialogNativeBody5Ch*);
    void (__thiscall* endExtraAt94)(GuiDialogNativeBody5Ch*);
};
static_assert(sizeof(SystemDialogNativeVtable38) == 0x98);
static_assert(offsetof(SystemDialogNativeVtable38, beginExtraAt8C) == 0x8C);
static_assert(offsetof(SystemDialogNativeVtable38, pollExtraAt90) == 0x90);
static_assert(offsetof(SystemDialogNativeVtable38, endExtraAt94) == 0x94);

// 明确的源leaf合同，不代表生产Root接线已经完成。Root身份始终opaque。
// 每个accessor只借所传固定receiver，不Ensure、不重新选Root、不增持/复制Ref。
// Ref目标必须是真实native正文，禁止RootRuntimePayload raw+offset或modern镜像。
class ISystemDialogNativeSource
{
public:
    virtual ~ISystemDialogNativeSource() = default;
    virtual void EnsureRootAt413BE0() = 0;
    virtual void* RootAt6198C4() = 0;
    virtual YtlReferenceSlotRuntime& RootWindowModeSlotAt0C(void* fixedRoot) = 0;
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void* fixedRoot) = 0;
    virtual YtlReferenceSlotRuntime& RootSceneManagerSlotAt30(void* fixedRoot) = 0;
    virtual std::int32_t& RootModalFlagAt1B8(void* fixedRoot) = 0;
    virtual std::uint8_t RootMenuStateAt219(void* fixedRoot) = 0;
    // 原478E50独立服务：fresh RECT，必须保持内部singleton/Win32/算术/EH顺序。
    // 本leaf不得因为传入Root而删去其内部原有413BE0；输出前缀不回滚。
    virtual RECT* LogicalRectangleAt478E50(void* fixedRoot, RECT* freshOutput) = 0;
    // 原4ACDD0返回fresh Ref12；调用者仅在成功返回之后获得临时Ref清理资格。
    virtual YtlReferenceSlotRuntime* CaptureBackgroundAt4ACDD0(void* nativeDraw,
        YtlReferenceSlotRuntime* freshOutput) = 0;
    virtual void* SystemAt4180C0() = 0;
    virtual HWND SystemWindowAt2C(void* fixedSystem) = 0;
    // 52687F..526928：caller已固定非零handle，此叶独立捕获Graphics一次并按base门控。
    // 空模块跳过；非空保留CString/GetProcAddress/Free/DLL原顺序，不先ModuleLoaded再捕获。
    virtual void DrawBackgroundAt52687F(std::int32_t handle, const RECT* clip) = 0;
    virtual BOOL IsIconic(HWND window) { return ::IsIconic(window); }
    virtual BOOL PostMessageA(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
    { return ::PostMessageA(window, message, wparam, lparam); }
};
struct SystemDialogNativeServices final
{
    ISystemDialogNativeSource& source;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
};
// 外部借用scope须覆盖父14h构造/EH、所有虚调用以及父/child最后一份Ref的释放。
// 不提供只包ctor的临时scope重载，避免返回悬空服务合同。
class SystemDialogNativeScope final
{
public:
    explicit SystemDialogNativeScope(SystemDialogNativeServices&) noexcept;
    // nullptr明确禁用外层残留服务，不能从另一代根的TLS借图形/输入来掩盖缺失。
    explicit SystemDialogNativeScope(SystemDialogNativeServices*) noexcept;
    ~SystemDialogNativeScope() noexcept;
    SystemDialogNativeScope(const SystemDialogNativeScope&) = delete;
    SystemDialogNativeScope& operator=(const SystemDialogNativeScope&) = delete;
private:
    SystemDialogNativeServices* previous_;
    GraphicsSurfaceNativeScope surfaces_;
    GuiControlGraphicsScope graphics_;
    GuiInputNativeScope input_;
    GuiDialogNativeScope screen_;
};
SystemDialogNativeServices& RequireSystemDialogNativeServices();
const SystemDialogNativeVtable38& SystemDialogNativeVtable() noexcept;
const SystemDialogNativeVtable38& SystemDialogNativeModalVtable() noexcept;
SystemDialogNativeBody88h* ConstructSystemDialogNativeAt4F41A0(void* storage88h);
// 供ISystemInformationDisplayNativeSource的同名成员直接转交，返回原5Ch首成员。
GuiDialogNativeBody5Ch* ConstructSystemDialogAt4F41A0(void* storage88h);
std::int32_t TriggerSystemDialogAt5104A0(GuiDialogNativeBody5Ch*);
void DestroySystemDialogNativeAt4F80D0(SystemDialogNativeBody88h*) noexcept;
SystemDialogNativeBody88h* DeleteSystemDialogNativeAt4FE5F0(
    SystemDialogNativeBody88h*, std::uint32_t flags) noexcept;

const GuiStaticDescriptorNative24h& SystemDialogNativeStaticSample18() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 4> SystemDialogNativeButtonsSample18() noexcept;
std::span<const std::uint32_t, 38> SystemDialogNativeOriginalVtableSample18() noexcept;
std::span<const std::uint32_t, 38> SystemDialogNativeOriginalModalVtableSample18() noexcept;
std::span<const std::uint32_t, 9> SystemDialogNativeRawStaticSample18() noexcept;
std::span<const std::array<std::uint32_t, 11>, 4> SystemDialogNativeRawButtonsSample18() noexcept;
std::span<const std::uint32_t, 7> SystemDialogNativeOriginalInputVtableSample18() noexcept;
std::span<const char, 8> SystemDialogNativeSettingsCommandSample18() noexcept;
}

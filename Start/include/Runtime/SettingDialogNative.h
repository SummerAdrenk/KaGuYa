#pragma once
#include <Runtime/SettingSystemPageNative.h>
#include <Runtime/GuiDialogIntoNative.h>
#include <Runtime/SettingDialogNativeConstruction.h>
#include <Runtime/InputRootNative.h>
#include <Runtime/RuntimeCStringVector.h>

namespace KaGuYa::Start::Runtime
{
struct SettingDialogNativeBodyACh final
{
    union { GuiDialogNativeBody5Ch common; };
    GuiDialogChildrenStorage dialogsAt5C;
    union { YtlReferenceSlotRuntime backgroundAt68; };
    std::int32_t phaseAt74;
    std::int32_t pageAt78;
    union { RuntimeCString firstNameAt7C; };
    union { RuntimeCString secondNameAt80; };
    EnvironmentVectorStorage<RuntimeCString> snapshotNamesAt84;
    std::uint32_t savedAt90;
    std::uint32_t savedAt94;
    std::uint32_t unwrittenAt98;
    std::uint32_t unwrittenAt9C;
    std::uint8_t savedBytesAtA0[4];
    std::int32_t fieldAtA4;
    std::int32_t confirmationAtA8;
    SettingDialogNativeBodyACh() noexcept {}
    ~SettingDialogNativeBodyACh() noexcept {} // CString/Ref只由原阶段入口销毁，不能自动再减持。
};
static_assert(sizeof(SettingDialogNativeBodyACh) == 0xAC);
static_assert(alignof(SettingDialogNativeBodyACh) == 4);
static_assert(std::is_standard_layout_v<SettingDialogNativeBodyACh>);
#define SETTING_DIALOG_OFFSET(field, offset) static_assert(offsetof(SettingDialogNativeBodyACh, field) == offset)
SETTING_DIALOG_OFFSET(common, 0);
SETTING_DIALOG_OFFSET(dialogsAt5C, 0x5C);
SETTING_DIALOG_OFFSET(backgroundAt68, 0x68);
SETTING_DIALOG_OFFSET(phaseAt74, 0x74);
SETTING_DIALOG_OFFSET(pageAt78, 0x78);
SETTING_DIALOG_OFFSET(firstNameAt7C, 0x7C);
SETTING_DIALOG_OFFSET(secondNameAt80, 0x80);
SETTING_DIALOG_OFFSET(snapshotNamesAt84, 0x84);
SETTING_DIALOG_OFFSET(savedAt90, 0x90);
SETTING_DIALOG_OFFSET(unwrittenAt98, 0x98);
SETTING_DIALOG_OFFSET(savedBytesAtA0, 0xA0);
SETTING_DIALOG_OFFSET(fieldAtA4, 0xA4);
SETTING_DIALOG_OFFSET(confirmationAtA8, 0xA8);
#undef SETTING_DIALOG_OFFSET

// 四个动态页由SettingDialogNativePages提供；原尺寸、parent和id仍显式传递。
class ISettingDialogNativePageConstruction
{
public:
    virtual ~ISettingDialogNativePageConstruction() = default;
    virtual GuiDialogNativeBody5Ch* ConstructPage01At4F1EE0(void* storage84h, GuiDialogNativeBody5Ch*, std::int32_t id) = 0;
    virtual GuiDialogNativeBody5Ch* ConstructPage02At4F23B0(void* storage84h, GuiDialogNativeBody5Ch*, std::int32_t id) = 0;
    virtual GuiDialogNativeBody5Ch* ConstructPage03At4F2910(void* storage8Ch, GuiDialogNativeBody5Ch*, std::int32_t id) = 0;
    virtual GuiDialogNativeBody5Ch* ConstructPage04At4F2FE0(void* storage88h, GuiDialogNativeBody5Ch*, std::int32_t id) = 0;
};
// 由SettingDialogNativeBusiness提供真实原核分派，不接受成功stub/通用modern桥。
// 51CFC0含配置/声音/文字/效果全链；三条Poll额外流程的确认对象及EH亦是独立依赖。
class ISettingDialogNativeRemainingBusiness
{
public:
    virtual ~ISettingDialogNativeRemainingBusiness() = default;
    virtual void ApplySettingsAt51CFC0(SettingDialogNativeBodyACh&) = 0;
    virtual std::int32_t PollModeConfirmationAt51E4B0() = 0;
    virtual std::int32_t PollDefaultsConfirmationAt51B730(SettingDialogNativeBodyACh&) = 0;
    virtual std::int32_t PollNameConfirmationAt521AD0(SettingDialogNativeBodyACh&) = 0;
};
class ISettingDialogNativeSource : public ISettingSystemPageNativeSource
{
public:
    virtual YtlReferenceSlotRuntime& RootDrawSlotAt18(void* fixedRoot) = 0;
    virtual YtlReferenceSlotRuntime& RootSoundSlotAt24(void* fixedRoot) = 0;
    virtual std::int32_t& RootPageAt214(void* fixedRoot) = 0;
    virtual EnvironmentRootStorage* SystemAt4180C0() = 0;
    virtual InputRootNativeBody24h* InputAt418210() = 0;
    virtual YtlReferenceSlotRuntime* CaptureBackgroundAt4ACDD0(void* nativeDraw, YtlReferenceSlotRuntime* fresh) = 0;
    virtual void StopSoundAt4384D0(void* nativeSound) = 0;
    virtual void StopSoundMappingsAt413270(void* nativeSound, EnvironmentVectorStorage<RuntimeCString>* fixedMappings30) = 0;
    virtual void PrepareSoundAt433F60(void* nativeSound) = 0;
    // 5272BF..368：handle已固定，此叶再捕获Graphics一次；空模块跳过，不预查再重捕获。
    virtual void DrawBackgroundAt5272BF(std::int32_t handle, const RECT* clip) = 0;
    virtual BOOL PostMessageA(HWND, UINT, WPARAM, LPARAM) = 0;
};
struct SettingDialogNativeServices final
{
    ISettingDialogNativeSource& source;
    ISettingDialogNativePageConstruction& pages;
    ISettingDialogNativeRemainingBusiness& remaining;
    IGuiControlResourceLoadServices& loader;
    IGraphicsSurfaceApi& surfaces;
    IGuiControlGraphicsSource& graphics;
    IGuiInputNativeSource& input;
    IGuiDialogNativeScreenSource& screen;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
    SettingSystemPageNativeServices system{source, loader, surfaces, graphics, input, screen, strings};
};
// 长寿scope须覆盖父/子构造、业务、EH和最后逃逸Ref。没有body身份map或隐藏owner。
class SettingDialogNativeScope final
{
public:
    explicit SettingDialogNativeScope(SettingDialogNativeServices&) noexcept;
    explicit SettingDialogNativeScope(SettingDialogNativeServices*) noexcept;
    ~SettingDialogNativeScope() noexcept;
    SettingDialogNativeScope(const SettingDialogNativeScope&) = delete;
    SettingDialogNativeScope& operator=(const SettingDialogNativeScope&) = delete;
private:
    SettingDialogNativeServices* previous_;
    SettingSystemPageNativeScope system_;
};
SettingDialogNativeServices& RequireSettingDialogNativeServices();
// ABI37槽：31项继承同一原目标，六项本地定序。非空不代表remaining/pages正文已完成。
const GuiDialogIntoNativeVtable37& SettingDialogNativeVtable() noexcept;
SettingDialogNativeBodyACh* ConstructSettingDialogNativeAt4F1BF0(void* storageACh);
GuiDialogNativeBody5Ch* ConstructSettingDialogAt4F1BF0(void* storageACh);
void DestroySettingDialogNativeAt4F9720(SettingDialogNativeBodyACh*) noexcept;
SettingDialogNativeBodyACh* DeleteSettingDialogNativeAt4FE220(SettingDialogNativeBodyACh*, std::uint32_t flags) noexcept;
void SnapshotSettingDialogNativeAt4FF430(SettingDialogNativeBodyACh&);
void SelectSettingDialogNativePageAt5352F0(SettingDialogNativeBodyACh&, std::int32_t selection);
// 可直接注入已冻结Scene20的构造口；不安装临时GUIscope，不接受ISettingDialog。
class SettingDialogNativeConstruction final : public ISettingDialogNativeConstruction
{
public:
    explicit SettingDialogNativeConstruction(SettingDialogNativeRuntimeServices* runtime=nullptr) noexcept:runtime_(runtime){}
    GuiDialogNativeBody5Ch* ConstructSettingDialogAt4F1BF0(void* raw) override
    { return KaGuYa::Start::Runtime::ConstructSettingDialogAt4F1BF0(raw); }
    SettingDialogNativeRuntimeServices* NativeRuntimeServices() const noexcept override{return runtime_;}
private:
    SettingDialogNativeRuntimeServices* runtime_;
};
std::span<const std::uint32_t, 37> SettingDialogNativeOriginalVtableSample18() noexcept;
std::span<const std::array<std::uint32_t, 2>, 8> SettingDialogNativeUnwindSample18() noexcept;
}

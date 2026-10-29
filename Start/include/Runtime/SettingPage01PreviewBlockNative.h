#pragma once
#include <Runtime/SettingPage01Native.h>
#include <Runtime/ColorTransWindowNative.h>
#include <Runtime/GraphicsTextNativeApi.h>
#include <Runtime/GuiSliderNative.h>
#include <Runtime/ScriptCallStacksNative.h>

namespace KaGuYa::Start::Runtime
{
class NativeGraphicsRuntimeLibrary;
// 4E5510：80h基类之后恰好两个vector、RECT和Surface Ref；不放现代服务指针。
struct SettingPage01PreviewBlockNativeBodyB4h final
{
    union { SettingPageChoiceNativeBody80h choice; };
    union { ScriptDwordVectorStorage transparencyAt80; };
    GuiControlChildrenStorage previewsAt8C;
    RECT previewBoundsAt98;
    union { YtlReferenceSlotRuntime textAtA8; };
    SettingPage01PreviewBlockNativeBodyB4h() noexcept {}
    ~SettingPage01PreviewBlockNativeBodyB4h() noexcept {} // 原释放口显式销毁各成员。
};
static_assert(sizeof(SettingPage01PreviewBlockNativeBodyB4h) == 0xB4);
static_assert(alignof(SettingPage01PreviewBlockNativeBodyB4h) == 4);
static_assert(std::is_standard_layout_v<SettingPage01PreviewBlockNativeBodyB4h>);
static_assert(offsetof(SettingPage01PreviewBlockNativeBodyB4h, transparencyAt80) == 0x80);
static_assert(offsetof(SettingPage01PreviewBlockNativeBodyB4h, previewsAt8C) == 0x8C);
static_assert(offsetof(SettingPage01PreviewBlockNativeBodyB4h, previewBoundsAt98) == 0x98);
static_assert(offsetof(SettingPage01PreviewBlockNativeBodyB4h, textAtA8) == 0xA8);

// 61A644/648是运行期指针槽，不是PE内的固定字符串；第二槽指向原CString字符正文。
class ISettingPage01PreviewBlockNativeGlobals
{
public:
    virtual ~ISettingPage01PreviewBlockNativeGlobals() = default;
    virtual const char* FontFaceAt61A644() = 0;
    virtual const char* SampleCharactersAt61A648() = 0;
};
class SettingPage01PreviewBlockNativeGlobals final : public ISettingPage01PreviewBlockNativeGlobals
{
public:
    SettingPage01PreviewBlockNativeGlobals(const char* const* faceCell,
        const char* const* sampleCell) noexcept : face_(faceCell), sample_(sampleCell) {}
    const char* FontFaceAt61A644() override;
    const char* SampleCharactersAt61A648() override;
private:
    const char* const* face_;
    const char* const* sample_;
};
// 只隔离Graphics原调用叶，参数保持58h原正文；不转换成现代字体参数。
class ISettingPage01PreviewBlockNativeText
{
public:
    virtual ~ISettingPage01PreviewBlockNativeText() = default;
    virtual bool CreateFontAt4388B0(const GraphicsTextNativeFontParameters*) = 0;
    virtual bool InitializeAt4389D0(const char*, std::int32_t, std::int32_t) = 0;
    virtual bool BlitAllAt53A980(std::int32_t, RECT*) = 0;
};
class SettingPage01PreviewBlockNativeText final : public ISettingPage01PreviewBlockNativeText
{
public:
    explicit SettingPage01PreviewBlockNativeText(NativeGraphicsRuntimeLibrary& graphics) noexcept
        : graphics_(graphics) {}
    bool CreateFontAt4388B0(const GraphicsTextNativeFontParameters*) override;
    bool InitializeAt4389D0(const char*, std::int32_t, std::int32_t) override;
    bool BlitAllAt53A980(std::int32_t, RECT*) override;
private:
    NativeGraphicsRuntimeLibrary& graphics_;
};
struct SettingPage01PreviewBlockNativeServices final
{
    ColorTransWindowNativeServices& colors;
    ISettingPage01PreviewBlockNativeText& text;
    ISettingPage01PreviewBlockNativeGlobals& globals;
};
// 同时需要既有SettingPage02NativeScope；两个scope须覆盖所有逃逸Ref的最后释放。
class SettingPage01PreviewBlockNativeScope final
{
public:
    explicit SettingPage01PreviewBlockNativeScope(SettingPage01PreviewBlockNativeServices&) noexcept;
    ~SettingPage01PreviewBlockNativeScope() noexcept;
    SettingPage01PreviewBlockNativeScope(const SettingPage01PreviewBlockNativeScope&) = delete;
    SettingPage01PreviewBlockNativeScope& operator=(const SettingPage01PreviewBlockNativeScope&) = delete;
private:
    SettingPage01PreviewBlockNativeServices* previous_;
};
SettingPage01PreviewBlockNativeServices& RequireSettingPage01PreviewBlockNativeServices();
const SettingPageChoiceNativeVtable38& SettingPage01PreviewBlockNativeVtable() noexcept;
SettingPage01PreviewBlockNativeBodyB4h* ConstructSettingPage01PreviewBlockNativeAt4E5510(
    void* storage, GuiDialogNativeBody5Ch* parent, std::int32_t id);
void DestroySettingPage01PreviewBlockNativeAt4FCFD0(SettingPage01PreviewBlockNativeBodyB4h*) noexcept;
SettingPage01PreviewBlockNativeBodyB4h* DeleteSettingPage01PreviewBlockNativeAt4FCFD0(
    SettingPage01PreviewBlockNativeBodyB4h*, std::uint32_t flags) noexcept;
std::int32_t ReadSettingPage01PreviewSelectionNativeAt535E60(GuiDialogNativeBody5Ch&);
void WriteSettingPage01PreviewSelectionNativeAt509940(GuiDialogNativeBody5Ch&, std::int32_t, std::int32_t);
void SaveSettingPage01PreviewTransparencyNativeAt4FF330(GuiDialogNativeBody5Ch&);
void BuildSettingPage01PreviewTextNativeAt53AA70(SettingPage01PreviewBlockNativeBodyB4h&,
    std::int32_t renderMode, std::int32_t shadow);
class SettingPage01PreviewBlockNativeConstruction final : public ISettingPage01Block02NativeConstruction
{
public:
    GuiDialogNativeBody5Ch* ConstructBlock02At4E5510(void*, GuiDialogNativeBody5Ch*, std::int32_t) override;
};
std::span<const GuiSliderDescriptorNative6Ch, 3> SettingPage01PreviewSlidersSample18() noexcept;
std::span<const GuiButtonDescriptorNative2Ch, 2> SettingPage01PreviewButtonsSample18() noexcept;
std::span<const char* const, 2> SettingPage01PreviewNamesSample18() noexcept;
std::span<const std::uint32_t, 38> SettingPage01PreviewOriginalTableSample18() noexcept;
std::span<const std::uint32_t, 54> SettingPage01PreviewOriginalUnwindSample18() noexcept;
// 407310/4073A0的CString构造段；CRT退出登记由完整进程宿主另行负责。
void ConstructSettingPage01PreviewFontFaceAt407310(RuntimeCString& fresh,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
void ConstructSettingPage01PreviewSampleAt4073A0(RuntimeCString& fresh,
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
}

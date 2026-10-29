#include <Runtime/LegacyConfirmationInternal.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
using namespace LegacyConfirmationDetail;
LegacyConfirmationSaveLoad::LegacyConfirmationSaveLoad(const LegacyConfirmationData& data,
    LegacyConfirmationServices& services) : LegacyConfirmation(services), data_(data)
{
    Scope scope(services_);
    try
    {
        // 4DE0BA先创建真实事件manager，再创建工厂；先静态组再按钮组，每个均有事件。
        NewEventManager(manager_);
        Slot factory{}; CreateFactoryToken(factory); RefLife factoryLife{factory};
        RECT combined{};
        for (unsigned i = 0; i < data_.saveStatics.size() + data_.saveButtons.size(); ++i)
        {
            Slot returned;
            if (i < data_.saveStatics.size()) CreateStatic(returned, &state_.common, data_.saveStatics[i], services_);
            else CreateButton(returned, &state_.common, data_.saveButtons[i - data_.saveStatics.size()], services_);
            RefLife returnedLife{returned};
            // 4FFB00有临时赋值和返回Ref，不以创建一个裸child替代拥有转交。
            Slot child{}; CopyAssignGraphicsSurfaceReferenceAt465870(child, returned);
            RefLife childLife{child}; returnedLife.Finish();
            auto& c = RequireGuiControl(child); RECT scratch;
            const auto* rect = c.vptrAt00->rectAt3C(&c, &scratch);
            ::UnionRect(&combined, &combined, rect);
            AttachEvent(child);
            childLife.Finish();
        }
        NewSurface(labels_, services_.surfaces, services_.strings);
        auto& surface = Surface(labels_);
        const auto width = std::bit_cast<std::int32_t>(std::uint32_t(combined.right) - std::uint32_t(combined.left));
        const auto height = std::bit_cast<std::int32_t>(std::uint32_t(combined.bottom) - std::uint32_t(combined.top));
        if (!surface.CreateAt415010(services_.surfaces, width, height, 1, 1)) throw 1;
        Surface(labels_).originXAt10 = combined.left; Surface(labels_).originYAt14 = combined.top;
        AdoptGuiInputAt536EA0(state_.common.inputAt18, NewGameGuiInputAt4EAAED());
        for (int direction : data_.directions)
            CreateAndAppendGuiNavigation(state_.common.navigationAt24, direction, data_.saveNavigation.data());
        factoryLife.Finish();
    }
    catch (...)
    {
        // 55B200：局部工厂/child先展开，manager/preview/labels/background/GuiDialog随后退役。
        ReleaseResources(); throw;
    }
}
GuiClickedEventReferences& LegacyConfirmationSaveLoad::Events()
{ return *ResolveEnvironmentObjectReference<GuiClickedEventReferences>(manager_); }
void LegacyConfirmationSaveLoad::SetMode(std::int32_t mode) { mode_ = mode; configuredMode_ = true; }
void LegacyConfirmationSaveLoad::Begin()
{
    Scope scope(services_);
    CaptureBackground();
    auto& p = state_.common;
    p.vptrAt00->writeEnabledAt24(&p, 1);
    p.vptrAt00->resetChildrenAt68(&p);
    p.vptrAt00->resetInputAt3C(&p);
    ClearGuiClickedEventFlagsAt532200(Events());
    // 4DE5D0：模式只来自kind2；原未初始化/越界读取改为明确拒绝，不钳制。
    if (!configuredMode_ || std::uint32_t(mode_) >= data_.saveDisabled.size())
        Reject("Baku 56E158: SaveLoadConfirm requires mode 0 or 1");
    for (auto* at = p.childrenAt0C.begin; at != p.childrenAt0C.end; ++at)
    {
        auto& child = RequireGuiControl(*at); child.vptrAt00->writeEnabledAt4C(&child, 1);
        for (int disabled : data_.saveDisabled[mode_])
        {
            if (RequireGuiControl(*at).eventAt04 != disabled) continue;
            auto& selected = RequireGuiControl(*at); selected.vptrAt00->writeEnabledAt4C(&selected, 0); break;
        }
    }
    p.activeAt4C = nullptr; phase_ = 0; begun_ = true;
}
bool LegacyConfirmationSaveLoad::Poll()
{
    Scope scope(services_);
    if (!begun_) Reject("Baku 4DE6B0 requires Begin");
    auto& p = state_.common;
    switch (phase_)
    {
    case 0: AddBackgroundBounds(); phase_ = 3; break;
    case 3:
    {
        p.vptrAt00->processInputAt54(&p);
        // 4FB0A0(1)：首个flag立即清；未知event也阻断后续，本帧不结束。
        const auto event = PollGuiClickedEventManagerNativeAt50FDB0(&Events(), 1);
        if (event == 3 || event == 5) { result_ = 6; resultWritten_ = true; phase_ = 4; }
        else if (event == 4 || event == 6) { result_ = 7; resultWritten_ = true; phase_ = 4; }
        break;
    }
    case 4: AddBackgroundBounds(); p.vptrAt00->writeEnabledAt24(&p, 0); phase_ = -1; break;
    case 1: case 2: Reject("Baku 4DE6B0 unreachable spin phase");
    default: return false;
    }
    p.vptrAt00->flushDirtyAt58(&p); return true;
}
bool LegacyConfirmationSaveLoad::End()
{ Scope scope(services_); ReleaseYtlReference(preview_); return false; } // 4DE690，只清preview，保留结果/背景。
void LegacyConfirmationSaveLoad::DrawExtras(const RECT* clip)
{
    auto& p = state_.common;
    if (!p.vptrAt00->readEnabledAt28(&p)) return;
    if (labels_.baseAt00)
    {
        const auto x = Surface(labels_).originXAt10, y = Surface(labels_).originYAt14;
        const auto handle = Surface(labels_).handleAt04;
        if (handle) static_cast<void>(services_.drawing.SurfaceDrawRaw(handle, x, y, nullptr, nullptr, clip, 0));
    }
    if (preview_.baseAt00)
    {
        // 4DE9E1：有符号算术右移后居中，不使用现代固定坐标615,200。
        const auto width = Surface(preview_).widthAt08, height = Surface(preview_).heightAt0C;
        const auto x = std::bit_cast<std::int32_t>(std::uint32_t(data_.previewCenter.x) - std::uint32_t(width >> 1));
        const auto y = std::bit_cast<std::int32_t>(std::uint32_t(data_.previewCenter.y) - std::uint32_t(height >> 1));
        const auto handle = Surface(preview_).handleAt04;
        if (handle) static_cast<void>(services_.drawing.SurfaceDrawRaw(handle, x, y, nullptr, nullptr, clip, 0));
    }
}
void LegacyConfirmationSaveLoad::AssignPreview(Slot& owned)
{
    Scope scope(services_); RefLife life{owned};
    CopyAssignGraphicsSurfaceReferenceAt465870(preview_, owned); life.Finish();
}
void LegacyConfirmationSaveLoad::DrawText(const char* text, POINT point, int height, std::uint8_t spacing, COLORREF color)
{
    if (!services_.text) Reject("LegacyConfirmationSaveLoad: real Graphics text source not bound");
    // 41C5C0与现代字体初始化逐字段同值；传语义参数，保留原TextCreateFont/Initialize/Blit三次调用。
    GraphicsTextNativeFontParameters native; InitializeGraphicsTextFontNativeAt40C6E0(native);
    native.fontAt00.lfHeight = -height; native.fontAt00.lfWeight = 700;
    ::lstrcpynA(native.fontAt00.lfFaceName, data_.fontFace, 32);
    GraphicsTextFontParameters font;
    font.font = native.fontAt00; font.characterSpacing = 0; font.lineSpacing = spacing;
    font.renderMode = 2; font.textColor = color; font.shadowColor = 0xFFFFFFFF;
    font.outlineColor = 0xFFFFFFFF; font.shadowX = 0; font.shadowY = 0; font.vertical = false; font.outlineWidth = 0;
    try
    {
        if (!services_.text->CreateTextFont(font)) throw 1;
        if (!services_.text->InitializeText(text, point.x, point.y)) throw 1;
        if (!services_.text->BlitAllTextToSurface(Surface(labels_).handleAt04, nullptr)) throw 1;
    }
    catch (...) {} // 55B364 -> 4DEF62原catch-all，只覆盖三图形叶；后续文本仍继续。
}
void LegacyConfirmationSaveLoad::ConfigureOwned(RuntimeCString& page, RuntimeCString& slot,
    RuntimeCString& timestamp, RuntimeCString& scene, std::uint8_t saveMode)
{
    Scope scope(services_);
    TextLife a{page}, b{slot}, c{timestamp}, d{scene};
    const auto handle = Surface(labels_).handleAt04;
    if (handle) services_.drawing.SurfaceClearRaw(handle, 0, nullptr);
    const auto color = data_.labelColors[saveMode != 0];
    DrawText(page.GetString(), data_.labelPositions[0], 22, 5, color);
    DrawText(slot.GetString(), data_.labelPositions[1], 22, 5, color);
    DrawText(timestamp.GetString(), data_.labelPositions[2], 22, 5, color);
    DrawText(scene.GetString(), data_.labelPositions[3], 16, 2, color);
    a.Finish(); b.Finish(); c.Finish(); d.Finish(); // 4DEBF0普通正序、EH反序。
}
void LegacyConfirmationSaveLoad::ConfigureBorrowed(const RuntimeCString& page, const RuntimeCString& slot,
    const RuntimeCString& timestamp, const RuntimeCString& scene, std::uint8_t saveMode)
{
    auto clone = [](TextSlot& dst, const RuntimeCString& src)
    {
        auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(src.GetString())) - 1;
        const auto* chars = reinterpret_cast<const char*>(CloneRuntimeCStringDataAt414540(header) + 1);
        std::memcpy(&dst.value, &chars, sizeof(chars));
    };
    // 44FC60调用栈从右到左复制scene/time/slot/page，成功后四个参数一并转交。
    TextSlot a, b, c, d;
    clone(d, scene); TextLife dl{d.value}; clone(c, timestamp); TextLife cl{c.value};
    clone(b, slot); TextLife bl{b.value}; clone(a, page); TextLife al{a.value};
    al.armed = bl.armed = cl.armed = dl.armed = false;
    ConfigureOwned(a.value, b.value, c.value, d.value, saveMode);
}
void LegacyConfirmationSaveLoad::ReleaseResources()
{
    if (extraReleased_) return;
    Scope scope(services_); extraReleased_ = true;
    struct BaseLife final { LegacyConfirmationSaveLoad& self; bool armed = true;
        ~BaseLife() noexcept { if (armed) self.LegacyConfirmation::ReleaseResources(); } } base{*this};
    RefLife labels{labels_}, preview{preview_};
    ReleaseYtlReference(manager_); // 4FFFA0：manager -> preview -> labels -> background -> common。
    preview.Finish(); labels.Finish(); base.armed = false; LegacyConfirmation::ReleaseResources();
}
LegacyConfirmationSaveLoad::~LegacyConfirmationSaveLoad() { ReleaseResources(); }
}

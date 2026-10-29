#include <Runtime/LegacyConfirmation.h>
#include <Runtime/LegacyConfirmationInternal.h>
#include <Logging/Logger.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
using namespace LegacyConfirmationDetail;
namespace
{

void __fastcall ResetLegacyButton(GuiControlNativeBody50h* body, void*)
{
    // 4FCD10 -> 4FC160：只清pressed/focus/hover，不清4FCEF0独立的键盘latch。
    auto& button = *reinterpret_cast<GuiButtonNativeBody5Ch*>(body);
    button.pressedAt50 = 0;
    body->focusAt20 = 0;
    body->hoverAt1C = 0;
}
std::int32_t __fastcall KeyboardLegacyButton(GuiControlNativeBody50h* body, void*)
{
    if (body->childIdAt0C == -1 || !body->vptrAt00->enabledAt1C(body)) return 0;
    auto& button = *reinterpret_cast<GuiButtonNativeBody5Ch*>(body);
    const bool latched = button.keyboardLatchAt58 != 0;
    auto* parent = static_cast<Common*>(body->parentAt4C);
    // 4FCEF0：先判断latch，起始和保持均用旧+68 Held，不是现代起始Pressed。
    const auto held = parent->vptrAt00->heldAt6C(parent, body->childIdAt0C);
    if (!latched)
    {
        if (!held) return 0;
        button.pressedAt50 = 1;
        button.keyboardLatchAt58 = 1;
        return 2;
    }
    if (held) return 2;
    button.pressedAt50 = 0;
    button.keyboardLatchAt58 = 0;
    Slot event;
    CopyGuiClickedEventReference(event, body->eventAt3C);
    RefLife lifetime{event};
    if (event.baseAt00)
    {
        auto* selected = ResolveGuiClickedEvent(event);
        selected->vptrAt00->invokeAt00(selected);
    }
    lifetime.Finish();
    return 1;
}
GuiControlNativeBody50h* __fastcall DeleteLegacyButton(GuiControlNativeBody50h* p, void*, std::uint32_t flags)
{ DestroyControl(*p, sizeof(GuiButtonNativeBody5Ch), flags); return p; }
}
const GuiButtonNativeVtable29& LegacyConfirmationDetail::ButtonTable()
{
    static const auto table = []
    {
        auto t = GuiButtonVtable();
        t.common.deletingDestructorAt00 = reinterpret_cast<decltype(t.common.deletingDestructorAt00)>(&DeleteLegacyButton);
        t.common.resetAt08 = reinterpret_cast<decltype(t.common.resetAt08)>(&ResetLegacyButton);
        t.common.keyboardAt14 = reinterpret_cast<decltype(t.common.keyboardAt14)>(&KeyboardLegacyButton);
        return t;
    }();
    return table;
}

LegacyConfirmation& LegacyConfirmation::From(Common* p) noexcept
{ return *reinterpret_cast<State*>(p)->owner; }

const GuiDialogNativeVtable35& LegacyConfirmation::Table()
{
    static const auto table = []
    {
        auto t = GuiDialogBaseVtable();
        // 旧基类没有priority/childChanged扩展；共享基表这两项是无业务的0/参数退役。
        // 4DC600/4DC960/4DCD90的导航、输入、绘制用统一模型，不复制旧偏移。
#define SLOT(field, function) t.field = reinterpret_cast<decltype(t.field)>(&function)
        SLOT(deletingDestructorAt04, RejectBaseDelete);
        SLOT(activeInputAt38, ActiveInput); SLOT(resetInputAt3C, ResetInput);
        SLOT(beginAt44, BeginEntry); SLOT(pollAt48, PollEntry);
        SLOT(endAt4C, EndEntry); SLOT(resultAt50, ResultEntry); SLOT(flushDirtyAt58, Flush);
        SLOT(processInputAt54, ProcessEntry); SLOT(readStateAt20, ReadStateEntry); SLOT(writeStateAt1C, WriteStateEntry);
#undef SLOT
        return t;
    }();
    return table;
}

Common* __fastcall LegacyConfirmation::RejectBaseDelete(Common*, void*, std::uint32_t)
{ Reject("LegacyConfirmation is owned by its C++/YTL owner, not by a GuiDialog body reference"); }
void __fastcall LegacyConfirmation::BeginEntry(Common* p, void*) { From(p).Begin(); }
std::int32_t __fastcall LegacyConfirmation::PollEntry(Common* p, void*) { return From(p).Poll(); }
std::int32_t __fastcall LegacyConfirmation::EndEntry(Common* p, void*) { return From(p).End(); }
std::int32_t __fastcall LegacyConfirmation::ResultEntry(Common* p, void*) { return From(p).Result(); }
void __fastcall LegacyConfirmation::ProcessEntry(Common* p, void*) { From(p).ProcessInput(); }
std::int32_t __fastcall LegacyConfirmation::ReadStateEntry(Common* p, void*) { return From(p).ReadInteractionState(); }
void __fastcall LegacyConfirmation::WriteStateEntry(Common* p, void*, std::int32_t value) { From(p).WriteInteractionState(value); }
void LegacyConfirmation::ProcessInput() { ProcessGuiDialogInputAt518C30(state_.common); }
std::int32_t LegacyConfirmation::ReadInteractionState() { return state_.common.stateAt34; }
void LegacyConfirmation::WriteInteractionState(std::int32_t v) { state_.common.stateAt34 = v; }

LegacyConfirmation::LegacyConfirmation(LegacyConfirmationServices& services) : services_(services)
{
    Scope scope(services_);
    // 4FB670：共同children/null-input/navigation/dirty完成后才获得基类清理资格。
    ConstructGuiDialogAt4E0BE0(&state_.common, nullptr, -1);
    state_.owner = this;
    state_.common.vptrAt00 = &Table();
}
LegacyConfirmation::LegacyConfirmation(const LegacyConfirmationData& data,
    std::int32_t mode, LegacyConfirmationServices& services) : LegacyConfirmation(services)
{
    Scope scope(services_);
    {
        Slot factory{};
        CreateFactoryToken(factory);
        RefLife factoryLife{factory};
        // 4DD15E：非法索引的原越界危险域明确拒绝，不钳制/映射到mode4。
        const auto index = static_cast<std::uint32_t>(mode);
        if (index >= data.modes.size()) Reject("Baku 4DD15E: confirmation mode outside nine rows");
        const auto& row = data.modes[index];
        AppendStatic(data.statics.at(static_cast<std::size_t>(row[0])));
        AppendStatic(data.statics.at(static_cast<std::size_t>(row[1])));
        for (const auto& button : data.buttons) AppendButton(button);
        // 4DD2E7..4DD31E：资源/事件完成之后才替换null input，早于两条导航。
        AdoptGuiInputAt536EA0(state_.common.inputAt18, NewGameGuiInputAt4EAAED());
        for (auto direction : data.directions)
            CreateAndAppendGuiNavigation(state_.common.navigationAt24, direction, data.navigation.data());
        mode_ = mode; // 4DD392：末尾才发布mode，result/phase/action仍未初始化。
        factoryLife.Finish();
    }
}

void LegacyConfirmation::AppendStatic(const GuiStaticDescriptorNative24h& descriptor)
{
    Slot child;
    // 4FD9D0：七槽组、可选position、Static，返回槽交给4C9490消费。
    CreateStatic(child, &state_.common, descriptor, services_);
    state_.common.vptrAt00->appendChildAt10(&state_.common, child);
}

void LegacyConfirmation::AppendButton(const GuiButtonDescriptorNative2Ch& descriptor)
{
    Slot child;
    CreateButton(child, &state_.common, descriptor, services_);
    RefLife childLife{child};
    AttachEvent(child);
    childLife.Finish();
}
void LegacyConfirmation::AttachEvent(Slot& child)
{
    Slot event;
    CreateToolClickedEventAt4E2F5A(event, child); // 4DD216分配之后才读取child事件编号。
    RefLife eventLife{event};
    Slot argument;
    CopyGuiClickedEventReference(argument, event);
    RefLife argumentLife{argument};
    // 4FBEF0/41F990：按值Ref赋值后退役，不套现代Popup的缓存control发布策略。
    CopyAssignGraphicsSurfaceReferenceAt465870(RequireGuiControl(child).eventAt3C, argument);
    argumentLife.Finish();
    Slot copied;
    CopyGuiClickedEventReference(copied, child);
    state_.common.vptrAt00->appendChildAt10(&state_.common, copied);
    AppendGuiClickedEventReference(Events(), event); // child先入列，再event；SaveLoad写真实manager。
    eventLife.Finish();
}

void __fastcall LegacyConfirmation::ResetInput(Common* p, void*)
{
    From(p).actionFive_ = 0; // 4FFE00 -> 4C9630。
    p->vptrAt00->setActiveAt30(p, nullptr);
    p->inputLatchAt30 = 0;
}

std::int32_t __fastcall LegacyConfirmation::ActiveInput(Common* p, void*)
{
    if (!From(p).UsesActionFive()) return 0; // 系统菜单53A59C的466D90原恒0。
    if (p->activeAt4C->typeAt08 != 0) return 0;
    auto& self = From(p);
    const auto previous = self.actionFive_;
    if (!previous)
    {
        if (!p->vptrAt00->pressedAt70(p, 5)) return 0;
        self.actionFive_ = 1;
        auto* child = p->activeAt4C;
        reinterpret_cast<const GuiButtonNativeVtable29*>(child->vptrAt00)->setPressedAt70(child, 1);
        return 1;
    }
    if (!p->vptrAt00->heldAt6C(p, 5))
    {
        Slot event;
        CopyGuiClickedEventReference(event, p->activeAt4C->eventAt3C);
        RefLife lifetime{event};
        if (event.baseAt00)
        {
            auto* selected = ResolveGuiClickedEvent(event);
            selected->vptrAt00->invokeAt00(selected);
        }
        self.actionFive_ = 0; // 4DDFF7：invoke异常不清；普通尾先清latch再释放。
        lifetime.Finish();
    }
    return previous;
}

void LegacyConfirmation::CaptureBackground()
{
    // 4DDA58..4DDA98：即使已有背景也重新截图、赋值、释放临时；没有现代if(!capture)。
    auto returned = services_.capture.CaptureScreenNativeAt4ACDD0();
    auto temporary = returned.Detach();
    RefLife lifetime{temporary};
    CopyAssignGraphicsSurfaceReferenceAt465870(background_, temporary);
    lifetime.Finish();
}
void LegacyConfirmation::Begin()
{
    Scope scope(services_);
    if (released_) Reject("LegacyConfirmation: Begin after retirement");
    CaptureBackground();
    auto& p = state_.common;
    p.vptrAt00->writeEnabledAt24(&p, 1);
    ClearGuiClickedEventFlagsAt532200(events_);
    phase_ = 0;
    p.vptrAt00->resetInputAt3C(&p);
    p.vptrAt00->resetChildrenAt68(&p);
    p.activeAt4C = nullptr;
    begun_ = true;
    // 不清result、不写现代Root::ModalGateAt1B8；唯一busy由4DC1B0拥有。
}

void LegacyConfirmation::AddBackgroundBounds()
{
    const auto width = Surface(background_).widthAt08;
    const auto height = Surface(background_).heightAt0C;
    auto& surface = Surface(background_);
    const auto x = surface.originXAt10, y = surface.originYAt14;
    auto& p = state_.common;
    auto* dirty = p.vptrAt00->dirtyAt00(&p);
    dirty->vptrAt00->addXYWHAt10(dirty, x, y, width, height);
}

bool LegacyConfirmation::Poll()
{
    Scope scope(services_);
    if (!begun_ || released_) Reject("Baku 4DDB90: Poll requires a completed Begin");
    auto& p = state_.common;
    switch (phase_)
    {
    case 0: AddBackgroundBounds(); phase_ = 3; break;
    case 3:
        p.vptrAt00->processInputAt54(&p);
        for (auto* at = events_.begin; at != events_.end; ++at)
        {
            if (!RequireGuiClickedEvent(*at).clickedFlagAt04) continue;
            switch (RequireGuiClickedEvent(*at).eventValueAt08)
            {
            case 10: result_ = 6; break;
            case 11: result_ = 7; break;
            default: throw 1; // 4DDD04，不吞未知事件、不继续扫描后续flag。
            }
            resultWritten_ = true;
            phase_ = 4;
            break;
        }
        break;
    case 4:
        AddBackgroundBounds();
        p.vptrAt00->writeEnabledAt24(&p, 0);
        phase_ = -1;
        break;
    case 1: case 2:
        // 4DDC4E/53原自旋且无写入者；正常Begin/Poll不可达，拒绝破坏状态造成的挂死。
        Reject("Baku 4DDB90: unreachable spinning confirmation phase 1/2");
    default: return false;
    }
    p.vptrAt00->flushDirtyAt58(&p);
    return true; // phase4关闭绘制这一帧仍为true，下一帧才false。
}

std::int32_t LegacyConfirmation::Result()
{
    if (!resultWritten_) Reject("Baku 4FFE20: confirmation result has not been written");
    return result_;
}

bool LegacyConfirmation::End()
{
    // 4DDB60没有result条件；mode0总是PostMessage(WM_CLOSE,0,0)，其它mode不取环境。
    if (mode_ == 0)
    {
        auto* env = services_.shared.CaptureEnvironmentAt4180C0();
        if (!env) Reject("Baku 416B40: missing environment for WM_CLOSE");
        static_cast<void>(services_.postMessage(reinterpret_cast<HWND>(env->windowAt2C), WM_CLOSE, 0, 0));
    }
    return false; // 保留背景和modal结果，让调用者419990消费；绝不调用现代End。
}

void __fastcall LegacyConfirmation::Flush(Common* p, void*)
{
    auto& self = From(p);
    auto* dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->coalesceAt04(dirty);
    dirty = p->vptrAt00->dirtyAt00(p);
    const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
    if (!count) return;
    dirty = p->vptrAt00->dirtyAt00(p);
    auto* rects = dirty->vptrAt00->rectsAt1C(dirty);
    for (std::int32_t i = 0; i < count; ++i)
    {
        const auto handle = Surface(self.background_).handleAt04;
        if (handle) static_cast<void>(self.services_.drawing.SurfaceDrawRaw(handle, 0, 0, nullptr, nullptr, rects + i, 0));
        p->vptrAt00->drawAt60(p, rects + i);
        self.DrawExtras(rects + i);
        self.services_.screen.InvalidateRectAt52603A(rects + i);
    }
    dirty = p->vptrAt00->dirtyAt00(p);
    dirty->vptrAt00->clearAt00(dirty); // 4DDE10：任一绘制/提交异常都不清dirty。
}

void LegacyConfirmation::DestroyMembers()
{
    // 4FFE70：原B4/A8两个Ref始终空，无资源；events -> capture -> dirty/nav/input/children。
    struct BaseUnwind final
    {
        Common& common;
        bool armed = true;
        ~BaseUnwind() noexcept { if (armed) DestroyDialog(common); }
    } baseUnwind{state_.common};
    RefLife backgroundUnwind{background_};
    ReleaseReferences(events_);
    backgroundUnwind.Finish(); // 55DC84 state0：普通背景释放异常仍展开基类。
    baseUnwind.armed = false;
    DestroyDialog(state_.common);
}
void LegacyConfirmation::ReleaseResources()
{
    if (released_) return;
    Scope scope(services_);
    released_ = true;
    DestroyMembers();
}
LegacyConfirmation::~LegacyConfirmation() { ReleaseResources(); }

LegacyConfirmationOwner::LegacyConfirmationOwner(const LegacyConfirmationData& data,
    LegacyConfirmationServices& services)
{
    modal_.Reset(); // 4DBFF7 -> 501870 -> 4199C0，先状态再消息框。
    try
    {
        for (std::int32_t i = 0; i < 9; ++i)
        {
            // 4DC046..4DC0A5：正文完成后才建Ref；control失败不回滚已完成正文。
            auto* message = new LegacyConfirmation(data, i, services);
            Slot temporary{};
            ReplaceEnvironmentObjectReference(temporary, message, sizeof(LegacyConfirmation));
            RefLife lifetime{temporary};
            AppendGuiControlChildReference(messages_, temporary); // 47AAE0失败展开临时Ref及已追加前缀。
            lifetime.Finish();
        }
        for (std::int32_t i = 9; i < 11; ++i)
        {
            LegacyConfirmation* message = i == 9
                ? static_cast<LegacyConfirmation*>(new LegacyConfirmationSystem(data, services))
                : static_cast<LegacyConfirmation*>(new LegacyConfirmationSaveLoad(data, services));
            Slot temporary{};
            ReplaceEnvironmentObjectReference(temporary, message, sizeof(LegacyConfirmation));
            RefLife lifetime{temporary};
            AppendGuiControlChildReference(messages_, temporary);
            lifetime.Finish();
        }
    }
    catch (...) { ReleaseResources(); throw; }
}
LegacyConfirmationOwner::~LegacyConfirmationOwner() noexcept { ReleaseResources(); }
void LegacyConfirmationOwner::ReleaseResources()
{
    // 原501A10/Ref vector正序释放，成功后释放存储；不反序析构消息框。
    // 普通退役允许首个资源异常外传，C++析构/构造展开才是noexcept边界。
    for (auto* at = messages_.begin; at != messages_.end; ++at)
        if (auto* message = ResolveEnvironmentObjectReference<LegacyConfirmation>(*at)) message->ReleaseResources();
    ReleaseReferences(messages_);
}
ILegacyTitleQuickLoadMessage* LegacyConfirmationOwner::FindMessage(const LegacyTitleQuickLoadModalState& state)
{
    std::uint32_t index;
    switch (state.kind)
    {
    case 0: index = static_cast<std::uint32_t>(state.mode); break;
    case 1: index = 9; break;
    case 2: index = 10; break;
    default: return nullptr; // 46A63F只有未知kind直接null。
    }
    if (index >= 11) Reject("Baku 4BB550: confirmation index outside original eleven slots");
    if (index >= static_cast<std::uint32_t>(GuiControlChildReferenceCount(messages_)))
        Reject("LegacyConfirmationOwner: lookup after collection retirement");
    return ResolveEnvironmentObjectReference<LegacyConfirmation>(messages_.begin[index]);
}

bool LegacyConfirmationOwner::Begin(std::int32_t kind, std::int32_t mode)
{
    // 4DC1B0：先提交busy/kind/mode，kind2必须先执行旧+84写mode，再Begin。
    try
    {
        modal_.busy = 1; modal_.kind = kind; modal_.mode = mode;
        auto* message = FindMessage(modal_);
        if (!message) throw 1;
        if (kind == 2) SaveLoad().SetMode(mode);
        message->Begin();
        return true;
    }
    catch (...) { modal_.Reset(); return false; }
}
LegacyConfirmationSaveLoad& LegacyConfirmationOwner::SaveLoad()
{
    auto state = modal_; state.kind = 2;
    return *static_cast<LegacyConfirmationSaveLoad*>(FindMessage(state));
}
bool LegacyConfirmationOwner::CanOpenSystem()
{
    // 4DC310/419A00/419A20：检查未消费result!=-1，不是busy；不要吞掉已完成确认结果。
    if (modal_.result != -1 || suppressSystem_) return false;
    if (GuiControlChildReferenceCount(messages_) <= 9) return false;
    auto state = modal_; state.kind = 1;
    auto* message = static_cast<LegacyConfirmationSystem*>(FindMessage(state));
    return message && message->CanOpen();
}

bool TryTickLegacyConfirmationFrame(ILegacyTitleQuickLoadSource& source, IModalDialogRunnerServices& frame)
{
    // 44CAF0：存在、busy、Tick各自取源；输入更新可以改变当前root。
    if (!source.CaptureModal()) return false;
    auto* selected = source.CaptureModal();
    if (!selected) Reject("Baku 44CB59: GUI disappeared before busy read");
    if (!selected->busy) return false;
    frame.UpdateInputAt450AA0();
    selected = source.CaptureModal();
    if (!selected) Reject("Baku 44CB94: GUI disappeared before modal tick");
    TickLegacyTitleQuickLoadModal(*selected, source);
    frame.RenderScreenIfGraphicsLoaded();
    return true;
}
}

#include "SaveLoadPopupNative.Internal.h"
#include <new>
#include <algorithm>
#include <vector>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadPopupNativeDetail;
void AppendChild(Common& p, const Slot& child)
{ GuiDialogBaseVtable().appendChildAt10(&p, Copy(child)); }
template<class Effect> void MoveGroup(const Slot& child, unsigned index, LONG y, Effect&& effect)
{
    auto* control = Resolve<GuiControlNativeBody50h>(child);
    Slot group = Copy(Read(control->resourcesAt28.begin)[index]);
    Local lifetime{group};
    auto* captured = group.controlAt08;
    const Slot cached{group.baseAt00, group.biasAt04, captured};
    MoveGuiControlResourceAt5086C0(Copy(cached), CurrentSaveLoadResourceProfile().popup.firstRowOrigin.x, y);
    const RECT rectangle = Resolve<GuiControlResourceNativeBody38h>(cached)->combinedRectAt28;
    effect(rectangle); // 原Union/child位置写入发生于缓存group退役之前。
    lifetime.Captured(captured);
}
void AddEvent(Body& p, Slot& child, Local& childLife, bool cachedTail)
{
    // 4EFBD0/4F0539/4F0786先new0C，成功后才解析当前child并读取+04。
    void* raw = ::operator new(0x0C);
    auto* body = raw ? ::new(raw) GuiClickedEventNativeBody{
        &GuiClickedEventVtable(), 0, Read(Resolve<GuiControlNativeBody50h>(child)->eventAt04)} : nullptr;
    Fresh event;
    ConstructGuiClickedEventReferenceAt4D7780(event.r, body, 1);
    Local eventLife{event.r};
    const auto base = event.r.baseAt00;
    auto* captured = event.r.controlAt08;
    Slot binding{base, event.r.biasAt04, captured};
    if (captured) ++captured->referenceCountAt04;
    BindEventAt536DB0(Resolve<GuiControlNativeBody50h>(child), binding);
    AppendChild(p.common, child);
    Slot managerArgument{base, Read(event.r.biasAt04), captured};
    if (captured) ++captured->referenceCountAt04;
    AppendEventAt4FEF80(Manager(p), managerArgument);
    if (cachedTail) eventLife.Captured(captured); else eventLife.Current(true);
    childLife.Current(!cachedTail); // 上箭头state20无清写；循环/下箭头显式原release。
}
void MakeNumber(Slot& out, std::int32_t event, const RECT& rectangle, SaveLoadPopupNativeServices& s)
{
    NewNumberReference(out); // 4F0053之后才启用state27；失败的control留下原孤儿。
    Local lifetime{out};
    auto* number = Resolve<Number>(out);
    number->eventAt00 = event;
    number->pageAt04 = -1;
    number->originAt08 = {rectangle.left, rectangle.top};
    for (unsigned i = 0; i != 3; ++i)
    {
        AdoptGraphicsSurfaceAt407A80(number->surfacesAt10[i], NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings));
        const auto height = Sub(rectangle.bottom, rectangle.top);
        const auto width = Sub(rectangle.right, rectangle.left);
        auto* surface = Resolve<GraphicsSurfaceNativeBody>(number->surfacesAt10[i]);
        // 前两份是415010相同的内联Reset/Create/提交次序；第三份直接调用415010。
        if (!surface->CreateAt415010(s.surfaces, width, height, 1, 1)) throw 1;
    }
    lifetime.active = false;
}
void LoadResources(Vector& destination, const char* const* names, unsigned count, SaveLoadPopupNativeServices& s)
{
    ReleaseGuiControlSurfaceRangeAt43EA10(destination.begin, destination.end);
    destination.end = destination.begin;
    for (unsigned i = 0; i != count; ++i)
    {
        Slot surface{};
        Local lifetime{surface}; // 原state42/46在Ensure前。
        void* receiver = s.loader.CaptureLoadReceiverAt413BE0();
        const char* name = names[i]; // 4F093B/4F0A9A在Capture之后读取。
        AdoptGraphicsSurfaceAt407A80(surface, NewGraphicsSurfaceAt40C2E0(s.surfaces, s.strings));
        if (!s.loader.LoadIntoAt4800A0(receiver, *Resolve<GraphicsSurfaceNativeBody>(surface), name)) throw 1;
        AppendGuiControlSurfaceReference(destination, surface);
        lifetime.Current(true); // 4C5A30重读并清base/control，bias保持。
    }
}
}
GuiDialogNativeBody5Ch* ConstructSaveLoadPopupNativeAt4EF660(void* rawC0, Common* parent,
    std::int32_t id, SaveLoadPopupNativeServices& s)
{
    SaveLoadPopupNativeScope constructionScope{s};
    auto* p = ::new(rawC0) Body;
    ConstructGuiDialogAt4E0BE0(&p->common, parent, id);
    p->managerAt70 = {};
    p->common.vptrAt00 = &SaveLoadPopupNativeVtable();
    p->digitsAt88 = {};
    p->markersAt94 = {};
    p->numbersAtA0 = {};
    struct Unwind final
    {
        Body& p; bool active = true;
        ~Unwind() noexcept { if (active) DestroyConstructedMembers(p); }
    } unwind{*p};
    auto* rawManager = ::operator new(0x0C);
    auto* manager = rawManager ? ::new(rawManager) GuiClickedEventReferences{} : nullptr;
    AdoptGuiClickedEventManagerAt4D7B90(p->managerAt70, manager);
    RECT combined{};
    const auto& profile=CurrentSaveLoadResourceProfile().popup;
    const auto& data = DataForId(id);
    const auto pageCount = PageCount();
    const auto maximum = MaximumFirstPage(pageCount);
    LoadResources(p->digitsAt88, data.digits, profile.digitStates * 10, s);
    LoadResources(p->markersAt94, data.markers, 3, s);
    if(!profile.tiledFrame)
    {
        Fresh background;
        CreateGuiStaticAt505800(background.r, &p->common, data.background, s.loader, s.surfaces, s.strings);
        Local backgroundLife{background.r};
        AppendChild(p->common, background.r);
        auto* child = Resolve<GuiControlNativeBody50h>(background.r);
        Slot group = Copy(Read(child->resourcesAt28.begin)[0]);
        Local groupLife{group};
        auto* captured = group.controlAt08;
        Fresh canvas;
        SelectSurfaceAt50B1F0(canvas.r, Resolve<GuiControlResourceNativeBody38h>(group), 0);
        Local canvasLife{canvas.r};
        groupLife.Captured(captured);
        Slot bar{};
        Local barLife{bar};
        auto* receiver = s.loader.CaptureLoadReceiverAt413BE0();
        if (!LoadGuiControlSurfaceReferenceAt480000(bar, receiver, data.bar, s.loader, s.surfaces, s.strings)) throw 1;
        auto* barControl = bar.controlAt08;
        const auto barBias = bar.biasAt04;
        Blit(Resolve<GraphicsSurfaceNativeBody>(canvas.r), Resolve<GraphicsSurfaceNativeBody>(bar), profile.barOrigin);
        // 4EF909重读bar.base，但继续使用原ESI control/EDI bias。
        const Slot cachedBar{Read(bar.baseAt00), barBias, barControl};
        auto* image = Resolve<GraphicsSurfaceNativeBody>(cachedBar);
        RECT rectangle{profile.barOrigin.x,profile.barOrigin.y,
            Add(Read(image->widthAt08),profile.barOrigin.x),Add(Read(image->heightAt0C),profile.barOrigin.y)};
        ::UnionRect(&combined, &combined, &rectangle);
        barLife.Captured(barControl);
        canvasLife.Current();
        Fresh slider;
        auto sliderDescriptor=data.slider;
        sliderDescriptor.eventAt00=SaveLoadConsumerEvent(SaveLoadEventScope::Popup,sliderDescriptor.eventAt00);
        CreateGuiSliderAt5052C0(slider.r, &p->common, sliderDescriptor, s.loader, s.surfaces, s.strings);
        Local sliderLife{slider.r};
        auto* selected = Resolve<GuiControlNativeBody50h>(slider.r);
        SetSliderRange(selected, maximum);
        selected = Resolve<GuiControlNativeBody50h>(slider.r);
        reinterpret_cast<const GuiSliderNativeVtable31*>(selected->vptrAt00)->setStepInputAt74(selected, 0);
        AppendChild(p->common, slider.r);
        sliderLife.Current();
        backgroundLife.Current(); // 黑底/Slider不参加combined，只有bar、箭头与Checkbox参加。
    }
    else
    {
        // 原旧Popup使用三段框与独立滚动条，借现代控件组承载，不构造旧Popup对象。
        GuiStaticDescriptorNative24h descriptor{56,{data.bar,nullptr,nullptr,data.bar,data.bar,nullptr,nullptr},data.slider.positionAt3C};
        Fresh bar;CreateGuiStaticAt505800(bar.r,&p->common,descriptor,s.loader,s.surfaces,s.strings);
        Local lifetime{bar.r};AppendChild(p->common,bar.r);
        auto* control=Resolve<GuiControlNativeBody50h>(bar.r);
        RECT bounds;control->vptrAt00->rectAt3C(control,&bounds);
        control->vptrAt00->writeEnabledAt4C(control,maximum>0);
        auto sliderDescriptor=data.slider;
        sliderDescriptor.eventAt00=57;
        sliderDescriptor.interactionRectAt4C=bounds;
        sliderDescriptor.travelDeltaAt5C={0,0,0,profile.sliderTravelLength-(bounds.bottom-bounds.top)};
        Fresh slider;CreateGuiSliderAt5052C0(slider.r,&p->common,sliderDescriptor,s.loader,s.surfaces,s.strings);
        Local sliderLife{slider.r};AppendChild(p->common,slider.r);
        control=Resolve<GuiControlNativeBody50h>(slider.r);
        const auto* table=reinterpret_cast<const GuiSliderNativeVtable31*>(control->vptrAt00);
        SetSliderRange(control,maximum);table->setStepInputAt74(control,0);
        control->vptrAt00->writeEnabledAt4C(control,maximum>0);
        ::UnionRect(&combined,&combined,&bounds);
    }
    LONG y;
    {
        Fresh arrow;
        auto descriptor=data.arrows[0];descriptor.common.eventAt00=58;
        CreateGuiButtonAt5041D0(arrow.r, &p->common, descriptor, s.loader, s.surfaces, s.strings);
        Local arrowLife{arrow.r};
        MoveGroup(arrow.r, 0, profile.firstRowOrigin.y, [&](const RECT& rectangle)
        {
            auto* child = Resolve<GuiControlNativeBody50h>(arrow.r);
            child->xAt34 = rectangle.left; child->yAt38 = rectangle.top;
            ::UnionRect(&combined, &combined, &rectangle);
            // 01:48F570以框顶图高度起算首行，不以箭头各态的包围框代替。
            const auto topHeight = profile.tiledFrame
                ? Resolve<GraphicsSurfaceNativeBody>(p->markersAt94.begin[0])->heightAt0C
                : Sub(rectangle.bottom, rectangle.top);
            y = Add(topHeight, profile.firstRowOrigin.y + profile.rowInset);
        });
        AddEvent(*p, arrow.r, arrowLife, true);
    }
    // 4EFD3E仍执行空范围release；保留vector头原生命周期，不另建现代列表。
    ReleaseGuiControlSurfaceRangeAt43EA10(p->numbersAtA0.begin, p->numbersAtA0.end);
    p->numbersAtA0.end = p->numbersAtA0.begin;
    for (std::int32_t i = 0; i != profile.visibleRows; ++i)
    {
        Fresh checkbox;
        CreateCheckBoxAt5043B0(checkbox.r, &p->common, data.checkbox, s);
        Local checkboxLife{checkbox.r};
        auto* row = Resolve<GuiControlNativeBody50h>(checkbox.r);
        row->eventAt04 = i + 60;
        row->vptrAt00->writeEnabledAt4C(row, i < VisibleRows(pageCount));
        if (profile.tiledFrame && !data.checkbox.normal[0])
        {
            // 原空白normal行仍有明确尺寸和矩形命中区，不能借用18号底图。
            auto* group = Resolve<GuiControlResourceNativeBody38h>(row->resourcesAt28.begin[0]);
            auto& bounds = group->combinedRectAt28;
            bounds.right = bounds.left + profile.blankRowSize.cx;
            bounds.bottom = bounds.top + profile.blankRowSize.cy;
        }
        RECT rectangle{};
        MoveGroup(checkbox.r, 0, y, [&](const RECT& moved) { ::UnionRect(&rectangle, &rectangle, &moved); });
        MoveGroup(checkbox.r, 1, y, [&](const RECT& moved) { ::UnionRect(&rectangle, &rectangle, &moved); });
        auto* child = Resolve<GuiControlNativeBody50h>(checkbox.r);
        child->xAt34 = rectangle.left; child->yAt38 = rectangle.top;
        Fresh number;
        MakeNumber(number.r, i + 60, rectangle, s);
        Local numberLife{number.r};
        AppendNumberAt4D8AC0(p->numbersAtA0, number.r);
        numberLife.Current(true);
        ::UnionRect(&combined, &combined, &rectangle);
        y = Add(y, profile.tiledFrame?profile.rowPitch:Sub(rectangle.bottom, rectangle.top));
        AddEvent(*p, checkbox.r, checkboxLife, false);
    }
    {
        Fresh arrow;
        auto descriptor=data.arrows[1];descriptor.common.eventAt00=59;
        CreateGuiButtonAt5041D0(arrow.r, &p->common, descriptor, s.loader, s.surfaces, s.strings);
        Local arrowLife{arrow.r};
        const auto bottomY = profile.tiledFrame
            ? profile.firstRowOrigin.y + Resolve<GraphicsSurfaceNativeBody>(p->markersAt94.begin[0])->heightAt0C +
                VisibleRows(pageCount) * profile.rowPitch + 2 * profile.rowInset
            : y;
        MoveGroup(arrow.r, 0, bottomY, [&](const RECT& rectangle)
        {
            auto* child = Resolve<GuiControlNativeBody50h>(arrow.r);
            child->xAt34 = rectangle.left; child->yAt38 = rectangle.top;
            ::UnionRect(&combined, &combined, &rectangle);
        });
        AddEvent(*p, arrow.r, arrowLife, false);
    }
    for (const auto& item:profile.navigation)
    {
        std::vector<GuiDialogNavigationMapping> pairs(item.pairs.begin(),item.pairs.end());
        for(auto& pair:pairs)
        {
            if(pair.sourceEventValue>=0)pair.sourceEventValue=SaveLoadConsumerEvent(SaveLoadEventScope::Popup,pair.sourceEventValue);
            if(pair.targetEventValue>=0)pair.targetEventValue=SaveLoadConsumerEvent(SaveLoadEventScope::Popup,pair.targetEventValue);
        }
        Fresh navigation;
        CreateGuiNavigationAt504CE0(navigation.r, item.input, pairs.data());
        Local navigationLife{navigation.r};
        AppendGuiNavigationReferenceAt4D9D60(p->common.navigationAt24, navigation.r);
        navigationLife.Current(true);
    }
    p->firstPageAt80 = -1;
    p->selectedPageAt7C = 0;
    p->boundsAt60 = combined;
    unwind.active = false;
    return &p->common;
}
}

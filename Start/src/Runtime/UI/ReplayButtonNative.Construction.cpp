#include "ReplayButtonNative.Internal.h"
#include <Runtime/ReplayPageResourceProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::ReplayButtonNativeDetail
{
namespace
{
template<class F> F Virtual(void* object, unsigned offset) noexcept
{ return Read(Field<F>(Read(Field<void*>(object,0)),offset)); }
template<class T> T* ResolveWithControl(const Slot& slot, YtlReferenceControlRuntime* control) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    const auto stride = control ? Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(slot.biasAt04) * stride);
}
Surface* ResolveWithBias(const Slot& slot, unsigned bias) noexcept
{
    const auto base = Read(slot.baseAt00);
    if (!base) return nullptr;
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    return reinterpret_cast<Surface*>(base + bias * (control ? Read(control->strideAt0C) : 0u));
}
ParamsGameSystem* Params(unsigned site)
{
    const auto* root = RequireReplayButtonNativeServices().source.CaptureParamsAt418480(site);
    return Resolve<ParamsGameSystem>(root->gameSystemAt04.Native());
}
int Groups(ParamsGameSystem* system) noexcept
{ return Count(system->registScene.storage); }
int Items(ParamsGameSystem* system, int category) noexcept
{
    auto* first = Read(system->registScene.storage.begin);
    auto* group = Resolve<ParamsRegistSceneGroupRuntime>(*Advance(first,category));
    return Count(group->itemsAt04.storage);
}
void* Secret(EnvironmentRootStorage* root) noexcept
{
    auto* last = Read(root->archivesAt30.end);
    auto* first = Read(root->archivesAt30.begin);
    if (unsigned(Distance(first,last)) <= 4u) return nullptr;
    return Resolve<void>(*Advance(first,4));
}
void* SoundSystem(unsigned site)
{
    auto* root = RequireReplayButtonNativeServices().source.CaptureEnvironmentAt4180C0(site);
    auto* registry = ResolveEnvironmentArchiveAt417990(*root,1);
    return Resolve<void>(Field<Slot>(registry,0x1C));
}
struct Frame final
{
    Body& body;
    ReplayButtonNativeServices& s;
    int state = 0;
    bool active = true;
    void* block = nullptr;
    void* rawA4 = nullptr;
    void* raw44 = nullptr;
    void* rawAC = nullptr;
    void* rawB0 = nullptr;
    unsigned arrayCount = 2;
    Slot page, temporary, appendPage, off, click, focus, button, image, work, clicked, focused;
    union { RuntimeCString name; };
    const char* normalNameChars = nullptr;
    Frame(Body& b, ReplayButtonNativeServices& services) noexcept : body(b), s(services) {}
    ~Frame() noexcept { if (active) UnwindTo(-1); }
    void UnwindTo(int target) noexcept
    {
        while (state > target)
        {
            const auto entry = constructorUnwind[state];
            switch (entry.action)
            {
            case 0: break;
            case 0x541E55: std::terminate();
            case 0x59CA20: DestroyReplayButtonNativeAt4A4C60(body); break;
            case 0x59CA28: s.mode.memory.DeleteBody(block,0x24); break;
            case 0x59CA37: DestroyRuntimeCStringAt40CD40(&Field<RuntimeCString>(block,0x1C)); break;
            case 0x59CA42: ReleaseSurface(page); break;
            case 0x59CA4A: s.mode.memory.DeleteBody(rawA4,0x24); break;
            case 0x59CA5C: s.mode.memory.DeleteBody(raw44,0x24); break;
            case 0x59CA6B: ReleaseSurface(temporary); break;
            case 0x59CA73: ReleaseSurface(appendPage); break;
            case 0x59CA7E: ReleaseSurface(off); break;
            case 0x59CA86: ReleaseSurface(click); break;
            case 0x59CA8E: ReleaseSurface(focus); break;
            case 0x59CA99: DestroyRuntimeCStringAt40CD40(&name); break;
            case 0x59CAA1: case 0x59CAC6: s.environmentLock.Leave(); break;
            case 0x59CAAC: s.mode.memory.DeleteBody(rawA4,0x6C); break;
            case 0x59CABE: ReleaseYtlReference(button); break;
            case 0x59CAD1: ReleaseSurface(image); break;
            case 0x59CAD9: s.mode.memory.DeleteBody(rawAC,0x24); break;
            case 0x59CAEB: s.mode.memory.DeleteBody(rawB0,0x24); break;
            case 0x59CAFD: ReleaseSurface(work); break;
            case 0x59CB05: ReleaseSurface(clicked); break;
            case 0x59CB10: ReleaseSurface(focused); break;
            case 0x59CB1B:
            {
                const auto product = arrayCount > 0xFFFFFFFFu / 0x6Cu ? 0xFFFFFFFFu : arrayCount * 0x6Cu;
                const auto bytes = product > 0xFFFFFFFFu - 4u ? 0xFFFFFFFFu : product + 4u;
                ::operator delete[](rawAC,std::size_t{bytes});
                break;
            }
            default: std::terminate(); // 仅已核35条FI action合法，不能静默跳过未知资格。
            }
            state = entry.previous;
        }
    }
    Surface* NewSurface(void*& raw, int allocationState, int previous)
    {
        raw = s.mode.memory.AllocateBody(0x24);
        state = allocationState;
        // 原首个inline state3->4->3的CString/432670资格由已验24h ctor自身完成。
        // 其失败先退CString，外层仍按原分配state释放raw，不重复析构完整Surface。
        auto* result = raw ? ::new(raw) Surface(s.surfaces,s.strings) : nullptr;
        state = previous;
        return result;
    }
    EnvironmentRootStorage* InlineEnvironment(bool second)
    {
        s.environmentLock.Enter();
        const bool published = Read(s.source.EnvironmentPublicationAt6198C8(second ? 0x49FD90 : 0x49FA78)) != nullptr;
        state = second ? 22 : 17; // 原检查发布槽后才写locked state。
        if (!published)
        {
            auto* raw = s.mode.memory.AllocateBody(0x60);
            auto* root = static_cast<EnvironmentRootStorage*>(raw);
            if (root)
            {
                // 两次nil成功后才清向量/Ref及发布；失败只解锁，原60h/首串不补回收。
                InitializeEmptyRuntimeCString(root->titleAt10,s.strings);
                InitializeEmptyRuntimeCString(root->appendTitleAt14,s.strings);
                root->archivesAt30 = {};
                root->errorsAt3C = {};
                ::new(&root->pathsAt48) Slot{};
                ::new(&root->versionAt54) Slot{};
            }
            s.source.EnvironmentPublicationAt6198C8(second ? 0x49FE48 : 0x49FB30) = root;
        }
        state = second ? 23 : 18;
        s.environmentLock.Leave();
        if (!second)
        {
            state = 16; // 49FB54：第一处解锁后直接Params，不额外读取6198C8。
            return nullptr;
        }
        auto* root = Read(s.source.EnvironmentPublicationAt6198C8(0x49FE69));
        state = 16; // 49FE74：原重读仍属于state23 terminate域。
        return root;
    }
};
struct PercentFrame final
{
    Slot digit;
    int state = 0;
    bool active = true;
    ~PercentFrame() noexcept { if (active) UnwindTo(-1); }
    void UnwindTo(int target) noexcept
    {
        while (state > target)
        {
            const auto entry = percentUnwind[state];
            if (entry.action == 0x541E55) std::terminate();
            if (entry.action == 0x59E8A0 || entry.action == 0x59E8A8 || entry.action == 0x59E8B0)
                ReleaseSurface(digit);
            state = entry.previous;
        }
    }
};
bool DrawPercent(int value, Surface* destination, const ReplayPageResourceProfile& profile)
{
    auto& s = RequireReplayButtonNativeServices();
    PercentFrame f;
    try
    {
        struct TryUnwind final
        { PercentFrame& f; bool active = true; ~TryUnwind() noexcept { if (active) f.UnwindTo(0); } } unwind{f};
        const auto tens = value / 10;
        f.state = 1;
        auto* receiver = Draw(0x4BC9BB,0x4BC9C0);
        if (!LoadGuiControlSurfaceReferenceAt480000(f.digit,receiver,
            profile.percentDigits[value % 10],s.pictures,s.surfaces,s.strings)) throw 1;
        auto* captured = Read(f.digit.controlAt08);
        Blit(destination,ResolveWithControl<Surface>(f.digit,captured),profile.percentPositions[0].x,profile.percentPositions[0].y);
        f.state = 2;
        ReleaseCaptured(f.digit,captured,false);
        f.digit = {};
        f.state = 3;
        const auto hundreds = tens / 10;
        captured = nullptr;
        if (tens)
        {
            receiver = Draw(0x4BCA9F,0x4BCAA4);
            if (!LoadGuiControlSurfaceReferenceAt480000(f.digit,receiver,
                profile.percentDigits[tens % 10],s.pictures,s.surfaces,s.strings)) throw 1;
            captured = Read(f.digit.controlAt08);
            Blit(destination,ResolveWithControl<Surface>(f.digit,captured),profile.percentPositions[1].x,profile.percentPositions[1].y);
        }
        f.state = 4;
        ReleaseCaptured(f.digit,captured,false);
        f.state = 0;
        if (hundreds)
        {
            f.digit = {};
            f.state = 5;
            // 原4BCBA0第三位固定取names[1]，不是hundreds%10，也不截断value。
            receiver = Draw(0x4BCB71,0x4BCB76);
            if (!LoadGuiControlSurfaceReferenceAt480000(f.digit,receiver,
                profile.percentDigits[1],s.pictures,s.surfaces,s.strings)) throw 1;
            Blit(destination,Resolve<Surface>(f.digit),profile.percentPositions[2].x,profile.percentPositions[2].y);
            ReleaseSurface(f.digit);
        }
        unwind.active = false;
    }
    catch (...)
    {
        // 5EE7CC：try0..5，handler6；局部先按当前槽展开，正常缓存释放失败则terminate。
        f.state = 6;
        f.active = false;
        return false;
    }
    f.active = false;
    return true;
}
void Build(Frame& f, int category)
{
    auto& b = f.body;
    auto& s = f.s;
    const auto& profile = CurrentReplayPageProfile();
    if (category < 0 || static_cast<std::size_t>(category) >= profile.backgrounds.size())
        throw std::out_of_range("Replay category is outside the selected page profile");
    const auto& background = profile.backgrounds[category];
    f.state = 2;
    ReleaseGuiControlSurfaceRangeAt43EA10(Read(b.pagesAt50.begin),Read(b.pagesAt50.end));
    b.pagesAt50.end = Read(b.pagesAt50.begin);
    f.state = 1;
    static_cast<void>(s.source.CaptureEnvironmentAt4180C0(0x49F315));
    int itemCount = -1;
    if (category < Groups(Params(0x49F31A))) itemCount = Items(Params(0x49F363),category);
    const auto quotient = itemCount / profile.itemsPerPage;
    const auto pageCount = Add(quotient,itemCount != Mul(quotient,profile.itemsPerPage) ? 1 : 0);
    const char* backgroundName = background.normal;
    if (background.availabilityIndex >= 0)
    {
        auto* system = SoundSystem(0x49F3FC);
        using Query = const std::uint8_t* (__thiscall*)(void*,int);
        const auto available = Read(*Virtual<Query>(system,0x84)(system,background.availabilityIndex));
        // 只有原表明确提供的未开放背景可以替换普通背景。
        if (!available && background.unavailable) backgroundName = background.unavailable;
    }
    for (int i = 0; i < pageCount; i = Add(i,1))
    {
        auto* surface = f.NewSurface(f.block,3,1);
        ConstructGraphicsSurfaceReferenceAt451500(f.page,surface,1);
        auto* pageControl = Read(f.page.controlAt08);
        f.state = 5;
        auto* receiver = Draw(0x49F52C,0x49F531);
        const auto* name = backgroundName;
        if (!Read(f.page.baseAt00))
        {
            AdoptGraphicsSurfaceAt407A80(f.page,f.NewSurface(f.rawA4,6,5));
            pageControl = Read(f.page.controlAt08);
        }
        if (!s.pictures.LoadIntoAt4800A0(receiver,*ResolveWithControl<Surface>(f.page,pageControl),name)) throw 1;
        if (pageCount > 1)
        {
            if (static_cast<std::size_t>(i) >= profile.pageMarks.size()) throw 1;
            auto* mark = f.NewSurface(f.raw44,7,5);
            ConstructGraphicsSurfaceReferenceAt451500(f.temporary,mark,1);
            f.state = 8;
            if (!LoadGuiControlSurfaceReferenceAt480000(f.temporary,Draw(0x49F625,0x49F62A),
                profile.pageMarks[i],s.pictures,s.surfaces,s.strings)) throw 1;
            auto* markSurface = Resolve<Surface>(f.temporary);
            const auto x = Read(markSurface->originXAt10);
            const auto y = Read(markSurface->originYAt14);
            Blit(ResolveWithControl<Surface>(f.page,pageControl),Resolve<Surface>(f.temporary),x,y);
            f.state = 9;
            ReleaseYtlReference(f.temporary);
        }
        f.appendPage = CopyCaptured(f.page,pageControl);
        f.state = 10;
        AppendGuiControlSurfaceReference(b.pagesAt50,f.appendPage);
        f.state = 11;
        ReleaseCaptured(f.appendPage,Read(f.appendPage.controlAt08),true);
        f.state = 12;
        ReleaseCaptured(f.page,pageControl,true);
        f.state = 1;
    }
    b.pageAt6C = 0;
    auto* secret = Secret(s.source.CaptureEnvironmentAt4180C0(0x49F7F5));
    using Percent = int (__thiscall*)(void*,int);
    const auto value = Virtual<Percent>(secret,0x24)(secret,category);
    for (int i = 0; unsigned(i) < unsigned(Count(b.pagesAt50)); i = Add(i,1))
        if (!DrawPercent(value,Resolve<Surface>(*Advance(Read(b.pagesAt50.begin),i)),profile)) throw 2;
    f.off = {}; f.click = {}; f.focus = {};
    f.state = 15;
    if (!LoadGuiControlSurfaceReferenceAt480000(f.click,Draw(0x49F90B,0x49F910),profile.stamps[0],s.pictures,s.surfaces,s.strings)) throw 1;
    if (!LoadGuiControlSurfaceReferenceAt480000(f.focus,Draw(0x49F953,0x49F958),profile.stamps[1],s.pictures,s.surfaces,s.strings)) throw 1;
    if (!LoadGuiControlSurfaceReferenceAt480000(f.off,Draw(0x49F99E,0x49F9A3),profile.stamps[2],s.pictures,s.surfaces,s.strings)) throw 1;
    InitializeEmptyRuntimeCString(f.name,s.strings);
    f.normalNameChars = f.name.GetString(); // 49F9F7：ESI保留nil字符指针，直到成功分支4A0234重捕。
    f.state = 16;
    auto* clickSurface = Resolve<Surface>(f.click);
    const auto width = Read(clickSurface->widthAt08);
    const auto height = Read(clickSurface->heightAt0C);
    b.common.itemCountAt3C = 0;
    for (int i = 0; i < profile.maximumItems; i = Add(i,1))
    {
        static_cast<void>(f.InlineEnvironment(false));
        if (category >= Groups(Params(0x49FB58))) break;
        if (i >= Items(Params(0x49FBA3),category)) break;
        f.rawA4 = s.mode.memory.AllocateBody(0x6C);
        f.state = 19;
        auto* created = f.rawA4 ? ConstructCgUiButtonNativeAt459530(*::new(f.rawA4) Button) : nullptr;
        f.state = 16;
        AdoptButton(f.button,created);
        f.state = 20;
        AppendChild(b.common.childrenAt14,f.button);
        f.state = 21;
        ReleaseCaptured(f.button,Read(f.button.controlAt08),true);
        f.state = 16;
        auto* button = Resolve<Button>(*Advance(Read(b.common.childrenAt14.begin),Sub(Count(b.common.childrenAt14),1)));
        secret = Secret(f.InlineEnvironment(true));
        using Thumbnail = const char* const* (__thiscall*)(void*,int,int);
        const auto* resource = Virtual<Thumbnail>(secret,0x1C)(secret,category,Read(b.common.itemCountAt3C));
        const auto pageIndex = i / profile.itemsPerPage;
        const auto position = profile.positions[i % profile.itemsPerPage];
        if (resource)
        {
            f.image = {};
            f.state = 24;
            auto* receiver = Draw(0x49FF20,0x49FF25);
            const auto* resourceName = Read(*resource);
            auto* image = f.NewSurface(f.rawAC,25,24);
            AdoptGraphicsSurfaceAt407A80(f.image,image);
            const auto imageBias = Read(f.image.biasAt04);
            if (!s.pictures.LoadIntoAt4800A0(receiver,*Resolve<Surface>(f.image),resourceName)) throw 1;
            auto* work = f.NewSurface(f.rawB0,26,24);
            ConstructGraphicsSurfaceReferenceAt451500(f.work,work,1);
            f.state = 27;
            if (!work->CreateAt415010(s.surfaces,width,height,1,1)) throw 1;
            Blit(reinterpret_cast<Surface*>(Read(f.work.baseAt00)),ResolveWithBias(f.image,imageBias),profile.imageOffset.x,profile.imageOffset.y);
            FormatRuntimeCStringAt417130(f.name,profile.thumbnailKeys[0],category,i);
            if (!AttachCgUiButtonSurfaceNativeAt4C1F20(*button,0,f.name.GetString(),f.work,s.mode.buttons)) throw 2;
            auto* workControl = Read(f.work.controlAt08);
            CloneGraphicsSurfaceAt444470(f.clicked,*ResolveWithControl<Surface>(f.work,workControl),1,s.surfaces,s.copy,s.strings);
            f.state = 28;
            Blit(Resolve<Surface>(f.clicked),Resolve<Surface>(f.click),0,0);
            FormatRuntimeCStringAt417130(f.name,profile.thumbnailKeys[1],category,i);
            if (!AttachCgUiButtonSurfaceNativeAt4C1F20(*button,2,f.name.GetString(),f.clicked,s.mode.buttons)) throw 2;
            f.state = 27;
            ReleaseSurface(f.clicked);
            CloneGraphicsSurfaceAt444470(f.focused,*ResolveWithControl<Surface>(f.work,workControl),1,s.surfaces,s.copy,s.strings);
            f.state = 29;
            Blit(Resolve<Surface>(f.focused),Resolve<Surface>(f.focus),0,0); // 原444770仅解址focus Ref。
            FormatRuntimeCStringAt417130(f.name,profile.thumbnailKeys[2],category,i);
            f.normalNameChars = f.name.GetString(); // 4A0234：Attach之前缓存，正常尾不重读当前CString。
            if (!AttachCgUiButtonSurfaceNativeAt4C1F20(*button,1,f.normalNameChars,f.focused,s.mode.buttons)) throw 2;
            ReleaseSurface(f.focused);
            button->positionAt24 = {Sub(position.x,profile.buttonOffset.x),Sub(position.y,profile.buttonOffset.y)};
            button->useNaturalAt2C = 0;
            button->stateAt04 = 0;
            button->latchAt20 = 0;
            ReleaseSurface(f.work);
            f.state = 16;
            ReleaseSurface(f.image);
        }
        else
        {
            auto* target = Resolve<Surface>(*Advance(Read(b.pagesAt50.begin),pageIndex));
            Blit(target,Resolve<Surface>(f.off),position.x,position.y);
        }
        b.common.itemCountAt3C = Add(Read(b.common.itemCountAt3C),1);
    }
    f.state = 15;
    // 4A031C..4A0335用缓存ESI；EH state16/59CA99仍按当前name槽释放。
    ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(f.normalNameChars) - sizeof(ATL::CStringData)));
    f.state = 30;
    ReleaseCaptured(f.focus,Read(f.focus.controlAt08),false);
    f.state = 31;
    ReleaseYtlReference(f.click); // 原4C5A30可抛，但外层31/32展开为terminate。
    f.state = 32;
    ReleaseYtlReference(f.off);
    f.state = 1;
    if (unsigned(Count(b.pagesAt50)) >= 2u)
    {
        f.arrayCount = 2;
        f.rawAC = ::operator new[](std::size_t{0xDC});
        f.state = 33;
        Button* array = nullptr;
        if (f.rawAC)
        {
            *static_cast<unsigned*>(f.rawAC) = 2;
            array = reinterpret_cast<Button*>(static_cast<unsigned*>(f.rawAC) + 1);
            struct Elements final
            {
                Button* array;
                unsigned constructed = 0;
                bool active = true;
                ~Elements() noexcept
                { if (active) while (constructed) DestroyCgUiButtonNativeAt463160(array[--constructed]); }
            } elements{array};
            for (; elements.constructed < 2; ++elements.constructed)
                ConstructCgUiButtonNativeAt459530(*::new(array + elements.constructed) Button);
            elements.active = false;
        }
        f.state = 1;
        AdoptPageArray(b.pageButtonsAt5C,array,2);
        for (int i = 0; i < Count(b.pageButtonsAt5C); i = Add(i,1))
        {
            auto* button = Resolve<Button>(b.pageButtonsAt5C,i);
            auto* receiver = Draw(0x4A043A,0x4A043F);
            if (!s.source.LoadButtonAt480930(receiver,button,&profile.pageButtons[i],0)) throw 8;
            button = Resolve<Button>(b.pageButtonsAt5C,i);
            button->pixelHitAt08 = 0;
            button->hitRectAt0C = {};
        }
    }
    b.perPageAt70 = profile.itemsPerPage;
    b.categoryAt74 = category;
    auto* root = Root(0x4A04DD,0x4A04E2);
    auto* scenes = Resolve<SceneManagerNativeBody48h>(s.source.RootScenesSlotAt30(root));
    Assign(b.selectorAt78,BorrowOptionCharDialogNativeAt478A30(*scenes));
    b.common.skippedGuiAt40 = 1;
    root = Root(0x4A0523,0x4A0528);
    scenes = Resolve<SceneManagerNativeBody48h>(s.source.RootScenesSlotAt30(root));
    Assign(b.common.publicGuiAt44,BorrowModeButtonsNativeAt47B7B0(*scenes));
    b.common.neighborsAt20 = profile.neighbors.data();
}
}
}
namespace KaGuYa::Start::Runtime
{
ReplayButtonNativeBody84h* ConstructReplayButtonNativeAt49F270(
    ReplayButtonNativeBody84h& body, int category, ReplayButtonNativeServices& services)
{
    using namespace ReplayButtonNativeDetail;
    ReplayButtonNativeScope scope(services);
    ::new(&body.common) ModeButtonNativeBody50h;
    ConstructModeButtonNativeAt4A2E60(body.common,services.mode);
    body.pagesAt50 = {};
    ::new(&body.pageButtonsAt5C) Slot{};
    ::new(&body.selectorAt78) Slot{};
    body.common.vptrAt00 = reinterpret_cast<const ModeButtonNativeVtable13*>(&ReplayButtonNativeVtable());
    Frame frame(body,services);
    try
    {
        struct TryUnwind final
        {
            Frame& frame;
            bool active = true;
            ~TryUnwind() noexcept
            { if (active && frame.state >= 1 && frame.state <= 33) frame.UnwindTo(1); }
        } unwind{frame};
        Build(frame,category);
        unwind.active = false;
    }
    catch (...)
    {
        if (frame.state < 1 || frame.state > 33) throw;
        frame.state = 34;
        frame.state = 0;
        // 原catch只清children，4A8490自身展开终止；不退页面/GUI、不重置晚写标量。
        [&]() noexcept { ClearModeButtonChildrenNativeAt4A8490(body.common); }();
    }
    frame.active = false;
    return &body;
}
}

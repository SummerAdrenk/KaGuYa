#include "CgButtonNative.Internal.h"
#include <Runtime/CgPageResourceProfile.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime::CgButtonNativeDetail
{
namespace
{
template<class F> F Virtual(void* p, unsigned offset)
{ return Read(Field<F>(Read(Field<void*>(p, 0)), offset)); }
SecretCgNativeArchive* Secret(EnvironmentRootStorage* root)
{
    auto* last = Read(root->archivesAt30.end);
    auto* first = Read(root->archivesAt30.begin);
    if (unsigned(Distance(first, last)) <= 3u) return nullptr;
    return Resolve<SecretCgNativeArchive>(*Advance(first, 3));
}
struct Frame final
{
    Body& body;
    CgButtonNativeServices& s;
    int state = 0;
    bool active = true;
    void* block = nullptr;
    void* raw74 = nullptr;
    void* raw88 = nullptr;
    void* raw144 = nullptr;
    void* rawD8 = nullptr;
    void* rawDC = nullptr;
    unsigned arrayCount = 2;
    Slot page, temporary, appendPage, off, click, focus, slash, button, image, work, clicked, focused;
    union { RuntimeCString name; };
    Frame(Body& b, CgButtonNativeServices& services) noexcept : body(b), s(services) {}
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
            case 0x59C360: DestroyDerived(body); break;
            case 0x59C368: s.mode.memory.DeleteBody(block, 0x24); break;
            case 0x59C377: DestroyRuntimeCStringAt40CD40(&Field<RuntimeCString>(block, 0x1C)); break;
            case 0x59C382: ReleaseSurface(page); break;
            case 0x59C38A: s.mode.memory.DeleteBody(raw74, 0x24); break;
            case 0x59C399: s.mode.memory.DeleteBody(raw88, 0x24); break;
            case 0x59C3AB: ReleaseSurface(temporary); break;
            case 0x59C3B3: ReleaseSurface(appendPage); break;
            case 0x59C3BE: ReleaseSurface(off); break;
            case 0x59C3C9: ReleaseSurface(click); break;
            case 0x59C3D4: ReleaseSurface(focus); break;
            case 0x59C3DF: ReleaseSurface(slash); break;
            case 0x59C3EA: DestroyRuntimeCStringAt40CD40(&name); break;
            case 0x59C3F2: case 0x59C417: s.environmentLock.Leave(); break;
            case 0x59C3FD: s.mode.memory.DeleteBody(raw144, 0x6C); break;
            case 0x59C40F: ReleaseYtlReference(button); break;
            case 0x59C422: ReleaseSurface(image); break;
            case 0x59C42A: s.mode.memory.DeleteBody(rawD8, 0x24); break;
            case 0x59C43C: s.mode.memory.DeleteBody(rawDC, 0x24); break;
            case 0x59C44E: ReleaseSurface(work); break;
            case 0x59C456: ReleaseSurface(clicked); break;
            case 0x59C461: ReleaseSurface(focused); break;
            case 0x59C46C:
            {
                const auto bytes = arrayCount > (0xFFFFFFFFu - 4u) / 0x6Cu ? 0xFFFFFFFFu : 4u + arrayCount * 0x6Cu;
                ::operator delete[](rawD8, std::size_t{bytes});
                break;
            }
            default: std::terminate(); // 只有已核验的38条原FI合法。
            }
            state = entry.previous;
        }
    }
    Surface* NewSurface(void*& raw, int allocationState, int previous)
    {
        raw = s.mode.memory.AllocateBody(0x24);
        state = allocationState;
        // 共享40C2E0内含CString资格及432670失败退役，等价原state3→4→3；不双清name。
        auto* p = raw ? ::new(raw) Surface(s.surfaces, s.strings) : nullptr;
        state = previous;
        return p;
    }
    EnvironmentRootStorage* InlineEnvironment(int lockedState, int leaveState)
    {
        s.environmentLock.Enter();
        state = lockedState;
        auto& publication = s.source.EnvironmentPublicationAt6198C8(lockedState == 18 ? 0x49A608 : 0x49A9BA);
        if (!Read(publication))
        {
            // 49A619/49A9CB内联new60h：两串建完才发布，失败只解锁，不补raw/CString回收。
            auto* raw = s.mode.memory.AllocateBody(0x60);
            auto* p = static_cast<EnvironmentRootStorage*>(raw);
            if (p)
            {
                InitializeEmptyRuntimeCString(p->titleAt10, s.strings);
                InitializeEmptyRuntimeCString(p->appendTitleAt14, s.strings);
                p->archivesAt30 = {};
                p->errorsAt3C = {};
                ::new(&p->pathsAt48) Slot{};
                ::new(&p->versionAt54) Slot{};
            }
            s.source.EnvironmentPublicationAt6198C8(lockedState == 18 ? 0x49A6C0 : 0x49AA72) = p;
        }
        state = leaveState;
        s.environmentLock.Leave();
        // caller第1处保留19到item Ref释放完成，第2处立即回到17。
        return Read(s.source.EnvironmentPublicationAt6198C8(lockedState == 18 ? 0x49A6E1 : 0x49AA93));
    }
};
struct DigitReference final
{
    Slot value;
    ~DigitReference() noexcept { ReleaseSurface(value); }
};
bool DrawDigits(int value, Surface* destination, bool percent, const int* x, int y)
{
    auto& s = RequireCgButtonNativeServices();
    const auto& profile=CurrentCgPageResourceProfile();
    const auto* names = percent ? profile.percentDigits.data() : profile.digits.data();
    const auto firstSite = percent ? 0x4BC9BBu : 0x4B4D8Bu;
    const auto secondSite = percent ? 0x4BCA9Fu : 0x4B4E6Cu;
    const auto thirdSite = percent ? 0x4BCB71u : 0x4B4F30u;
    try
    {
        const auto tens = value / 10;
        {
            DigitReference digit;
            if (!LoadGuiControlSurfaceReferenceAt480000(digit.value, Draw(firstSite, percent ? 0x4BC9C0 : 0x4B4D90), names[value % 10], s.pictures, s.surfaces, s.strings)) throw 1;
            Blit(destination, Resolve<Surface>(digit.value), x[0], percent ? profile.percentPositions[0].y : y);
        }
        if (tens)
        {
            DigitReference digit;
            if (!LoadGuiControlSurfaceReferenceAt480000(digit.value, Draw(secondSite, percent ? 0x4BCAA4 : 0x4B4E71), names[tens % 10], s.pictures, s.surfaces, s.strings)) throw 1;
            Blit(destination, Resolve<Surface>(digit.value), x[1], percent ? profile.percentPositions[1].y : y);
        }
        if (tens / 10)
        {
            DigitReference digit;
            // 4BCBA0完成率第三位固定资源1；4B4F29条目第三位取余数，不截断数值。
            const auto d = percent ? 1 : (tens / 10) % 10;
            if (!LoadGuiControlSurfaceReferenceAt480000(digit.value, Draw(thirdSite, percent ? 0x4BCB76 : 0x4B4F35), names[d], s.pictures, s.surfaces, s.strings)) throw 1;
            Blit(destination, Resolve<Surface>(digit.value), x[2], percent ? profile.percentPositions[2].y : y);
        }
        return true;
    }
    catch (...) { return false; } // 4BCBF0/4B4FAA及原PE catch-all；引用清理异常仍terminate。
}
int CategoryCount(SecretCgNativeArchive& p)
{
    auto* end = Read(p.mappingsAt18.end);
    const auto bytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(end) -
        reinterpret_cast<std::uintptr_t>(Read(p.mappingsAt18.begin)));
    return std::bit_cast<int>(bytes) >> 4; // 原499EBD/49A73B为SAR4，不是截零IDIV16。
}
void Numbers(Body& body, int pageIndex, int cell, int open, int total)
{
    if (unsigned(pageIndex) >= unsigned(Count(body.pagesAt50))) return;
    const auto& profile=CurrentCgPageResourceProfile();
    auto target = [&]() { return Resolve<Surface>(*Advance(Read(body.pagesAt50.begin), pageIndex)); };
    const int* openX;
    const int* totalX;
    int openError, totalError;
    if (open < 10) { openX = profile.oneDigitX[cell].data(); openError = 4; }
    else if (open < 100) { openX = profile.twoDigitX[cell].data(); openError = 5; }
    else { openX = profile.threeDigitX[cell].data(); openError = 9; }
    if (!DrawDigits(open, target(), false, openX, profile.counterY[cell][0])) throw openError;
    if (total < 10) { totalX = profile.oneDigitX[cell].data() + 1; totalError = 6; }
    else if (total < 100) { totalX = profile.twoDigitX[cell].data() + 2; totalError = 7; }
    else { totalX = profile.threeDigitX[cell].data() + 3; totalError = 10; }
    if (!DrawDigits(total, target(), false, totalX, profile.counterY[cell][1])) throw totalError;
}
void Build(Frame& f, int category)
{
    auto& b = f.body;
    auto& s = f.s;
    const auto& profile=CurrentCgPageResourceProfile();
    if(static_cast<unsigned>(category)>=profile.categories.size())
        throw std::out_of_range("CG category is outside the audited background table");
    const auto& background=profile.categories[category];
    f.state = 2;
    ReleaseGuiControlSurfaceRangeAt43EA10(Read(b.pagesAt50.begin), Read(b.pagesAt50.end));
    b.pagesAt50.end = Read(b.pagesAt50.begin);
    f.state = 1;
    auto* secret = Secret(s.source.CaptureEnvironmentAt4180C0(0x499E67));
    int itemCount = -1;
    if (category < CategoryCount(*secret))
        itemCount = Count(Advance(Read(secret->mappingsAt18.begin), category)->referencesAt04);
    const auto quotient = itemCount / 9;
    const auto pageCount = Add(quotient, itemCount != Mul(quotient, 9) ? 1 : 0);
    const char* backgroundName=background.normal;
    if (background.characterDisplayIndex >= 0)
    {
        auto* system = System(0x499F10);
        using Availability = const std::uint8_t* (__thiscall*)(void*, int);
        const bool unavailable=!Read(*Virtual<Availability>(system, 0x84)(system, background.characterDisplayIndex));
        // Mamatomo原499F10分支允许空locked槽回到normal；不用于装载失败兜底。
        if(unavailable && background.unavailable)backgroundName=background.unavailable;
    }
    for (int i = 0; i < pageCount; i = Add(i, 1))
    {
        auto* surface = f.NewSurface(f.block, 3, 1);
        ConstructGraphicsSurfaceReferenceAt451500(f.page, surface, 1);
        f.state = 5;
        auto* receiver = Draw(0x49A040, 0x49A045);
        const auto* name = backgroundName;
        if (!Read(f.page.baseAt00)) AdoptGraphicsSurfaceAt407A80(f.page, f.NewSurface(f.raw74, 6, 5));
        if (!s.pictures.LoadIntoAt4800A0(receiver, *Resolve<Surface>(f.page), name)) throw 1;
        if (pageCount > 1)
        {
            if (unsigned(i) >= 3u) throw 1;
            auto* mark = f.NewSurface(f.raw88, 7, 5);
            ConstructGraphicsSurfaceReferenceAt451500(f.temporary, mark, 1);
            f.state = 8;
            if (!LoadGuiControlSurfaceReferenceAt480000(f.temporary, Draw(0x49A139, 0x49A13E), profile.pageMarks[i], s.pictures, s.surfaces, s.strings)) throw 1;
            const auto x = Read(Resolve<Surface>(f.temporary)->originXAt10);
            const auto y = Read(Resolve<Surface>(f.temporary)->originYAt14);
            Blit(Resolve<Surface>(f.page), Resolve<Surface>(f.temporary), x, y);
            f.state = 9;
            ReleaseSurface(f.temporary);
        }
        f.appendPage = Copy(f.page);
        f.state = 10;
        AppendGuiControlSurfaceReference(b.pagesAt50, f.appendPage);
        f.state = 11;
        ReleaseSurface(f.appendPage);
        f.state = 12;
        ReleaseSurface(f.page);
        f.state = 1;
    }
    b.pageAt6C = 0;
    secret = Secret(s.source.CaptureEnvironmentAt4180C0(0x49A308));
    using Percent = int (__thiscall*)(void*, int);
    const auto value = Virtual<Percent>(secret, 0x24)(secret, category);
    const int percentX[3]{profile.percentPositions[0].x, profile.percentPositions[1].x, profile.percentPositions[2].x};
    for (int i = 0; unsigned(i) < unsigned(Count(b.pagesAt50)); i = Add(i, 1))
        if (!DrawDigits(value, Resolve<Surface>(*Advance(Read(b.pagesAt50.begin), i)), true, percentX, 0)) throw 2;
    f.off = {}; f.click = {}; f.focus = {};
    f.state = 15;
    if (!LoadGuiControlSurfaceReferenceAt480000(f.click, Draw(0x49A425, 0x49A42A), profile.stamps[0], s.pictures, s.surfaces, s.strings)) throw 1;
    if (!LoadGuiControlSurfaceReferenceAt480000(f.focus, Draw(0x49A470, 0x49A475), profile.stamps[1], s.pictures, s.surfaces, s.strings)) throw 1;
    if (!LoadGuiControlSurfaceReferenceAt480000(f.off, Draw(0x49A4BB, 0x49A4C0), profile.stamps[2], s.pictures, s.surfaces, s.strings)) throw 1;
    f.slash = {}; f.state = 16;
    if (!LoadGuiControlSurfaceReferenceAt480000(f.slash, Draw(0x49A528, 0x49A52D), profile.stamps[3], s.pictures, s.surfaces, s.strings)) throw 1;
    InitializeEmptyRuntimeCString(f.name, s.strings);
    f.state = 17;
    const auto width = Read(Resolve<Surface>(f.click)->widthAt08);
    const auto height = Read(Resolve<Surface>(f.click)->heightAt0C);
    b.common.itemCountAt3C = 0;
    for (int i = 0; unsigned(i) < 27u; i = Add(i, 1))
    {
        secret = Secret(f.InlineEnvironment(18, 19));
        if (category >= CategoryCount(*secret)) break;
        auto& items = Advance(Read(secret->mappingsAt18.begin), category)->referencesAt04;
        if (i >= Count(items)) break;
        auto item = Copy(*Advance(Read(items.begin), i));
        const auto total = Count(item);
        int open = 0;
        auto* records = total > 0 ? Read(secret->recordsAt0C.begin) : nullptr;
        for (int j = 0; j < total; j = Add(j, 1))
        {
            const auto index = Read(*Resolve<int>(item, j));
            auto* flag = Resolve<SecretCgFlagRecord>(*Advance(records, index));
            if (Read(flag->byteFlagAt04)) open = Add(open, 1);
        }
        f.state = 20;
        ReleaseYtlReference(item);
        f.state = 17;
        f.raw144 = s.mode.memory.AllocateBody(0x6C);
        f.state = 21;
        auto* created = f.raw144 ? ConstructCgUiButtonNativeAt459530(*::new(f.raw144) Button) : nullptr;
        f.state = 17;
        AdoptButton(f.button, created);
        f.state = 22;
        AppendChild(b.common.childrenAt14, f.button);
        f.state = 23;
        ReleaseYtlReference(f.button);
        f.state = 17;
        auto* button = Child(b, Sub(Count(b.common.childrenAt14), 1));
        secret = Secret(f.InlineEnvironment(24, 25));
        f.state = 17;
        using Thumbnail = const char* const* (__thiscall*)(void*, int, int);
        const auto* resource = Virtual<Thumbnail>(secret, 0x1C)(secret, category, Read(b.common.itemCountAt3C));
        const auto page = i / 9;
        const auto cell = i % 9;
        const auto position = profile.positions[cell];
        if (resource)
        {
            f.image = {}; f.state = 26;
            auto* receiver = Draw(0x49AB56, 0x49AB5B);
            const auto* resourceName = Read(*resource);
            auto* image = f.NewSurface(f.rawD8, 27, 26);
            AdoptGraphicsSurfaceAt407A80(f.image, image);
            if (!s.pictures.LoadIntoAt4800A0(receiver, *Resolve<Surface>(f.image), resourceName)) throw 1;
            auto* work = f.NewSurface(f.rawDC, 28, 26);
            ConstructGraphicsSurfaceReferenceAt451500(f.work, work, 1);
            f.state = 29;
            if (!Resolve<Surface>(f.work)->CreateAt415010(s.surfaces, width, height, 1, 1)) throw 1;
            Blit(Resolve<Surface>(f.work), Resolve<Surface>(f.image), profile.imageInset.x, profile.imageInset.y);
            FormatRuntimeCStringAt417130(f.name, profile.generatedNames[0], category, i);
            if (!AttachCgUiButtonSurfaceNativeAt4C1F20(*button, 0, f.name.GetString(), f.work, s.mode.buttons)) throw 2;
            CloneGraphicsSurfaceAt444470(f.clicked, *Resolve<Surface>(f.work), 1, s.surfaces, s.copy, s.strings);
            f.state = 30;
            Blit(Resolve<Surface>(f.clicked), Resolve<Surface>(f.click), 0, 0);
            FormatRuntimeCStringAt417130(f.name, profile.generatedNames[1], category, i);
            if (!AttachCgUiButtonSurfaceNativeAt4C1F20(*button, 2, f.name.GetString(), f.clicked, s.mode.buttons)) throw 2;
            f.state = 29;
            ReleaseSurface(f.clicked);
            CloneGraphicsSurfaceAt444470(f.focused, *Resolve<Surface>(f.work), 1, s.surfaces, s.copy, s.strings);
            f.state = 31;
            Blit(Resolve<Surface>(f.focused), Resolve<Surface>(f.focus), 0, 0);
            FormatRuntimeCStringAt417130(f.name, profile.generatedNames[2], category, i);
            if (!AttachCgUiButtonSurfaceNativeAt4C1F20(*button, 1, f.name.GetString(), f.focused, s.mode.buttons)) throw 2;
            ReleaseSurface(f.focused);
            button->positionAt24 = {Sub(position.x, profile.buttonOffset.x), Sub(position.y, profile.buttonOffset.y)};
            button->useNaturalAt2C = 0; button->stateAt04 = 0; button->latchAt20 = 0;
            ReleaseSurface(f.work);
            f.state = 17;
            ReleaseSurface(f.image);
        }
        else
        {
            // 原空缩略资源仍已创建主按钮；直接叠off图，不附状态图。
            auto* target = Resolve<Surface>(*Advance(Read(b.pagesAt50.begin), page));
            Blit(target, Resolve<Surface>(f.off), position.x, position.y);
        }
        Numbers(b, page, cell, open, total);
        if (unsigned(page) < unsigned(Count(b.pagesAt50)) && Read(f.slash.baseAt00))
            Blit(Resolve<Surface>(*Advance(Read(b.pagesAt50.begin), page)), Resolve<Surface>(f.slash), profile.slashes[cell].x, profile.slashes[cell].y);
        b.common.itemCountAt3C = Add(Read(b.common.itemCountAt3C), 1);
    }
    f.state = 16;
    DestroyRuntimeCStringAt40CD40(&f.name);
    f.state = 32; ReleaseSurface(f.slash);
    f.state = 33; ReleaseSurface(f.focus);
    f.state = 34; ReleaseSurface(f.click);
    f.state = 35; ReleaseSurface(f.off);
    f.state = 1;
    if (unsigned(Count(b.pagesAt50)) >= 2u)
    {
        f.arrayCount = 2;
        f.rawD8 = ::operator new[](std::size_t{0xDC});
        f.state = 36;
        Button* array = nullptr;
        if (f.rawD8)
        {
            *static_cast<unsigned*>(f.rawD8) = 2;
            array = reinterpret_cast<Button*>(static_cast<unsigned*>(f.rawD8) + 1);
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
        AdoptPageArray(b.pageButtonsAt5C, array, 2);
        for (int i = 0; i < Count(b.pageButtonsAt5C); i = Add(i, 1))
        {
            auto* button = Resolve<Button>(b.pageButtonsAt5C, i);
            auto* receiver = Draw(0x49B20E, 0x49B213);
            if (!s.source.LoadButtonAt480930(receiver, button, &profile.pageButtons[i], 0)) throw 8;
            button = Resolve<Button>(b.pageButtonsAt5C, i);
            button->pixelHitAt08 = 0;
            button->hitRectAt0C = {};
        }
    }
    b.perPageAt70 = 9;
    b.categoryAt74 = category;
    auto* root = Root(0x49B2B1, 0x49B2B6);
    auto* scenes = Resolve<SceneManagerNativeBody48h>(s.source.RootScenesSlotAt30(root));
    Assign(b.selectorAt78, BorrowOptionCharDialogNativeAt478A30(*scenes));
    b.common.skippedGuiAt40 = 0;
    root = Root(0x49B2F7, 0x49B2FC);
    scenes = Resolve<SceneManagerNativeBody48h>(s.source.RootScenesSlotAt30(root));
    Assign(b.common.publicGuiAt44, BorrowModeButtonsNativeAt47B7B0(*scenes));
    b.common.neighborsAt20 = profile.neighbors.data();
}
}
}
namespace KaGuYa::Start::Runtime
{
CgButtonNativeBody84h* ConstructCgButtonNativeAt499DC0(CgButtonNativeBody84h& b, int category, CgButtonNativeServices& s)
{
    using namespace CgButtonNativeDetail;
    CgButtonNativeScope scope(s);
    ::new(&b.common) ModeButtonNativeBody50h;
    ConstructModeButtonNativeAt4A2E60(b.common, s.mode);
    b.pagesAt50 = {};
    ::new(&b.pageButtonsAt5C) Slot{};
    ::new(&b.selectorAt78) Slot{};
    b.common.vptrAt00 = reinterpret_cast<const ModeButtonNativeVtable13*>(&CgButtonNativeVtable());
    Frame frame(b, s);
    try
    {
        struct TryUnwind final
        {
            Frame& frame;
            bool active = true;
            ~TryUnwind() noexcept
            {
                // 原FI先展开局部，再进入catch；不能在catch正文内模拟释放而丢失EH深度。
                if (active && frame.state >= 1 && frame.state <= 36) frame.UnwindTo(1);
            }
        } unwind{frame};
        Build(frame, category);
        unwind.active = false;
    }
    catch (...)
    {
        // 5EFE6C仅[1,36]有catch；terminate state不吞。先局部unwind到1，再catch37→0。
        if (frame.state < 1 || frame.state > 36) throw;
        frame.state = 37;
        frame.state = 0;
        // 原4A8490自有terminate unwind；共享可抛范围口在此保留该调用资格。
        [&]() noexcept { ClearModeButtonChildrenNativeAt4A8490(b.common); }();
    }
    frame.active = false;
    return &b;
}
}

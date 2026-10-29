#include "CgDisplayResourceNative.Internal.h"
#include <mbstring.h>

namespace KaGuYa::Start::Runtime::CgDisplayResourceNativeDetail
{
namespace
{
struct Frame final
{
    CgDisplayResourceNativeServices& services;
    int state = 0;
    unsigned groupFlags = 0;
    union { Array plan; };
    union { Array group; };
    union { ArchiveReaderReference reader; };
    union { RuntimeCString physical; };
    Scanner scanner;
    std::uintptr_t groupBase = 0;
    YtlReferenceControlRuntime* groupControl = nullptr;
    explicit Frame(CgDisplayResourceNativeServices& s) noexcept : services(s) {}
    ~Frame() noexcept {}
    void RetireReader() noexcept { reader.Reset(); } // 40D750及4638A0的EHFlags均为5。
    void Unwind() noexcept
    {
        while (state >= 0)
        {
            const auto entry = unwind[state];
            switch (entry.action)
            {
            case 0: break;
            case 0x541E55: std::terminate();
            case 0x59DD00: plan.~Array(); break;
            case 0x59DD08: DestroyScanner(scanner); break;
            case 0x59DD10:
                if (groupFlags & 1u) { groupFlags &= ~1u; group.~Array(); }
                break;
            case 0x59DD3F: DestroyRuntimeCStringAt40CD40(&physical); break;
            case 0x59DD5D: case 0x59DD68: case 0x59DD29: case 0x59DD34:
            case 0x59DD47: case 0x59DD52: RetireReader(); break;
            default: std::terminate();
            }
            state = entry.previous;
        }
    }
    void Open(const char* name, int opening, int completed, unsigned site)
    {
        ::new(&reader) ArchiveReaderReference;
        state = opening;
        services.source.OpenReaderAt483D90(reader, name, site);
        state = completed;
    }
    bool HasSuffix(const char* name)
    {
        ConstructScanner(scanner, name, services.strings);
        state = 2;
        union Text { RuntimeCString value; Text() noexcept {} ~Text() noexcept {} } suffix;
        ConstructArchiveResourceNameAt40B530(&suffix.value, Chars(scanner.textAt04), services.strings);
        state = 1; // 原4A869A撤scanner资格；suffix并无外层EH清理资格。
        ReleaseScanner(scanner);
        const auto result = Length(suffix.value) != 0;
        DestroyRuntimeCStringAt40CD40(&suffix.value);
        return result;
    }
    void FinishGroup()
    {
        groupFlags &= ~1u;
        state = 11;
        ReleaseCached(groupControl);
        if (groupControl)
        { group.RawSlot().controlAt08 = nullptr; group.RawSlot().baseAt00 = 0; }
        state = 1;
    }
};
bool Equal(const char* first, const char* second)
{ return ::_mbsicmp(reinterpret_cast<const unsigned char*>(first), reinterpret_cast<const unsigned char*>(second)) == 0; }
RuntimeCString& GroupName(Frame& f, int member) noexcept
{
    // 原EBX为Clear之前base，EDI为Clear之后control；bias及stride每次回读。
    const auto stride = f.groupControl ? Read(static_cast<EnvironmentObjectReferenceControl*>(f.groupControl)->strideAt0C) : 0u;
    auto* p = f.groupBase ? reinterpret_cast<RuntimeCString*>(f.groupBase +
        (Read(f.group.RawSlot().biasAt04) + std::uint32_t(member)) * stride) : nullptr;
    return *p;
}
void ClearWindow(CgDisplayWindowNativeBody60h& window, int index)
{
    // 原4AA5D0含signed index分支：负数清全数组，不把它简化成单个索引。
    if (index >= Count(window.spritesAt24)) return;
    if (index < 0)
    {
        for (unsigned i = 0; i < CgDisplayWindowSpriteCountAt4ACF90(window); ++i)
            ClearSpriteNativeAt43F140(*Resolve<Sprite>(window.spritesAt24, std::bit_cast<int>(i)));
    }
    else ClearSpriteNativeAt43F140(*Resolve<Sprite>(window.spritesAt24, index));
}
void Direct(Frame& f, Scene& scene, int index)
{
    f.Open(Chars(Name(f.plan, index)), 3, 4, 0x4A8D30);
    if (!Read(f.reader.Slot().baseAt00))
        ThrowRuntimeCStringConcatAt466760(Name(f.plan, index), readFailure, f.services.strings);
    // 原reader成功后才固定Window；Environment回调后仍沿该Window解析Slot。
    auto* window = Display(scene);
    const auto initialization = Initialization(f.services, 0x4A8D73);
    auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(window->spritesAt24.controlAt08));
    bool loaded = false;
    if (control && std::uint32_t(index) < Read(control->countAt10))
        loaded = f.services.source.LoadSpriteAt480650(*Resolve<Sprite>(window->spritesAt24, index),
            f.reader, initialization, 0, 0x4A8DBD);
    if (!loaded) ThrowRuntimeCStringConcatAt466760(Name(f.plan, index), loadFailure, f.services.strings);
    f.state = 1;
    f.RetireReader();
}
void FileGroup(Frame& f, Scene& scene, int index)
{
    f.groupBase = Read(f.group.RawSlot().baseAt00);
    auto* receiver = At(scene, index);
    ClearSpriteNativeAt43F140(*receiver);
    f.groupControl = Read(f.group.RawSlot().controlAt08);
    for (int member = 0;; member = Add(member, 1))
    {
        const auto count = f.groupControl ? std::bit_cast<int>(Read(
            static_cast<EnvironmentObjectReferenceControl*>(f.groupControl)->countAt10)) : 0;
        if (member >= count) break;
        bool changed = true;
        if (!member)
        {
            const auto* desired = Chars(GroupName(f, member));
            if (!desired) throw ATL::CAtlException(E_FAIL);
            changed = !Equal(Chars(receiver->primaryNameAt14), desired);
        }
        else
        {
            auto* begin = Read(receiver->memberNamesAt18.begin);
            auto* end = Read(receiver->memberNamesAt18.end);
            const auto countBytes = std::uint32_t(reinterpret_cast<std::uintptr_t>(end) - reinterpret_cast<std::uintptr_t>(begin));
            if ((std::bit_cast<int>(countBytes) >> 2) >= member)
            {
                const auto* desired = Chars(GroupName(f, member));
                if (!desired) throw ATL::CAtlException(E_FAIL);
                changed = !Equal(Chars(*Advance(begin, member - 1)), desired);
            }
        }
        if (!changed) continue;
        f.Open(Chars(GroupName(f, member)), 6, 7, 0x4A8942);
        if (!Read(f.reader.Slot().baseAt00))
            ThrowRuntimeCStringConcatAt466760(Name(f.group, member), readFailure, f.services.strings);
        const auto initialization = Initialization(f.services, 0x4A8958);
        if (!f.services.source.LoadSpriteMemberAt4B0840(*receiver, member, f.reader, initialization, 0x4A8976))
            ThrowRuntimeCStringConcatAt466760(Name(f.group, member), loadFailure, f.services.strings);
        f.state = 5;
        f.RetireReader();
    }
    AssignRuntimeCStringAt410220(receiver->logicalNameAt0C, Name(f.plan, index));
    receiver->activeAt08 = 1;
}
void ExcPosition(Frame& f, Scene& scene, int index)
{
    InitializeEmptyRuntimeCString(f.physical, f.services.strings);
    f.state = 8;
    const auto* logical = Chars(Name(f.plan, index));
    const auto* pattern = Pattern(f.services, 0x4A8A82);
    const auto found = FindCgDisplayPatternNameNativeAt47A020(*pattern, logical);
    bool matched = false;
    if (found >= 0)
    {
        auto* body = Resolve<void>(*Advance(Read(pattern->itemsAt04.begin), found));
        if (Read(Field<unsigned>(body, 0)) == 2)
        {
            body = Resolve<void>(*Advance(Read(pattern->itemsAt04.begin), found));
            AssignRuntimeCStringAt410220(f.physical, Field<RuntimeCString>(body, 8));
            matched = true;
        }
    }
    if (!matched) ThrowRuntimeCStringConcatAt466760(Name(f.plan, index), missingResource, f.services.strings);
    // Assign可重入，随后在同一Pattern重新解析条目，不重新捕获根/重验kind。
    auto* body = Resolve<void>(*Advance(Read(pattern->itemsAt04.begin), found));
    const auto x = Read(Field<int>(body, 0x0C));
    const auto y = Read(Field<int>(body, 0x10));
    const auto* physical = Chars(f.physical);
    auto* receiver = At(scene, index); // 4AD510固定正文，之后Load不沿Scene重解。
    f.Open(physical, 9, 10, 0x4A8BCD);
    if (!Read(f.reader.Slot().baseAt00))
        ThrowRuntimeCStringConcatAt466760(f.physical, readFailure, f.services.strings);
    const auto initialization = Initialization(f.services, 0x4A8BE3);
    if (!f.services.source.LoadSpriteAt480650(*receiver, f.reader, initialization, 0, 0x4A8C01))
        ThrowRuntimeCStringConcatAt466760(f.physical, loadFailure, f.services.strings);
    receiver->xAt2C = x;
    receiver->yAt30 = y;
    AssignRuntimeCStringAt410220(receiver->logicalNameAt0C, Name(f.plan, index));
    // 原Exc正常尾仍在state10，物理名Free失败会再走reader/physical的EH叶。
    f.RetireReader();
    DestroyRuntimeCStringAt40CD40(&f.physical);
}
void WithoutSuffix(Frame& f, Scene& scene, int index)
{
    const auto* logical = Chars(Name(f.plan, index));
    const auto* pattern = Pattern(f.services, 0x4A8734);
    ::new(&f.group) Array;
    f.groupControl = nullptr;
    f.groupFlags |= 1u;
    f.state = 5;
    const auto found = FindCgDisplayPatternNameNativeAt47A020(*pattern, logical);
    if (found >= 0)
    {
        auto* item = Resolve<void>(*Advance(Read(pattern->itemsAt04.begin), found));
        if (Read(Field<unsigned>(item, 0)) == 1)
        {
            item = Resolve<void>(*Advance(Read(pattern->itemsAt04.begin), found));
            f.group = Field<Array>(item, 8);
        }
    }
    if (Read(f.group.RawSlot().baseAt00)) FileGroup(f, scene, index);
    else
    {
        f.groupControl = Read(f.group.RawSlot().controlAt08); // 原4A8A2A仅base空路径才在此保存EDI。
        ExcPosition(f, scene, index);
    }
    f.FinishGroup();
}
void Execute(Frame& f, Scene& scene)
{
    const auto selected = Read(scene.selectionAt18.selectedPairAt10);
    auto* pair = Advance(Read(scene.selectionAt18.pairsAt14.begin), selected);
    const auto* pattern = Pattern(f.services, 0x4A852A);
    BuildCgDisplayNamesNativeAt4AD900(f.plan, *pattern, Read(pair->resourceIdAt04), f.services.strings);
    f.state = 1;
    for (unsigned slot = 0;; ++slot)
    {
        auto* window = Display(scene);
        auto* control = static_cast<EnvironmentObjectReferenceControl*>(Read(window->spritesAt24.controlAt08));
        if (!control || slot >= Read(control->countAt10)) break;
        const auto index = std::bit_cast<int>(slot);
        auto* names = static_cast<EnvironmentObjectReferenceControl*>(Read(f.plan.RawSlot().controlAt08));
        if (!names || slot >= Read(names->countAt10) || !Length(Name(f.plan, index)))
        { ClearWindow(*Display(scene), index); continue; }
        const auto* current = Chars(At(scene, index)->logicalNameAt0C);
        if (!current) throw ATL::CAtlException(E_FAIL);
        if (Equal(Chars(Name(f.plan, index)), current)) continue;
        if (f.HasSuffix(Chars(Name(f.plan, index)))) Direct(f, scene, index);
        else WithoutSuffix(f, scene, index);
    }
    Display(scene)->window.layer.enabledAt0C = 1;
    auto* operation = Resolve<CgOperationWindowNativeBody30h>(scene.operationWindowAtA8);
    const auto ordinal = Read(Advance(Read(scene.selectionAt18.pairsAt14.begin), selected)->ordinalAt00);
    operation->window.layer.enabledAt0C = 1;
    Resolve<CgOperationDialogNativeBodyA8h>(operation->dialogAt24)->selectionAt7C = ordinal;
    auto* dialog = Resolve<GuiDialogNativeBody5Ch>(operation->dialogAt24);
    Read(dialog->vptrAt00)->beginAt44(dialog);
    f.state = 12;
    ReleaseRetained(f.plan.RawSlot());
}
}
}
namespace KaGuYa::Start::Runtime
{
bool ExecuteCgDisplayResourceNativeAt4A84E0(CgSceneNativeBodyB4h& scene, CgDisplayResourceNativeServices& services)
{
    using namespace CgDisplayResourceNativeDetail;
    GraphicsSurfaceNativeScope surfaces(services.surfaces);
    CgOperationDialogNativeScope operation(services.operationDialog);
    Frame frame(services);
    try
    {
        struct Unwind final
        { Frame& frame; bool active = true; ~Unwind() noexcept { if (active) frame.Unwind(); } } cleanup{frame};
        Execute(frame, scene);
        cleanup.active = false;
        return true;
    }
    catch (...) { frame.state = 13; return false; } // 5F0188: try[0,12], catch13；不回滚已提交业务字段。
}
}

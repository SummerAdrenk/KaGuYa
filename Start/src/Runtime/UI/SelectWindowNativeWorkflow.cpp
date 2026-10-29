#include <Runtime/SelectWindowNativeWorkflow.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ScriptParamsNativeBody.h>
#include <Runtime/ScriptNativeBody.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = SelectWindowNativeBody6Ch;
using Slot = YtlReferenceSlotRuntime;
template<class T> T Read(const T& field) noexcept { return *static_cast<const volatile T*>(&field); }
template<class T> T* Resolve(const Slot& reference) noexcept
{
    const auto base = Read(reference.baseAt00);
    if (!base) return nullptr;
    auto* control = Read(reference.controlAt08);
    const auto stride = control ? Read(static_cast<EnvironmentObjectReferenceControl*>(control)->strideAt0C) : 0u;
    return reinterpret_cast<T*>(base + Read(reference.biasAt04) * stride);
}
GuiDialogNativeBody5Ch* Dialog(Body& body) noexcept { return Resolve<GuiDialogNativeBody5Ch>(body.dialogAt38); }
SaveDataEnvironmentArchive* Save(EnvironmentRootOwner& environment)
{ return static_cast<SaveDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*environment.CaptureAt4180C0(), 1)); }
SaveDataEnvironmentArchive* SaveInline(EnvironmentRootOwner& environment)
{
    const volatile auto& slots = environment.CaptureAt4180C0()->archivesAt30;
    const auto end = reinterpret_cast<std::uintptr_t>(slots.end);
    auto* const begin = slots.begin;
    const auto bytes = std::uint32_t(end - reinterpret_cast<std::uintptr_t>(begin));
    const auto count = std::uint32_t(std::bit_cast<std::int32_t>(bytes) / 12);
    return count <= 1U ? nullptr : Resolve<SaveDataEnvironmentArchive>(begin[1]);
}
std::uint32_t NormalColor(Body& body, EnvironmentRootOwner& environment)
{
    if (Read(body.normalColorAt48.baseAt00)) return *Resolve<std::uint32_t>(body.normalColorAt48);
    auto* save = Save(environment);
    std::uint32_t value;
    std::memcpy(&value, save->bytesAt60.data() + 4, sizeof(value));
    return value;
}
std::int32_t Candidate(const ScriptDwordVectorStorage& candidates, std::int32_t ordinal) noexcept
{
    const auto address = reinterpret_cast<std::uintptr_t>(Read(candidates.storage.begin)) + std::uint32_t(ordinal) * 4U;
    return std::bit_cast<std::int32_t>(*reinterpret_cast<const std::uint32_t*>(address));
}
void ClearName(Slot& name)
{
    auto* control = name.controlAt08;
    if (!control) return;
    if (--control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        name.controlAt08->DeleteControlAtVslot04(1); // 436C83/89不增加null门控。
    }
    name.controlAt08 = nullptr; name.baseAt00 = 0;
}
void ReleaseCaptured(YtlReferenceControlRuntime* control)
{
    if (control && --control->referenceCountAt04 == 0)
    {
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        control->DeleteControlAtVslot04(1);
    }
}
void ClearCaptured(Slot& slot, YtlReferenceControlRuntime* captured)
{
    if (!captured) return;
    ReleaseCaptured(captured);
    slot.controlAt08 = nullptr; slot.baseAt00 = 0;
}
struct FormatLifetime final
{
    Slot name{}, voices{}, body{};
    bool active = true;
    ~FormatLifetime() noexcept
    {
        if (!active) return;
        // 593B30/4100C0：展开用当前槽，和普通尾保存的EBX/ESI不同。
        ReleaseYtlReference(body); ReleaseYtlReference(voices); ReleaseYtlReference(name);
    }
    void Finish(YtlReferenceControlRuntime* text, YtlReferenceControlRuntime* voice) noexcept
    {
        active = false;
        ReleaseCaptured(text); ReleaseCaptured(voice); ReleaseCaptured(name.controlAt08);
    }
};
RuntimeCString* Text(void* command, RuntimeCString* output)
{
    using Function = RuntimeCString* (__thiscall*)(void*, RuntimeCString*);
    const std::byte* table;
    std::memcpy(&table, command, sizeof(table));
    Function function;
    std::memcpy(&function, table + 4, sizeof(function));
    return function(command, output);
}
void InitializeText(const Slot& command, GraphicsTextNativeApi& graphics)
{
    const char* input = nullptr;
    auto text = ArchiveRawResourceNameRuntime(ArchiveRawResourceNameRuntime::ProducerTag{},
        [&](RuntimeCString* output) { input = Text(Resolve<void>(command), output)->GetString(); });
    const bool result = graphics.InitializeText(input, 0, 0);
    text.ReleaseNormal(); // 先结束字符串EH，再释放；释放失败进入外层catch，不重试。
    if (!result) throw 1;
}
LONG Center(LONG begin, LONG end, LONG textBegin, LONG textEnd) noexcept
{
    const auto delta = std::uint32_t(end) + std::uint32_t(textBegin) - std::uint32_t(textEnd) - std::uint32_t(begin);
    return std::bit_cast<std::int32_t>(std::uint32_t(begin) + ((delta >> 1U) | (delta & 0x80000000U)));
}
}

void RebuildSelectWindowNativeAt4369E0(Body& body, SelectWindowNativeWorkflowServices& services)
{
    try
    {
        auto* dialog = Dialog(body);
        dialog->vptrAt00->beginAt44(dialog);
        auto* canvas = Resolve<GraphicsSurfaceNativeBody>(body.canvasAt2C);
        const auto handle = Read(canvas->handleAt04);
        if (handle) services.drawing.SurfaceClearRaw(handle, Read(canvas->clearColorAt20), nullptr);
        GraphicsTextNativeFontParameters font;
        InitializeGraphicsTextFontNativeAt40C6E0(font);
        font.fontAt00.lfHeight = -24;
        font.fontAt00.lfWeight = 700;
        ::lstrcpynA(font.fontAt00.lfFaceName, services.fontFaceAt619824, LF_FACESIZE);
        font.renderModeAt3E = 2;
        font.textColorAt40 = NormalColor(body, services.environment);
        font.outlineColorAt48 = 0xFFFFFFFFU; font.shadowColorAt44 = 0xFFFFFFFFU;
        font.shadowXAt4C = 0; font.shadowYAt50 = 0;
        if (Read(body.shadowColorAt60.baseAt00))
        {
            font.shadowColorAt44 = *Resolve<std::uint32_t>(body.shadowColorAt60);
            font.shadowXAt4C = 1; font.shadowYAt50 = 2;
        }
        font.characterSpacingAt3C = 0; font.lineSpacingAt3D = 0; font.verticalAt55 = 0;
        if (services.source.ReadMessageVersionAfter413BE0() < 4.0f)
            (void)services.text.CreateTextFontNativeAt4388B0(&font);
        FormatLifetime refs;
        YtlReferenceControlRuntime* textControl = nullptr;
        YtlReferenceControlRuntime* voiceControl = nullptr;
        const auto& candidates = services.source.CaptureParamsAfter413BE0()->menuCandidatesAt64.storage;
        const auto candidateEnd = reinterpret_cast<std::uintptr_t>(Read(candidates.end));
        const auto candidateBegin = reinterpret_cast<std::uintptr_t>(Read(candidates.begin));
        const auto bytes = std::uint32_t(candidateEnd - candidateBegin);
        const auto count = std::bit_cast<std::int32_t>(bytes) >> 2;
        for (std::int32_t ordinal = 0; ordinal < count; ++ordinal)
        {
            const auto id = Candidate(services.source.CaptureParamsAfter413BE0()->menuCandidatesAt64, ordinal);
            if (id == -1) continue;
            ClearName(refs.name);
            ClearCaptured(refs.body, textControl);
            ClearCaptured(refs.voices, voiceControl);
            MessageFormatSelectionRawTarget output{refs.name, refs.voices, refs.body};
            // COMISS方向不同：unordered不创建旧字体，但此处分流进入旧reader。
            if (!(services.source.ReadMessageVersionAfter413BE0() >= 4.0f))
            {
                if (services.source.ReadLegacyAfter413BE0(id, output) < 0 || !refs.name.baseAt00) throw 1;
                InitializeText(refs.name, services.text);
            }
            else
            {
                auto selected = services.source.ReadChoiceAfter413BE0(id, output);
                if (selected < 0) throw 1;
                bool allowSelected = true;
                if (services.source.CaptureScriptAfter413BE0()->commandAt24.bytes[4] != 0)
                {
                    if (std::uint32_t(ordinal) > 3U) allowSelected = false;
                    else selected = services.source.ReadProgramFlagAfterRoot(std::uint32_t(ordinal)) != 0;
                }
                if (allowSelected && selected != 0 && services.source.CaptureScriptAfter413BE0()->commandAt24.bytes[3] != 0 &&
                    Read(body.selectedColorAt54.baseAt00))
                    font.textColorAt40 = *Resolve<std::uint32_t>(body.selectedColorAt54);
                else font.textColorAt40 = NormalColor(body, services.environment);
                (void)services.text.CreateTextFontNativeAt4388B0(&font);
                InitializeText(refs.body, services.text);
            }
            RECT measured;
            const RECT textBounds = *services.text.ReadTextDrawRectNativeAt418A30(&measured);
            RECT visible;
            const RECT itemBounds = *SelectVisibleBoundsNativeAt50AD40(
                *Resolve<SelectDialogNativeBody88h>(body.dialogAt38), &visible, ordinal);
            services.text.SetTextBasePosition(Center(itemBounds.left, itemBounds.right, textBounds.left, textBounds.right),
                Center(itemBounds.top, itemBounds.bottom, textBounds.top, textBounds.bottom));
            for (std::int32_t glyph = 0; glyph < services.text.GetTextLength(); ++glyph)
            {
                const auto target = Read(Resolve<GraphicsSurfaceNativeBody>(body.canvasAt2C)->handleAt04);
                (void)services.text.BlitTextGlyphToSurface(target, std::uint32_t(glyph), nullptr);
            }
            voiceControl = refs.voices.controlAt08;
            textControl = refs.body.controlAt08;
        }
        refs.Finish(textControl, voiceControl);
        auto* parent = body.window.layer.parentAt10;
        parent->vptrAt00->addDirtyAt20(parent, &body.window.boundsAt14);
        body.savedByteAt44 = SaveInline(services.environment)->bytesAt60[0];
        body.savedByteAt45 = SaveInline(services.environment)->bytesAt60[1];
    }
    catch (...) {} // 4375BA原catch：保留已经画出的字形/dirty/第一个saved byte。
}

std::int32_t TickSelectWindowNativeAt4BBFD0(Body& body, std::uint8_t blocked,
    SelectWindowNativeWorkflowServices& services)
{
    std::int32_t result = -1;
    if (!blocked)
    {
        auto* mouse = services.source.CaptureMouseAt418210();
        if (Read(mouse->secondaryAt2C) == 0 && Read(mouse->wheelAt34) > 0)
        {
            auto* unsave = static_cast<UnSaveDataRuntime*>(
                ResolveEnvironmentArchiveAt417990(*services.environment.CaptureAt4180C0(), 2));
            unsave->FlagsAt1C() |= 2U;
        }
        else
        {
            auto* dialog = Dialog(body);
            if (dialog->vptrAt00->pollAt48(dialog) == 0)
            {
                dialog = Dialog(body);
                result = dialog->vptrAt00->resultAt50(dialog);
                const auto id = Candidate(services.source.CaptureParamsAfter413BE0()->menuCandidatesAt64, result);
                services.source.MarkChoiceAfter413BE0(id);
            }
            dialog = Dialog(body);
            auto* dirty = dialog->vptrAt00->dirtyAt00(dialog);
            dirty->vptrAt00->coalesceAt04(dirty);
            dialog = Dialog(body); dirty = dialog->vptrAt00->dirtyAt00(dialog);
            const auto count = std::bit_cast<std::int32_t>(dirty->vptrAt00->countAt18(dirty));
            if (count != 0)
            {
                dialog = Dialog(body); dirty = dialog->vptrAt00->dirtyAt00(dialog);
                auto* rectangle = dirty->vptrAt00->rectsAt1C(dirty);
                for (std::int32_t index = 0; index < count; ++index, ++rectangle)
                {
                    auto* parent = body.window.layer.parentAt10;
                    parent->vptrAt00->addDirtyAt20(parent, rectangle);
                }
                dialog = Dialog(body); dirty = dialog->vptrAt00->dirtyAt00(dialog);
                dirty->vptrAt00->clearAt00(dirty);
            }
        }
    }
    services.sound.CaptureRootAfter413BE0().Resolve().volume.Refresh();
    return result;
}
}

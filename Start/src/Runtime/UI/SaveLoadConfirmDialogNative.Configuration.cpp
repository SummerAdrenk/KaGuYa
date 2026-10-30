#include "SaveLoadConfirmDialogNative.Internal.h"
#include <Runtime/SaveLoadResourceProfile.h>
#include <stdexcept>
#include <cstring>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace SaveLoadConfirmNativeDetail
{
void ConstructText(TextStorage& storage, const char* source, int length)
{
    InitializeEmptyRuntimeCString(storage.value, RequireSaveLoadConfirmDialogNativeServices().strings);
    TextLifetime lifetime{storage.value};
    SetRuntimeCStringAt437680(storage.value, source, length);
    lifetime.armed = false;
}
void TrimLeft(RuntimeCString& value)
{
    const auto* begin = reinterpret_cast<const unsigned char*>(value.GetString());
    const auto* cursor = begin;
    while (*cursor && _mbschr(reinterpret_cast<const unsigned char*>(" \t"), *cursor)) cursor = _mbsinc(cursor);
    if (cursor == begin) return;
    const auto removed = static_cast<int>(cursor - begin);
    const auto oldLength = value.GetLength();
    PrepareRuntimeCStringWriteAt42D590(value, oldLength);
    auto* text = const_cast<char*>(value.GetString());
    const auto length = Sub(value.GetLength(), removed);
    (void)MoveRuntimeBytesAt43D9B0(text, std::uint32_t(value.GetLength()) + 1u,
        text + removed, std::uint32_t(length) + 1u);
    auto* header = reinterpret_cast<ATL::CStringData*>(text) - 1;
    if (length < 0 || length > header->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength = length;
    const_cast<char*>(value.GetString())[length] = 0;
}
void TrimRight(RuntimeCString& value)
{
    const auto* begin = reinterpret_cast<const unsigned char*>(value.GetString());
    const unsigned char* trailing = nullptr;
    for (auto* cursor = begin; *cursor; cursor = _mbsinc(cursor))
    {
        if (_mbschr(reinterpret_cast<const unsigned char*>(" \t"), *cursor))
        { if (!trailing) trailing = cursor; }
        else trailing = nullptr;
    }
    if (!trailing) return;
    const auto length = static_cast<int>(trailing - begin);
    auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(value.GetString())) - 1;
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    if (header->nRefs > 1 || header->nAllocLength < length) PrepareRuntimeCStringWriteAt42D590(value, length);
    auto* text = const_cast<char*>(value.GetString());
    header = reinterpret_cast<ATL::CStringData*>(text) - 1;
    if (length > header->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength = length;
    const_cast<char*>(value.GetString())[length] = 0;
}
}
namespace
{
using namespace SaveLoadConfirmNativeDetail;
void CloneArgument(TextStorage& storage, const RuntimeCString& source)
{
    auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(source.GetString())) - 1;
    const auto* chars = reinterpret_cast<const char*>(CloneRuntimeCStringDataAt414540(header) + 1);
    // 414540已取得引用；TextStorage为空槽，原生指针表示由TextLifetime接管，不经CString赋值。
    std::memcpy(static_cast<void*>(&storage.value), &chars, sizeof(chars));
}
void Blit(Body& body, std::int32_t sourceHandle, LONG x, LONG y)
{
    const auto destinationHandle = Surface(body.labelsAt90)->handleAt04;
    RequireSaveLoadConfirmDialogNativeServices().rawCopy.SurfaceCopyBlitRaw(
        destinationHandle, sourceHandle, x, y, nullptr, nullptr, nullptr, 0);
}
void DrawPage(Body& body, const char* page, std::uint8_t saveMode)
{
    const auto& profile = CurrentSaveLoadResourceProfile().confirmation;
    TextStorage text;
    ConstructText(text, page, static_cast<int>(std::strlen(page)));
    TextLifetime lifetime{text.value};
    TrimLeft(text.value); TrimRight(text.value);
    const auto* bytes = reinterpret_cast<const unsigned char*>(text.value.GetString());
    int special = -1;
    if (_mbscmp(bytes, reinterpret_cast<const unsigned char*>("Quick")) == 0) special = 0;
    else if (_mbscmp(bytes, reinterpret_cast<const unsigned char*>("Auto")) == 0) special = 1;
    if (special >= 0)
    {
        const auto handle = Surface(body.specialPagesAtD8.begin[special])->handleAt04;
        const auto point=profile.specialPageOrigins[special];
        Blit(body, handle, point.x, point.y);
        lifetime.Finish();
        return;
    }
    const auto positions=profile.digitPositions[1];
    const bool reversed=profile.kind==SaveLoadConfirmationKind::PairedDigitsWithSeparators;
    // 07的4DA870按正序四位消费；18的508110只取末三位，不能统一倒序或补位。
    const auto count=reversed?positions.size():static_cast<std::size_t>(text.value.GetLength());
    if(count>positions.size())throw std::out_of_range("save confirmation page digits exceed profile");
    for (std::size_t position = 0; position != count; ++position)
    {
        const auto character=reversed?text.value.GetLength()-1-static_cast<int>(position):static_cast<int>(position);
        Slot selected{};
        RefLifetime referenceLifetime{selected};
        if (character < 0 || character > text.value.GetLength()) throw ATL::CAtlException(E_INVALIDARG);
        const auto digit = text.value.GetString()[character];
        if (digit >= '0' && digit <= '9')
        {
            const auto index = digit - '0' + (saveMode || profile.kind==SaveLoadConfirmationKind::SingleThemeDigits ? 0 : 10);
            CopyAssignGraphicsSurfaceReferenceAt465870(selected, body.digitsAtA8[1].begin[index]);
        }
        auto* captured = selected.controlAt08;
        if (selected.baseAt00)
        {
            const auto handle = Surface(selected)->handleAt04;
            // 非数字同样推进坐标；不补齐短串，不改源字符串的末三字节。
            Blit(body, handle, positions[position].x, positions[position].y);
        }
        referenceLifetime.FinishCaptured(captured);
    }
    lifetime.Finish();
}
void DrawSlot(Body& body, const char* slot, std::uint8_t saveMode)
{
    const auto& profile=CurrentSaveLoadResourceProfile().confirmation;
    TextStorage text;
    ConstructText(text, slot, static_cast<int>(std::strlen(slot)));
    TextLifetime lifetime{text.value};
    TrimLeft(text.value); TrimRight(text.value);
    Slot selected{};
    RefLifetime referenceLifetime{selected};
    const auto character = Add(text.value.GetLength(), -1);
    if (character < 0 || character > text.value.GetLength()) throw ATL::CAtlException(E_INVALIDARG);
    const auto digit = text.value.GetString()[character];
    if (digit >= '0' && digit <= '9')
        CopyAssignGraphicsSurfaceReferenceAt465870(selected,
            body.digitsAtA8[0].begin[digit - '0' + (saveMode || profile.kind==SaveLoadConfirmationKind::SingleThemeDigits ? 0 : 10)]);
    auto* captured = selected.controlAt08;
    if (selected.baseAt00)
    {
        const auto point=profile.digitPositions[0].front();
        Blit(body, Surface(selected)->handleAt04, point.x, point.y);
    }
    referenceLifetime.FinishCaptured(captured, false);
    lifetime.Finish();
}
void DrawTimestampPart(Body& body, RuntimeCString& text, std::uint8_t saveMode,
    std::uint32_t group, char separator, std::span<const POINT> coordinates)
{
    const auto& profile=CurrentSaveLoadResourceProfile().confirmation;
    const bool separators=profile.kind==SaveLoadConfirmationKind::PairedDigitsWithSeparators;
    // 07的4DAE10（08同策略）时间复用日期图组，分隔符不占字位；新版分隔符有独立图。
    const auto actualGroup=group==3 && profile.kind==SaveLoadConfirmationKind::SingleThemeDigits ? 2u : group;
    std::size_t position=0;
    for (int index = 0; index < text.GetLength(); ++index)
    {
        Slot selected{};
        RefLifetime lifetime{selected};
        if (index < 0 || index > text.GetLength()) throw ATL::CAtlException(E_INVALIDARG);
        const auto digit = text.GetString()[index];
        int resource = -1;
        if (digit >= '0' && digit <= '9') resource = digit - '0';
        else if (digit == separator && separators) resource = 10;
        if (resource >= 0)
        {
            resource += saveMode || profile.kind==SaveLoadConfirmationKind::SingleThemeDigits ? 0 : separators ? 11 : 10;
            // 日期0、时间0..3原内联复制；其余465870对空槽赋值等效复制。
            if ((group == 2 && digit == '0') || (group == 3 && digit >= '0' && digit <= '3'))
                Copy(selected, body.digitsAtA8[actualGroup].begin[resource]);
            else CopyAssignGraphicsSurfaceReferenceAt465870(selected, body.digitsAtA8[actualGroup].begin[resource]);
        }
        auto* captured = selected.controlAt08;
        if (selected.baseAt00)
        {
            const auto handle = Surface(selected)->handleAt04;
            if(position>=coordinates.size())throw std::out_of_range("save confirmation timestamp exceeds profile");
            Blit(body, handle, coordinates[position].x, coordinates[position].y);
        }
        if(separators || digit!=separator)++position;
        if (group == 2) lifetime.Finish(true); // 507A08回调后重读当前槽control。
        else lifetime.FinishCaptured(captured); // 507E05固定EBX。
    }
}
void DrawTimestamp(Body& body, const char* timestamp, std::uint8_t saveMode)
{
    TextStorage text;
    ConstructText(text, timestamp, static_cast<int>(std::strlen(timestamp)));
    TextLifetime lifetime{text.value};
    TrimLeft(text.value); TrimRight(text.value);
    if (text.value.GetLength() < 0) { lifetime.Finish(); return; } // 50761C的原负长度分支。
    const auto* separator = _mbsstr(reinterpret_cast<const unsigned char*>(text.value.GetString()),
        reinterpret_cast<const unsigned char*>("\r\n"));
    if (separator)
    {
        const auto position = static_cast<int>(separator - reinterpret_cast<const unsigned char*>(text.value.GetString()));
        TextStorage date;
        ConstructRuntimeCStringLeftAt419D00(&date.value, text.value, position,
            RequireSaveLoadConfirmDialogNativeServices().strings);
        TextLifetime dateLifetime{date.value};
        TrimRight(date.value);
        DrawTimestampPart(body, date.value, saveMode, 2, '/', CurrentSaveLoadResourceProfile().confirmation.digitPositions[2]);
        dateLifetime.Finish();
        TextStorage time;
        const auto start = Add(position, 2);
        ConstructRuntimeCStringMidAt420C80(&time.value, text.value, start, Sub(text.value.GetLength(), start),
            RequireSaveLoadConfirmDialogNativeServices().strings);
        TextLifetime timeLifetime{time.value};
        TrimLeft(time.value); TrimRight(time.value);
        DrawTimestampPart(body, time.value, saveMode, 3, ':', CurrentSaveLoadResourceProfile().confirmation.digitPositions[3]);
        timeLifetime.Finish();
    }
    lifetime.Finish();
}
void DrawText(Body& body, const char* scene, unsigned role, std::uint8_t saveMode)
{
    auto& source = RequireSaveLoadConfirmDialogNativeServices().source;
    const auto& style=CurrentSaveLoadResourceProfile().confirmation.text[role];
    GraphicsTextNativeFontParameters font;
    InitializeGraphicsTextFontNativeAt40C6E0(font);
    font.fontAt00.lfHeight = style.height;
    font.fontAt00.lfWeight = style.weight;
    ::lstrcpynA(font.fontAt00.lfFaceName, style.face, 32);
    font.textColorAt40 = style.colors[saveMode?0:1];
    font.lineSpacingAt3D = style.lineSpacing;
    font.characterSpacingAt3C = style.characterSpacing;
    font.renderModeAt3E = style.renderMode;
    font.shadowColorAt44 = 0xFFFFFFFFu;
    font.shadowXAt4C = 0; font.shadowYAt50 = 0; font.verticalAt55 = 0;
    // 5F4AE8 -> 5F4AD4 -> 5F4AB4：原catch(...)只包DLL三叶，不吞标签解析异常。
    try
    {
        if (!source.CreateTextFontAt4388B0(&font)) throw 1;
        if (!source.TextAt4182F0().InitializeText(scene, style.origin.x, style.origin.y)) throw 1;
        const auto handle = Surface(body.labelsAt90)->handleAt04;
        if (!source.TextAt4182F0().BlitAllTextToSurface(handle, nullptr)) throw 1;
    }
    catch (...) {}
}
}
void ConfigureSaveLoadConfirmDialogNativeAt535FF0(GuiDialogNativeBody5Ch* common,
    RuntimeCString& page, RuntimeCString& slot, RuntimeCString& timestamp, RuntimeCString& scene,
    std::uint8_t saveMode)
{
    using namespace SaveLoadConfirmNativeDetail;
    TextLifetime pageLifetime{page}, slotLifetime{slot}, timestampLifetime{timestamp}, sceneLifetime{scene};
    const auto kind = CurrentSaveLoadResourceProfile().confirmation.kind;
    switch (kind)
    {
    case SaveLoadConfirmationKind::Text:
    case SaveLoadConfirmationKind::SingleThemeDigits:
    case SaveLoadConfirmationKind::PairedDigits:
    case SaveLoadConfirmationKind::PairedDigitsWithSeparators:
        break;
    default:
        throw std::logic_error("save confirmation kind is not supported");
    }
    auto& body = Dialog(common);
    const auto handle = Surface(body.labelsAt90)->handleAt04;
    if (handle) RequireSaveLoadConfirmDialogNativeServices().drawing.SurfaceClearRaw(handle, 0, nullptr);
    if (kind == SaveLoadConfirmationKind::Text)
    {
        DrawText(body,page.GetString(),0,saveMode);
        DrawText(body,slot.GetString(),1,saveMode);
        DrawText(body,timestamp.GetString(),2,saveMode);
        DrawText(body,scene.GetString(),3,saveMode);
    }
    else
    {
        DrawText(body,scene.GetString(),3,saveMode);
        DrawPage(body, page.GetString(), saveMode);
        DrawSlot(body, slot.GetString(), saveMode);
        DrawTimestamp(body, timestamp.GetString(), saveMode);
    }
    // 5360AA..536123：正常尾page/slot/time/scene；EH反序，Free抛出不重试。
    pageLifetime.Finish(); slotLifetime.Finish(); timestampLifetime.Finish(); sceneLifetime.Finish();
}
void ConfigureSaveLoadConfirmDialogNativeBorrowed(GuiDialogNativeBody5Ch* common,
    const RuntimeCString& page, const RuntimeCString& slot, const RuntimeCString& timestamp,
    const RuntimeCString& scene, std::uint8_t saveMode)
{
    using namespace SaveLoadConfirmNativeDetail;
    TextStorage a, b, c, d;
    CloneArgument(a, page); TextLifetime aLifetime{a.value};
    CloneArgument(b, slot); TextLifetime bLifetime{b.value};
    CloneArgument(c, timestamp); TextLifetime cLifetime{c.value};
    CloneArgument(d, scene); TextLifetime dLifetime{d.value};
    aLifetime.armed = bLifetime.armed = cLifetime.armed = dLifetime.armed = false;
    ConfigureSaveLoadConfirmDialogNativeAt535FF0(common, a.value, b.value, c.value, d.value, saveMode);
}
void AssignSaveLoadConfirmDialogNativePreviewAt538430(GuiDialogNativeBody5Ch* common,
    YtlReferenceSlotRuntime& argument)
{
    using namespace SaveLoadConfirmNativeDetail;
    RefLifetime lifetime{argument};
    auto& target = Dialog(common).previewAt9C;
    if (&target != &argument)
    {
        ReleaseUncleared(target);
        Copy(target, argument);
    }
    lifetime.Finish();
}
}

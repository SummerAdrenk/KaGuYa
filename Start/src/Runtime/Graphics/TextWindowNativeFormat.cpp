#include <Runtime/TextWindowNativeFormat.h>

#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/FileManagerNative.h>
#include <Runtime/MessageOwnerNative.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/ScriptInvalidCommandNative.h>
#include <Runtime/UnSaveDataRuntime.h>

#include <atlbase.h>
#include <bit>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
SaveDataEnvironmentArchive* CaptureInitialSave(EnvironmentRootOwner& environment)
{
    // 4C2754..279A先end再begin；后续417990调用具有自己的读取顺序。
    const volatile auto& array = environment.CaptureAt4180C0()->archivesAt30;
    const auto end = reinterpret_cast<std::uintptr_t>(array.end);
    const auto begin = reinterpret_cast<std::uintptr_t>(array.begin);
    const auto count = static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(end - begin)) / 12);
    if (count <= 1u) return nullptr;
    const volatile auto& slot = *reinterpret_cast<const YtlReferenceSlotRuntime*>(begin + 12u);
    const auto base = slot.baseAt00;
    if (!base) return nullptr;
    const auto* control = static_cast<const volatile EnvironmentObjectReferenceControl*>(slot.controlAt08);
    const auto bias = slot.biasAt04;
    const auto stride = control ? control->strideAt0C : 0u;
    return reinterpret_cast<SaveDataEnvironmentArchive*>(base + bias * stride);
}
SaveDataEnvironmentArchive* CaptureSave(EnvironmentRootOwner& environment)
{
    return static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*environment.CaptureAt4180C0(), 1));
}
bool NamesDiffer(const RuntimeCString& current, const char* original)
{
    // 4C27C3/4C2804仅拒绝默认名null；使用进程多字节状态，不创建局部locale。
    if (!original) throw ATL::CAtlException(E_FAIL);
    return _mbscmp(reinterpret_cast<const unsigned char*>(current.GetString()),
        reinterpret_cast<const unsigned char*>(original)) != 0;
}
void ReportReadError(IScrVmCommandErrorSink& errors)
{
    auto& root = errors.CaptureAt4180C0();
    // PE 5B48F8，4C28A3原MessageBoxA参数。
    constexpr char message[] = "\x83\x81\x83\x62\x83\x5A\x81\x5B\x83\x57\x83\x66"
        "\x81\x5B\x83\x5E\x82\xCC\x93\xC7\x82\xDD\x8D\x9E\x82\xDD\x82\xC9"
        "\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD";
    static_cast<void>(errors.ShowMessageBoxA(reinterpret_cast<HWND>(root.windowAt2C),
        message, root.titleAt10.GetString(), MB_ICONERROR));
}
}

RootTextWindowNativeFormatSource::RootTextWindowNativeFormatSource(
    RootRuntimeOwner& root, ParamsRuntimeRootOwner& params) noexcept : root_(root), params_(params) {}
ParamsGameSystem* RootTextWindowNativeFormatSource::CaptureGameSystemAt418480()
{
    return params_.CaptureAt418480()->gameSystemAt04.get();
}
std::int32_t RootTextWindowNativeFormatSource::ReadFormatAfterEnsureAt413BE0(
    std::int32_t index, MessageFormatSelectionRawTarget output, std::uint32_t variant)
{
    root_.EnsureCreatedAt413BE0();
    auto* const root = root_.Published();
    auto* const files = ResolveFileManagerAt479FF0(root->FileManagerSlotAt00());
    auto* const message = ResolveMessageReference(files->messageAt10);
    return message->ReadFormatRawAt417C40(index, output, variant, true);
}
ScriptParamsNativeBody* RootTextWindowNativeFormatSource::CaptureParamsAfterEnsureAt413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return &root_.Published()->ParamsAt47BDB0();
}

TextWindowNativeFormat::TextWindowNativeFormat(TextWindowLayerNativeBodyE8h& body,
    ITextWindowNativeFormatSource& source, TextWindowLayerNativeServices& window,
    TextWindowNativeText& layout, IGraphicsTextApi& text, IScrVmCommandErrorSink& errors) noexcept
    : body_(body), source_(source), window_(window), layout_(layout), text_(text), errors_(errors) {}

bool TextWindowNativeFormat::SetFormatAt4C2740(std::int32_t index, std::uint32_t initialize)
{
    MessageFormatSelectionRawTarget output{body_.nameAt70, body_.voicesAt7C, body_.bodyAt88};
    ClearMessageFormatRawAt4C5850(output);
    auto* const save = CaptureInitialSave(window_.environment);
    auto* const first = source_.CaptureGameSystemAt418480();
    const auto variant = NamesDiffer(save->stringAt58, first->defaultGivenName.Native().GetString()) ||
        NamesDiffer(save->stringAt5C, source_.CaptureGameSystemAt418480()->defaultFamilyName.Native().GetString()) ? 1u : 0u;
    const auto oldRead = source_.ReadFormatAfterEnsureAt413BE0(index, output, variant);
    if (oldRead != 0 && oldRead != 1)
    {
        ReportReadError(errors_);
        return false;
    }
    body_.unreadAt9A = static_cast<std::uint8_t>(oldRead == 0);
    body_.shadowAt98 = CaptureSave(window_.environment)->bytesAt60[0];
    body_.renderAt99 = CaptureSave(window_.environment)->bytesAt60[1];
    body_.formatIndexAt94 = index;
    auto* const unsave = static_cast<UnSaveDataRuntime*>(
        ResolveEnvironmentArchiveAt417990(*window_.environment.CaptureAt4180C0(), 2));
    AssignMessageReferenceAt410360(unsave->ArrayReferenceAt3C().RawSlot(), body_.voicesAt7C);
    body_.playbackStartedAt54 = 0;
    body_.glyphCursorAt50 = 0;
    ClearTextWindowFormatRegionNativeAt4C291B(body_, window_);
    if (initialize != 0) static_cast<void>(layout_.PrepareTextAt415150());
    return true;
}

bool TextWindowNativeFormat::RebuildAt4C1650(std::uint32_t refreshUnread)
{
    try
    {
        const auto index = source_.CaptureParamsAfterEnsureAt413BE0()->messageFormatIndexAt60;
        if (index == -1) throw 1;
        const auto unread = body_.unreadAt9A;
        if (!SetFormatAt4C2740(index, 0)) throw 2;
        if (refreshUnread == 0) body_.unreadAt9A = unread;
        static_cast<void>(layout_.PrepareTextAt415150());
        static_cast<void>(layout_.BlitRangeAt419090(0, static_cast<std::uint32_t>(text_.GetTextLength())));
        body_.playbackStartedAt54 = 1;
        body_.waitAt58 = 0;
        body_.autoWaitAt5C = 0;
        body_.glyphCursorAt50 = -1;
        return true;
    }
    catch (...)
    {
        return false;
    }
}
}

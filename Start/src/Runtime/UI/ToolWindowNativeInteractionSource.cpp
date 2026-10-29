#include <Runtime/ToolWindowNativeInteraction.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/TextWindowLayerNative.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ScrVmExecutableProfile.h>

#include <bit>

namespace KaGuYa::Start::Runtime
{
EnvironmentToolWindowNativeInteractionSource::EnvironmentToolWindowNativeInteractionSource(
    EnvironmentRootOwner& environment, InputRootOwner& input, RootRuntimeOwner& root,
    IDrawManagerNativeRootSource& draws, ScriptNamedValueNativeTables names, IScrVmCommandErrorSink& errors,
    IRuntimeStringManagerSource& strings) noexcept
    : environment_(environment), input_(input), root_(root), draws_(draws), names_(names), errors_(errors), strings_(strings) {}
UnSaveDataRuntime* EnvironmentToolWindowNativeInteractionSource::CaptureUnSave(ToolUnSaveNativeRead kind)
{
    auto* environment = environment_.CaptureAt4180C0();
    if (kind == ToolUnSaveNativeRead::Lookup417990)
        return static_cast<UnSaveDataRuntime*>(ResolveEnvironmentArchiveAt417990(*environment, 2));
    const volatile auto& array = environment->archivesAt30;
    const auto end = reinterpret_cast<std::uintptr_t>(array.end);
    auto* const begin = array.begin;
    const auto bytes = std::uint32_t(end - reinterpret_cast<std::uintptr_t>(begin));
    const auto count = std::uint32_t(std::bit_cast<std::int32_t>(bytes) / 12);
    return count <= 2U ? nullptr : ResolveEnvironmentObjectReference<UnSaveDataRuntime>(begin[2]);
}
KeyboardManagerNativeBody438h* EnvironmentToolWindowNativeInteractionSource::CaptureKeyboardAt418210()
{ return ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(input_.CaptureAt418210()->keyboardAt18); }
std::int32_t EnvironmentToolWindowNativeInteractionSource::ReadTextCursorAfter413BE0()
{
    auto* draw = draws_.CaptureDrawManagerAfter413BE0();
    auto* text = reinterpret_cast<TextWindowLayerNativeBodyE8h*>(DrawLayerAt418C10(*draw, 3));
    return text->glyphCursorAt50;
}
std::uint8_t EnvironmentToolWindowNativeInteractionSource::ReadMessageEnabledAfter413BE0()
{ return DrawLayerAt418C10(*draws_.CaptureDrawManagerAfter413BE0(), 1)->enabledAt0C; }
std::int32_t EnvironmentToolWindowNativeInteractionSource::ReadNamedAfterRoot(const char* name)
{
    root_.EnsureCreatedAt413BE0();
    auto* root = root_.ReadPublishedAt6198C4();
    (void)ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 0);
    const auto index = ResolveScriptNamedValueNativeAt498020(name,names_,strings_);
    return ReadScriptValueAt4C73A0(root->ParamsAt47BDB0(), index, errors_);
}
}

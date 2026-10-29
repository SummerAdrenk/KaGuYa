#include <Runtime/SelectWindowNativeWorkflow.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/FileManagerNative.h>
#include <Runtime/MessageOwnerNative.h>
#include <Runtime/MessageTextSelectionRuntime.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ScriptArithmeticCommandNative.h>
#include <Runtime/ScrVmExecutableProfile.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
MessageOwnerNative34h* MessageAfterEnsure(RootRuntimeOwner& source)
{
    source.EnsureCreatedAt413BE0();
    auto* root = source.Published();
    auto* files = ResolveFileManagerAt479FF0(root->FileManagerSlotAt00());
    return ResolveMessageReference(files->messageAt10);
}
}
RootSelectWindowNativeWorkflowSource::RootSelectWindowNativeWorkflowSource(RootRuntimeOwner& root,
    EnvironmentRootOwner& environment, InputRootOwner& input, ScriptNamedValueNativeTables names,
    IScrVmCommandErrorSink& errors, IStartFileApi& files, const IMessageEnvironmentPathSource& paths,
    IRuntimeStringManagerSource& strings) noexcept
    : root_(root), environment_(environment), input_(input), names_(names), errors_(errors), files_(files), paths_(paths),strings_(strings) {}
ScriptParamsNativeBody* RootSelectWindowNativeWorkflowSource::CaptureParamsAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return &root_.Published()->ParamsAt47BDB0();
}
float RootSelectWindowNativeWorkflowSource::ReadMessageVersionAfter413BE0()
{
    auto* message = MessageAfterEnsure(root_);
    auto reader = message->SourceWithContext({files_});
    return reader.ReadVersionAt47DDE0();
}
std::int32_t RootSelectWindowNativeWorkflowSource::ReadChoiceAfter413BE0(std::int32_t id,
    MessageFormatSelectionRawTarget output)
{
    auto* message = MessageAfterEnsure(root_);
    auto reader = message->SourceWithContext({files_});
    MessageChoiceSelectionRuntime choices(reader, message->flagsAt10[1]);
    return choices.ReadChoiceRawAt418700(id, output, 0);
}
std::int32_t RootSelectWindowNativeWorkflowSource::ReadLegacyAfter413BE0(std::int32_t id,
    MessageFormatSelectionRawTarget output)
{
    auto* message = MessageAfterEnsure(root_);
    auto reader = message->SourceWithContext({files_});
    auto flags = message->ReadFlagsWithSources(files_, paths_);
    MessageFormatSelectionRuntime formats(reader, flags);
    return formats.ReadFormatRawAt417C40(id, output, 0, false);
}
ScriptNativeBody* RootSelectWindowNativeWorkflowSource::CaptureScriptAfter413BE0()
{
    root_.EnsureCreatedAt413BE0();
    return root_.Published()->ScriptAt47CD00();
}
std::int32_t RootSelectWindowNativeWorkflowSource::ReadProgramFlagAfterRoot(std::uint32_t ordinal)
{
    constexpr const char* names[]{"prog_param00", "prog_param01", "prog_param02", "prog_param03"};
    root_.EnsureCreatedAt413BE0();
    auto* root = root_.Published();
    // 436E41..EC6先固定root，再取slot0/名称ID，最后通过固定root晚取CParams。
    (void)ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 0);
    const auto index = ResolveScriptNamedValueNativeAt498020(names[ordinal],names_,strings_);
    return ReadScriptValueAt4C73A0(root->ParamsAt47BDB0(), index, errors_);
}
MouseManagerNativeBody3Ch* RootSelectWindowNativeWorkflowSource::CaptureMouseAt418210()
{ return ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(input_.CaptureAt418210()->mouseAt00); }
void RootSelectWindowNativeWorkflowSource::MarkChoiceAfter413BE0(std::int32_t id)
{
    auto* message = MessageAfterEnsure(root_);
    auto flags = message->ReadFlagsWithSources(files_, paths_);
    (void)flags.MarkChoiceAt4C29D0(id, 1);
}
}

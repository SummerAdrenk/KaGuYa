#include <Runtime/ReadScriptInteractionStateSource.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/RootRuntimeOwner.h>
#include <Runtime/ScrVmValueTable.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/UnSaveDataRuntime.h>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
SaveDataEnvironmentArchive& FixedReadScriptInteractionStateSource::CaptureSaveAt4180C0() { return save_; }
UnSaveDataRuntime& FixedReadScriptInteractionStateSource::CaptureUnSaveAt4180C0() { return unsave_; }
std::uint32_t FixedReadScriptInteractionStateSource::ReadMessageFormatAt47BDB0()
{
    if (!params_) throw std::logic_error("Fixed interaction source has no message-format binding");
    return params_->CurrentParamsGeneration().messageFormatIndexAt60;
}
SaveDataEnvironmentArchive& RootReadScriptInteractionStateSource::CaptureSaveAt4180C0()
{
    return *static_cast<SaveDataEnvironmentArchive*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 1));
}
UnSaveDataRuntime& RootReadScriptInteractionStateSource::CaptureUnSaveAt4180C0()
{
    return *static_cast<UnSaveDataRuntime*>(
        ResolveEnvironmentArchiveAt417990(*environment_.CaptureAt4180C0(), 2));
}
std::uint32_t RootReadScriptInteractionStateSource::ReadMessageFormatAt47BDB0()
{
    // 4B2775/4B2780：先Ensure，再由当前root取得CParams；Draw由后续独立调用捕获。
    root_.EnsureCreatedAt413BE0();
    return root_.Published()->ParamsAt47BDB0().messageFormatIndexAt60;
}
}

#include <Runtime/NormalStartupScriptServices.h>
#include <Runtime/DrawManagerResourceRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
NormalStartupScriptServices::NormalStartupScriptServices(
    ScrVmParamsGenerationBinding generationSlot,
    ScrScriptRuntimeState& scriptRuntime,
    const IScrResourceLookupSource& resourceNames,
    IStartupParamsState& paramsState,
    const ResourceCatalog& catalog,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& fileApi,
    const IEnvironmentKeyTableSource& environmentKeyBytes,
    INormalStartupRegistryState& registry,
    INormalStartupRootOwnerSynchronizer& rootOwners,
    IGraphicsRuntimeCalls& graphics,
    IGraphicsResourceErrorDisplay& errors,
    ArchiveCodecServices codecs)
    : paramsState_(paramsState),
      scriptRuntime_(&scriptRuntime),
      resourceNames_(&resourceNames),
      catalog_(&catalog),
      transactionContext_(transactionContext),
      fileApi_(&fileApi),
      environmentKeyBytes_(&environmentKeyBytes),
      registry_(&registry),
      rootOwners_(&rootOwners),
      graphics_(&graphics),
      errors_(&errors),
      codecs_(codecs)
{
    if (!generationSlot)
        throw std::invalid_argument("normal startup params generation slot is null");
    if (!scriptRuntime_->UsesParamsGenerationSlot(generationSlot))
    {
        throw std::invalid_argument(
            "normal startup script runtime uses a different params generation slot");
    }
}

void NormalStartupScriptServices::ReplaceParamsGeneration()
{
    paramsState_.ReplaceParamsGeneration();
}

void NormalStartupScriptServices::WriteRegistrySlot2DwordAt04(const std::uint32_t value)
{
    registry_->WriteSlot2DwordAt04(value);
}

void NormalStartupScriptServices::WriteRegistrySlot2ByteAt0C(const std::uint8_t value)
{
    registry_->WriteSlot2ByteAt0C(value);
}

void NormalStartupScriptServices::WriteRegistrySlot2DwordAt18(const std::uint32_t value)
{
    registry_->WriteSlot2DwordAt18(value);
}

void NormalStartupScriptServices::WriteRegistrySlot2DwordAt14(const std::uint32_t value)
{
    registry_->WriteSlot2DwordAt14(value);
}

void NormalStartupScriptServices::WriteRegistrySlot2DwordAt1C(const std::uint32_t value)
{
    registry_->WriteSlot2DwordAt1C(value);
}

void NormalStartupScriptServices::WriteRegistrySlot2DwordAt20(const std::uint32_t value)
{
    registry_->WriteSlot2DwordAt20(value);
}

void NormalStartupScriptServices::WriteRegistrySlot2ByteAt08(const std::uint8_t value)
{
    registry_->WriteSlot2ByteAt08(value);
}

void NormalStartupScriptServices::WriteRegistrySlot2ByteAt09(const std::uint8_t value)
{
    registry_->WriteSlot2ByteAt09(value);
}

void NormalStartupScriptServices::WriteRegistrySlot2ByteAt28(const std::uint8_t value)
{
    registry_->WriteSlot2ByteAt28(value);
}

void NormalStartupScriptServices::WriteRegistrySlot2ByteAt29(const std::uint8_t value)
{
    registry_->WriteSlot2ByteAt29(value);
}

std::int32_t NormalStartupScriptServices::ResolveScriptId(
    const char* const scriptNameCp932) const
{
    // 反查自己按原调用点取得当前Pattern；构造不捕获，后续LoadScript另行取源。
    return resourceNames_->FindResourceIdAt47A020(scriptNameCp932);
}

ScrScriptSwitchResult NormalStartupScriptServices::LoadScript(const std::int32_t scriptId)
{
    auto result = scriptRuntime_->LoadScript(
        scriptId, *resourceNames_, *catalog_, transactionContext_, *fileApi_,
        *environmentKeyBytes_, codecs_);
    // 4D3EE6先显示装载错误，4D3EFC才返回false；未调用Load的分支不经过这里。
    if (!result.success) errors_->Show(result.message.c_str());
    return result;
}

void NormalStartupScriptServices::CopyRegistryPrologueFlag()
{
    paramsState_.CopyRegistryPrologueFlag();
}

void NormalStartupScriptServices::WriteReplayFile(const std::int32_t scriptId)
{
    paramsState_.WriteReplayFile(scriptId);
}

void NormalStartupScriptServices::WriteReplayPosition(const std::int32_t entry)
{
    paramsState_.WriteReplayPosition(entry);
}

void NormalStartupScriptServices::SynchronizeRootOwnersAtVslot18()
{
    rootOwners_->SynchronizeAtVslot18();
}

const char* NormalStartupScriptServices::ReadRegistrySlot1NameAt58() const
{
    return registry_->ReadSlot1NameAt58();
}

const char* NormalStartupScriptServices::ReadRegistrySlot1NameAt5C() const
{
    return registry_->ReadSlot1NameAt5C();
}

void NormalStartupScriptServices::SetGraphicsTextFirstName(const char* const nameCp932)
{
    graphics_->SetTextFirstName(nameCp932);
}

void NormalStartupScriptServices::SetGraphicsTextSecondName(const char* const nameCp932)
{
    graphics_->SetTextSecondName(nameCp932);
}

void NormalStartupScriptServices::MirrorParamsDwordAt3CTo40()
{
    paramsState_.ClearHistoryAt4D5730();
}

void NormalStartupScriptServices::CallRegistrySlot4Nullsub3()
{
    registry_->CallSlot4Nullsub3();
}
}

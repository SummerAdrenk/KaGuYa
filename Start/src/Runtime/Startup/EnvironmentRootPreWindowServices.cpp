#include <Runtime/EnvironmentRootPreWindowServices.h>
#include <Runtime/EnvironmentPreWindowServices.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Runtime/EnvironmentPreWindowPersistenceNative.h>
#include <Runtime/SoundParamsEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootPreWindowServices::EnvironmentRootPreWindowServices(
    EnvironmentRootOwner& environment, ParamsRuntimeRootOwner& params, IEnvironmentPathApi& paths,
    IStartFileApi& files, const FileTransactionContext* transaction, IGraphicsWindowModeApi& graphics,
    IWindowModePlatformApi& windowApi, YtlReferenceSlotRuntime& windowModeOwner,
    WindowModeRuntimeServices& windowModeServices,
    IEnvironmentInitializationDisplay& display) noexcept
    : environment_(environment), params_(params), registry_(environment), paths_(paths), files_(files),
      transaction_(transaction), graphics_(graphics), windowApi_(windowApi),
      windowModeOwner_(windowModeOwner), windowModeServices_(windowModeServices), display_(display) {}

void EnvironmentRootPreWindowServices::RegisterEnvironmentArchives(
    const std::array<EnvironmentArchiveKind, 7>& order)
{
    if (order != EnvironmentArchiveRegistrationOrder)
        throw std::invalid_argument("47E2B0 archive registration order mismatch");
    SoundParamsSettingsRootSource sounds(params_);
    registry_.RegisterAt47E2B0(params_, sounds, paths_);
}

bool EnvironmentRootPreWindowServices::PrepareEnvironmentPersistence()
{
    return ReadPreWindowEnvironmentAt480230(registry_, files_, transaction_);
}

void EnvironmentRootPreWindowServices::WriteEnvironmentPersistence()
{
    WritePreWindowEnvironmentAt488740(registry_, files_, transaction_);
}

void EnvironmentRootPreWindowServices::InstallWindowModeManager()
{
    // 4778DB保存的receiver不随CString/分配回调换根；服务仅借用，不拥有正文。
    windowModeServices_ = {&graphics_, &windowApi_, nullptr, nullptr, nullptr, &environment_};
    CreateAndReplaceWindowModeAt47791D(windowModeOwner_);
}
const SaveDataEnvironmentArchive& EnvironmentRootPreWindowServices::SaveDataAtSlot1() const
{ return registry_.SaveData(); }
ClickInfoArchive& EnvironmentRootPreWindowServices::MutableClickInfoAtSlot5() { return registry_.ClickInfo(); }
std::string EnvironmentRootPreWindowServices::BuildInitializationErrorText()
{
    const auto result = registry_.ErrorTextAt479EA0();
    return std::string(RuntimeStringView(result.Value()));
}
void EnvironmentRootPreWindowServices::ShowInitializationError(
    std::string_view text, std::string_view caption, unsigned type)
{ display_.Show(text, caption, type); }
}

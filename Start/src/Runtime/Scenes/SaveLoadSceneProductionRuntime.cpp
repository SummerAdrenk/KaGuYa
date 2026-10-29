#include <Runtime/SaveLoadSceneProductionRuntime.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
SaveLoadSceneProductionServices::SaveLoadSceneProductionServices(
    ISaveLoadSceneServices& upstream,
    SaveLoadDialogProductionDependencies dialogDependencies) noexcept
    : upstream_(&upstream), dialogDependencies_(dialogDependencies)
{
}

std::uint8_t SaveLoadSceneProductionServices::ReadRootByteAt1AF() const
{
    return upstream_->ReadRootByteAt1AF();
}

SaveLoadDialogRef SaveLoadSceneProductionServices::CreateSaveLoadDialog(
    const SaveLoadSceneMode mode,
    const std::uint8_t rootByteAt1AF)
{
    if (dialogOwner_)
    {
        throw std::logic_error(
            "SaveLoadScene attempted to create its dialog more than once");
    }

    // 0x4A1802 将 root+0x1AF 作为第二个显式参数传入 0x4EB950；
    // 0x4EB950 的完整反汇编没有读取该参数，不能为它虚构业务含义。
    static_cast<void>(rootByteAt1AF);
    dialogDependencies_.mode = mode;
    dialogOwner_ = std::make_unique<SaveLoadDialogProductionRuntime>(
        dialogDependencies_);
    return dialogOwner_->Dialog();
}

std::uint32_t
SaveLoadSceneProductionServices::ReadRegistrySlot2DwordAt04() const
{
    return upstream_->ReadRegistrySlot2DwordAt04();
}

void SaveLoadSceneProductionServices::WriteRegistrySlot2DwordAt04(
    const std::uint32_t value)
{
    upstream_->WriteRegistrySlot2DwordAt04(value);
}

std::uint8_t SaveLoadSceneProductionServices::ReadRootByteAt219() const
{
    return upstream_->ReadRootByteAt219();
}

void SaveLoadSceneProductionServices::WriteRootByteAt219(
    const std::uint8_t value)
{
    upstream_->WriteRootByteAt219(value);
}

void SaveLoadSceneProductionServices::WriteRootSubobjectAt4AD230DwordAt10(
    const std::uint32_t value)
{
    upstream_->WriteRootSubobjectAt4AD230DwordAt10(value);
}

void SaveLoadSceneProductionServices::AdvanceActiveSceneFromPendingAt4C1760()
{
    upstream_->AdvanceActiveSceneFromPendingAt4C1760();
}

void SaveLoadSceneProductionServices::ClearActiveSceneAt4C1460()
{
    upstream_->ClearActiveSceneAt4C1460();
}

void SaveLoadSceneProductionServices::QueueLoadAfterSelectionSceneAt47F920()
{
    upstream_->QueueLoadAfterSelectionSceneAt47F920();
}

const SaveLoadDialogProductionRuntime*
SaveLoadSceneProductionServices::DialogOwner() const noexcept
{
    return dialogOwner_.get();
}

SaveLoadSceneProductionObjectGraphRuntime::
SaveLoadSceneProductionObjectGraphRuntime(
    ISaveLoadSceneServices& sceneUpstream,
    SaveLoadDialogProductionDependencies dialogDependencies,
    const SaveLoadSceneMode mode)
    : services_(sceneUpstream, dialogDependencies),
      scene_(std::make_shared<SaveLoadSceneRuntime>(services_, mode))
{
}

const std::shared_ptr<SaveLoadSceneRuntime>&
SaveLoadSceneProductionObjectGraphRuntime::Scene() const noexcept
{
    return scene_;
}

const SaveLoadDialogProductionRuntime&
SaveLoadSceneProductionObjectGraphRuntime::DialogOwner() const
{
    const auto* const owner = services_.DialogOwner();
    if (owner == nullptr)
        throw std::logic_error("SaveLoadScene has no production dialog owner");
    return *owner;
}

SaveLoadSceneConstructionServicesRuntime::
SaveLoadSceneConstructionServicesRuntime(
    ISceneConstructionServices& upstream,
    ISaveLoadSceneServices& saveLoadServices,
    SaveLoadDialogProductionDependencies dialogDependencies) noexcept
    : SceneConstructionServicesForwarderRuntime(upstream),
      saveLoadServices_(&saveLoadServices),
      dialogDependencies_(dialogDependencies)
{
}

std::shared_ptr<ISceneTask>
SaveLoadSceneConstructionServicesRuntime::CreateSaveScreen()
{
    return CreateSaveLoadScene(SaveLoadSceneMode::Save);
}

std::shared_ptr<ISceneTask>
SaveLoadSceneConstructionServicesRuntime::CreateLoadScreen()
{
    return CreateSaveLoadScene(SaveLoadSceneMode::Load);
}

std::shared_ptr<ISceneTask>
SaveLoadSceneConstructionServicesRuntime::CreateSaveLoadScene(
    const SaveLoadSceneMode mode)
{
    auto owner = std::make_shared<SaveLoadSceneProductionObjectGraphRuntime>(
        *saveLoadServices_, dialogDependencies_, mode);
    SaveLoadSceneRuntime* const scene = owner->Scene().get();
    if (scene == nullptr)
        throw std::logic_error("SaveLoad production owner has no scene");

    // aliasing shared_ptr 让 SceneController 看见 ISceneTask，同时由同一
    // 控制块持有完整 owner；场景弹出后才会释放 Dialog 与其资源。
    return std::shared_ptr<ISceneTask>(std::move(owner), scene);
}
}

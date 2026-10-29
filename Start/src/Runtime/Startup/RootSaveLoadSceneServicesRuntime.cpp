#include <Runtime/RootSaveLoadSceneServicesRuntime.h>
#include <Runtime/RootSaveLoadNativeSource.h>

namespace KaGuYa::Start::Runtime
{
RootSaveLoadSceneServicesRuntime::RootSaveLoadSceneServicesRuntime(RootSaveLoadSceneServicesDependencies d) noexcept
    :parent_({d.persistence,d.environment,d.input,d.graphics,d.pictures,d.deletionConfirmation}),
    construction_{parent_,d.graphics,SaveLoadDialogNativeVtable(),d.strings},drawing_(d.graphics),screen_(d.graphics),
    returnTitle_(d.persistence,d.generalConfirmation),deletion_(d.root,d.environment,d.strings),
    runtime_{construction_,d.pages,parent_,parent_,drawing_,parent_,d.graphics,screen_,returnTitle_,
        d.persistence,deletion_,d.persistence,d.memory},adapter_(runtime_),
    recovery_(d.startup,d.root,&::MessageBoxA,d.memory,d.strings,&d.text),
    source_({d.root,d.environment,recovery_}),scene_{source_,adapter_,d.memory}
{}
}

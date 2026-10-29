#include <Runtime/RootSettingDialogNativeConfirmRuntime.h>

namespace KaGuYa::Start::Runtime
{
RootSettingDialogNativeConfirmRuntime::RootSettingDialogNativeConfirmRuntime(RootSettingDialogNativeConfirmDependencies d) noexcept
    :serialization_{d.memory,d.strings,d.files},source_(d.root,d.environment,d.params,serialization_,d.post),
     services_{source_,constructors_,d.general,d.names,d.modal,d.memory}{}
}

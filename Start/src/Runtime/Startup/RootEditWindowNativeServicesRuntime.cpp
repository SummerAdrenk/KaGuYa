#include <Runtime/RootEditWindowNativeServicesRuntime.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <Runtime/ProcessWindowClassNative.h>

namespace KaGuYa::Start::Runtime
{
RootEditWindowNativeServicesRuntime::RootEditWindowNativeServicesRuntime(RootRuntimeOwner& root,
    EnvironmentRootOwner& environment,IRuntimeStringManagerSource& strings)noexcept
    :parent_(environment),
     creation_({ProcessAtlBaseModuleNative(),ProcessWindowClassWinModuleNative(),editClass_,controlClass_,rectangle_,
        ProcessAtlBaseModuleNative().instanceAt04,DefaultWindowClassNativeApi(),DefaultEditWindowNativeCreationApi(),parent_}),
     maps_{DefaultEditWindowNativeMapApi(),creation_,strings},thunks_(ProcessWindowThunkNativeServices()),
     source_(creation_,maps_,thunks_),modes_(root),dimensions_(environment),viewport_{modes_,dimensions_}
{
    InitializeEditWindowClassNativeAt601838(editClass_,&InitialEditWindowNativeAt48E090);
    InitializeEditControlClassNativeAt6015F0(controlClass_,&InitialEditControlNativeAt48E020);
}
}

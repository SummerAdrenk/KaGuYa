#include "ProgramSelectDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace { thread_local ProgramSelectDialogNativeServices* selectedServices = nullptr; }

ProgramSelectDialogNativeScope::ProgramSelectDialogNativeScope(
    ProgramSelectDialogNativeServices& services) noexcept : ProgramSelectDialogNativeScope(&services) {}
ProgramSelectDialogNativeScope::ProgramSelectDialogNativeScope(
    ProgramSelectDialogNativeServices* services) noexcept
    : previous_(selectedServices), selected_(services)
{
    selectedServices=selected_;
    main00_.emplace(services?services->main00:nullptr);
    main01_.emplace(services?services->main01:nullptr);
    system_.emplace(services?services->system:nullptr);
    if(services)surfaces_.emplace(services->surfaces);
}
ProgramSelectDialogNativeScope::~ProgramSelectDialogNativeScope() noexcept { selectedServices = previous_; }
ProgramSelectDialogNativeServices* SelectedProgramSelectDialogNativeServices() noexcept
{ return selectedServices; }
ProgramSelectDialogNativeServices& RequireProgramSelectDialogNativeServices()
{
    if (!selectedServices) std::terminate();
    return *selectedServices;
}
}

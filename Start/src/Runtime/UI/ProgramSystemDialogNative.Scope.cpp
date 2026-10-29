#include "ProgramSystemDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace { thread_local ProgramSystemDialogNativeServices* selected = nullptr; }
ProgramSystemDialogNativeScope::ProgramSystemDialogNativeScope(ProgramSystemDialogNativeServices& s) noexcept
    : ProgramSystemDialogNativeScope(&s) {}
ProgramSystemDialogNativeScope::ProgramSystemDialogNativeScope(ProgramSystemDialogNativeServices* s) noexcept
    : previous_(selected)
{
    selected=s;
    if(s)
    {
        surfaces_.emplace(s->parent.surfaces);graphics_.emplace(s->parent.graphics);
        input_.emplace(s->parent.input);screen_.emplace(s->parent.screen);
    }
}
ProgramSystemDialogNativeScope::~ProgramSystemDialogNativeScope() noexcept { selected = previous_; }
ProgramSystemDialogNativeServices* SelectedProgramSystemDialogNativeServices() noexcept { return selected; }
ProgramSystemDialogNativeServices& RequireProgramSystemDialogNativeServices()
{
    if (!selected) std::terminate();
    return *selected;
}
}

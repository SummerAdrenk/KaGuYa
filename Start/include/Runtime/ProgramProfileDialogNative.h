#pragma once
#include <Runtime/ProgramDialogResourceData.h>
#include <Runtime/ProgramSelectDialogNative.h>

namespace KaGuYa::Start::Runtime
{
std::size_t ProgramProfileDialogStorageSize() noexcept;
GuiDialogNativeBody5Ch* ConstructProgramProfileDialog(void*, const ProgramModeProfile&,
    ProgramSelectDialogNativeServices&);
bool ProgramProfileDialogHasModal(const GuiDialogNativeBody5Ch*) noexcept;
bool IsProgramProfileDialog(const GuiDialogNativeBody5Ch*) noexcept;
const ProgramModeProfile* SelectedProgramDialogProfile() noexcept;
}

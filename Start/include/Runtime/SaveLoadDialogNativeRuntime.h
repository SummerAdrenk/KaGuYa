#pragma once
#include <Runtime/SaveLoadDialogNativeWorkflow.h>
#include <Runtime/SaveLoadDialogNativeCommands.h>
#include <Runtime/SaveLoadDialogNativeDeletion.h>
#include <Runtime/SaveLoadDialogNativeConfirmation.h>
#include <Runtime/SaveLoadDialogNativeScope.h>
#include <Runtime/SaveLoadPageFactoryNative.h>

namespace KaGuYa::Start::Runtime
{
// 只借调用来源和已完成原核，所有状态仍在唯一184h/各child正文上。
struct SaveLoadDialogNativeRuntimeServices final
{
    SaveLoadDialogNativeConstructionServices& construction;
    SaveLoadPageFactoryNative& pages;
    ISaveLoadDialogNativeBeginSource& begin;
    ISaveLoadDialogNativeCatalogSource& catalog;
    ISaveLoadDialogNativeDrawingSource& drawing;
    ISaveLoadDialogNativeTransitionSource& transition;
    IGraphicsTransitApi& transit;
    IGuiDialogNativeScreenSource& screen;
    ISaveLoadDialogNativeReturnTitleSource& returnTitle;
    ISaveLoadDialogNativeDeleteConfirmationSource& deleteConfirmation;
    ISaveLoadDialogNativeDeletionSource& deletion;
    ISaveLoadDialogNativeConfirmationSource& persistence;
    IEnvironmentRootMemory& memory=DefaultEnvironmentRootMemory();
};
class SaveLoadDialogNativeRuntimeScope final
{
public:
    explicit SaveLoadDialogNativeRuntimeScope(SaveLoadDialogNativeRuntimeServices&) noexcept;
    ~SaveLoadDialogNativeRuntimeScope() noexcept;
    SaveLoadDialogNativeRuntimeScope(const SaveLoadDialogNativeRuntimeScope&)=delete;
    SaveLoadDialogNativeRuntimeScope& operator=(const SaveLoadDialogNativeRuntimeScope&)=delete;
private:
    SaveLoadDialogNativeRuntimeServices* previous_;
    SaveLoadDialogNativeConstructionScope construction_;
    SaveLoadPageFactoryNativeScope pages_;
    GuiDialogNativeScope screen_;
};
SaveLoadDialogNativeRuntimeServices& RequireSaveLoadDialogNativeRuntimeServices();
const SaveLoadDialogNativeVtable37& SaveLoadDialogNativeVtable() noexcept;
SaveLoadDialogNativeCommandEntries SaveLoadDialogNativeRuntimeCommands(SaveLoadDialogNativeRuntimeServices&) noexcept;
}

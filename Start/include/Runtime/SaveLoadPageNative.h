#pragma once
#include <Runtime/SaveLoadPageSecondNative.h>
#include <Runtime/SaveFileDeleteFactoryNative.h>
#include <Runtime/SaveLoadDialogNative.h>
#include <span>

namespace KaGuYa::Start::Runtime
{
struct SaveLoadPageSlotNativeServices;
struct SaveLoadPageNativeServices final
{
    SaveLoadPageSecondNativeServices& common;
    ISaveFileDeleteFactoryNativeSource& factory;
    IGraphicsWindowSurfaceApi& drawing;
    SaveLoadPageSlotNativeServices& slots;
};
// 派生不增加AC正文；新增服务只在外部scope持有，不能存进原未写字段。
class SaveLoadPageNativeScope final
{
public:
    explicit SaveLoadPageNativeScope(SaveLoadPageNativeServices&) noexcept;
    ~SaveLoadPageNativeScope() noexcept;
    SaveLoadPageNativeScope(const SaveLoadPageNativeScope&)=delete;
    SaveLoadPageNativeScope& operator=(const SaveLoadPageNativeScope&)=delete;
private:
    SaveLoadPageNativeServices* previous_;
    SaveLoadPageSecondNativeScope common_;
};
SaveLoadPageNativeServices& RequireSaveLoadPageNativeServices();
const SaveLoadPageSecondNativeVtable36& SaveLoadPage00NativeVtable() noexcept;
const SaveLoadPageSecondNativeVtable36& SaveLoadPage01NativeVtable() noexcept;
GuiDialogNativeBody5Ch* ConstructSaveLoadPage00NativeAt4ED190(void* rawAC,
    GuiDialogNativeBody5Ch* parent,std::int32_t id,SaveLoadPageNativeServices&);
GuiDialogNativeBody5Ch* ConstructSaveLoadPage01NativeAt4EE390(void* rawAC,
    GuiDialogNativeBody5Ch* parent,std::int32_t id,SaveLoadPageNativeServices&);
std::int32_t ChangeSaveLoadPage00NativeAt5339C0(GuiDialogNativeBody5Ch*,std::int32_t page,std::int32_t notify,std::int32_t force);
std::int32_t ChangeSaveLoadPage01NativeAt5345A0(GuiDialogNativeBody5Ch*,std::int32_t page,std::int32_t notify,std::int32_t force);
std::int32_t EndSaveLoadPage00NativeAt509420(GuiDialogNativeBody5Ch*);
std::int32_t EndSaveLoadPage01NativeAt509490(GuiDialogNativeBody5Ch*);

const GuiStaticDescriptorNative24h& SaveLoadPageNativeBackground(bool load,std::int32_t id);
std::span<const GuiButtonDescriptorNative2Ch,3> SaveLoadPageNativeTopButtons(bool load) noexcept;
const GuiButtonDescriptorNative2Ch& SaveLoadPageNativeFileButton(bool load) noexcept;
const GuiButtonDescriptorNative2Ch& SaveLoadPageNativeDeleteButton(bool load) noexcept;
std::span<const POINT,8> SaveLoadPageNativeFilePositions() noexcept;
struct SaveLoadPageNativeNavigation final{std::int32_t input;const GuiDialogNavigationMapping* pairs;};
std::span<const SaveLoadPageNativeNavigation,4> SaveLoadPageNativeNavigationTable() noexcept;
}

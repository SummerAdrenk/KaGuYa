#include <Runtime/SaveLoadPageFactoryNative.h>

namespace KaGuYa::Start::Runtime
{
GuiDialogNativeBody5Ch* SaveLoadPageFactoryNative::ConstructPage00At4ED190(void* raw,
    GuiDialogNativeBody5Ch* parent,std::int32_t id)
{return ConstructSaveLoadPage00NativeAt4ED190(raw,parent,id,pages_);}
GuiDialogNativeBody5Ch* SaveLoadPageFactoryNative::ConstructPage01At4EE390(void* raw,
    GuiDialogNativeBody5Ch* parent,std::int32_t id)
{return ConstructSaveLoadPage01NativeAt4EE390(raw,parent,id,pages_);}
GuiDialogNativeBody5Ch* SaveLoadPageFactoryNative::ConstructSystemAt4F0C80(void* raw,
    GuiDialogNativeBody5Ch* parent,std::int32_t id)
{return ConstructSaveLoadSystemPageNativeAt4F0C80(raw,parent,id,system_);}
GuiDialogNativeBody5Ch* SaveLoadPageFactoryNative::ConstructPopupAt4EF660(void* raw,
    GuiDialogNativeBody5Ch* parent,std::int32_t id)
{return ConstructSaveLoadPopupNativeAt4EF660(raw,parent,id,popup_);}
}

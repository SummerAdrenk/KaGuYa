#include <Runtime/SettingDialogNativePages.h>

namespace KaGuYa::Start::Runtime
{
GuiDialogNativeBody5Ch* SettingDialogNativePages::ConstructPage01At4F1EE0(void* raw,GuiDialogNativeBody5Ch* p,std::int32_t id)
{return &ConstructSettingPage01NativeAt4F1EE0(raw,p,id,preview_)->common;}
GuiDialogNativeBody5Ch* SettingDialogNativePages::ConstructPage02At4F23B0(void* raw,GuiDialogNativeBody5Ch* p,std::int32_t id)
{return &ConstructSettingPage02NativeAt4F23B0(raw,p,id)->common;}
GuiDialogNativeBody5Ch* SettingDialogNativePages::ConstructPage03At4F2910(void* raw,GuiDialogNativeBody5Ch* p,std::int32_t id)
{return &ConstructSettingPage03NativeAt4F2910(raw,p,id)->common;}
GuiDialogNativeBody5Ch* SettingDialogNativePages::ConstructPage04At4F2FE0(void* raw,GuiDialogNativeBody5Ch* p,std::int32_t id)
{return &ConstructSettingPage04NativeAt4F2FE0(raw,p,id)->common;}
}

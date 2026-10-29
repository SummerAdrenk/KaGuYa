#include <Runtime/SaveLoadDialogNativeConstructionRuntime.h>

namespace KaGuYa::Start::Runtime
{
GuiDialogNativeBody5Ch* SaveLoadDialogNativeConstructionRuntime::ConstructAt4EB950(
    void* raw,std::uint32_t mode,std::uint32_t rootByte)
{
    SaveLoadDialogNativeRuntimeScope scope(services_);
    return &ConstructSaveLoadDialogNativeAt4EB950(raw,mode,rootByte,services_.construction)->common;
}
}

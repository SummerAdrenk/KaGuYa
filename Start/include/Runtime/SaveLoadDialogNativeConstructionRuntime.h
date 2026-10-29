#pragma once
#include <Runtime/SaveLoadDialogNativeRuntime.h>
#include <Runtime/SaveLoadSceneNative.h>

namespace KaGuYa::Start::Runtime
{
class SaveLoadDialogNativeConstructionRuntime final:public ISaveLoadDialogNativeConstruction
{
public:
    explicit SaveLoadDialogNativeConstructionRuntime(SaveLoadDialogNativeRuntimeServices& services) noexcept:services_(services){}
    GuiDialogNativeBody5Ch* ConstructAt4EB950(void*,std::uint32_t,std::uint32_t) override;
    SaveLoadDialogNativeRuntimeServices* NativeRuntimeServices() const noexcept override{return &services_;}
private:
    SaveLoadDialogNativeRuntimeServices& services_;
};
}

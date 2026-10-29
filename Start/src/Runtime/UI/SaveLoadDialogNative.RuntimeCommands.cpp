#include <Runtime/SaveLoadDialogNativeRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=SaveLoadDialogNativeBody184h;
SaveLoadDialogNativeRuntimeServices& Services(void* raw) noexcept
{return *static_cast<SaveLoadDialogNativeRuntimeServices*>(raw);}
std::int32_t ReturnTitle(Body&,void* raw)
{return ReturnToTitleFromSaveLoadNativeAt51E320(Services(raw).returnTitle);}
std::int32_t Delete(Body& body,void* raw)
{auto& services=Services(raw);return ConfirmSaveLoadDeleteNativeAt51B8F0(body,services.deleteConfirmation,services.deletion);}
std::int32_t Save(Body& body,void* raw)
{return ProcessSaveLoadSaveConfirmationNativeAt525480(body,Services(raw).persistence);}
std::int32_t Load(Body& body,void* raw)
{auto& services=Services(raw);return ProcessSaveLoadLoadConfirmationNativeAt520AB0(body,services.persistence,services.memory);}
}
SaveLoadDialogNativeCommandEntries SaveLoadDialogNativeRuntimeCommands(SaveLoadDialogNativeRuntimeServices& services) noexcept
{return {ReturnTitle,Delete,Save,Load,&services};}
}

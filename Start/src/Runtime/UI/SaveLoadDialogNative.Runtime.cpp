#include <Runtime/SaveLoadDialogNativeRuntime.h>
#include <Runtime/GuiDialogIntoNative.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=SaveLoadDialogNativeBody184h;using Common=GuiDialogNativeBody5Ch;
thread_local SaveLoadDialogNativeRuntimeServices* activeServices;
Body& Dialog(Common* body) noexcept{return *reinterpret_cast<Body*>(body);}
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
GuiDirtyRectNativeBody80h* __fastcall Dirty(Common* body,void*) noexcept
{
    // 50A020先以base判空，control/stride/bias之后重新读base；不同于509ED0固定EDX。
    const auto& slot=body->dirtyAt50;if(!Read(slot.baseAt00))return nullptr;
    auto* control=static_cast<EnvironmentObjectReferenceControl*>(Read(slot.controlAt08));
    const auto stride=control?Read(control->strideAt0C):0u;
    const auto bias=Read(slot.biasAt04);
    return reinterpret_cast<GuiDirtyRectNativeBody80h*>(Read(slot.baseAt00)+bias*stride);
}
Common* __fastcall Delete(Common* body,void*,std::uint32_t flags) noexcept
{
    auto& services=RequireSaveLoadDialogNativeRuntimeServices();
    DeleteSaveLoadDialogNativeAt4FDD70(&Dialog(body),flags,services.construction);return body;
}
void __fastcall Begin(Common* body,void*)
{
    auto& services=RequireSaveLoadDialogNativeRuntimeServices();
    BeginSaveLoadDialogNativeAt50E8B0(Dialog(body),services.begin,services.pages);
}
std::int32_t __fastcall Poll(Common* body,void*)
{
    auto& services=RequireSaveLoadDialogNativeRuntimeServices();
    return PollSaveLoadDialogNativeAt52F7A0(Dialog(body),services.catalog,services.pages,
        services.transition,services.transit,SaveLoadDialogNativeRuntimeCommands(services));
}
std::int32_t __fastcall Result(Common* body,void*) noexcept
{return std::bit_cast<std::int32_t>(Read(Dialog(body).resultAt68));}
void __fastcall Flush(Common* body,void*)
{FlushSaveLoadDialogDirtyNativeAt525FB0(Dialog(body),RequireSaveLoadDialogNativeRuntimeServices().screen);}
void __fastcall Composite(Common* body,void*,YtlReferenceSlotRuntime owned,const RECT* clip)
{
    // 参数在服务拒绝前已经交付；Ref12无C++自动减持，保护原524889的EH资格。
    struct Unwind{YtlReferenceSlotRuntime& slot;bool armed=true;~Unwind() noexcept{if(armed)ReleaseYtlReference(slot);}} unwind{owned};
    auto& drawing=RequireSaveLoadDialogNativeRuntimeServices().drawing;
    unwind.armed=false;
    CompositeSaveLoadDialogNativeAt524860(Dialog(body),owned,clip,drawing);
}
void __fastcall Draw(Common* body,void*,const RECT* clip)
{DrawSaveLoadDialogNativeAt524800(Dialog(body),clip,RequireSaveLoadDialogNativeRuntimeServices().drawing);}
}
SaveLoadDialogNativeRuntimeScope::SaveLoadDialogNativeRuntimeScope(SaveLoadDialogNativeRuntimeServices& services) noexcept
    :previous_(activeServices),construction_(services.construction),pages_(services.pages),screen_(&services.screen)
{activeServices=&services;}
SaveLoadDialogNativeRuntimeScope::~SaveLoadDialogNativeRuntimeScope() noexcept{activeServices=previous_;}
SaveLoadDialogNativeRuntimeServices& RequireSaveLoadDialogNativeRuntimeServices()
{
    if(!activeServices)throw std::logic_error("SaveLoad184 native runtime services are not bound");
    return *activeServices;
}
const SaveLoadDialogNativeVtable37& SaveLoadDialogNativeVtable() noexcept
{
    static const auto table=[]
    {
        const auto& into=GuiDialogIntoBaseVtable();
        SaveLoadDialogNativeVtable37 value{into.common,into.appendDialogAt8C,into.findDialogAt90};
#define BIND(field,entry) value.common.field=reinterpret_cast<decltype(value.common.field)>(&entry)
        BIND(dirtyAt00,Dirty);BIND(deletingDestructorAt04,Delete);BIND(beginAt44,Begin);
        BIND(pollAt48,Poll);BIND(resultAt50,Result);BIND(flushDirtyAt58,Flush);
        BIND(compositeAt5C,Composite);BIND(drawAt60,Draw);
#undef BIND
        return value;
    }();return table;
}
}

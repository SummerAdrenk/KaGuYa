#include "SaveLoadPageNative.Internal.h"
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace SaveLoadPageNativeDetail;
thread_local SaveLoadPageNativeServices* activeServices;
Common* __fastcall Delete00(Common* body,void*,std::uint32_t flags) noexcept
{
    body->vptrAt00=&SaveLoadPage00NativeVtable().inheritedGui35;
    DestroySaveLoadPageSecondNativeAt4F9470(&Page(body));
    if(flags&1u)::operator delete(body,sizeof(Body));return body;
}
Common* __fastcall Delete01(Common* body,void*,std::uint32_t flags) noexcept
{
    body->vptrAt00=&SaveLoadPage01NativeVtable().inheritedGui35;
    DestroySaveLoadPageSecondNativeAt4F9470(&Page(body));
    if(flags&1u)::operator delete(body,sizeof(Body));return body;
}
std::int32_t __fastcall End00(Common* body,void*){return EndSaveLoadPage00NativeAt509420(body);}
std::int32_t __fastcall End01(Common* body,void*){return EndSaveLoadPage01NativeAt509490(body);}
std::int32_t __fastcall Change00(Common* body,void*,std::int32_t page,std::int32_t notify,std::int32_t force)
{return ChangeSaveLoadPage00NativeAt5339C0(body,page,notify,force);}
std::int32_t __fastcall Change01(Common* body,void*,std::int32_t page,std::int32_t notify,std::int32_t force)
{return ChangeSaveLoadPage01NativeAt5345A0(body,page,notify,force);}
}
__declspec(noinline) SaveLoadPageNativeScope::SaveLoadPageNativeScope(SaveLoadPageNativeServices& services) noexcept
    :previous_(activeServices),common_(services.common){activeServices=&services;}
__declspec(noinline) SaveLoadPageNativeScope::~SaveLoadPageNativeScope() noexcept{activeServices=previous_;}
__declspec(noinline) SaveLoadPageNativeServices& RequireSaveLoadPageNativeServices()
{
    if(!activeServices)throw std::logic_error("SaveLoadPage native services are not bound");
    return *activeServices;
}
const SaveLoadPageSecondNativeVtable36& SaveLoadPage00NativeVtable() noexcept
{
    static const auto table=[]
    {
        auto value=SaveLoadPageSecondNativeVtable();
        value.inheritedGui35.deletingDestructorAt04=reinterpret_cast<decltype(value.inheritedGui35.deletingDestructorAt04)>(&Delete00);
        value.inheritedGui35.endAt4C=reinterpret_cast<decltype(value.inheritedGui35.endAt4C)>(&End00);
        value.changePageAt8C=reinterpret_cast<decltype(value.changePageAt8C)>(&Change00);
        return value;
    }();return table;
}
const SaveLoadPageSecondNativeVtable36& SaveLoadPage01NativeVtable() noexcept
{
    static const auto table=[]
    {
        auto value=SaveLoadPageSecondNativeVtable();
        value.inheritedGui35.deletingDestructorAt04=reinterpret_cast<decltype(value.inheritedGui35.deletingDestructorAt04)>(&Delete01);
        value.inheritedGui35.endAt4C=reinterpret_cast<decltype(value.inheritedGui35.endAt4C)>(&End01);
        value.changePageAt8C=reinterpret_cast<decltype(value.changePageAt8C)>(&Change01);
        return value;
    }();return table;
}
namespace SaveLoadPageNativeDetail
{
SaveFileStateNativeArchive* CaptureSave(SaveLoadPageNativeServices& services)
{
    auto& source=services.common.source;auto* environment=source.CaptureEnvironmentAt4180C0();
    return source.SaveAt417990(environment,6);
}
}
std::int32_t EndSaveLoadPage00NativeAt509420(Common* body)
{
    // C++赋值先算右侧，不能用一行表达式把page读点移到4180C0之前。
    auto* save=CaptureSave(RequireSaveLoadPageNativeServices());
    const auto page=Read(Page(body).pageAt7C);save->regularSelectedPageAt0C=page;
    return 1;
}
std::int32_t EndSaveLoadPage01NativeAt509490(Common* body)
{
    if(body->vptrAt00->idAt0C(body)==3)
    {
        auto* save=CaptureSave(RequireSaveLoadPageNativeServices());
        const auto page=Read(Page(body).pageAt7C);save->regularDisplayedPageAt10=page;
    }
    return 1;
}
}

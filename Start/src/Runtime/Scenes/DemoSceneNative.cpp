#include <Runtime/DemoSceneNative.h>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body=DemoSceneNativeBody18h;using Table=DemoSceneNativeVtable;
thread_local DemoSceneNativeServices* active=nullptr;
DemoSceneNativeServices& Services()
{
    if(!active)throw std::logic_error("native Demo scene requires its external service scope");
    return *active;
}
template<class T>T Read(const T& value) noexcept{return *static_cast<const volatile T*>(&value);}
[[noreturn]] void Pure(){_purecall();std::terminate();}
std::uint8_t __fastcall PureRun(Body*,void*,std::int32_t){Pure();}
void __fastcall PureFinish(Body*,void*){Pure();}
struct TaskTable final
{
    decltype(Table::runAt00) run;
    decltype(Table::deleteAt04) destroy;
};
Body* __fastcall DeleteTask(Body* body,void*,std::uint32_t flags) noexcept;
const TaskTable task{
    reinterpret_cast<decltype(TaskTable::run)>(&PureRun),
    reinterpret_cast<decltype(TaskTable::destroy)>(&DeleteTask)};
void Retire(Body& body) noexcept{body.vptrAt00=reinterpret_cast<const Table*>(&task);}
Body* __fastcall DeleteTask(Body* body,void*,std::uint32_t flags) noexcept
{
    Retire(*body);
    // 469A70是已退役ITask的4-byte删除口，不沿用派生对象18h。
    if(flags&1u)::operator delete(body,std::size_t{4});
    return body;
}
std::uint8_t __fastcall Run(Body* body,void*,std::int32_t argument)
{return RunDemoSceneNativeAt4C3630(*body,argument,Services());}
void __fastcall Update(Body* body,void*){UpdateDemoSceneNativeAt4B5FC0(*body,Services());}
void __fastcall Draw(Body*,void*){}
Body* Delete(Body* body,std::uint32_t flags,DemoSceneNativeServices& services) noexcept
{
    DestroyDemoSceneNativeAt4A3C20(*body,services);
    if(flags&1u)::operator delete(body,std::size_t{0x18});
    return body;
}
Body* __fastcall DeleteBase(Body* body,void*,std::uint32_t flags) noexcept
{return Delete(body,flags,Services());}
Body* __fastcall DeleteLogo(Body* body,void*,std::uint32_t flags) noexcept
{return DeleteLogoNativeAt4A6B10(body,flags,Services());}
Body* __fastcall DeleteEnding(Body* body,void*,std::uint32_t flags) noexcept
{return DeleteEndingNativeAt4A6890(body,flags,Services());}
constexpr char title[]="\x83\x5E\x83\x43\x83\x67\x83\x8B";
void __fastcall FinishLogo(Body*,void*)
{
    auto& services=Services();services.logo.ClearRootObjectAt24Binding();services.logo.QueueScene(title);
}
void __fastcall FinishEnding(Body*,void*)
{
    auto& services=Services();services.ending.WriteDescriptor2DwordAt04(1);
    services.ending.ClearScreen(0);services.ending.InvalidateScreenRect(nullptr);
    services.ending.AdvanceActiveSceneFromPending();
}
const Table base{
    reinterpret_cast<decltype(Table::runAt00)>(&Run),reinterpret_cast<decltype(Table::deleteAt04)>(&DeleteBase),
    reinterpret_cast<decltype(Table::updateAt08)>(&Update),reinterpret_cast<decltype(Table::drawAt0C)>(&Draw),
    reinterpret_cast<decltype(Table::finishAt10)>(&PureFinish)};
const Table logo{
    reinterpret_cast<decltype(Table::runAt00)>(&Run),reinterpret_cast<decltype(Table::deleteAt04)>(&DeleteLogo),
    reinterpret_cast<decltype(Table::updateAt08)>(&Update),reinterpret_cast<decltype(Table::drawAt0C)>(&Draw),
    reinterpret_cast<decltype(Table::finishAt10)>(&FinishLogo)};
const Table ending{
    reinterpret_cast<decltype(Table::runAt00)>(&Run),reinterpret_cast<decltype(Table::deleteAt04)>(&DeleteEnding),
    reinterpret_cast<decltype(Table::updateAt08)>(&Update),reinterpret_cast<decltype(Table::drawAt0C)>(&Draw),
    reinterpret_cast<decltype(Table::finishAt10)>(&FinishEnding)};
DemoSequenceNativeBodyA4h* Sequence(Body& body)
{return ResolveEnvironmentObjectReference<DemoSequenceNativeBodyA4h>(body.sequenceAt08);}
class EndingName final
{
public:
    explicit EndingName(ATL::CStringData* header) noexcept:header_(header){}
    ~EndingName() noexcept{if(header_)ReleaseRuntimeCStringHeader(header_);}
    const char* Chars() const noexcept{return reinterpret_cast<const char*>(header_+1);}
    void ReleaseNormal()
    {auto* header=header_;header_=nullptr;ReleaseRuntimeCStringHeader(header);}
private:
    ATL::CStringData* header_;
};
struct BodyUnwind final
{
    Body& body;DemoSceneNativeServices& services;bool armed=true;
    ~BodyUnwind() noexcept{if(armed)DestroyDemoSceneNativeAt4A3C20(body,services);}
};
struct LookupUnwind final
{
    YtlReferenceSlotRuntime& value;bool armed=true;
    ~LookupUnwind() noexcept{if(armed)ReleaseParamsDemoLookupAt4A32A0(value);}
    void ReleaseNormal() noexcept{armed=false;ReleaseParamsDemoLookupAt4A32A0(value);}
};
template<class Construct>Body* NewScene(DemoSceneNativeServices& services,Construct&& construct)
{
    auto* raw=services.sequences.memory.AllocateBody(0x18);
    if(!raw)return nullptr;
    struct RawUnwind final
    {
        void* raw;IEnvironmentRootMemory& memory;bool armed=true;
        ~RawUnwind() noexcept{if(armed)memory.DeleteBody(raw,0x18);}
    } unwind{raw,services.sequences.memory};
    auto* completed=construct(*::new(raw) Body,services);unwind.armed=false;
    return completed;
}
}
DemoSceneNativeScope::DemoSceneNativeScope(DemoSceneNativeServices& services) noexcept
    :previous_(active),sequences_(services.sequences){active=&services;}
DemoSceneNativeScope::~DemoSceneNativeScope() noexcept{active=previous_;}
const Table& DemoSceneBaseMethods() noexcept{return base;}
const Table& LogoSceneNativeMethods() noexcept{return logo;}
const Table& EndingSceneNativeMethods() noexcept{return ending;}
Body* ConstructDemoSceneNativeAt49B5C0(Body& body,DemoSceneNativeServices& services)
{
    DemoSceneNativeScope scope(services);
    body.vptrAt00=&base;body.sequenceAt08={};
    // 49B617/61D：WORD14=0100后phase04=0；16/17分配字节不写。
    *reinterpret_cast<volatile std::uint16_t*>(reinterpret_cast<std::byte*>(&body)+0x14)=0x0100;
    body.phaseAt04=0;
    BodyUnwind unwind{body,services};
    AdoptDemoSequenceNativeAt49B644(body.sequenceAt08,NewDemoSequenceNativeAt49B624(services.sequences),services.sequences.memory);
    unwind.armed=false;
    return &body;
}
Body* ConstructLogoNativeAt4AAFEF(Body& body,DemoSceneNativeServices& services)
{
    DemoSceneNativeScope scope(services);
    ConstructDemoSceneNativeAt49B5C0(body,services);
    body.vptrAt00=&logo;
    BodyUnwind unwind{body,services};
    constexpr char name[]="\x83\x8D\x83\x53";
    YtlReferenceSlotRuntime found;
    FindParamsDemoAt4ACC40(services.params.CaptureGameSystemAt418480(),name,found);
    LookupUnwind lookup{found};BindDemoSequenceNativeAt4C2220(*Sequence(body),found);lookup.ReleaseNormal();
    services.logo.ClearGlobalDescriptorTarget(2);
    unwind.armed=false;
    return &body;
}
Body* ConstructEndingNativeAt49DA10(Body& body,DemoSceneNativeServices& services)
{
    DemoSceneNativeScope scope(services);
    ConstructDemoSceneNativeAt49B5C0(body,services);body.vptrAt00=&ending;
    BodyUnwind unwind{body,services};
    const auto* source=services.ending.CaptureDemoNameSlotAt49DA53();
    // 原49DA7E直接读chars；不将空名字槽转换成invalid_parameter。
    auto* chars=*reinterpret_cast<char* const*>(source);
    EndingName name(CloneRuntimeCStringDataAt414540(reinterpret_cast<ATL::CStringData*>(chars)-1));
    YtlReferenceSlotRuntime found;
    FindParamsDemoAt4ACC40(services.params.CaptureGameSystemAt418480(),name.Chars(),found);
    LookupUnwind lookup{found};BindDemoSequenceNativeAt4C2220(*Sequence(body),found);lookup.ReleaseNormal();
    name.ReleaseNormal();
    services.ending.StopRootSlot24Sounds();services.ending.StopRootSlot24SoundsUsingDescriptor2ListAt30();
    services.ending.WriteDescriptor2DwordAt04(0);services.ending.WriteDescriptor2DwordAt14(1);
    unwind.armed=false;
    return &body;
}
Body* NewLogoNativeAt4AAFF1(DemoSceneNativeServices& services)
{return NewScene(services,ConstructLogoNativeAt4AAFEF);}
Body* NewEndingNativeAt4AB782(DemoSceneNativeServices& services)
{return NewScene(services,ConstructEndingNativeAt49DA10);}
void DestroyDemoSceneNativeAt4A3C20(Body& body,DemoSceneNativeServices& services) noexcept
{
    DemoSceneNativeScope scope(services);
    ReleaseYtlReference(body.sequenceAt08);Retire(body);
}
Body* DeleteLogoNativeAt4A6B10(Body* body,std::uint32_t flags,DemoSceneNativeServices& services) noexcept
{return Delete(body,flags,services);}
Body* DeleteEndingNativeAt4A6890(Body* body,std::uint32_t flags,DemoSceneNativeServices& services) noexcept
{return Delete(body,flags,services);}
std::uint8_t RunDemoSceneNativeAt4C3630(Body& body,std::int32_t argument,DemoSceneNativeServices& services)
{
    (void)argument;DemoSceneNativeScope scope(services);
    body.vptrAt00->updateAt08(&body);
    // 4C3638 Update之后重新读vptr，不能缓存先前Draw或根据类型直接调空函数。
    body.vptrAt00->drawAt0C(&body);return 1;
}
void UpdateDemoSceneNativeAt4B5FC0(Body& body,DemoSceneNativeServices& services)
{
    DemoSceneNativeScope scope(services);
    if(Read(body.inputEnabledAt15)&&!Read(body.completionAt14)&&Read(body.phaseAt04)!=2&&
        services.logo.ShouldRequestCompletion()) {body.completionAt14=1;body.phaseAt04=2;}
    switch(Read(body.phaseAt04))
    {
    case 0:
        switch(RunDemoSequenceNativeAt4BDAE0(*Sequence(body),Read(body.completionAt14),services.sequences))
        {
        case -1:services.logo.QueueScene(title);break;
        case 0:body.phaseAt04=2;break;
        case 1:body.phaseAt04=1;break;
        case 2:UpdateDemoSpritesNativeAt4A7690(*Sequence(body),services.sequences);break;
        default:break;
        }
        break;
    case 1:
        if(AdvanceDemoTransitionNativeAt4C38A0(*Sequence(body),services.sequences))
        {body.phaseAt04=0;if(Read(body.completionAt14))body.phaseAt04=2;}
        break;
    case 2:body.vptrAt00->finishAt10(&body);break;
    default:break;
    }
}
}

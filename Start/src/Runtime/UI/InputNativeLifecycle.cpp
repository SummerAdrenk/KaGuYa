#include "InputNativeObjects.Internal.h"
#include <Runtime/ProcessInputRuntime.h>
#include <cstring>
#include <new>
#include <stdexcept>
#include <exception>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local InputNativeServices* activeServices=nullptr;
using Deleting=void* (__thiscall*)(void*,std::uint32_t);
struct ManagerTable{Deleting deleting;};
void* __fastcall DeleteMouseManager(void* p,void*,std::uint32_t f)
{return DeleteMouseManagerNativeAt450730(*static_cast<MouseManagerNativeBody3Ch*>(p),f);}
void* __fastcall DeleteKeyboardManager(void* p,void*,std::uint32_t f)
{return DeleteKeyboardManagerNativeAt450610(*static_cast<KeyboardManagerNativeBody438h*>(p),f);}
const ManagerTable mouseMethods{reinterpret_cast<Deleting>(&DeleteMouseManager)};
const ManagerTable keyboardMethods{reinterpret_cast<Deleting>(&DeleteKeyboardManager)};
template<class Body,std::size_t DeleteSlot>
class InputControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit InputControl(Body* body) noexcept:EnvironmentObjectReferenceControl(body,UninitializedFieldsTag{}){}
    void DestroyPayloadAtVslot00() override
    {
        auto* body=static_cast<Body*>(payloadAt14);
        if(!body)return;
        auto* table=*reinterpret_cast<const std::byte* const*>(body);
        Deleting destroy;std::memcpy(&destroy,table+DeleteSlot,4);destroy(body,1);
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override{RetireEnvironmentObjectControl(this,flags);}
};
template<class Body,std::size_t Slot>
void Install(YtlReferenceSlotRuntime& reference,Body* created,InputNativeServices& services)
{
    ReleaseYtlReference(reference);
    if(!created)return;
    // 450875/450961/450A1D检查的是释放流程已清零的ECX，不再读取reference.control。
    reference.biasAt04=0;reference.baseAt00=reinterpret_cast<std::uintptr_t>(created);
    auto* raw=services.memory.AllocateBody(0x18);
    auto* control=raw?::new(raw) InputControl<Body,Slot>(created):nullptr;
    reference.controlAt08=control;
    reference.controlAt08->referenceCountAt04=1;
    reference.controlAt08->owningAt08=1;
    static_cast<EnvironmentObjectReferenceControl*>(reference.controlAt08)->strideAt0C=sizeof(Body);
    static_cast<EnvironmentObjectReferenceControl*>(reference.controlAt08)->countAt10=1;
}
template<class Body,class Construct>
Body* NewBody(InputNativeServices& services,Construct&& construct)
{
    void* raw=services.memory.AllocateBody(sizeof(Body));
    struct Unwind
    {
        InputNativeServices& services;void* raw;bool active=true;
        ~Unwind() noexcept{if(active)services.memory.DeleteBody(raw,sizeof(Body));}
    } guard{services,raw};
    Body* body=raw?construct(*::new(raw) Body):nullptr;
    guard.active=false;return body;
}
MouseRawNativeBody10h* NewMouseRaw(InputNativeServices& services)
{
    return NewBody<MouseRawNativeBody10h>(services,[&](auto& body){
        body.vptrAt00=MouseRawNativeMethods();
        std::memset(body.pressedAt04,0,3);std::memset(body.latchedAt07,0,3);
        body.wheelAt0A=0;body.remainderAt0C=0;
        services.hooks.CaptureAt450E20().AddRaw(&body,services);
        return &body;
    });
}
KeyboardRawNativeBody104h* NewKeyboardRaw(InputNativeServices& services)
{
    return NewBody<KeyboardRawNativeBody104h>(services,[&](auto& body){
        body.vptrAt00=KeyboardRawNativeMethods();std::memset(body.keysAt04,0,256);
        services.hooks.CaptureAt450E20().AddRaw(&body,services);
        return &body;
    });
}
void DestroyActions(KeyboardManagerNativeBody438h& body,InputNativeServices& services) noexcept
{
    // EH vector destructor iterator严格逆序，raw Keyboard在88项退役期间仍已注册。
    try{for(std::size_t index=0x58;index;--index)DestroyKeyboardBindingsNativeAt4500D0(body.actionsAt14[index-1],services.memory);}
    catch(...){std::terminate();}
}
}
InputNativeServices& CurrentInputNativeServices()
{
    // 原raw虚槽直接使用进程平台服务；scope只显式借入受控服务，不是失败后的换路。
    return activeServices?*activeServices:ProcessInputNativeServices();
}
InputNativeScope::InputNativeScope(InputNativeServices& services) noexcept:previous_(activeServices){activeServices=&services;}
InputNativeScope::~InputNativeScope() noexcept{activeServices=previous_;}
MouseManagerNativeBody3Ch* ConstructMouseManagerNativeAt44FE50(MouseManagerNativeBody3Ch& body,InputNativeServices& services)
{
    InputNativeScope scope(services);body.vptrAt00=&mouseMethods;body.rawAt04={};
    body.logicalAt18={};body.clientAt20={};
    struct MemberUnwind{MouseManagerNativeBody3Ch& body;bool active=true;~MemberUnwind() noexcept{if(active)ReleaseYtlReference(body.rawAt04);}} guard{body};
    auto* raw=NewMouseRaw(services);Install<MouseRawNativeBody10h,4>(body.rawAt04,raw,services);
    body.pressedAt10={};body.previousAt30={};body.latchedAt13={};
    body.secondaryAt2C=0;body.suppressionAt28=0;body.logicalAt18={};body.movedAt33=0;body.wheelAt34=0;
    // +16/+17和+38..+3B按原构造不写，不能借initialForceClearLeft参数清成现代默认。
    guard.active=false;return &body;
}
KeyboardManagerNativeBody438h* ConstructKeyboardManagerNativeAt44FC70(KeyboardManagerNativeBody438h& body,
    const std::uint8_t* table,InputNativeServices& services)
{
    InputNativeScope scope(services);body.vptrAt00=&keyboardMethods;body.rawAt04={};
    for(auto& action:body.actionsAt14)action={};
    struct MemberUnwind
    {
        KeyboardManagerNativeBody438h& body;InputNativeServices& services;bool active=true;
        ~MemberUnwind() noexcept{if(active){DestroyActions(body,services);ReleaseYtlReference(body.rawAt04);}}
    } guard{body,services};
    auto* raw=NewKeyboardRaw(services);Install<KeyboardRawNativeBody104h,12>(body.rawAt04,raw,services);
    for(auto& action:body.actionsAt14)action.end=action.begin;
    body.bufferAt10=0;body.actionCountAt434=0;
    while(table[0]!=0xFF)
    {
        const auto index=static_cast<std::uint32_t>(table[0]);
        if(index<0x58)
        {
            const KeyboardBindingNative3 binding{table[1],{0,0}};
            AppendKeyboardBindingNativeAt44FA70(body.actionsAt14[index],binding,services.memory);
            if(static_cast<std::int32_t>(index)>=body.actionCountAt434)body.actionCountAt434=static_cast<std::int32_t>(index+1);
        }
        table+=2;
    }
    guard.active=false;return &body;
}
CursorNativeBody10h* ConstructCursorNativeAt4508EB(CursorNativeBody10h& body,HWND window,InputNativeServices& services)
{
    InputNativeScope scope(services);body.vptrAt00=CursorNativeMethods();body.currentAt04=nullptr;body.windowAt0C=window;
    body.previousAt08=services.cursor.Get();
    using Reset=std::uint8_t(__thiscall*)(void*);
    Reset reset;std::memcpy(&reset,reinterpret_cast<const std::byte*>(body.vptrAt00)+8,4);static_cast<void>(reset(&body));
    services.hooks.CaptureAt450E20().AddRaw(&body,services);
    return &body;
}
MouseManagerNativeBody3Ch* DeleteMouseManagerNativeAt450730(MouseManagerNativeBody3Ch& body,std::uint32_t flags) noexcept
{
    try
    {
        body.vptrAt00=&mouseMethods;ReleaseYtlReference(body.rawAt04);
        if(flags&1)::operator delete(&body,std::size_t{0x3C});return &body;
    }
    catch(...){std::terminate();}
}
void DestroyKeyboardManagerNativeAt4501F0(KeyboardManagerNativeBody438h& body) noexcept
{
    try{body.vptrAt00=&keyboardMethods;DestroyActions(body,CurrentInputNativeServices());ReleaseYtlReference(body.rawAt04);}
    catch(...){std::terminate();}
}
KeyboardManagerNativeBody438h* DeleteKeyboardManagerNativeAt450610(KeyboardManagerNativeBody438h& body,std::uint32_t flags) noexcept
{
    DestroyKeyboardManagerNativeAt4501F0(body);
    if(flags&1)::operator delete(&body,std::size_t{0x438});return &body;
}
void InitializeInputNativeAt4507E0(InputRootNativeBody24h& body,HWND window,const std::uint8_t* bindings,InputNativeServices& services)
{
    InputNativeScope scope(services);
    try
    {
        auto* mouse=NewBody<MouseManagerNativeBody3Ch>(services,[&](auto& object){return ConstructMouseManagerNativeAt44FE50(object,services);});
        Install<MouseManagerNativeBody3Ch,0>(body.mouseAt00,mouse,services);
        if(window)
        {
            auto* cursor=NewBody<CursorNativeBody10h>(services,[&](auto& object){return ConstructCursorNativeAt4508EB(object,window,services);});
            Install<CursorNativeBody10h,12>(body.cursorAt0C,cursor,services);
        }
        auto* keyboard=NewBody<KeyboardManagerNativeBody438h>(services,[&](auto& object){return ConstructKeyboardManagerNativeAt44FC70(object,bindings,services);});
        Install<KeyboardManagerNativeBody438h,0>(body.keyboardAt18,keyboard,services);
    }
    catch(...)
    {
        // 450A86在handler里调用451150；清理异常外传，不被本catch再次截获。
        ReleaseInputMembersAt451150(body);
    }
}
void ReleaseInputNativeAt451150(InputRootNativeBody24h& body,InputNativeServices& services)
{InputNativeScope scope(services);ReleaseInputMembersAt451150(body);}
}

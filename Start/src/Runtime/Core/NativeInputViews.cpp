#include <Runtime/NativeInputRuntime.h>
#include <bit>
#include <cstring>
namespace KaGuYa::Start::Runtime
{
namespace
{
using Mouse=MouseManagerNativeBody3Ch;using Keyboard=KeyboardManagerNativeBody438h;
template<class T>T Read(const T& value) noexcept{return static_cast<const volatile T&>(value);}
const Platform::MouseViewMethods mouseCalls{
    [](const void* p,std::size_t i){return Read(static_cast<const Mouse*>(p)->pressedAt10[i]);},
    [](const void* p,std::size_t i){return Read(static_cast<const Mouse*>(p)->previousAt30[i]);},
    [](const void* p,std::size_t i){return Read(static_cast<const Mouse*>(p)->latchedAt13[i]);},
    [](const void* p){return Read(static_cast<const Mouse*>(p)->wheelAt34);},
    [](const void* p)->std::int32_t{return Read(static_cast<const Mouse*>(p)->logicalAt18.x);},
    [](const void* p)->std::int32_t{return Read(static_cast<const Mouse*>(p)->logicalAt18.y);},
    [](const void* p){const auto& v=static_cast<const Mouse*>(p)->logicalAt18;return POINT{Read(v.x),Read(v.y)};},
    [](const void* p){const auto& v=static_cast<const Mouse*>(p)->clientAt20;return POINT{Read(v.x),Read(v.y)};},
    [](const void* p){return Read(static_cast<const Mouse*>(p)->movedAt33)!=0;},
    [](const void* p){return Read(static_cast<const Mouse*>(p)->suppressionAt28);},
    [](const void* p){return Read(static_cast<const Mouse*>(p)->secondaryAt2C);},
    [](const void* p,std::size_t i,std::uint8_t v){const_cast<Mouse*>(static_cast<const Mouse*>(p))->latchedAt13[i]=v;},
    [](const void* p,std::uint32_t v){const_cast<Mouse*>(static_cast<const Mouse*>(p))->suppressionAt28=v;},
    [](const void* p,std::uint32_t v){const_cast<Mouse*>(static_cast<const Mouse*>(p))->secondaryAt2C=v;},
    [](const void* p){const_cast<Mouse*>(static_cast<const Mouse*>(p))->unwrittenAt38[0]=1;}};
const KeyboardBindingNative3* Binding(const void* p,std::size_t action,std::size_t index)
{
    const auto& storage=static_cast<const Keyboard*>(p)->actionsAt14[action];
    return reinterpret_cast<const KeyboardBindingNative3*>(reinterpret_cast<std::uintptr_t>(Read(storage.begin))+index*3u);
}
const Platform::KeyboardViewMethods keyboardCalls{
    [](const void*,std::int32_t action)->std::size_t{return static_cast<std::uint32_t>(action);},
    [](const void* p)->std::size_t{return static_cast<std::uint32_t>(Read(static_cast<const Keyboard*>(p)->actionCountAt434));},
    [](const void*)->std::size_t{return 0x58;},
    [](const void* p,std::size_t a)->std::size_t{
        const auto& v=static_cast<const Keyboard*>(p)->actionsAt14[a];
        return static_cast<std::uint32_t>(std::bit_cast<std::int32_t>(std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(v.end)))-
            std::uint32_t(reinterpret_cast<std::uintptr_t>(Read(v.begin))))/3);},
    [](const void* p,std::size_t a,std::size_t b){return Read(Binding(p,a,b)->key);},
    [](const void* p,std::size_t a,std::size_t b){
        const auto address=reinterpret_cast<std::uintptr_t>(Binding(p,a,b))+Read(static_cast<const Keyboard*>(p)->bufferAt10)+1u;
        return *reinterpret_cast<const volatile std::uint8_t*>(address);},
    [](const void* p,std::size_t a,std::size_t b){
        const auto address=reinterpret_cast<std::uintptr_t>(Binding(p,a,b))-Read(static_cast<const Keyboard*>(p)->bufferAt10)+2u;
        return *reinterpret_cast<const volatile std::uint8_t*>(address);}};
const Platform::CursorViewMethods cursorCalls{
    [](void* p,void* c,LPCSTR n,bool system,bool force){return SetCursorNativeAt451200(*static_cast<CursorNativeBody10h*>(p),n,system,force,*static_cast<InputNativeServices*>(c))!=0;},
    [](void* p,void* c){
        InputNativeScope scope(*static_cast<InputNativeServices*>(c));
        using Reset=std::uint8_t(__thiscall*)(void*);Reset reset;
        std::memcpy(&reset,reinterpret_cast<const std::byte*>(static_cast<CursorNativeBody10h*>(p)->vptrAt00)+8,4);
        return reset(p)!=0;},
    [](void* p,void*){return Read(static_cast<CursorNativeBody10h*>(p)->currentAt04);}};
}
Platform::MouseView NativeInputRuntime::BorrowMouse() const
{return {ResolveEnvironmentObjectReference<MouseManagerNativeBody3Ch>(owner_.CaptureAt418210()->mouseAt00),&mouseCalls};}
Platform::KeyboardView NativeInputRuntime::BorrowKeyboard() const
{return {ResolveEnvironmentObjectReference<KeyboardManagerNativeBody438h>(owner_.CaptureAt418210()->keyboardAt18),&keyboardCalls};}
Platform::CursorView NativeInputRuntime::BorrowCursor() const
{return {ResolveEnvironmentObjectReference<CursorNativeBody10h>(owner_.CaptureAt418210()->cursorAt0C),&services_,&cursorCalls};}
}

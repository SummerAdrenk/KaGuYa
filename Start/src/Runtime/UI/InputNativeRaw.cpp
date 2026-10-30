#include "InputNativeObjects.Internal.h"
#include <bit>
#include <cstring>
#include <exception>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Dispatch=decltype(InputRawMessageVtable::dispatchAt00);
using Delete=void* (__thiscall*)(void*,std::uint32_t);
struct MouseTable{Dispatch dispatch;Delete deleting;};
struct KeyboardTable{Dispatch dispatch;std::uint8_t (__thiscall* key)(void*,std::uint32_t);void (__thiscall* reset)(void*);Delete deleting;};
struct CursorTable{Dispatch dispatch;std::uint8_t (__thiscall* set)(void*,LPCSTR,std::uint8_t,std::uint8_t);std::uint8_t (__thiscall* reset)(void*);Delete deleting;};
std::int32_t __fastcall Mouse(void* p,void*,HWND w,UINT m,WPARAM a,LPARAM l,LRESULT* r,std::uint32_t id)
{return DispatchMouseRawNativeAt451000(*static_cast<MouseRawNativeBody10h*>(p),w,m,a,l,r,id);}
std::int32_t __fastcall Keyboard(void* p,void*,HWND w,UINT m,WPARAM a,LPARAM l,LRESULT* r,std::uint32_t id)
{return DispatchKeyboardRawNativeAt450F60(*static_cast<KeyboardRawNativeBody104h*>(p),w,m,a,l,r,id,CurrentInputNativeServices());}
std::int32_t __fastcall Cursor(void* p,void*,HWND w,UINT m,WPARAM a,LPARAM l,LRESULT* r,std::uint32_t id)
{return DispatchCursorNativeAt450F30(*static_cast<CursorNativeBody10h*>(p),w,m,a,l,r,id,CurrentInputNativeServices());}
void* __fastcall DeleteMouse(void* p,void*,std::uint32_t f){return DeleteMouseRawNativeAt4506C0(*static_cast<MouseRawNativeBody10h*>(p),f);}
void* __fastcall DeleteKeyboard(void* p,void*,std::uint32_t f){return DeleteKeyboardRawNativeAt4505A0(*static_cast<KeyboardRawNativeBody104h*>(p),f);}
void* __fastcall DeleteCursor(void* p,void*,std::uint32_t f){return DeleteCursorNativeAt450530(*static_cast<CursorNativeBody10h*>(p),f);}
std::uint8_t __fastcall Key(void* p,void*,std::uint32_t key)
{
    // 450F10按DWORD直接索引，不增加256范围检查；只正规化本次原byte。
    return *reinterpret_cast<const volatile std::uint8_t*>(reinterpret_cast<std::uintptr_t>(p)+4u+key)!=0;
}
void __fastcall ResetKeyboard(void* p,void*){std::memset(static_cast<KeyboardRawNativeBody104h*>(p)->keysAt04,0,256);}
std::uint8_t __fastcall SetCursor(void* p,void*,LPCSTR n,std::uint8_t s,std::uint8_t f)
{return SetCursorNativeAt451200(*static_cast<CursorNativeBody10h*>(p),n,s,f,CurrentInputNativeServices());}
std::uint8_t __fastcall ResetCursor(void* p,void*)
{
    auto& body=*static_cast<CursorNativeBody10h*>(p);
    auto* methods=reinterpret_cast<const CursorTable*>(body.vptrAt00);
    return methods->set(p,MAKEINTRESOURCEA(0x7F00),1,1);
}
// 保持原 x86 __thiscall ABI：__fastcall 首参走 ECX，未命名 EDX 占位不读取。
// 后续实参仍按原布局入栈并由被调方清栈；返回类型相同，异常约定兼容。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const MouseTable mouseMethods{reinterpret_cast<Dispatch>(&Mouse),reinterpret_cast<Delete>(&DeleteMouse)};
const KeyboardTable keyboardMethods{reinterpret_cast<Dispatch>(&Keyboard),
    reinterpret_cast<decltype(KeyboardTable::key)>(&Key),reinterpret_cast<decltype(KeyboardTable::reset)>(&ResetKeyboard),reinterpret_cast<Delete>(&DeleteKeyboard)};
const CursorTable cursorMethods{reinterpret_cast<Dispatch>(&Cursor),reinterpret_cast<decltype(CursorTable::set)>(&SetCursor),
    reinterpret_cast<decltype(CursorTable::reset)>(&ResetCursor),reinterpret_cast<Delete>(&DeleteCursor)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}
const InputRawMessageVtable* MouseRawNativeMethods() noexcept{return reinterpret_cast<const InputRawMessageVtable*>(&mouseMethods);}
const InputRawMessageVtable* KeyboardRawNativeMethods() noexcept{return reinterpret_cast<const InputRawMessageVtable*>(&keyboardMethods);}
const InputRawMessageVtable* CursorNativeMethods() noexcept{return reinterpret_cast<const InputRawMessageVtable*>(&cursorMethods);}
bool DispatchInputNativeHook(void* body,void* services,unsigned int id,UINT message,WPARAM w,LPARAM l,LRESULT& result)
{
    InputNativeScope scope(*static_cast<InputNativeServices*>(services));
    auto* table=*static_cast<const InputRawMessageVtable* const*>(body);
    return table->dispatchAt00(body,nullptr,message,w,l,&result,id)!=0;
}
std::int32_t DispatchMouseRawNativeAt451000(MouseRawNativeBody10h& body,HWND,UINT message,
    WPARAM w,LPARAM,LRESULT* result,std::uint32_t id)
{
    if(id)return 0;
    switch(message)
    {
    case WM_LBUTTONDOWN:body.latchedAt07[0]=1;body.pressedAt04[0]=1;break;
    case WM_LBUTTONUP:body.pressedAt04[0]=0;break;
    case WM_RBUTTONDOWN:body.latchedAt07[1]=1;body.pressedAt04[1]=1;break;
    case WM_RBUTTONUP:body.pressedAt04[1]=0;break;
    case WM_MBUTTONDOWN:body.latchedAt07[2]=1;body.pressedAt04[2]=1;break;
    case WM_MBUTTONUP:body.pressedAt04[2]=0;break;
    case WM_MOUSEMOVE:body.pressedAt04[0]=w&1;body.pressedAt04[1]=(w>>1)&1;body.pressedAt04[2]=body.pressedAt04[1];break;
    case WM_MOUSEWHEEL:
    {
        body.remainderAt0C=std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(std::uint16_t(body.remainderAt0C)+HIWORD(w)));
        const auto remainder=static_cast<std::int32_t>(body.remainderAt0C);
        if(remainder>=120||remainder<=-120)
        {
            body.wheelAt0A=std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(std::uint16_t(body.wheelAt0A)+remainder/120));
            body.remainderAt0C=static_cast<std::int16_t>(remainder%120);
        }
        break;
    }
    default:return 0;
    }
    *result=0;return 1;
}
std::int32_t DispatchKeyboardRawNativeAt450F60(KeyboardRawNativeBody104h& body,HWND,UINT message,
    WPARAM key,LPARAM,LRESULT* result,std::uint32_t id,InputNativeServices& services)
{
    if(id)return 0;
    switch(message)
    {
    case WM_KEYDOWN:
        // 原test al只检查GetKeyState低byte的符号位，不能改测常见的高bit15。
        if((static_cast<std::uint8_t>(services.platform.GetKeyStateAt450F7C(VK_MENU))&0x80)==0&&key<256)body.keysAt04[key]=1;
        break;
    case WM_KEYUP:if(key<256)body.keysAt04[key]=0;break;
    case WM_KILLFOCUS:case WM_SYSKEYUP:
        reinterpret_cast<const KeyboardTable*>(body.vptrAt00)->reset(&body);break;
    default:return 0;
    }
    *result=0;return 1;
}
std::int32_t DispatchCursorNativeAt450F30(CursorNativeBody10h& body,HWND,UINT message,WPARAM,LPARAM,
    LRESULT* result,std::uint32_t id,InputNativeServices& services)
{
    if(!id&&message==WM_SETCURSOR){if(body.currentAt04)services.cursor.Set(body.currentAt04);*result=0;}
    return 0;
}
std::uint8_t SetCursorNativeAt451200(CursorNativeBody10h& body,LPCSTR name,std::uint8_t system,
    std::uint8_t force,InputNativeServices& services)
{
    const auto module=services.platform.ReadModuleAt451210();
    const auto loaded=services.cursor.Load(system?nullptr:module,name);
    if(!loaded)return 0;
    body.currentAt04=loaded;
    if(!force)
    {
        POINT point{};services.cursor.ReadScreenPosition(point);services.cursor.ConvertToClient(body.windowAt0C,point);
        RECT rectangle{};services.cursor.ReadClientRectangle(body.windowAt0C,rectangle);
        if(!services.cursor.Contains(rectangle,point))
        {static_cast<void>(reinterpret_cast<const CursorTable*>(body.vptrAt00)->reset(&body));return 1;}
    }
    services.cursor.Set(loaded);return 1;
}
MouseRawNativeBody10h* DeleteMouseRawNativeAt4506C0(MouseRawNativeBody10h& body,std::uint32_t flags) noexcept
{
    // 5E4A8C state0 -> 541E55。显式保留handler，避免优化后的裸noexcept叶缺少EH帧。
    try
    {
        body.vptrAt00=MouseRawNativeMethods();CurrentInputNativeServices().hooks.CaptureAt450E20().RemoveRaw(&body);
        if(flags&1)::operator delete(&body,std::size_t{0x10});return &body;
    }
    catch(...){std::terminate();}
}
KeyboardRawNativeBody104h* DeleteKeyboardRawNativeAt4505A0(KeyboardRawNativeBody104h& body,std::uint32_t flags) noexcept
{
    try
    {
        body.vptrAt00=KeyboardRawNativeMethods();CurrentInputNativeServices().hooks.CaptureAt450E20().RemoveRaw(&body);
        if(flags&1)::operator delete(&body,std::size_t{0x104});return &body;
    }
    catch(...){std::terminate();}
}
CursorNativeBody10h* DeleteCursorNativeAt450530(CursorNativeBody10h& body,std::uint32_t flags) noexcept
{
    try
    {
        auto& services=CurrentInputNativeServices();body.vptrAt00=CursorNativeMethods();
        services.cursor.Set(body.previousAt08);services.hooks.CaptureAt450E20().RemoveRaw(&body);
        if(flags&1)::operator delete(&body,std::size_t{0x10});return &body;
    }
    catch(...){std::terminate();}
}
}

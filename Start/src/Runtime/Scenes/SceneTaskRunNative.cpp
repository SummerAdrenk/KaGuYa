#include <Runtime/SceneTaskRunNative.h>

namespace KaGuYa::Start::Runtime
{
__declspec(naked) void __fastcall DrawSceneTaskNativeAt4B5E20(void*,void*){__asm{ret}}
__declspec(naked) std::uint8_t __fastcall RunSceneTaskNativeAt4C3630(void*,void*,std::int32_t)
{
    __asm{
        push esi
        mov esi,ecx
        mov eax,[esi]
        call dword ptr[eax+8]
        mov eax,[esi]
        mov ecx,esi
        call dword ptr[eax+0Ch]
        mov al,1
        pop esi
        ret 4
    }
}
}

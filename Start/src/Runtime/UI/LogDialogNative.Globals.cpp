#include "LogDialogNative.Internal.h"
#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
alignas(RuntimeCString) constinit std::byte fontStorage[sizeof(RuntimeCString)]{};
RuntimeCString& Font() noexcept{return *reinterpret_cast<RuntimeCString*>(fontStorage);}
void __cdecl ExitFontAt5AC310(){DestroyRuntimeCStringAt40CD40(&Font());}
class Registrar final:public ILogDialogNativeExitRegistrar
{public:int RegisterAtExit(LogDialogNativeExit f) override{return std::atexit(f);}};
void __cdecl InitializeFont(){Registrar r;InitializeLogDialogNativeFontAt406620(Font(),ExitFontAt5AC310,r);}
}
void ConstructLogDialogFontFaceAt406620(RuntimeCString& out,IRuntimeStringManagerSource& strings)
{
    InitializeEmptyRuntimeCString(out,strings);
    struct Prefix final{RuntimeCString* p;~Prefix() noexcept{if(p)DestroyRuntimeCStringAt40CD40(p);}} guard{&out};
    SetRuntimeCStringAt437680(out,LogDialogNativeDetail::FontLiteral(),13);guard.p=nullptr;
}
RuntimeCString& ProcessLogDialogNativeFontFace() noexcept{return Font();}
void InitializeLogDialogNativeFontAt406620(RuntimeCString& out,LogDialogNativeExit callback,
    ILogDialogNativeExitRegistrar& registrar,IRuntimeStringManagerSource& strings)
{
    ConstructLogDialogFontFaceAt406620(out,strings);
    struct Unwind final{RuntimeCString* p;~Unwind() noexcept{if(p)DestroyRuntimeCStringAt40CD40(p);}} unwind{&out};
    static_cast<void>(registrar.RegisterAtExit(callback));unwind.p=nullptr;
}
// 对应原5AD740；raw槽没有C++静态析构，唯一退役入口由406689登记。
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD740",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD740")) Initializer const KaGuYaLogDialogFontInitializer=InitializeFont;
#pragma comment(linker,"/include:_KaGuYaLogDialogFontInitializer")
}

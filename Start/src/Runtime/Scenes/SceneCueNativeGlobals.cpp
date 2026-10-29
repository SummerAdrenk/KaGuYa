#include <Runtime/SceneCueNativeGlobals.h>

#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
// .data原始raw-end=619800，五槽均在virtual-end=61BA30内的零填尾部。
// 只存真实可写chars指针，无CString静态构造/析构，也无退出后懒建。
alignas(RuntimeCString) constinit const char* fontFaceAt619824 = nullptr;
alignas(RuntimeCString) constinit const char* titleBgmAt61A654 = nullptr;
alignas(RuntimeCString) constinit const char* graffitiAt61A688[3]{};
static_assert(sizeof(fontFaceAt619824) == sizeof(RuntimeCString));
static_assert(sizeof(graffitiAt61A688) == 3 * sizeof(RuntimeCString));

RuntimeCString& Slot(const char*& chars) noexcept { return reinterpret_cast<RuntimeCString&>(chars); }

void __cdecl ExitAt5AA250() { DestroySceneFontFaceNativeAt5AA250(ProcessSceneFontFaceNative()); }
void __cdecl ExitAt5AC3B0() { DestroyTitleBgmNativeAt5AC3B0(ProcessTitleBgmNative()); }
void __cdecl ExitAt5AC2F0() { DestroyGraffitiLikeCueNativeAt5AC2F0(ProcessGraffitiLikeCueNative()); }
void __cdecl ExitAt5AC3F0() { DestroyGraffitiWeightCueNativeAt5AC3F0(ProcessGraffitiWeightCueNative()); }
void __cdecl ExitAt5AC2D0() { DestroyGraffitiTrainingCueNativeAt5AC2D0(ProcessGraffitiTrainingCueNative()); }

void __cdecl InitializeAt401180()
{ InitializeSceneFontFaceNativeAt401180(ProcessSceneFontFaceNative(), ExitAt5AA250, &std::atexit); }
void __cdecl InitializeAt4068F0()
{ InitializeTitleBgmNativeAt4068F0(ProcessTitleBgmNative(), ExitAt5AC3B0, &std::atexit); }
void __cdecl InitializeAt406590()
{ InitializeGraffitiLikeCueNativeAt406590(ProcessGraffitiLikeCueNative(), ExitAt5AC2F0, &std::atexit); }
void __cdecl InitializeAt406A10()
{ InitializeGraffitiWeightCueNativeAt406A10(ProcessGraffitiWeightCueNative(), ExitAt5AC3F0, &std::atexit); }
void __cdecl InitializeAt406500()
{ InitializeGraffitiTrainingCueNativeAt406500(ProcessGraffitiTrainingCueNative(), ExitAt5AC2D0, &std::atexit); }
}

RuntimeCString& ProcessSceneFontFaceNative() noexcept { return Slot(fontFaceAt619824); }
const char*& ProcessSceneFontFaceNativeCharsSlot() noexcept { return fontFaceAt619824; }
RuntimeCString& ProcessTitleBgmNative() noexcept { return Slot(titleBgmAt61A654); }
RuntimeCString& ProcessGraffitiLikeCueNative() noexcept { return Slot(graffitiAt61A688[0]); }
RuntimeCString& ProcessGraffitiWeightCueNative() noexcept { return Slot(graffitiAt61A688[1]); }
RuntimeCString& ProcessGraffitiTrainingCueNative() noexcept { return Slot(graffitiAt61A688[2]); }

using Initializer = void(__cdecl*)();
// 原CRT表位置，不按初始化函数地址排序；反序退出为training/weight/like/title/font。
#pragma section(".CRT$XCUKaGuYa5AD4E0", read)
#pragma section(".CRT$XCUKaGuYa5AD728", read)
#pragma section(".CRT$XCUKaGuYa5AD744", read)
#pragma section(".CRT$XCUKaGuYa5AD748", read)
#pragma section(".CRT$XCUKaGuYa5AD74C", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD4E0")) Initializer const KaGuYaSceneFontFaceInitializer = InitializeAt401180;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD728")) Initializer const KaGuYaTitleBgmInitializer = InitializeAt4068F0;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD744")) Initializer const KaGuYaGraffitiLikeCueInitializer = InitializeAt406590;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD748")) Initializer const KaGuYaGraffitiWeightCueInitializer = InitializeAt406A10;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD74C")) Initializer const KaGuYaGraffitiTrainingCueInitializer = InitializeAt406500;
#pragma comment(linker, "/include:_KaGuYaSceneFontFaceInitializer")
#pragma comment(linker, "/include:_KaGuYaTitleBgmInitializer")
#pragma comment(linker, "/include:_KaGuYaGraffitiLikeCueInitializer")
#pragma comment(linker, "/include:_KaGuYaGraffitiWeightCueInitializer")
#pragma comment(linker, "/include:_KaGuYaGraffitiTrainingCueInitializer")
}

#include <Runtime/SaveFileMagicNativeGlobals.h>

#include <cstdlib>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原PE .data raw-end=619800，virtual-end=61BA30；61A5C4在loader零填尾部。
// 只存4-byte chars，不建立带自动构造/析构的全局CString。
alignas(RuntimeCString) constinit const char* magicAt61A5C4 = nullptr;
static_assert(sizeof(magicAt61A5C4) == sizeof(RuntimeCString));

void __cdecl ExitAt5ABE90()
{ DestroySaveFileMagicNativeAt5ABE90(ProcessSaveFileMagicNative()); }

void __cdecl InitializeAt4055C0()
{ InitializeSaveFileMagicNativeAt4055C0(ProcessSaveFileMagicNative(), ExitAt5ABE90, &std::atexit); }
}

RuntimeCString& ProcessSaveFileMagicNative() noexcept
{ return reinterpret_cast<RuntimeCString&>(magicAt61A5C4); }
const char*& ProcessSaveFileMagicNativeCharsSlot() noexcept { return magicAt61A5C4; }

using Initializer = void(__cdecl*)();
// 从5AD494表逐DWORD查得5AD690 -> 4055C0，不按函数地址猜排序。
#pragma section(".CRT$XCUKaGuYa5AD690", read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD690")) Initializer const KaGuYaSaveFileMagicInitializer = InitializeAt4055C0;
#pragma comment(linker, "/include:_KaGuYaSaveFileMagicInitializer")
}

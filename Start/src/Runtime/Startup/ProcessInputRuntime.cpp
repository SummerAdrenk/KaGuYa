#include <Runtime/ProcessInputRuntime.h>
#include <Runtime/ProcessWinHookRuntime.h>
#include <Runtime/ProcessAtlBaseModuleNative.h>
#include <array>
#include <cstdlib>
#include <memory>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T>struct Storage
{
    alignas(T)std::array<std::byte,sizeof(T)> bytes{};
    T& Get() noexcept{return *std::launder(reinterpret_cast<T*>(bytes.data()));}
    template<class...Args>void Construct(Args&&...args)
    {std::construct_at(reinterpret_cast<T*>(bytes.data()),std::forward<Args>(args)...);}
};
class ProcessInputPlatform final:public IInputNativePlatform
{
public:
    HINSTANCE ReadModuleAt451210() override
    {
        // 451210读取61A698+8；40752C/53E893证明是ATL资源实例，不是Environment.module。
        return static_cast<const volatile WindowClassModuleNativeBody38h&>(ProcessAtlBaseModuleNative()).resourceInstanceAt08;
    }
    SHORT GetKeyStateAt450F7C(int key) override{return ::GetKeyState(key);}
};
constinit Storage<EnvironmentRootCriticalSection> inputLock;
constinit Storage<InputNativeWinHookSource> inputHooks;
constinit Storage<ProcessInputPlatform> inputPlatform;
constinit Storage<Platform::Win32CursorApi> inputCursor;
constinit Storage<InputNativeServices> inputServices;
constinit Storage<InputRootOwner> inputOwner;
constinit Storage<NativeInputRuntime> inputRuntime;
void __cdecl ReleaseInputLockAt5AA170() noexcept{std::destroy_at(&inputLock.Get());}
void __cdecl ReleaseInputAt5AB1A0() noexcept{inputOwner.Get().ReleaseAt5AB1A0();}
void __cdecl InitializeInputLockAt5AD4E8()
{
    // 401030的601088独立锁；服务外观不加入24h正文，也不创建第二套输入状态。
    inputLock.Construct();static_cast<void>(std::atexit(ReleaseInputLockAt5AA170));
    inputHooks.Construct(ProcessWinHookRootNativeOwner(),ProcessWinHookNativeServices());
    inputPlatform.Construct();inputCursor.Construct();
    inputServices.Construct(inputHooks.Get(),inputPlatform.Get(),inputCursor.Get(),DefaultEnvironmentRootMemory());
    inputOwner.Construct(inputLock.Get(),DefaultEnvironmentRootMemory(),inputServices.Get());
    inputRuntime.Construct(inputOwner.Get(),inputServices.Get());
}
void __cdecl RegisterInputAt5AD5B8(){static_cast<void>(std::atexit(ReleaseInputAt5AB1A0));}
}
InputRootOwner& ProcessInputRootOwner() noexcept{return inputOwner.Get();}
InputNativeServices& ProcessInputNativeServices() noexcept{return inputServices.Get();}
NativeInputRuntime& ProcessNativeInputRuntime() noexcept{return inputRuntime.Get();}
InputRootNativeBody24h* CaptureProcessInputAt418210(){return inputOwner.Get().CaptureAt418210();}
using Initializer=void(__cdecl*)();
#pragma section(".CRT$XCUKaGuYa5AD4E8",read)
#pragma section(".CRT$XCUKaGuYa5AD5B8",read)
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD4E8")) Initializer const KaGuYaInputLockInitializer=InitializeInputLockAt5AD4E8;
extern "C" __declspec(allocate(".CRT$XCUKaGuYa5AD5B8")) Initializer const KaGuYaInputInitializer=RegisterInputAt5AD5B8;
#pragma comment(linker,"/include:_KaGuYaInputLockInitializer")
#pragma comment(linker,"/include:_KaGuYaInputInitializer")
}

#include <Runtime/MainFrameWindowClassNative.h>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
template<class T> T Read(const T& value) noexcept { return *static_cast<const volatile T*>(&value); }
class Api final : public IMainFrameWindowClassNativeApi
{
public:
    void EnterCriticalSection(LPCRITICAL_SECTION lock) override { ::EnterCriticalSection(lock); }
    void LeaveCriticalSection(LPCRITICAL_SECTION lock) override { ::LeaveCriticalSection(lock); }
    BOOL GetClassInfoExA(HINSTANCE instance, LPCSTR name, WNDCLASSEXA* info) override
    { return ::GetClassInfoExA(instance, name, info); }
    HCURSOR LoadCursorA(HINSTANCE instance, LPCSTR name) override { return ::LoadCursorA(instance, name); }
    int FormatAtlName(char* buffer, std::size_t count, const void* info) override
    { return DefaultWindowClassNativeApi().FormatAtlName(buffer, count, info); }
    int GetSystemMetrics(int index) override { return ::GetSystemMetrics(index); }
    HANDLE LoadImageA(HINSTANCE instance, LPCSTR name, UINT type, int width, int height, UINT flags) override
    { return ::LoadImageA(instance, name, type, width, height, flags); }
    ATOM RegisterClassExA(const WNDCLASSEXA* info) override { return ::RegisterClassExA(info); }
};
static_assert(std::is_trivially_destructible_v<Api>);
constinit Api processApi;
}
IMainFrameWindowClassNativeApi& DefaultMainFrameWindowClassNativeApi() noexcept { return processApi; }
void InitializeMainFrameWindowClassNativeAt601208(MainFrameWindowClassNativeInfo54h& info, WNDPROC initial) noexcept
{
    // FBD PE601208初值：cb30/proc438340/brush8/cursor7F00/system1/icon80，其余均0。
    std::memset(&info, 0, sizeof(info));
    info.descriptorAt00.cbSize = 0x30;
    info.descriptorAt00.lpfnWndProc = initial;
    info.descriptorAt00.hbrBackground = reinterpret_cast<HBRUSH>(8);
    info.cursorNameAt38 = MAKEINTRESOURCEA(0x7F00);
    info.systemCursorAt3C = 1;
    info.iconResourceAt50 = 0x80;
}
ATOM RegisterMainFrameWindowClassNativeAt4316F0(MainFrameWindowClassNativeInfo54h& info, WNDPROC* previous,
    MainFrameWindowClassNativeServices& services)
{
    auto& api = services.api;
    if (!Read(info.atomAt40))
    {
        api.EnterCriticalSection(&services.winModule.lockAt04);
        if (!Read(info.atomAt40))
        {
            const bool inherited = Read(info.superclassAt30) != nullptr; // 43171F早于hModule读取。
            auto image = Read(services.imageInstance); // 431723仅在内层缓存仍空时读取。
            auto& descriptor = info.descriptorAt00;
            LPCSTR name;
            if (inherited)
            {
                const auto procedure = Read(descriptor.lpfnWndProc);
                name = Read(descriptor.lpszClassName);
                WNDCLASSEXA superclass;
                superclass.cbSize = 0x30;
                std::memset(reinterpret_cast<BYTE*>(&superclass) + 4, 0, 0x2C);
                if (!api.GetClassInfoExA(image, Read(info.superclassAt30), &superclass) &&
                    !api.GetClassInfoExA(nullptr, Read(info.superclassAt30), &superclass))
                {
                    api.LeaveCriticalSection(&services.winModule.lockAt04);
                    return 0; // 431774早退不写previous，不沿用回调修改的atom。
                }
                std::memcpy(&descriptor, &superclass, 0x30);
                info.previousProcedureAt34 = Read(descriptor.lpfnWndProc);
                descriptor.lpszClassName = name;
                descriptor.lpfnWndProc = procedure;
            }
            else
            {
                const auto cursorName = Read(info.cursorNameAt38);
                const auto cursorInstance = Read(info.systemCursorAt3C) ? nullptr : image;
                const auto cursor = api.LoadCursorA(cursorInstance, cursorName);
                name = Read(descriptor.lpszClassName);
                descriptor.hCursor = cursor;
            }
            descriptor.style = Read(descriptor.style) & ~UINT{0x4000};
            descriptor.hInstance = image;
            if (!name)
            {
                name = info.generatedNameAt42;
                static_cast<void>(api.FormatAtlName(info.generatedNameAt42, 13, &info));
                image = Read(descriptor.hInstance);
                descriptor.lpszClassName = name;
            }
            WNDCLASSEXA existing;
            std::memcpy(&existing, &descriptor, 0x30);
            const auto existingAtom = static_cast<ATOM>(api.GetClassInfoExA(image, name, &existing));
            info.atomAt40 = existingAtom;
            if (!existingAtom)
            {
                if (Read(info.iconResourceAt50))
                {
                    const auto largeInstance = Read(services.resourceInstance);
                    const auto largeHeight = api.GetSystemMetrics(12);
                    const auto largeWidth = api.GetSystemMetrics(11);
                    const auto largeId = static_cast<WORD>(Read(info.iconResourceAt50));
                    descriptor.hIcon = reinterpret_cast<HICON>(api.LoadImageA(largeInstance, MAKEINTRESOURCEA(largeId),
                        IMAGE_ICON, largeWidth, largeHeight, 0));
                    const auto smallInstance = Read(services.resourceInstance);
                    const auto smallHeight = api.GetSystemMetrics(50);
                    const auto smallWidth = api.GetSystemMetrics(49);
                    const auto smallId = static_cast<WORD>(Read(info.iconResourceAt50));
                    descriptor.hIconSm = reinterpret_cast<HICON>(api.LoadImageA(smallInstance, MAKEINTRESOURCEA(smallId),
                        IMAGE_ICON, smallWidth, smallHeight, 0));
                }
                info.atomAt40 = api.RegisterClassExA(&descriptor);
                // 原43187D直接缓存AX，没有451DA0的WinModule atom数组追加。
            }
        }
        api.LeaveCriticalSection(&services.winModule.lockAt04);
    }
    if (Read(info.superclassAt30)) *previous = Read(info.previousProcedureAt34);
    return Read(info.atomAt40);
}
}

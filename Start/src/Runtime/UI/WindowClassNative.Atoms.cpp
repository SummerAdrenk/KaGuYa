#include "WindowClassNative.Internal.h"
#include <atlbase.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
using namespace WindowClassNativeDetail;
namespace WindowClassNativeDetail
{
bool AppendAtomAt451EF3(WindowClassWinModuleNativeBody2Ch& body, ATOM atom,
    const void* marker, IWindowClassNativeApi& api)
{
    auto count = Read(body.atomCountAt24);
    auto capacity = Read(body.atomCapacityAt28);
    if (count == capacity)
    {
        auto* const base = Read(body.atomsAt20);
        const auto start = reinterpret_cast<std::uintptr_t>(base);
        const auto end = start + capacity * 2u; // 原x86 LEA模2^32，不能改成无溢出size_t校验。
        const auto address = reinterpret_cast<std::uintptr_t>(marker);
        if (address >= start && address < end) throw ATL::CAtlException(E_FAIL);
        if (!capacity)
        {
            capacity = 1;
        }
        else
        {
            capacity = count + count;
            if ((capacity & 0x80000000u) || capacity > 0x3FFFFFFFu) return false;
        }
        auto* const grown = static_cast<ATOM*>(api.Recalloc(base, capacity, 2));
        if (!grown) return false;
        count = Read(body.atomCountAt24); // 原重入后重读count，不复用分配前值。
        body.atomCapacityAt28 = capacity;
        body.atomsAt20 = grown;
    }
    const auto address = reinterpret_cast<std::uintptr_t>(Read(body.atomsAt20)) + count * 2u;
    if (address)
    {
        std::memcpy(reinterpret_cast<void*>(address), &atom, 2);
        count = Read(body.atomCountAt24);
    }
    body.atomCountAt24 = count + 1u; // 原零目标仍增加count；不补count<capacity门控。
    return true;
}
}

void InitializeWindowClassWinModuleNativeAt4012F0(WindowClassWinModuleNativeBody2Ch& body,
    BYTE& startupFailed, IWindowClassNativeApi& api)
{
    if (!api.InitializeCriticalSectionAndSpinCount(&body.lockAt04, 0))
    {
        auto result = api.GetLastError();
        if (std::bit_cast<std::int32_t>(result) > 0) result = (result & 0xFFFFu) | 0x80070000u;
        if (std::bit_cast<std::int32_t>(result) < 0)
        {
            startupFailed = 1;
            body.sizeAt00 = 0;
        }
    }
    // _atexit登记由主线负责，不隐式增加退出回调或另一份正文。
}

void ReleaseWindowClassWinModuleNativeAt5AA340(WindowClassWinModuleNativeBody2Ch& body,
    const WindowClassModuleNativeBody38h& module, IWindowClassNativeApi& api)
{
    const auto size = Read(body.sizeAt00);
    const auto instance = Read(module.instanceAt04);
    if (size && size == 0x2C)
    {
        std::int32_t index = 0;
        auto count = std::bit_cast<std::int32_t>(Read(body.atomCountAt24));
        while (index < count)
        {
            // 原5AA370每轮带有range检查；count在Unregister之后重新加载。
            if (index < 0 || index >= count) api.RaiseException(0xC000008Cu, 1, 0, nullptr);
            auto* const base = Read(body.atomsAt20);
            const auto atom = Read(base[static_cast<std::uint32_t>(index)]);
            static_cast<void>(api.UnregisterClassA(reinterpret_cast<LPCSTR>(static_cast<std::uintptr_t>(atom)), instance));
            count = std::bit_cast<std::int32_t>(Read(body.atomCountAt24));
            index = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(index) + 1u);
        }
        if (auto* const base = Read(body.atomsAt20))
        {
            api.Free(base);
            body.atomsAt20 = nullptr;
        }
        body.atomCountAt24 = 0;
        body.atomCapacityAt28 = 0;
        api.DeleteCriticalSection(&body.lockAt04);
        body.sizeAt00 = 0;
    }
    // 原析构后再走数组析构。DeleteCriticalSection内若重入发布数组，尾部仍重读并Free。
    if (auto* const base = Read(body.atomsAt20))
    {
        api.Free(base);
        body.atomsAt20 = nullptr;
    }
    body.atomCountAt24 = 0;
    body.atomCapacityAt28 = 0;
}
}

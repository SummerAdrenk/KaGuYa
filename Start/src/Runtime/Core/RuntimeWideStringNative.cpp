#include <Runtime/RuntimeWideStringNative.h>
#include <atlbase.h>
#include <bit>
#include <cstdlib>
#include <cstdint>
#include <malloc.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
class WideApi final : public IRuntimeWideStringNativeApi
{
public:
    void* AllocateZero(std::size_t n, std::size_t width) override { return std::calloc(n, width); }
    void* ReallocateZero(void* p, std::size_t n, std::size_t width) override { return ::_recalloc(p, n, width); }
    void Free(void* p) override { std::free(p); }
    int Convert(UINT page, DWORD flags, const char* source, int length, wchar_t* target, int capacity) override
    { return ::MultiByteToWideChar(page, flags, source, length, target, capacity); }
    DWORD LastError() override { return ::GetLastError(); }
};
}
IRuntimeWideStringNativeApi& DefaultRuntimeWideStringNativeApi() noexcept { static WideApi api; return api; }

void RuntimeWideStringNative::ResizeAt407BF0(int count)
{
    if (count < 0) throw ATL::CAtlException(E_INVALIDARG);
    auto* const inlineBuffer = body_.inlineAt04.data();
    auto* result = inlineBuffer;
    if (body_.pointerAt00 != inlineBuffer)
    {
        if (count > 128)
        {
            result = static_cast<wchar_t*>(api_.ReallocateZero(body_.pointerAt00, count, 2));
            if (!result) throw ATL::CAtlException(E_OUTOFMEMORY); // 原407C25保留旧pointer。
        }
        else api_.Free(body_.pointerAt00);
    }
    else if (count > 128) result = static_cast<wchar_t*>(api_.AllocateZero(count, 2));
    body_.pointerAt00 = result;
    if (!result) throw ATL::CAtlException(E_OUTOFMEMORY); // 初次calloc失败先发布null。
}
RuntimeWideStringNative::RuntimeWideStringNative(const char* source, IRuntimeWideStringNativeApi& api) : api_(api)
{
    body_.pointerAt00 = body_.inlineAt04.data();
    if (!source) { body_.pointerAt00 = nullptr; return; }
    auto at = reinterpret_cast<std::uintptr_t>(source);
    while (*reinterpret_cast<const char*>(at)) ++at;
    const auto bytes = static_cast<std::uint32_t>(at - reinterpret_cast<std::uintptr_t>(source)) + 1U;
    const auto sourceLength = std::bit_cast<std::int32_t>(bytes);
    ResizeAt407BF0(sourceLength);
    auto result = api_.Convert(CP_THREAD_ACP, 0, source, sourceLength, body_.pointerAt00, sourceLength);
    if (!result && api_.LastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        const auto required = api_.Convert(CP_THREAD_ACP, 0, source, sourceLength, nullptr, 0);
        ResizeAt407BF0(required);
        result = api_.Convert(CP_THREAD_ACP, 0, source, sourceLength, body_.pointerAt00, required);
    }
    if (!result)
    {
        // 407C70先Free，4138A0再取GetLastError；不能缓存首次错误或给constructor补EH回收。
        FreeAt40CBB0();
        auto error = api_.LastError();
        if (std::bit_cast<std::int32_t>(error) > 0) error = 0x80070000U | (error & 0xFFFFU);
        throw ATL::CAtlException(static_cast<HRESULT>(error));
    }
}
void RuntimeWideStringNative::FreeAt40CBB0()
{
    if (body_.pointerAt00 != body_.inlineAt04.data()) api_.Free(body_.pointerAt00);
}
RuntimeWideStringNative::~RuntimeWideStringNative() noexcept { if (active_) FreeAt40CBB0(); }
void RuntimeWideStringNative::ReleaseNormal() { active_ = false; FreeAt40CBB0(); }
void RuntimeWideStringNative::ReleasePhysicalNormal() { FreeAt40CBB0(); active_ = false; }
}

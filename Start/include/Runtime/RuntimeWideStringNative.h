#pragma once
#include <Windows.h>
#include <array>
#include <cstddef>

namespace KaGuYa::Start::Runtime
{
class IRuntimeWideStringNativeApi
{
public:
    virtual ~IRuntimeWideStringNativeApi() = default;
    virtual void* AllocateZero(std::size_t count, std::size_t width) = 0;
    virtual void* ReallocateZero(void*, std::size_t count, std::size_t width) = 0;
    virtual void Free(void*) = 0;
    virtual int Convert(UINT codePage, DWORD flags, const char*, int sourceLength,
        wchar_t*, int capacity) = 0;
    virtual DWORD LastError() = 0;
};
IRuntimeWideStringNativeApi& DefaultRuntimeWideStringNativeApi() noexcept;

struct RuntimeWideStringNativeBody104h final
{
    wchar_t* pointerAt00;
    std::array<wchar_t, 128> inlineAt04;
};
static_assert(sizeof(RuntimeWideStringNativeBody104h) == 0x104);
static_assert(offsetof(RuntimeWideStringNativeBody104h, inlineAt04) == 4);

// 40B3C0；正文外的服务/清理资格不进入原104h布局。
class RuntimeWideStringNative final
{
public:
    RuntimeWideStringNative(const char*, IRuntimeWideStringNativeApi& = DefaultRuntimeWideStringNativeApi());
    ~RuntimeWideStringNative() noexcept;
    RuntimeWideStringNative(const RuntimeWideStringNative&) = delete;
    RuntimeWideStringNative& operator=(const RuntimeWideStringNative&) = delete;
    const wchar_t* Characters() const noexcept { return body_.pointerAt00; }
    void ReleaseNormal();
    // 42BE56..42BE69：physical正常Free仍有展开资格，成功后才撤资格。
    void ReleasePhysicalNormal();
private:
    void ResizeAt407BF0(int count);
    void FreeAt40CBB0();
    RuntimeWideStringNativeBody104h body_;
    IRuntimeWideStringNativeApi& api_;
    bool active_ = true;
};
}

#pragma once

#include <Runtime/YtlReferenceArrayRuntime.h>
#include <Runtime/RuntimeCStringResult.h>
#include <Windows.h>
#include <winver.h>

#include <cstddef>
#include <string>

namespace KaGuYa::Start::Runtime
{
// 495CDE/495D00/495D2B/495D4E/495D71：保持 ANSI 参数及原调用顺序。
// QueryValue 不改写 Win32 输出语义；注入者须显式写 length，非零时还须写 value。
class IResourceVersionPlatformApi
{
public:
    virtual ~IResourceVersionPlatformApi() = default;
    virtual DWORD ReadModuleFileName(HMODULE module, char* filename, DWORD capacity) = 0;
    virtual DWORD ReadFileVersionInfoSize(const char* filename, DWORD* handle) = 0;
    virtual BOOL ReadFileVersionInfo(const char* filename, DWORD handle, DWORD size, void* data) = 0;
    virtual BOOL QueryValue(const void* data, const char* subBlock, void** value, UINT* length) = 0;
};

class Win32ResourceVersionPlatformApi final : public IResourceVersionPlatformApi
{
public:
    DWORD ReadModuleFileName(HMODULE module, char* filename, DWORD capacity) override;
    DWORD ReadFileVersionInfoSize(const char* filename, DWORD* handle) override;
    BOOL ReadFileVersionInfo(const char* filename, DWORD handle, DWORD size, void* data) override;
    BOOL QueryValue(const void* data, const char* subBlock, void** value, UINT* length) override;
};

// 4983E0..498499：new(14h)，环境 +54/+58/+5C 是另一个 Ref，控制块 stride=20/count=1。
// 412700：对象前 12 字节是 array_ref<unsigned char>，不是 vector 或三个版本数。
struct ResourceVersionStateRuntime final
{
    YtlReferenceSlotRuntime bytesAt00;
    void* translationAt0C;
    void* fixedInfoAt10;
};
static_assert(offsetof(ResourceVersionStateRuntime, bytesAt00) == 0x00);
static_assert(offsetof(ResourceVersionStateRuntime, translationAt0C) == 0x0C);
static_assert(offsetof(ResourceVersionStateRuntime, fixedInfoAt10) == 0x10);
static_assert(sizeof(ResourceVersionStateRuntime) == 20);

class ResourceVersionRuntime final
{
public:
    explicit ResourceVersionRuntime(HMODULE module);
    ResourceVersionRuntime(HMODULE module, IResourceVersionPlatformApi& api);
    ~ResourceVersionRuntime() noexcept;
    ResourceVersionRuntime(const ResourceVersionRuntime&) = delete;
    ResourceVersionRuntime& operator=(const ResourceVersionRuntime&) = delete;

    // 497947/497A87/497E2A 只检查 Ref 的 base，不检查两个内部指针或资源签名。
    [[nodiscard]] bool HasVersionInfo() const noexcept;
    // 真实隐藏返回槽与414540复制；不经std::string中转。内部临时正常释放允许抛出。
    [[nodiscard]] RuntimeCStringResult FileVersionNative(
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) const;
    [[nodiscard]] RuntimeCStringResult ProductVersionNative(
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) const;
    [[nodiscard]] RuntimeCStringResult QueryStringNative(const char* name,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) const;
    [[nodiscard]] RuntimeCStringResult QueryStringNative(const char* name, IResourceVersionPlatformApi& api,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) const;
    // 工具观察适配器；三个实现均调用上面的原生链，不作为CSysData的返回协议。
    [[nodiscard]] std::string FileVersion() const;    // 497920
    [[nodiscard]] std::string ProductVersion() const; // 497A60
    // 对象不存 API 指针以保持 20 字节；注入构造后须使用匹配的 QueryString(name, api)。
    // 497F2E：未写 length 的原行为不确定，现代口抛 logic_error，不补空串。
    // 成功的根/语言表查询须提供可读指针；原版未校验畸形资源的指针/长度。
    [[nodiscard]] std::string QueryString(const char* name) const;
    [[nodiscard]] std::string QueryString(const char* name, IResourceVersionPlatformApi& api) const;

private:
    // 495CB2..495CBF 只清零 Ref；+0C/+10 在查询之前不初始化，失败后可能悬空。
    // 所有消费都先检查 base；禁止把这两个指针当作独立 owner 或有效性标志。
    ResourceVersionStateRuntime state_;
};
static_assert(sizeof(ResourceVersionRuntime) == 20, "CResourceVersion requires C++20 x86");

class IResourceVersionSource
{
public:
    virtual ~IResourceVersionSource() = default;
    // 4964A6/496503/49655E：每次查询独立捕获Environment并重解+54 Ref，不增持。
    [[nodiscard]] virtual const ResourceVersionRuntime& CaptureResourceVersionAt4180C0() const = 0;
};
}

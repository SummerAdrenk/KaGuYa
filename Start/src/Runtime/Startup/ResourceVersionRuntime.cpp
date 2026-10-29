#include <Runtime/ResourceVersionRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>

#include <array>
#include <cstring>
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>

#pragma comment(lib, "Version.lib")

namespace KaGuYa::Start::Runtime
{
namespace
{
Win32ResourceVersionPlatformApi& NativeApi()
{
    static Win32ResourceVersionPlatformApi api;
    return api;
}

// 412771..41279D：复用现有控制块虚槽协议，补齐字节数组的 stride/count/payload。
class VersionBytesControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit VersionBytesControl(unsigned char* bytes) noexcept : EnvironmentObjectReferenceControl(bytes) {}
    void DestroyPayloadAtVslot00() override { delete[] static_cast<unsigned char*>(payloadAt14); }
    // 5AE3E8 ->410B10同样先退化为5ADD18，再按flags释放24-byte控制块。
    void DeleteControlAtVslot04(std::uint32_t flags) override { RetireEnvironmentObjectControl(this, flags); }

};
static_assert(sizeof(VersionBytesControl) == 24);

void ConstructNil(RuntimeCString* result, IRuntimeStringManagerSource& strings)
{
    auto* manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL);
    std::construct_at(result, manager);
}

void ConstructEmpty(RuntimeCString* result, IRuntimeStringManagerSource& strings)
{
    ConstructNil(result, strings);
    try { SetRuntimeCStringAt437680(*result, "", 0); } // 49799B/497ADB/497E7F，不省略空串赋值。
    catch (...)
    {
        [](RuntimeCString* value) noexcept { DestroyRuntimeCStringAt40CD40(value); }(result);
        throw;
    }
}

void FormatVersion(RuntimeCString& result, DWORD high, DWORD low)
{
    FormatRuntimeCStringAt417130(result, "%d.%d.%d.%d",
        static_cast<int>(HIWORD(high)), static_cast<int>(LOWORD(high)),
        static_cast<int>(HIWORD(low)), static_cast<int>(LOWORD(low)));
}
}

DWORD Win32ResourceVersionPlatformApi::ReadModuleFileName(HMODULE module, char* filename, DWORD capacity)
{
    return ::GetModuleFileNameA(module, filename, capacity);
}
DWORD Win32ResourceVersionPlatformApi::ReadFileVersionInfoSize(const char* filename, DWORD* handle)
{
    return ::GetFileVersionInfoSizeA(filename, handle);
}
BOOL Win32ResourceVersionPlatformApi::ReadFileVersionInfo(
    const char* filename, DWORD handle, DWORD size, void* data)
{
    return ::GetFileVersionInfoA(filename, handle, size, data);
}
BOOL Win32ResourceVersionPlatformApi::QueryValue(
    const void* data, const char* subBlock, void** value, UINT* length)
{
    return ::VerQueryValueA(data, subBlock, value, length);
}

ResourceVersionRuntime::ResourceVersionRuntime(HMODULE module) : ResourceVersionRuntime(module, NativeApi()) {}

ResourceVersionRuntime::ResourceVersionRuntime(HMODULE module, IResourceVersionPlatformApi& api)
{
    struct MemberUnwind final
    {
        YtlReferenceSlotRuntime& slot;
        bool armed = false;
        ~MemberUnwind() noexcept
        {
            if (armed) ReleaseYtlReferencesAt48F980(std::span(&slot, 1));
        }
    } unwind{state_.bytesAt00};
    // EH 5ECF88 -> TryBlock 5ECF5C {1,1,2,1,5ECF4C}；Handler {40h,0,0,495D7C}。
    // 无类型描述符是 catch(...)，覆盖从 495CDE 开始的查询和 412700 分配。
    try
    {
        std::array<char, 260> filename;
        const DWORD pathLength = api.ReadModuleFileName(module, filename.data(), 260);
        if (pathLength == 0 || pathLength == 260) return; // 495CE4..495CF1，不重试长路径。
        const DWORD size = api.ReadFileVersionInfoSize(filename.data(), nullptr);
        if (!size) return; // 495D07..495D09：无资源和 API 失败在原版不区分。

        // 41273C..41273E 是有符号 >0；不把高位为 1 的 DWORD 变成巨额分配。
        ReleaseYtlReference(state_.bytesAt00);
        if (size <= 0x7FFFFFFFu)
        {
            // 412700先释放旧Ref；正文写入先于控制块分配，不用RAII撤销部分发布。
            auto* bytes = new unsigned char[size];
            if (state_.bytesAt00.controlAt08)
                state_.bytesAt00.controlAt08->DeleteControlAtVslot04(1);
            state_.bytesAt00.biasAt04 = 0;
            state_.bytesAt00.baseAt00 = reinterpret_cast<std::uintptr_t>(bytes);
            void* storage = ::operator new(sizeof(VersionBytesControl));
            auto* control = storage ? ::new (storage) VersionBytesControl(bytes) : nullptr;
            state_.bytesAt00.controlAt08 = control; // 41277E先发布，再写四个字段；未知区保持原字节。
            control->referenceCountAt04 = 1;
            control->owningAt08 = 1;
            control->strideAt0C = 1;
            control->countAt10 = size;
        }
        void* data = ResolveEnvironmentObjectReference<unsigned char>(state_.bytesAt00);
        if (!api.ReadFileVersionInfo(filename.data(), 0, size, data)) throw 1; // 495DB3
        UINT length;
        // 495D48/495D6B各自重解Ref，不能跨API调用缓存data或假设bias永远为0。
        if (!api.QueryValue(ResolveEnvironmentObjectReference<unsigned char>(state_.bytesAt00),
            "\\", &state_.fixedInfoAt10, &length)) throw 1; // 495DCE
        if (!api.QueryValue(ResolveEnvironmentObjectReference<unsigned char>(state_.bytesAt00),
            "\\VarFileInfo\\Translation", &state_.translationAt0C, &length))
            throw 1; // 495DE9：长度没有保存/校验；没有备用语言。
    }
    catch (...)
    {
        // 495D7C -> 476650可再次抛出；5ECF70 state0 ->59BA10 ->40CF00随后析构成员。
        // 清理本身失败不能直接terminate；只有展开中的成员析构再失败才终止。
        unwind.armed = true;
        ReleaseYtlReference(state_.bytesAt00);
        unwind.armed = false;
    }
}

ResourceVersionRuntime::~ResourceVersionRuntime() noexcept
{
    // 498770：外层 Ref 销毁正文时只释放内部字节 Ref，再 delete(14h)。
    ReleaseYtlReferencesAt48F980(std::span(&state_.bytesAt00, 1));
}

bool ResourceVersionRuntime::HasVersionInfo() const noexcept
{
    return state_.bytesAt00.baseAt00 != 0;
}

RuntimeCStringResult ResourceVersionRuntime::FileVersionNative(IRuntimeStringManagerSource& strings) const
{
    return RuntimeCStringResult([&](RuntimeCString* result)
    {
        if (!HasVersionInfo()) { ConstructEmpty(result, strings); return; }
        RuntimeCStringResult local([&](RuntimeCString* value) { ConstructNil(value, strings); });
        // 4979D6在Capture/GetNil后读取+10，不能提前缓存版本字段。
        const auto* info = static_cast<const VS_FIXEDFILEINFO*>(state_.fixedInfoAt10);
        FormatVersion(local.Value(), info->dwFileVersionMS, info->dwFileVersionLS);
        std::construct_at(result, local.Value()); // 414540：Clone/共享或复制，再发布返回槽。
        // 497A17已撤local清理资格；析构首次Free抛出不重试，也不回收已写返回槽。
    });
}

RuntimeCStringResult ResourceVersionRuntime::ProductVersionNative(IRuntimeStringManagerSource& strings) const
{
    return RuntimeCStringResult([&](RuntimeCString* result)
    {
        if (!HasVersionInfo()) { ConstructEmpty(result, strings); return; }
        RuntimeCStringResult local([&](RuntimeCString* value) { ConstructNil(value, strings); });
        const auto* info = static_cast<const VS_FIXEDFILEINFO*>(state_.fixedInfoAt10); // 497B16
        FormatVersion(local.Value(), info->dwProductVersionMS, info->dwProductVersionLS);
        std::construct_at(result, local.Value());
    });
}

RuntimeCStringResult ResourceVersionRuntime::QueryStringNative(const char* name,
    IRuntimeStringManagerSource& strings) const
{
    return QueryStringNative(name, NativeApi(), strings);
}

RuntimeCStringResult ResourceVersionRuntime::QueryStringNative(const char* name,
    IResourceVersionPlatformApi& api, IRuntimeStringManagerSource& strings) const
{
    return RuntimeCStringResult([&](RuntimeCString* result)
    {
        if (!HasVersionInfo()) { ConstructEmpty(result, strings); return; }
        RuntimeCStringResult path([&](RuntimeCString* out) { ConstructNil(out, strings); });
        const auto* translation = static_cast<const WORD*>(state_.translationAt0C); // 497EBB
        FormatRuntimeCStringAt417130(path.Value(), "\\StringFileInfo\\%04X%04X\\%s",
            static_cast<unsigned>(translation[0]), static_cast<unsigned>(translation[1]), name);
        void* value;
        UINT length = (std::numeric_limits<UINT>::max)();
        (void)api.QueryValue(ResolveEnvironmentObjectReference<unsigned char>(state_.bytesAt00),
            path.Value().GetString(), &value, &length); // 497F0F不消费BOOL，重新解析字节Ref。
        RuntimeCStringResult local([&](RuntimeCString* out) { ConstructNil(out, strings); });
        // 原栈输出未初始化，API未写length的行为没有定义。保留已公开的安全拒绝，非原分支。
        if (length == (std::numeric_limits<UINT>::max)())
            throw std::logic_error("497F2E: version query did not provide a usable length");
        // 497F14的Capture/GetNil完成后才strlen；API返回length不是拷贝长度。
        const auto* source = length ? static_cast<const char*>(value) : "";
        SetRuntimeCStringAt437680(local.Value(), source, source ? static_cast<int>(std::strlen(source)) : 0);
        std::construct_at(result, local.Value());
        // 顺序local后path；发布结果后两次正常Free任一失败，不回滚返回槽。
    });
}

std::string ResourceVersionRuntime::FileVersion() const
{
    const auto result = FileVersionNative();
    return std::string(RuntimeStringView(result.Value()));
}

std::string ResourceVersionRuntime::ProductVersion() const
{
    const auto result = ProductVersionNative();
    return std::string(RuntimeStringView(result.Value()));
}

std::string ResourceVersionRuntime::QueryString(const char* name) const
{
    return QueryString(name, NativeApi());
}

std::string ResourceVersionRuntime::QueryString(const char* name, IResourceVersionPlatformApi& api) const
{
    const auto result = QueryStringNative(name, api);
    return std::string(RuntimeStringView(result.Value()));
}
}

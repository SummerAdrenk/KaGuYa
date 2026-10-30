#include <Runtime/GraphicsModuleNative.h>

#include <cstdlib>
#include <cstring>
#include <exception>
#include <new>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Body = GraphicsModuleNativeBody8h;
using Table = GraphicsModuleNativeVtable;
using Control = GraphicsModuleNativeControl18h;
using ControlTable = GraphicsModuleNativeControlVtable;

thread_local IEnvironmentRootMemory* currentMemory = nullptr;
thread_local IGraphicsModuleNativeWin32Api* currentWin32 = nullptr;

IEnvironmentRootMemory& Memory()
{
    if (!currentMemory) throw std::logic_error("GraphicsModuleNativeScope memory is not installed");
    return *currentMemory;
}
IGraphicsModuleNativeWin32Api& Win32()
{
    if (!currentWin32) throw std::logic_error("GraphicsModuleNativeScope Win32 API is not installed");
    return *currentWin32;
}

class Win32Api final : public IGraphicsModuleNativeWin32Api
{
public:
    HMODULE LoadLibraryA(const char* path) override { return ::LoadLibraryA(path); }
    FARPROC GetProcAddress(HMODULE module, const char* name) override
        { return ::GetProcAddress(module, name); }
    BOOL FreeLibrary(HMODULE module) override { return ::FreeLibrary(module); }
};

struct MemoryBorrow final
{
    explicit MemoryBorrow(IEnvironmentRootMemory& memory) noexcept : previous(currentMemory)
        { currentMemory = &memory; }
    ~MemoryBorrow() noexcept { currentMemory = previous; }
    IEnvironmentRootMemory* previous;
};

Body* __fastcall DeleteModule(Body* body, void*, std::uint32_t flags);
void __fastcall DestroyPayload(Control* control, void*)
{
    // 440E00：payload与Ref经bias解析的地址不同；每次使用正文当前虚表。
    auto* const body = control->payloadAt14;
    if (body) body->vptrAt00->deletingDestructorAt00(body, 1);
}
Control* __fastcall DeleteControl(Control* control, void*, std::uint32_t flags);
[[noreturn]] void __fastcall PurePayload(Control*, void*)
    { _purecall(); std::terminate(); }
Control* __fastcall DeleteRetiredControl(Control* control, void*, std::uint32_t flags);

// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const Table methods{
    reinterpret_cast<decltype(Table::deletingDestructorAt00)>(&DeleteModule)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const ControlTable controlMethods{
    reinterpret_cast<decltype(ControlTable::destroyPayloadAt00)>(&DestroyPayload),
    reinterpret_cast<decltype(ControlTable::deletingDestructorAt04)>(&DeleteControl)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
// 保留原生x86虚槽：ECX接收者、未读取的EDX占位及栈参数按对应槽布局桥接。
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
const ControlTable retiredControlMethods{
    reinterpret_cast<decltype(ControlTable::destroyPayloadAt00)>(&PurePayload),
    reinterpret_cast<decltype(ControlTable::deletingDestructorAt04)>(&DeleteRetiredControl)};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

Body* __fastcall DeleteModule(Body* body, void*, std::uint32_t flags)
{
    // 4404F6先缓存handle，再4404F9恢复表；Free返回后覆盖回调晚填的handle。
    const HMODULE module = body->moduleAt04;
    body->vptrAt00 = &methods;
    if (module)
    {
        static_cast<void>(Win32().FreeLibrary(module));
        body->moduleAt04 = nullptr;
    }
    if (flags & 1u) Memory().DeleteBody(body, 8);
    return body;
}

Control* __fastcall DeleteControl(Control* control, void*, std::uint32_t flags)
{
    // 4404CA：flags0/2也退化为5ADD18，后续派发不能再次进入派生payload槽。
    control->vptrAt00 = &retiredControlMethods;
    if (flags & 1u) Memory().DeleteBody(control, 0x18);
    return control;
}
Control* __fastcall DeleteRetiredControl(Control* control, void*, std::uint32_t flags)
{
    // 412460：退化基类的删除尺寸为14h，不是原派生control的18h。
    control->vptrAt00 = &retiredControlMethods;
    if (flags & 1u) Memory().DeleteBody(control, 0x14);
    return control;
}

struct NameStorage final
{
    union { RuntimeCString value; };
    NameStorage() noexcept {}
    ~NameStorage() noexcept {}
};
struct NameUnwind final
{
    RuntimeCString* value;
    ~NameUnwind() noexcept { if (value) DestroyRuntimeCStringAt40CD40(value); }
};
}

IGraphicsModuleNativeWin32Api& DefaultGraphicsModuleNativeWin32Api() noexcept
{
    static Win32Api api;
    return api;
}

GraphicsModuleNativeScope::GraphicsModuleNativeScope(IEnvironmentRootMemory& memory,
    IGraphicsModuleNativeWin32Api& win32) noexcept
    : previousMemory_(currentMemory), previousWin32_(currentWin32)
{
    currentMemory = &memory;
    currentWin32 = &win32;
}
GraphicsModuleNativeScope::~GraphicsModuleNativeScope() noexcept
{
    currentMemory = previousMemory_;
    currentWin32 = previousWin32_;
}

const GraphicsModuleNativeVtable& GraphicsModuleNativeMethods() noexcept { return methods; }
const GraphicsModuleNativeControlVtable& GraphicsModuleNativeControlMethods() noexcept
    { return controlMethods; }
const GraphicsModuleNativeControlVtable& GraphicsModuleNativeRetiredControlMethods() noexcept
    { return retiredControlMethods; }

Body* ConstructGraphicsModuleNativeAt4407D2(Body& body) noexcept
{
    body.vptrAt00 = &methods;
    body.moduleAt04 = nullptr;
    return &body;
}
Body* CreateGraphicsModuleNativeAt4407C4(IEnvironmentRootMemory& memory)
{
    void* const storage = memory.AllocateBody(8);
    if (!storage) return nullptr;
    auto* const body = ::new (storage) Body;
    return ConstructGraphicsModuleNativeAt4407D2(*body);
}
Body* DeleteGraphicsModuleNativeAt4404F0(Body& body, std::uint32_t flags,
    IEnvironmentRootMemory& memory, IGraphicsModuleNativeWin32Api& win32)
{
    const GraphicsModuleNativeScope scope(memory, win32);
    return DeleteModule(&body, nullptr, flags);
}
Control* DeleteGraphicsModuleControlAt4404C0(Control& control, std::uint32_t flags,
    IEnvironmentRootMemory& memory)
{
    const MemoryBorrow scope(memory);
    return DeleteControl(&control, nullptr, flags);
}

void InstallGraphicsModuleReferenceAt4407E3(YtlReferenceSlotRuntime& slot,
    Body* created, IEnvironmentRootMemory& memory, IGraphicsModuleNativeWin32Api& win32)
{
    const GraphicsModuleNativeScope scope(memory, win32);
    ReleaseYtlReference(slot);
    if (!created) return;
    // 44081B用缓存ECX=0；不能借通用Replace再删一次当前槽control。
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(created);
    void* const storage = memory.AllocateBody(0x18);
    auto* const control = storage ? ::new (storage) Control : nullptr;
    if (control)
    {
        control->vptrAt00 = &controlMethods;
        control->payloadAt14 = created;
    }
    // 44084A后才初始化引用字段；null分配仍在+04首次写入处无效访问。
    slot.controlAt08 = reinterpret_cast<YtlReferenceControlRuntime*>(control);
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    reinterpret_cast<Control*>(slot.controlAt08)->strideAt0C = 8;
    reinterpret_cast<Control*>(slot.controlAt08)->countAt10 = 1;
}

Body* ResolveGraphicsModuleReference(const YtlReferenceSlotRuntime& slot) noexcept
{
    const auto base = slot.baseAt00;
    if (!base) return nullptr;
    std::uint32_t stride = 0;
    if (slot.controlAt08)
    {
        // 只读公共ABI的+0C；同时允许真实control和测试的当前表fixture。
        const auto* bytes = reinterpret_cast<const std::byte*>(slot.controlAt08);
        std::memcpy(&stride, bytes + 0x0C, sizeof(stride));
    }
    return reinterpret_cast<Body*>(base + slot.biasAt04 * stride);
}
void ReleaseGraphicsModuleReferenceAt440620(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory, IGraphicsModuleNativeWin32Api& win32)
{
    const GraphicsModuleNativeScope scope(memory, win32);
    ReleaseYtlReference(slot);
}

FARPROC ResolveGraphicsModuleExportWithCString(Body* module, const char* name,
    std::int32_t length, IRuntimeStringManagerSource& strings,
    IGraphicsModuleNativeWin32Api& win32)
{
    NameStorage text;
    InitializeEmptyRuntimeCString(text.value, strings);
    NameUnwind unwind{&text.value};
    // 4406D1/440D6C仍保留低地址资源名叶，不用strlen替代原显式长度。
    const auto address = reinterpret_cast<std::uintptr_t>(name);
    if (address & 0xFFFF0000u) SetRuntimeCStringAt437680(text.value, name, length);
    else static_cast<void>(LoadRuntimeCStringResourceAt420510(text.value, static_cast<std::uint16_t>(address)));
    const char* const capturedName = text.value.GetString();
    // 4406F6/440B9B/440D91先撤EH资格；resolver或正常Free抛出均不能再减持名称。
    unwind.value = nullptr;
    const HMODULE capturedModule = module->moduleAt04;
    const FARPROC result = win32.GetProcAddress(capturedModule, capturedName);
    auto* const header = reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(capturedName) - sizeof(ATL::CStringData));
    ReleaseRuntimeCStringHeader(header);
    return result;
}
}

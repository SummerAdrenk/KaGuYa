#include <Runtime/SoundRootNativeInitialize.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/EnvironmentPathRuntime.h>
#include <Runtime/ProcessEnvironmentRuntime.h>

#include <bit>
#include <cerrno>
#include <cstdlib>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char RelativePath[] = "Lib\\sound.dll";
static_assert(sizeof(RelativePath) - 1 == 13);
template<class Function>
Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    Function entry;
    std::memcpy(&entry, table + offset, sizeof(entry));
    return entry;
}
ATL::CStringData* Header(const char* characters) noexcept
{ return reinterpret_cast<ATL::CStringData*>(reinterpret_cast<std::uintptr_t>(characters) - 16U); }
void InitializePathNil(RuntimeCString& output, EnvironmentPathRuntime* paths,
    IRuntimeStringManagerSource& strings)
{
    using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
    using Nil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    auto* manager = Header(paths->libraryAt18.GetString())->pStringMgr;
    if (manager) manager = ManagerSlot<Clone>(manager, 0x10)(manager);
    if (!manager)
    {
        auto* const global = strings.CaptureAt417360();
        manager = ManagerSlot<Clone>(global, 0x10)(global); // 53E1E2不补空global检查。
        if (!manager) throw ATL::CAtlException(E_FAIL);
    }
    auto* const nil = ManagerSlot<Nil>(manager, 0x0C)(manager);
    const auto characters = reinterpret_cast<std::uintptr_t>(nil) + 16U;
    std::memcpy(&output, &characters, sizeof(characters));
}
const char* AppendPath(RuntimeCString& output, EnvironmentPathRuntime* paths)
{
    // 53E200：Clone/GetNil后重读固定路径正文；扩容后仍复制此前保存的源地址/长度。
    const char* const source = paths->libraryAt18.GetString();
    const auto count = static_cast<std::uint32_t>(Header(source)->nDataLength);
    const auto length = std::bit_cast<std::int32_t>(count + 13U);
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    auto* buffer = const_cast<char*>(output.GetString());
    const auto capacity = static_cast<std::uint32_t>(Header(buffer)->nAllocLength);
    const auto refs = static_cast<std::uint32_t>(Header(buffer)->nRefs);
    if (std::bit_cast<std::int32_t>((capacity - static_cast<std::uint32_t>(length)) | (1U - refs)) < 0)
    {
        PrepareRuntimeCStringWriteAt42D590(output, length);
        buffer = const_cast<char*>(output.GetString());
    }
    (void)MoveRuntimeBytesAt43D9B0(buffer, count, source, count);
    auto* const suffix = reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(buffer) + count);
    if (suffix) std::memcpy(suffix, RelativePath, 13);
    else { errno = EINVAL; _invalid_parameter_noinfo(); }
    auto* const header = Header(buffer);
    if (length > header->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength = length; buffer[length] = 0;
    return buffer;
}
GraphicsModuleNativeBody8h* Module(const YtlReferenceSlotRuntime& input) noexcept
{
    const auto& ref = static_cast<const volatile YtlReferenceSlotRuntime&>(input);
    const auto base = ref.baseAt00;
    if (!base) return nullptr;
    auto* const control = ref.controlAt08;
    const auto stride = control ? *reinterpret_cast<const volatile std::uint32_t*>(
        reinterpret_cast<const std::byte*>(control) + 0x0C) : 0U;
    return reinterpret_cast<GraphicsModuleNativeBody8h*>(base + ref.biasAt04 * stride);
}
FARPROC ResolveName(GraphicsModuleNativeBody8h* module, const char* name,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32)
{
    union Storage { RuntimeCString value; Storage() noexcept {} ~Storage() noexcept {} } text;
    ConstructArchiveResourceNameAt40B530(&text.value, name, strings);
    const char* const characters = text.value.GetString();
    const HMODULE handle = static_cast<volatile GraphicsModuleNativeBody8h&>(*module).moduleAt04;
    // 53E311/53DF55没有新EH状态；resolver失败不增加局部析构或补回收。
    const auto address = win32.GetProcAddress(handle, characters);
    ReleaseRuntimeCStringHeader(Header(text.value.GetString())); // 53E31D/53DF61重读本地槽。
    return address;
}
class ProcessPaths final : public ISoundInitializationPathsNativeSource
{
public:
    EnvironmentPathRuntime* CapturePathsAt4180C0() override
    { return CaptureProcessEnvironmentPathsAt4180C0(); }
};
}
ISoundInitializationPathsNativeSource& DefaultSoundInitializationPathsNativeSource() noexcept
{ static ProcessPaths source; return source; }

bool InitializeSoundRootNativeAt53DFC0(YtlReferenceSlotRuntime& root, HWND window,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings,
    IGraphicsModuleNativeWin32Api& win32, ISoundInitializationPathsNativeSource& paths)
{
    const GraphicsModuleNativeScope scope(memory, win32);
    try
    {
        auto* const created = CreateGraphicsModuleNativeAt4407C4(memory);
        // PE53E010/53E07B为共同5B1D9C/5B1E6C；发布/回收与4407E3同序。
        InstallGraphicsModuleReferenceAt4407E3(root, created, memory, win32);
        auto* const fixedPaths = paths.CapturePathsAt4180C0();
        ArchiveRawResourceNameRuntime full(ArchiveRawResourceNameRuntime::ProducerTag{},
            [&](RuntimeCString* out) { InitializePathNil(*out, fixedPaths, strings); });
        const char* const characters = AppendPath(full.Value(), fixedPaths);
        auto* const loadReceiver = Module(root);
        const auto loaded = win32.LoadLibraryA(characters);
        loadReceiver->moduleAt04 = loaded; // 53E2BB固定旧receiver，不能追随Load回调换Ref。
        if (!loaded) throw 1;
        full.ReleaseNormalCached(characters);
        using Init = std::uint8_t (__cdecl*)(HWND);
        const auto init = reinterpret_cast<Init>(ResolveName(Module(root), "InitSystem", strings, win32));
        if (!init(window)) throw 2;
        return true;
    }
    catch (...) {}
    // 53E369在try[0,4]/handler5外；普通释放再抛应外传，不能二次捕获或调用ExitSystem。
    ReleaseGraphicsModuleReferenceAt440620(root, memory, win32);
    return false;
}
void ShutdownSoundRootNativeAt53DF20(YtlReferenceSlotRuntime& root,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32)
{
    const GraphicsModuleNativeScope scope(memory, win32);
    if (!static_cast<const volatile YtlReferenceSlotRuntime&>(root).baseAt00) return;
    using Exit = void (__cdecl*)();
    const auto exit = reinterpret_cast<Exit>(ResolveName(Module(root), "ExitSystem", strings, win32));
    exit();
    ReleaseYtlReference(root); // 53DF80之后重读固定root当前control；没有局部catch。
}
}

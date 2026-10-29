#include <Runtime/GraphicsRootNativeInitialize.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/EnvironmentPathRuntime.h>
#include <Runtime/ProcessEnvironmentRuntime.h>
#include <Runtime/RuntimeCStringFolderNative.h>

#include <bit>
#include <cerrno>
#include <cstdlib>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr char RelativePath[] = "Lib\\Graphics.dll";
constexpr char LoadError[] =
    "Graphics.dll\x82\xCC\x83\x8D\x81\x5B\x83\x68\x82\xC9\x8E\xB8\x94\x73\x81\x49 : "
    "[Graphics - Initialize]";
constexpr char SyntaxError[] = "syntax error : [Graphics - Initialize]";
constexpr char InitializeErrorSuffix[] =
    "\n\x8F\x89\x8A\xFA\x90\xDD\x92\xE8\x82\xCC\x8E\xB8\x94\x73\x81\x49 : "
    "[Graphics - Initialize]";
static_assert(sizeof(RelativePath) - 1 == 16);
static_assert(sizeof(SyntaxError) - 1 == 0x26);

template<class Function>
Function ManagerSlot(ATL::IAtlStringMgr* manager, std::size_t offset) noexcept
{
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    Function result;
    std::memcpy(&result, table + offset, sizeof(result));
    return result;
}

ATL::CStringData* Header(const char* characters) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(characters) - sizeof(ATL::CStringData));
}

void InitializePathNil(RuntimeCString& output, EnvironmentPathRuntime* fixedPaths,
    IRuntimeStringManagerSource& strings)
{
    using Clone = ATL::IAtlStringMgr* (__thiscall*)(ATL::IAtlStringMgr*);
    using GetNil = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*);
    auto* manager = Header(fixedPaths->libraryAt18.GetString())->pStringMgr;
    if (manager) manager = ManagerSlot<Clone>(manager, 0x10)(manager);
    if (!manager)
    {
        auto* const global = strings.CaptureAt417360();
        // 4409CE没有空manager检查；只在Clone返回空时进入440C8B的E_FAIL。
        manager = ManagerSlot<Clone>(global, 0x10)(global);
        if (!manager) throw ATL::CAtlException(E_FAIL);
    }
    auto* const nil = ManagerSlot<GetNil>(manager, 0x0C)(manager);
    const auto characters = reinterpret_cast<std::uintptr_t>(nil) + 16;
    std::memcpy(&output, &characters, sizeof(characters));
}

const char* AppendLibraryPath(RuntimeCString& output, EnvironmentPathRuntime* fixedPaths)
{
    // 4409F0在Clone/GetNil后重读固定路径对象；并非重取Environment发布槽。
    const char* const source = fixedPaths->libraryAt18.GetString();
    const auto sourceLength = static_cast<std::uint32_t>(Header(source)->nDataLength);
    const auto length = std::bit_cast<std::int32_t>(sourceLength + 16u);
    if (length < 0) throw ATL::CAtlException(E_INVALIDARG);
    auto* buffer = const_cast<char*>(output.GetString());
    const auto capacity = static_cast<std::uint32_t>(Header(buffer)->nAllocLength);
    const auto refs = static_cast<std::uint32_t>(Header(buffer)->nRefs);
    if (std::bit_cast<std::int32_t>((capacity - static_cast<std::uint32_t>(length)) | (1u - refs)) < 0)
    {
        PrepareRuntimeCStringWriteAt42D590(output, length);
        buffer = const_cast<char*>(output.GetString());
    }
    // 4405A0与43D9B0同样先清无效目标容量，再报告EINVAL/ERANGE；此处不消费返回值。
    static_cast<void>(MoveRuntimeBytesAt43D9B0(buffer, sourceLength, source, sourceLength));
    auto* const suffix = reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(buffer) + sourceLength);
    if (suffix) std::memcpy(suffix, RelativePath, 16);
    else
    {
        errno = EINVAL;
        _invalid_parameter_noinfo();
    }
    // 440A5E仍用ESI，不像通用4145E0那样重新解析output槽。
    auto* const header = Header(buffer);
    if (length > header->nAllocLength) throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength = length;
    buffer[length] = 0;
    return buffer;
}

struct FolderScannerLocal final
{
    RuntimeCStringFolderNativeBody8h body;
    bool active = false;
    ~FolderScannerLocal() noexcept
    {
        if (active) DestroyRuntimeCStringFolderNativeAt440360(body);
    }
    void ReleaseNormal()
    {
        active = false;
        ReleaseRuntimeCStringFolderInlineAt440AEE(body);
    }
};

class ProcessPaths final : public IGraphicsInitializationPathsNativeSource
{
public:
    EnvironmentPathRuntime* CapturePathsAt4180C0() override
    {
        return CaptureProcessEnvironmentPathsAt4180C0();
    }
};
}

IGraphicsInitializationPathsNativeSource& DefaultGraphicsInitializationPathsNativeSource() noexcept
{
    static ProcessPaths source;
    return source;
}

bool InitializeGraphicsRootNativeAt440770(GraphicsRootNativeBody1Ch& root,
    std::uint32_t renderMode, HWND window, std::int32_t width, std::int32_t height,
    const std::uint8_t* configuration, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings, IGraphicsModuleNativeWin32Api& win32,
    IGraphicsInitializationPathsNativeSource& paths)
{
    using RawText = ArchiveRawResourceNameRuntime;
    try
    {
        SetRuntimeCStringAt437680(root.errorAt18, "", 0);
        auto* const created = CreateGraphicsModuleNativeAt4407C4(memory);
        InstallGraphicsModuleReferenceAt4407E3(root.moduleAt00, created, memory, win32);

        RawText folder(RawText::ProducerTag{}, [&](RuntimeCString* output)
        {
            InitializeEmptyRuntimeCString(*output, strings);
        });
        auto* const fixedPaths = paths.CapturePathsAt4180C0();
        RawText fullPath(RawText::ProducerTag{}, [&](RuntimeCString* output)
        {
            InitializePathNil(*output, fixedPaths, strings);
        });
        const char* const fullPathCharacters = AppendLibraryPath(fullPath.Value(), fixedPaths);
        auto* const loadReceiver = ResolveGraphicsModuleReference(root.moduleAt00);
        const auto loaded = win32.LoadLibraryA(fullPathCharacters);
        loadReceiver->moduleAt04 = loaded;
        if (!loaded)
        {
            SetRuntimeCStringAt437680(root.errorAt18, LoadError, sizeof(LoadError) - 1);
            throw 1;
        }
        FolderScannerLocal scanner;
        ConstructRuntimeCStringFolderNativeAt440100(scanner.body, fullPathCharacters, strings);
        scanner.active = true;
        const char* const directory = scanner.body.textAt04.GetString();
        SetRuntimeCStringAt437680(folder.Value(), directory,
            directory ? static_cast<int>(std::strlen(directory)) : 0);
        scanner.ReleaseNormal();
        fullPath.ReleaseNormalCached(fullPathCharacters);

        auto* const initializeReceiver = ResolveGraphicsModuleReference(root.moduleAt00);
        using Initialize = std::uint32_t (__cdecl*)(std::uint32_t, HWND, std::int32_t,
            std::int32_t, const std::uint8_t*, const char*);
        const auto initialize = reinterpret_cast<Initialize>(ResolveGraphicsModuleExportWithCString(
            initializeReceiver, "InitSystem", 10, strings, win32));
        const char* const folderCharacters = folder.Value().GetString();
        const auto result = initialize(renderMode, window, width, height, configuration, folderCharacters);
        if ((result & 0xFFu) == 0)
        {
            const char* const error = GetGraphicsErrorMessageAt440D00(root, strings, win32);
            SetRuntimeCStringAt437680(root.errorAt18, error, error ? static_cast<int>(std::strlen(error)) : 0);
            AppendRuntimeCStringAt413550(root.errorAt18, InitializeErrorSuffix, sizeof(InitializeErrorSuffix) - 1);
            throw 1;
        }
        root.windowAt0C = window;
        root.widthAt10 = width;
        root.heightAt14 = height;
        folder.ReleaseNormalCached(folderCharacters);
        return true;
    }
    catch (int) {}
    catch (...)
    {
        SetRuntimeCStringAt437680(root.errorAt18, SyntaxError, sizeof(SyntaxError) - 1);
    }
    // 440C30位于try/handler之后；释放再抛不能被上面的catch重复截获。
    ReleaseGraphicsModuleAt440620(root, memory, win32);
    return false;
}
}

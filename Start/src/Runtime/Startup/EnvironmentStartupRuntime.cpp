#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/SystemDataEnvironmentValueRuntime.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/EnvironmentRootVersionSource.h>

#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
void InitializeEnvironmentAt498280(EnvironmentRootStorage& receiver, EnvironmentRootOwner& source,
    ParamsRuntimeRootOwner& params, HMODULE module, HINSTANCE previousInstance,
    LPSTR commandLine, int showCommand, IEnvironmentPathApi& pathApi,
    IResourceVersionPlatformApi& versionApi, IStartFileApi& files,
    ISystemDataEnvironmentDiagnostics& diagnostics, IRuntimeStringManagerSource& strings,
    ISettingTagErrorSource& errors, IEnvironmentRootMemory& memory)
{
    InitializeEnvironmentWindowFieldsAt4982AA(receiver, module, previousInstance, commandLine, showCommand);
    source.ResetCategorizedSoundEnvironment();
    ClearEnvironmentArchiveReferencesAt498312(receiver.archivesAt30);

    void* rawPath = ::operator new(sizeof(EnvironmentPathRuntime));
    EnvironmentPathRuntime* nextPath = nullptr;
    try { if (rawPath) nextPath = ::new (rawPath) EnvironmentPathRuntime(pathApi, "install.inf", strings); }
    catch (...)
    {
        ::operator delete(rawPath, sizeof(EnvironmentPathRuntime));
        throw;
    }
    ReplaceEnvironmentObjectReference(receiver.pathsAt48, nextPath, 0x30);

    void* rawVersion = ::operator new(sizeof(ResourceVersionRuntime));
    ResourceVersionRuntime* nextVersion = nullptr;
    try { if (rawVersion) nextVersion = ::new (rawVersion) ResourceVersionRuntime(module, versionApi); }
    catch (...)
    {
        ::operator delete(rawVersion, sizeof(ResourceVersionRuntime));
        throw;
    }
    ReplaceEnvironmentObjectReference(receiver.versionAt54, nextVersion, 0x14);

    // 内部getter使用source当代根；外层498280的receiver不因路径/API回调而更换。
    EnvironmentRootVersionSource versions(source);
    MessageWindowConfigExpressionParser expressionParser;
    SystemDataEnvironmentValueRuntime values(versions, expressionParser, versionApi);
    RegisterSystemDataArchiveAt4984A0(receiver, source, params, files, values,
        diagnostics, strings, errors, memory);
}

const ResourceVersionRuntime& EnvironmentStartupRuntime::CaptureResourceVersionAt4180C0() const
{
    return *VersionAt54();
}

const SystemDataInstalledFiles& EnvironmentStartupRuntime::CaptureSystemDataInstallFilesAt4180C0() const
{
    return archives_.SystemData().installFilesAt08;
}

EnvironmentPathRuntime& EnvironmentStartupRuntime::CaptureInstallPathsAt4180C0() const
{
    auto* paths = PathsAt48();
    if (!paths) throw std::logic_error("47E8F1: Environment path has no payload");
    return *paths;
}

const RuntimeCString& EnvironmentStartupRuntime::CaptureMessagePrefixAt45A5D0() const
{
    return CaptureInstallPathsAt4180C0().currentAt08;
}

const RuntimeCString& EnvironmentStartupRuntime::CaptureUnreadPrefixAt43D3D0() const
{
    return CaptureInstallPathsAt4180C0().saveAt10;
}

EnvironmentStartupRuntime::EnvironmentStartupRuntime(ParamsRuntimeRootOwner& params)
    : params_(params), lock_(std::in_place), memory_(DefaultEnvironmentRootMemory()),
      strings_(DefaultRuntimeStringManagerSource()), ownedRoot_(std::in_place, *lock_, memory_, strings_),
      root_(*ownedRoot_), archives_(root_, strings_, memory_) {}

EnvironmentStartupRuntime::EnvironmentStartupRuntime(ParamsRuntimeRootOwner& params,
    IEnvironmentRootLock& lock, IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
    : params_(params), memory_(memory), strings_(strings), ownedRoot_(std::in_place, lock, memory, strings),
      root_(*ownedRoot_), archives_(root_, strings, memory) {}

EnvironmentStartupRuntime::EnvironmentStartupRuntime(ParamsRuntimeRootOwner& params,
    EnvironmentRootOwner& root, IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings) noexcept
    : params_(params), memory_(memory), strings_(strings), root_(root),
      archives_(root_, strings, memory) {}

EnvironmentRootStorage& EnvironmentStartupRuntime::CaptureAt4180C0() const
{
    auto* root = root_.CaptureAt4180C0();
    if (!root) throw std::logic_error("4180C0: missing Environment root (safe boundary)");
    return *root;
}

const EnvironmentInstalledFiles& EnvironmentStartupRuntime::CaptureInstalledTableAt4180C0()
{
    const auto* paths = PathsAt48();
    if (!paths) throw std::logic_error("458B60: Environment path has no payload");
    return paths->installedFilesAt20;
}

void EnvironmentStartupRuntime::InitializeAt498280(HMODULE module, HINSTANCE previousInstance,
    LPSTR commandLine, int showCommand, IEnvironmentPathApi& pathApi,
    IResourceVersionPlatformApi& versionApi, IStartFileApi& files,
    ISystemDataEnvironmentDiagnostics& diagnostics)
{
    InitializeEnvironmentAt498280(CaptureAt4180C0(), root_, params_, module,
        previousInstance, commandLine, showCommand, pathApi, versionApi, files, diagnostics,
        strings_, ProcessSettingTagErrorOwner(), memory_);
}
}

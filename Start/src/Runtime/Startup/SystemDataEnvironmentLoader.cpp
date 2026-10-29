#include <Runtime/SystemDataEnvironmentLoader.h>
#include <Runtime/FileStreamRuntime.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentPathRuntime.h>
#include <Runtime/NativeArchiveRuntime.h>
#include <Runtime/NativeSerialize18.h>
#include <Logging/Logger.h>
#include <cstring>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
union ParamsPathStorage final
{
    RuntimeCStringConcatenation value;
    ParamsPathStorage(RuntimeStringBorrow directory, IRuntimeStringManagerSource& strings)
    { ::new (&value) RuntimeCStringConcatenation(directory, "params.dat", strings); }
    ~ParamsPathStorage() noexcept {}
};
struct ParamsPathLife final
{
    ParamsPathStorage& storage;
    bool armed = true;
    ~ParamsPathLife() noexcept { if (armed) storage.value.~RuntimeCStringConcatenation(); }
    void Normal()
    {
        auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(storage.value.GetString())) - 1;
        armed = false; // 496077/496DC0先撤路径展开资格，再执行可能抛出的Free。
        ReleaseRuntimeCStringHeader(header);
    }
};
struct ParamsSerializerLife final
{
    NativeSerialize18Body body;
    bool armed = true;
    ParamsSerializerLife() noexcept { ConstructNativeSerialize18InlineDword(&body); }
    ~ParamsSerializerLife() noexcept { if (armed) DestroyNativeSerialize18At463E60(&body); }
    void Normal()
    {
        DestroyNativeSerialize18At463E60(&body);
        armed = false; // 49606C/496DB2调用期间仍保留原serializer展开资格。
    }
};
bool ReadParams(RuntimeStringBorrow directory, ParamsRuntimeRootOwner& params,
    IStartFileApi& files, IRuntimeStringManagerSource& strings)
{
    NativeSerialize18Services services{DefaultEnvironmentRootMemory(), strings, files};
    NativeSerialize18Scope scope(services);
    // 495EF1 -> 466760 -> 4145E0：保留prefix管理器，GetNil之后重读路径，不补斜杠。
    ParamsPathStorage path(directory, strings);
    ParamsPathLife pathLife{path};
    ParamsSerializerLife input;
    const bool loaded = [&]
    {
        SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
        ScopedFileStreamRuntime stream(files);
        if (!stream.OpenReaderWithApi({path.value.GetString(), GENERIC_READ, 0, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, &security})) return false;
        // 417A80返回64位值，但495F8E只消费低DWORD；size异常由原局部catch返回0。
        const auto size = static_cast<DWORD>(stream.Native().SizeWithApi(files));
        if (size == 0) return false;
        auto* buffer = input.body.vptrAt00->common.bufferAt14(&input.body);
        ResizeNativeSerialize18ByteVectorAt4570D0(buffer, size, buffer);
        buffer = input.body.vptrAt00->common.bufferAt14(&input.body);
        return stream.Native().ReadExactWithApi(buffer->begin, size, files) != 0;
    }(); // 49602D..496031：先析构文件流，再读取params。
    bool recognized = false;
    if (loaded)
    {
        const std::uint32_t reading = 0; // 496036是DWORD mode0，不是普通setMode的byte写。
        std::memcpy(&input.body.writingAt10, &reading, sizeof(reading));
        input.body.cursorAt14 = 0;
        auto* current = &params.GetOrCreateAt418480();
        const NativeArchiveVtable2* methods;
        std::memcpy(&methods, current, sizeof(methods));
        methods->serializeAt00(current, &input.body);
        // 496053重新取根；496061在释放buffer/路径之前直接比较原+28 double。
        recognized = !(params.GetOrCreateAt418480().versionAt28 < 0.0);
    }
    input.Normal();
    pathLife.Normal();
    return recognized;
}
}

SystemDataEnvironmentArchive::SystemDataEnvironmentArchive(
    RuntimeStringBorrow currentDirectoryCp932,
    ParamsRuntimeRootOwner& params, IStartFileApi& fileApi,
    ISystemDataEnvironmentValueSource& values,
    ISystemDataEnvironmentDiagnostics& diagnostics,
    SystemDataEnvironmentWindowTarget ownerTarget,
    IRuntimeStringManagerSource& strings, ISettingTagErrorSource& errors)
    : SystemDataEnvironmentArchive(UnreadyTag{}, strings)
{
    InitializeFromDirectory(currentDirectoryCp932, params, fileApi, values,
        diagnostics, ownerTarget, strings, errors);
}

SystemDataEnvironmentArchive::SystemDataEnvironmentArchive(EnvironmentRootOwner& environment,
    ParamsRuntimeRootOwner& params, IStartFileApi& fileApi,
    ISystemDataEnvironmentValueSource& values, ISystemDataEnvironmentDiagnostics& diagnostics,
    IRuntimeStringManagerSource& strings, ISettingTagErrorSource& errors)
    : SystemDataEnvironmentArchive(UnreadyTag{}, strings)
{
    // 495EBC..495EE4：三串已构造才获取当代路径；解析不增加Ref，也不缓存之前的根。
    const auto* root = environment.CaptureAt4180C0();
    const auto* paths = root ? ResolveEnvironmentObjectReference<EnvironmentPathRuntime>(root->pathsAt48) : nullptr;
    if (!paths) throw std::logic_error("495EE4: Environment path has no payload (safe boundary)");
    InitializeFromDirectory(paths->currentAt08, params, fileApi, values,
        diagnostics, SystemDataEnvironmentWindowTarget(environment), strings, errors);
}

void SystemDataEnvironmentArchive::InitializeFromDirectory(RuntimeStringBorrow currentDirectoryCp932,
    ParamsRuntimeRootOwner& params, IStartFileApi& fileApi,
    ISystemDataEnvironmentValueSource& values, ISystemDataEnvironmentDiagnostics& diagnostics,
    SystemDataEnvironmentWindowTarget ownerTarget, IRuntimeStringManagerSource& strings,
    ISettingTagErrorSource& errors)
{
    if (!ReadParams(currentDirectoryCp932, params, fileApi, strings))
    {
        KaGuYa::Logging::Write(KaGuYa::Logging::Level::Warning,
            "CSysData params read/header failed; retaining native not-ready object (0x496DA7).");
        return;
    }

    // 不捕获 reader/config 构造异常；原495E10未把这些异常转成普通I/O失败。
    // 496090 才取 GameSystem；由构造核保活，禁止提前复制 Pattern/SceneLabel 或安装项。
    InitializeFromLoadedRoot(params.GetOrCreateAt418480(), values, diagnostics, ownerTarget, strings, errors);
}

void RegisterSystemDataArchiveAt4984A0(EnvironmentRootStorage& receiver,
    EnvironmentRootOwner& source, ParamsRuntimeRootOwner& params, IStartFileApi& files,
    ISystemDataEnvironmentValueSource& values, ISystemDataEnvironmentDiagnostics& diagnostics,
    IRuntimeStringManagerSource& strings, ISettingTagErrorSource& errors,
    IEnvironmentRootMemory& memory)
{
    void* raw = ::operator new(SystemDataEnvironmentArchive::OriginalObjectSize);
    SystemDataEnvironmentArchive* archive = nullptr;
    try
    {
        if (raw) archive = ::new (raw) SystemDataEnvironmentArchive(
            source, params, files, values, diagnostics, strings, errors);
    }
    catch (...)
    {
        // 4984BA/state3：仅构造区间持有raw-delete资格，之后不能补做事务回滚。
        ::operator delete(raw, SystemDataEnvironmentArchive::OriginalObjectSize);
        throw;
    }
    // 49852D固定写外层receiver；ready=0也登记，内部Capture换根不更换发布目标。
    RegisterEnvironmentArchiveAt46A4A0(receiver, archive, memory);
}

SystemDataEnvironmentLoader::SystemDataEnvironmentLoader(RuntimeStringBorrow currentDirectoryCp932,
    ParamsRuntimeRootOwner& params, IStartFileApi& files,
    ISystemDataEnvironmentValueSource& values, ISystemDataEnvironmentDiagnostics& diagnostics,
    SystemDataEnvironmentWindowTarget ownerTarget, IRuntimeStringManagerSource& strings,
    ISettingTagErrorSource& errors)
{
    // 4984A0..4984C7：只有正文构造区间持有raw-delete资格。
    void* raw = ::operator new(SystemDataEnvironmentArchive::OriginalObjectSize);
    SystemDataEnvironmentArchive* archive = nullptr;
    try
    {
        if (raw) archive = ::new (raw) SystemDataEnvironmentArchive(
            currentDirectoryCp932, params, files, values, diagnostics, ownerTarget, strings, errors);
    }
    catch (...)
    {
        ::operator delete(raw, SystemDataEnvironmentArchive::OriginalObjectSize);
        throw;
    }
    // 4984E3..498526与46A4E5..46A528使用同一原control/字段时序。
    InitializeEnvironmentArchiveReferenceAt46A4E5(state_, archive);
}

SystemDataEnvironmentLoader::~SystemDataEnvironmentLoader() noexcept
{
    // 460490/5E652C EHFlags=5：正文回调后重读control，异常终止。
    ReleaseYtlReference(state_);
}

const SystemDataEnvironmentArchive& SystemDataEnvironmentLoader::State() const noexcept
{
    return *ResolveEnvironmentObjectReference<SystemDataEnvironmentArchive>(state_);
}

bool SystemDataEnvironmentLoader::Ready() const noexcept
{
    return State().readyAt04;
}

const SystemDataEnvironmentArchive& SystemDataEnvironmentLoader::ReadyArchive() const
{
    if (!Ready())
        throw std::logic_error("CSysData success-only fields requested on not-ready object");
    return State();
}

const SystemDataInstalledFiles& SystemDataEnvironmentLoader::InstallFiles() const noexcept
{
    return State().installFilesAt08;
}

void SystemDataEnvironmentLoader::Serialize(SerializeBuffer& serializer) noexcept
{
    ResolveEnvironmentObjectReference<SystemDataEnvironmentArchive>(state_)->Serialize(serializer);
}
}

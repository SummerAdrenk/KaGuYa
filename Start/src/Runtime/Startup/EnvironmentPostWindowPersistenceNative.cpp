#include "EnvironmentPostWindowPersistenceNative.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace EnvironmentPostWindowPersistenceNativeDetail;
using SetBase = void (__thiscall*)(void*, U);
using Fill = void (__thiscall*)(void*, std::uint8_t);

U SignedHalfKeyCount(FileManagerRootRuntime& keys)
{
    const auto count = std::bit_cast<std::int32_t>(static_cast<U>(keys.RawByteCount()));
    return static_cast<U>(count / 2); // cdq/sub/sar：完整DWORD按signed向零除2，不裁剪负值。
}
void* CaptureArchive(Services& services, U index)
{ return services.environment.Archives().CaptureArchiveAt4180C0(index); }

union PathStorage final
{
    RuntimeCStringConcatenation path;
    PathStorage(RuntimeStringBorrow prefix, const char* suffix, IRuntimeStringManagerSource& strings)
    { ::new (&path) RuntimeCStringConcatenation(prefix, suffix, strings); }
    ~PathStorage() noexcept {}
};
struct PathUnwind final
{
    PathStorage& storage;
    bool active = true;
    ~PathUnwind() noexcept { if (active) storage.path.~RuntimeCStringConcatenation(); }
    void Normal()
    {
        // 481379/48A326先缓存chars，再撤CString清理资格。
        auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(storage.path.GetString())) - 1;
        active = false;
        ReleaseRuntimeCStringHeader(header);
    }
};
struct SerializerUnwind final
{
    NativeSerialize18Body& body;
    bool active = true;
    ~SerializerUnwind() noexcept { if (active) DestroyNativeSerialize18At463E60(&body); }
    void Normal()
    {
        // 481374/48A321仍有serializer资格；正常Free抛出会在展开时重进当前正文清理。
        DestroyNativeSerialize18At463E60(&body);
        active = false;
    }
};

void ReadSecretAt481220(Services& services)
{
    auto* system = static_cast<SystemDataEnvironmentArchive*>(CaptureArchive(services, 0));
    auto* paths = services.environment.PathsAt48();
    // SysData固定跨过第二次Capture，返回后才读该正文的+2C。
    PathStorage path(paths->saveAt10, system->stringAt2C, services.strings);
    PathUnwind pathUnwind{path}; // 4812C7：拼接成功返回后才取得资格。
    NativeSerialize18Body serializer;
    ConstructSerializer(&serializer);
    SerializerUnwind serializerUnwind{serializer}; // 4812FB：六个构造store完成后才取得资格。
    const int result = LoadFileAt480550(&serializer, path.path.GetString(), services);
    if (result != 0)
    {
        auto* cg = CaptureArchive(services, 3); // 48130A：流已关闭，重新取当前归档。
        Slot<Fill>(cg, 0x18)(cg, 0);
        auto* sc = CaptureArchive(services, 4); // 481321：CG fill回调后才重新取SC。
        Slot<Fill>(sc, 0x18)(sc, 0);
    }
    else
    {
        const auto* firstTable = Table(&serializer); // 481338：在mode/cursor和Capture之前缓存表。
        serializer.writingAt10 = 0;
        serializer.cursorAt14 = 0;
        auto* cg = CaptureArchive(services, 3);
        auto* returned = DispatchArchive(firstTable, &serializer, cg, services);
        const auto* secondTable = Table(returned); // 48135D：使用返回receiver，在Capture4之前缓存表。
        auto* sc = CaptureArchive(services, 4);
        static_cast<void>(DispatchArchive(secondTable, returned, sc, services));
    }
    serializerUnwind.Normal();
    pathUnwind.Normal();
}
}

void LoadSecretEnvironmentPostWindowNativeAt48A16A(EnvironmentPostWindowPersistenceNativeServices& services)
{
    using namespace EnvironmentPostWindowPersistenceNativeDetail;
    NativeSerialize18Services serializerServices{services.memory, services.strings, services.files};
    NativeSerialize18Scope serializerScope(serializerServices);
    auto* cg = CaptureArchive(services, 3);
    Slot<SetBase>(cg, 0x14)(cg, 0);
    auto* sc = CaptureArchive(services, 4); // 固定EBX，跨FileManager Ensure及key count读取。
    const U half = SignedHalfKeyCount(services.keys);
    Slot<SetBase>(sc, 0x14)(sc, half);
    ReadSecretAt481220(services);
    services.rootState.ByteAt1AD() = 1;
}

void LoadGlobalEnvironmentPostWindowNativeAt48A1E8(EnvironmentPostWindowPersistenceNativeServices& services)
{
    using namespace EnvironmentPostWindowPersistenceNativeDetail;
    NativeSerialize18Services serializerServices{services.memory, services.strings, services.files};
    NativeSerialize18Scope serializerScope(serializerServices);
    const U half = SignedHalfKeyCount(services.keys); // 48A1ED：在Capture7之前固定半长。
    auto* global = static_cast<GlobalParamsArchive*>(CaptureArchive(services, 7));
    global->lowKeyOffsetAt04 = 0;
    global->highKeyOffsetAt08 = half;
    auto* system = static_cast<SystemDataEnvironmentArchive*>(CaptureArchive(services, 0));
    auto* paths = services.environment.PathsAt48();
    PathStorage path(paths->saveAt10, system->stringAt30, services.strings);
    PathUnwind pathUnwind{path}; // 原state5，父state3仍由caller持有progress资格。
    NativeSerialize18Body serializer;
    ConstructSerializer(&serializer);
    SerializerUnwind serializerUnwind{serializer}; // 原state6 -> state5。
    if (LoadFileAt480550(&serializer, path.path.GetString(), services) == 0)
    {
        serializer.writingAt10 = 0;
        serializer.cursorAt14 = 0;
        auto* current = CaptureArchive(services, 7); // 48A306：不是先前设置key基址的Global。
        const auto* table = Table(&serializer); // 48A315：与Secret不同，在Capture7之后缓存表。
        static_cast<void>(DispatchArchive(table, &serializer, current, services));
    }
    serializerUnwind.Normal();
    pathUnwind.Normal();
    services.rootState.ByteAt1B3() = 1;
}
}

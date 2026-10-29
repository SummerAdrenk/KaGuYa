#include "EnvironmentPostWindowPersistenceNative.Internal.h"
#include "../Persistence/NativeSerialize18.Internal.h"
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
using namespace EnvironmentPostWindowPersistenceNativeDetail;
struct SerializerLife final
{
    NativeSerialize18Body& body;
    bool armed = true;
    ~SerializerLife() noexcept { if (armed) DestroyNativeSerialize18At463E60(&body); }
    void Normal()
    {
        // 489406/489040仍有state0资格；Free抛出后展开再次进入当前serializer清理。
        DestroyNativeSerialize18At463E60(&body);
        armed = false;
    }
};
union PathStorage final
{
    RuntimeCStringConcatenation path;
    PathStorage(RuntimeStringBorrow prefix, const char* suffix, IRuntimeStringManagerSource& strings)
    { ::new(&path) RuntimeCStringConcatenation(prefix, suffix, strings); }
    ~PathStorage() noexcept {}
};
struct PathLife final
{
    PathStorage& storage;
    bool armed = true;
    ~PathLife() noexcept { if (armed) storage.path.~RuntimeCStringConcatenation(); }
    void Normal()
    {
        const auto* chars = storage.path.GetString();
        armed = false; // 4893E4/48901E先退state0，再对缓存chars减持。
        ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(const_cast<char*>(chars)) - 1);
    }
};
void* CaptureArchive(Services& s, U index)
{ return s.environment.Archives().CaptureArchiveAt4180C0(index); }
bool WriteCurrentPath(NativeSerialize18Body& serializer, Services& s,
    const char* SystemDataEnvironmentArchive::* member)
{
    auto* system = static_cast<SystemDataEnvironmentArchive*>(CaptureArchive(s, 0));
    auto* paths = s.environment.PathsAt48();
    // 489350/489397与488F8A/488FD1独立捕获，第二次捕获之后才读已选SysData的文件名。
    PathStorage path(paths->saveAt10, system->*member, s.strings);
    PathLife life{path};
    // 原488C10为直接调用，不通过可被回调改写的serializer+64重新选函数。
    const bool success = NativeSerialize18Detail::SaveFile(&serializer, nullptr, path.path.GetString()) == 0;
    life.Normal();
    return success;
}
}
bool WriteSecretEnvironmentNativeAt489250(EnvironmentPostWindowPersistenceNativeServices& s)
{
    NativeSerialize18Services services{s.memory, s.strings, s.files, s.transaction};
    NativeSerialize18Scope scope(services);
    NativeSerialize18Body serializer;
    ConstructSerializerWithModeDword(&serializer);
    SerializerLife life{serializer};
    auto* cg = CaptureArchive(s, 3);
    auto* returned = DispatchArchive(Table(&serializer), &serializer, cg, s);
    auto* sc = CaptureArchive(s, 4);
    // 4892FB保存返回receiver，489348在第二次Capture之后重读其当前表。
    static_cast<void>(DispatchArchive(Table(returned), returned, sc, s));
    const bool success = WriteCurrentPath(serializer, s, &SystemDataEnvironmentArchive::stringAt2C);
    life.Normal();
    return success;
}
bool WriteGlobalEnvironmentNativeAt488EE0(EnvironmentPostWindowPersistenceNativeServices& s)
{
    NativeSerialize18Services services{s.memory, s.strings, s.files, s.transaction};
    NativeSerialize18Scope scope(services);
    NativeSerialize18Body serializer;
    ConstructSerializerWithModeDword(&serializer);
    SerializerLife life{serializer};
    auto* global = CaptureArchive(s, 7);
    static_cast<void>(DispatchArchive(Table(&serializer), &serializer, global, s));
    const bool success = WriteCurrentPath(serializer, s, &SystemDataEnvironmentArchive::stringAt30);
    life.Normal();
    return success;
}
}

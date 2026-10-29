#include "EnvironmentPostWindowPersistenceNative.Internal.h"
#include <Runtime/EnvironmentArchiveNative18.h>
#include <new>

namespace KaGuYa::Start::Runtime::EnvironmentPostWindowPersistenceNativeDetail
{
NativeSerialize18Body* ConstructSerializer(void* storage) noexcept
{
    // 4812F1/48A2E9的读取caller同样内联写DWORD1，不能借通用byte构造留下padding。
    return ConstructSerializerWithModeDword(storage);
}
NativeSerialize18Body* ConstructSerializerWithModeDword(void* storage) noexcept
{
    // 489278..48929B/488F07..488F2A：内联构造写mode整个DWORD，不沿40BB30只写byte。
    ::new(storage) NativeSerialize18Body;
    return ConstructNativeSerialize18InlineDword(storage);
}
void* DispatchArchive(const NativeSerialize18Vtable31* cachedTable, void* serializer,
    void* archive, Services& services)
{
    EnvironmentArchiveNative18Scope scope(services.keys);
    // caller决定缓存表的时点；当前Env捕获后才读该表+5C，保留原返回receiver。
    return cachedTable->common.archiveAt5C(serializer, archive);
}

}

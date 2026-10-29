#pragma once

#include <Runtime/EnvironmentPostWindowPersistenceNative.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/FileManagerRootRuntime.h>
#include <Runtime/RootControlNativeState.h>
#include <Runtime/RuntimeCStringConcatenation.h>
#include <bit>
#include <cstring>

namespace KaGuYa::Start::Runtime::EnvironmentPostWindowPersistenceNativeDetail
{
using U = std::uint32_t;
using Services = EnvironmentPostWindowPersistenceNativeServices;
template<class T> T Read(const void* address, U offset = 0) noexcept
{
    T value;
    std::memcpy(&value, static_cast<const std::byte*>(address) + offset, sizeof(value));
    return value;
}
template<class Function> Function Slot(const void* body, U offset) noexcept
{ return Read<Function>(Read<const void*>(body), offset); }
inline const NativeSerialize18Vtable31* Table(const void* body) noexcept
{ return Read<const NativeSerialize18Vtable31*>(body); }
inline void* Argument(void* serializer, U offset, void* value)
{ return Slot<NativeSerialize18Argument>(serializer, offset)(serializer, value); }
inline std::uint8_t Mode(void* serializer)
{ return Slot<decltype(NativeISerializeVtable29::modeAt04)>(serializer, 4)(serializer); }

NativeSerialize18Body* ConstructSerializer(void* storage18h) noexcept;
NativeSerialize18Body* ConstructSerializerWithModeDword(void* storage18h) noexcept;
void* DispatchArchive(const NativeSerialize18Vtable31* cachedTable, void* serializer,
    void* currentArchive, Services&);
int LoadFileAt480550(void* serializer, const char* path, Services&);
}

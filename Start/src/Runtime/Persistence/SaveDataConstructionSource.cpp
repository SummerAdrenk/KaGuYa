#include <Runtime/SaveDataConstructionSource.h>
#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <bit>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
const RuntimeCString& Name(ParamsRuntimeRootOwner& root, SaveDataName name)
{
    auto* game = root.GetOrCreateAt418480().GameSystem().get();
    if (!game) throw std::logic_error("45B578: missing GameSystem name source");
    return name == SaveDataName::Given ? game->defaultGivenName.Native() : game->defaultFamilyName.Native();
}
std::uint32_t Value(const SystemDataEnvironmentArchive& sys, SaveDataSystemField field)
{
    switch (field)
    {
    case SaveDataSystemField::At40: return sys.valueAt40;
    case SaveDataSystemField::At44: return std::bit_cast<std::uint32_t>(sys.valueAt44);
    case SaveDataSystemField::At50: return sys.valueAt50;
    case SaveDataSystemField::At54: return std::bit_cast<std::uint32_t>(sys.valueAt54);
    case SaveDataSystemField::At38: return sys.valueAt38;
    case SaveDataSystemField::At3C: return sys.valueAt3C;
    }
    throw std::logic_error("unknown SaveData SysData field");
}
}
const RuntimeCString& SaveDataFixedConstructionSource::ReadName(SaveDataName name)
{ return name == SaveDataName::Given ? game_.defaultGivenName.Native() : game_.defaultFamilyName.Native(); }
std::uint32_t SaveDataFixedConstructionSource::ReadSystemValue(SaveDataSystemField field)
{ return Value(system_, field); }
const RuntimeCString& SaveDataRootConstructionSource::ReadName(SaveDataName name) { return Name(params_, name); }
std::uint32_t SaveDataRootConstructionSource::ReadSystemValue(SaveDataSystemField field)
{
    auto* root = environment_.CaptureAt4180C0();
    auto* sys = root ? static_cast<SystemDataEnvironmentArchive*>(ResolveEnvironmentArchiveAt417990(*root, 0)) : nullptr;
    if (!sys) throw std::logic_error("45B5FD: missing SysData value source");
    return Value(*sys, field);
}
}

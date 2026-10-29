#include "ProgramMain01DialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace { thread_local ProgramMain01DialogNativeServices* selectedServices = nullptr; }
ProgramMain01DialogNativeScope::ProgramMain01DialogNativeScope(ProgramMain01DialogNativeServices& s) noexcept
    : ProgramMain01DialogNativeScope(&s) {}
ProgramMain01DialogNativeScope::ProgramMain01DialogNativeScope(ProgramMain01DialogNativeServices* s) noexcept
    : previous_(selectedServices) { selectedServices = s; }
ProgramMain01DialogNativeScope::~ProgramMain01DialogNativeScope() noexcept { selectedServices = previous_; }
ProgramMain01DialogNativeServices* SelectedProgramMain01DialogNativeServices() noexcept { return selectedServices; }
ProgramMain01DialogNativeServices& RequireProgramMain01DialogNativeServices()
{ if (!selectedServices) std::terminate(); return *selectedServices; }

EnvironmentRootStorage* ProgramMain01DialogNativeSharedSource::CaptureEnvironmentAt4180C0(std::uint32_t site)
{ return source_.CaptureEnvironmentAt4180C0(site); }
void* ProgramMain01DialogNativeSharedSource::RegistryAt417990(EnvironmentRootStorage* env, std::uint32_t index, std::uint32_t site)
{ return source_.RegistryAt417990(env, index, site); }
void ProgramMain01DialogNativeSharedSource::EnsureRootAt413BE0(std::uint32_t site) { source_.EnsureRootAt413BE0(site); }
void* ProgramMain01DialogNativeSharedSource::RootAt6198C4(std::uint32_t site) { return source_.RootAt6198C4(site); }
YtlReferenceSlotRuntime& ProgramMain01DialogNativeSharedSource::RootDrawSlotAt18(void* root)
{ return source_.RootDrawSlotAt18(root); }
ScriptParamsNativeBody* ProgramMain01DialogNativeSharedSource::ParamsAt47BDB0(void* root, std::uint32_t site)
{ return source_.ParamsAt47BDB0(root, site); }
std::int32_t ProgramMain01DialogNativeSharedSource::ResolveNamedAt498020(void* archive, const char* name, std::uint32_t site)
{ return source_.ResolveNamedAt498020(archive, name, site); }
std::int32_t ProgramMain01DialogNativeSharedSource::ReadValueAt4C73A0(ScriptParamsNativeBody* params, std::int32_t index, std::uint32_t site)
{ return source_.ReadValueAt4C73A0(params, index, site); }
std::uint8_t ProgramMain01DialogNativeSharedSource::LoadSurfaceAt480000(void* draw, YtlReferenceSlotRuntime* out, const char* name, std::uint32_t site)
{ return source_.LoadSurfaceAt480000(draw, out, name, site); }
}

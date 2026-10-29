#include <Runtime/SaveFileContainerNativeBinding.h>
#include <Runtime/SaveFileContainerRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
class FixedEnvironment final : public ISaveFileContainerEnvironmentSource
{
public:
    explicit FixedEnvironment(SaveFileContainerNativeDependencies d) noexcept : dependencies_(d) {}
    RuntimeStringBorrow CaptureDirectoryAt4180C0() override { return dependencies_.directory; }
    std::uint8_t CaptureHistoryFlagAt4C7F9B() override { return dependencies_.save.bytesAt74[1]; }
    IRuntimeStringManagerSource& Strings() override { return DefaultRuntimeStringManagerSource(); }
private:
    SaveFileContainerNativeDependencies dependencies_;
};
}
std::uint32_t WriteSaveFileContainerNativeAt4C7B10(ScriptParamsNativeBody& receiver,
    const char* name, SaveFileContainerNativeDependencies dependencies)
{
    FixedEnvironment environment(dependencies);
    return WriteSaveFileContainerNativeAt4C7B10(receiver, name, SaveFileContainerNativeServices{
        dependencies.files, environment, dependencies.random, dependencies.profile,
        {DefaultEnvironmentRootMemory(), DefaultRuntimeStringManagerSource(), dependencies.files}});
}
std::uint32_t ReadSaveFileContainerNativeAt4C7420(ScriptParamsNativeBody& receiver,
    const char* name, std::uint32_t mode, SaveFileContainerNativeDependencies dependencies)
{
    FixedEnvironment environment(dependencies);
    return ReadSaveFileContainerNativeAt4C7420(receiver, name, mode, SaveFileContainerNativeServices{
        dependencies.files, environment, dependencies.random, dependencies.profile,
        {DefaultEnvironmentRootMemory(), DefaultRuntimeStringManagerSource(), dependencies.files}});
}
}

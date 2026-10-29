#pragma once
#include <Runtime/RuntimeStringBorrow.h>
#include <Runtime/NativeSerialize18.h>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class IStartFileApi;
class SaveDataEnvironmentArchive;
class IScrVmProgramRandSource;
struct SaveFileFormatProfile;
struct ScriptParamsNativeBody;
class ISaveFileContainerEnvironmentSource;
// 显式固定依赖的原子绑定；不重新捕获Root/CParams，不复制运行数据或持有另一个generation。
// 默认Root的逐次Environment来源总装仍须单独完成，不能称该结构为完整原生容器。
struct SaveFileContainerNativeDependencies final
{
    IStartFileApi& files;
    const SaveDataEnvironmentArchive& save;
    RuntimeStringBorrow directory;
    IScrVmProgramRandSource& random;
    const SaveFileFormatProfile& profile;
};
std::uint32_t WriteSaveFileContainerNativeAt4C7B10(ScriptParamsNativeBody& receiver,
    const char* name, SaveFileContainerNativeDependencies);
std::uint32_t ReadSaveFileContainerNativeAt4C7420(ScriptParamsNativeBody& receiver,
    const char* name, std::uint32_t mode, SaveFileContainerNativeDependencies);
// 正式调用期服务只借同一来源；receiver固定，目录/历史开关仍在各原点捕获。
struct SaveFileContainerNativeServices final
{
    IStartFileApi& files;
    ISaveFileContainerEnvironmentSource& environment;
    IScrVmProgramRandSource& random;
    const SaveFileFormatProfile& profile;
    NativeSerialize18Services serialization;
};
std::uint32_t WriteSaveFileContainerNativeAt4C7B10(ScriptParamsNativeBody&,
    const char*, SaveFileContainerNativeServices);
std::uint32_t ReadSaveFileContainerNativeAt4C7420(ScriptParamsNativeBody&,
    const char*, std::uint32_t, SaveFileContainerNativeServices);
}

#pragma once

#include <Runtime/ArchiveCodecServices.h>
#include <Runtime/ParamsByteArray.h>

namespace KaGuYa::Start::Runtime
{
class FileManagerRuntime;
class FileManagerNative28h;

// 每个方法对应原调用点的重新取源；不能在入口捕获一次GS/manager供整条链复用。
template<class Manager>
class IParamsKeyInstallationSourceFor
{
public:
    virtual ~IParamsKeyInstallationSourceFor() = default;
    virtual ParamsByteArray& CurrentBlobAt418480() = 0;
    virtual Manager& CurrentManagerAt479FF0() = 0;
    // 现代有界借用验证，不重新取GS/增持正文。固定fixture须保证借用仍活。
    // 原版原始指针失效后可能越界；此口明确拒绝，不能悄悄换用新的源。
    virtual void ValidateCapturedInput(const ParamsByteArray* captured,
        const YtlReferenceSlotRuntime& identity) const = 0;
    virtual void ValidateCapturedManager(const Manager* captured) const = 0;
};
using IParamsKeyInstallationSource = IParamsKeyInstallationSourceFor<FileManagerRuntime>;
using IParamsKeyNativeInstallationSource = IParamsKeyInstallationSourceFor<FileManagerNative28h>;

// 489F7C..48A160；不捕获异常、不回滚已经发布的key，正常尾声释放当时的GS源。
// 三次codec根捕获均使用显式services，不能用独立BMR owner代替共享根。
void InstallParamsKeyAt489F7C(IParamsKeyInstallationSource& source, ArchiveCodecServices codecs);
// 原生入口直接写28h正文+1C；与现代兼容入口共用算法，不复制manager/数组或增持Ref。
void InstallParamsKeyAt489F7C(IParamsKeyNativeInstallationSource& source, ArchiveCodecServices codecs);
}

#include <Runtime/FileManagerNative.h>
#include <Runtime/MessageOwnerNative.h>
#include <Runtime/ProcessEnvironmentRuntime.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
class ProcessConstructionSource final : public IFileManagerNativeConstructionSource, public IInstalledFileTableSource
{
public:
    const char* CaptureTitleAt489E7C() override
    { return ProcessEnvironmentStartupRuntime().RootSource().CaptureAt4180C0()->titleAt10.GetString(); }
    bool ReadEnvironmentReadyAt458C1E() override
    { return CaptureProcessEnvironmentPathsAt4180C0()->readyAt00; }
    RuntimeStringBorrow CaptureCachePrefixAt458C55() override
    { return CaptureProcessEnvironmentPathsAt4180C0()->saveAt10; }
    void CreateMessageAt4593AD(YtlReferenceSlotRuntime& destination) override
    { CreateAndReplaceMessageAt4593AD(destination); }
    const EnvironmentInstalledFiles& CaptureInstalledTableAt4180C0() override
    { return CaptureProcessEnvironmentPathsAt4180C0()->installedFilesAt20; }
};
}

FileManagerNative28h* NewFileManagerAt489E56(const YtlReferenceSlotRuntime& progress)
{
    ProcessConstructionSource source;
    Win32StartFileApi files;
    // InstallTable只借服务；458D8C/458DD0/458FC7各自捕获当前根/路径，不在new前取快照。
    InstallTableCatalogSource installed(source);
    return NewFileManagerAt489E56(source, installed, files, progress, nullptr);
}
}

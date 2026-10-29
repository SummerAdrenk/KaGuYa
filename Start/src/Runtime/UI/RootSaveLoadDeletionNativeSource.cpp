#include <Runtime/RootSaveLoadDeletionNativeSource.h>
#include <Runtime/RootRuntimeOwner.h>

namespace KaGuYa::Start::Runtime
{
EnvironmentRootStorage* RootSaveLoadDeletionNativeSource::CaptureEnvironmentAt4180C0()
{
    return environment_.CaptureAt4180C0();
}
IRuntimeStringManagerSource& RootSaveLoadDeletionNativeSource::Strings()
{
    return strings_;
}
BOOL RootSaveLoadDeletionNativeSource::DeleteFileA(const char* file)
{
    return ::DeleteFileA(file);
}
void* RootSaveLoadDeletionNativeSource::CaptureInformationAfter413BE0()
{
    // 50710B/507110：Ensure后只读本次发布根；48DBD0不消费ECX，不能额外解+3C。
    root_.EnsureCreatedAt413BE0();
    return root_.ReadPublishedAt6198C4();
}
}

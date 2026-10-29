#pragma once
#include <Runtime/SaveLoadDialogNativeDeletion.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
// 只借同一Root/Environment/字符串服务；不缓存目录、catalog或另一份删除状态。
class RootSaveLoadDeletionNativeSource final : public ISaveLoadDialogNativeDeletionSource
{
public:
    RootSaveLoadDeletionNativeSource(RootRuntimeOwner& root, EnvironmentRootOwner& environment,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource()) noexcept
        : root_(root), environment_(environment), strings_(strings) {}
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    IRuntimeStringManagerSource& Strings() override;
    BOOL DeleteFileA(const char*) override;
    void* CaptureInformationAfter413BE0() override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
    IRuntimeStringManagerSource& strings_;
};
}

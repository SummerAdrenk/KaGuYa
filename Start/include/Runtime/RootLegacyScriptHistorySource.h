#pragma once
#include <Runtime/LegacyScriptHistory.h>
#include <Runtime/EnvironmentRootRuntime.h>

namespace KaGuYa::Start::Runtime
{
class RootRuntimeOwner;
class ParamsRuntimeRootOwner;
class IMessageEnvironmentPathSource;
struct RootLegacyScriptHistoryDependencies final
{
    RootRuntimeOwner& root;
    EnvironmentRootOwner& environment;
    ParamsRuntimeRootOwner& params;
    IStartFileApi& messageFiles;
    const IMessageEnvironmentPathSource& messagePaths;
    IStartFileApi& historyFiles;
    IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource();
    ILegacyScriptHistoryTempApi& temporary = DefaultLegacyScriptHistoryTempApi();
    decltype(&::MessageBoxA) messageBox = &::MessageBoxA;
};

// 只借当前消息/目录/姓名模型。选中的CParams由caller传入，不再次换根选择它。
class RootLegacyScriptHistorySource final : public ILegacyScriptHistorySource
{
public:
    explicit RootLegacyScriptHistorySource(RootLegacyScriptHistoryDependencies d) noexcept : dependencies_(d) {}
    void ConstructDirectoryAt4C7340(RuntimeCString*) override;
    std::uint8_t NamesMatchDefaultsAt41B450() override;
    std::int32_t ReadFormatAt41B4C0(std::int32_t, MessageFormatSelectionRawTarget,
        std::uint32_t, bool) override;
    void WarnAt416B00(const char*, UINT) override;
    // 挂接唯一上下文，随后执行真实文件/索引重建；不能代替前序存档容器读取。
    std::uint8_t RebuildSelected(ScriptParamsNativeBody&);
    void ResetSelected(ScriptParamsNativeBody&);
private:
    RootLegacyScriptHistoryDependencies dependencies_;
};
}

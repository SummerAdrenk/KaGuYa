#pragma once

#include <Runtime/ReadScriptCommandNative.h>

namespace KaGuYa::Start::Runtime
{
struct EnvironmentRootStorage;
class EnvironmentRootOwner;
class RootRuntimeOwner;

class IReadScriptAdvanceCommandGateNativeSource : public IReadScriptCommandNativeSource
{
public:
    virtual EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t caller) = 0;
};

// 4AA3E0完整门控；两个Environment及两个Root各自晚取，不为命令临时串增加EH资格。
[[nodiscard]] bool ReadScriptAdvanceCommandAllowedNativeAt4AA3E0(
    IReadScriptAdvanceCommandGateNativeSource& source);

// 只借用owner，不保存Environment、Root、SceneManager或CString正文。
class RootReadScriptAdvanceCommandGateNativeSource final
    : public IReadScriptAdvanceCommandGateNativeSource
{
public:
    RootReadScriptAdvanceCommandGateNativeSource(
        RootRuntimeOwner& root, EnvironmentRootOwner& environment) noexcept;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0(std::uint32_t caller) override;
    void EnsureCommandRootAt413BE0(std::uint32_t caller) override;
    void* CommandRootAt6198C4(std::uint32_t caller) override;
    SceneManagerNativeBody48h* CommandManagerAt4186D0(
        void* capturedRoot, std::uint32_t caller) override;
private:
    RootRuntimeOwner& root_;
    EnvironmentRootOwner& environment_;
};
}

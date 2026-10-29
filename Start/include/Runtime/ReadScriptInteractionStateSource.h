#pragma once
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
class SaveDataEnvironmentArchive;
class UnSaveDataRuntime;
class ScrVmValueTable;
class EnvironmentRootOwner;
class RootRuntimeOwner;

// 4B2730/4B11B0逐调用点取源；调用者保留原读取宽度与跨回调的局部receiver。
class IReadScriptInteractionStateSource
{
public:
    virtual ~IReadScriptInteractionStateSource() = default;
    virtual SaveDataEnvironmentArchive& CaptureSaveAt4180C0() = 0;
    virtual UnSaveDataRuntime& CaptureUnSaveAt4180C0() = 0;
    virtual std::uint32_t ReadMessageFormatAt47BDB0() = 0;
};
class FixedReadScriptInteractionStateSource final : public IReadScriptInteractionStateSource
{
public:
    FixedReadScriptInteractionStateSource(SaveDataEnvironmentArchive& save, UnSaveDataRuntime& unsave,
        const ScrVmValueTable* params = nullptr) noexcept : save_(save), unsave_(unsave), params_(params) {}
    SaveDataEnvironmentArchive& CaptureSaveAt4180C0() override;
    UnSaveDataRuntime& CaptureUnSaveAt4180C0() override;
    std::uint32_t ReadMessageFormatAt47BDB0() override;
private:
    SaveDataEnvironmentArchive& save_;
    UnSaveDataRuntime& unsave_;
    const ScrVmValueTable* params_;
};
class RootReadScriptInteractionStateSource final : public IReadScriptInteractionStateSource
{
public:
    RootReadScriptInteractionStateSource(EnvironmentRootOwner& environment, RootRuntimeOwner& root) noexcept
        : environment_(environment), root_(root) {}
    SaveDataEnvironmentArchive& CaptureSaveAt4180C0() override;
    UnSaveDataRuntime& CaptureUnSaveAt4180C0() override;
    std::uint32_t ReadMessageFormatAt47BDB0() override;
private:
    EnvironmentRootOwner& environment_;
    RootRuntimeOwner& root_;
};
}

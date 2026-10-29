#pragma once
#include <Runtime/GuiInputStateRuntime.h>
#include <Runtime/ScriptNamedValueNative.h>

namespace KaGuYa::Start::Runtime
{
class SaveDataEnvironmentArchive;
class UnSaveDataRuntime;
struct ScrVmRuntimeStateBundle;
class ScrVmExecutableProfile;
class RootRuntimeOwner;
class EnvironmentRootOwner;
class InputRootOwner;
class IScrVmCommandErrorSink;

// 每个方法对应一个原调用点；不保活环境/输入/脚本，也不缓存跨回调的当前正文。
class IReadScriptWaitStateSource
{
public:
    virtual ~IReadScriptWaitStateSource()=default;
    virtual const SaveDataEnvironmentArchive& CaptureSaveAt4180C0()=0;
    virtual UnSaveDataRuntime& CaptureUnSaveAt4180C0()=0;
    virtual bool ReadPressedAt47F4E0(std::int32_t action)=0;
    virtual bool ReadHeldAt4198D0(std::int32_t action)=0;
    virtual bool MousePressedEdge(std::size_t button)=0;
    virtual void SetMouseCountdownAt28(std::int32_t value)=0;
    virtual bool SuppressEffectAt4C3430()=0;
    virtual bool SkipEffectAt4AA970()=0;
    virtual std::int32_t ReadWaitValueAfter413BE0(unsigned index)=0;
    virtual void WriteWaitSkippedAfter413BE0(std::int32_t value)=0;
    virtual void ClearRootByteAt1B0After413BE0()=0;
};

// 显式固定模型用于既有构造和夹具；切原生Source时不要求构造这些模型。
class FixedReadScriptWaitStateSource final:public IReadScriptWaitStateSource
{
public:
    FixedReadScriptWaitStateSource(ScrVmRuntimeStateBundle&,const ScrVmExecutableProfile&,
        const SaveDataEnvironmentArchive&,UnSaveDataRuntime&,Platform::MouseView,
        Platform::KeyboardView,std::uint8_t&) noexcept;
    const SaveDataEnvironmentArchive& CaptureSaveAt4180C0() override;
    UnSaveDataRuntime& CaptureUnSaveAt4180C0() override;
    bool ReadPressedAt47F4E0(std::int32_t) override;
    bool ReadHeldAt4198D0(std::int32_t) override;
    bool MousePressedEdge(std::size_t) override;
    void SetMouseCountdownAt28(std::int32_t) override;
    bool SuppressEffectAt4C3430() override;
    bool SkipEffectAt4AA970() override;
    std::int32_t ReadWaitValueAfter413BE0(unsigned) override;
    void WriteWaitSkippedAfter413BE0(std::int32_t) override;
    void ClearRootByteAt1B0After413BE0() override;
private:
    ScrVmRuntimeStateBundle& bundle_;const ScrVmExecutableProfile& profile_;
    const SaveDataEnvironmentArchive& save_;UnSaveDataRuntime& unsave_;
    Platform::MouseView mouse_;GameGuiInputStateRuntime input_;std::uint8_t& rootByte_;
};

class RootReadScriptWaitStateSource final:public IReadScriptWaitStateSource
{
public:
    RootReadScriptWaitStateSource(EnvironmentRootOwner&,InputRootOwner&,RootRuntimeOwner&,
        IScrVmCommandErrorSink&,ScriptNamedValueNativeTables,IRuntimeStringManagerSource&) noexcept;
    const SaveDataEnvironmentArchive& CaptureSaveAt4180C0() override;
    UnSaveDataRuntime& CaptureUnSaveAt4180C0() override;
    bool ReadPressedAt47F4E0(std::int32_t) override;
    bool ReadHeldAt4198D0(std::int32_t) override;
    bool MousePressedEdge(std::size_t) override;
    void SetMouseCountdownAt28(std::int32_t) override;
    bool SuppressEffectAt4C3430() override;
    bool SkipEffectAt4AA970() override;
    std::int32_t ReadWaitValueAfter413BE0(unsigned) override;
    void WriteWaitSkippedAfter413BE0(std::int32_t) override;
    void ClearRootByteAt1B0After413BE0() override;
private:
    EnvironmentRootOwner& environment_;InputRootOwner& input_;RootRuntimeOwner& root_;
    IScrVmCommandErrorSink& errors_;ScriptNamedValueNativeTables names_;IRuntimeStringManagerSource& strings_;
};
}

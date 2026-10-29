#pragma once
#include <Runtime/ReadScriptSoundWaitSource.h>
#include <Runtime/ScriptSoundLeafSource.h>
#include <Runtime/SoundMappingParameterReceiver.h>
#include <Runtime/DrawLayersView.h>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
class EnvironmentRootOwner;
class ParamsRuntimeRootOwner;
class InputRootOwner;
class FrameLayerItem;
struct TextWindowLayerNativeBodyE8h;

// 两个业务视图借同一个已选manager；query/restore仍由caller独立捕获。
struct ReadScriptSoundRequestsReceiver final
{
    ReadScriptSoundWaitReceiver slots;
    ScriptSoundManagerReceiver effects;
};
struct ReadScriptUnreadReceiver final
{
    void* object;
    std::uint8_t (*readAt9A)(void*);
    std::uint8_t Read() const { return readAt9A(object); }
    static ReadScriptUnreadReceiver Native(TextWindowLayerNativeBodyE8h*) noexcept;
    // 旧Draw producer过渡口，保留其显式类型/optional保护；不冒充原E8h。
    static ReadScriptUnreadReceiver Modern(FrameLayerItem*) noexcept;
    static ReadScriptUnreadReceiver View(DrawTextView selected) noexcept
    {return {const_cast<void*>(selected.Receiver()),selected.BorrowUnreadReaderAt9A()};}
};

class IReadScriptSoundRequestsSource
{
public:
    virtual ~IReadScriptSoundRequestsSource() = default;
    virtual std::int32_t MappingCountInlineAt4A9202() = 0;
    virtual bool MappingEmptyInlineAt4A9371(std::uint32_t port) = 0;
    virtual RuntimeCString* CaptureMappingSlotInlineAt4A9505(std::uint32_t port) = 0;
    virtual ReadScriptSoundRequestsReceiver CaptureManagerAfter413BE0() = 0;
    virtual ReadScriptUnreadReceiver CaptureTextAfter413BE0At4A95A2() = 0;
    virtual bool ReadKeyboardSixHeldAt4AA573() = 0;
    virtual std::uint32_t ReadUnsaveFlagsAt14Via417990() = 0;
    virtual std::uint8_t ReadSaveFlagsAt38Via417990() = 0;
    virtual const ParamsGameSystem* CaptureGameSystemAt418480() = 0;
    virtual const char* CaptureMappingNameVia417990(std::uint32_t port) = 0;
    virtual SoundMappingParameterReceiver CaptureParametersVia417990() = 0;
};

// Environment/Input/Params各原捕获点独立取源；不保留正文、映射或分组计数镜像。
class EnvironmentReadScriptSoundRequestsSource final : public IReadScriptSoundRequestsSource
{
public:
    EnvironmentReadScriptSoundRequestsSource(EnvironmentRootOwner&, ParamsRuntimeRootOwner&,
        InputRootOwner&, RootRuntimeOwner&) noexcept;
    std::int32_t MappingCountInlineAt4A9202() override;
    bool MappingEmptyInlineAt4A9371(std::uint32_t) override;
    RuntimeCString* CaptureMappingSlotInlineAt4A9505(std::uint32_t) override;
    ReadScriptSoundRequestsReceiver CaptureManagerAfter413BE0() override;
    ReadScriptUnreadReceiver CaptureTextAfter413BE0At4A95A2() override;
    bool ReadKeyboardSixHeldAt4AA573() override;
    std::uint32_t ReadUnsaveFlagsAt14Via417990() override;
    std::uint8_t ReadSaveFlagsAt38Via417990() override;
    const ParamsGameSystem* CaptureGameSystemAt418480() override;
    const char* CaptureMappingNameVia417990(std::uint32_t) override;
    SoundMappingParameterReceiver CaptureParametersVia417990() override;
private:
    EnvironmentRootOwner& environment_;
    ParamsRuntimeRootOwner& params_;
    InputRootOwner& input_;
    RootRuntimeOwner& root_;
};
}

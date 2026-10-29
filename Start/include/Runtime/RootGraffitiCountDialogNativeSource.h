#pragma once
#include <Runtime/GraffitiCountDialogNative.h>
#include <Runtime/RootGraffitiPageNativeSource.h>

namespace KaGuYa::Start::Runtime
{
// 三个原进程CString的借用槽；装配者负责按原CRT初始化，Source不复制/默认补造名字。
struct GraffitiCountCueSlots final
{
    const RuntimeCString& likeAt61A688;
    const RuntimeCString& weightAt61A68C;
    const RuntimeCString& trainingAt61A690;
};
struct GraffitiCountCueDefaultRecord final
{
    std::uint32_t slotAddress,initializerAddress,stringAddress;
    const char* bytesCp932;
};
const std::array<GraffitiCountCueDefaultRecord,3>& GraffitiCountCueDefaultRecords() noexcept;
class RootGraffitiCountDialogNativeSource final:public IGraffitiCountDialogNativeSource
{
public:
    RootGraffitiCountDialogNativeSource(RootRuntimeOwner&,EnvironmentRootOwner&,GraffitiCountCueSlots) noexcept;
    EnvironmentRootStorage* CaptureEnvironmentAt4180C0() override;
    GraffitiPageNativeParamsReceiver CaptureParamsRootAfter413BE0() override;
    const char* ReadCueName(std::uint32_t) override;
    GraffitiCountDialogSoundBorrow CaptureSoundAfter413BE0() override;
private:
    RootRuntimeOwner& root_;
    RootGraffitiPageNativeSource common_;
    GraffitiCountCueSlots cues_;
};
}

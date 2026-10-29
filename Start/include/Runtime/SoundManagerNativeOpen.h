#pragma once
#include <Runtime/SoundManagerNativePlayback.h>
#include <Runtime/RootArchiveResourceSource.h>
#include <Runtime/RuntimeWideStringNative.h>

namespace KaGuYa::Start::Runtime
{
class SoundManagerNativeOpen final : public ISoundManagerNativeOpen
{
public:
    SoundManagerNativeOpen(IArchiveResourceSource&, SoundModuleNativeApi&,
        IRuntimeWideStringNativeApi& = DefaultRuntimeWideStringNativeApi(),
        IRuntimeStringManagerSource& = DefaultRuntimeStringManagerSource()) noexcept;
    bool OpenAt42BC90(SoundManagerNativeBody50h&, std::uint32_t slot, const char* heldName,
        std::int32_t value, std::int32_t loop, std::uint8_t noAutoPlay) override;
private:
    IArchiveResourceSource& archives_;
    SoundModuleNativeApi& sound_;
    IRuntimeWideStringNativeApi& wide_;
    IRuntimeStringManagerSource& strings_;
};
}

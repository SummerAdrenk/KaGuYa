#include <Runtime/ParamsKeyInstallationRuntime.h>
#include <Runtime/ArchiveCodecManagerRuntime.h>
#include <Runtime/BmrDecoderRuntime.h>
#include <Runtime/FileManagerRuntime.h>
#include <Runtime/FileManagerNative.h>

#include <cstring>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
BmrDecoderRuntime& CaptureDecoder(ArchiveCodecServices codecs)
{
    auto* const manager = codecs.managers.CaptureAt445910();
    if (!manager)
        throw std::logic_error("Params key codec manager is null (safe boundary)");
    auto* const decoder = GetBmrAt445110(*manager);
    if (!decoder)
        throw std::logic_error("Params key BMR decoder is null (safe boundary)");
    return *decoder;
}

void RequireCoveredRange(std::span<const std::uint8_t> extent, std::span<const std::uint8_t> access)
{
    const auto begin = reinterpret_cast<std::uintptr_t>(extent.data());
    const auto at = reinterpret_cast<std::uintptr_t>(access.data());
    if (at < begin || at - begin > extent.size() || access.size() > extent.size() - (at - begin))
        throw std::out_of_range("Params key borrowed access exceeds its current extent (safe boundary)");
}

template<class Manager>
class InputReadGuard final : public IBmrInputReadGuard
{
public:
    InputReadGuard(const IParamsKeyInstallationSourceFor<Manager>& source, const ParamsByteArray& blob) noexcept
        : source_(source), blob_(&blob), identity_(blob.storage.RawSlot()) {}
    void ValidateRead(std::span<const std::uint8_t> access) override
    {
        source_.ValidateCapturedInput(blob_, identity_);
        RequireCoveredRange(BorrowBoundedByteArrayCopySpan(blob_->storage), access);
    }
private:
    const IParamsKeyInstallationSourceFor<Manager>& source_;
    const ParamsByteArray* blob_;
    YtlReferenceSlotRuntime identity_;
};

template<class Manager>
class DestinationAccessGuard final : public IBmrDestinationAccessGuard
{
public:
    DestinationAccessGuard(const IParamsKeyInstallationSourceFor<Manager>& source, Manager& manager) noexcept
        : source_(source), manager_(&manager), identity_(manager.KeyReferenceAt1C().RawSlot()) {}
    void ValidateAccess(std::span<const std::uint8_t> access) override
    {
        source_.ValidateCapturedManager(manager_);
        const auto& key = manager_->KeyReferenceAt1C();
        const auto& current = key.RawSlot();
        if (current.baseAt00 != identity_.baseAt00 || current.biasAt04 != identity_.biasAt04 ||
            current.controlAt08 != identity_.controlAt08)
            throw std::runtime_error("Params key destination reference changed (safe boundary)");
        RequireCoveredRange(BorrowBoundedByteArrayCopySpan(key), access);
    }
private:
    const IParamsKeyInstallationSourceFor<Manager>& source_;
    Manager* manager_;
    YtlReferenceSlotRuntime identity_;
};

template<class Manager>
void InstallParamsKeyCore(IParamsKeyInstallationSourceFor<Manager>& source, ArchiveCodecServices codecs)
{
    // gate只读取control.count；空源也必须进入重新取GS的公共释放尾声。
    if (source.CurrentBlobAt418480().storage.Size() != 0)
    {
        auto& identifiedBlob = source.CurrentBlobAt418480();
        InputReadGuard readGuard(source, identifiedBlob);
        auto input = BorrowBoundedByteArrayCopySpan(identifiedBlob.storage);
        auto& identifyingDecoder = CaptureDecoder(codecs);
        std::uint32_t outputSize;
        const auto* const payload = identifyingDecoder.IdentifyAt445A00(input, outputSize, &readGuard);
        if (!payload)
        {
            const auto requested = source.CurrentBlobAt418480().storage.Size();
            source.CurrentManagerAt479FF0().KeyReferenceAt1C().AllocateAt412700(requested);
            const auto copyCount = source.CurrentManagerAt479FF0().KeyReferenceAt1C().Size();
            auto& targetManager = source.CurrentManagerAt479FF0();
            const auto currentInput = BorrowBoundedByteArrayCopySpan(source.CurrentBlobAt418480().storage);
            source.ValidateCapturedManager(&targetManager);
            const auto target = BorrowBoundedByteArrayCopySpan(targetManager.KeyReferenceAt1C());
            if (copyCount > currentInput.size() || copyCount > target.size())
                throw std::out_of_range("Params key memmove exceeds the current source or target extent");
            // 分支在445A00已固定；晚取输入即使变成BMR也不能再次识别。
            if (copyCount) std::memmove(target.data(), currentInput.data(), copyCount);
        }
        else
        {
            // ESI保留原source+10h；第二次decoder获取可改变Header，不重新取GS压缩正文。
            source.CurrentManagerAt479FF0().KeyReferenceAt1C().AllocateAt412700(outputSize);
            auto& targetManager = source.CurrentManagerAt479FF0();
            auto& decodingDecoder = CaptureDecoder(codecs);
            source.ValidateCapturedManager(&targetManager);
            const auto target = BorrowBoundedByteArrayCopySpan(targetManager.KeyReferenceAt1C());
            const auto payloadBytes = input.subspan(16);
            if (payload != payloadBytes.data())
                throw std::logic_error("BMR identification returned an unexpected borrowed payload");
            DestinationAccessGuard destinationGuard(source, targetManager);
            decodingDecoder.DecodeAt43DCF0(payloadBytes, target, codecs.managers, codecs.errors,
                &readGuard, &destinationGuard);
        }
    }
    // 48A105先取当前GS；Reset内部payload回调后仍重读该槽control，不再重新取根。
    source.CurrentBlobAt418480().storage.Reset();
}
}

void InstallParamsKeyAt489F7C(IParamsKeyInstallationSource& source, ArchiveCodecServices codecs)
{ InstallParamsKeyCore(source, codecs); }

void InstallParamsKeyAt489F7C(IParamsKeyNativeInstallationSource& source, ArchiveCodecServices codecs)
{ InstallParamsKeyCore(source, codecs); }
}

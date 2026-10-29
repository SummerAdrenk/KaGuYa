#pragma once

#include <Windows.h>
#include <mmreg.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>

namespace KaGuYa::Sound
{
enum class AudioInputKind
{
    WavePcm,
    OggVorbis,
    None,
};

enum class AudioDecodeState
{
    Ready,
    EndOfStream,
    Failed,
};

enum class AudioError
{
    None,
    InvalidArgument,
    EmptyPath,
    FileOpenFailed,
    FileSeekFailed,
    FileSizeFailed,
    SegmentOutOfRange,
    SegmentTooLarge,
    SignatureReadFailed,
    UnsupportedSignature,
    InvalidRiffHeader,
    InvalidWaveHeader,
    MissingFormatChunk,
    InvalidFormatChunkSize,
    TruncatedChunk,
    UnsupportedWaveFormat,
    UnsupportedChannelCount,
    UnsupportedBitsPerSample,
    InvalidWaveFormat,
    MissingDataChunk,
    VorbisOpenFailed,
    VorbisNotSeekable,
    VorbisLengthFailed,
    BufferNotBlockAligned,
    ReadFailed,
    DecodeFailed,
    SeekFailed,
    PcmDataTooLarge,
    OutOfMemory,
    InternalError,
};

struct AudioOpenRequest
{
    std::wstring logicalPath;
    std::wstring physicalPath;
    std::uint64_t offset = 0;

    // 原03/04：0取整物理文件长度的低DWORD，不减offset；非零值原样作为输入段长度。
    std::uint32_t size = 0;

    // 资源字符串可含首位NUL但声明长度非零；该情况下物理参数仍是非null，不能回退逻辑名。
    bool physicalPathSpecified = false;
};

struct AudioReadResult
{
    std::size_t bytesRead = 0;
    AudioDecodeState state = AudioDecodeState::Failed;
    AudioError error = AudioError::None;

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return error == AudioError::None;
    }
};

class IAudioDecoder
{
public:
    virtual ~IAudioDecoder() = default;

    [[nodiscard]] virtual AudioInputKind Kind() const noexcept = 0;
    [[nodiscard]] virtual const WAVEFORMATEX& Format() const noexcept = 0;
    [[nodiscard]] virtual const WAVEFORMATEX* FormatPointer() const noexcept { return &Format(); }
    [[nodiscard]] virtual std::uint64_t TotalPcmBytes() const noexcept = 0;
    [[nodiscard]] virtual std::uint64_t PositionPcmBytes() const noexcept = 0;
    [[nodiscard]] virtual AudioDecodeState State() const noexcept = 0;
    [[nodiscard]] virtual AudioError LastError() const noexcept = 0;

    // WAV按原输入segment/低DWORD位置裁读，普通读失败保留ReadFailed/0及非锁存状态。
    // Vorbis允许真实EOF短读，但ov_read的负错误仍明确失败。
    // 不复制原6F10将负错误码当无符号累计长度的危险行为。
    [[nodiscard]] virtual AudioReadResult ReadPcm(std::span<std::byte> destination) = 0;
    [[nodiscard]] virtual AudioError SeekToStart() = 0;

    // TotalPcmBytes是原格式下的元数据，混合声道Vorbis不能拿它限制ReadPcm或要求填满。
};

// physicalPath 非空时使用它访问磁盘，否则使用 logicalPath；logicalPath 仍由上层保留作资源名。
[[nodiscard]] AudioError OpenAudioDecoder(const AudioOpenRequest& request,
    std::unique_ptr<IAudioDecoder>& decoder, std::wstring* detail = nullptr) noexcept;

[[nodiscard]] const wchar_t* AudioErrorName(AudioError error) noexcept;
}

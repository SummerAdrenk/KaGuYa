#include "AudioDecoder.h"
#include "AudioFile.h"
#include "VorbisInputCallbacks.h"
#include <Logging/Logger.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <limits>
#include <new>
#include <utility>

// 原VorbisInput使用自有文件回调；保留其物理位置/段长公式，不套标准FILE回调。
#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>

namespace KaGuYa::Sound
{
namespace
{
using FilePointer = std::unique_ptr<AudioFile>;

constexpr std::array<std::byte, 4> RiffSignature{
    std::byte{'R'}, std::byte{'I'}, std::byte{'F'}, std::byte{'F'}};
constexpr std::array<std::byte, 4> OggSignature{
    std::byte{'O'}, std::byte{'g'}, std::byte{'g'}, std::byte{'S'}};
constexpr std::array<std::byte, 4> WaveSignature{
    std::byte{'W'}, std::byte{'A'}, std::byte{'V'}, std::byte{'E'}};
constexpr std::array<std::byte, 4> FormatChunk{
    std::byte{'f'}, std::byte{'m'}, std::byte{'t'}, std::byte{' '}};
constexpr std::array<std::byte, 4> DataChunk{
    std::byte{'d'}, std::byte{'a'}, std::byte{'t'}, std::byte{'a'}};

struct OpenedSegment
{
    FilePointer file;
    std::wstring logicalPath;
    std::wstring sourcePath;
    std::uint64_t offset = 0;
    std::uint32_t size = 0;
};

void SetDetail(std::wstring* const detail, const wchar_t* const text)
{
    if (detail != nullptr)
    {
        *detail = text;
    }
}

bool SeekAbsolute(AudioFile* const file, const std::uint64_t position) noexcept
{
    if (file == nullptr || position > static_cast<std::uint64_t>((std::numeric_limits<__int64>::max)()))
    {
        return false;
    }
    return file->Seek(static_cast<std::int64_t>(position), FILE_BEGIN);
}

bool ReadExact(AudioFile* const file, void* const destination, const std::size_t size) noexcept
{
    if (size == 0)
    {
        return true;
    }
    DWORD actual = 0;
    return file != nullptr && destination != nullptr && size <= (std::numeric_limits<DWORD>::max)()
        && file->Read(destination, static_cast<DWORD>(size), actual) && actual == size;
}

std::uint16_t ReadU16Le(const std::byte* const source) noexcept
{
    return static_cast<std::uint16_t>(std::to_integer<unsigned char>(source[0]))
        | static_cast<std::uint16_t>(std::to_integer<unsigned char>(source[1]) << 8);
}

std::uint32_t ReadU32Le(const std::byte* const source) noexcept
{
    return static_cast<std::uint32_t>(std::to_integer<unsigned char>(source[0]))
        | (static_cast<std::uint32_t>(std::to_integer<unsigned char>(source[1])) << 8)
        | (static_cast<std::uint32_t>(std::to_integer<unsigned char>(source[2])) << 16)
        | (static_cast<std::uint32_t>(std::to_integer<unsigned char>(source[3])) << 24);
}

AudioError OpenSegment(const AudioOpenRequest& request, OpenedSegment& segment,
    std::wstring* const detail)
{
    const std::wstring& sourcePath = request.physicalPathSpecified || !request.physicalPath.empty()
        ? request.physicalPath : request.logicalPath;
    if (sourcePath.empty())
    {
        SetDetail(detail, L"选中的音频物理路径为空");
        return AudioError::EmptyPath;
    }

    auto file = std::make_unique<AudioFile>();
    if (!file->Open(sourcePath.c_str()))
    {
        SetDetail(detail, L"无法以只读方式打开音频物理文件");
        return AudioError::FileOpenFailed;
    }
    if (!SeekAbsolute(file.get(), request.offset))
    {
        SetDetail(detail, L"无法定位音频段起点");
        return AudioError::FileSeekFailed;
    }

    segment.file = std::move(file);
    segment.logicalPath = request.logicalPath;
    segment.sourcePath = sourcePath;
    segment.offset = request.offset;
    segment.size = request.size;
    return AudioError::None;
}

class DecoderBase : public IAudioDecoder
{
public:
    const WAVEFORMATEX& Format() const noexcept override { return format_; }
    std::uint64_t TotalPcmBytes() const noexcept override { return totalPcmBytes_; }
    std::uint64_t PositionPcmBytes() const noexcept override { return positionPcmBytes_; }
    AudioDecodeState State() const noexcept override { return state_; }
    AudioError LastError() const noexcept override { return lastError_; }

protected:
    AudioReadResult Failure(const AudioError error) noexcept
    {
        lastError_ = error;
        state_ = AudioDecodeState::Failed;
        return {0, state_, error};
    }

    AudioReadResult Success(const std::size_t bytesRead) noexcept
    {
        lastError_ = AudioError::None;
        state_ = positionPcmBytes_ >= totalPcmBytes_
            ? AudioDecodeState::EndOfStream
            : AudioDecodeState::Ready;
        return {bytesRead, state_, AudioError::None};
    }

    WAVEFORMATEX format_{};
    std::uint64_t totalPcmBytes_ = 0;
    std::uint64_t positionPcmBytes_ = 0;
    AudioDecodeState state_ = AudioDecodeState::Ready;
    AudioError lastError_ = AudioError::None;
};

class WaveDecoder final : public DecoderBase
{
public:
    explicit WaveDecoder(OpenedSegment&& segment) noexcept
        : file_(std::move(segment.file)), segmentOffset_(segment.offset), segmentSize_(segment.size)
    {
    }

    AudioInputKind Kind() const noexcept override { return AudioInputKind::WavePcm; }

    AudioError Initialize(std::wstring* detail) noexcept
    {
        if (!SeekAbsolute(file_.get(), segmentOffset_))
        {
            SetDetail(detail, L"RIFF/WAVE 文件头被截断");
            return AudioError::InvalidRiffHeader;
        }

        std::array<std::byte, 12> riffHeader{};
        if (!ReadExact(file_.get(), riffHeader.data(), riffHeader.size()))
        {
            SetDetail(detail, L"读取 RIFF/WAVE 文件头失败");
            return AudioError::SignatureReadFailed;
        }
        if (!std::equal(RiffSignature.begin(), RiffSignature.end(), riffHeader.begin()))
        {
            SetDetail(detail, L"RIFF 标记不匹配");
            return AudioError::InvalidRiffHeader;
        }
        if (!std::equal(WaveSignature.begin(), WaveSignature.end(), riffHeader.begin() + 8))
        {
            SetDetail(detail, L"WAVE 标记不匹配");
            return AudioError::InvalidWaveHeader;
        }

        std::uint64_t cursor = segmentOffset_ + riffHeader.size();
        // 原5D80/5AC0按物理文件顺序找fmt/data；段长只在PCM读取时裁剪。
        for (;;)
        {
            if (!SeekAbsolute(file_.get(), cursor))
            {
                SetDetail(detail, L"定位 WAV 块失败");
                return AudioError::FileSeekFailed;
            }

            std::array<std::byte, 8> chunkHeader{};
            if (!ReadExact(file_.get(), chunkHeader.data(), chunkHeader.size()))
            {
                SetDetail(detail, L"读取 WAV 块头失败");
                return AudioError::ReadFailed;
            }
            const std::uint32_t chunkSize = ReadU32Le(chunkHeader.data() + 4);
            const std::uint64_t payload = cursor + chunkHeader.size();

            if (std::equal(FormatChunk.begin(), FormatChunk.end(), chunkHeader.begin()))
            {
                if (chunkSize != 16)
                {
                    SetDetail(detail, L"fmt 块长度不是原引擎要求的 16 字节");
                    return AudioError::InvalidFormatChunkSize;
                }
                std::array<std::byte, 16> rawFormat{};
                if (!ReadExact(file_.get(), rawFormat.data(), rawFormat.size()))
                {
                    SetDetail(detail, L"fmt 块内容被截断");
                    return AudioError::TruncatedChunk;
                }

                format_.wFormatTag = ReadU16Le(rawFormat.data());
                format_.nChannels = ReadU16Le(rawFormat.data() + 2);
                format_.nSamplesPerSec = ReadU32Le(rawFormat.data() + 4);
                format_.nAvgBytesPerSec = ReadU32Le(rawFormat.data() + 8);
                format_.nBlockAlign = ReadU16Le(rawFormat.data() + 12);
                format_.wBitsPerSample = ReadU16Le(rawFormat.data() + 14);
                format_.cbSize = 0;

                if (format_.wFormatTag != WAVE_FORMAT_PCM)
                {
                    SetDetail(detail, L"WAV fmt 不是 PCM(format tag=1)");
                    return AudioError::UnsupportedWaveFormat;
                }
                // 原输入层只限定fmt长度16/tag1；设备格式接受域由Create处理。
                // 使用blockAlign作除数的Polling仍在其危险操作前明确拒绝零值。
                cursor = payload + chunkSize;
                break;
            }

            // 原 WaveInput 的块扫描按 chunkSize 前进，不额外猜测 RIFF 奇数字节填充。
            cursor = payload + chunkSize;
        }

        for (;;)
        {
            if (!SeekAbsolute(file_.get(), cursor))
            {
                SetDetail(detail, L"定位 WAV data 块失败");
                return AudioError::FileSeekFailed;
            }

            std::array<std::byte, 8> chunkHeader{};
            if (!ReadExact(file_.get(), chunkHeader.data(), chunkHeader.size()))
            {
                SetDetail(detail, L"读取 WAV data 块头失败");
                return AudioError::ReadFailed;
            }
            const std::uint32_t chunkSize = ReadU32Le(chunkHeader.data() + 4);
            const std::uint64_t payload = cursor + chunkHeader.size();
            if (std::equal(DataChunk.begin(), DataChunk.end(), chunkHeader.begin()))
            {
                dataOffset_ = payload;
                totalPcmBytes_ = chunkSize;
                // 5D80:61155不检查Seek(0)的普通返回，合法空data也不因此Open失败。
                static_cast<void>(SeekToStart());
                return AudioError::None;
            }
            cursor = payload + chunkSize;
        }

    }

    AudioReadResult ReadPcm(const std::span<std::byte> destination) noexcept override
    {
        if (state_ == AudioDecodeState::Failed)
        {
            return {0, state_, lastError_};
        }
        if (!destination.empty() && destination.data() == nullptr)
        {
            return Failure(AudioError::InvalidArgument);
        }
        if (destination.empty())
        {
            return Success(0);
        }

        // 6470:64FB..6511按输入segment而非data块裁剪，且只用位置低DWORD。
        const auto physical = file_->Position();
        if (physical < 0)
        {
            // 原位置失败返回-1后仍做同一运算并Read，不能改成零读/静音。
            Logging::Write(Logging::Level::Warning,
                "Wave position query failed; retaining original low-DWORD read calculation.");
        }
        const auto desired = static_cast<std::uint32_t>(destination.size());
        const std::uint32_t expression = static_cast<std::uint32_t>(physical) - segmentSize_ -
            static_cast<std::uint32_t>(segmentOffset_) + desired;
        const auto excess = std::bit_cast<std::int32_t>(expression) >= 0 ? expression : 0u;
        const std::uint32_t requested = desired - excess;
        if (requested > desired)
        {
            // 原越过段尾后可发生DWORD下溢，产生超出调用方缓冲的请求；不复制越界。
            Logging::Write(Logging::Level::Error, "Wave read calculation exceeds destination: rejected.");
            return Failure(AudioError::SegmentOutOfRange);
        }
        DWORD read = 0;
        if (!file_->Read(destination.data(), requested, read))
        {
            // 普通文件失败返回0，不把输入永久锁成Failed；已写前缀仍留在调用方缓冲。
            lastError_ = AudioError::ReadFailed;
            return {0, state_, lastError_};
        }
        positionPcmBytes_ += read;
        return Success(read);
    }

    AudioError SeekToStart() noexcept override
    {
        // 6300:632F..6336先拒绝position>=PCM总长；请求0时空data不发任何seek。
        if (totalPcmBytes_ == 0)
        {
            state_ = AudioDecodeState::EndOfStream;
            lastError_ = AudioError::SeekFailed;
            return lastError_;
        }
        positionPcmBytes_ = 0;
        state_ = AudioDecodeState::Ready;
        // 637E/63DE分别绝对定位segment起点、data起点，两次普通返回均不决定成功。
        const bool segmentMoved = SeekAbsolute(file_.get(), segmentOffset_);
        const bool dataMoved = SeekAbsolute(file_.get(), dataOffset_);
        if (!segmentMoved || !dataMoved)
        {
            lastError_ = AudioError::SeekFailed;
            Logging::Write(Logging::Level::Warning,
                "Wave seek ordinary failure; original accepted-request return retained.");
        }
        else lastError_ = AudioError::None;
        // 640D无条件返回1；None表示原请求被接受，不抹掉上面的底层失败诊断。
        return AudioError::None;
    }

private:
    FilePointer file_;
    std::uint64_t segmentOffset_ = 0;
    std::uint32_t segmentSize_ = 0;
    std::uint64_t dataOffset_ = 0;
};

class VorbisDecoder final : public DecoderBase
{
public:
    explicit VorbisDecoder(OpenedSegment&& segment) noexcept
        : file_(std::move(segment.file)), callbacks_{*file_, segment.offset, segment.size}
    {
    }

    ~VorbisDecoder() override
    {
        if (opened_)
        {
            ov_clear(&vorbis_);
        }
    }

    AudioInputKind Kind() const noexcept override { return AudioInputKind::OggVorbis; }

    AudioError Initialize(std::wstring* detail) noexcept
    {
        // 工厂已读签名并回到起点；6CB0本身不额外seek或回卷。
        const ov_callbacks callbacks{&VorbisInputCallbacks::Read, &VorbisInputCallbacks::Seek,
            &VorbisInputCallbacks::Close, &VorbisInputCallbacks::Tell};
        if (ov_open_callbacks(&callbacks_, &vorbis_, nullptr, 0, callbacks) != 0)
        {
            SetDetail(detail, L"ov_open_callbacks 无法打开指定 Ogg 文件段");
            return AudioError::VorbisOpenFailed;
        }
        opened_ = true;
        if (!ov_seekable(&vorbis_))
        {
            SetDetail(detail, L"Ogg 输入不支持回到流首");
            return AudioError::VorbisNotSeekable;
        }

        const auto* info = ov_info(&vorbis_, -1);
        if (info == nullptr)
        {
            SetDetail(detail, L"ov_info 未提供有效格式");
            return AudioError::VorbisOpenFailed;
        }
        // 原10006DF5..10006E1D直接按Vorbis声道数填写PCM格式，不限于mono/stereo。
        if (info->channels <= 0)
        {
            SetDetail(detail, L"Ogg Vorbis 声道数无效");
            return AudioError::UnsupportedChannelCount;
        }
        if (info->rate <= 0)
        {
            SetDetail(detail, L"Ogg Vorbis 采样率不是正数");
            return AudioError::InvalidWaveFormat;
        }

        format_.wFormatTag = WAVE_FORMAT_PCM;
        format_.nChannels = static_cast<WORD>(info->channels);
        format_.nSamplesPerSec = static_cast<DWORD>(info->rate);
        format_.wBitsPerSample = 16;
        format_.nBlockAlign = static_cast<WORD>(info->channels * sizeof(std::int16_t));
        const std::uint64_t average =
            static_cast<std::uint64_t>(format_.nSamplesPerSec) * format_.nBlockAlign;
        if (average > (std::numeric_limits<DWORD>::max)())
        {
            SetDetail(detail, L"Ogg Vorbis PCM 平均字节率溢出 WAVEFORMATEX");
            return AudioError::InvalidWaveFormat;
        }
        format_.nAvgBytesPerSec = static_cast<DWORD>(average);
        format_.cbSize = 0;

        // 原10006E90 ->100160C0(vf,-1)：总长度是所有逻辑流之和，不是第一段长度。
        const auto totalSamples = ov_pcm_total(&vorbis_, -1);
        if (totalSamples < 0 || static_cast<std::uint64_t>(totalSamples) >
            (std::numeric_limits<std::uint64_t>::max)() / format_.nBlockAlign)
        {
            SetDetail(detail, L"Ogg Vorbis PCM 总长度溢出");
            return AudioError::VorbisLengthFailed;
        }
        totalPcmBytes_ = static_cast<std::uint64_t>(totalSamples) * format_.nBlockAlign;
        // Static/Polling Open随后各自Seek(0)，并忽略普通返回；不能在这里先多做一次且升级为Open失败。
        return AudioError::None;
    }

    AudioReadResult ReadPcm(const std::span<std::byte> destination) noexcept override
    {
        if (state_ == AudioDecodeState::Failed)
        {
            return {0, state_, lastError_};
        }
        if (!opened_ || (!destination.empty() && destination.data() == nullptr))
        {
            return Failure(AudioError::InvalidArgument);
        }
        if (format_.nBlockAlign == 0)
        {
            return Failure(AudioError::BufferNotBlockAligned);
        }
        if (destination.empty())
        {
            return {0, state_, AudioError::None};
        }

        // 原10006F10不按10006E90元数据裁读；跨声道link后实际字节量可大于或小于它。
        const std::size_t requestedBytes = destination.size();
        std::size_t decodedBytes = 0;
        bool reachedEnd = false;
        auto* output = reinterpret_cast<char*>(destination.data());

        // 原10006F10反复ov_read，小端/16位/有符号；跨link由库处理，不手工拼接PCM。
        while (decodedBytes < requestedBytes)
        {
            const std::size_t pending = requestedBytes - decodedBytes;
            const int request = static_cast<int>((std::min)(pending,
                static_cast<std::size_t>((std::numeric_limits<int>::max)())));
            const auto bytes = ov_read(&vorbis_, output + decodedBytes, request, 0, 2, 1, nullptr);
            if (bytes < 0)
            {
                return Failure(AudioError::DecodeFailed);
            }
            if (bytes == 0)
            {
                reachedEnd = true;
                break;
            }
            decodedBytes += static_cast<std::size_t>(bytes);
        }

        positionPcmBytes_ += decodedBytes;
        lastError_ = AudioError::None;
        state_ = reachedEnd ? AudioDecodeState::EndOfStream : AudioDecodeState::Ready;
        return {decodedBytes, state_, AudioError::None};
    }

    AudioError SeekToStart() noexcept override
    {
        if (!opened_ || ov_pcm_seek(&vorbis_, 0) != 0)
        {
            lastError_ = AudioError::SeekFailed;
            // 原6EC0只返回失败，不给后续6F10增加现代Failed锁存。
            return lastError_;
        }
        positionPcmBytes_ = 0;
        lastError_ = AudioError::None;
        state_ = totalPcmBytes_ == 0 ? AudioDecodeState::EndOfStream : AudioDecodeState::Ready;
        return AudioError::None;
    }

private:
    FilePointer file_;
    VorbisInputCallbacks callbacks_;
    OggVorbis_File vorbis_{};
    bool opened_ = false;
};
}

AudioError OpenAudioDecoder(const AudioOpenRequest& request,
    std::unique_ptr<IAudioDecoder>& decoder, std::wstring* const detail) noexcept
{
    decoder.reset();
    if (detail != nullptr)
    {
        detail->clear();
    }

    try
    {
        OpenedSegment segment;
        AudioError error = OpenSegment(request, segment, detail);
        if (error != AudioError::None)
        {
            return error;
        }
        std::array<std::byte, 4> signature{};
        if (!ReadExact(segment.file.get(), signature.data(), signature.size()))
        {
            SetDetail(detail, L"读取音频格式标记失败");
            return AudioError::SignatureReadFailed;
        }
        if (!SeekAbsolute(segment.file.get(), segment.offset))
        {
            SetDetail(detail, L"读取格式标记后无法回到音频段起点");
            return AudioError::FileSeekFailed;
        }

        if (signature == RiffSignature)
        {
            if (request.size == 0) segment.size = static_cast<std::uint32_t>(segment.file->Size());
            auto wave = std::make_unique<WaveDecoder>(std::move(segment));
            error = wave->Initialize(detail);
            if (error == AudioError::None)
            {
                decoder = std::move(wave);
            }
            return error;
        }
        if (signature == OggSignature)
        {
            if (request.size == 0) segment.size = static_cast<std::uint32_t>(segment.file->Size());
            auto vorbis = std::make_unique<VorbisDecoder>(std::move(segment));
            error = vorbis->Initialize(detail);
            if (error == AudioError::None)
            {
                decoder = std::move(vorbis);
            }
            return error;
        }

        SetDetail(detail, L"音频段既不是 RIFF/WAVE，也不是 OggS Vorbis");
        return AudioError::UnsupportedSignature;
    }
    catch (const std::bad_alloc&)
    {
        SetDetail(detail, L"创建音频解码器时内存不足");
        return AudioError::OutOfMemory;
    }
    catch (...)
    {
        SetDetail(detail, L"创建音频解码器时发生未分类异常");
        return AudioError::InternalError;
    }
}

const wchar_t* AudioErrorName(const AudioError error) noexcept
{
    switch (error)
    {
    case AudioError::None: return L"None";
    case AudioError::InvalidArgument: return L"InvalidArgument";
    case AudioError::EmptyPath: return L"EmptyPath";
    case AudioError::FileOpenFailed: return L"FileOpenFailed";
    case AudioError::FileSeekFailed: return L"FileSeekFailed";
    case AudioError::FileSizeFailed: return L"FileSizeFailed";
    case AudioError::SegmentOutOfRange: return L"SegmentOutOfRange";
    case AudioError::SegmentTooLarge: return L"SegmentTooLarge";
    case AudioError::SignatureReadFailed: return L"SignatureReadFailed";
    case AudioError::UnsupportedSignature: return L"UnsupportedSignature";
    case AudioError::InvalidRiffHeader: return L"InvalidRiffHeader";
    case AudioError::InvalidWaveHeader: return L"InvalidWaveHeader";
    case AudioError::MissingFormatChunk: return L"MissingFormatChunk";
    case AudioError::InvalidFormatChunkSize: return L"InvalidFormatChunkSize";
    case AudioError::TruncatedChunk: return L"TruncatedChunk";
    case AudioError::UnsupportedWaveFormat: return L"UnsupportedWaveFormat";
    case AudioError::UnsupportedChannelCount: return L"UnsupportedChannelCount";
    case AudioError::UnsupportedBitsPerSample: return L"UnsupportedBitsPerSample";
    case AudioError::InvalidWaveFormat: return L"InvalidWaveFormat";
    case AudioError::MissingDataChunk: return L"MissingDataChunk";
    case AudioError::VorbisOpenFailed: return L"VorbisOpenFailed";
    case AudioError::VorbisNotSeekable: return L"VorbisNotSeekable";
    case AudioError::VorbisLengthFailed: return L"VorbisLengthFailed";
    case AudioError::BufferNotBlockAligned: return L"BufferNotBlockAligned";
    case AudioError::ReadFailed: return L"ReadFailed";
    case AudioError::DecodeFailed: return L"DecodeFailed";
    case AudioError::SeekFailed: return L"SeekFailed";
    case AudioError::PcmDataTooLarge: return L"PcmDataTooLarge";
    case AudioError::OutOfMemory: return L"OutOfMemory";
    case AudioError::InternalError: return L"InternalError";
    }
    return L"UnknownAudioError";
}
}

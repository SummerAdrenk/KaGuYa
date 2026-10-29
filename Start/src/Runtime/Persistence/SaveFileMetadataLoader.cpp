#include <Runtime/SaveFileMetadataLoader.h>

#include <Windows.h>

#include <algorithm>
#include <array>
#include <ctime>
#include <stdexcept>
#include <utility>
#include <vector>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::size_t SaveParams2ByteCount =
    sizeof(std::uint32_t) * (2u + 512u);

class ByteCursor final
{
public:
    explicit ByteCursor(const std::span<const std::byte> bytes) noexcept : bytes_(bytes) {}

    [[nodiscard]] bool ReadDword(std::uint32_t& value) noexcept
    {
        if (bytes_.size() - offset_ < sizeof(value))
            return false;
        const auto* const source = bytes_.data() + offset_;
        value = std::to_integer<std::uint8_t>(source[0]) |
            (static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(source[1])) << 8u) |
            (static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(source[2])) << 16u) |
            (static_cast<std::uint32_t>(std::to_integer<std::uint8_t>(source[3])) << 24u);
        offset_ += sizeof(value);
        return true;
    }

    [[nodiscard]] bool ReadBytes(
        const std::size_t byteCount,
        std::span<const std::byte>& value) noexcept
    {
        if (byteCount > bytes_.size() - offset_)
            return false;
        value = bytes_.subspan(offset_, byteCount);
        offset_ += byteCount;
        return true;
    }

private:
    std::span<const std::byte> bytes_;
    std::size_t offset_ = 0;
};

void XorBlock(std::vector<std::byte>& bytes, const std::uint8_t key) noexcept
{
    for (std::byte& value : bytes)
    {
        value ^= static_cast<std::byte>(key);
    }
}

[[nodiscard]] bool ReadCString(
    const std::span<const std::byte> bytes,
    std::size_t& offset,
    std::string& value)
{
    if (offset > bytes.size())
        return false;
    const auto first = bytes.begin() + static_cast<std::ptrdiff_t>(offset);
    const auto terminator = std::find(first, bytes.end(), std::byte{});
    if (terminator == bytes.end())
        return false;
    const std::size_t length = static_cast<std::size_t>(terminator - first);
    value.assign(reinterpret_cast<const char*>(bytes.data() + offset), length);
    offset += length + 1u;
    return true;
}

[[nodiscard]] bool ReadEncryptedBlock(
    ByteCursor& cursor,
    const std::uint8_t key,
    std::vector<std::byte>& plainBytes)
{
    std::uint32_t byteCount = 0;
    if (!cursor.ReadDword(byteCount))
        return false;

    std::span<const std::byte> encryptedBytes;
    if (!cursor.ReadBytes(byteCount, encryptedBytes))
        return false;
    plainBytes.assign(encryptedBytes.begin(), encryptedBytes.end());
    XorBlock(plainBytes, key);
    return true;
}

[[nodiscard]] bool ValidateThirdBlock(const std::span<const std::byte> bytes) noexcept
{
    if (bytes.empty())
        return true;
    if (bytes.size() < sizeof(std::uint32_t))
        return false;

    ByteCursor cursor(bytes);
    std::uint32_t count = 0;
    if (!cursor.ReadDword(count))
        return false;
    const std::size_t availableCount =
        (bytes.size() - sizeof(std::uint32_t)) / sizeof(std::uint32_t);
    return static_cast<std::size_t>(count) <= availableCount;
}

[[nodiscard]] std::uint64_t ConvertLastWriteTime(const FILETIME& fileTime)
{
    FILETIME localFileTime{};
    SYSTEMTIME systemTime{};
    if (FileTimeToLocalFileTime(&fileTime, &localFileTime) == FALSE ||
        FileTimeToSystemTime(&localFileTime, &systemTime) == FALSE)
    {
        // 45E1A0 在这里抛出非 int 的 COM 错误，不能转成普通槽位失败。
        throw std::runtime_error("save timestamp conversion failed");
    }
    if (systemTime.wYear < 1900u)
        return 0;
    if (systemTime.wMonth < 1u || systemTime.wMonth > 12u ||
        systemTime.wDay < 1u || systemTime.wDay > 31u ||
        systemTime.wHour > 23u || systemTime.wMinute > 59u ||
        systemTime.wSecond > 59u)
    {
        throw std::runtime_error("save timestamp fields are invalid");
    }

    std::tm localTime{};
    localTime.tm_sec = systemTime.wSecond;
    localTime.tm_min = systemTime.wMinute;
    localTime.tm_hour = systemTime.wHour;
    localTime.tm_mday = systemTime.wDay;
    localTime.tm_mon = static_cast<int>(systemTime.wMonth) - 1;
    localTime.tm_year = static_cast<int>(systemTime.wYear) - 1900;
    localTime.tm_isdst = -1;
    const __time64_t converted = _mktime64(&localTime);
    if (converted == static_cast<__time64_t>(-1))
        throw std::runtime_error("save timestamp is outside the CTime range");
    return static_cast<std::uint64_t>(converted);
}

[[nodiscard]] bool ReadFileExact(
    const HANDLE file,
    void* const buffer,
    const DWORD byteCount) noexcept
{
    if (byteCount == 0)
        return true;
    DWORD bytesRead = 0;
    return ReadFile(file, buffer, byteCount, &bytesRead, nullptr) != FALSE &&
        bytesRead == byteCount;
}

void AppendDwordBytes(std::vector<std::byte>& bytes, const std::uint32_t value)
{
    bytes.push_back(static_cast<std::byte>(value));
    bytes.push_back(static_cast<std::byte>(value >> 8u));
    bytes.push_back(static_cast<std::byte>(value >> 16u));
    bytes.push_back(static_cast<std::byte>(value >> 24u));
}

[[nodiscard]] bool ReadContainerBlock(
    const HANDLE file,
    std::vector<std::byte>& containerBytes)
{
    std::uint32_t byteCount = 0;
    if (!ReadFileExact(file, &byteCount, sizeof(byteCount)))
        return false;
    AppendDwordBytes(containerBytes, byteCount);

    const std::size_t oldSize = containerBytes.size();
    if (static_cast<std::size_t>(byteCount) > containerBytes.max_size() - oldSize)
        throw std::length_error("save block exceeds the x86 vector limit");
    containerBytes.resize(oldSize + static_cast<std::size_t>(byteCount));
    return ReadFileExact(file, containerBytes.data() + oldSize, byteCount);
}

class ScopedHandle final
{
public:
    explicit ScopedHandle(const HANDLE handle) noexcept : handle_(handle) {}
    ~ScopedHandle()
    {
        if (handle_ != INVALID_HANDLE_VALUE)
            CloseHandle(handle_);
    }
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;

private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
};
}

SaveFileFormatProfile CreateNativeSaveFileFormatProfile(const SaveFileMagicNativeSlot& slot) noexcept
{ return {{}, &slot}; }

SaveFileFormatProfile CreateMamatomoSaveFileFormatProfile()
{
    // CP932: "データを改造すると誤動作します。"。此处写死字节，避免受源码/ACP 影响。
    return {std::string(
        "\x83\x66\x81\x5B\x83\x5E\x82\xF0\x89\xFC\x91\xA2\x82\xB7\x82\xE9"
        "\x82\xC6\x8C\xEB\x93\xAE\x8D\xEC\x82\xB5\x82\xDC\x82\xB7\x81\x42",
        32u)};
}

bool TryParseSaveFileMetadataBytes(
    const std::span<const std::byte> fileBytes,
    const SaveFileFormatProfile& profile,
    SaveFileMetadata& metadata)
{
    if (profile.nativeMagicSlot)
        throw std::invalid_argument("independent metadata parser requires an owned magic profile");
    ByteCursor cursor(fileBytes);
    std::span<const std::byte> magic;
    if (!cursor.ReadBytes(profile.magicBytes.size(), magic) ||
        !std::equal(magic.begin(), magic.end(),
            reinterpret_cast<const std::byte*>(profile.magicBytes.data())))
    {
        return false;
    }

    std::uint32_t xorSeed = 0;
    if (!cursor.ReadDword(xorSeed))
        return false;
    const auto xorKey = static_cast<std::uint8_t>(xorSeed);

    std::vector<std::byte> saveParams;
    if (!ReadEncryptedBlock(cursor, xorKey, saveParams))
        return false;
    std::size_t stringOffset = 0;
    std::string titleAt08;
    std::string descriptionAt04;
    // 48B720：CSaveParams+8 在前，+4 在后。
    if (!ReadCString(saveParams, stringOffset, titleAt08) ||
        !ReadCString(saveParams, stringOffset, descriptionAt04))
    {
        return false;
    }

    std::vector<std::byte> saveParams2;
    if (!ReadEncryptedBlock(cursor, xorKey, saveParams2) ||
        saveParams2.size() < SaveParams2ByteCount)
    {
        return false;
    }

    std::vector<std::byte> thirdBlock;
    if (!ReadEncryptedBlock(cursor, xorKey, thirdBlock) ||
        !ValidateThirdBlock(thirdBlock))
    {
        return false;
    }

    // 470B3F/470C08：目录优先使用 +4，空时才取 +8；与磁盘顺序相反。
    metadata.primaryLabel = std::move(descriptionAt04);
    metadata.secondaryLabel = std::move(titleAt08);
    return true;
}

StartSaveFileMetadataLoader::StartSaveFileMetadataLoader(SaveFileFormatProfile profile)
    : profile_(std::move(profile))
{
    if (profile_.magicBytes.empty())
        throw std::invalid_argument("save file magic must not be empty");
}

bool StartSaveFileMetadataLoader::TryLoad(
    const std::string& path,
    SaveFileMetadata& metadata)
{
    SECURITY_ATTRIBUTES securityAttributes{};
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.bInheritHandle = TRUE;
    const HANDLE file = CreateFileA(path.c_str(), GENERIC_READ, 0, &securityAttributes,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return false;
    const ScopedHandle closeFile(file);

    WIN32_FIND_DATAA findData{};
    const HANDLE find = FindFirstFileA(path.c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE)
        return false;
    if (FindClose(find) == FALSE)
        return false;

    // 元数据读取器验证三块，不验证尾部EOF；原4C7420的mode0只读首块。
    std::vector<std::byte> bytes(profile_.magicBytes.size());
    if (!ReadFileExact(file, bytes.data(), static_cast<DWORD>(bytes.size())))
        return false;
    std::uint32_t xorSeed = 0;
    if (!ReadFileExact(file, &xorSeed, sizeof(xorSeed)))
        return false;
    AppendDwordBytes(bytes, xorSeed);
    if (!ReadContainerBlock(file, bytes) || !ReadContainerBlock(file, bytes) ||
        !ReadContainerBlock(file, bytes))
    {
        return false;
    }

    SaveFileMetadata parsed;
    if (!TryParseSaveFileMetadataBytes(bytes, profile_, parsed))
        return false;
    parsed.lastWriteTime = ConvertLastWriteTime(findData.ftLastWriteTime);
    metadata = std::move(parsed);
    return true;
}
}

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <array>
#include <charconv>
#include <climits>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace fs = std::filesystem;
namespace {
using Bytes = std::vector<unsigned char>;
using Name = std::variant<std::uint32_t, std::wstring>;
using Key = std::array<Name, 3>;
void Require(bool value, const char* message) { if (!value) throw std::runtime_error(message); }
struct Handle {
    HANDLE value = INVALID_HANDLE_VALUE;
    ~Handle() { if (value && value != INVALID_HANDLE_VALUE) CloseHandle(value); }
};
Bytes Read(const fs::path& path) {
    std::ifstream stream(path, std::ios::binary | std::ios::ate);
    Require(bool(stream), "Cannot read input file");
    const auto size = stream.tellg();
    Require(size >= 0 && static_cast<std::uint64_t>(size) <= Bytes{}.max_size(), "File is too large");
    Bytes result(static_cast<std::size_t>(size));
    stream.seekg(0);
    if (!result.empty()) stream.read(reinterpret_cast<char*>(result.data()), static_cast<std::streamsize>(result.size()));
    Require(bool(stream), "Truncated file read");
    return result;
}
std::uint32_t Number(const std::string& text) {
    std::uint32_t result = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), result);
    Require(error == std::errc{} && end == text.data() + text.size(), "Invalid unsigned number");
    return result;
}
std::wstring Utf8(const std::string& value) {
    if (value.empty()) return {};
    Require(value.size() <= INT_MAX, "UTF-8 resource name is too large");
    const int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), nullptr, 0);
    Require(count > 0, "Invalid UTF-8 resource name");
    std::wstring result(count, L'\0');
    Require(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), result.data(), count) == count, "UTF-8 conversion failed");
    return result;
}
Name ParseName(const std::string& token) {
    Require(!token.empty(), "Empty resource key token");
    if (token[0] == 'n') return Number(token.substr(1));
    Require(token[0] == 's' && token.size() % 2 == 1, "Invalid resource string encoding");
    std::string value;
    for (std::size_t i = 1; i < token.size(); i += 2) {
        unsigned byte = 0;
        const auto [end, error] = std::from_chars(token.data() + i, token.data() + i + 2, byte, 16);
        Require(error == std::errc{} && end == token.data() + i + 2, "Invalid hexadecimal resource name");
        value.push_back(static_cast<char>(byte));
    }
    return Utf8(value);
}
std::map<Key, std::uint32_t> ReadTable(const fs::path& path) {
    std::ifstream stream(path, std::ios::binary);
    Require(bool(stream), "Cannot read resource metadata table");
    std::string line;
    Require(bool(std::getline(stream, line)), "Missing metadata header");
    if (!line.empty() && line.back() == '\r') line.pop_back();
    Require(line == "KAGUYA_RESOURCE_METADATA_V1", "Unsupported metadata table schema");
    std::map<Key, std::uint32_t> result;
    while (std::getline(stream, line)) {
        std::istringstream fields(line);
        std::string a, b, c, cp, extra;
        Require(bool(fields >> a >> b >> c >> cp) && !(fields >> extra), "Invalid metadata record");
        Require(result.emplace(Key{ParseName(a), ParseName(b), ParseName(c)}, Number(cp)).second, "Duplicate metadata key");
    }
    Require(stream.eof(), "Metadata table read failed");
    return result;
}
struct Leaf { std::size_t codepage, payload, length; };
class Image {
    const Bytes& data_;
    struct Section { std::uint32_t rva, size, offset; };
    std::vector<Section> sections_;
    std::uint64_t root_ = 0, resourceSize_ = 0;
    std::set<std::uint32_t> active_;
    void Bound(std::uint64_t offset, std::uint64_t size) const {
        Require(offset <= data_.size() && size <= data_.size() - offset, "Truncated PE structure");
    }
    void ResourceBound(std::uint64_t offset, std::uint64_t size) const {
        Require(offset <= resourceSize_ && size <= resourceSize_ - offset, "Resource directory offset out of range");
        Bound(root_ + offset, size);
    }
    std::size_t Map(std::uint32_t rva, std::uint32_t size) const {
        for (const auto& section : sections_) {
            if (rva >= section.rva && std::uint64_t(rva) + size <= std::uint64_t(section.rva) + section.size) {
                const auto offset = std::uint64_t(section.offset) + rva - section.rva;
                Bound(offset, size);
                return static_cast<std::size_t>(offset);
            }
        }
        throw std::runtime_error("RVA has no file-backed section");
    }
    Name ResourceName(std::uint32_t entry) const {
        if (!(entry & 0x80000000u)) return entry;
        const auto offset = entry & 0x7fffffffu;
        ResourceBound(offset, 2);
        const auto count = U16(root_ + offset);
        ResourceBound(offset, 2ull + count * 2ull);
        std::wstring value;
        for (unsigned i = 0; i < count; ++i) value.push_back(static_cast<wchar_t>(U16(root_ + offset + 2ull + i * 2ull)));
        if (!value.empty()) Require(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value.data(), count, nullptr, 0, nullptr, nullptr) > 0, "Invalid UTF-16 resource name");
        return value;
    }
    void Visit(std::uint32_t offset, std::vector<Name> parents) {
        Require(parents.size() < 3 && active_.insert(offset).second, "Invalid or cyclic resource directory");
        ResourceBound(offset, 16);
        const unsigned count = unsigned(U16(root_ + offset + 12)) + U16(root_ + offset + 14);
        ResourceBound(offset, 16ull + 8ull * count);
        for (unsigned i = 0; i < count; ++i) {
            const auto at = root_ + offset + 16ull + i * 8ull;
            auto key = parents;
            key.push_back(ResourceName(U32(at)));
            const auto child = U32(at + 4);
            if (child & 0x80000000u) Visit(child & 0x7fffffffu, key);
            else {
                Require(key.size() == 3, "Resource leaf must have type, ID and language");
                ResourceBound(child, 16);
                const auto size = U32(root_ + child + 4);
                const auto payload = Map(U32(root_ + child), size);
                Require(leaves.emplace(Key{key[0], key[1], key[2]}, Leaf{static_cast<std::size_t>(root_ + child + 8), payload, size}).second, "Duplicate PE resource key");
            }
        }
        active_.erase(offset);
    }
public:
    std::map<Key, Leaf> leaves;
    std::uint16_t U16(std::uint64_t at) const { Bound(at, 2); return std::uint16_t(data_[at] | (unsigned(data_[at + 1]) << 8)); }
    std::uint32_t U32(std::uint64_t at) const { Bound(at, 4); return std::uint32_t(U16(at)) | (std::uint32_t(U16(at + 2)) << 16); }
    explicit Image(const Bytes& data) : data_(data) {
        Require(U16(0) == 0x5a4d, "Expected MZ image");
        const std::uint64_t pe = U32(0x3c), optional = pe + 24;
        Require(U32(pe) == 0x00004550 && U16(pe + 4) == 0x14c && U16(optional) == 0x10b, "Expected x86 PE32");
        const auto optionalSize = U16(pe + 20);
        Require(optionalSize >= 224 && U32(optional + 92) >= 5, "Truncated PE32 optional header");
        Bound(optional, optionalSize);
        Require(U32(optional + 64) == 0, "Expected generated image with zero checksum");
        Require(U32(optional + 128) == 0 && U32(optional + 132) == 0, "Refusing a signed image");
        const auto count = U16(pe + 6);
        Bound(optional + optionalSize, std::uint64_t(count) * 40);
        for (unsigned i = 0; i < count; ++i) {
            const auto at = optional + optionalSize + i * 40ull;
            sections_.push_back({U32(at + 12), U32(at + 16), U32(at + 20)});
        }
        resourceSize_ = U32(optional + 116);
        Require(U32(optional + 112) != 0 && resourceSize_ > 0, "Missing resource directory");
        root_ = Map(U32(optional + 112), static_cast<std::uint32_t>(resourceSize_));
        Visit(0, {});
    }
};
bool Within(const fs::path& path, const fs::path& parent) {
    auto child = path.begin();
    for (auto part = parent.begin(); part != parent.end(); ++part, ++child) {
        if (child == path.end() || CompareStringOrdinal(child->c_str(), -1, part->c_str(), -1, TRUE) != CSTR_EQUAL) return false;
    }
    return child != path.end();
}
std::wstring Quote(const std::wstring& arg) {
    std::wstring result = L"\"";
    std::size_t slashes = 0;
    for (const auto ch : arg) {
        if (ch == L'\\') { ++slashes; continue; }
        result.append(slashes * (ch == L'\"' ? 2 : 1) + (ch == L'\"' ? 1 : 0), L'\\');
        result.push_back(ch);
        slashes = 0;
    }
    result.append(slashes * 2, L'\\');
    result.push_back(L'\"');
    return result;
}
DWORD RunLinker(int argc, wchar_t** argv, int first) {
    std::wstring command;
    for (int i = first; i < argc; ++i) { if (i != first) command += L' '; command += Quote(argv[i]); }
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startup.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startup.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    PROCESS_INFORMATION process{};
    Require(CreateProcessW(nullptr, command.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &startup, &process) != FALSE, "Cannot launch native linker");
    Handle child{process.hProcess}, thread{process.hThread};
    Require(WaitForSingleObject(child.value, INFINITE) == WAIT_OBJECT_0, "Waiting for native linker failed");
    DWORD result = 0;
    Require(GetExitCodeProcess(child.value, &result) != FALSE, "Cannot read native linker exit code");
    return result;
}
void Publish(const fs::path& image, const Bytes& data) {
    fs::path temporary;
    Handle file;
    for (unsigned attempt = 0; attempt < 32; ++attempt) {
        temporary = image.native() + L".metadata." + std::to_wstring(GetCurrentProcessId()) + L"." + std::to_wstring(GetTickCount64()) + L"." + std::to_wstring(attempt) + L".tmp";
        file.value = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file.value != INVALID_HANDLE_VALUE) break;
        Require(GetLastError() == ERROR_FILE_EXISTS, "Cannot create metadata temporary file");
    }
    Require(file.value != INVALID_HANDLE_VALUE, "Cannot obtain a unique metadata temporary file");
    struct Cleanup {
        fs::path path;
        Handle& file;
        ~Cleanup() {
            if (file.value != INVALID_HANDLE_VALUE) { CloseHandle(file.value); file.value = INVALID_HANDLE_VALUE; }
            std::error_code ec;
            fs::remove(path, ec);
        }
    } cleanup{temporary, file};
    std::size_t written = 0;
    while (written < data.size()) {
        const auto count = static_cast<DWORD>((std::min)(data.size() - written, std::size_t(MAXDWORD)));
        DWORD done = 0;
        Require(WriteFile(file.value, data.data() + written, count, &done, nullptr) != FALSE && done > 0, "Cannot write metadata temporary file");
        written += done;
    }
    Require(FlushFileBuffers(file.value) != FALSE, "Cannot flush metadata temporary file");
    CloseHandle(file.value); file.value = INVALID_HANDLE_VALUE;
    Require(Read(temporary) == data, "Metadata temporary file readback differs");
    // 先核完整新文件，再替换链接产物；任何先前失败不触碰原文件。
    Require(MoveFileExW(temporary.c_str(), image.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != FALSE, "Cannot publish metadata image");
}
void Apply(const fs::path& root, const fs::path& path, const fs::path& table) {
    const auto base = fs::canonical(root), image = fs::canonical(path);
    Require(Within(image, base / L".BuildCache") || Within(image, base / L"Release"), "Only generated images under .BuildCache or Release may be modified");
    const auto expected = ReadTable(table);
    const auto before = Read(image);
    const Image original(before);
    Require(original.leaves.size() == expected.size(), "Resource set differs from explicit metadata");
    auto after = before;
    std::set<std::size_t> editable;
    unsigned changed = 0;
    for (const auto& [key, codepage] : expected) {
        const auto item = original.leaves.find(key);
        Require(item != original.leaves.end(), "Resource key missing from image");
        const auto at = item->second.codepage;
        if (original.U32(at) != codepage) ++changed;
        for (unsigned byte = 0; byte < 4; ++byte) { after[at + byte] = static_cast<unsigned char>(codepage >> (byte * 8)); editable.insert(at + byte); }
    }
    const Image verified(after);
    Require(verified.leaves.size() == original.leaves.size(), "Unexpected resource tree change");
    for (const auto& [key, leaf] : original.leaves) {
        const auto& other = verified.leaves.at(key);
        Require(verified.U32(other.codepage) == expected.at(key), "CodePage verification failed");
        Require(leaf.length == other.length && std::equal(before.begin() + leaf.payload, before.begin() + leaf.payload + leaf.length, after.begin() + other.payload), "Resource payload changed");
    }
    for (std::size_t i = 0; i < before.size(); ++i) Require(before[i] == after[i] || editable.contains(i), "Non-CodePage bytes changed");
    if (changed) Publish(image, after);
    std::cout << "Resource metadata: " << expected.size() << " entries verified, " << changed << " CodePage fields updated.\n";
}
}
int wmain(int argc, wchar_t** argv) {
    try {
        fs::path root, image, table;
        int first = 1;
        for (; first < argc && std::wstring(argv[first]) != L"--"; first += 2) {
            Require(first + 1 < argc, "Missing option value");
            const std::wstring key = argv[first];
            fs::path* value = key == L"--root" ? &root : key == L"--image" ? &image : key == L"--table" ? &table : nullptr;
            Require(value && value->empty(), "Unknown or duplicate option");
            *value = argv[first + 1];
        }
        Require(!root.empty() && !image.empty() && !table.empty() && first + 1 < argc, "Expected --root, --image, --table and -- linker command");
        // 链接失败保留其退出码；不能继续修资源，也不能发布旧的成功输出。
        const auto result = RunLinker(argc, argv, first + 1);
        if (result) return static_cast<int>(result);
        Apply(root, image, table);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Resource metadata: " << error.what() << '\n';
        return 1;
    }
}

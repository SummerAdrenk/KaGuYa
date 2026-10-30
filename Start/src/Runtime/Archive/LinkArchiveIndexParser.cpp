#include <Runtime/LinkArchiveIndexParser.h>
#include <Runtime/IFileInfoProgress.h>
#include <Runtime/ArchiveDirectoryRuntime.h>
#include <Runtime/ArchiveResourceNameRuntime.h>
#include <Runtime/LinkArchiveNativeInput.h>
#include <Runtime/ArchiveTitleScannerRuntime.h>

#include <Windows.h>
#include <mbstring.h>

#include <algorithm>
#include <cstring>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Byte = unsigned char;

class Reader
{
public:
    virtual ~Reader() = default;

    [[nodiscard]] virtual std::uint64_t Position() const noexcept = 0;
    [[nodiscard]] virtual std::uint64_t Size() const noexcept = 0;
    virtual void ReadExact(void* output, std::size_t size) = 0;
    virtual void Seek(std::uint64_t position) = 0;
    virtual void SkipUnchecked(const std::uint32_t distance)
    {
        Seek(Position() + distance);
    }

    [[nodiscard]] std::vector<std::byte> Read(
        const std::size_t size, const bool readEmpty = false)
    {
        std::vector<std::byte> result(size);
        if (size != 0u || readEmpty)
            ReadExact(result.data(), size);
        return result;
    }

    [[nodiscard]] std::uint8_t ReadU8()
    {
        std::uint8_t value = 0;
        ReadExact(&value, sizeof(value));
        return value;
    }

    [[nodiscard]] std::uint16_t ReadU16()
    {
        const auto value = Read(sizeof(std::uint16_t));
        return static_cast<std::uint16_t>(
            std::to_integer<std::uint8_t>(value[0]) |
            (std::to_integer<std::uint8_t>(value[1]) << 8u));
    }

    [[nodiscard]] std::uint32_t ReadU32()
    {
        const auto value = Read(sizeof(std::uint32_t));
        return std::to_integer<std::uint8_t>(value[0]) |
            (std::to_integer<std::uint8_t>(value[1]) << 8u) |
            (std::to_integer<std::uint8_t>(value[2]) << 16u) |
            (std::to_integer<std::uint8_t>(value[3]) << 24u);
    }
};

class SpanReader final : public Reader
{
public:
    explicit SpanReader(const std::span<const std::byte> bytes) : bytes_(bytes) {}

    [[nodiscard]] std::uint64_t Position() const noexcept override
    {
        return position_;
    }

    [[nodiscard]] std::uint64_t Size() const noexcept override
    {
        return bytes_.size();
    }

    void ReadExact(void* const output, const std::size_t size) override
    {
        if (position_ > bytes_.size() || size > bytes_.size() - position_)
            throw LinkArchiveFormatError(position_, "truncated LINK data");
        std::memcpy(output, bytes_.data() + position_, size);
        position_ += size;
    }

    void Seek(const std::uint64_t position) override
    {
        if (position > bytes_.size())
            throw LinkArchiveFormatError(position_, "LINK seek extends past EOF");
        position_ = static_cast<std::size_t>(position);
    }

private:
    std::span<const std::byte> bytes_;
    std::size_t position_ = 0;
};

class FileReader final : public Reader
{
public:
    FileReader(
        IStartFileApi& api,
        const HANDLE file,
        const std::uint64_t size,
        const bool runtime = false) noexcept
        : api_(api), file_(file), size_(size), runtime_(runtime)
    {
    }

    [[nodiscard]] std::uint64_t Position() const noexcept override
    {
        return position_;
    }

    [[nodiscard]] std::uint64_t Size() const noexcept override
    {
        return size_;
    }

    void ReadExact(void* const output, const std::size_t size) override
    {
        if ((!runtime_ && (position_ > size_ || size > size_ - position_)) ||
            size > std::numeric_limits<DWORD>::max())
        {
            throw LinkArchiveFormatError(position_, "truncated LINK data");
        }

        DWORD bytesRead = 0;
        const DWORD requested = static_cast<DWORD>(size);
        const BOOL result = api_.Read(file_, output, requested, &bytesRead);
        // 431418..43143F：生产FALSE分支只查LastError，TRUE才比较长度。
        // 自有流的游标按实际读取推进；工具保留严格BOOL/长度检查。
        if (runtime_) position_ += bytesRead;
        const bool succeeded = result != FALSE ? bytesRead == requested :
            (runtime_ && api_.LastError() == ERROR_SUCCESS);
        if (!succeeded)
        {
            throw LinkArchiveFormatError(position_, "LINK file read failed");
        }
        if (!runtime_) position_ += size;
    }

    void Seek(const std::uint64_t position) override
    {
        if (!runtime_ && position > size_)
            throw LinkArchiveFormatError(position, "LINK seek extends past EOF");
        if (api_.SeekAbsolute(file_, position) == FALSE)
            throw LinkArchiveFormatError(position, "LINK file seek failed");
        position_ = position;
    }

    void SkipUnchecked(const std::uint32_t distance) override
    {
        // 4863D5/48642E忽略相对seek的BOOL；现有API只有绝对定位，失败不推进位置。
        const std::uint64_t target = position_ + distance;
        if (api_.SeekAbsolute(file_, target) != FALSE)
            position_ = target;
    }

private:
    IStartFileApi& api_;
    HANDLE file_ = INVALID_HANDLE_VALUE;
    std::uint64_t size_ = 0;
    std::uint64_t position_ = 0;
    bool runtime_ = false;
};

class FileHandle final
{
public:
    FileHandle(IStartFileApi& api, const HANDLE handle) noexcept
        : api_(api), handle_(handle)
    {
    }

    ~FileHandle()
    {
        if (handle_ != INVALID_HANDLE_VALUE)
            api_.Close(handle_);
    }

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

private:
    IStartFileApi& api_;
    HANDLE handle_ = INVALID_HANDLE_VALUE;
};

[[nodiscard]] std::string BytesToString(const std::span<const std::byte> bytes)
{
    if (bytes.empty())
        return {};
    const char* const data = reinterpret_cast<const char*>(bytes.data());
    const auto terminator = std::find(bytes.begin(), bytes.end(), std::byte{0});
    const auto length = static_cast<std::size_t>(terminator - bytes.begin());
    return std::string(data, length);
}

[[nodiscard]] std::string Utf16NameToThreadCodePage(
    const std::span<const std::byte> bytes,
    const std::uint64_t errorOffset)
{
    // 原函数为 nameLength 字节后补两个零字节，再以 CP_THREAD_ACP 转换零结尾 WCHAR。
    std::vector<wchar_t> wide((bytes.size() + 1u) / 2u + 1u, L'\0');
    if (!bytes.empty())
        std::memcpy(wide.data(), bytes.data(), bytes.size());

    const int required = WideCharToMultiByte(
        CP_THREAD_ACP, 0, wide.data(), -1, nullptr, 0, nullptr, nullptr);
    if (required <= 0)
        throw LinkArchiveFormatError(errorOffset, "LINK6 file name conversion failed");

    std::vector<char> converted(static_cast<std::size_t>(required), '\0');
    if (WideCharToMultiByte(
            CP_THREAD_ACP,
            0,
            wide.data(),
            -1,
            converted.data(),
            required,
            nullptr,
            nullptr) <= 0)
    {
        throw LinkArchiveFormatError(errorOffset, "LINK6 file name conversion failed");
    }
    return std::string(converted.data(), static_cast<std::size_t>(required - 1));
}

[[nodiscard]] std::string ExtractTitle(const std::string& name)
{
    const auto* const bytes = reinterpret_cast<const Byte*>(name.c_str());
    const Byte* const dot = _mbsrchr(bytes, '.');
    if (dot == nullptr)
        return {};

    const std::size_t offset = static_cast<std::size_t>(dot - bytes);
    if (offset + 1u >= name.size())
        return {};
    return name.substr(0, offset);
}

void ReserveChildOrderSlot(std::vector<ResourceChildOrderEntry>& order)
{
    if (order.size() != order.capacity())
        return;
    if (order.size() == order.max_size())
        throw std::length_error("LINK child order is too large");
    const std::size_t growth = std::min(order.max_size() - order.size(),
        std::max(order.size(), std::size_t{1}));
    order.reserve(order.size() + growth);
}

[[nodiscard]] ResourceFolderEntry& FindOrAppendFolder(
    ResourceFolderEntry& parent,
    const std::string& name)
{
    for (ResourceFolderEntry& folder : parent.folders)
    {
        if (_mbscmp(
                reinterpret_cast<const Byte*>(folder.name.c_str()),
                reinterpret_cast<const Byte*>(name.c_str())) == 0)
        {
            return folder;
        }
    }
    const std::size_t index = parent.folders.size();
    // 两个现代vector共同表达原单个Ref追加，先备好顺序槽，避免发布孤立节点。
    ReserveChildOrderSlot(parent.childOrder);
    parent.folders.push_back(ResourceFolderEntry{name, {}, {}, {}});
    parent.childOrder.push_back({ResourceChildKind::Folder, index});
    return parent.folders.back();
}

void AppendFile(ResourceFolderEntry& folder, ResourceFileEntry file)
{
    const std::size_t index = folder.files.size();
    ReserveChildOrderSlot(folder.childOrder);
    folder.files.push_back(std::move(file));
    folder.childOrder.push_back({ResourceChildKind::File, index});
}

void InsertFile(ResourceFolderEntry& root, std::string fullName, ResourceFileEntry file)
{
    ResourceFolderEntry* folder = &root;
    for (;;)
    {
        const Byte* const separator = _mbschr(
            reinterpret_cast<const Byte*>(fullName.c_str()), '\\');
        if (separator == nullptr)
            break;

        const std::size_t offset = static_cast<std::size_t>(
            separator - reinterpret_cast<const Byte*>(fullName.c_str()));
        folder = &FindOrAppendFolder(*folder, fullName.substr(0, offset));
        fullName.erase(0, offset + 1u);
    }

    file.name = std::move(fullName);
    file.title = ExtractTitle(file.name);
    AppendFile(*folder, std::move(file));
}

void InsertDeferredFile(ResourceFolderEntry& root, ResourceFileEntry& file)
{
    ResourceFolderEntry* folder = &root;
    for (;;)
    {
        const auto* const name = reinterpret_cast<const Byte*>(file.name.c_str());
        const Byte* const separator = _mbschr(name, '\\');
        if (separator == nullptr)
            break;
        const std::size_t offset = static_cast<std::size_t>(separator - name);
        folder = &FindOrAppendFolder(*folder, file.name.substr(0, offset));
        // 493893..49397B：先发布目录，再修改同一fileinfo的名字，title不重算。
        file.name.erase(0, offset + 1u);
    }
    // 原局部Ref与树共享对象；值模型在叶节点发布时复制，局部记录随后销毁。
    AppendFile(*folder, file);
}

[[nodiscard]] bool IsMagic(const std::string& magic, const char* expected) noexcept
{
    return magic == expected;
}

LinkArchiveHeaderMetadata ReadLinkArchiveHeader(Reader& reader, const bool runtime)
{
    // 493BDC只在486480完整读头成功后输出magic；失败不覆盖此前的header。
    LinkArchiveHeaderMetadata header;
    const auto magicBytes = reader.Read(5);
    header.magic = BytesToString(magicBytes);
    const bool link3 = IsMagic(header.magic, "LINK3");
    const bool link4 = IsMagic(header.magic, "LINK4");
    const bool link5 = IsMagic(header.magic, "LINK5");
    const bool link6 = IsMagic(header.magic, "LINK6");
    if (!runtime && !link3 && !link4 && !link5 && !link6)
    {
        throw LinkArchiveFormatError(0, "unsupported LINK archive magic");
    }

    // 47FE87..47FEEC的outer版本比较不区分大小写，scanner布局比较则区分。
    header.version = std::numeric_limits<std::uint32_t>::max();
    const char* const versions[] = {"LINK3", "LINK4", "LINK5", "LINK6"};
    for (std::uint32_t i = 0; i < 4u; ++i)
    {
        if (_mbsicmp(reinterpret_cast<const Byte*>(header.magic.c_str()),
                reinterpret_cast<const Byte*>(versions[i])) == 0)
        {
            header.version = i + 3u;
            break;
        }
    }
    if (link6)
    {
        header.flags = reader.ReadU16();
        const std::uint8_t nameLength = reader.ReadU8();
        const auto nameBytes = reader.Read(nameLength);
        header.embeddedArchiveName = BytesToString(nameBytes);
    }
    else
    {
        const auto nameBytes = reader.Read(3);
        header.embeddedArchiveName = BytesToString(nameBytes);
        if (link4 || link5)
            header.flags = reader.ReadU16();
    }
    header.headerSize = reader.Position();
    return header;
}

struct ReadLinkEntry final
{
    std::string name;
    std::uint32_t headerSize;
    std::uint32_t dataLength;
    std::uint64_t dataOffset;
    std::uint16_t sourceFlags;
};

std::optional<ReadLinkEntry> ReadLinkArchiveEntry(Reader& reader, bool link6, bool runtime)
{
    const std::uint64_t entryOffset = reader.Position();
    const std::uint32_t chunkSize = reader.ReadU32();
    if (chunkSize == 0u) return std::nullopt;
    if (!runtime && chunkSize < 4u)
        throw LinkArchiveFormatError(entryOffset, "LINK chunk size is smaller than its size field");
    const std::uint64_t chunkEnd = entryOffset + chunkSize;
    if (!runtime && (chunkEnd < entryOffset || chunkEnd > reader.Size()))
        throw LinkArchiveFormatError(entryOffset, "LINK chunk extends past EOF");

    const std::uint16_t sourceFlags = reader.ReadU16();
    static_cast<void>(reader.ReadU16()); // 年
    static_cast<void>(reader.ReadU8());  // 月
    static_cast<void>(reader.ReadU8());  // 日
    static_cast<void>(reader.ReadU8());  // 时
    static_cast<void>(reader.ReadU8());  // 分
    static_cast<void>(reader.ReadU8());  // 秒
    std::string fullName;
    std::uint32_t entryHeaderSize;
    if (link6)
    {
        const std::uint16_t nameByteLength = reader.ReadU16();
        const std::uint64_t nameOffset = reader.Position();
        const auto nameBytes = reader.Read(nameByteLength, runtime);
        fullName = Utf16NameToThreadCodePage(nameBytes, nameOffset);
        entryHeaderSize = 15u + nameByteLength;
    }
    else
    {
        const std::uint8_t nameByteLength = reader.ReadU8();
        if (runtime) reader.SkipUnchecked(2u);
        else static_cast<void>(reader.Read(2));
        const auto nameBytes = reader.Read(nameByteLength, runtime);
        fullName = BytesToString(nameBytes);
        entryHeaderSize = 16u + nameByteLength;
    }
    if (!runtime && reader.Position() > chunkEnd)
        throw LinkArchiveFormatError(entryOffset, "LINK entry header exceeds chunk size");
    const std::uint64_t dataLength = runtime
        ? static_cast<std::uint32_t>(chunkSize - entryHeaderSize) : chunkEnd - reader.Position();
    if (dataLength > std::numeric_limits<std::uint32_t>::max())
        throw LinkArchiveFormatError(entryOffset, "LINK entry data length exceeds uint32");
    const auto dataOffset = reader.Position();
    // 486220 在文件节点分配之前跳过正文；不能将这次 I/O 推迟到插表之后。
    if (runtime) reader.SkipUnchecked(static_cast<std::uint32_t>(dataLength));
    else reader.Seek(chunkEnd);
    return ReadLinkEntry{std::move(fullName), entryHeaderSize,
        static_cast<std::uint32_t>(dataLength), dataOffset, sourceFlags};
}

void ParseLinkArchiveIndexCoreInto(
    Reader& reader, LinkArchiveIndex& result, IFileInfoProgress* progress, const bool runtime)
{
    result.header = ReadLinkArchiveHeader(reader, runtime);
    const bool link3 = IsMagic(result.header.magic, "LINK3");
    const bool link4 = IsMagic(result.header.magic, "LINK4");
    const bool link5 = IsMagic(result.header.magic, "LINK5");
    const bool link6 = IsMagic(result.header.magic, "LINK6");
    // 493C28使用已截断到NUL的内嵌名长度，不用实际流位置计算fileinfo offset。
    std::uint64_t logicalOffset = link6 ? result.header.embeddedArchiveName.size() + 8u
        : (link4 || link5 ? 10u : 8u);

    // 493C6B..493C79为栈上Ref容器，成功清理或异常展开时均销毁，不跨调用保存。
    std::vector<ResourceFileEntry> pathFiles;
    for (;;)
    {
        // 493E00..493E35：终止项同样先检查取消，不在读到零以后补检查。
        if (progress != nullptr && progress->PollCancelAt47F0B0()) throw 5;
        const auto entry = ReadLinkArchiveEntry(reader, link6, runtime);
        if (!entry) break;
        const auto& fullName = entry->name;

        ResourceFileEntry file;
        logicalOffset += entry->headerSize;
        file.offset = runtime ? logicalOffset : entry->dataOffset;
        file.length = entry->dataLength;
        // LINK3 只保留“是否压缩”，原 reader 将任意非零值归一化为 2。
        file.flags = link3
            ? static_cast<std::uint16_t>(entry->sourceFlags == 0u ? 0u : 2u)
            : entry->sourceFlags;
        file.name = fullName;
        // 493F7E先取文件名再取title；尾部反斜杠不触发45D950的取尾段分支。
        const Byte* const lastSeparator = _mbsrchr(
            reinterpret_cast<const Byte*>(fullName.c_str()), '\\');
        const std::size_t titleStart = lastSeparator == nullptr ? 0u
            : static_cast<std::size_t>(lastSeparator -
                reinterpret_cast<const Byte*>(fullName.c_str())) + 1u;
        file.title = ExtractTitle(titleStart < fullName.size()
            ? fullName.substr(titleStart) : fullName);
        if (_mbschr(reinterpret_cast<const Byte*>(fullName.c_str()), '\\') != nullptr)
            pathFiles.push_back(std::move(file));
        else
            AppendFile(result.archive.root, std::move(file));

        logicalOffset += entry->dataLength;
        ++result.fileCount;
        result.maximumDataLength = std::max(
            result.maximumDataLength, entry->dataLength);
        if (progress != nullptr) progress->StepAt46E820();
    }

    // 493CD4..493CDB ->4936C0：路径文件的第二阶段不触发progress。
    for (auto& file : pathFiles)
    {
        if (runtime)
            InsertDeferredFile(result.archive.root, file);
        else
            InsertFile(result.archive.root, file.name, file);
    }
}

struct ArchiveLocalReference final
{
    YtlReferenceSlotRuntime value;
    bool unwind = true;
    ~ArchiveLocalReference() noexcept { if (unwind) ReleaseArchiveReferenceOnUnwind(value); }
    void CompleteNormal(YtlReferenceControlRuntime* control) noexcept
    {
        unwind = false;
        ReleaseArchiveLocalReference(control);
    }
    void CompleteDirect(YtlReferenceControlRuntime* control) noexcept
    {
        unwind = false;
        ReleaseArchiveDirectLocalReference(value, control);
    }
};

ArchiveNodeRuntime& RequireNode(const YtlReferenceSlotRuntime& reference)
{
    auto* node = ResolveArchiveNode(reference);
    if (!node) throw std::logic_error("LINK fileinfo has no node payload (safe boundary)");
    return *node;
}

void InsertNativeDeferredFile(ArchiveDirectoryRuntime& parent, YtlReferenceSlotRuntime& reference)
{
    auto& name = RequireNode(reference).NameAt04();
    const auto* bytes = reinterpret_cast<const Byte*>(name.GetString());
    const auto* separator = name.GetLength() > 0 ? _mbschr(bytes, '\\') : nullptr;
    if (!separator)
    {
        parent.Append(reference);
        return;
    }
    const auto split = static_cast<int>(separator - bytes);
    // 493764 ->419D00：零长度前缀也走指定长度的CString构造，不用std::string影子节点。
    auto prefix=ArchiveResourceNameLeftAt419D00(RequireNode(reference).NameAt04(),split);
    const char* const prefixCharacters=prefix.Value().GetString(); // 49376C跨整个递归阶段保留EDI。
    ArchiveFolderNodeRuntime* destination = nullptr;
    for (auto* slot = parent.beginAt00; slot != parent.endAt04; ++slot)
    {
        if (RequireNode(*slot).IsFile()) continue;
        if(!prefixCharacters)throw ATL::CAtlException(E_FAIL); // 4937C4仅检查Str2。
        if (_mbscmp(reinterpret_cast<const Byte*>(RequireNode(*slot).NameAt04().GetString()),
                reinterpret_cast<const Byte*>(prefixCharacters)) == 0)
        {
            destination = static_cast<ArchiveFolderNodeRuntime*>(ResolveArchiveNode(*slot));
            break;
        }
    }
    if (!destination)
    {
        auto* body = CreateArchiveFolderBody();
        ArchiveLocalReference folder;
        ReplaceArchiveFolderReference(folder.value, body);
        auto* capturedControl = folder.value.controlAt08;
        AssignRuntimeCStringAt410220(RequireNode(folder.value).NameAt04(),prefix.Value());
        const bool grows = parent.endAt04 == parent.capacityAt08;
        parent.Append(folder.value);
        if (grows) capturedControl = folder.value.controlAt08; // 4938CB 仅增长后重取 esi。
        destination = static_cast<ArchiveFolderNodeRuntime*>(&RequireNode(folder.value));
        folder.CompleteNormal(capturedControl);
    }
    // 493913..49399F：目录发布后重新解析file Ref，再修改同一正文的name；title不变。
    {
        auto& current = RequireNode(reference).NameAt04();
        const int start = split + 1;
        auto suffix=ArchiveResourceNameMidAt420C80(current,start,current.GetLength()-start);
        AssignRuntimeCStringAt410220(RequireNode(reference).NameAt04(),suffix.Value());
        suffix.ReleaseNormal(); // 493980：赋值之后读取临时槽。
    }
    InsertNativeDeferredFile(destination->ChildrenAt08(), reference);
    prefix.ReleaseNormalCached(prefixCharacters); // 4939AC先撤资格，正常Free抛出不重试。
}

void ScanNativeDirectory(LinkArchiveNativeInput& reader,RuntimeCString& outerMagic,ArchiveDirectoryRuntime& directory,
    std::uint32_t& count, std::uint32_t& maximum, IFileInfoProgress& progress)
{
    LinkArchiveNativeHeader header;
    if(!header.ReadAt486480(reader))throw 3;
    AssignRuntimeCStringAt410220(outerMagic,header.Magic()); // 493BDC：扫描前即发布magic。
    const char* const magic=header.Magic().GetString();
    const auto equals=[&](const char* text){return _mbscmp(reinterpret_cast<const Byte*>(magic),reinterpret_cast<const Byte*>(text))==0;};
    const bool link6=equals("LINK6");
    ArchiveRawResourceNameRuntime streamNameOwner(ArchiveRawResourceNameRuntime::ProducerTag{},
        [](RuntimeCString* output){InitializeEmptyRuntimeCString(*output);});
    auto& streamName=streamNameOwner.Value();
    LinkArchiveNativeEntryFields entry;
    std::uint64_t logicalOffset=equals("LINK6")?static_cast<unsigned>(header.EmbeddedName().GetLength())+8u
        :(equals("LINK4")||equals("LINK5")?10u:8u);
    // 493CE5..493CED：路径 Ref 容器必须先于最后一个仍共享名称的流 CString 析构。
    ArchiveDirectoryRuntime pathFiles;
    const bool link3=equals("LINK3");
    for (;;)
    {
        if (progress.PollCancelAt47F0B0()) throw 5;
        const auto headerSize = ReadLinkArchiveNativeEntryAt486220(reader, link6, streamName, entry);
        if (headerSize == 0) break;
        auto* body = CreateArchiveFileBody();
        ArchiveLocalReference local;
        ReplaceArchiveFileReference(local.value, body);
        auto* capturedControl = local.value.controlAt08;
        auto& file = static_cast<ArchiveFileNodeRuntime&>(RequireNode(local.value));
        AssignRuntimeCStringAt410220(file.NameAt04(),streamName); // 493F08..493F61：同管理器共享源CString。
        {
            // 493F67 在 title scanner 构造前捕获源地址，来源仍是流名称而非 file.Name。
            const char* fullName = streamName.GetString();
            ArchiveRawResourceNameRuntime title("");
            ScanLinkTitleAt493F67(title.Value(),fullName);
            const char* characters=title.Value().GetString();
            SetRuntimeCStringAt437680(file.TitleAt08(),characters,characters?static_cast<int>(std::strlen(characters)):0);
            title.ReleaseNormal();
        } // 4940A5..4940CB：title 临时先释放，随后才写 offset/length/flags。
        logicalOffset += headerSize;
        file.offsetAt10 = logicalOffset;
        file.lengthAt18 = entry.dataLength;
        file.flagsAt1C = link3 ? (entry.sourceFlags ? 2u : 0u) : entry.sourceFlags;
        auto& target = _mbsrchr(reinterpret_cast<const Byte*>(file.NameAt04().GetString()), '\\')
            ? pathFiles : directory;
        const bool grows = target.endAt04 == target.capacityAt08;
        target.Append(local.value);
        if (grows) capturedControl = local.value.controlAt08; // 494160 仅增长后重取 edi。
        // 49417D..4941C9：append之后从实际节点重读长度，不沿用条目临时值。
        logicalOffset += static_cast<ArchiveFileNodeRuntime&>(RequireNode(local.value)).lengthAt18;
        ++count;
        const auto length = static_cast<ArchiveFileNodeRuntime&>(RequireNode(local.value)).lengthAt18;
        if (length > maximum)
            maximum = static_cast<ArchiveFileNodeRuntime&>(RequireNode(local.value)).lengthAt18;
        local.CompleteDirect(capturedControl); // 4941CC早于Step，正常释放不重读control槽。
        progress.StepAt46E820();
    }
    auto* end = pathFiles.endAt04; // 493CB4/493CD0：第二阶段使用扫描结束时捕获的 end。
    for (auto* reference = pathFiles.beginAt00; reference != end; ++reference)
        InsertNativeDeferredFile(directory, *reference);
    pathFiles.DestroyStorage(); // 493CE8：在stream/header正常退役之前释放临时路径Ref。
    streamNameOwner.ReleaseNormal();
    header.ReleaseNormal();
}
}

LinkArchiveFormatError::LinkArchiveFormatError(
    const std::uint64_t offset,
    const char* const message)
    : std::runtime_error(message), offset_(offset)
{
}

std::uint64_t LinkArchiveFormatError::Offset() const noexcept
{
    return offset_;
}

LinkArchiveIndex ParseLinkArchiveIndex(
    const std::span<const std::byte> bytes,
    std::string nameSpace,
    std::string archivePath)
{
    SpanReader reader(bytes);
    LinkArchiveIndex result;
    result.archive.nameSpace = std::move(nameSpace);
    result.archive.archivePath = std::move(archivePath);
    ParseLinkArchiveIndexCoreInto(reader, result, nullptr, false);
    result.archive.archiveVersion = result.header.version;
    return result;
}

static LinkArchiveIndex ParseLinkArchiveIndexFileCore(
    const char* const archivePath,
    std::string nameSpace,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api,
    IFileInfoProgress* progress)
{
    SECURITY_ATTRIBUTES securityAttributes{};
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.bInheritHandle = TRUE;
    FileOpenRequest request{
        archivePath,
        GENERIC_READ,
        FILE_SHARE_READ, // 47FA10 的直接索引读取与 finfo 读取均允许共享读。
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        &securityAttributes};
    const HANDLE file = OpenFileUsingStartPolicy(request, transactionContext, api);
    if (file == INVALID_HANDLE_VALUE)
        throw LinkArchiveFormatError(0, "LINK archive open failed");
    const FileHandle owner(api, file);

    DWORD sizeHigh = 0;
    const DWORD sizeLow = api.FileSizeLow(file, &sizeHigh);
    if (sizeLow == INVALID_FILE_SIZE && api.LastError() != ERROR_SUCCESS)
        throw LinkArchiveFormatError(0, "LINK archive size query failed");
    const std::uint64_t fileSize =
        (static_cast<std::uint64_t>(sizeHigh) << 32u) | sizeLow;

    FileReader reader(api, file, fileSize);
    LinkArchiveIndex result;
    result.archive.nameSpace = std::move(nameSpace);
    result.archive.archivePath = archivePath;
    ParseLinkArchiveIndexCoreInto(reader, result, progress, false);
    result.archive.archiveVersion = result.header.version;
    return result;
}

LinkArchiveIndex ParseLinkArchiveIndexFile(const char* archivePath, std::string nameSpace,
    const FileTransactionContext* transaction, IStartFileApi& api)
{
    return ParseLinkArchiveIndexFileCore(archivePath, std::move(nameSpace), transaction, api, nullptr);
}

LinkArchiveIndex ParseLinkArchiveIndexFile(const char* archivePath, std::string nameSpace,
    const FileTransactionContext* transaction, IStartFileApi& api, IFileInfoProgress& progress)
{
    return ParseLinkArchiveIndexFileCore(archivePath, std::move(nameSpace), transaction, api, &progress);
}

void LoadRuntimeLinkArchiveIndexFileInto(
    const char* const archivePath,
    std::string nameSpace,
    const FileTransactionContext* const transactionContext,
    IStartFileApi& api,
    LinkArchiveIndex& index,
    IFileInfoProgress* const progress,
    ILinkArchiveIndexCompletion* const completion)
{
    SECURITY_ATTRIBUTES securityAttributes{};
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.bInheritHandle = TRUE;
    const FileOpenRequest request{
        archivePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, &securityAttributes};
    const HANDLE file = OpenFileUsingStartPolicy(request, transactionContext, api);
    if (file == INVALID_HANDLE_VALUE)
        throw 1;
    const FileHandle owner(api, file);

    // 47FE60/47FE6A：仅成功打开后复位，不清空旧树。
    index.maximumDataLength = 0;
    index.fileCount = 0;
    bool scanned = false;
    try
    {
        // 原scanner不查询FileSize；保留按实际Read结果失败的边界。
        FileReader reader(api, file, 0, true);
        reader.Seek(0); // 493B81：起始定位失败由scanner转false。
        index.archive.nameSpace = std::move(nameSpace);
        index.archive.archivePath = archivePath;
        ParseLinkArchiveIndexCoreInto(reader, index, progress, true);
        scanned = true;
    }
    catch (...)
    {
        // 493D5D/493D63：不回滚已发布树、header和计数；局部路径记录已随展开销毁。
    }

    // 47FE78 -> 47FFF5：scanner返回失败后先抛int5，再展开关闭流。
    if (!scanned) throw 5;

    // 47FE93..47FEEF先提交版本/loaded，47FF14才析构流；回调不属于scanner catch。
    index.archive.archiveVersion = index.header.version;
    if (completion != nullptr)
        completion->OnCompleted(index);
}

void LoadRuntimeLinkArchiveDirectoryFileInto(const char* path,
    const FileTransactionContext* transaction, IStartFileApi& files,
    ArchiveDirectoryRuntime& directory, std::uint32_t& count, std::uint32_t& maximum,
    std::uint32_t& version, std::uint32_t& loaded, IFileInfoProgress& progress)
{
    SECURITY_ATTRIBUTES security{};
    security.nLength = sizeof(security);
    security.bInheritHandle = TRUE;
    const FileOpenRequest request{path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, &security};
    ScopedFileStreamRuntime owner(files);
    if(!owner.OpenReaderWithApi(request,transaction))throw 1;
    ArchiveRawResourceNameRuntime magic(ArchiveRawResourceNameRuntime::ProducerTag{},
        [](RuntimeCString* output){InitializeEmptyRuntimeCString(*output);});
    maximum = 0;
    count = 0;
    bool scanned=false;
    try
    {
        LinkArchiveNativeInput reader(owner.Native(),files);
        reader.Begin();
        ScanNativeDirectory(reader,magic.Value(),directory,count,maximum,progress);
        scanned=true;
    }
    catch (...) {} // 493D5D：保留已发布节点及计数，47FFF5统一抛int5。
    if(!scanned)throw 5;
    const char* const source=magic.Value().GetString();
    const char* const versions[]={"LINK3","LINK4","LINK5","LINK6"};
    std::uint32_t selectedVersion=0xFFFFFFFFu;
    for(unsigned i=0;i<4;++i)
        if(_mbsicmp(reinterpret_cast<const Byte*>(source),reinterpret_cast<const Byte*>(versions[i]))==0)
        {selectedVersion=i+3;break;}
    version=selectedVersion;
    loaded = 1; // 47FEEF写入后才在47FF14关闭流。
    magic.ReleaseNormalCached(source);
}
}

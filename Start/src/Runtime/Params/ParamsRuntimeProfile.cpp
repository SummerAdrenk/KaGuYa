#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/ParamsDemoCommandRuntime.h>

#include <Windows.h>

#include <algorithm>
#include <bit>
#include <cstring>
#include <limits>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
namespace
{
constexpr std::string_view V58Header = "[SCR-PARAMS]v05.8";

[[noreturn]] void ThrowParams(
    const ParamsRuntimeError error,
    const std::size_t offset,
    const char* const message)
{
    throw ParamsRuntimeException(error, offset, message);
}

class Cursor final
{
public:
    explicit Cursor(const std::span<const std::byte> bytes) noexcept : bytes_(bytes) {}

    [[nodiscard]] std::size_t Offset() const noexcept { return offset_; }
    [[nodiscard]] std::size_t Remaining() const noexcept { return bytes_.size() - offset_; }
    [[nodiscard]] bool End() const noexcept { return offset_ == bytes_.size(); }

    [[nodiscard]] std::uint8_t U8()
    {
        Require(1);
        return std::to_integer<std::uint8_t>(bytes_[offset_++]);
    }

    [[nodiscard]] std::uint16_t U16()
    {
        Require(2);
        const auto* const input = reinterpret_cast<const std::uint8_t*>(bytes_.data() + offset_);
        const std::uint16_t value = static_cast<std::uint16_t>(input[0]) |
            static_cast<std::uint16_t>(input[1] << 8u);
        offset_ += 2;
        return value;
    }

    [[nodiscard]] std::uint32_t U32()
    {
        Require(4);
        const auto* const input = reinterpret_cast<const std::uint8_t*>(bytes_.data() + offset_);
        const std::uint32_t value = static_cast<std::uint32_t>(input[0]) |
            (static_cast<std::uint32_t>(input[1]) << 8u) |
            (static_cast<std::uint32_t>(input[2]) << 16u) |
            (static_cast<std::uint32_t>(input[3]) << 24u);
        offset_ += 4;
        return value;
    }

    [[nodiscard]] std::vector<std::byte> Bytes(const std::size_t count)
    {
        Require(count);
        std::vector<std::byte> result(
            bytes_.begin() + static_cast<std::ptrdiff_t>(offset_),
            bytes_.begin() + static_cast<std::ptrdiff_t>(offset_ + count));
        offset_ += count;
        return result;
    }

    [[nodiscard]] std::string Ascii(const std::size_t count)
    {
        const std::vector<std::byte> bytes = Bytes(count);
        return {reinterpret_cast<const char*>(bytes.data()), bytes.size()};
    }

    [[nodiscard]] ParamsText Text16()
    {
        return Text(U16());
    }

    [[nodiscard]] ParamsText ShortText16()
    {
        return Text(U8());
    }

    [[nodiscard]] ParamsText TypedText()
    {
        const std::size_t typeOffset = offset_;
        if (U32() != 0)
        {
            ThrowParams(ParamsRuntimeError::InvalidTypedValue, typeOffset,
                "params typed value is not a string");
        }
        return Text16();
    }

    [[nodiscard]] std::uint32_t TypedInteger()
    {
        const std::size_t typeOffset = offset_;
        if (U32() != 1)
        {
            ThrowParams(ParamsRuntimeError::InvalidTypedValue, typeOffset,
                "params typed value is not an integer");
        }
        return U32();
    }

    [[nodiscard]] std::array<std::uint32_t, 2> TypedPoint()
    {
        const std::size_t typeOffset = offset_;
        if (U32() != 2)
        {
            ThrowParams(ParamsRuntimeError::InvalidTypedValue, typeOffset,
                "params typed value is not a point");
        }
        return {U32(), U32()};
    }

    void RequireCount(const std::uint32_t count, const std::size_t minimumBytesPerItem)
    {
        if (minimumBytesPerItem != 0 &&
            count > Remaining() / minimumBytesPerItem)
        {
            ThrowParams(ParamsRuntimeError::TruncatedInput, offset_,
                "params item count exceeds remaining input");
        }
    }

private:
    void Require(const std::size_t count)
    {
        if (count > Remaining())
        {
            ThrowParams(ParamsRuntimeError::TruncatedInput, offset_,
                "params input is truncated");
        }
    }

    [[nodiscard]] ParamsText Text(const std::size_t byteCount)
    {
        const std::size_t textOffset = offset_;
        if ((byteCount & 1u) != 0)
        {
            ThrowParams(ParamsRuntimeError::InvalidUtf16, textOffset,
                "params UTF-16 byte length is odd");
        }
        Require(byteCount);

        ParamsText result;
        result.utf16.resize(byteCount / 2u);
        if (byteCount != 0)
        {
            std::memcpy(result.utf16.data(), bytes_.data() + offset_, byteCount);
        }
        offset_ += byteCount;

        if (result.utf16.empty())
        {
            return result;
        }
        if (result.utf16.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        {
            ThrowParams(ParamsRuntimeError::Cp932ConversionFailed, textOffset,
                "params string is too long for CP932 conversion");
        }

        BOOL usedDefault = FALSE;
        const auto* const wide = reinterpret_cast<const wchar_t*>(result.utf16.data());
        const int wideLength = static_cast<int>(result.utf16.size());
        const int required = WideCharToMultiByte(932, WC_NO_BEST_FIT_CHARS, wide, wideLength,
            nullptr, 0, nullptr, &usedDefault);
        if (required <= 0 || usedDefault != FALSE)
        {
            ThrowParams(ParamsRuntimeError::Cp932ConversionFailed, textOffset,
                "params UTF-16 text cannot be represented by CP932");
        }
        result.cp932.resize(static_cast<std::size_t>(required));
        usedDefault = FALSE;
        if (WideCharToMultiByte(932, WC_NO_BEST_FIT_CHARS, wide, wideLength,
                result.cp932.data(), required, nullptr, &usedDefault) != required ||
            usedDefault != FALSE)
        {
            ThrowParams(ParamsRuntimeError::Cp932ConversionFailed, textOffset,
                "params UTF-16 to CP932 conversion failed");
        }
        return result;
    }

    std::span<const std::byte> bytes_;
    std::size_t offset_ = 0;
};

[[nodiscard]] ParamsSettingTag ReadSettingTag(Cursor& cursor)
{
    ParamsSettingTag tag;
    tag.name = cursor.Text16();
    const std::uint32_t pairCount = cursor.U32();
    cursor.RequireCount(pairCount, 4);
    tag.pairs.reserve(pairCount);
    for (std::uint32_t index = 0; index < pairCount; ++index)
    {
        tag.pairs.push_back({cursor.Text16(), cursor.Text16()});
    }

    const std::uint32_t childCount = cursor.U32();
    cursor.RequireCount(childCount, 6);
    tag.children.reserve(childCount);
    for (std::uint32_t index = 0; index < childCount; ++index)
    {
        tag.children.push_back(ReadSettingTag(cursor));
    }
    return tag;
}

DecodedParamsDemoCommand DecodeDemoPayload(
    const ParamsDemoCommand& command, const std::size_t commandOffset)
{
    Cursor payload(command.payload);
    DecodedParamsDemoCommand result;
    switch (command.type)
    {
    case 0:
        result = DemoEndCommand{};
        break;
    case 1:
        result = DemoNextCommand{};
        break;
    case 2:
        result = DemoWaitCommand{payload.U8(), std::bit_cast<std::int32_t>(payload.U32())};
        break;
    case 3:
        result = DemoSoundCommand{payload.U8(), payload.U8(), payload.ShortText16()};
        break;
    case 4:
        result = DemoLoadCommand{payload.U8(), payload.ShortText16()};
        break;
    case 5:
        result = DemoTransitionCommand{payload.ShortText16(),
            std::bit_cast<std::int32_t>(payload.U32()), payload.ShortText16()};
        break;
    case 6:
    {
        const std::uint8_t slot = payload.U8();
        // 41A8AB..41A8C4：这是 NOT byte -> MOVZX -> DEC，不是 int8_t 符号扩展。
        const std::int32_t decodedSlot = slot <= 0x80u ? slot
            : static_cast<std::int32_t>(static_cast<std::uint8_t>(~slot)) - 1;
        result = DemoVisibilityCommand{decodedSlot, payload.U8() != 0};
        break;
    }
    case 7:
        result = DemoUpdateCommand{};
        break;
    case 8:
        result = DemoMoveCommand{payload.U8(),
            std::bit_cast<std::int32_t>(payload.U32()),
            std::bit_cast<std::int32_t>(payload.U32()),
            std::bit_cast<std::int32_t>(payload.U32()),
            std::bit_cast<std::int32_t>(payload.U32()),
            std::bit_cast<std::int32_t>(payload.U32())};
        break;
    case 9:
        result = DemoPositionCommand{payload.U8(),
            std::bit_cast<std::int32_t>(payload.U32()),
            std::bit_cast<std::int32_t>(payload.U32())};
        break;
    default:
        ThrowParams(ParamsRuntimeError::InvalidDemoData, commandOffset,
            "params demo command type is invalid");
    }
    if (!payload.End())
    {
        ThrowParams(ParamsRuntimeError::InvalidDemoData, commandOffset,
            "params demo command payload has trailing bytes");
    }
    return result;
}

[[nodiscard]] ParamsDemo ReadDemo(Cursor& cursor)
{
    ParamsDemo demo;
    demo.name = cursor.Text16();
    const std::size_t magicOffset = cursor.Offset();
    if (cursor.Ascii(9) != "[Demo3.0]")
    {
        ThrowParams(ParamsRuntimeError::InvalidDemoData, magicOffset,
            "params demo magic is invalid");
    }

    const std::uint16_t commandCount = cursor.U16();
    cursor.RequireCount(commandCount, 2);
    demo.commands.reserve(commandCount);
    for (std::uint16_t index = 0; index < commandCount; ++index)
    {
        const std::size_t commandOffset = cursor.Offset();
        ParamsDemoCommand command;
        command.type = cursor.U8();
        const std::uint8_t length = cursor.U8();
        if (length < 2)
        {
            ThrowParams(ParamsRuntimeError::InvalidDemoData, commandOffset,
                "params demo command length is below two");
        }
        command.payload = cursor.Bytes(static_cast<std::size_t>(length) - 2u);
        static_cast<void>(DecodeDemoPayload(command, commandOffset));
        demo.commands.push_back(std::move(command));
    }
    return demo;
}

[[nodiscard]] ParamsGameSystem ReadGameSystem(Cursor& cursor, std::uint32_t& rawTripleCountMetadata)
{
    ParamsGameSystem gameSystem;
    gameSystem.versionMarker = cursor.U16();
    gameSystem.width = cursor.U32();
    gameSystem.height = cursor.U32();
    gameSystem.startupConfigBytes.AssignSnapshot(cursor.Bytes(cursor.U8()));
    gameSystem.gameTitle = cursor.Text16();
    gameSystem.appendTitle = cursor.Text16();
    gameSystem.brand = cursor.Text16();
    gameSystem.defaultNameMode = cursor.U8();
    gameSystem.defaultGivenName = cursor.Text16();
    gameSystem.defaultFamilyName = cursor.Text16();

    const std::uint8_t installCount = cursor.U8();
    std::vector<ParamsInstallEntry> installValues;
    installValues.reserve(installCount);
    for (std::uint8_t index = 0; index < installCount; ++index)
    {
        installValues.push_back({cursor.Text16(), cursor.Text16()});
    }
    // 严格工具入口先完成 UTF-16/CP932 校验，再显式构造运行时双 CString 表。
    gameSystem.installTable.AssignSnapshot(installValues);
    for (std::uint32_t& slot : gameSystem.soundSlotCounts)
    {
        slot = cursor.U32();
    }
    gameSystem.nameSelectionMode = cursor.U8();

    for (std::size_t index = 0; index != gameSystem.settingTags.size(); ++index)
    {
        if (cursor.U8() != 0)
        {
            gameSystem.settingTags[index].AssignSnapshot(ReadSettingTag(cursor));
        }
    }

    rawTripleCountMetadata = cursor.U32();
    const std::uint32_t tripleCount = rawTripleCountMetadata & 0xFFu;
    cursor.RequireCount(tripleCount, 12);
    gameSystem.v53Triples.ResizeAt40ACB0(tripleCount);
    for (std::uint32_t index = 0; index < tripleCount; ++index)
    {
        gameSystem.v53Triples[index] = {cursor.U32(), cursor.U32(), cursor.U32()};
    }

    const std::uint32_t rawBlobLength = cursor.U32();
    gameSystem.rawBlob.AssignSnapshot(cursor.Bytes(rawBlobLength));

    const std::uint8_t demoCount = cursor.U8();
    std::vector<ParamsDemo> demoSnapshot;
    demoSnapshot.reserve(demoCount);
    for (std::uint8_t index = 0; index < demoCount; ++index)
    {
        demoSnapshot.push_back(ReadDemo(cursor));
    }
    // 先完成原 strict DTO 校验，再显式深导入唯一原生存储。
    gameSystem.demos.AssignSnapshot(demoSnapshot);

    const std::uint32_t stringCount = cursor.U32();
    cursor.RequireCount(stringCount, 2);
    // 严格工具parser的现代值仅用于一次性导入，不驻留生产+C4数组。
    std::vector<ParamsText> stringSnapshot;
    stringSnapshot.reserve(stringCount);
    for (std::uint32_t index = 0; index < stringCount; ++index)
    {
        stringSnapshot.push_back(cursor.Text16());
    }
    gameSystem.stringList.AssignSnapshot(stringSnapshot);

    const std::uint32_t placeCount = cursor.U32();
    cursor.RequireCount(placeCount, 6);
    std::vector<ParamsNamedValue> placeSnapshot;
    placeSnapshot.reserve(placeCount);
    for (std::uint32_t index = 0; index < placeCount; ++index)
    {
        placeSnapshot.push_back({cursor.Text16(), cursor.U32()});
    }
    gameSystem.places.AssignSnapshot(placeSnapshot);

    // 41D207..41D3FE：v05.4+ 名称后跟外层数量；每层为 u32 数量和 UTF-16 字符串列。
    gameSystem.v54NestedListName = cursor.Text16();
    const std::uint32_t nestedOuterCount = cursor.U32();
    cursor.RequireCount(nestedOuterCount, 4);
    std::vector<std::vector<ParamsText>> nestedSnapshot;
    nestedSnapshot.reserve(nestedOuterCount);
    for (std::uint32_t outer = 0; outer < nestedOuterCount; ++outer)
    {
        const std::uint32_t nestedStringCount = cursor.U32();
        cursor.RequireCount(nestedStringCount, 2);
        std::vector<ParamsText> strings;
        strings.reserve(nestedStringCount);
        for (std::uint32_t inner = 0; inner < nestedStringCount; ++inner)
        {
            strings.push_back(cursor.Text16());
        }
        nestedSnapshot.push_back(std::move(strings));
    }
    gameSystem.v54NestedLists.AssignSnapshot(nestedSnapshot);

    const std::uint32_t thumbnailUnits = cursor.U32();
    if (thumbnailUnits % 11u != 0)
    {
        ThrowParams(ParamsRuntimeError::InvalidUnitCount, cursor.Offset() - 4u,
            "params thumbnail unit count is not divisible by eleven");
    }
    const std::uint32_t thumbnailCount = thumbnailUnits / 11u;
    cursor.RequireCount(thumbnailCount, 72);
    for (std::uint32_t index = 0; index < thumbnailCount; ++index)
    {
        ParamsThumbnail thumbnail;
        for (ParamsText& text : thumbnail.strings)
        {
            text = cursor.TypedText();
        }
        for (std::uint32_t& integer : thumbnail.integers)
        {
            integer = cursor.TypedInteger();
        }
        gameSystem.thumbnails.push_back(std::move(thumbnail));
    }

    const std::uint32_t sceneNameCount = cursor.U32();
    cursor.RequireCount(sceneNameCount, 6);
    for (std::uint32_t index = 0; index < sceneNameCount; ++index)
    {
        gameSystem.sceneNames.push_back(cursor.TypedText());
    }

    const std::uint32_t cgUnits = cursor.U32();
    std::uint32_t usedCgUnits = 0;
    while (usedCgUnits < cgUnits)
    {
        ParamsRegistCgGroup group;
        group.name = cursor.TypedText();
        const std::uint32_t itemCount = cursor.TypedInteger();
        if (cgUnits - usedCgUnits < 2u)
        {
            ThrowParams(ParamsRuntimeError::InvalidUnitCount, cursor.Offset(),
                "params CG registration unit count underflowed");
        }
        usedCgUnits += 2;
        if (itemCount > (cgUnits - usedCgUnits) / 3u)
        {
            ThrowParams(ParamsRuntimeError::InvalidUnitCount, cursor.Offset(),
                "params CG registration item count exceeds units");
        }
        group.items.reserve(itemCount);
        for (std::uint32_t itemIndex = 0; itemIndex < itemCount; ++itemIndex)
        {
            ParamsRegistCgItem item;
            item.name = cursor.TypedText();
            const auto point = cursor.TypedPoint();
            item.x = point[0];
            item.y = point[1];
            item.value = cursor.TypedInteger();
            group.items.push_back(std::move(item));
            usedCgUnits += 3;
        }
        gameSystem.registCg.push_back(std::move(group));
    }
    if (usedCgUnits != cgUnits)
    {
        ThrowParams(ParamsRuntimeError::InvalidUnitCount, cursor.Offset(),
            "params CG registration units do not match");
    }

    const std::uint32_t sceneUnits = cursor.U32();
    std::uint32_t usedSceneUnits = 0;
    while (usedSceneUnits < sceneUnits)
    {
        ParamsRegistSceneGroup group;
        group.name = cursor.TypedText();
        const std::uint32_t itemCount = cursor.TypedInteger();
        if (sceneUnits - usedSceneUnits < 2u)
        {
            ThrowParams(ParamsRuntimeError::InvalidUnitCount, cursor.Offset(),
                "params scene registration unit count underflowed");
        }
        usedSceneUnits += 2;
        group.items.reserve(itemCount);
        for (std::uint32_t itemIndex = 0; itemIndex < itemCount; ++itemIndex)
        {
            if (sceneUnits - usedSceneUnits < 2u)
            {
                ThrowParams(ParamsRuntimeError::InvalidUnitCount, cursor.Offset(),
                    "params scene registration item exceeds units");
            }
            ParamsRegistSceneItem item;
            item.name = cursor.TypedText();
            const std::uint32_t nestedCount = cursor.TypedInteger();
            usedSceneUnits += 2;
            if (nestedCount > sceneUnits - usedSceneUnits)
            {
                ThrowParams(ParamsRuntimeError::InvalidUnitCount, cursor.Offset(),
                    "params scene registration nested count exceeds units");
            }
            item.scenes.reserve(nestedCount);
            for (std::uint32_t nested = 0; nested < nestedCount; ++nested)
            {
                item.scenes.push_back(cursor.TypedText());
                ++usedSceneUnits;
            }
            group.items.push_back(std::move(item));
        }
        gameSystem.registScene.push_back(std::move(group));
    }
    if (usedSceneUnits != sceneUnits)
    {
        ThrowParams(ParamsRuntimeError::InvalidUnitCount, cursor.Offset(),
            "params scene registration units do not match");
    }
    return gameSystem;
}

[[nodiscard]] std::vector<ParamsPatternGroup> ReadPatternGroupTable(Cursor& cursor)
{
    const std::uint32_t groupCount = cursor.U32();
    cursor.RequireCount(groupCount, 4);
    std::vector<ParamsPatternGroup> groups;
    groups.reserve(groupCount);
    for (std::uint32_t groupIndex = 0; groupIndex < groupCount; ++groupIndex)
    {
        ParamsPatternGroup group;
        group.name = cursor.Text16();
        const std::uint16_t indexCount = cursor.U16();
        cursor.RequireCount(indexCount, 4);
        group.indices.reserve(indexCount);
        for (std::uint16_t index = 0; index < indexCount; ++index)
        {
            group.indices.push_back(cursor.U32());
        }
        groups.push_back(std::move(group));
    }
    return groups;
}

[[nodiscard]] ParamsPattern ReadPattern(Cursor& cursor)
{
    ParamsPattern pattern;
    const std::uint32_t itemCount = cursor.U32();
    cursor.RequireCount(itemCount, 3);
    pattern.items.reserve(itemCount);
    for (std::uint32_t index = 0; index < itemCount; ++index)
    {
        ParamsPatternItem item;
        item.name = cursor.Text16();
        const std::size_t kindOffset = cursor.Offset();
        item.kind = cursor.U8();
        switch (item.kind)
        {
        case 0:
            break;
        case 1:
        {
            const std::uint32_t count = cursor.U32();
            cursor.RequireCount(count, 2);
            std::vector<ParamsText> strings;
            strings.reserve(count);
            for (std::uint32_t stringIndex = 0; stringIndex < count; ++stringIndex)
            {
                strings.push_back(cursor.Text16());
            }
            item.strings.AssignSnapshot(strings);
            break;
        }
        case 2:
            item.subName = cursor.Text16();
            item.value1 = cursor.U32();
            item.value2 = cursor.U32();
            break;
        case 3:
            item.subName = cursor.Text16();
            item.value1 = cursor.U32();
            break;
        default:
            ThrowParams(ParamsRuntimeError::InvalidPatternKind, kindOffset,
                "params Pattern item kind is invalid");
        }
        pattern.items.push_back(std::move(item));
    }

    const std::uint32_t integerArrayCount = cursor.U32();
    cursor.RequireCount(integerArrayCount, 1);
    pattern.integerArrays.reserve(integerArrayCount);
    for (std::uint32_t arrayIndex = 0; arrayIndex < integerArrayCount; ++arrayIndex)
    {
        const std::uint8_t count = cursor.U8();
        cursor.RequireCount(count, 4);
        std::vector<std::uint32_t> values;
        values.reserve(count);
        for (std::uint8_t index = 0; index < count; ++index)
        {
            values.push_back(cursor.U32());
        }
        pattern.integerArrays.push_back(std::move(values));
    }
    pattern.groupTables[0] = ReadPatternGroupTable(cursor);
    pattern.groupTables[1] = ReadPatternGroupTable(cursor);
    return pattern;
}

[[nodiscard]] std::vector<ParamsSceneLabel> ReadSceneLabels(Cursor& cursor)
{
    const std::uint32_t count = cursor.U32();
    cursor.RequireCount(count, 10);
    std::vector<ParamsSceneLabel> labels;
    labels.reserve(count);
    for (std::uint32_t index = 0; index < count; ++index)
    {
        labels.push_back({cursor.Text16(), cursor.U32(), cursor.U32()});
    }
    return labels;
}
}

DecodedParamsDemoCommand DecodeParamsDemoCommand(const ParamsDemoCommand& command)
{
    return DecodeDemoPayload(command, 0);
}

ParamsRuntimeException::ParamsRuntimeException(
    const ParamsRuntimeError error,
    const std::size_t offset,
    const char* const message)
    : std::runtime_error(message), error_(error), offset_(offset)
{
}

ParamsRuntimeError ParamsRuntimeException::Error() const noexcept
{
    return error_;
}

std::size_t ParamsRuntimeException::Offset() const noexcept
{
    return offset_;
}

ParamsRuntimeProfile ParamsRuntimeProfileParser::Parse(
    const std::span<const std::byte> bytes)
{
    if (bytes.empty())
    {
        ThrowParams(ParamsRuntimeError::EmptyInput, 0, "params.dat is empty");
    }

    Cursor cursor(bytes);
    const std::size_t headerOffset = cursor.Offset();
    const std::string header = cursor.Ascii(V58Header.size());
    if (header != V58Header)
    {
        ThrowParams(ParamsRuntimeError::UnsupportedVersion, headerOffset,
            "params.dat version is not v05.8");
    }

    ParamsRuntimeProfile profile;
    profile.header = header;
    profile.version = 5.8;
    profile.gameSystem = ReadGameSystem(cursor, profile.v53TripleRawCount);
    profile.pattern = ReadPattern(cursor);
    profile.sceneLabels = ReadSceneLabels(cursor);
    if (!cursor.End())
    {
        ThrowParams(ParamsRuntimeError::TrailingData, cursor.Offset(),
            "params.dat has trailing bytes after SceneLabel");
    }
    return profile;
}
}

#include <Runtime/ParamsDemoReader.h>
#include <Runtime/ParamsRuntimeReadCursor.h>

#include <bit>
#include <cstdlib>
#include <cstring>
#include <mbstring.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 原 PE 5AE06C/5AE094，保留 char* 类型和完整 CP932 消息。
char DemoFormatError[] =
    "\x96\xA2\x91\xCE\x89\x9E\x82\xCC\x83\x66\x83\x82\x83\x66\x81\x5B"
    "\x83\x5E\x82\xCC\x83\x74\x83\x48\x81\x5B\x83\x7D\x83\x62\x83\x67"
    "\x82\xC5\x82\xB7\x81\x49";
char DemoCommandError[] =
    "\x96\xA2\x91\xCE\x89\x9E\x82\xCC\x83\x66\x83\x82\x83\x66\x81\x5B"
    "\x83\x5E\x82\xCC\x83\x52\x83\x7D\x83\x93\x83\x68\x82\xC5\x82\xB7\x81\x49";

ATL::CStringData* Header(const char* characters) noexcept
{
    return reinterpret_cast<ATL::CStringData*>(
        reinterpret_cast<std::uintptr_t>(characters) - sizeof(ATL::CStringData));
}

class TextCell final
{
public:
    ~TextCell() noexcept { if (active_) DestroyRuntimeCStringAt40CD40(&cell_.value); }
    RuntimeCString& Value() noexcept { return cell_.value; }
    void Arm() noexcept { active_ = true; }
    void Clone(const RuntimeCString& source)
    {
        auto* header = CloneRuntimeCStringDataAt414540(Header(source.GetString()));
        const auto characters = reinterpret_cast<std::uintptr_t>(header) + sizeof(ATL::CStringData);
        std::memcpy(&cell_.value, &characters, sizeof(characters));
        active_ = true;
    }
    void ReleaseNormal()
    {
        const auto* characters = cell_.value.GetString();
        ReleaseCached(characters);
    }
    void ReleaseCached(const char* characters)
    {
        active_ = false;
        ReleaseRuntimeCStringHeader(Header(characters));
    }
private:
    union Cell { RuntimeCString value; Cell() noexcept {} ~Cell() noexcept {} } cell_;
    bool active_ = false;
};

class MemberTextUnwind final
{
public:
    ~MemberTextUnwind() noexcept { if (text_) DestroyRuntimeCStringAt40CD40(text_); }
    void Construct(RuntimeCString& text, IRuntimeStringManagerSource& strings)
    {
        InitializeEmptyRuntimeCString(text, strings);
        text_ = &text;
    }
    void Disarm() noexcept { text_ = nullptr; }
private:
    RuntimeCString* text_ = nullptr;
};

struct RawBodyUnwind final
{
    IEnvironmentRootMemory& memory;
    void* body;
    std::uint32_t bytes;
    bool active = true;
    ~RawBodyUnwind() noexcept { if (active) memory.DeleteBody(body, bytes); }
};

class ReferenceUnwind final
{
public:
    explicit ReferenceUnwind(YtlReferenceSlotRuntime& reference) noexcept : reference_(reference) {}
    ~ReferenceUnwind() noexcept { if (active_) ReleaseParamsDemoCommandAt40D3D0(reference_); }
    void Disarm() noexcept { active_ = false; }
private:
    YtlReferenceSlotRuntime& reference_;
    bool active_ = true;
};

std::uint8_t ByteAt(const ParamsRuntimeReadCursor& cursor, std::uint32_t offset)
{
    return std::to_integer<std::uint8_t>(cursor.Slice(offset, 1)[0]);
}

std::uint32_t DwordAt(const ParamsRuntimeReadCursor& cursor, std::uint32_t offset)
{
    const auto bytes = cursor.Slice(offset, 4);
    std::uint32_t value;
    std::memcpy(&value, bytes.data(), sizeof(value));
    return value;
}

void ReadShortText(TextCell& text, const ParamsRuntimeReadCursor& cursor,
    std::uint32_t lengthOffset, IRuntimeStringManagerSource& strings)
{
    const auto length = ByteAt(cursor, lengthOffset);
    // 只捕获有效源尾，不在两次 GetNil 前拒绝短正文；实际 WCHAR 访问处才检查范围。
    const auto input = cursor.CaptureSourceAt(lengthOffset + 1u);
    try
    {
        InitializeDemoShortRuntimeCStringAt420650(text.Value(),
            reinterpret_cast<const wchar_t*>(input.data), length, strings,
            input.availableBytes.value_or(static_cast<std::size_t>(-1)));
    }
    catch (const DemoShortTextInputTruncated&)
    {
        throw ParamsRuntimeException(ParamsRuntimeError::TruncatedInput, lengthOffset + 1u,
            "Demo WCHAR text exceeds captured input (safe boundary)");
    }
    text.Arm();
}

void ReadHeader(TextCell& text, const ParamsRuntimeReadCursor& cursor,
    std::uint32_t start, IRuntimeStringManagerSource& strings)
{
    const auto input = cursor.CaptureSourceAt(start);
    auto* manager = strings.CaptureAt417360();
    if (!manager) throw ATL::CAtlException(E_FAIL);
    if (!input.data) throw ATL::CAtlException(E_INVALIDARG);
    using Allocate = ATL::CStringData* (__thiscall*)(ATL::IAtlStringMgr*, int, int);
    const std::byte* table;
    std::memcpy(&table, manager, sizeof(table));
    Allocate allocate;
    std::memcpy(&allocate, table, sizeof(allocate));
    auto* header = allocate(manager, 9, 1);
    if (!header) throw ATL::CAtlException(E_OUTOFMEMORY);
    const auto characters = reinterpret_cast<std::uintptr_t>(header) + sizeof(ATL::CStringData);
    std::memcpy(&text.Value(), &characters, sizeof(characters));
    if (header->nAllocLength < 9) throw ATL::CAtlException(E_INVALIDARG);
    header->nDataLength = 9;
    reinterpret_cast<char*>(characters)[9] = 0;
    // 419F3B 才取得 header 清理资格；安全截断失败不擅自添加更早的回收。
    if (input.availableBytes && *input.availableBytes < 9)
        throw ParamsRuntimeException(ParamsRuntimeError::TruncatedInput, start,
            "Demo header exceeds captured input (safe boundary)");
    static_cast<void>(MoveRuntimeBytesAt43D9B0(reinterpret_cast<void*>(characters), 9, input.data, 9));
    text.Arm();
}

void AppendCompleted(ParamsDemoDataRuntime& target, ParamsDemoCommandType type,
    void* body, IEnvironmentRootMemory& memory)
{
    YtlReferenceSlotRuntime temporary;
    auto* control = PublishParamsDemoCommandReference(temporary, type, body, memory);
    ReferenceUnwind unwind(temporary);
    const bool grows = target.commandsAt00.end == target.commandsAt00.capacity;
    if (!grows && type <= ParamsDemoCommandType::Update)
    {
        // End..Update 内联快路径把 bias 写0；Move/Pos才调用通用43D350复制原bias。
        auto* output = target.commandsAt00.end;
        output->baseAt00 = temporary.baseAt00;
        output->biasAt04 = 0;
        output->controlAt08 = control;
        if (control) ++control->referenceCountAt04;
        ++target.commandsAt00.end;
    }
    else AppendParamsDemoCommandAt43D350(target.commandsAt00, temporary, memory);
    if (grows || type == ParamsDemoCommandType::Move)
        control = static_cast<EnvironmentObjectReferenceControl*>(temporary.controlAt08);
    unwind.Disarm();
    if (type == ParamsDemoCommandType::Pos)
        ReleaseParamsDemoCommandAt40D3D0(temporary);
    else
        ReleaseParamsDemoCommandCached(temporary, control);
}

void ReadNamedCommand(ParamsDemoDataRuntime& target, ParamsDemoCommandType type,
    const ParamsRuntimeReadCursor& cursor, std::uint32_t payload,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    const auto first = ByteAt(cursor, payload);
    const auto second = type == ParamsDemoCommandType::Sound ? ByteAt(cursor, payload + 1u) : 0u;
    TextCell name;
    ReadShortText(name, cursor, payload + (type == ParamsDemoCommandType::Sound ? 2u : 1u), strings);
    auto* const body = AllocateParamsDemoCommandBody(type, memory);
    RawBodyUnwind raw{memory, body, 12};
    if (body)
    {
        TextCell parameter;
        parameter.Clone(name.Value());
        const auto* cachedParameter = parameter.Value().GetString();
        auto& member = *reinterpret_cast<RuntimeCString*>(static_cast<std::byte*>(body) + 8);
        MemberTextUnwind memberUnwind;
        memberUnwind.Construct(member, strings);
        if (type == ParamsDemoCommandType::Sound)
        {
            auto& value = *static_cast<ParamsDemoCmdSound*>(body);
            value.modeAt04 = first;
            value.typeAt00 = 3;
            value.valueAt05 = static_cast<std::uint8_t>(second);
        }
        else
        {
            auto& value = *static_cast<ParamsDemoCmdLoad*>(body);
            value.slotAt04 = first;
            value.typeAt00 = 4;
        }
        AssignRuntimeCStringAt410220(member, parameter.Value());
        // 正常参数 Free 前同时撤成员和参数资格；只保留 raw body + reader name。
        memberUnwind.Disarm();
        parameter.ReleaseCached(cachedParameter);
    }
    raw.active = false;
    AppendCompleted(target, type, body, memory);
    name.ReleaseNormal();
}

void ReadTransit(ParamsDemoDataRuntime& target, const ParamsRuntimeReadCursor& cursor,
    std::uint32_t payload, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    TextCell effect;
    ReadShortText(effect, cursor, payload, strings);
    // 41A634..41A654：首串转换后重取输入和首长度，不沿用调用前的长度或 span。
    const auto effectLength = ByteAt(cursor, payload);
    const auto step = DwordAt(cursor, payload + effectLength + 1u);
    TextCell mask;
    ReadShortText(mask, cursor, payload + effectLength + 5u, strings);
    auto* const body = static_cast<ParamsDemoCmdTransit*>(
        AllocateParamsDemoCommandBody(ParamsDemoCommandType::Transit, memory));
    RawBodyUnwind raw{memory, body, 16};
    if (body)
    {
        TextCell maskParameter;
        maskParameter.Clone(mask.Value());
        const auto* cachedMask = maskParameter.Value().GetString();
        TextCell effectParameter;
        effectParameter.Clone(effect.Value());
        const auto* cachedEffect = effectParameter.Value().GetString();
        MemberTextUnwind effectUnwind;
        effectUnwind.Construct(body->effectAt04, strings);
        MemberTextUnwind maskUnwind;
        maskUnwind.Construct(body->maskAt0C, strings);
        body->typeAt00 = 5;
        AssignRuntimeCStringAt410220(body->effectAt04, effectParameter.Value());
        body->stepAt08 = step;
        AssignRuntimeCStringAt410220(body->maskAt0C, maskParameter.Value());
        maskUnwind.Disarm();
        effectUnwind.Disarm();
        effectParameter.ReleaseCached(cachedEffect);
        maskParameter.ReleaseCached(cachedMask);
    }
    raw.active = false;
    AppendCompleted(target, ParamsDemoCommandType::Transit, body, memory);
    mask.ReleaseNormal();
    effect.ReleaseNormal();
}

void ReadPlainCommand(ParamsDemoDataRuntime& target, ParamsDemoCommandType type,
    const ParamsRuntimeReadCursor& cursor, std::uint32_t payload, IEnvironmentRootMemory& memory)
{
    void* body = nullptr;
    switch (type)
    {
    case ParamsDemoCommandType::End:
    case ParamsDemoCommandType::Next:
    case ParamsDemoCommandType::Update:
        body = AllocateParamsDemoCommandBody(type, memory);
        if (body) *static_cast<std::uint32_t*>(body) = static_cast<std::uint32_t>(type);
        break;
    case ParamsDemoCommandType::Wait:
    {
        const auto mode = ByteAt(cursor, payload);
        const auto ticks = DwordAt(cursor, payload + 1u);
        auto* value = static_cast<ParamsDemoCmdWait*>(body = AllocateParamsDemoCommandBody(type, memory));
        if (value) { value->typeAt00 = 2; value->modeAt04 = mode; value->ticksAt08 = ticks; }
        break;
    }
    case ParamsDemoCommandType::Disp:
    {
        const auto visible = ByteAt(cursor, payload + 1u) != 0;
        const auto slot = ByteAt(cursor, payload);
        auto* value = static_cast<ParamsDemoCmdDisp*>(body = AllocateParamsDemoCommandBody(type, memory));
        if (value)
        {
            value->typeAt00 = 6;
            value->slotAt04 = slot > 0x80u ? static_cast<std::uint32_t>(static_cast<std::uint8_t>(~slot)) - 1u : slot;
            value->visibleAt08 = static_cast<std::uint8_t>(visible);
        }
        break;
    }
    case ParamsDemoCommandType::Move:
    {
        const auto slot = ByteAt(cursor, payload);
        const auto duration = DwordAt(cursor, payload + 1u);
        const auto startX = DwordAt(cursor, payload + 5u);
        const auto startY = DwordAt(cursor, payload + 9u);
        const auto endX = DwordAt(cursor, payload + 13u);
        const auto endY = DwordAt(cursor, payload + 17u);
        auto* value = static_cast<ParamsDemoCmdMove*>(body = AllocateParamsDemoCommandBody(type, memory));
        if (value)
        {
            value->startXAt0C = 0; value->startYAt10 = 0; value->endXAt14 = 0; value->endYAt18 = 0;
            value->slotAt04 = slot; value->durationAt08 = duration;
            value->startXAt0C = startX; value->startYAt10 = startY; value->endXAt14 = endX;
            value->typeAt00 = 8; value->endYAt18 = endY;
        }
        break;
    }
    case ParamsDemoCommandType::Pos:
    {
        const auto slot = ByteAt(cursor, payload);
        const auto x = DwordAt(cursor, payload + 1u);
        const auto y = DwordAt(cursor, payload + 5u);
        auto* value = static_cast<ParamsDemoCmdPos*>(body = AllocateParamsDemoCommandBody(type, memory));
        if (value)
        {
            value->xAt08 = 0; value->yAt0C = 0; value->slotAt04 = slot;
            value->xAt08 = x; value->typeAt00 = 9; value->yAt0C = y;
        }
        break;
    }
    default: throw DemoCommandError;
    }
    AppendCompleted(target, type, body, memory);
}
}

void ReadParamsDemoDataAt419EB0(ParamsRuntimeReadCursor& cursor, ParamsDemoDataRuntime* target,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    const auto start = static_cast<std::uint32_t>(cursor.Offset());
    TextCell header;
    ReadHeader(header, cursor, start, strings);
    const auto* headerCharacters = header.Value().GetString();
    if (_mbscmp(reinterpret_cast<const unsigned char*>(headerCharacters),
        reinterpret_cast<const unsigned char*>("[Demo3.0]")) != 0) throw DemoFormatError;
    auto countCursor = cursor;
    countCursor.Seek(start + 9u);
    const auto count = countCursor.U16();
    if (!target) _invalid_parameter_noinfo_noreturn();
    ClearParamsDemoCommandsAt419F71(target->commandsAt00);
    std::uint32_t consumed = 11;
    for (std::uint32_t index = 0; index < count; ++index)
    {
        const auto type = static_cast<ParamsDemoCommandType>(ByteAt(cursor, start + consumed));
        const auto payload = start + consumed + 2u;
        if (type == ParamsDemoCommandType::Sound || type == ParamsDemoCommandType::Load)
            ReadNamedCommand(*target, type, cursor, payload, strings, memory);
        else if (type == ParamsDemoCommandType::Transit)
            ReadTransit(*target, cursor, payload, strings, memory);
        else
            ReadPlainCommand(*target, type, cursor, payload, memory);
        // 全部临时 Ref/串正常释放后才重取输入 length；允许0/1、重叠和填充。
        consumed += ByteAt(cursor, start + consumed + 1u);
    }
    if (count) header.ReleaseNormal(); // 41AD29 重取 var_C8；零条目路径保留最初 ESI。
    else header.ReleaseCached(headerCharacters);
    cursor.CommitOffsetAfter419EB0(start + consumed);
}

void ReadParamsDemoEntriesAt41CDB8(ParamsRuntimeReadCursor& cursor, ParamsDemoStorage& target,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    const auto count = cursor.U8();
    if (!count) return;
    static_cast<void>(RebuildParamsDemoEntriesAt41CDCC(target.storage, count, memory, strings));
    for (std::uint32_t index = 0;
        std::bit_cast<std::int32_t>(index) < std::bit_cast<std::int32_t>(target.size()); ++index)
    {
        auto* entry = target.Resolve(index);
        cursor.Text16(entry ? &entry->nameAt00 : nullptr);
        entry = target.Resolve(index);
        if (!entry) _invalid_parameter_noinfo_noreturn();
        static_cast<void>(RebuildParamsDemoDataAt41CF13(entry->dataAt04, memory));
        entry = target.Resolve(index);
        if (!entry) _invalid_parameter_noinfo_noreturn();
        ReadParamsDemoDataAt419EB0(cursor, ResolveParamsDemoDataAt4A52A0(entry->dataAt04), strings, memory);
    }
}
}

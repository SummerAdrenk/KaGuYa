#include <Runtime/ParamsDemoStorage.h>
#include <Runtime/ParamsDemoCommandRuntime.h>

#include <cstring>
#include <limits>
#include <new>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
constexpr std::uint32_t MaximumEntries = 0x0FFFFFFFu;
constexpr std::uint32_t MaximumCommands = (0xFFFFFFFFu - 0x23u) / 12u;

void WriteText(RuntimeCString& destination, std::string_view text)
{
    if (text.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        throw std::length_error("Params demo snapshot exceeds CString length");
    SetRuntimeCStringAt437680(destination, text.data(), static_cast<int>(text.size()));
}

// 只保护现代快照工厂的已发布无control正文，不改变原生reader的泄留资格。
struct EntriesOrphanCleanup final
{
    Slot& slot;
    std::uint32_t count;
    ~EntriesOrphanCleanup() noexcept
    {
        if (!slot.baseAt00 || slot.controlAt08) return;
        auto* const entries = reinterpret_cast<ParamsDemoEntryRuntime*>(slot.baseAt00);
        while (count) DestroyParamsDemoEntryAt40FB50(entries[--count]);
        auto* const cookie = reinterpret_cast<std::uint32_t*>(entries) - 1;
        const auto bytes = *cookie * 16u + 4u;
        ::operator delete[](cookie, static_cast<std::size_t>(bytes));
        slot = {};
    }
};

void BuildEntries(ParamsDemoStorage& destination, std::size_t count)
{
    if (count > MaximumEntries)
        throw std::length_error("Params demo snapshot exceeds entry count");
    EntriesOrphanCleanup cleanup{destination.storage, static_cast<std::uint32_t>(count)};
    auto* const entries = RebuildParamsDemoEntriesAt41CDCC(
        destination.storage, static_cast<std::uint32_t>(count));
    if (count && !entries) throw std::bad_alloc();
}

struct DataOrphanCleanup final
{
    Slot& slot;
    ~DataOrphanCleanup() noexcept
    {
        if (!slot.baseAt00 || slot.controlAt08) return;
        auto* const body = reinterpret_cast<ParamsDemoDataRuntime*>(slot.baseAt00);
        DestroyParamsDemoDataAt40DF10(*body);
        ::operator delete(body, std::size_t{0x0C});
        slot = {};
    }
};

ParamsDemoDataRuntime& BuildData(Slot& destination)
{
    DataOrphanCleanup cleanup{destination};
    auto* const body = RebuildParamsDemoDataAt41CF13(destination);
    if (!body) throw std::bad_alloc();
    return *body;
}

std::uint32_t CommandCount(const ParamsDemoCommandVector& source)
{
    if (source.begin == source.end) return 0;
    const auto begin = reinterpret_cast<std::uintptr_t>(source.begin);
    const auto end = reinterpret_cast<std::uintptr_t>(source.end);
    const auto capacity = reinterpret_cast<std::uintptr_t>(source.capacity);
    if (!begin || end < begin || capacity < end || (end - begin) % sizeof(Slot) != 0)
        throw std::logic_error("Params demo snapshot has invalid command storage");
    return static_cast<std::uint32_t>((end - begin) / sizeof(Slot));
}

void AllocateCommands(ParamsDemoCommandVector& destination, std::size_t count)
{
    if (count > MaximumCommands)
        throw std::length_error("Params demo snapshot exceeds command count");
    if (!count) return;
    const auto bytes = static_cast<std::uint32_t>(count) * 12u;
    const auto request = bytes < 0x1000u ? bytes : bytes + 0x23u;
    void* const raw = DefaultEnvironmentRootMemory().AllocateBody(request);
    if (!raw) throw std::bad_alloc();
    auto address = reinterpret_cast<std::uintptr_t>(raw);
    if (bytes >= 0x1000u)
    {
        address = (address + 0x23u) & ~std::uintptr_t{31};
        std::memcpy(reinterpret_cast<void*>(address - 4u), &raw, sizeof(raw));
    }
    // 工具层一次分配，end只登记已完成的Ref；容量仍满足原生vector删除/后续增长规则。
    destination.begin = reinterpret_cast<Slot*>(address);
    destination.end = destination.begin;
    destination.capacity = reinterpret_cast<Slot*>(address + bytes);
}

class CommandConstruction final
{
public:
    explicit CommandConstruction(ParamsDemoCommandType type)
        : type_(type), body_(AllocateParamsDemoCommandBody(type))
    {
        if (!body_) throw std::bad_alloc();
        const auto value = static_cast<std::uint32_t>(type);
        std::memcpy(body_, &value, sizeof(value));
    }
    ~CommandConstruction() noexcept
    {
        if (!body_) return;
        while (constructed_) DestroyRuntimeCStringAt40CD40(strings_[--constructed_]);
        ::operator delete(body_, static_cast<std::size_t>(ParamsDemoCommandBodySize(type_)));
    }
    CommandConstruction(const CommandConstruction&) = delete;
    CommandConstruction& operator=(const CommandConstruction&) = delete;

    template<class T> T& Body() noexcept { return *static_cast<T*>(body_); }

    void Text(RuntimeCString& destination, std::string_view text)
    {
        InitializeEmptyRuntimeCString(destination);
        strings_[constructed_++] = &destination;
        WriteText(destination, text);
    }

    void Publish(Slot& destination)
    {
        static_cast<void>(PublishParamsDemoCommandReference(destination, type_, body_));
        body_ = nullptr;
    }

private:
    ParamsDemoCommandType type_;
    void* body_;
    std::array<RuntimeCString*, 2> strings_{};
    std::size_t constructed_ = 0;
};

ParamsDemoCommandType CommandType(const void* body)
{
    std::uint32_t type;
    std::memcpy(&type, body, sizeof(type));
    const auto result = static_cast<ParamsDemoCommandType>(type);
    static_cast<void>(ParamsDemoCommandBodySize(result));
    return result;
}

void ImportCommand(Slot& destination, const ParamsDemoCommand& source)
{
    // 固定CP932的严格工具decoder；不借用runtime的420650/线程ACP语义。
    const auto decoded = DecodeParamsDemoCommand(source);
    const auto type = static_cast<ParamsDemoCommandType>(source.type);
    CommandConstruction command(type);
    switch (type)
    {
    case ParamsDemoCommandType::End:
    case ParamsDemoCommandType::Next:
    case ParamsDemoCommandType::Update:
        break;
    case ParamsDemoCommandType::Wait:
    {
        const auto& value = std::get<DemoWaitCommand>(decoded);
        auto& body = command.Body<ParamsDemoCmdWait>();
        body.modeAt04 = value.mode;
        body.ticksAt08 = static_cast<std::uint32_t>(value.ticks);
        break;
    }
    case ParamsDemoCommandType::Sound:
    {
        const auto& value = std::get<DemoSoundCommand>(decoded);
        auto& body = command.Body<ParamsDemoCmdSound>();
        body.modeAt04 = value.mode;
        body.valueAt05 = value.valueAt05;
        command.Text(body.nameAt08, value.resourceName.cp932);
        break;
    }
    case ParamsDemoCommandType::Load:
    {
        const auto& value = std::get<DemoLoadCommand>(decoded);
        auto& body = command.Body<ParamsDemoCmdLoad>();
        body.slotAt04 = value.slot;
        command.Text(body.nameAt08, value.resourceName.cp932);
        break;
    }
    case ParamsDemoCommandType::Transit:
    {
        const auto& value = std::get<DemoTransitionCommand>(decoded);
        auto& body = command.Body<ParamsDemoCmdTransit>();
        body.stepAt08 = static_cast<std::uint32_t>(value.step);
        command.Text(body.effectAt04, value.effectName.cp932);
        command.Text(body.maskAt0C, value.maskName.cp932);
        break;
    }
    case ParamsDemoCommandType::Disp:
    {
        const auto& value = std::get<DemoVisibilityCommand>(decoded);
        auto& body = command.Body<ParamsDemoCmdDisp>();
        body.slotAt04 = static_cast<std::uint32_t>(value.slot);
        // 严格decoder已确认payload恰为两字节；bool DTO不能丢弃原visible byte。
        body.visibleAt08 = std::to_integer<std::uint8_t>(source.payload[1]);
        break;
    }
    case ParamsDemoCommandType::Move:
    {
        const auto& value = std::get<DemoMoveCommand>(decoded);
        auto& body = command.Body<ParamsDemoCmdMove>();
        body.slotAt04 = value.slot;
        body.durationAt08 = static_cast<std::uint32_t>(value.duration);
        body.startXAt0C = static_cast<std::uint32_t>(value.startX);
        body.startYAt10 = static_cast<std::uint32_t>(value.startY);
        body.endXAt14 = static_cast<std::uint32_t>(value.endX);
        body.endYAt18 = static_cast<std::uint32_t>(value.endY);
        break;
    }
    case ParamsDemoCommandType::Pos:
    {
        const auto& value = std::get<DemoPositionCommand>(decoded);
        auto& body = command.Body<ParamsDemoCmdPos>();
        body.slotAt04 = value.slot;
        body.xAt08 = static_cast<std::uint32_t>(value.x);
        body.yAt0C = static_cast<std::uint32_t>(value.y);
        break;
    }
    }
    command.Publish(destination);
}

void CopyCommand(Slot& destination, const Slot& source)
{
    const auto* const raw = ResolveParamsDemoCommand(source);
    if (!raw)
    {
        destination = {};
        return;
    }
    const auto type = CommandType(raw);
    CommandConstruction command(type);
    switch (type)
    {
    case ParamsDemoCommandType::End:
    case ParamsDemoCommandType::Next:
    case ParamsDemoCommandType::Update:
        break;
    case ParamsDemoCommandType::Wait:
    {
        const auto& value = *static_cast<const ParamsDemoCmdWait*>(raw);
        auto& body = command.Body<ParamsDemoCmdWait>();
        body.modeAt04 = value.modeAt04;
        body.ticksAt08 = value.ticksAt08;
        break;
    }
    case ParamsDemoCommandType::Sound:
    {
        const auto& value = *static_cast<const ParamsDemoCmdSound*>(raw);
        auto& body = command.Body<ParamsDemoCmdSound>();
        body.modeAt04 = value.modeAt04;
        body.valueAt05 = value.valueAt05;
        command.Text(body.nameAt08, RuntimeStringView(value.nameAt08));
        break;
    }
    case ParamsDemoCommandType::Load:
    {
        const auto& value = *static_cast<const ParamsDemoCmdLoad*>(raw);
        auto& body = command.Body<ParamsDemoCmdLoad>();
        body.slotAt04 = value.slotAt04;
        command.Text(body.nameAt08, RuntimeStringView(value.nameAt08));
        break;
    }
    case ParamsDemoCommandType::Transit:
    {
        const auto& value = *static_cast<const ParamsDemoCmdTransit*>(raw);
        auto& body = command.Body<ParamsDemoCmdTransit>();
        body.stepAt08 = value.stepAt08;
        command.Text(body.effectAt04, RuntimeStringView(value.effectAt04));
        command.Text(body.maskAt0C, RuntimeStringView(value.maskAt0C));
        break;
    }
    case ParamsDemoCommandType::Disp:
    {
        const auto& value = *static_cast<const ParamsDemoCmdDisp*>(raw);
        auto& body = command.Body<ParamsDemoCmdDisp>();
        body.slotAt04 = value.slotAt04;
        body.visibleAt08 = value.visibleAt08;
        break;
    }
    case ParamsDemoCommandType::Move:
        command.Body<ParamsDemoCmdMove>() = *static_cast<const ParamsDemoCmdMove*>(raw);
        break;
    case ParamsDemoCommandType::Pos:
        command.Body<ParamsDemoCmdPos>() = *static_cast<const ParamsDemoCmdPos*>(raw);
        break;
    }
    command.Publish(destination);
}

bool EqualCommand(const Slot& left, const Slot& right)
{
    const auto* const a = ResolveParamsDemoCommand(left);
    const auto* const b = ResolveParamsDemoCommand(right);
    if (!a || !b) return a == b;
    const auto type = CommandType(a);
    if (type != CommandType(b)) return false;
    switch (type)
    {
    case ParamsDemoCommandType::End:
    case ParamsDemoCommandType::Next:
    case ParamsDemoCommandType::Update:
        return true;
    case ParamsDemoCommandType::Wait:
    {
        const auto& x = *static_cast<const ParamsDemoCmdWait*>(a);
        const auto& y = *static_cast<const ParamsDemoCmdWait*>(b);
        return x.modeAt04 == y.modeAt04 && x.ticksAt08 == y.ticksAt08;
    }
    case ParamsDemoCommandType::Sound:
    {
        const auto& x = *static_cast<const ParamsDemoCmdSound*>(a);
        const auto& y = *static_cast<const ParamsDemoCmdSound*>(b);
        return x.modeAt04 == y.modeAt04 && x.valueAt05 == y.valueAt05 &&
            RuntimeStringView(x.nameAt08) == RuntimeStringView(y.nameAt08);
    }
    case ParamsDemoCommandType::Load:
    {
        const auto& x = *static_cast<const ParamsDemoCmdLoad*>(a);
        const auto& y = *static_cast<const ParamsDemoCmdLoad*>(b);
        return x.slotAt04 == y.slotAt04 &&
            RuntimeStringView(x.nameAt08) == RuntimeStringView(y.nameAt08);
    }
    case ParamsDemoCommandType::Transit:
    {
        const auto& x = *static_cast<const ParamsDemoCmdTransit*>(a);
        const auto& y = *static_cast<const ParamsDemoCmdTransit*>(b);
        return x.stepAt08 == y.stepAt08 &&
            RuntimeStringView(x.effectAt04) == RuntimeStringView(y.effectAt04) &&
            RuntimeStringView(x.maskAt0C) == RuntimeStringView(y.maskAt0C);
    }
    case ParamsDemoCommandType::Disp:
    {
        const auto& x = *static_cast<const ParamsDemoCmdDisp*>(a);
        const auto& y = *static_cast<const ParamsDemoCmdDisp*>(b);
        return x.slotAt04 == y.slotAt04 && x.visibleAt08 == y.visibleAt08;
    }
    case ParamsDemoCommandType::Move:
    {
        const auto& x = *static_cast<const ParamsDemoCmdMove*>(a);
        const auto& y = *static_cast<const ParamsDemoCmdMove*>(b);
        return x.slotAt04 == y.slotAt04 && x.durationAt08 == y.durationAt08 &&
            x.startXAt0C == y.startXAt0C && x.startYAt10 == y.startYAt10 &&
            x.endXAt14 == y.endXAt14 && x.endYAt18 == y.endYAt18;
    }
    case ParamsDemoCommandType::Pos:
    {
        const auto& x = *static_cast<const ParamsDemoCmdPos*>(a);
        const auto& y = *static_cast<const ParamsDemoCmdPos*>(b);
        return x.slotAt04 == y.slotAt04 && x.xAt08 == y.xAt08 && x.yAt0C == y.yAt0C;
    }
    }
    return false;
}

const ParamsDemoEntryRuntime& RequireEntry(const ParamsDemoStorage& source, std::uint32_t index)
{
    const auto* const body = source.Resolve(index);
    if (!body) throw std::logic_error("Params demo snapshot has null entry storage");
    return *body;
}
}

void ParamsDemoStorage::AssignSnapshot(std::span<const ParamsDemo> values)
{
    ParamsDemoStorage snapshot;
    BuildEntries(snapshot, values.size());
    for (std::uint32_t index = 0; index < snapshot.size(); ++index)
    {
        auto& entry = *snapshot.Resolve(index);
        const auto& value = values[index];
        WriteText(entry.nameAt00, value.name.cp932);
        auto& data = BuildData(entry.dataAt04);
        AllocateCommands(data.commandsAt00, value.commands.size());
        for (const auto& command : value.commands)
        {
            ImportCommand(*data.commandsAt00.end, command);
            ++data.commandsAt00.end;
        }
    }
    *this = std::move(snapshot);
}

void ParamsDemoStorage::AssignSnapshot(const ParamsDemoStorage& source)
{
    if (this == &source) return;
    ParamsDemoStorage snapshot;
    BuildEntries(snapshot, source.size());
    for (std::uint32_t index = 0; index < snapshot.size(); ++index)
    {
        const auto& original = RequireEntry(source, index);
        auto& entry = *snapshot.Resolve(index);
        WriteText(entry.nameAt00, RuntimeStringView(original.nameAt00));
        const auto* const originalData = ResolveParamsDemoDataAt4A52A0(original.dataAt04);
        // 缺失Data与已分配的空Data是不同状态；空command槽亦保留，而非生成END。
        if (!originalData) continue;
        const auto count = CommandCount(originalData->commandsAt00);
        auto& data = BuildData(entry.dataAt04);
        AllocateCommands(data.commandsAt00, count);
        for (std::uint32_t command = 0; command < count; ++command)
        {
            CopyCommand(*data.commandsAt00.end, originalData->commandsAt00.begin[command]);
            ++data.commandsAt00.end;
        }
    }
    *this = std::move(snapshot);
}

ParamsDemoStorage::ParamsDemoStorage(const ParamsDemoStorage& other)
{
    AssignSnapshot(other);
}

ParamsDemoStorage& ParamsDemoStorage::operator=(const ParamsDemoStorage& other)
{
    AssignSnapshot(other);
    return *this;
}

bool ParamsDemoStorage::operator==(const ParamsDemoStorage& other) const
{
    const auto count = size();
    if (count != other.size()) return false;
    for (std::uint32_t index = 0; index < count; ++index)
    {
        const auto& left = RequireEntry(*this, index);
        const auto& right = RequireEntry(other, index);
        if (RuntimeStringView(left.nameAt00) != RuntimeStringView(right.nameAt00)) return false;
        const auto* const a = ResolveParamsDemoDataAt4A52A0(left.dataAt04);
        const auto* const b = ResolveParamsDemoDataAt4A52A0(right.dataAt04);
        if (!a || !b)
        {
            if (a != b) return false;
            continue;
        }
        const auto commands = CommandCount(a->commandsAt00);
        if (commands != CommandCount(b->commandsAt00)) return false;
        for (std::uint32_t command = 0; command < commands; ++command)
            if (!EqualCommand(a->commandsAt00.begin[command], b->commandsAt00.begin[command])) return false;
    }
    return true;
}
}

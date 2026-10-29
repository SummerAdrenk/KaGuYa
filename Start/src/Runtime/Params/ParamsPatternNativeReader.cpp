#include <Runtime/ParamsPatternNativeReader.h>
#include <Runtime/ParamsPatternItemNative.h>
#include <Runtime/ParamsRuntimeReadCursor.h>
#include <Runtime/ParamsLegacyTables.h>
#include <Runtime/EnvironmentArchiveVectorRuntime.h>
#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/ByteArrayReferenceRuntime.h>

#include <bit>
#include <cstring>
#include <cwchar>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
using Slot = YtlReferenceSlotRuntime;
using Control = YtlReferenceControlRuntime;

char unsupportedPatternKind[] =
    "\x83\x74\x83\x40\x83\x43\x83\x8B\x96\xBC\x83\x8A"
    "\x83\x58\x83\x67\x82\xCC\x83\x5E\x83\x43\x83\x76"
    "\x82\xAA\x96\xA2\x91\xCE\x89\x9E\x81\x49\x81\x49";

// 原局部Ref析构均flags5；是否已有引用资格留在栈guard，不扩充Ref12。
struct LocalReference final
{
    Slot slot{};
    bool armed = true;
    ~LocalReference() noexcept { if (armed) ReleaseYtlReference(slot); }
    void FinishCurrent() noexcept
    {
        armed = false;
        ReleaseYtlReference(slot);
    }
    void FinishCached(Control* captured) noexcept
    {
        // state12/14/16/17/1B/1C：异常直接terminate，不展开外围局部。
        armed = false;
        if (captured && --captured->referenceCountAt04 == 0)
        {
            if (captured->owningAt08) captured->DestroyPayloadAtVslot00();
            captured->DeleteControlAtVslot04(1);
        }
        if (captured)
        {
            slot.controlAt08 = nullptr;
            slot.baseAt00 = 0;
        }
    }
};

struct TemporaryName final
{
    union { RuntimeCString text; };
    bool armed = false;
    TemporaryName() noexcept {}
    explicit TemporaryName(IRuntimeStringManagerSource& strings)
    {
        InitializeEmptyRuntimeCString(text, strings);
        armed = true;
    }
    ~TemporaryName() noexcept { if (armed) DestroyRuntimeCStringAt40CD40(&text); }
    void Finish()
    {
        // 41EF08先取header、41EF0E撤资格；正常Free失败不重试，也不回滚已追加item。
        auto* header = reinterpret_cast<ATL::CStringData*>(const_cast<char*>(text.GetString())) - 1;
        armed = false;
        ReleaseRuntimeCStringHeader(header);
    }
};

template<class T> T& RequireBody(T* body)
{
    if (!body) throw std::logic_error("Pattern reader dereferenced a null body (safe boundary)");
    return *body;
}

std::int32_t Count(ParamsRuntimeReadCursor& cursor) { return std::bit_cast<std::int32_t>(cursor.U32()); }

bool ReadPatternText8(ParamsRuntimeReadCursor& cursor, RuntimeCString* destination,
    const std::uint8_t& key, IRuntimeStringManagerSource& strings)
{
    const auto start = cursor.Offset();
    const auto length = std::to_integer<std::uint8_t>(cursor.Slice(start, 1)[0]);
    if (length)
    {
        // 41E170/41E600/41F266/41F580：逐字节重取输入begin与正文key，原地解密。
        for (std::uint32_t index = 0; index < length; ++index)
            cursor.XorByteAt(start + 1u + index, key);
        // 41E190/41E61F/41F28B/41F5A9及420934：417360之前缓存源地址。
        const auto source = cursor.CaptureSourceAt(start + 1u);
        TemporaryName temporary;
        auto* const manager = strings.CaptureAt417360();
        ConstructRuntimeCStringAt40B490(&temporary.text,
            reinterpret_cast<const char*>(source.data), length, manager);
        temporary.armed = true;
        AssignRuntimeCStringAt410220(RequireBody(destination), temporary.text);
        // 原正常Free前撤临时资格，失败不重复减持；仅成功后累计消费量。
        temporary.Finish();
    }
    cursor.CommitOffsetAfter419EB0(static_cast<std::uint32_t>(start + length + 1u));
    return length != 0;
}

bool ReadPatternLegacyText(ParamsRuntimeReadCursor& cursor, RuntimeCString* destination,
    double version, const std::uint8_t& key, IRuntimeStringManagerSource& strings)
{
    if (version < 5.0) return ReadPatternText8(cursor, destination, key, strings);
    cursor.Text16(destination);
    return false;
}

void ReadLegacyMemberText16At41E66D(ParamsRuntimeReadCursor& cursor, RuntimeCString* destination)
{
    const auto start = cursor.Offset();
    auto next = cursor;
    const auto length = static_cast<std::uint32_t>(next.U16());
    if (length)
    {
        ByteArrayReferenceRuntime temporary;
        temporary.AllocateAt412700(length + 2u);
        auto* control = static_cast<EnvironmentObjectReferenceControl*>(temporary.RawSlot().controlAt08);
        const auto address = [&]() noexcept
        {
            const auto base = temporary.RawSlot().baseAt00;
            return base ? reinterpret_cast<std::uint8_t*>(base +
                temporary.RawSlot().biasAt04 * (control ? control->strideAt0C : 0u)) : nullptr;
        };
        // 41E6DA..41E767：三个操作各重读base；转换前还重读control，不能套420B00缓存法。
        std::memset(address(), 0, control ? control->countAt10 : 0u);
        auto* const target = address();
        const auto input = cursor.Slice(start + 2u, length);
        std::memmove(target, input.data(), length);
        control = static_cast<EnvironmentObjectReferenceControl*>(temporary.RawSlot().controlAt08);
        const auto* const wide = reinterpret_cast<const wchar_t*>(address());
        if (!std::wmemchr(wide, L'\0', (length + 2u) / sizeof(wchar_t)))
            throw ParamsRuntimeException(ParamsRuntimeError::InvalidUtf16, start + 2u,
                "params Pattern WCHAR text has no complete NUL within its byte allocation (safe boundary)");
        AssignRuntimeCStringWideAt4102D0(destination, wide);
        // 41E76F的state0B：正常释放固定ESI且异常终止；展开仍由40CF00处理当前Ref。
        [&]() noexcept
        {
            if (control && --control->referenceCountAt04 == 0)
            {
                if (control->owningAt08) control->DestroyPayloadAtVslot00();
                control->DeleteControlAtVslot04(1);
            }
            temporary.RawSlot().controlAt08 = nullptr;
            if (control) temporary.RawSlot().baseAt00 = 0;
        }();
    }
    cursor.CommitOffsetAfter419EB0(static_cast<std::uint32_t>(start + length + 2u));
}

void Retain(Slot& out, std::uintptr_t base, std::uint32_t bias, Control* control) noexcept
{
    out = {base, bias, control};
    if (control) ++control->referenceCountAt04;
}

// 快路用原寄存器base/control；慢路用实际source槽，返回后供正常释放重取control。
Control* AppendAt41EEA4(ParamsPatternReferenceVector& table, Slot& source,
    std::uintptr_t cachedBase, Control* cachedControl, IEnvironmentRootMemory& memory,
    bool zeroFastBias = false)
{
    if (table.end != table.capacity)
    {
        Retain(*table.end, cachedBase, zeroFastBias ? 0u : source.biasAt04, cachedControl);
        ++table.end;
        return cachedControl;
    }
    AppendEnvironmentArchiveReferenceAt49852D(table, source, memory);
    return source.controlAt08;
}

void ReadItems(ParamsRuntimeReadCursor& cursor, ParamsPatternNative& pattern,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    ClearEnvironmentArchiveReferencesAt498312(pattern.itemsAt04);
    const auto count = Count(cursor);
    for (std::int32_t index = 0; index < count; ++index)
    {
        TemporaryName name(strings);
        cursor.Text16(name.text);
        LocalReference generic;
        std::uintptr_t cachedBase = 0;
        Control* cachedControl = nullptr;
        const auto kind = cursor.U8();
        switch (kind)
        {
        case 0:
        {
            static_cast<void>(CreateParamsPatternFileNameAt41E904(generic.slot, strings, memory));
            cachedBase = generic.slot.baseAt00;
            cachedControl = generic.slot.controlAt08;
            AssignRuntimeCStringAt410220(RequireBody(ResolveParamsPatternFileName(generic.slot)).nameAt04, name.text);
            break;
        }
        case 1:
        {
            LocalReference typed;
            typed.armed = false;
            auto* body = CreateParamsPatternFileGroupAt41E972(typed.slot, strings, memory);
            typed.armed = true;
            cachedBase = reinterpret_cast<std::uintptr_t>(body);
            cachedControl = typed.slot.controlAt08;
            AssignRuntimeCStringAt410220(RequireBody(body).nameAt04, name.text);
            const auto stringsCount = Count(cursor);
            body->stringsAt08.RebuildAt412870(stringsCount, strings);
            for (std::int32_t item = 0; item < stringsCount; ++item)
                cursor.Text16(body->stringsAt08.Resolve(static_cast<std::uint32_t>(item)));
            Retain(generic.slot, cachedBase, 0, cachedControl);
            typed.FinishCached(cachedControl);
            break;
        }
        case 2:
        {
            LocalReference typed;
            typed.armed = false;
            auto* body = CreateParamsPatternExcPositionAt41EB42(typed.slot, strings, memory);
            typed.armed = true;
            cachedBase = reinterpret_cast<std::uintptr_t>(body);
            cachedControl = typed.slot.controlAt08;
            AssignRuntimeCStringAt410220(RequireBody(body).nameAt04, name.text);
            cursor.Text16(RequireBody(ResolveParamsPatternExcPosition(typed.slot)).subNameAt08);
            // 41EC77先写generic，41ECA8才增持。现代截断异常不得对refs=1双重减持。
            generic.slot = {cachedBase, 0, cachedControl};
            generic.armed = false;
            body->xAt0C = cursor.U32();
            body->yAt10 = cursor.U32();
            if (cachedControl) ++cachedControl->referenceCountAt04;
            generic.armed = true;
            typed.FinishCached(cachedControl);
            break;
        }
        case 3:
        {
            LocalReference typed;
            typed.armed = false;
            auto* const created = CreateParamsPatternFileConvertAt41ECE8(typed.slot, strings, memory);
            typed.armed = true;
            cachedBase = reinterpret_cast<std::uintptr_t>(created);
            cachedControl = typed.slot.controlAt08;
            AssignRuntimeCStringAt410220(RequireBody(ResolveParamsPatternFileConvert(typed.slot)).nameAt04, name.text);
            cursor.Text16(RequireBody(ResolveParamsPatternFileConvert(typed.slot)).subNameAt08);
            auto* body = ResolveParamsPatternFileConvert(typed.slot);
            RequireBody(body).flagsAt0C = cursor.U32();
            ReleaseParamsPatternItemAt43B810(generic.slot);
            Retain(generic.slot, cachedBase, 0, cachedControl);
            typed.FinishCached(cachedControl);
            break;
        }
        default:
            throw static_cast<char*>(unsupportedPatternKind);
        }
        cachedControl = AppendAt41EEA4(pattern.itemsAt04, generic.slot, cachedBase, cachedControl, memory);
        generic.FinishCached(cachedControl);
        name.Finish();
    }
}

void ReadLegacyItems(ParamsRuntimeReadCursor& cursor, ParamsPatternNative& pattern,
    double version, IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    ClearEnvironmentArchiveReferencesAt498312(pattern.itemsAt04);
    const auto count = Count(cursor);
    for (std::int32_t index = 0; index < count; ++index)
    {
        if (version < 4.0)
        {
            // 41E080..41E344：先造单文件正文，读完名称就append；没有外层临时name或kind字节。
            LocalReference owner;
            owner.armed = false;
            auto* const created = CreateParamsPatternFileNameAt41E080(owner.slot, strings, memory);
            owner.armed = true;
            auto* control = owner.slot.controlAt08;
            ReadPatternText8(cursor, created ? &created->nameAt04 : nullptr, pattern.legacyByte00, strings);
            // 41E318快路重取base但固定bias=0；408DE0慢路仍使用实际source槽。
            control = AppendAt41EEA4(pattern.itemsAt04, owner.slot, owner.slot.baseAt00, control, memory, true);
            owner.FinishCached(control);
            continue;
        }

        // 41E400..41E831：临时name先于generic Ref；U8是成员数，非新版kind。
        TemporaryName name(strings);
        ReadPatternLegacyText(cursor, &name.text, version, pattern.legacyByte00, strings);
        LocalReference generic;
        const auto stringsCount = cursor.U8();
        std::uintptr_t base;
        Control* control;
        if (!stringsCount)
        {
            static_cast<void>(CreateParamsPatternFileNameAt41E47C(generic.slot, strings, memory));
            base = generic.slot.baseAt00;
            control = generic.slot.controlAt08;
            AssignRuntimeCStringAt410220(RequireBody(ResolveParamsPatternFileName(generic.slot)).nameAt04, name.text);
        }
        else
        {
            // 41E4F9的14h构造保护与41E972相同；旧路直接拥有generic，不加新版typed临时Ref。
            auto* body = CreateParamsPatternFileGroupAt41E972(generic.slot, strings, memory);
            control = generic.slot.controlAt08;
            AssignRuntimeCStringAt410220(RequireBody(body).nameAt04, name.text);
            body->stringsAt08.RebuildAt412870(stringsCount, strings);
            for (std::uint32_t item = 0; item < stringsCount; ++item)
            {
                auto* const target = RequireBody(body).stringsAt08.Resolve(item);
                if (version < 5.0) ReadPatternText8(cursor, target, pattern.legacyByte00, strings);
                else ReadLegacyMemberText16At41E66D(cursor, target);
                body = reinterpret_cast<ParamsPatternFileGroupNative*>(generic.slot.baseAt00);
            }
            // 41E7C7/41E7CA：control保留创建值，base在所有成员完成后重取。
            base = generic.slot.baseAt00;
        }
        control = AppendAt41EEA4(pattern.itemsAt04, generic.slot, base, control, memory);
        generic.FinishCached(control);
        name.Finish();
    }
}

void ReadIntegers(ParamsRuntimeReadCursor& cursor, ParamsPatternNative& pattern, IEnvironmentRootMemory& memory)
{
    ClearEnvironmentArchiveReferencesAt498312(pattern.integersAt10);
    const auto count = Count(cursor);
    for (std::int32_t index = 0; index < count; ++index)
    {
        const auto wireCount = cursor.U8();
        LocalReference owner;
        auto* const created = CreateParamsPatternOuterIntegersAt41EF45(owner.slot, wireCount, memory);
        auto base = reinterpret_cast<std::uintptr_t>(created);
        auto* const control = owner.slot.controlAt08;
        for (std::int32_t n = 0; n < std::bit_cast<std::int32_t>(control ?
            static_cast<EnvironmentObjectReferenceControl*>(control)->countAt10 : 0u); ++n)
        {
            // 41EFE6..41F015：先按缓存base/ESI解目标，读DWORD后重取base，最后写旧目标。
            Slot identity{base, 0, control};
            auto* const target = ResolveParamsLegacyInteger(identity, static_cast<std::uint32_t>(n));
            const auto value = std::bit_cast<std::int32_t>(cursor.U32());
            base = owner.slot.baseAt00;
            RequireBody(target) = value;
        }
        LocalReference temporary;
        Retain(temporary.slot, base, 0, control);
        // 41F047快路独立重读owner.base；慢路则读取41F01C已经捕获的临时Ref。
        auto* const temporaryControl = AppendAt41EEA4(pattern.integersAt10, temporary.slot,
            owner.slot.baseAt00, control, memory);
        temporary.FinishCached(temporaryControl);
        owner.FinishCached(control);
    }
}

ParamsPatternGroupNative* CachedGroup(std::uintptr_t base, Control* control, std::uint32_t bias) noexcept
{
    const Slot identity{base, bias, control};
    return ResolveEnvironmentObjectReference<ParamsPatternGroupNative>(identity);
}

void ReadGroups(ParamsRuntimeReadCursor& cursor, ParamsPatternReferenceVector& table,
    bool second, double version, const std::uint8_t& key,
    IEnvironmentRootMemory& memory, IRuntimeStringManagerSource& strings)
{
    ClearEnvironmentArchiveReferencesAt498312(table);
    LocalReference owner;
    const auto count = Count(cursor);
    for (std::int32_t index = 0; index < count; ++index)
    {
        auto* const created = second ? RebuildParamsPatternGroupAt412490(owner.slot, strings, memory) :
            CreateParamsPatternFirstGroupAt41F163(owner.slot, strings, memory);
        auto base = second ? owner.slot.baseAt00 : reinterpret_cast<std::uintptr_t>(created);
        auto* const control = owner.slot.controlAt08;
        const auto body = [&]() noexcept
        {
            return second ? CachedGroup(base, control, owner.slot.biasAt04) : created;
        };
        auto* const nameBody = body();
        const bool narrowNonempty = ReadPatternLegacyText(cursor,
            nameBody ? &nameBody->nameAt00 : nullptr, version, key, strings);
        // 41F5F2只在第二表非空窄串的正常Free后重取base，零长及宽串继续使用缓存ESI。
        if (second && narrowNonempty) base = owner.slot.baseAt00;
        // 41F2FA/41F636：5.6开始U16，之前U8；两张组表使用同一门槛。
        const auto wireCount = version < 5.6 ? static_cast<std::uint16_t>(cursor.U8()) : cursor.U16();
        if (wireCount)
        {
            auto& indices = RequireBody(body()).indicesAt04;
            static_cast<void>(RebuildParamsPatternGroupIndices(indices, wireCount, memory));
            for (std::int32_t n = 0;
                n < std::bit_cast<std::int32_t>(ParamsLegacyTableCount(RequireBody(body()).indicesAt04)); ++n)
            {
                auto* target = ResolveParamsLegacyInteger(RequireBody(body()).indicesAt04, static_cast<std::uint32_t>(n));
                RequireBody(target) = std::bit_cast<std::int32_t>(cursor.U32());
            }
        }
        LocalReference temporary;
        // 第一表用创建body/control；第二表临时拷当前base/bias，但快路仍用缓存base。
        Retain(temporary.slot, second ? owner.slot.baseAt00 : base, second ? owner.slot.biasAt04 : 0, control);
        static_cast<void>(AppendAt41EEA4(table, temporary.slot, base, control, memory));
        temporary.FinishCurrent();
        // owner直到下轮factory入口或循环末才释放，不提前移动生命周期。
    }
    owner.FinishCurrent();
}
}

void ReadParamsPatternNativeAt41DFD0(ParamsRuntimeReadCursor& cursor,
    ParamsPatternNative& pattern, double version, IEnvironmentRootMemory& memory,
    IRuntimeStringManagerSource& strings)
{
    // 版本由434940识别；此函数按原比较分流，不增加原入口没有的二次版本门禁。
    // 原EDI属于本次reader，只有正常返回后434940才累加消费量；对象前缀不随游标回滚。
    auto local = cursor;
    // 41E014..41E021：旧key先发布，再清items；>=5完全不改首DWORD。
    if (version < 5.0) pattern.legacyByte00 = local.U8();
    if (version < 5.7) ReadLegacyItems(local, pattern, version, memory, strings);
    else ReadItems(local, pattern, memory, strings);
    ReadIntegers(local, pattern, memory);
    ReadGroups(local, pattern.firstGroupsAt1C, false, version, pattern.legacyByte00, memory, strings);
    ReadGroups(local, pattern.secondGroupsAt28, true, version, pattern.legacyByte00, memory, strings);
    cursor.CommitOffsetAfter419EB0(static_cast<std::uint32_t>(local.Offset()));
}
}

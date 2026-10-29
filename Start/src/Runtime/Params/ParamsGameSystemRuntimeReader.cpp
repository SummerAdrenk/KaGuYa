#include <Runtime/ParamsGameSystemRuntimeReader.h>
#include <Runtime/ParamsRuntimeReadCursor.h>
#include <Runtime/ParamsSettingTagReader.h>
#include <Runtime/ParamsDemoReader.h>
#include <Runtime/ParamsLegacyTables.h>
#include <Runtime/RuntimeCStringVector.h>

#include <bit>
#include <cstdlib>
#include <cstring>

namespace KaGuYa::Start::Runtime
{
namespace
{
// Start18 原 PE #bk/Start.exe，ImageBase=00400000；直接提取含终止零的窄串。
// VA 005AE0B8 / 文件偏移 001ACCB8。
char TypedStringError[] =
    "\x95\xB6\x8E\x9A\x97\xF1\x83\x66\x81\x5B\x83\x5E\x82\xC5\x82\xCD"
    "\x82\xC8\x82\xA2\x81\x49";
// VA 005AE0D0 / 文件偏移 001ACCD0。
char TypedIntegerError[] =
    "\x90\x94\x92\x6C\x83\x66\x81\x5B\x83\x5E\x82\xC5\x82\xCD\x82\xC8\x82\xA2\x81\x49";
// VA 005AE0E8 / 文件偏移 001ACCE8。
char TypedPointError[] =
    "\x8D\xC0\x95\x57\x92\x6C\x83\x66\x81\x5B\x83\x5E\x82\xC5\x82\xCD"
    "\x82\xC8\x82\xA2\x81\x49";

[[nodiscard]] std::int32_t SignedCount(const std::uint32_t count) noexcept
{
    return std::bit_cast<std::int32_t>(count);
}

template<class T>
T& LegacyBody(T* body)
{
    // 在原首次解址处拒绝空正文，不回滚此前已发布的槽。
    if (!body) _invalid_parameter_noinfo_noreturn();
    return *body;
}

void ReadLegacyInlineText(ParamsRuntimeReadCursor& cursor, RuntimeCString* destination,
    const double version, const std::uint8_t& key, const bool decrypt,
    const bool releaseCurrentWideReference = false)
{
    if (version < 5.0)
    {
        // 41B148/41B755/41BAFD/41BEB4/41C21A/41C59B/41C808/41CAC3:
        // 与4207F0同序：缓存源、417360、40B490、410220、撤清理资格、Free。
        // 安装项和第二串表按原分支不做XOR。
        cursor.Text8At4207F0(destination, key, decrypt);
    }
    else if (!destination)
    {
        // 零长度不访问目标；非空字段到4102D0的实际空目标边界才拒绝。
        cursor.Text16(destination);
    }
    else if (releaseCurrentWideReference)
    {
        // 41C99D/41CC0A与41D33B正常尾均调用40CF00，按当前Ref退休。
        cursor.NestedText16At41D294(*destination);
    }
    else
    {
        // 41B8BB/41BC8C/41C036缓存control/bias、重读base；正常退缓存control，异常退当前Ref。
        cursor.SettingText16At420045(*destination);
    }
}

template<class Body, class Resolve>
void ReadLegacyStrings(ParamsRuntimeReadCursor& cursor, Resolve resolve,
    RuntimeCStringArrayReference Body::* member, const double version,
    const std::uint8_t& key, const bool decrypt, const bool releaseCurrentWideReference = false)
{
    const auto count = cursor.U8();
    if (!count) return;
    const auto array = [&]() -> RuntimeCStringArrayReference& { return LegacyBody(resolve()).*member; };
    // 41B943/41BD1C/41C65E/41CA01复用既有412870数组构造协议。
    array().RebuildAt412870(count);
    for (std::uint32_t index = 0; SignedCount(index) <
        SignedCount(static_cast<std::uint32_t>(array().Size())); ++index)
    {
        // 每轮重取父Ref和子control.count，不缓存输入数量作为后续循环界限。
        auto* const destination = array().Resolve(index);
        ReadLegacyInlineText(cursor, destination, version, key, decrypt, releaseCurrentWideReference);
    }
}

void ReadLegacyVoice(ParamsRuntimeReadCursor& cursor, ParamsGameSystem& gameSystem, const double version)
{
    const auto count = cursor.U8(); // 41B575：零数量不改旧Ref。
    if (!count) return;
    auto& reference = gameSystem.legacyVoiceAt58;
    static_cast<void>(RebuildParamsLegacyVoiceAt41B581(reference, count));
    for (std::uint32_t index = 0; SignedCount(index) < SignedCount(ParamsLegacyTableCount(reference)); ++index)
    {
        auto* const valueTarget = ResolveParamsLegacyVoice(reference, index);
        const auto value = cursor.U8();
        LegacyBody(valueTarget).valueAt00 = value;
        const auto resolve = [&]() { return ResolveParamsLegacyVoice(reference, index); };
        auto* const nameTarget = resolve();
        ReadLegacyInlineText(cursor, nameTarget ? &nameTarget->nameAt04 : nullptr,
            version, gameSystem.legacyModeAt00, true);
        ReadLegacyStrings(cursor, resolve, &ParamsLegacyVoiceRuntime::stringsAt08,
            version, gameSystem.legacyModeAt00, true); // 41BAEA: XOR.
        ReadLegacyStrings(cursor, resolve, &ParamsLegacyVoiceRuntime::stringsAt14,
            version, gameSystem.legacyModeAt00, false); // 41BEAE: no XOR.
    }
}

void ReadLegacyMulti(ParamsRuntimeReadCursor& cursor, ParamsGameSystem& gameSystem, const double version)
{
    const auto count = cursor.U8();
    if (!count) return;
    auto& reference = gameSystem.legacyMultiAt64;
    static_cast<void>(RebuildParamsLegacyMultiAt41C0A6(reference, count));
    for (std::uint32_t index = 0; SignedCount(index) < SignedCount(ParamsLegacyTableCount(reference)); ++index)
    {
        auto* const nameTarget = ResolveParamsLegacyMulti(reference, index);
        if (version < 5.0)
            cursor.Text8At4207F0(nameTarget ? &nameTarget->nameAt00 : nullptr, gameSystem.legacyModeAt00, true);
        else
            cursor.Text16(nameTarget ? &nameTarget->nameAt00 : nullptr); // 41C297: actual 4209B0 call.
        const auto integerCount = cursor.U8();
        if (!integerCount) continue;
        const auto integers = [&]() -> YtlReferenceSlotRuntime&
            { return LegacyBody(ResolveParamsLegacyMulti(reference, index)).integersAt04; };
        static_cast<void>(RebuildParamsLegacyIntegersAt41C2DE(integers(), integerCount));
        for (std::uint32_t item = 0; SignedCount(item) < SignedCount(ParamsLegacyTableCount(integers())); ++item)
        {
            auto* const destination = ResolveParamsLegacyInteger(integers(), item);
            // 41C3EC：线序只消费一个byte，零扩展写入既有DWORD数组。
            const auto value = cursor.U8();
            LegacyBody(destination) = value;
        }
    }
}

void ReadLegacySound(ParamsRuntimeReadCursor& cursor, ParamsGameSystem& gameSystem, const double version)
{
    const auto count = cursor.U8();
    if (!count) return;
    auto& reference = gameSystem.legacySoundAt70;
    static_cast<void>(RebuildParamsLegacySoundAt41C420(reference, count));
    for (std::uint32_t index = 0; SignedCount(index) < SignedCount(ParamsLegacyTableCount(reference)); ++index)
    {
        const auto resolve = [&]() { return ResolveParamsLegacySound(reference, index); };
        auto* const nameTarget = resolve();
        if (version < 5.0)
            cursor.Text8At4207F0(nameTarget ? &nameTarget->nameAt00 : nullptr, gameSystem.legacyModeAt00, true);
        else
            cursor.Text16(nameTarget ? &nameTarget->nameAt00 : nullptr); // 41C614: actual 4209B0 call.
        ReadLegacyStrings(cursor, resolve, &ParamsLegacySoundRuntime::stringsAt04,
            version, gameSystem.legacyModeAt00, true, true); // 41C7EC: XOR.
        ReadLegacyStrings(cursor, resolve, &ParamsLegacySoundRuntime::stringsAt10,
            version, gameSystem.legacyModeAt00, false, true); // 41CAC0: no XOR.
    }
}

[[nodiscard]] std::uint32_t ReadTypedInteger(ParamsRuntimeReadCursor& cursor)
{
    // 418080：错型通过 __TI2PAD 抛 char*，不是 std::exception。
    if (cursor.U32() != 1)
    {
        throw TypedIntegerError;
    }
    return cursor.U32();
}

class ThumbnailReferenceScope final
{
public:
    explicit ThumbnailReferenceScope(YtlReferenceSlotRuntime& reference) noexcept : reference_(reference) {}
    ~ThumbnailReferenceScope() noexcept { if (active_) ReleaseParamsThumbnailAt43B9D0(reference_); }
    void ReleaseNormal() noexcept
    {
        active_ = false;
        ReleaseParamsThumbnailAt43B9D0(reference_);
    }
private:
    YtlReferenceSlotRuntime& reference_;
    bool active_ = true;
};

class CachedTextScope final
{
public:
    explicit CachedTextScope(IRuntimeStringManagerSource& strings)
        { InitializeEmptyRuntimeCString(text_.value, strings); }
    ~CachedTextScope() noexcept
        { if (active_) DestroyRuntimeCStringAt40CD40(&text_.value); }
    RuntimeCString& Value() noexcept { return text_.value; }
    void ReleaseCached(const char* characters)
    {
        // 41D702/41D838先撤掉Str1清理资格，再释放缓存header；Free抛错不可重试。
        active_ = false;
        ReleaseRuntimeCStringHeader(reinterpret_cast<ATL::CStringData*>(
            reinterpret_cast<std::uintptr_t>(characters) - sizeof(ATL::CStringData)));
    }
private:
    union Cell { RuntimeCString value; Cell() noexcept {} ~Cell() noexcept {} } text_;
    bool active_ = true;
};

void ReadThumbnails(ParamsRuntimeReadCursor& cursor, ParamsThumbnailTable& thumbnails,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    // 41D411..41D434：清表早于单位计数，保留capacity；未完成正文不入表。
    thumbnails.clear();
    const auto units = SignedCount(cursor.U32());
    for (std::uint32_t used = 0; SignedCount(used) < units; used += 11u)
    {
        YtlReferenceSlotRuntime reference;
        // 41D52F之前没有临时Ref清理资格，factory失败不额外回收裸正文。
        CreateParamsThumbnailAt41D463(reference, strings, memory);
        auto* const body = reinterpret_cast<ParamsThumbnailRuntime*>(reference.baseAt00);
        auto* const control = reference.controlAt08;
        ThumbnailReferenceScope owner(reference);
        if (cursor.U32() != 0) throw TypedStringError;
        // 原null正文到437680才硬故障；不让空正文掩盖更早的错型分支。
        if (!body) _invalid_parameter_noinfo_noreturn();
        SetRuntimeCStringAt437680(body->nameAt00, "", 0);
        cursor.Text16(body->nameAt00);
        CachedTextScope temporary(strings);
        const char* cachedCharacters = nullptr;
        for (std::uint32_t index = 0; index < 7; ++index)
        {
            if (cursor.U32() != 0) throw TypedStringError;
            SetRuntimeCStringAt437680(temporary.Value(), "", 0);
            cursor.Text16(temporary.Value());
            cachedCharacters = SetParamsThumbnailAliasAt41D590(*body, index, temporary.Value(), memory);
        }
        for (auto& integer : body->integersAt20)
            integer = ReadTypedInteger(cursor);
        // 41D6B3取缓存control，新增独立临时Ref；正常/展开都先释放它。
        YtlReferenceSlotRuntime appended{reinterpret_cast<std::uintptr_t>(body), 0, control};
        if (control) ++control->referenceCountAt04;
        ThumbnailReferenceScope appendedOwner(appended);
        thumbnails.AppendReference(appended, memory);
        appendedOwner.ReleaseNormal();
        temporary.ReleaseCached(cachedCharacters);
        owner.ReleaseNormal();
    }
}

template<void (*Release)(YtlReferenceSlotRuntime&)>
class RegistCgReferenceScope final
{
public:
    explicit RegistCgReferenceScope(YtlReferenceSlotRuntime& reference) noexcept : reference_(reference) {}
    ~RegistCgReferenceScope() noexcept { if (active_) Release(reference_); }
    void ReleaseNormal() noexcept
    {
        active_ = false;
        Release(reference_);
    }
private:
    YtlReferenceSlotRuntime& reference_;
    bool active_ = true;
};

void ReadRegistCgText(ParamsRuntimeReadCursor& cursor, RuntimeCString* text)
{
    // 41D95F/41DA5D：错误类型先于对空正文的访问；不让保护检查遮蔽原错型异常。
    if (cursor.U32() != 0) throw TypedStringError;
    if (!text) _invalid_parameter_noinfo_noreturn();
    SetRuntimeCStringAt437680(*text, "", 0);
    cursor.Text16(*text);
}

void ReadRegistCg(ParamsRuntimeReadCursor& cursor, ParamsRegistCgTable& groups,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    // 41D857..41D87A：先清 +104；单位累计按 DWORD 回绕、有符号比较，不校验精确总数。
    groups.clear();
    const auto units = SignedCount(cursor.U32());
    std::uint32_t used = 0;
    while (SignedCount(used) < units)
    {
        YtlReferenceSlotRuntime group;
        auto* const createdGroup = CreateParamsRegistCgGroupAt41D8A6(group, strings, memory);
        auto* const groupControl = group.controlAt08; // 原var_10，不能改为追加时重取control。
        RegistCgReferenceScope<ReleaseParamsRegistCgGroupAt43B950> groupOwner(group);
        ReadRegistCgText(cursor, reinterpret_cast<RuntimeCString*>(createdGroup));
        const auto itemCount = SignedCount(ReadTypedInteger(cursor));
        used += 2u;
        for (std::int32_t index = 0; index < itemCount; ++index)
        {
            YtlReferenceSlotRuntime item;
            auto* const body = CreateParamsRegistCgItemAt41D9C2(item, strings, memory);
            auto* const itemControl = item.controlAt08; // 原ESI跨文本转换/point读取保持。
            RegistCgReferenceScope<ReleaseParamsRegistCgItemAt43BA10> itemOwner(item);
            ReadRegistCgText(cursor, reinterpret_cast<RuntimeCString*>(body));
            if (cursor.U32() != 2)
            {
                throw TypedPointError;
            }
            body->xAt04 = cursor.U32();
            body->yAt08 = cursor.U32();
            body->valueAt0C = ReadTypedInteger(cursor);
            used += 3u;
            // 41DAD5重取局部group base，41DAFC取当前item base，control仍用缓存ESI。
            auto* const groupBody = reinterpret_cast<ParamsRegistCgGroupRuntime*>(group.baseAt00);
            YtlReferenceSlotRuntime appended{item.baseAt00, 0, itemControl};
            if (itemControl) ++itemControl->referenceCountAt04;
            RegistCgReferenceScope<ReleaseParamsRegistCgItemAt43BA10> appendedOwner(appended);
            groupBody->itemsAt04.AppendReference(appended, memory);
            appendedOwner.ReleaseNormal(); // 40DB40自身noexcept。
            itemOwner.ReleaseNormal(); // state28 -> 541E55，不能外传payload/删除异常。
        }
        YtlReferenceSlotRuntime appended{group.baseAt00, 0, groupControl};
        if (groupControl) ++groupControl->referenceCountAt04;
        RegistCgReferenceScope<ReleaseParamsRegistCgGroupAt43B950> appendedOwner(appended);
        groups.AppendReference(appended, memory);
        appendedOwner.ReleaseNormal();
        groupOwner.ReleaseNormal(); // 40D9F0：正常/展开均为终止边界。
    }
}

template<void (*Release)(YtlReferenceSlotRuntime&) noexcept>
class SceneReferenceScope final
{
public:
    explicit SceneReferenceScope(YtlReferenceSlotRuntime& reference, const bool active = true) noexcept
        : reference_(reference), active_(active) {}
    ~SceneReferenceScope() noexcept { if (active_) Release(reference_); }
    void Arm() noexcept { active_ = true; }
    void ReleaseNormal() noexcept { active_ = false; Release(reference_); }
private:
    YtlReferenceSlotRuntime& reference_;
    bool active_;
};

class SceneTextScope final
{
public:
    explicit SceneTextScope(IRuntimeStringManagerSource& strings)
        { InitializeEmptyRuntimeCString(text_.value, strings); }
    ~SceneTextScope() noexcept { if (active_) DestroyRuntimeCStringAt40CD40(&text_.value); }
    RuntimeCString& Value() noexcept { return text_.value; }
    void ReleaseNew()
    {
        // 41DD58 先回退到2B：正常Free抛出时，不重复释放这一个CString。
        active_ = false;
        DestroyRuntimeCStringAt40CD40(&text_.value);
    }
    void ReleaseOld()
    {
        // 41DED1 仍在2F：正常Free抛出，展开必须再次进入同一个CString析构。
        DestroyRuntimeCStringAt40CD40(&text_.value);
        active_ = false;
    }
private:
    union Cell { RuntimeCString value; Cell() noexcept {} ~Cell() noexcept {} } text_;
    bool active_ = true;
};

void ReadTypedNativeText(ParamsRuntimeReadCursor& cursor, RuntimeCString* text)
{
    // 418920成功返回后才add edi,eax。只复制借用/offset，仍晚读同一个输入owner；
    // 失败保留原游标，但清空目标、已写字符串等副作用不回滚。
    auto field = cursor;
    ReadRegistCgText(field, text);
    cursor = field;
}

std::uint32_t ReadSceneInteger(ParamsRuntimeReadCursor& cursor)
{
    auto field = cursor;
    const auto result = ReadTypedInteger(field);
    cursor = field; // 418080同样仅成功时提交外层EDI。
    return result;
}

void ReadSceneItemName(ParamsRuntimeReadCursor& cursor, const YtlReferenceSlotRuntime& item)
{
    auto* const body = ResolveParamsRegistSceneItemAt4105F0(item);
    // 原 null+0C 最终硬故障；类型错误仍先抛，不能提前遮住418920的校验。
    ReadTypedNativeText(cursor, body ? &body->nameAt0C : nullptr);
}

void AppendSceneSegment(const YtlReferenceSlotRuntime& item, const RuntimeCString& text,
    IEnvironmentRootMemory& memory)
{
    // 41DD49/41DE90：文字转换回调结束后，重新读取当前Ref的base/bias/stride。
    auto* const body = ResolveParamsRegistSceneItemAt4105F0(item);
    if (!body) _invalid_parameter_noinfo_noreturn();
    AppendRuntimeCStringAt41D5E9(body->scenesAt00, text, memory);
}

void AppendSceneItem(const YtlReferenceSlotRuntime& group, const YtlReferenceSlotRuntime& item,
    IEnvironmentRootMemory& memory)
{
    auto* const body = ResolveParamsRegistSceneGroupAt4105C0(group);
    if (!body) _invalid_parameter_noinfo_noreturn();
    body->itemsAt04.AppendReference(item, memory);
}

void ReadRegistScene(ParamsRuntimeReadCursor& cursor, ParamsRegistSceneTable& groups,
    const double version, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    groups.clear(); // 41DBF7：先清表，计数与后续失败不恢复旧表。
    const auto units = SignedCount(cursor.U32());
    if (units <= 0) return;
    const bool newFormat = version >= 5.8;
    std::uint32_t used = 0;
    while (SignedCount(used) < units)
    {
        YtlReferenceSlotRuntime group;
        CreateParamsRegistSceneGroupAt41DC40(group, strings, memory);
        SceneReferenceScope<ReleaseParamsRegistSceneGroupAt40DA60> groupOwner(group);
        auto* const groupBody = ResolveParamsRegistSceneGroupAt4105C0(group);
        ReadTypedNativeText(cursor, groupBody ? &groupBody->nameAt00 : nullptr);
        const auto itemCount = SignedCount(ReadSceneInteger(cursor));
        used += 2u;
        if (newFormat)
        {
            for (std::int32_t index = 0; index < itemCount; ++index)
            {
                YtlReferenceSlotRuntime item;
                CreateParamsRegistSceneItemAt41DCB0(item, strings, memory);
                SceneReferenceScope<ReleaseParamsRegistSceneItemAt40DBB0> itemOwner(item);
                ReadSceneItemName(cursor, item);
                const auto sceneCount = SignedCount(ReadSceneInteger(cursor));
                used += 2u;
                if (sceneCount > 0) used += static_cast<std::uint32_t>(sceneCount);
                for (std::int32_t scene = 0; scene < sceneCount; ++scene)
                {
                    SceneTextScope text(strings);
                    ReadTypedNativeText(cursor, &text.Value());
                    AppendSceneSegment(item, text.Value(), memory);
                    text.ReleaseNew();
                }
                AppendSceneItem(group, item, memory);
                itemOwner.ReleaseNormal();
            }
        }
        else
        {
            // 41DE3A：正itemCount一次性计入DWORD总量，不能加除尽或溢出拒绝规则。
            if (itemCount > 0) used += static_cast<std::uint32_t>(itemCount) * 2u;
            for (std::int32_t index = 0; index < itemCount; ++index)
            {
                YtlReferenceSlotRuntime item;
                CreateParamsRegistSceneItemAt41DCB0(item, strings, memory);
                SceneReferenceScope<ReleaseParamsRegistSceneItemAt40DBB0> itemOwner(item, false);
                // 41DE6A尚在2D：临时CString构造失败只清group，不能额外回滚item。
                SceneTextScope text(strings);
                itemOwner.Arm();
                ReadTypedNativeText(cursor, &text.Value());
                AppendSceneSegment(item, text.Value(), memory);
                ReadSceneItemName(cursor, item);
                AppendSceneItem(group, item, memory);
                text.ReleaseOld();
                itemOwner.ReleaseNormal();
            }
        }
        groups.AppendReference(group, memory);
        groupOwner.ReleaseNormal();
    }
}
}

void ReadParamsThumbnailsAt41D411(ParamsRuntimeReadCursor& cursor, ParamsThumbnailTable& thumbnails,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    ReadThumbnails(cursor, thumbnails, strings, memory);
}

void ReadParamsSceneNamesAt41D74C(ParamsRuntimeReadCursor& cursor, ParamsSceneNameTable& names,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    names.clear(); // 41D757的state22：范围释放失败终止；成功后重读begin设置end。
    const auto count = cursor.U32();
    if (count == 0) return;
    CachedTextScope temporary(strings); // 非零负数也走417360/GetNil；仅循环使用signed比较。
    for (std::int32_t index = 0; index < SignedCount(count); ++index)
    {
        // 41D7F3/41D7F6：tag及4209B0全部成功后才推进外层消耗量。
        ReadTypedNativeText(cursor, &temporary.Value());
        names.Append(temporary.Value(), memory);
    }
    // 41D832先捕获当前chars，41D838撤state23，正常Free抛错不再展开该临时串。
    temporary.ReleaseCached(temporary.Value().GetString());
}

void ReadParamsStartupBytesAt41AE52(ParamsRuntimeReadCursor& cursor, ParamsByteArray& bytes)
{
    // 数量先窥读；412700失败时仍停在数量字节，不撤销目标的部分发布。
    auto countCursor = cursor;
    const auto count = countCursor.U8();
    bytes.storage.AllocateAt412700(count);
    cursor = countCursor;
    for (std::uint32_t index = 0; SignedCount(index) < SignedCount(bytes.storage.Size()); ++index)
    {
        // 41AE89先解析当前目标，41AEA2才读取输入；下一轮重读control.count。
        auto* const target = bytes.Resolve(index);
        const auto value = cursor.U8();
        // U8已经消费，本项目标仍是读输入前缓存的指针。只验证，不改绑或跳过写入。
        std::span<std::uint8_t> writable;
        try { writable = BorrowBoundedByteArrayCopySpan(bytes.storage); }
        catch (const std::out_of_range&)
        {
            throw ParamsRuntimeException(ParamsRuntimeError::InvalidUnitCount, cursor.Offset(),
                "GameSystem startup byte has no writable range");
        }
        const auto address = reinterpret_cast<std::uintptr_t>(target);
        const auto begin = reinterpret_cast<std::uintptr_t>(writable.data());
        if (!target || address < begin || address - begin >= writable.size())
            throw ParamsRuntimeException(ParamsRuntimeError::InvalidUnitCount, cursor.Offset(),
                "GameSystem startup byte target is outside the current array extent");
        *target = static_cast<std::byte>(value);
    }
}

void ReadParamsRawBlobAt41CD56(ParamsRuntimeReadCursor& cursor, ParamsByteArray& bytes)
{
    const auto length = cursor.U32();
    if (!length) return;
    bytes.storage.AllocateAt412700(length);
    const auto bodyOffset = cursor.Offset();
    const auto input = cursor.Slice(bodyOffset, length);
    std::span<std::uint8_t> target;
    try { target = BorrowBoundedByteArrayCopySpan(bytes.storage); }
    catch (const std::out_of_range&)
    {
        throw ParamsRuntimeException(ParamsRuntimeError::InvalidUnitCount, bodyOffset,
            "GameSystem blob reference has no writable byte range");
    }
    // 原版没有容量形参。仅在实际memmove边界拒绝不可写区间，不裁剪或补零。
    if (length > target.size())
        throw ParamsRuntimeException(ParamsRuntimeError::InvalidUnitCount, bodyOffset,
            "GameSystem blob has no writable array extent");
    std::memmove(target.data(), input.data(), length);
    cursor.Seek(bodyOffset + length);
}

void ReadParamsStringListAt41D023(ParamsRuntimeReadCursor& cursor, ParamsCStringArray& strings,
    IRuntimeStringManagerSource& managers)
{
    const auto count = cursor.U32();
    if (!count) return;
    strings.storage.RebuildAt412870(SignedCount(count), managers);
    for (std::int32_t index = 0; index < SignedCount(count); ++index)
    {
        // 41D050..41D07F：先缓存本项目标，转换内部回调换槽只影响下一项。
        // 不取control.count作循环上限；plain S既无tag也不在读取前清空CString。
        auto* const target = strings.storage.Resolve(static_cast<std::uint32_t>(index));
        cursor.Text16(target);
    }
}

void ReadParamsPlacesAt41D08F(ParamsRuntimeReadCursor& cursor, ParamsPlaceArray& places,
    IRuntimeStringManagerSource& strings)
{
    const auto count = cursor.U32();
    if (!count) return;
    places.RebuildAt41D0B2(SignedCount(count), strings);
    for (std::int32_t index = 0; index < SignedCount(count); ++index)
    {
        auto* const nameTarget = places.Resolve(static_cast<std::uint32_t>(index));
        cursor.Text16(nameTarget ? &nameTarget->nameAt00 : nullptr);
        // 41D1A9..41D1DE：转换可改写Ref，value重新解址；输入读取先于目标写入。
        auto* const valueTarget = places.Resolve(static_cast<std::uint32_t>(index));
        const auto value = cursor.U32();
        if (!valueTarget) _invalid_parameter_noinfo_noreturn();
        valueTarget->valueAt04 = value;
    }
}

void ReadParamsRegistCgAt41D857(ParamsRuntimeReadCursor& cursor, ParamsRegistCgTable& groups,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    ReadRegistCg(cursor, groups, strings, memory);
}

void ReadParamsNestedListsAt41D21F(ParamsRuntimeReadCursor& cursor, ParamsNestedStringTable& lists,
    IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    const auto outerCount = SignedCount(cursor.U32());
    for (std::int32_t outer = 0; outer < outerCount; ++outer)
    {
        const auto innerCount = cursor.U32();
        if (!innerCount) continue; // 41D252 -> 41D4A3 -> 41D3EF：整组跳过，不追加空vector。
        CachedTextScope text(strings);
        struct InnerScope final
        {
            EnvironmentVectorStorage<RuntimeCString> storage{nullptr, nullptr, nullptr};
            IEnvironmentRootMemory& memory;
            bool active = true;
            ~InnerScope() noexcept
            {
                if (active) DestroyRuntimeCStringVectorAt40DD20(storage, memory);
            }
            void ReleaseNormal() noexcept
            {
                // 41D3BB的state1C终止边界：43A700失败不能再次释放本组或继续Free临时串。
                active = false;
                DestroyRuntimeCStringVectorAt40DD20(storage, memory);
            }
        } inner{{nullptr, nullptr, nullptr}, memory};
        for (std::int32_t index = 0; index < SignedCount(innerCount); ++index)
        {
            cursor.NestedText16At41D294(text.Value());
            AppendRuntimeCStringAt41D5E9(inner.storage, text.Value(), memory);
        }
        // 负signed innerCount仍复制追加空组；插入完成前失败不发布本组。
        lists.Append(inner.storage, memory);
        inner.ReleaseNormal();
        text.ReleaseCached(text.Value().GetString()); // 41D3CA先撤临时串清理资格，再Free。
    }
}

void ReadParamsRegistSceneAt41DBF7(ParamsRuntimeReadCursor& cursor, ParamsRegistSceneTable& groups,
    const double version, IRuntimeStringManagerSource& strings, IEnvironmentRootMemory& memory)
{
    ReadRegistScene(cursor, groups, version, strings, memory);
}

void ReadParamsTriplesAt41CCCE(ParamsRuntimeReadCursor& cursor, ParamsTripleTable& triples,
    std::uint32_t& rawCountMetadata, IEnvironmentRootMemory& memory)
{
    triples.clear();
    // 原41CCE7只加载低BYTE，其余三字节只跳过；完整U32是现代快照元数据。
    // 有限输入要求完整四字节，截断不等同原PE越界；clear副作用不回滚。
    rawCountMetadata = cursor.U32();
    const auto count = rawCountMetadata & 0xFFu;
    if (!count) return;
    triples.ResizeAt40ACB0(count, memory);
    for (std::uint32_t index = 0; index < count; ++index)
    {
        for (std::uint32_t component = 0; component < 3; ++component)
        {
            // 41CD0E/41CD24/41CD37先取当前目标，再加载本DWORD；不缓存整项引用。
            auto* const target = &triples[index][component];
            const auto value = cursor.U32();
            *target = value;
        }
    }
}

void ReadParamsGameSystemAt41ADE0(
    ParamsRuntimeReadCursor& cursor,
    ParamsGameSystem& gameSystem,
    const double version, std::uint32_t* const rawTripleCountMetadata)
{
    // 版本头由434940选择；41ADE0本身只比较版本，不另加原码没有的白名单异常。
    // 41AE25：v3之前跳过+04；各版本均有宽高和启动byte数组。
    if (version >= 3.0) gameSystem.versionMarker = cursor.U16();
    gameSystem.width = cursor.U32();
    gameSystem.height = cursor.U32();
    ReadParamsStartupBytesAt41AE52(cursor, gameSystem.startupConfigBytes);
    // 41AEDA..41AFD7：普通 S 的零长度不覆盖原字段。
    const auto scalarText = [&](RuntimeCString& text)
    {
        if (version < 5.0) cursor.Text8At4207F0(text, gameSystem.legacyModeAt00, false);
        else cursor.Text16(text);
    };
    scalarText(gameSystem.gameTitle.Native());
    scalarText(gameSystem.appendTitle.Native());
    scalarText(gameSystem.brand.Native());
    // 41AF4A/41AF56：仅等于v2时有+28，不是所有低于v3的值都有。
    if (version == 2.0) cursor.Text8At4207F0(gameSystem.legacyV2TextAt28.Native(), gameSystem.legacyModeAt00, false);
    gameSystem.defaultNameMode = cursor.U8();
    scalarText(gameSystem.defaultGivenName.Native());
    scalarText(gameSystem.defaultFamilyName.Native());

    const auto installCount = cursor.U8();
    if (installCount != 0)
    {
        // 41AFF2..41B0D6：+38 整个新数组先发布；count=0 不动旧表。
        gameSystem.installTable.RebuildAt41B025(installCount);
        for (std::uint32_t index = 0; SignedCount(index) <
            SignedCount(static_cast<std::uint32_t>(gameSystem.installTable.size())); ++index)
        {
            scalarText(gameSystem.installTable.At(index).file);
            // 41B386：文件名转换后重取当前数组，不能持有旧项跨过可分配的调用。
            scalarText(gameSystem.installTable.At(index).media);
        }
    }
    for (std::size_t index = 0; index < 3; ++index)
    {
        gameSystem.soundSlotCounts[index] = cursor.U32();
    }
    // 41B51A / 41B538：这两个字段先置零，再尝试读取。
    gameSystem.soundSlotCounts[3] = 0;
    if (version >= 5.2) gameSystem.soundSlotCounts[3] = cursor.U32();
    gameSystem.nameSelectionMode = 0;
    if (version >= 5.5) gameSystem.nameSelectionMode = cursor.U8();
    if (version < 5.2)
    {
        // 41B54D..41CC3C：旧三表占据SettingTags的位置；仅v5之前读取key。
        if (version < 5.0) gameSystem.legacyModeAt00 = cursor.U8();
        ReadLegacyVoice(cursor, gameSystem, version);
        ReadLegacyMulti(cursor, gameSystem, version);
        ReadLegacySound(cursor, gameSystem, version);
    }
    else
    {
        // 41CC65/41CC8A/41CCAC：+88、+7C、+94；缺席标志保留旧节点。
        for (std::size_t index = 0; index != gameSystem.settingTags.size(); ++index)
        {
            if (cursor.U8() != 0)
                ReadParamsSettingTagAt41FE80(cursor, gameSystem.settingTags[index].Native());
        }
    }

    std::uint32_t ignoredRawCount = 0;
    if (version >= 5.3)
        ReadParamsTriplesAt41CCCE(cursor, gameSystem.v53Triples,
            rawTripleCountMetadata ? *rawTripleCountMetadata : ignoredRawCount);
    ReadParamsRawBlobAt41CD56(cursor, gameSystem.rawBlob);
    if (version >= 5.2) ReadParamsDemoEntriesAt41CDB8(cursor, gameSystem.demos);
    if (version >= 5.1)
    {
        ReadParamsStringListAt41D023(cursor, gameSystem.stringList);
        ReadParamsPlacesAt41D08F(cursor, gameSystem.places);
    }

    if (version >= 5.4)
    {
        cursor.Text16(gameSystem.v54NestedListName.Native());
        ReadParamsNestedListsAt41D21F(cursor, gameSystem.v54NestedLists);
    }
    if (version >= 5.3)
    {
        ReadParamsThumbnailsAt41D411(cursor, gameSystem.thumbnails);
        ReadParamsSceneNamesAt41D74C(cursor, gameSystem.sceneNames);
        ReadParamsRegistCgAt41D857(cursor, gameSystem.registCg);
        ReadParamsRegistSceneAt41DBF7(cursor, gameSystem.registScene, version);
    }
    // 41DF15 返回局部消耗量；本接口通过 Cursor 表示当前位置，无本地 catch 或回滚。
}
}

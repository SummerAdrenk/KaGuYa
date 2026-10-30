#include <Runtime/MessageWindowNativeConfig.h>

#include <Runtime/EnvironmentArchiveRegistration.h>
#include <Runtime/EnvironmentStartupRuntime.h>
#include <Runtime/ParamsGameSystemRuntime.h>
#include <Runtime/ParamsRuntimeRoot.h>
#include <Runtime/ProcessEnvironmentRuntime.h>
#include <Runtime/SaveDataEnvironmentArchive.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

#include <bit>
#include <cstring>
#include <mbstring.h>
#include <new>

namespace KaGuYa::Start::Runtime
{
namespace
{
thread_local EnvironmentRootOwner* currentEnvironment = nullptr;

constexpr char Type1Section[] = "\x83\x65\x83\x4C\x83\x58\x83\x67";
constexpr char Type2Section[] = "\x83\x65\x83\x4C\x83\x58\x83\x67\x82\x51";
constexpr char ColorKey[] = "\x94\x77\x8C\x69\x90\x46";
constexpr char TransparencyKey[] = "\x93\xA7\x89\xDF\x97\xA6";
constexpr char MissingChild[] = "%s \x83\x5E\x83\x4F\x82\xC9 %s "
    "\x8D\x80\x96\xDA\x82\xCD\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49";
constexpr char MissingValue[] = "%s \x83\x5E\x83\x4F\x82\xC9 %s "
    "\x91\xAE\x90\xAB\x82\xCD\x82\xA0\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x49";
char SettingError[] = "error !";

const unsigned char* Bytes(const char* value) noexcept
{
    return reinterpret_cast<const unsigned char*>(value);
}
}

// 477E90/47D470 的 raw 边界：不添加 RequireSettingNode 或分配独立 locale。
YtlReferenceSlotRuntime CaptureSettingChildNativeAt477E90(RuntimeCString* error,
    const YtlReferenceSlotRuntime& parent, const char* name)
{
    SetRuntimeCStringAt437680(*error, "", 0);
    auto* cursor = ResolveParamsSettingTagAt410560(parent)->childrenAt10.begin;
    while (cursor != ResolveParamsSettingTagAt410560(parent)->childrenAt10.end)
    {
        auto* child = ResolveParamsSettingTagAt410560(*cursor);
        if (!name) AtlThrow(E_FAIL);
        if (_mbscmp(Bytes(child->nameAt00.GetString()), Bytes(name)) == 0)
        {
            auto result = *cursor;
            if (result.controlAt08) ++result.controlAt08->referenceCountAt04;
            return result;
        }
        ++cursor;
    }
    FormatRuntimeCStringAt417130(*error, MissingChild,
        ResolveParamsSettingTagAt410560(parent)->nameAt00.GetString(), name);
    throw SettingError;
}

namespace
{
void ReadValue(RuntimeCString& out, RuntimeCString* error,
    const YtlReferenceSlotRuntime& node, const char* key)
{
    SetRuntimeCStringAt437680(*error, "", 0);
    auto* cursor = ResolveParamsSettingTagAt410560(node)->pairsAt04.begin;
    while (cursor != ResolveParamsSettingTagAt410560(node)->pairsAt04.end)
    {
        if (!key) AtlThrow(E_FAIL);
        if (_mbscmp(Bytes(cursor->keyAt00.GetString()), Bytes(key)) == 0)
        {
            auto* header = reinterpret_cast<ATL::CStringData*>(
                const_cast<char*>(cursor->valueAt04.GetString())) - 1;
            const auto* chars = static_cast<const char*>(CloneRuntimeCStringDataAt414540(header)->data());
            // Clone返回的引用以原生指针写入输出槽，不触发CString赋值。
            std::memcpy(static_cast<void*>(&out), &chars, sizeof(chars));
            return;
        }
        ++cursor;
    }
    FormatRuntimeCStringAt417130(*error, MissingValue,
        ResolveParamsSettingTagAt410560(node)->nameAt00.GetString(), key);
    throw SettingError;
}

class LocalTag final
{
public:
    explicit LocalTag(YtlReferenceSlotRuntime value) noexcept : slot(value) {}
    ~LocalTag() noexcept { if (armed) ReleaseYtlReference(slot); }
    void ReleaseClearing() noexcept
    {
        armed = false;
        ReleaseYtlReference(slot);
    }
    void ReleaseWithoutClearing(bool cached) noexcept
    {
        armed = false;
        auto* control = slot.controlAt08;
        if (!control || --control->referenceCountAt04 != 0) return;
        if (control->owningAt08) control->DestroyPayloadAtVslot00();
        // 479C1F 固定 ESI；459F46/45A1C5 则重读局部 control，并允许空。
        if (!cached) control = slot.controlAt08;
        if (control) control->DeleteControlAtVslot04(1);
    }
    YtlReferenceSlotRuntime slot;
private:
    bool armed = true;
};

class LocalName final
{
public:
    LocalName() noexcept {}
    ~LocalName() noexcept { if (armed) DestroyRuntimeCStringAt40CD40(&value); }
    void ReleaseNormal()
    {
        // 459E93/45A119 已撤销 name EH 资格；Free 抛出由当前颜色 catch 接收，不重试。
        armed = false;
        DestroyRuntimeCStringAt40CD40(&value);
    }
    union { RuntimeCString value; };
    bool armed = false;
};

template<MessageWindowConfigType Type>
class ConfigControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ConfigControl(MessageWindowNativeConfig14h* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() noexcept override
    {
        // 493003/493083 只取一次 payload；正文回调换 control.payload 不改变此次 delete。
        auto* body = static_cast<MessageWindowNativeConfig14h*>(payloadAt14);
        if (!body) return;
        DestroyMessageWindowNativeConfig(*body);
        ::operator delete(body, sizeof(MessageWindowNativeConfig14h));
    }
    void DeleteControlAtVslot04(std::uint32_t flags) override
    {
        RetireEnvironmentObjectControl(this, flags);
    }
};
class ColorControl final : public EnvironmentObjectReferenceControl
{
public:
    explicit ColorControl(void* body) noexcept
        : EnvironmentObjectReferenceControl(body, UninitializedFieldsTag{}) {}
    void DestroyPayloadAtVslot00() override { ::operator delete(payloadAt14, 4); }
    void DeleteControlAtVslot04(std::uint32_t flags) override
        { RetireEnvironmentObjectControl(this, flags); }
};
static_assert(sizeof(ConfigControl<MessageWindowConfigType::Type1>) == 0x18);
static_assert(sizeof(ConfigControl<MessageWindowConfigType::Type2>) == 0x18);
static_assert(sizeof(ColorControl) == 0x18);

void PublishControl(YtlReferenceSlotRuntime& slot, EnvironmentObjectReferenceControl* control,
    std::uint32_t stride)
{
    slot.controlAt08 = control;
    control->referenceCountAt04 = 1;
    slot.controlAt08->owningAt08 = 1;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->strideAt0C = stride;
    static_cast<EnvironmentObjectReferenceControl*>(slot.controlAt08)->countAt10 = 1;
}

std::uint32_t ReadSave(EnvironmentRootOwner* explicitEnvironment, std::size_t offset)
{
    auto& environment = explicitEnvironment ? *explicitEnvironment : currentEnvironment ?
        *currentEnvironment : ProcessEnvironmentStartupRuntime().RootSource();
    auto* root = environment.CaptureAt4180C0();
    auto* save = ResolveEnvironmentArchiveAt417990(*root, 1);
    std::uint32_t value;
    std::memcpy(&value, reinterpret_cast<const std::byte*>(save) + offset, sizeof(value));
    return value;
}

MessageWindowColors* ReadConfig(MessageWindowNativeConfig14h& body, MessageWindowColors* out,
    EnvironmentRootOwner* environment, std::size_t saveOffset)
{
    out->color = body.colorAt04.baseAt00 ? ResolveMessageWindowNativeColor(body.colorAt04)->valueAt00 :
        ReadSave(environment, saveOffset);
    // 47E06E/47E0DE 在输出 color 之后才读取当前 +10，允许输出别名改变此值。
    const auto transparency = body.transparencyAt10;
    out->transparency = transparency >= 0 ? transparency :
        std::bit_cast<std::int32_t>(ReadSave(environment, saveOffset + 4));
    return out;
}
MessageWindowColors* __fastcall ReadType1(MessageWindowNativeConfig14h* body, void*, MessageWindowColors* out)
{
    return ReadConfig(*body, out, nullptr, 0x40);
}
MessageWindowColors* __fastcall ReadType2(MessageWindowNativeConfig14h* body, void*, MessageWindowColors* out)
{
    return ReadConfig(*body, out, nullptr, 0x48);
}
const MessageWindowNativeConfigVtable Type1Table{ReadType1};
const MessageWindowNativeConfigVtable Type2Table{ReadType2};

void ReadColor(YtlReferenceSlotRuntime& config, MessageWindowNativeConfigServices& services,
    MessageWindowParamsReadPoint namePoint, MessageWindowParamsReadPoint definitionPoint, const char* section)
{
    const auto& game = services.params.ReadGameSystem(namePoint);
    auto* error = services.errors.CaptureAt47BCF0();
    LocalName name;
    LocalTag window(CaptureSettingChildNativeAt477E90(error, game.settingTags[2].Native(), section));
    ReadValue(name.value, error, window.slot, ColorKey);
    name.armed = true;
    window.ReleaseClearing();
    if (_mbscmp(Bytes(name.value.GetString()), Bytes("")) != 0)
    {
        const char* characters = name.value.GetString(); // 459DE0/45A066：跨下两次取源固定。
        const auto& definitions = services.params.ReadGameSystem(definitionPoint);
        error = services.errors.CaptureAt47BCF0();
        LocalTag definition(CaptureSettingChildNativeAt477E90(error, definitions.settingTags[1].Native(), characters));
        const auto color = services.expressions.ReadRgbAt4797C0(error, definition.slot, "Value");
        definition.ReleaseWithoutClearing(true);
        RebuildMessageWindowColorReferenceAt469EC0(ResolveMessageWindowNativeConfig(config)->colorAt04,
            services.memory);
        ResolveMessageWindowNativeColor(ResolveMessageWindowNativeConfig(config)->colorAt04)->valueAt00 = color;
    }
    name.ReleaseNormal();
}

std::int32_t ReadTransparency(MessageWindowNativeConfigServices& services,
    MessageWindowParamsReadPoint point, const char* section)
{
    const auto& game = services.params.ReadGameSystem(point);
    auto* error = services.errors.CaptureAt47BCF0();
    LocalTag window(CaptureSettingChildNativeAt477E90(error, game.settingTags[2].Native(), section));
    const auto value = services.expressions.ReadIntegerAt47BBE0(error, window.slot, TransparencyKey);
    window.ReleaseWithoutClearing(false);
    return value;
}
void ApplyTransparency(YtlReferenceSlotRuntime& config, MessageWindowNativeConfigServices& services,
    MessageWindowParamsReadPoint point, const char* section)
{
    std::int32_t percentage;
    try { percentage = ReadTransparency(services, point, section); }
    catch (...) { return; }
    // 459F5B/45A1DA 已恢复外层 state4；此处非法 config 不是局部缺项。
    if (percentage >= 0)
        ResolveMessageWindowNativeConfig(config)->transparencyAt10 =
            std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(percentage) << 8u) / 100;
}
}

const MessageWindowNativeConfigVtable& MessageWindowNativeConfigTable(MessageWindowConfigType type) noexcept
{
    return type == MessageWindowConfigType::Type1 ? Type1Table : Type2Table;
}
MessageWindowNativeConfig14h* ConstructMessageWindowNativeConfig(
    MessageWindowNativeConfig14h& body, MessageWindowConfigType type) noexcept
{
    // 到459B75/459C39才激活 union Ref；其三项DMI恰按base/bias/control清零。
    ::new (static_cast<void*>(&body.colorAt04)) YtlReferenceSlotRuntime;
    body.vptrAt00 = &MessageWindowNativeConfigTable(type);
    body.transparencyAt10 = -1;
    return &body;
}
MessageWindowNativeConfig14h* NewMessageWindowNativeConfig(MessageWindowConfigType type,
    IEnvironmentRootMemory& memory)
{
    void* raw = memory.AllocateBody(sizeof(MessageWindowNativeConfig14h));
    if (!raw) return nullptr;
    return ConstructMessageWindowNativeConfig(*::new (raw) MessageWindowNativeConfig14h, type);
}
void ReplaceMessageWindowNativeConfigReference(YtlReferenceSlotRuntime& slot,
    MessageWindowNativeConfig14h* completed, MessageWindowConfigType type, IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    if (!completed) return;
    // 459BC7/459C8B 的 ECX 已清零，459BD4/459C98 不重读 slot.control。
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(completed);
    void* raw = memory.AllocateBody(0x18);
    EnvironmentObjectReferenceControl* control = nullptr;
    if (raw)
    {
        if (type == MessageWindowConfigType::Type1)
            control = ::new (raw) ConfigControl<MessageWindowConfigType::Type1>(completed);
        else control = ::new (raw) ConfigControl<MessageWindowConfigType::Type2>(completed);
    }
    PublishControl(slot, control, 0x14);
}
void CreateAndReplaceMessageWindowNativeConfigReference(YtlReferenceSlotRuntime& slot,
    MessageWindowConfigType type, IEnvironmentRootMemory& memory)
{
    auto* body = NewMessageWindowNativeConfig(type, memory);
    ReplaceMessageWindowNativeConfigReference(slot, body, type, memory);
}
MessageWindowNativeConfig14h* ResolveMessageWindowNativeConfig(const YtlReferenceSlotRuntime& slot) noexcept
{
    return ResolveEnvironmentObjectReference<MessageWindowNativeConfig14h>(slot);
}
void DestroyMessageWindowNativeConfig(MessageWindowNativeConfig14h& body) noexcept
{
    ReleaseYtlReference(body.colorAt04);
}
void RebuildMessageWindowColorReferenceAt469EC0(YtlReferenceSlotRuntime& slot, IEnvironmentRootMemory& memory)
{
    ReleaseYtlReference(slot);
    void* rawColor = memory.AllocateBody(4);
    if (slot.controlAt08) slot.controlAt08->DeleteControlAtVslot04(1);
    slot.biasAt04 = 0;
    slot.baseAt00 = reinterpret_cast<std::uintptr_t>(rawColor);
    void* rawControl = memory.AllocateBody(0x18);
    auto* control = rawControl ? ::new (rawControl) ColorControl(rawColor) : nullptr;
    PublishControl(slot, control, 4);
}
MessageWindowNativeColor4h* ResolveMessageWindowNativeColor(const YtlReferenceSlotRuntime& slot) noexcept
{
    return ResolveEnvironmentObjectReference<MessageWindowNativeColor4h>(slot);
}
MessageWindowColors* ReadMessageWindowNativeConfigAt47E030(
    MessageWindowNativeConfig14h& body, MessageWindowColors* out, EnvironmentRootOwner& environment)
{
    return ReadConfig(body, out, &environment, 0x40);
}
MessageWindowColors* ReadMessageWindowNativeConfigAt47E0A0(
    MessageWindowNativeConfig14h& body, MessageWindowColors* out, EnvironmentRootOwner& environment)
{
    return ReadConfig(body, out, &environment, 0x48);
}
MessageWindowNativeConfigScope::MessageWindowNativeConfigScope(EnvironmentRootOwner& environment) noexcept
    : previous_(currentEnvironment) { currentEnvironment = &environment; }
MessageWindowNativeConfigScope::~MessageWindowNativeConfigScope() noexcept { currentEnvironment = previous_; }

const ParamsGameSystem& MessageWindowNativeParamsSource::ReadGameSystem(MessageWindowParamsReadPoint)
{
    return *owner_.CaptureAt418480()->gameSystemAt04.get();
}
std::uint32_t MessageWindowNativeConfigExpressionAdapter::ReadRgbAt4797C0(
    RuntimeCString* error, const YtlReferenceSlotRuntime& node, const char* key)
{
    return ReadSettingNodeRgbColorAt4797C0(error, node, key, parser_);
}
std::int32_t MessageWindowNativeConfigExpressionAdapter::ReadIntegerAt47BBE0(
    RuntimeCString* error, const YtlReferenceSlotRuntime& node, const char* key)
{
    return ReadSettingNodeIntegerAt47BBE0(error, node, key, parser_);
}
void ApplyMessageWindowNativeConfigOverrides(YtlReferenceSlotRuntime& type1,
    YtlReferenceSlotRuntime& type2, MessageWindowNativeConfigServices& services)
{
    using Point = MessageWindowParamsReadPoint;
    if (!services.params.ReadGameSystem(Point::WindowSettings).settingTags[2].Native().baseAt00) return;
    try { ReadColor(type1, services, Point::Type1ColorName, Point::Type1ColorDefinition, Type1Section); }
    catch (...) {}
    ApplyTransparency(type1, services, Point::Type1Transparency, Type1Section);
    try { ReadColor(type2, services, Point::Type2ColorName, Point::Type2ColorDefinition, Type2Section); }
    catch (...) {}
    ApplyTransparency(type2, services, Point::Type2Transparency, Type2Section);
}
}

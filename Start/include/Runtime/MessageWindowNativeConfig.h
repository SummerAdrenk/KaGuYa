#pragma once

#include <Runtime/EnvironmentObjectReference.h>
#include <Runtime/MessageWindowConfigRuntime.h>

#include <type_traits>

namespace KaGuYa::Start::Runtime
{
struct MessageWindowNativeConfig14h;
struct MessageWindowNativeConfigVtable final
{
    MessageWindowColors* (__fastcall* readAt00)(MessageWindowNativeConfig14h*, void*, MessageWindowColors*);
};
static_assert(sizeof(MessageWindowNativeConfigVtable) == 4);
static_assert(sizeof(MessageWindowColors) == 8);

// 459B75/459C39：唯一单槽表，无虚析构；空用户构造不提前初始化color Ref。
struct MessageWindowNativeConfig14h final
{
    const MessageWindowNativeConfigVtable* vptrAt00;
    union { YtlReferenceSlotRuntime colorAt04; };
    std::int32_t transparencyAt10;
    MessageWindowNativeConfig14h() noexcept {}
};
struct MessageWindowNativeColor4h final { std::uint32_t valueAt00; };
static_assert(sizeof(MessageWindowNativeConfig14h) == 0x14);
static_assert(offsetof(MessageWindowNativeConfig14h, colorAt04) == 4);
static_assert(offsetof(MessageWindowNativeConfig14h, transparencyAt10) == 0x10);
static_assert(std::is_standard_layout_v<MessageWindowNativeConfig14h>);
static_assert(std::is_trivially_destructible_v<MessageWindowNativeConfig14h>);
static_assert(sizeof(MessageWindowNativeColor4h) == 4);

const MessageWindowNativeConfigVtable& MessageWindowNativeConfigTable(MessageWindowConfigType type) noexcept;
MessageWindowNativeConfig14h* ConstructMessageWindowNativeConfig(
    MessageWindowNativeConfig14h& body, MessageWindowConfigType type) noexcept;
// 注入申请须兼容全局 sized delete；完成对象/control 不保存 memory 或额外释放上下文。
MessageWindowNativeConfig14h* NewMessageWindowNativeConfig(MessageWindowConfigType type,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
// 459B9B/459C5F：完成body后才退旧Ref；control失败不补删已发布14h。
void ReplaceMessageWindowNativeConfigReference(YtlReferenceSlotRuntime& slot,
    MessageWindowNativeConfig14h* completed, MessageWindowConfigType type,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
void CreateAndReplaceMessageWindowNativeConfigReference(YtlReferenceSlotRuntime& slot,
    MessageWindowConfigType type, IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
MessageWindowNativeConfig14h* ResolveMessageWindowNativeConfig(const YtlReferenceSlotRuntime& slot) noexcept;
// 492FE0/493060正文部分：仅退color，不写config表；无虚析构可供调用。
void DestroyMessageWindowNativeConfig(MessageWindowNativeConfig14h& body) noexcept;
void RebuildMessageWindowColorReferenceAt469EC0(YtlReferenceSlotRuntime& slot,
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory());
MessageWindowNativeColor4h* ResolveMessageWindowNativeColor(const YtlReferenceSlotRuntime& slot) noexcept;

MessageWindowColors* ReadMessageWindowNativeConfigAt47E030(
    MessageWindowNativeConfig14h& body, MessageWindowColors* out, EnvironmentRootOwner& environment);
MessageWindowColors* ReadMessageWindowNativeConfigAt47E0A0(
    MessageWindowNativeConfig14h& body, MessageWindowColors* out, EnvironmentRootOwner& environment);
// 只借用getter现场来源，支持嵌套；不捕获SaveData、不进入14h或control。
// 无scope的虚槽使用唯一ProcessEnvironmentStartupRuntime().RootSource()。
class MessageWindowNativeConfigScope final
{
public:
    explicit MessageWindowNativeConfigScope(EnvironmentRootOwner& environment) noexcept;
    ~MessageWindowNativeConfigScope() noexcept;
    MessageWindowNativeConfigScope(const MessageWindowNativeConfigScope&) = delete;
    MessageWindowNativeConfigScope& operator=(const MessageWindowNativeConfigScope&) = delete;
private:
    EnvironmentRootOwner* previous_;
};

class MessageWindowNativeParamsSource final : public IMessageWindowParamsSource
{
public:
    explicit MessageWindowNativeParamsSource(ParamsRuntimeRootOwner& owner) noexcept : owner_(owner) {}
    const ParamsGameSystem& ReadGameSystem(MessageWindowParamsReadPoint point) override;
private:
    ParamsRuntimeRootOwner& owner_;
};

// 4797C0/47BBE0内部表达式算法为显式边界；node是真实临时Ref，调用期借用。
class IMessageWindowNativeConfigExpressions
{
public:
    virtual ~IMessageWindowNativeConfigExpressions() = default;
    virtual std::uint32_t ReadRgbAt4797C0(RuntimeCString* error,
        const YtlReferenceSlotRuntime& node, const char* key) = 0;
    virtual std::int32_t ReadIntegerAt47BBE0(RuntimeCString* error,
        const YtlReferenceSlotRuntime& node, const char* key) = 0;
};
// 显式复用现代表达式核；不声称其string/locale分配、错误类型和内部EH已原生化。
class MessageWindowNativeConfigExpressionAdapter final : public IMessageWindowNativeConfigExpressions
{
public:
    explicit MessageWindowNativeConfigExpressionAdapter(IMessageWindowConfigExpressionParser& parser) noexcept
        : parser_(parser) {}
    std::uint32_t ReadRgbAt4797C0(RuntimeCString*, const YtlReferenceSlotRuntime&, const char*) override;
    std::int32_t ReadIntegerAt47BBE0(RuntimeCString*, const YtlReferenceSlotRuntime&, const char*) override;
private:
    IMessageWindowConfigExpressionParser& parser_;
};
struct MessageWindowNativeConfigServices final
{
    IMessageWindowParamsSource& params;
    ISettingTagErrorSource& errors;
    IMessageWindowNativeConfigExpressions& expressions;
    IEnvironmentRootMemory& memory = DefaultEnvironmentRootMemory();
};
// CMesWin与CTextWin共同的477E90；返回一份已增持的原始Ref，由原caller安排正常尾/EH。
YtlReferenceSlotRuntime CaptureSettingChildNativeAt477E90(RuntimeCString* error,
    const YtlReferenceSlotRuntime& parent, const char* name);
// 459CED在四段catch之外；只执行459D20..45A228前的四段原位提交，不接管外层Ref。
void ApplyMessageWindowNativeConfigOverrides(YtlReferenceSlotRuntime& type1,
    YtlReferenceSlotRuntime& type2, MessageWindowNativeConfigServices& services);
}

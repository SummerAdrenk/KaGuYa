#pragma once

#include <Runtime/SettingTagErrorRuntime.h>

#include <cstdint>
#include <optional>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
struct ParamsGameSystem;
struct YtlReferenceSlotRuntime;
class SaveDataEnvironmentArchive;
class ParamsRuntimeRootOwner;

enum class MessageWindowParamsReadPoint : std::uint32_t
{
    WindowSettings = 0x459CED,
    Type1ColorName = 0x459D24,
    Type1ColorDefinition = 0x459DE6,
    Type1Transparency = 0x459ECF,
    Type2ColorName = 0x459FAD,
    Type2ColorDefinition = 0x45A06C,
    Type2Transparency = 0x45A14E,
};

// 每个实际读点独立取源；返回值只用于同步查键，不借过 parser 回调，不持有整个 GameSystem。
// 命中子节点后只增持它的 native Ref；求值期间可换根，旧节点仍独立存活。
class IMessageWindowParamsSource
{
public:
    virtual ~IMessageWindowParamsSource() = default;
    [[nodiscard]] virtual const ParamsGameSystem& ReadGameSystem(MessageWindowParamsReadPoint point) = 0;
};

class MessageWindowParamsRootSource final : public IMessageWindowParamsSource
{
public:
    explicit MessageWindowParamsRootSource(ParamsRuntimeRootOwner& root) noexcept : root_(root) {}
    [[nodiscard]] const ParamsGameSystem& ReadGameSystem(MessageWindowParamsReadPoint point) override;
private:
    ParamsRuntimeRootOwner& root_;
};

struct MessageWindowColors final
{
    std::uint32_t color = 0;
    std::int32_t transparency = 0;

    bool operator==(const MessageWindowColors&) const = default;
};

class IMessageWindowColorConfig
{
public:
    virtual ~IMessageWindowColorConfig() = default;
    [[nodiscard]] virtual MessageWindowColors Read() const = 0;
};

// 45E770 去掉 ASCII 空格/Tab 后交给 484E40/48E330 的整数表达式边界。
// 具体实现是 MessageWindowConfigExpressionParser；此依赖必须显式提供，不使用 stoi。
// 参数仅在调用期间有效；返回原 32 位结果，语法错误必须抛出。
class IMessageWindowConfigExpressionParser
{
public:
    virtual ~IMessageWindowConfigExpressionParser() = default;
    [[nodiscard]] virtual std::int32_t Evaluate(
        std::string_view expressionCp932WithoutSpacesOrTabs) = 0;
};

// 477E90 -> 4797C0：直接读取色库 settingTags[1] 下指定子项的 Value。
// 精确 CP932 首匹配，复用 RGB 表达式解析；缺项/解析失败向调用方抛出。
[[nodiscard]] std::uint32_t ReadSettingRgbColorAt4797C0(
    RuntimeCString* errorCString, const ParamsGameSystem& params, const char* nameCp932,
    IMessageWindowConfigExpressionParser& parser);

// 已定位节点的 47D470 -> 4797C0/47BBE0 入口，不再从 params 根重复查找。
// key 按精确 CP932 首匹配；可为空键，不能含 NUL。借用真实 Ref 槽逐次 resolve。
// 47D470 返回 Clone CString 后才求值；调用方持有节点，不传 DTO/深快照树。
// 45E908/45E90B 写整数结果/type=0；45E910 的异常路径写 type=1。
// 47BC42、4799F2/479A34/479A76 均拒绝非零标记；不是允许浮点再转整数。
// 现有 parser 的 int32 返回/抛错承载这两条路径，复用原 EvaluateInteger/ParseRgb，
// 不合成成功值、不吞异常；现代异常类型沿用既有 parser，不伪造原 char* 错误对象。
[[nodiscard]] std::uint32_t ReadSettingNodeRgbColorAt4797C0(
    RuntimeCString* errorCString, const YtlReferenceSlotRuntime& node, std::string_view keyCp932,
    IMessageWindowConfigExpressionParser& parser);
[[nodiscard]] std::int32_t ReadSettingNodeIntegerAt47BBE0(
    RuntimeCString* errorCString, const YtlReferenceSlotRuntime& node, std::string_view keyCp932,
    IMessageWindowConfigExpressionParser& parser);

enum class MessageWindowConfigType : std::uint8_t
{
    Type1,
    Type2,
};

class MessageWindowConfigPairRuntime;
class MessageWindowConfigRuntime;
// 固定输入入口：即使外部 root 换代，也继续使用调用方保证存活的这个对象。
void ApplyMessageWindowConfigOverrides(const ParamsGameSystem& params,
    MessageWindowConfigRuntime& type1, MessageWindowConfigRuntime& type2,
    IMessageWindowConfigExpressionParser& parser,
    ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());
// 生产晚取入口；首次 459CED 在四个局部 catch 外，后六个条件读点在各自块内。
void ApplyMessageWindowConfigOverrides(ParamsRuntimeRootOwner& root,
    MessageWindowConfigRuntime& type1, MessageWindowConfigRuntime& type2,
    IMessageWindowConfigExpressionParser& parser,
    ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());
void ApplyMessageWindowConfigOverrides(IMessageWindowParamsSource& source,
    MessageWindowConfigRuntime& type1, MessageWindowConfigRuntime& type2,
    IMessageWindowConfigExpressionParser& parser,
    ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());

// 47E030/47E0A0：两个 override 分别决定是否读取同一存档对象的实时字段。
// save 必须比本对象长寿；这里不复制存档，不分配原 Ref12 控制块，也不钳制透明度。
class MessageWindowConfigRuntime final : public IMessageWindowColorConfig
{
public:
    MessageWindowConfigRuntime(
        const SaveDataEnvironmentArchive& save,
        MessageWindowConfigType type,
        std::optional<std::uint32_t> colorOverride = std::nullopt,
        std::int32_t transparencyOverride = -1);

    [[nodiscard]] MessageWindowColors Read() const noexcept override;
    [[nodiscard]] std::optional<std::uint32_t> ColorOverride() const noexcept;
    [[nodiscard]] std::int32_t TransparencyOverride() const noexcept;

private:
    friend void ApplyMessageWindowConfigOverrides(IMessageWindowParamsSource&,
        MessageWindowConfigRuntime&, MessageWindowConfigRuntime&,
        IMessageWindowConfigExpressionParser&, ISettingTagErrorSource&);

    const SaveDataEnvironmentArchive& save_;
    MessageWindowConfigType type_;
    std::optional<std::uint32_t> colorOverride_;
    std::int32_t transparencyOverride_ = -1;
};

// 459AB0 的配置子集：type1 色 -> type1 透过率 -> type2 色 -> type2 透过率。
// 四段独立 catch，缺项/求值异常只终止当前段；不实现 CMesWin 外层 catch 或资源构造。
// params/parser 只在构造期借用；save 在 Read 时继续读取，必须保持有效。
class MessageWindowConfigPairRuntime final
{
public:
    MessageWindowConfigPairRuntime(
        const ParamsGameSystem& params,
        const SaveDataEnvironmentArchive& save,
        IMessageWindowConfigExpressionParser& parser,
        ISettingTagErrorSource& errors = ProcessSettingTagErrorOwner());

    [[nodiscard]] const MessageWindowConfigRuntime& Type1() const noexcept;
    [[nodiscard]] const MessageWindowConfigRuntime& Type2() const noexcept;

private:
    MessageWindowConfigRuntime type1_;
    MessageWindowConfigRuntime type2_;
};
}

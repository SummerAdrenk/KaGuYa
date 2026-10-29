#pragma once

#include <Runtime/ParamsTextArrayRuntime.h>
#include <Runtime/ParamsGameSystemRuntime.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace KaGuYa::Start::Runtime
{
enum class ParamsRuntimeError : std::uint8_t
{
    EmptyInput,
    UnsupportedVersion,
    TruncatedInput,
    InvalidUtf16,
    Cp932ConversionFailed,
    InvalidTypedValue,
    InvalidDemoData,
    InvalidPatternKind,
    InvalidUnitCount,
    TrailingData,
};

class ParamsRuntimeException final : public std::runtime_error
{
public:
    ParamsRuntimeException(ParamsRuntimeError error, std::size_t offset, const char* message);

    [[nodiscard]] ParamsRuntimeError Error() const noexcept;
    [[nodiscard]] std::size_t Offset() const noexcept;

private:
    ParamsRuntimeError error_;
    std::size_t offset_;
};

// 严格导入与测试的显式值快照；运行时SettingTag只持原生Ref/node，不保存这些镜像。
struct ParamsSettingPair final
{
    bool operator==(const ParamsSettingPair&) const = default;
    ParamsText key;
    ParamsText value;
};

struct ParamsSettingTag final
{
    bool operator==(const ParamsSettingTag&) const = default;
    ParamsText name;
    std::vector<ParamsSettingPair> pairs;
    std::vector<ParamsSettingTag> children;
};

// 仅 strict/fixture 的临时 wire 值；生产 GS 不保留或消费这份 payload。
struct ParamsDemoCommand final
{
    bool operator==(const ParamsDemoCommand&) const = default;
    std::uint8_t type = 0;
    std::vector<std::byte> payload;
};

struct ParamsDemo final
{
    bool operator==(const ParamsDemo&) const = default;
    ParamsText name;
    std::vector<ParamsDemoCommand> commands;
};

struct ParamsPatternItem final
{
    bool operator==(const ParamsPatternItem&) const = default;
    ParamsText name;
    std::uint8_t kind = 0;
    ParamsTextArray strings;
    ParamsText subName;
    std::uint32_t value1 = 0;
    std::uint32_t value2 = 0;
};

struct ParamsPatternGroup final
{
    bool operator==(const ParamsPatternGroup&) const = default;
    ParamsText name;
    std::vector<std::uint32_t> indices;
};

struct ParamsPattern final
{
    bool operator==(const ParamsPattern&) const = default;
    std::vector<ParamsPatternItem> items;
    std::vector<std::vector<std::uint32_t>> integerArrays;
    std::array<std::vector<ParamsPatternGroup>, 2> groupTables;
};

struct ParamsSceneLabel final
{
    bool operator==(const ParamsSceneLabel&) const = default;
    ParamsText name;
    std::uint32_t value1 = 0;
    std::uint32_t value2 = 0;
};

struct ParamsRuntimeProfile final
{
    bool operator==(const ParamsRuntimeProfile&) const = default;
    std::string header;
    double version = -1.0;
    ParamsGameSystem gameSystem;
    ParamsPattern pattern;
    std::vector<ParamsSceneLabel> sceneLabels;
    // 现代wire诊断，原11Ch正文只消费数量低BYTE；不占用原对象padding。
    std::uint32_t v53TripleRawCount = 0;
};

class ParamsRuntimeProfileParser final
{
public:
    // 严格验证入口：仅 v05.8、固定 CP932 并强制 EOF；不是原 434940 的原位读取契约。
    [[nodiscard]] static ParamsRuntimeProfile Parse(std::span<const std::byte> bytes);
};
}

#pragma once

#include <Runtime/ParamsPatternNative.h>
#include <Runtime/ParamsPatternItemNative.h>
#include <Runtime/PatternResourceAccess.h>

namespace KaGuYa::Start::Runtime
{
struct ParamsPattern;

[[nodiscard]] std::size_t ParamsPatternVectorSize(const ParamsPatternReferenceVector& table) noexcept;
[[nodiscard]] const YtlReferenceSlotRuntime& ParamsPatternReferenceAt(
    const ParamsPatternReferenceVector& table, std::size_t index);
[[nodiscard]] std::uint32_t ParamsPatternItemKindAt(const ParamsPatternNative& pattern, std::size_t index);
[[nodiscard]] const RuntimeCString& ParamsPatternItemNameAt(const ParamsPatternNative& pattern, std::size_t index);
[[nodiscard]] const ParamsPatternFileGroupNative& ParamsPatternFileGroupAt(const ParamsPatternNative& pattern, std::size_t index);
[[nodiscard]] const ParamsPatternExcPositionNative& ParamsPatternExcPositionAt(const ParamsPatternNative& pattern, std::size_t index);
[[nodiscard]] const ParamsPatternFileConvertNative& ParamsPatternFileConvertAt(const ParamsPatternNative& pattern, std::size_t index);
[[nodiscard]] const ParamsPatternGroupNative& ParamsPatternGroupAt(const ParamsPatternReferenceVector& table, std::size_t index);

// 4AD900/47A020/4D35A0：Native只解原Ref，不补vector边界、kind或空正文检查。
[[nodiscard]] std::size_t ParamsPatternVectorSize(
    const ParamsPatternReferenceVector& table, PatternResourceAccess access) noexcept;
[[nodiscard]] const YtlReferenceSlotRuntime& ParamsPatternReferenceAt(
    const ParamsPatternReferenceVector& table, std::size_t index, PatternResourceAccess access);
[[nodiscard]] std::uint32_t ParamsPatternItemKindAt(
    const ParamsPatternNative& pattern, std::size_t index, PatternResourceAccess access);
[[nodiscard]] const RuntimeCString& ParamsPatternItemNameAt(
    const ParamsPatternNative& pattern, std::size_t index, PatternResourceAccess access);
[[nodiscard]] const ParamsPatternFileGroupNative& ParamsPatternFileGroupAt(
    const ParamsPatternNative& pattern, std::size_t index, PatternResourceAccess access);
[[nodiscard]] const ParamsPatternExcPositionNative& ParamsPatternExcPositionAt(
    const ParamsPatternNative& pattern, std::size_t index, PatternResourceAccess access);
[[nodiscard]] const ParamsPatternFileConvertNative& ParamsPatternFileConvertAt(
    const ParamsPatternNative& pattern, std::size_t index, PatternResourceAccess access);

// 明确的离线/fixture边界，绝不由运行时查名miss触发，也不在根内缓存或同步旧值树。
[[nodiscard]] ParamsPatternReference ImportParamsPatternSnapshot(const ParamsPattern& snapshot);
[[nodiscard]] ParamsPattern ExportParamsPatternSnapshot(const ParamsPatternNative& pattern);
}

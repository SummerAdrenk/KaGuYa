#pragma once

#include <Runtime/ParamsInstallTable.h>
#include <Runtime/ParamsThumbnailTable.h>
#include <Runtime/ParamsRegistCgTable.h>
#include <Runtime/ParamsRegistSceneTable.h>
#include <Runtime/ParamsSceneNameTable.h>
#include <Runtime/ParamsCStringArray.h>
#include <Runtime/ParamsPlaceArray.h>
#include <Runtime/ParamsNestedStringTable.h>
#include <Runtime/ParamsByteArray.h>
#include <Runtime/ParamsScalarString.h>
#include <Runtime/ParamsTripleTable.h>
#include <Runtime/ParamsSettingTagRuntime.h>
#include <Runtime/ParamsDemoStorage.h>

#include <array>
#include <cstddef>
#include <cstdint>

namespace KaGuYa::Start::Runtime
{
// 保留既有线序索引0/1/2，唯一物理槽顺序仍是+7C/+88/+94，无旁路镜像。
struct ParamsGameSystemSettingTags final
{
    [[nodiscard]] static constexpr std::size_t size() noexcept { return 3; }
    [[nodiscard]] ParamsSettingTagReference& operator[](std::size_t index) noexcept
        { return physical[index < 2 ? 1 - index : index]; }
    [[nodiscard]] const ParamsSettingTagReference& operator[](std::size_t index) const noexcept
        { return physical[index < 2 ? 1 - index : index]; }
    bool operator==(const ParamsGameSystemSettingTags&) const = default;
    std::array<ParamsSettingTagReference, 3> physical;
};
static_assert(sizeof(ParamsGameSystemSettingTags) == 0x24);

// 40C410/40F370：正文无vptr，所有拥有槽都位于原偏移；诊断元数据不放进padding。
struct ParamsGameSystem final
{
    struct RawAt40C410 final {};
    explicit ParamsGameSystem(RawAt40C410,
        IRuntimeStringManagerSource& strings = DefaultRuntimeStringManagerSource());
    ~ParamsGameSystem() noexcept;

    // 默认构造和复制仅供strict/fixture/显式快照；生产工厂必须选RawAt40C410。
    ParamsGameSystem();
    ParamsGameSystem(const ParamsGameSystem& source);
    ParamsGameSystem& operator=(const ParamsGameSystem& source);
    [[nodiscard]] bool operator==(const ParamsGameSystem& source) const;

    std::uint8_t legacyModeAt00;
    std::array<std::byte, 3> unwrittenAt01;
    // 41ADE0：v3+读取U16到+04；历史成员名，不是文件头版本，业务用途未证。
    std::uint32_t versionMarker;
    std::uint32_t width;
    std::uint32_t height;
    union { ParamsByteArray startupConfigBytes; };
    union { ParamsScalarString gameTitle; };
    union { ParamsScalarString appendTitle; };
    union { ParamsScalarString brand; };
    union { ParamsScalarString legacyV2TextAt28; };
    std::uint8_t defaultNameMode;
    std::array<std::byte, 3> unwrittenAt2D;
    union { ParamsScalarString defaultGivenName; };
    union { ParamsScalarString defaultFamilyName; };
    union { ParamsInstallTable installTable; };
    std::array<std::uint32_t, 4> soundSlotCounts;
    // 41ADE0：v5.5+读取U8到+54；历史成员名不证明姓名模式，原值保留。
    std::uint32_t nameSelectionMode;
    union { YtlReferenceSlotRuntime legacyVoiceAt58; };
    union { YtlReferenceSlotRuntime legacyMultiAt64; };
    union { YtlReferenceSlotRuntime legacySoundAt70; };
    union { ParamsGameSystemSettingTags settingTags; };
    union { ParamsTripleTable v53Triples; };
    union { ParamsByteArray rawBlob; };
    union { ParamsDemoStorage demos; };
    union { ParamsCStringArray stringList; };
    union { ParamsPlaceArray places; };
    union { ParamsScalarString v54NestedListName; };
    union { ParamsNestedStringTable v54NestedLists; };
    union { ParamsThumbnailTable thumbnails; };
    union { ParamsSceneNameTable sceneNames; };
    union { ParamsRegistCgTable registCg; };
    union { ParamsRegistSceneTable registScene; };
};
static_assert(sizeof(ParamsGameSystem) == 0x11C);
static_assert(offsetof(ParamsGameSystem, versionMarker) == 4);
static_assert(offsetof(ParamsGameSystem, startupConfigBytes) == 0x10);
static_assert(offsetof(ParamsGameSystem, gameTitle) == 0x1C);
static_assert(offsetof(ParamsGameSystem, appendTitle) == 0x20);
static_assert(offsetof(ParamsGameSystem, brand) == 0x24);
static_assert(offsetof(ParamsGameSystem, legacyV2TextAt28) == 0x28);
static_assert(offsetof(ParamsGameSystem, defaultNameMode) == 0x2C);
static_assert(offsetof(ParamsGameSystem, defaultGivenName) == 0x30);
static_assert(offsetof(ParamsGameSystem, defaultFamilyName) == 0x34);
static_assert(offsetof(ParamsGameSystem, installTable) == 0x38);
static_assert(offsetof(ParamsGameSystem, soundSlotCounts) == 0x44);
static_assert(offsetof(ParamsGameSystem, nameSelectionMode) == 0x54);
static_assert(offsetof(ParamsGameSystem, legacyVoiceAt58) == 0x58);
static_assert(offsetof(ParamsGameSystem, legacyMultiAt64) == 0x64);
static_assert(offsetof(ParamsGameSystem, legacySoundAt70) == 0x70);
static_assert(offsetof(ParamsGameSystem, settingTags) == 0x7C);
static_assert(offsetof(ParamsGameSystem, v53Triples) == 0xA0);
static_assert(offsetof(ParamsGameSystem, rawBlob) == 0xAC);
static_assert(offsetof(ParamsGameSystem, demos) == 0xB8);
static_assert(offsetof(ParamsGameSystem, stringList) == 0xC4);
static_assert(offsetof(ParamsGameSystem, places) == 0xD0);
static_assert(offsetof(ParamsGameSystem, v54NestedListName) == 0xDC);
static_assert(offsetof(ParamsGameSystem, v54NestedLists) == 0xE0);
static_assert(offsetof(ParamsGameSystem, thumbnails) == 0xEC);
static_assert(offsetof(ParamsGameSystem, sceneNames) == 0xF8);
static_assert(offsetof(ParamsGameSystem, registCg) == 0x104);
static_assert(offsetof(ParamsGameSystem, registScene) == 0x110);
}

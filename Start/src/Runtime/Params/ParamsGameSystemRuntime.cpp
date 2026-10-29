#include <Runtime/ParamsGameSystemRuntime.h>

#include <cstring>
#include <memory>
#include <stdexcept>

namespace KaGuYa::Start::Runtime
{
namespace
{
void RequireLegacySnapshotDomain(const ParamsGameSystem& value)
{
    const auto empty = [](const YtlReferenceSlotRuntime& slot)
        { return !slot.baseAt00 && !slot.controlAt08; };
    if (!empty(value.legacyVoiceAt58) || !empty(value.legacyMultiAt64) || !empty(value.legacySoundAt70))
        throw std::logic_error("GameSystem legacy table snapshot is not implemented");
}
}

ParamsGameSystem::ParamsGameSystem(RawAt40C410, IRuntimeStringManagerSource& strings)
{
    // 40C410无SEH：失败串之前的拥有槽不自动回滚，后续未写字段保持原字节。
    width = 0;
    height = 0;
    std::construct_at(&startupConfigBytes);
    std::construct_at(&gameTitle, strings);
    std::construct_at(&appendTitle, strings);
    std::construct_at(&brand, strings);
    std::construct_at(&legacyV2TextAt28, strings);
    std::construct_at(&defaultGivenName, strings);
    std::construct_at(&defaultFamilyName, strings);
    std::construct_at(&installTable);
    std::construct_at(&legacyVoiceAt58);
    std::construct_at(&legacyMultiAt64);
    std::construct_at(&legacySoundAt70);
    std::construct_at(&settingTags);
    std::construct_at(&v53Triples);
    std::construct_at(&rawBlob);
    std::construct_at(&demos);
    std::construct_at(&stringList);
    std::construct_at(&places);
    std::construct_at(&v54NestedListName, strings);
    std::construct_at(&v54NestedLists);
    std::construct_at(&thumbnails);
    std::construct_at(&sceneNames);
    std::construct_at(&registCg);
    std::construct_at(&registScene);
}

ParamsGameSystem::~ParamsGameSystem() noexcept
{
    // 40F370的整体flags5：第一次释放异常就终止，不补自动成员展开或catch后继续。
    std::destroy_at(&registScene);
    std::destroy_at(&registCg);
    std::destroy_at(&sceneNames);
    std::destroy_at(&thumbnails);
    std::destroy_at(&v54NestedLists);
    std::destroy_at(&v54NestedListName);
    std::destroy_at(&places);
    std::destroy_at(&stringList);
    std::destroy_at(&demos);
    std::destroy_at(&rawBlob);
    std::destroy_at(&v53Triples);
    std::destroy_at(&settingTags);
    ReleaseYtlReference(legacySoundAt70);
    ReleaseYtlReference(legacyMultiAt64);
    ReleaseYtlReference(legacyVoiceAt58);
    std::destroy_at(&installTable);
    std::destroy_at(&defaultFamilyName);
    std::destroy_at(&defaultGivenName);
    std::destroy_at(&legacyV2TextAt28);
    std::destroy_at(&brand);
    std::destroy_at(&appendTitle);
    std::destroy_at(&gameTitle);
    std::destroy_at(&startupConfigBytes);
}

ParamsGameSystem::ParamsGameSystem() : ParamsGameSystem(RawAt40C410{})
{
    // 显式现代值域默认值；不是40C410的额外写集，生产Raw构造不执行这里。
    legacyModeAt00 = 0;
    versionMarker = 0;
    defaultNameMode = 0;
    soundSlotCounts = {};
    nameSelectionMode = 0;
}

ParamsGameSystem::ParamsGameSystem(const ParamsGameSystem& source) : ParamsGameSystem()
{
    *this = source;
}

ParamsGameSystem& ParamsGameSystem::operator=(const ParamsGameSystem& source)
{
    if (this == &source) return *this;
    // 现代深快照尚未定义非空旧表的值类型；不能把这些拥有表静默删掉或浅拷贝。
    RequireLegacySnapshotDomain(source);
    RequireLegacySnapshotDomain(*this);
    // 快照不包含legacyModeAt00；旧版正式读取/使用原生正文，不以此快照替代。
    // 其他公开数值用字节复制保留表示，不用typed读取半读对象中的标量。
    std::memcpy(&versionMarker, &source.versionMarker, sizeof(versionMarker));
    width = source.width;
    height = source.height;
    startupConfigBytes = source.startupConfigBytes;
    gameTitle = source.gameTitle;
    appendTitle = source.appendTitle;
    brand = source.brand;
    legacyV2TextAt28 = source.legacyV2TextAt28;
    std::memcpy(&defaultNameMode, &source.defaultNameMode, sizeof(defaultNameMode));
    defaultGivenName = source.defaultGivenName;
    defaultFamilyName = source.defaultFamilyName;
    installTable = source.installTable;
    std::memcpy(&soundSlotCounts, &source.soundSlotCounts, sizeof(soundSlotCounts));
    std::memcpy(&nameSelectionMode, &source.nameSelectionMode, sizeof(nameSelectionMode));
    legacyVoiceAt58 = source.legacyVoiceAt58;
    legacyMultiAt64 = source.legacyMultiAt64;
    legacySoundAt70 = source.legacySoundAt70;
    settingTags = source.settingTags;
    v53Triples = source.v53Triples;
    rawBlob = source.rawBlob;
    demos = source.demos;
    stringList = source.stringList;
    places = source.places;
    v54NestedListName = source.v54NestedListName;
    v54NestedLists = source.v54NestedLists;
    thumbnails = source.thumbnails;
    sceneNames = source.sceneNames;
    registCg = source.registCg;
    registScene = source.registScene;
    return *this;
}

bool ParamsGameSystem::operator==(const ParamsGameSystem& source) const
{
    // 仅已解析/现代值快照的语义比较；不读取v05.8未写的旧byte与padding。
    RequireLegacySnapshotDomain(*this);
    RequireLegacySnapshotDomain(source);
    return versionMarker == source.versionMarker && width == source.width && height == source.height &&
        startupConfigBytes == source.startupConfigBytes && gameTitle == source.gameTitle &&
        appendTitle == source.appendTitle && brand == source.brand && legacyV2TextAt28 == source.legacyV2TextAt28 &&
        defaultNameMode == source.defaultNameMode && defaultGivenName == source.defaultGivenName &&
        defaultFamilyName == source.defaultFamilyName && installTable == source.installTable &&
        soundSlotCounts == source.soundSlotCounts && nameSelectionMode == source.nameSelectionMode &&
        settingTags == source.settingTags && v53Triples == source.v53Triples && rawBlob == source.rawBlob &&
        demos == source.demos && stringList == source.stringList && places == source.places &&
        v54NestedListName == source.v54NestedListName && v54NestedLists == source.v54NestedLists &&
        thumbnails == source.thumbnails && sceneNames == source.sceneNames &&
        registCg == source.registCg && registScene == source.registScene;
}
}

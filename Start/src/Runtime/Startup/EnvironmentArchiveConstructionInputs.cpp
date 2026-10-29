#include <Runtime/EnvironmentArchiveConstructionInputs.h>
#include <Runtime/SystemDataEnvironmentArchive.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
SaveDataConstructionInputs BuildSaveDataConstructionInputs(
    const SystemDataEnvironmentArchive& sysData,
    const ParamsRuntimeProfile& profile,
    const SaveDataConstructionStorage& storage)
{
    // 现代成功来源前置条件；原 45B410 不检查 SysData.ready，不将此保护归因于原码。
    if (!sysData.readyAt04)
        throw std::invalid_argument("SaveData construction requires a ready SysData source (45B410)");

    SaveDataConstructionInputs inputs;
    inputs.storageBytesAt06 = storage.storageBytesAt06;
    inputs.storageBytesAt62 = storage.storageBytesAt62;
    inputs.storageBytesAt86 = storage.storageBytesAt86;

    // 原45B57F/45B5B1向SaveData+58/+5C执行410220 CString赋值，不是chars赋值。
    // 本builder仍是现代值快照；保留显式长度，不冒充原共享/Clone拥有链。
    inputs.runtimeStringAt58 = profile.gameSystem.defaultGivenName.View();
    inputs.runtimeStringAt5C = profile.gameSystem.defaultFamilyName.View();

    // 字段旧名 paramsValue 保持不变：45B600/45B64D 实际读取环境 slot0 的 +40/+44。
    inputs.paramsValueAt40 = sysData.valueAt40;
    inputs.paramsValueAt44 = static_cast<std::uint32_t>(sysData.valueAt44);
    // 45B69A/45B6E7：SysData+50/+54 -> SaveData+48/+4C；透明度只复制 DWORD 位值。
    inputs.paramsValueAt48 = sysData.valueAt50;
    inputs.paramsValueAt4C = static_cast<std::uint32_t>(sysData.valueAt54);
    // 45B73A/45B787：复制已配置的 +38/+3C，不是 SysData 固定的 +48/+4C。
    inputs.paramsValueAt64 = sysData.valueAt38;
    inputs.paramsValueAt68 = sysData.valueAt3C;
    return inputs;
}

SecretCgConstructionInputs BuildSecretCgConstructionInputs(const ParamsRuntimeProfile& profile)
{
    SecretCgConstructionInputs inputs;
    // 41D411 写 GameSystem+EC；45BEE8/45BF15 取每个 thumbnail 对象的首字符串。
    inputs.selectedFlagGroupNamesAtEC.reserve(profile.gameSystem.thumbnails.size());
    for (const auto& thumbnail : profile.gameSystem.thumbnails)
        inputs.selectedFlagGroupNamesAtEC.emplace_back(RuntimeStringView(thumbnail.nameAt00));

    // 41F47A 写 Pattern+1C，41F4B6 起的 +28 是第二表，不参与这两次查找。
    // 45BFEF/45C043 及 45C7FE -> 4786A0 均读取第一表，不能预排序或合并重名项。
    inputs.availableFlagGroupsAt1C.reserve(profile.pattern.groupTables[0].size());
    for (const auto& group : profile.pattern.groupTables[0])
        inputs.availableFlagGroupsAt1C.push_back({group.name.cp932, group.indices});
    inputs.resolvableFlagListsAt1C = inputs.availableFlagGroupsAt1C;

    // 41D857 写 GameSystem+104；45C426 起复制组名，45C714 取每个注册项的名称。
    // item 的 x/y/value 不进入 flag-list 输入；ID 到首个 flag 索引由 45C8D0 语义处理。
    inputs.mappingsAt104.reserve(profile.gameSystem.registCg.size());
    for (const auto& group : profile.gameSystem.registCg)
    {
        SecretCgMappingSource mapping;
        mapping.name = RuntimeStringView(group.nameAt00);
        mapping.flagListNames.reserve(group.itemsAt04.size());
        for (const auto& item : group.itemsAt04)
            mapping.flagListNames.emplace_back(RuntimeStringView(item.nameAt00));
        inputs.mappingsAt104.push_back(std::move(mapping));
    }
    return inputs;
}
}

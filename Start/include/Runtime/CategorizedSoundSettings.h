#pragma once
#include <Runtime/ParamsRuntimeProfile.h>
#include <Runtime/SettingTagErrorRuntime.h>

namespace KaGuYa::Start::Runtime
{
// Baku 4583C0/45A0E0 的分类配置适配；只生成构造期临时树，不改写 params 原树。
ParamsSettingTag AdaptCategorizedSoundSettings(const ParamsGameSystem& game,
    ISettingTagErrorSource& errors);
}

#pragma once
#include <Runtime/SettingPage03Native.h>
#include <array>
#include <span>
#include <string_view>

namespace KaGuYa::Start::Runtime
{
enum class SettingSoundMaster { Music, Effects, Voice };
struct SettingSoundMasterRow final
{
    SettingSoundMaster role;
    int slider, checkbox;
};
struct SettingSoundLabel final
{
    SettingPage03LabelPair images;
    // -1表示原构造不读取Display；不是猜测未公开角色。
    int displayIndex;
};
enum class SettingSoundLoopKind { None, CheckBox, Radio };
struct SettingSoundLoop final
{
    SettingSoundLoopKind kind = SettingSoundLoopKind::None;
    int checkbox = -1;
    int radioGroup = -1;
    int yes = -1, no = -1;
    std::span<const GuiButtonDescriptorNative2Ch> buttons;
    const GuiStaticDescriptorNative24h* label = nullptr;
};
struct SettingSoundPager final
{
    bool enabled = false;
    int split = 9;
    std::array<int,2> indicators{-1,-1};
    std::array<int,2> buttons{-1,-1};
    // 顺序为slider、label、preview、checkbox，不能从编号推算label。
    std::span<const std::array<int,4>> rows;
};
struct SettingPage03ResourceProfile final
{
    // 块0/1/2/3/4为共享master/effect/character/loop/pager，不是原对象布局。
    std::span<const unsigned> blockOrder;
    std::array<std::span<const SettingPage03ControlGroup>,5> groups;
    std::span<const SettingSoundMasterRow> masters;
    std::span<const SettingPage03NamedRow> effects, characters;
    std::span<const SettingSoundLabel> labels;
    std::span<const SettingPage03PreviewRow> effectPreviews, characterPreviews;
    std::span<const int> effectParamsIndices, characterParamsIndices;
    std::span<const SettingPage03MuteRow> muteRows;
    std::string_view registration;
    bool categorized = false;
    SettingSoundLoop loop;
    SettingSoundPager pager;
    const GuiStaticDescriptorNative24h* footer;
};
const SettingPage03ResourceProfile& CurrentSettingPage03Profile();
const SettingSoundMasterRow& SettingSoundMasterRowFor(SettingSoundMaster role);
}

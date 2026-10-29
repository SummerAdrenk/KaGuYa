#include <Runtime/ScrVmExecutableProfile.h>
#include <Runtime/ScriptNamedValueNative.h>

#include <mbstring.h>

#include <stdexcept>
#include <utility>

namespace KaGuYa::Start::Runtime
{
ScrVmExecutableProfile::ScrVmExecutableProfile(std::vector<std::string> namedValues)
    : namedValues_(std::move(namedValues))
{
    // 保留现代输入校验；空字符串不是 498067/49808A 检查的 null 表项。
    for (const std::string& name : namedValues_)
    {
        if (name.empty())
            throw std::invalid_argument("SCR VM executable profile contains an empty name");
    }
}

std::int32_t ScrVmExecutableProfile::ResolveNamedValue(
    const std::string_view cp932Name) const
{
    // 49804A -> 40B530：只复制首 NUL 前缀，不把含 NUL 的输入判作未命中。
    // 空查询仍构造 string，但不冒充原 CString 管理器/空串块及其分配失败时点。
    const std::string query(cp932Name.substr(0, cp932Name.find('\0')));
    for (std::size_t index = 0; index < namedValues_.size(); ++index)
    {
        // 0x498020 使用 _mbsicmp；运行游戏时由 Locale Emulator 提供日语多字节环境。
        if (_mbsicmp(
                reinterpret_cast<const unsigned char*>(query.c_str()),
                reinterpret_cast<const unsigned char*>(namedValues_[index].c_str())) == 0)
        {
            return static_cast<std::int32_t>(index);
        }
    }
    return -1;
}

const std::string& ScrVmExecutableProfile::NameAt(const std::size_t index) const
{
    return namedValues_.at(index);
}

std::size_t ScrVmExecutableProfile::Size() const noexcept
{
    return namedValues_.size();
}

ScriptNamedValueNativeTables MamatomoScriptNamedValueTables() noexcept
{
    // 49807C/49809F 限定 41+45 项；601954/601A0C 的 null 均在扫描范围外。
    static const char* const names[] = {
        "menu_result",
        "date_win",
        "effect_file",
        "replay_file",
        "replay_position",
        "replay",
        "replay_result",
        "game_end",
        "follow_return",
        "follow_point",
        "prologue_flag",
        "staff_ptn",
        "prog_result",
        "prog_param00",
        "prog_param01",
        "prog_param02",
        "prog_param03",
        "prog_param04",
        "prog_param05",
        "prog_param06",
        "prog_param07",
        "prog_param08",
        "prog_param09",
        "prog_param10",
        "prog_param11",
        "prog_param12",
        "prog_param13",
        "prog_param14",
        "prog_param15",
        "prog_param16",
        "prog_param17",
        "prog_param18",
        "bgd_type",
        "text_string",
        "text_context",
        "text_pos_x",
        "text_pos_y",
        "text_pos_center",
        "text_effect",
        "variable_name00",
        "wait_skiped",
        "mess_win_type",
        "month",
        "date",
        "day",
        "time",
        "days",
        "place",
        "face_pattern",
        "selected_page",
        "gague_disp",
        "gague_x",
        "gague_y",
        "gague_point",
        "restart_point",
        "countdown_suffix",
        "\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F",
        "\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x8E\x77\x92\xE8",
        "\x90\x53\x8D\xD8\x81\x40\x91\xCC\x8F\x64",
        "\x90\x53\x8D\xD8\x81\x40\x83\x58\x83\x67\x83\x8C\x83\x58",
        "\x90\x53\x8D\xD8\x81\x40\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F",
        "\x90\x53\x8D\xD8\x81\x40\x8E\x63\x82\xE8\x89\xF1\x90\x94",
        "\x90\x53\x8D\xD8\x81\x40\x90\x65\x96\xA7\x93\x78",
        "\x90\x53\x8D\xD8\x81\x40\x88\xCB\x91\xB6\x93\x78",
        "\x90\xE1\x88\xA4\x81\x40\x91\xCC\x8F\x64",
        "\x90\xE1\x88\xA4\x81\x40\x83\x58\x83\x67\x83\x8C\x83\x58",
        "\x90\xE1\x88\xA4\x81\x40\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F",
        "\x90\xE1\x88\xA4\x81\x40\x8E\x63\x82\xE8\x89\xF1\x90\x94",
        "\x90\xE1\x88\xA4\x81\x40\x90\x65\x96\xA7\x93\x78",
        "\x90\xE1\x88\xA4\x81\x40\x88\xCB\x91\xB6\x93\x78",
        "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x40\x91\xCC\x8F\x64",
        "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x40\x83\x58\x83\x67\x83\x8C\x83\x58",
        "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x40\x83\x67\x83\x8C\x81\x5B\x83\x6A\x83\x93\x83\x4F",
        "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x40\x8E\x63\x82\xE8\x89\xF1\x90\x94",
        "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x40\x90\x65\x96\xA7\x93\x78",
        "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x40\x88\xCB\x91\xB6\x93\x78",
        "\x89\xC4\x91\xE3\x8E\x71\x81\x40\x8F\x6F\x8C\xBB\x83\x74\x83\x89\x83\x4F",
        "\x89\xC4\x91\xE3\x8E\x71\x81\x40\x90\x65\x96\xA7\x93\x78",
        "\x90\x53\x8D\xD8\x81\x40\x83\x8B\x81\x5B\x83\x67",
        "\x90\xE1\x88\xA4\x81\x40\x83\x8B\x81\x5B\x83\x67",
        "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x40\x83\x8B\x81\x5B\x83\x67",
        "\x90\x53\x8D\xD8\x81\x40\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76",
        "\x90\xE1\x88\xA4\x81\x40\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76",
        "\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67\x81\x40\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76",
        "\x89\xC4\x91\xE3\x8E\x71\x81\x40\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76",
        "\x83\x6E\x81\x5B\x83\x8C\x83\x80\x81\x40\x83\x43\x83\x78\x83\x93\x83\x67\x83\x5E\x83\x43\x83\x76",
    };
    static_assert(std::size(names) == 86);
    return {std::span<const char* const, 41>(names, 41),
        std::span<const char* const, 45>(names + 41, 45)};
}

ScrVmExecutableProfile BuildMamatomoScrVmExecutableProfile()
{
    const auto tables = MamatomoScriptNamedValueTables();
    std::vector<std::string> names;
    names.reserve(86);
    for (const auto* name : tables.common) names.emplace_back(name);
    for (const auto* name : tables.game) names.emplace_back(name);
    return ScrVmExecutableProfile(std::move(names));
}
}

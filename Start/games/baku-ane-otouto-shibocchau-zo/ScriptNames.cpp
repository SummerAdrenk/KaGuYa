#include "ScriptNames.h"

namespace KaGuYa::Start::Runtime::BakuScriptNames
{
ScriptNamedValueNativeTables Tables() noexcept
{
    // 486290读取原5677C8/567830；表顺序就是脚本与存档的数值索引。
    static const char* const names[]{
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
        "bgd_type",
        "text_string",
        "text_context",
        "text_pos_x",
        "text_pos_y",
        "text_pos_center",
        "text_effect",
        "variable_name00",
        "prog_param02",
        "prog_param03",
        "mess_win_type",
        "month",
        "date",
        "day",
        "time",
        "days",
        "place",
        "face_pattern",
        "\x94\x54\x8A\x47\x8E\xC0\x81\x40\x8D\x44\x8A\xB4\x93\x78",
        "\x97\x46\x97\x9C\x89\xB9\x81\x40\x8D\x44\x8A\xB4\x93\x78",
        "\x8E\xC0\x8D\xF7\x81\x40\x8D\x44\x8A\xB4\x93\x78",
        "\x8D\xD8\x97\xAF\x81\x40\x8D\x44\x8A\xB4\x93\x78",
        "\x83\x41\x83\x8A\x83\x58\x81\x40\x8D\x44\x8A\xB4\x93\x78",
        "\x83\x41\x83\x8A\x83\x58\x81\x40\x93\x6F\x8F\xEA\x83\x74\x83\x89\x83\x4F",
        "\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F",
        "\x94\x54\x8A\x47\x8E\xC0\x81\x40\x83\x8B\x81\x5B\x83\x67",
        "\x97\x46\x97\x9C\x89\xB9\x81\x40\x83\x8B\x81\x5B\x83\x67",
        "\x8E\xC0\x8D\xF7\x81\x40\x83\x8B\x81\x5B\x83\x67",
        "\x8D\xD8\x97\xAF\x81\x40\x83\x8B\x81\x5B\x83\x67",
        "\x83\x41\x83\x8A\x83\x58\x81\x40\x83\x8B\x81\x5B\x83\x67",
    };
    static_assert(std::size(names) == 45);
    return {{names, 25}, {names + 25, 20}};
}
}

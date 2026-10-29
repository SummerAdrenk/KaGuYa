#include "ScriptNames.h"

namespace KaGuYa::Start::Runtime::ChuPetScriptNames
{
ScriptNamedValueNativeTables Tables() noexcept
{
    // 原0x48B4A0：0x572E48的25项及0x572EB0的21项；计数由循环边界确认。
    // 保留原CP932字节、两段顺序及数值索引，不按相邻null截表。
    static const char* const names[]{
        "menu_result", // 0
        "date_win", // 1
        "effect_file", // 2
        "replay_file", // 3
        "replay_position", // 4
        "replay", // 5
        "replay_result", // 6
        "game_end", // 7
        "follow_return", // 8
        "follow_point", // 9
        "prologue_flag", // 10
        "staff_ptn", // 11
        "prog_result", // 12
        "prog_param00", // 13
        "prog_param01", // 14
        "bgd_type", // 15
        "text_string", // 16
        "text_context", // 17
        "text_pos_x", // 18
        "text_pos_y", // 19
        "text_pos_center", // 20
        "text_effect", // 21
        "variable_name00", // 22
        "prog_param02", // 23
        "prog_param03", // 24
        // 游戏段从编号25开始。
        "mess_win_type", // 25
        "month", // 26
        "date", // 27
        "day", // 28
        "time", // 29
        "days", // 30
        "place", // 31
        "face_pattern", // 32
        "selected_page", // 33
        "\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F", // 34
        "\x8D\xE7\x96\xEB\x81\x40\x88\xA4\x8F\xEE\x93\x78", // 35
        "\x8D\xE7\x96\xEB\x81\x40\x8F\x5D\x8F\x87\x93\x78", // 36
        "\x8B\xD5\x97\x9C\x81\x40\x88\xA4\x8F\xEE\x93\x78", // 37
        "\x8B\xD5\x97\x9C\x81\x40\x8F\x5D\x8F\x87\x93\x78", // 38
        "\x83\x56\x83\x85\x83\x8A\x81\x40\x88\xA4\x8F\xEE\x93\x78", // 39
        "\x83\x56\x83\x85\x83\x8A\x81\x40\x8F\x5D\x8F\x87\x93\x78", // 40
        "\x89\xE8\x88\xCB\x81\x40\x83\x7D\x83\x62\x83\x76\x8F\xEA\x8F\x8A", // 41
        "\x97\xDA\x97\x9E\x8D\x81\x81\x40\x83\x7D\x83\x62\x83\x76\x8F\xEA\x8F\x8A", // 42
        "\x8E\xD1\x8A\xF3\x81\x40\x83\x7D\x83\x62\x83\x76\x8F\xEA\x8F\x8A", // 43
        "\x83\x43\x83\x6F\x83\x89\x83\x4C\x81\x40\x83\x7D\x83\x62\x83\x76\x8F\xEA\x8F\x8A", // 44
        "\x83\x6E\x81\x5B\x83\x8C\x83\x80\x81\x40\x83\x7D\x83\x62\x83\x76\x8F\xEA\x8F\x8A", // 45
    };
    static_assert(std::size(names) == 46);
    return {{names, 25}, {names + 25, 21}};
}
}

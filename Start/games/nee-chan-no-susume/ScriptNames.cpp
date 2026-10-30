#include "ScriptNames.h"

namespace KaGuYa::Start::Runtime::NeeChanScriptNames
{
ScriptNamedValueNativeTables Tables() noexcept
{
    // 原0x46B8F0：0x557240的26项及0x5572A8的27项；计数由循环边界确认。
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
        "wait_skiped", // 25
        // 游戏段从编号26开始。
        "mess_win_type", // 26
        "month", // 27
        "date", // 28
        "day", // 29
        "time", // 30
        "days", // 31
        "place", // 32
        "face_pattern", // 33
        "selected_page", // 34
        "gague_disp", // 35
        "gague_x", // 36
        "gague_y", // 37
        "gague_point", // 38
        "restart_point", // 39
        "\x8D\x44\x8A\xB4\x93\x78\x81\x40\x95\x5C\x8E\xA6\x83\x74\x83\x89\x83\x4F", // 40
        "\x97\x95\x88\xDF\x81\x40\x8D\x44\x8A\xB4\x93\x78", // 41
        "\x97\x95\x88\xDF\x81\x40\x92\xED\x88\xA4\x93\x78", // 42
        "\x8E\x8D\x8E\x71\x81\x40\x8D\x44\x8A\xB4\x93\x78", // 43
        "\x8E\x8D\x8E\x71\x81\x40\x95\xEA\x90\xAB\x93\x78", // 44
        "\x90\xC2\x97\x74\x81\x40\x8D\x44\x8A\xB4\x93\x78", // 45
        "\x90\xC2\x97\x74\x81\x40\x92\xED\x88\xA4\x93\x78", // 46
        "\x8E\x9E\x89\x4A\x81\x40\x8D\x44\x8A\xB4\x93\x78", // 47
        "\x97\x95\x88\xDF\x81\x40\x83\x8B\x81\x5B\x83\x67", // 48
        "\x8E\x8D\x8E\x71\x81\x40\x83\x8B\x81\x5B\x83\x67", // 49
        "\x90\xC2\x97\x74\x81\x40\x83\x8B\x81\x5B\x83\x67", // 50
        "\x8E\x9E\x89\x4A\x81\x40\x83\x8B\x81\x5B\x83\x67", // 51
        "\x83\x6E\x81\x5B\x83\x8C\x83\x80\x81\x40\x83\x8B\x81\x5B\x83\x67", // 52
    };
    static_assert(std::size(names) == 53);
    return {{names, 26}, {names + 26, 27}};
}
}

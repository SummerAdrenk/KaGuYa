#include <Runtime/PopupMenuSample18Data.h>

#include <array>
#include <string>

namespace KaGuYa::Start::Runtime
{
std::span<const CgOperationChildDescriptor> PopupMenuStaticDescriptorsSample18()
{
    static const auto descriptors = []
    {
        // 606A88..606C37：十二项36字节。名称为原CP932，不添加vrm路径前缀。
        const std::array<const char*, 12> names{
            "\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm", // 5BB9D4
            "\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x49\x83\x74.vrm", // 5BB9EC
            "\x83\x7C\x83\x62\x83\x76\x83\x5A\x81\x5B\x83\x75.vrm", // 5BBA00
            "\x83\x7C\x83\x62\x83\x76\x83\x8D\x81\x5B\x83\x68.vrm", // 5BBA14
            "\x83\x7C\x83\x62\x83\x76\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75.vrm", // 5BBA28
            "\x83\x7C\x83\x62\x83\x76\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68.vrm", // 5BBA44
            "\x83\x7C\x83\x62\x83\x76\x83\x7B\x83\x43\x83\x58.vrm", // 5BBA60
            "\x83\x7C\x83\x62\x83\x76\x83\x5E\x83\x43\x83\x67\x83\x8B.vrm", // 5BBA74
            "\x83\x7C\x83\x62\x83\x76\x83\x56\x81\x5B\x83\x93.vrm", // 5BBA88
            "\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7C\x83\x62\x83\x76\x83\x49\x81\x5B\x83\x67.vrm", // 5BBA9C
            "\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7C\x83\x62\x83\x76\x83\x58\x83\x4C\x83\x62\x83\x76.vrm", // 5BBAB8
            "\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x7C\x83\x62\x83\x76\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45.vrm"}; // 5BBAD4
        constexpr std::array<std::int32_t, 12> events{0, 1, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
        std::array<CgOperationChildDescriptor, 12> result;
        for (std::size_t index = 0; index < result.size(); ++index)
        {
            const std::string name = names[index];
            // 槽0/3/4共用同名；+20全null。Static的childId=-1来自4F407C。
            result[index] = {CgOperationControlKind::Static, events[index],
                {{name, {}, {}, name, name, {}, {}}, std::nullopt}, 7, -1};
        }
        return result;
    }();
    return descriptors;
}

std::span<const CgOperationChildDescriptor> PopupMenuCheckBoxDescriptorsSample18()
{
    static const auto descriptors = []
    {
        // 606C38起九项44字节，606C3C只是首项+04的七资源指针区。
        const std::string prefix = "\x83\x52\x83\x7D\x83\x93\x83\x68";
        const std::array<const char*, 9> commands{
            "\x83\x5A\x81\x5B\x83\x75", // セーブ
            "\x83\x8D\x81\x5B\x83\x68", // ロード
            "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75", // クイックセーブ
            "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68", // クイックロード
            "\x83\x7B\x83\x43\x83\x58", // ボイス
            "\x83\x58\x83\x67\x83\x62\x83\x76", // ストップ
            "\x83\x49\x81\x5B\x83\x67", // オート
            "\x83\x58\x83\x4C\x83\x62\x83\x76", // スキップ
            "\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x8F\xC1\x8B\x8E"}; // ウィンドウ消去
        constexpr std::array<std::int32_t, 9> events{2, 3, 4, 5, 6, 8, 9, 10, 11};
        constexpr std::array<std::int32_t, 9> childIds{18, 19, 20, 21, -1, -1, 26, -1, 8};
        std::array<CgOperationChildDescriptor, 9> result;
        for (std::size_t index = 0; index < result.size(); ++index)
        {
            const auto name = [&](const char* state) { return prefix + state + commands[index] + ".vrm"; };
            const auto target = name("\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67");
            // 五个非空槽，四种资源；槽3/4别名。+24=7，+28按原DWORD保留-1。
            result[index] = {CgOperationControlKind::CheckBox, events[index],
                {{name("\x83\x49\x83\x93"), name("\x83\x49\x83\x74"), {}, target, target,
                  name("\x83\x4E\x83\x8A\x83\x62\x83\x4E"), {}}, std::nullopt}, 7, childIds[index]};
        }
        return result;
    }();
    return descriptors;
}

PopupMenuCheckStampData PopupMenuCheckStampsSample18() noexcept
{
    // 606DC4/606DC8指向5BBF38/5BBF50；606DCC/606DD0为(96,1)。
    return {"\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x60\x83\x46\x83\x62\x83\x4E.vrm",
        "\x83\x52\x83\x7D\x83\x93\x83\x68\x83\x49\x83\x74\x83\x60\x83\x46\x83\x62\x83\x4E.vrm", {96, 1}};
}

std::span<const std::int32_t> PopupMenuLayoutEventsSample18(const std::int32_t mode) noexcept
{
    static constexpr std::array<std::int32_t, 8> mode1{11, 6, 10, 9, 5, 4, 3, 2}; // 606F20
    static constexpr std::array<std::int32_t, 5> other{8, 11, 6, 10, 9}; // 60699C
    return mode == 1 ? std::span<const std::int32_t>{mode1} : std::span<const std::int32_t>{other};
}

std::span<const PopupMenuNavigationDescriptor> PopupMenuNavigationSample18(const std::int32_t mode) noexcept
{
    // 原表以source=-1结束；504CE0不把终止项写入GuiOperationKey的vector。
    // 保留实际终止source供504CE0读取；负source后的target不被读取，展示span不包含终止项。
    static constexpr std::array<GuiDialogNavigationMapping, 9> forward1{{
        {2, 3}, {3, 4}, {4, 5}, {5, 9}, {9, 10}, {10, 6}, {6, 11}, {11, 2}, {-1, 0}}}; // 606E10
    static constexpr std::array<GuiDialogNavigationMapping, 9> backward1{{
        {2, 11}, {3, 2}, {4, 3}, {5, 4}, {9, 5}, {10, 9}, {6, 10}, {11, 6}, {-1, 0}}}; // 606E58
    static constexpr std::array<GuiDialogNavigationMapping, 6> forwardOther{{
        {9, 10}, {10, 6}, {6, 11}, {11, 8}, {8, 9}, {-1, 0}}}; // 606EB0
    static constexpr std::array<GuiDialogNavigationMapping, 6> backwardOther{{
        {9, 8}, {10, 9}, {6, 10}, {11, 6}, {8, 11}, {-1, 0}}}; // 606EE0
    static const std::array<PopupMenuNavigationDescriptor, 2> mode1{{
        {2, std::span{forward1}.first(8)}, {1, std::span{backward1}.first(8)}}}; // 606EA0
    static const std::array<PopupMenuNavigationDescriptor, 2> other{{
        {2, std::span{forwardOther}.first(5)}, {1, std::span{backwardOther}.first(5)}}}; // 606F10
    return mode == 1 ? mode1 : other;
}

std::span<const PopupMenuHoverPair> PopupMenuHoverMapSample18() noexcept
{
    // 606F40/606F44：保留event7这一对，即使构造的九个CheckBox没有event7。
    static constexpr std::array<PopupMenuHoverPair, 10> pairs{{
        {2, 12}, {3, 13}, {4, 14}, {5, 15}, {6, 16}, {7, 17}, {8, 18}, {9, 19}, {10, 20}, {11, 21}}};
    return pairs;
}

POINT PopupMenuHintOffsetSample18() noexcept { return {0, -15}; } // 606F90/606F94
POINT PopupMenuCursorOffsetSample18() noexcept { return {110, 60}; } // 606F98/606F9C

std::string_view PopupMenuSceneCommandSample18(const std::int32_t result) noexcept
{
    switch (result)
    {
    case 2: return "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA"; // 5AE218
    case 3: return "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA"; // 5AE224
    case 4: return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75"; // 5AE8AC
    case 5: return "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x8D\x81\x5B\x83\x68"; // 5AE8BC
    default: return {};
    }
}
}

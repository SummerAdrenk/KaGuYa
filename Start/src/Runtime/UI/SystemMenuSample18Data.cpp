#include <Runtime/SystemMenuSample18Data.h>

#include <cstdint>
#include <string>

namespace KaGuYa::Start::Runtime
{
const std::array<CgOperationChildDescriptor, 5>& SystemMenuChildDescriptorsSample18At606978()
{
    static const auto descriptors = []
    {
        // 606978原始文件偏移205378、6069B0偏移2053B0：已有静态DWORD，不是BSS构造器补值。
        // 与TitleDialogChildDescriptors一致，UTF-8源码中用转义保留CP932字节，不做ACP转换。
        // 公共前缀：vrm:\メニュー。拼接片段均逐项对照5BB6D8..5BB9B4原NUL结尾字符串。
        const std::string prefix = "vrm:\\\x83\x81\x83\x6A\x83\x85\x81\x5B";
        const auto frame = prefix + "\x98\x67.alp";
        const std::array<const char*, 4> commands{
            "\x83\x45\x83\x42\x83\x93\x83\x68\x83\x45\x89\xE6\x96\xCA", // ウィンドウ画面
            "\x83\x74\x83\x8B\x83\x58\x83\x4E\x83\x8A\x81\x5B\x83\x93\x89\xE6\x96\xCA", // フルスクリーン画面
            "\x90\xDD\x92\xE8\x89\xE6\x96\xCA", // 設定画面
            "\x83\x51\x81\x5B\x83\x80\x8F\x49\x97\xB9"}; // ゲーム終了
        constexpr const char* on = "\x83\x49\x83\x93";
        constexpr const char* off = "\x83\x49\x83\x74";
        constexpr const char* target = "\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67";
        constexpr const char* click = "\x83\x4E\x83\x8A\x83\x62\x83\x4E";
        constexpr const char* range = "\x94\xCD\x88\xCD";

        std::array<CgOperationChildDescriptor, 5> result;
        // 606978：event=0；槽0/3/4同指5BB6D8，其他槽和+20坐标指针为0。
        // Static的childId=-1来自4F407C；既有Static描述以7表示无另设按钮命中槽。
        result[0] = {CgOperationControlKind::Static, 0,
            {{frame, {}, {}, frame, frame, {}, {}}, std::nullopt}, 7, -1};

        // 每项七个状态槽，六个非空引用、五种不同资源；槽2空，槽3/4共用target。
        // 表基址及五个唯一字符串指针（on/off/target/click/range）：
        // 6069B0: 5BB6EC/5BB710/5BB734/5BB760/5BB788，event=1。
        // 6069DC: 5BB7AC/5BB7D4/5BB7FC/5BB82C/5BB858，event=2。
        // 606A08: 5BB880/5BB8A0/5BB8C0/5BB8E4/5BB908，event=3。
        // 606A34: 5BB928/5BB948/5BB968/5BB990/5BB9B4，event=4。
        for (std::size_t index = 0; index < commands.size(); ++index)
        {
            const auto name = [&](const char* state) { return prefix + state + commands[index] + ".alp"; };
            const auto focused = name(target);
            // 各表+20均为null：沿用资源原点，不伪造(0,0)平移。
            // +24=6经504355送Button虚槽60，选择范围槽；不是状态数量或布尔开关。
            // +28=FFFFFFFF经50432D..504330写childId；ClickedEvent启用1由caller 4F437B写，不属于表。
            result[index + 1] = {CgOperationControlKind::Button, static_cast<std::int32_t>(index + 1),
                {{name(on), name(off), {}, focused, focused, name(click), name(range)}, std::nullopt}, 6, -1};
        }
        return result;
    }();
    return descriptors;
}

std::string_view SystemMenuSettingsSceneNameSample18At5B4E68() noexcept
{
    // 5B4E68原DWORD为E892DD90/CA96E689：設定画面，CP932共八字节。
    // 5095BD push 8；5095BF push 5B4E68；5095C4 lea ecx,[eax+1C]；5095C7 call437680。
    static constexpr char name[] = "\x90\xDD\x92\xE8\x89\xE6\x96\xCA";
    static_assert(sizeof(name) - 1 == 8);
    return {name, sizeof(name) - 1};
}
}

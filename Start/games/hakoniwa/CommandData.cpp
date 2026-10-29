#include "CommandData.h"

#include <array>

namespace KaGuYa::Start::Runtime::HakoniwaCommandData
{
namespace
{
// Hakoniwa 原 PE 0x57A388..0x57A504：特殊選択肢_*，保留 CP932 原字节。
// 不使用编译器执行字符集转换，也不把资源文件名中的数字解释为脚本条件。
constexpr char resource83101015On[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_83101015_on.alp";
constexpr char resource83101015Target[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_83101015_target.alp";
constexpr char resource83101015Click[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_83101015_click.alp";
constexpr char resource10158310On[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_10158310_on.alp";
constexpr char resource10158310Target[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_10158310_target.alp";
constexpr char resource10158310Click[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_10158310_click.alp";
constexpr char resource10150831On[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_10150831_on.alp";
constexpr char resource10150831Target[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_10150831_target.alp";
constexpr char resource10150831Click[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_10150831_click.alp";
constexpr char resource08311015On[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_08311015_on.alp";
constexpr char resource08311015Target[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_08311015_target.alp";
constexpr char resource08311015Click[] =
    "vrm:\\\x93\xC1\x8E\xEA\x91\x49\x91\xF0\x8E\x88_08311015_click.alp";

// 0x5AC9AC={kind=1,count=4,records=0x5A7B00}。
// 0x4C408F 的 cmp eax,1 限定一组，不能读取紧邻的其它数据为第二组。
// 七槽为 ON/null/null/TARGET/TARGET/CLICK/null；position=null 保留 ARC 原点。
// hit=0 指定 ON 的 alpha 命中，不是矩形命中；child=-1 不改成 event。
constexpr std::array<GuiButtonDescriptorNative2Ch, 4> buttons{{
    {{0, {resource83101015On, nullptr, nullptr, resource83101015Target,
          resource83101015Target, resource83101015Click, nullptr}, nullptr}, 0, -1}, // 0x5A7B00
    {{1, {resource10158310On, nullptr, nullptr, resource10158310Target,
          resource10158310Target, resource10158310Click, nullptr}, nullptr}, 0, -1}, // 0x5A7B2C
    {{2, {resource10150831On, nullptr, nullptr, resource10150831Target,
          resource10150831Target, resource10150831Click, nullptr}, nullptr}, 0, -1}, // 0x5A7B58
    {{3, {resource08311015On, nullptr, nullptr, resource08311015Target,
          resource08311015Target, resource08311015Click, nullptr}, nullptr}, 0, -1}, // 0x5A7B84
}};

// 0x5ACCC8 / 0x5ACCF0：配对目标是 event，负 source 在 0x4E4380 停止复制。
constexpr GuiDialogNavigationMapping horizontal[]{{0, 1}, {1, 0}, {2, 3}, {3, 2}, {-1, 0}};
constexpr GuiDialogNavigationMapping vertical[]{{0, 2}, {1, 3}, {2, 0}, {3, 1}, {-1, 0}};

// 0x5ACD18；0x4C415F..0x4C4165 按 8 字节递增至 0x20，仅四项。
// input 原序 4/3/1/2，两对共用 pair 指针；不要补普通 Select 的 255 哨兵。
constexpr std::array<SelectNavigationDescriptorNative8h, 4> navigation{{
    {4, horizontal}, {3, horizontal}, {1, vertical}, {2, vertical},
}};
}

std::span<const GuiButtonDescriptorNative2Ch, 4> Buttons() noexcept { return buttons; }
std::span<const SelectNavigationDescriptorNative8h, 4> Navigation() noexcept { return navigation; }
const CommandSelectionResourceProfile& Profile() noexcept
{
    static const CommandSelectionResourceProfile profile{Buttons(), Navigation()};
    return profile;
}
}

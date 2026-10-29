#include <Runtime/SettingDialogNativeApply.h>
#include <array>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 51DF9A原跳表沿共享尾分支的写入次序，不按最终字段值矩阵重排。
constexpr SettingDialogNativeApplyWrite c0[]{{0x0C,1},{0x34,0},{0x14,3},{0x24,0},{0x1C,0},{0x2C,0}};
constexpr SettingDialogNativeApplyWrite c1[]{{0x0C,0},{0x14,1},{0x34,0},{0x24,0},{0x1C,0},{0x2C,0}};
constexpr SettingDialogNativeApplyWrite c2[]{{0x0C,0},{0x14,3},{0x24,1},{0x34,0},{0x1C,0},{0x2C,0}};
constexpr SettingDialogNativeApplyWrite c3[]{{0x0C,0},{0x14,3},{0x1C,1},{0x24,0},{0x34,0},{0x2C,0}};
constexpr SettingDialogNativeApplyWrite c4[]{{0x0C,0},{0x14,3},{0x1C,0},{0x24,0},{0x2C,1},{0x34,0}};
constexpr SettingDialogNativeApplyWrite c5[]{{0x0C,0},{0x34,1},{0x14,3},{0x24,0},{0x1C,0},{0x2C,0}};
constexpr std::array<std::uint32_t,28> unwind{
    0xFFFFFFFF,0x5A7440,0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x5A7448,0xFFFFFFFF,0x541E55,
    0xFFFFFFFF,0x5A7450,0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x5A7458,0xFFFFFFFF,0x541E55,
    0xFFFFFFFF,0x5A7460,0xFFFFFFFF,0x541E55,0xFFFFFFFF,0x5A7468,0xFFFFFFFF,0x541E55,
    0xFFFFFFFF,0x5A7470,0xFFFFFFFF,0x541E55};
constexpr std::array<std::uint32_t,6> textUnwind{0xFFFFFFFF,0x593820,0xFFFFFFFF,0,0xFFFFFFFF,0};
// 完整FuncInfo及registered try/handler均来自PE，不能仅凭decompile或unwindMap推断catch。
constexpr std::array<std::uint32_t,9> funcInfo{0x19930522,14,0x5F74E8,0,0,0,0,0,1};
constexpr std::array<std::uint32_t,9> textFuncInfo{0x19930522,3,0x5E1794,1,0x5E1780,0,0,0,1};
constexpr std::array<std::uint32_t,5> textTry{1,1,2,1,0x5E1770};
constexpr std::array<std::uint32_t,4> textHandler{0x40,0,0,0x433ED0};
constexpr char textError[]="\x83\x74\x83\x48\x83\x93\x83\x67\x82\xCC\x95\xCF\x8D\x58\x8C\xE3"
    "\x82\xCC\x8D\xC4\x95\x60\x89\xE6\x82\xC9\x8E\xB8\x94\x73\x82\xB5\x82\xDC\x82\xB5\x82\xBD";
}
std::span<const SettingDialogNativeApplyWrite> SettingDialogNativeApplyPolicyWritesSample18(unsigned choice) noexcept
{switch(choice){case 0:return c0;case 1:return c1;case 2:return c2;case 3:return c3;case 4:return c4;case 5:return c5;}return {};}
std::span<const std::uint32_t,28> SettingDialogNativeApplyOriginalUnwindSample18() noexcept{return unwind;}
std::span<const std::uint32_t,6> SettingDialogNativeApplyTextOriginalUnwindSample18() noexcept{return textUnwind;}
std::span<const std::uint32_t,9> SettingDialogNativeApplyOriginalFuncInfoSample18() noexcept{return funcInfo;}
std::span<const std::uint32_t,9> SettingDialogNativeApplyTextOriginalFuncInfoSample18() noexcept{return textFuncInfo;}
std::span<const std::uint32_t,5> SettingDialogNativeApplyTextOriginalTryBlockSample18() noexcept{return textTry;}
std::span<const std::uint32_t,4> SettingDialogNativeApplyTextOriginalHandlerSample18() noexcept{return textHandler;}
const char* SettingDialogNativeApplyTextErrorSample18() noexcept{return textError;}
}

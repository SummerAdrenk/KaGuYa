#include "SaveLoadConfirmDialogNative.Internal.h"

namespace KaGuYa::Start::Runtime
{
namespace
{
// 603C28/603CC0，字节逐项取自指定SHA的CP932数据，空槽就是nullptr。
constexpr char loadTitle[] = "\x83\x8D\x81\x5B\x83\x68\x8A\x6D\x94\x46.vrm";
constexpr char saveTitle[] = "\x83\x5A\x81\x5B\x83\x75\x8A\x6D\x94\x46.vrm";
constexpr char overwriteTitle[] = "\x8F\xE3\x8F\x91\x82\xAB\x8A\x6D\x94\x46.vrm";
constexpr char loadNormal[] = "\x83\x8D\x81\x5B\x83\x68\x8A\x6D\x94\x46\x83\x6D\x81\x5B\x83\x7D\x83\x8B.vrm";
constexpr char loadTarget[] = "\x83\x8D\x81\x5B\x83\x68\x8A\x6D\x94\x46\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char loadClick[] = "\x83\x8D\x81\x5B\x83\x68\x8A\x6D\x94\x46\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr char loadCancelNormal[] = "\x83\x8D\x81\x5B\x83\x68\x82\xE2\x82\xDF\x82\xE9\x83\x6D\x81\x5B\x83\x7D\x83\x8B.vrm";
constexpr char loadCancelTarget[] = "\x83\x8D\x81\x5B\x83\x68\x82\xE2\x82\xDF\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char loadCancelClick[] = "\x83\x8D\x81\x5B\x83\x68\x82\xE2\x82\xDF\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr char saveNormal[] = "\x83\x5A\x81\x5B\x83\x75\x8A\x6D\x94\x46\x83\x6D\x81\x5B\x83\x7D\x83\x8B.vrm";
constexpr char saveTarget[] = "\x83\x5A\x81\x5B\x83\x75\x8A\x6D\x94\x46\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char saveClick[] = "\x83\x5A\x81\x5B\x83\x75\x8A\x6D\x94\x46\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr char saveCancelNormal[] = "\x83\x5A\x81\x5B\x83\x75\x82\xE2\x82\xDF\x82\xE9\x83\x6D\x81\x5B\x83\x7D\x83\x8B.vrm";
constexpr char saveCancelTarget[] = "\x83\x5A\x81\x5B\x83\x75\x82\xE2\x82\xDF\x82\xE9\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.vrm";
constexpr char saveCancelClick[] = "\x83\x5A\x81\x5B\x83\x75\x82\xE2\x82\xDF\x82\xE9\x83\x4E\x83\x8A\x83\x62\x83\x4E.vrm";
constexpr std::array<GuiStaticDescriptorNative24h, 3> statics{{
    {0, {loadTitle, nullptr, nullptr, loadTitle, loadTitle, nullptr, nullptr}, nullptr},
    {1, {saveTitle, nullptr, nullptr, saveTitle, saveTitle, nullptr, nullptr}, nullptr},
    {2, {overwriteTitle, nullptr, nullptr, overwriteTitle, overwriteTitle, nullptr, nullptr}, nullptr}}};
constexpr std::array<GuiButtonDescriptorNative2Ch, 4> buttons{{
    {{3, {loadNormal, nullptr, nullptr, loadTarget, loadTarget, loadClick, nullptr}, nullptr}, 0, -1},
    {{4, {loadCancelNormal, nullptr, nullptr, loadCancelTarget, loadCancelTarget, loadCancelClick, nullptr}, nullptr}, 0, -1},
    {{5, {saveNormal, nullptr, nullptr, saveTarget, saveTarget, saveClick, nullptr}, nullptr}, 0, -1},
    {{6, {saveCancelNormal, nullptr, nullptr, saveCancelTarget, saveCancelTarget, saveCancelClick, nullptr}, nullptr}, 0, -1}}};
constexpr const char* slotNames[]{
    nullptr, "file_1.vrm", "file_2.vrm", "file_3.vrm", "file_4.vrm", "file_5.vrm", "file_6.vrm", "file_7.vrm", "file_8.vrm", nullptr,
    nullptr, "Lfile_1.vrm", "Lfile_2.vrm", "Lfile_3.vrm", "Lfile_4.vrm", "Lfile_5.vrm", "Lfile_6.vrm", "Lfile_7.vrm", "Lfile_8.vrm", nullptr};
constexpr const char* pageNames[]{
    "file_p_0.vrm", "file_p_1.vrm", "file_p_2.vrm", "file_p_3.vrm", "file_p_4.vrm", "file_p_5.vrm", "file_p_6.vrm", "file_p_7.vrm", "file_p_8.vrm", "file_p_9.vrm",
    "Lfile_p_0.vrm", "Lfile_p_1.vrm", "Lfile_p_2.vrm", "Lfile_p_3.vrm", "Lfile_p_4.vrm", "Lfile_p_5.vrm", "Lfile_p_6.vrm", "Lfile_p_7.vrm", "Lfile_p_8.vrm", "Lfile_p_9.vrm"};
constexpr const char* dateNames[]{
    "file_y_0.vrm", "file_y_1.vrm", "file_y_2.vrm", "file_y_3.vrm", "file_y_4.vrm", "file_y_5.vrm", "file_y_6.vrm", "file_y_7.vrm", "file_y_8.vrm", "file_y_9.vrm",
    "\x8A\x6D\x94\x46\x8E\xCE\x90\xFC.vrm",
    "Lfile_y_0.vrm", "Lfile_y_1.vrm", "Lfile_y_2.vrm", "Lfile_y_3.vrm", "Lfile_y_4.vrm", "Lfile_y_5.vrm", "Lfile_y_6.vrm", "Lfile_y_7.vrm", "Lfile_y_8.vrm", "Lfile_y_9.vrm",
    "L\x8A\x6D\x94\x46\x8E\xCE\x90\xFC.vrm"};
constexpr const char* timeNames[]{
    "file_t_0.vrm", "file_t_1.vrm", "file_t_2.vrm", "file_t_3.vrm", "file_t_4.vrm", "file_t_5.vrm", "file_t_6.vrm", "file_t_7.vrm", "file_t_8.vrm", "file_t_9.vrm",
    "\x8A\x6D\x94\x46\x83\x52\x83\x8D\x83\x93.vrm",
    "Lfile_t_0.vrm", "Lfile_t_1.vrm", "Lfile_t_2.vrm", "Lfile_t_3.vrm", "Lfile_t_4.vrm", "Lfile_t_5.vrm", "Lfile_t_6.vrm", "Lfile_t_7.vrm", "Lfile_t_8.vrm", "Lfile_t_9.vrm",
    "L\x8A\x6D\x94\x46\x83\x52\x83\x8D\x83\x93.vrm"};
constexpr std::array<std::uint32_t, 39> slots{
    0x509ED0, 0x4FDD40, 0x5025A0, 0x50A5D0, 0x4FF020, 0x509D20, 0x536310,
    0x536500, 0x50FF40, 0x538660, 0x510630, 0x500800, 0x532170, 0x51E740,
    0x510BA0, 0x5106D0, 0x511610, 0x50E7E0, 0x52DBC0, 0x5093D0, 0x50ABA0,
    0x518C30, 0x526DA0, 0x5231C0, 0x5230B0, 0x50FC80, 0x522E90, 0x510030,
    0x5100E0, 0x50A6A0, 0x50B920, 0x510220, 0x510170, 0x5102B0, 0x518A80,
    0x5086B0, 0x525300, 0x51C110, 0x536140};
// 只读取证后编译内置的原邻接DWORD；运行时绝不读原PE文件。
constexpr std::array<std::uint32_t, 153> immutableWords{
    0x000000BCu, 0x0000004Du, 0x000000A1u, 0x0000004Du, 0x00000086u, 0x0000004Du, 0x0000007Bu, 0x00000049u,
    0x0000007Bu, 0x00000049u, 0x00000000u, 0x00000071u, 0x0000008Bu, 0x00000081u, 0x0000008Bu, 0x00000091u,
    0x00000089u, 0x0000009Eu, 0x0000008Bu, 0x000000AEu, 0x0000008Bu, 0x000000BEu, 0x00000089u, 0x000000CBu,
    0x0000008Bu, 0x000000DBu, 0x0000008Bu, 0x00000076u, 0x000000B2u, 0x0000008Du, 0x000000B2u, 0x000000AAu,
    0x000000B6u, 0x000000B9u, 0x000000B2u, 0x000000D0u, 0x000000B2u, 0x000000ECu, 0x00000085u, 0x0000011Du,
    0x0000003Eu, 0x0000011Du, 0x000000DFu, 0x0000011Du, 0x00000180u, 0x0000011Du, 0x00000221u, 0x00000016u,
    0x000002BCu, 0x00000000u, 0x00000000u, 0x00000016u, 0x000002BCu, 0x00000000u, 0x00000006u, 0x00000001u,
    0x00000002u, 0x00000005u, 0x00000006u, 0x00000000u, 0x00000002u, 0x00000003u, 0x00000004u, 0x00000000u,
    0x00000001u, 0x00000003u, 0x00000004u, 0x00000015u, 0x000001F4u, 0x00000000u, 0x00000004u, 0x0000007Du,
    0x00000093u, 0x0000007Du, 0x000000B3u, 0x0000005Au, 0x00000047u, 0x00000039u, 0x00000115u, 0x46946D8Au,
    0x762E6798u, 0x00006D72u, 0x46946D8Au, 0x62838183u, 0x8F5F5A83u, 0x2EB99749u, 0x006D7276u, 0x46946D8Au,
    0x62838183u, 0x835F5A83u, 0x8343835Eu, 0x2E8B8367u, 0x006D7276u, 0x46946D8Au, 0x62838183u, 0x8F5F5A83u,
    0x90FA8A89u, 0x2EE892DDu, 0x006D7276u, 0x46946D8Au, 0x62838183u, 0x835F5A83u, 0x8343834Eu, 0x834E8362u,
    0x835B815Au, 0x72762E75u, 0x0000006Du, 0x46946D8Au, 0x62838183u, 0x835F5A83u, 0x8343834Eu, 0x834E8362u,
    0x835B818Du, 0x72762E68u, 0x0000006Du, 0x46946D8Au, 0xF0914991u, 0x82CD825Fu, 0x72762EA2u, 0x0000006Du,
    0x46946D8Au, 0xF0914991u, 0x82CD825Fu, 0x814A83A2u, 0x835C835Bu, 0x72762E8Bu, 0x0000006Du, 0x46946D8Au,
    0xF0914991u, 0x82CD825Fu, 0x834E83A2u, 0x8362838Au, 0x72762E4Eu, 0x0000006Du, 0x46946D8Au, 0xF0914991u,
    0x82A2825Fu, 0x2EA682A2u, 0x006D7276u, 0x46946D8Au, 0xF0914991u, 0x82A2825Fu, 0x83A682A2u, 0x835B814Au,
    0x2E8B835Cu, 0x006D7276u, 0x46946D8Au, 0xF0914991u, 0x82A2825Fu, 0x83A682A2u, 0x838A834Eu, 0x2E4E8362u,
    0x006D7276u};
static_assert(immutableWords.size() * sizeof(std::uint32_t) == 0x5B6D28u - 0x5B6AC4u);
}
std::span<const std::uint32_t, 39> SaveLoadConfirmDialogNativeOriginalSlots() noexcept { return slots; }
std::span<const GuiStaticDescriptorNative24h, 3> SaveLoadConfirmDialogNativeStaticDescriptors() noexcept { return statics; }
std::span<const GuiButtonDescriptorNative2Ch, 4> SaveLoadConfirmDialogNativeButtonDescriptors() noexcept { return buttons; }
std::span<const char* const> SaveLoadConfirmDialogNativeDigitNames(std::uint32_t group) noexcept
{
    const std::span<const char* const> groups[]{slotNames, pageNames, dateNames, timeNames};
    return groups[group];
}
const char* SaveLoadConfirmDialogNativeMaskNameSample18() noexcept
{ return "\x8A\x6D\x94\x46\x89\xE6\x96\xCA\x97\x70\x94\xBC\x93\xA7\x96\xBE\x8D\x95.vrm"; }
const char* SaveLoadConfirmDialogNativeFontFaceSample18() noexcept
{ return "\x82\x6C\x82\x72 \x83\x53\x83\x56\x83\x62\x83\x4E"; }
std::int32_t ReadSaveLoadConfirmDialogNativeImmutableDword(std::uint32_t address)
{
    if (address < 0x5B6AC4u || address >= 0x5B6D28u || ((address - 0x5B6AC4u) & 3u))
        throw ATL::CAtlException(E_INVALIDARG);
    return std::bit_cast<std::int32_t>(immutableWords[(address - 0x5B6AC4u) / 4u]);
}

namespace SaveLoadConfirmNativeDetail
{
std::int32_t ImmutableDword(std::uint32_t address)
{
    if (address >= 0x5B6AC4u && address < 0x5B6D28u && ((address - 0x5B6AC4u) & 3u) == 0)
        return ReadSaveLoadConfirmDialogNativeImmutableDword(address);
    // 宿主可扩展经过证据核对且编译内置的域，绝不能借此运行时读原EXE。
    return RequireSaveLoadConfirmDialogNativeServices().source.ReadImmutableDwordAtOriginalVa(address);
}
}
}

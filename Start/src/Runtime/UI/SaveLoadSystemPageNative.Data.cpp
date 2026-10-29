#include <Runtime/SaveLoadSystemPageNative.h>

namespace KaGuYa::Start::Runtime
{
namespace
{
// 常量全部按权威18号原PE节映射读取；CP932保留原字节和重复指针。
constexpr char name5B756C[] = "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x82\xD6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B7588[] = "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x82\xD6\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B75A0[] = "\x83\x8D\x81\x5B\x83\x68\x89\xE6\x96\xCA\x82\xD6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B75BC[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B75D4[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B75E8[] = "\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B7600[] = "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B7618[] = "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B762C[] = "\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B7644[] = "\x91\x53\x8F\xC1\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B7658[] = "\x91\x53\x8F\xC1\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B7668[] = "\x91\x53\x8F\xC1\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B7678[] = "\x91\x53\x8F\xC1\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B768C[] = "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x82\xD6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B76A8[] = "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x82\xD6\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B76F0[] = "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x82\xD6\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B7708[] = "\x83\x5A\x81\x5B\x83\x75\x89\xE6\x96\xCA\x82\xD6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B7724[] = "\x4C\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B773C[] = "\x4C\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B7750[] = "\x4C\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B7764[] = "\x4C\x83\x5E\x83\x43\x83\x67\x83\x8B\x82\xD6\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B777C[] = "\x4C\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B7794[] = "\x4C\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B77A8[] = "\x4C\x83\x4C\x83\x83\x83\x93\x83\x5A\x83\x8B\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B77C0[] = "\x4C\x91\x53\x8F\xC1\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B77D4[] = "\x4C\x91\x53\x8F\xC1\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B77E4[] = "\x4C\x91\x53\x8F\xC1\x83\x5E\x83\x51\x2E\x76\x72\x6D";
constexpr char name5B77F4[] = "\x4C\x91\x53\x8F\xC1\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B7808[] = "\x92\xCA\x8F\xED\x83\x5A\x81\x5B\x83\x75\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B7820[] = "\x92\xCA\x8F\xED\x83\x5A\x81\x5B\x83\x75\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B7834[] = "\x92\xCA\x8F\xED\x83\x5A\x81\x5B\x83\x75\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
constexpr char name5B7850[] = "\x92\xCA\x8F\xED\x83\x5A\x81\x5B\x83\x75\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B7868[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B7884[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B789C[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
constexpr char name5B78BC[] = "\x83\x4E\x83\x43\x83\x62\x83\x4E\x83\x5A\x81\x5B\x83\x75\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";
constexpr char name5B78D8[] = "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75\x83\x6D\x81\x5B\x83\x7D\x83\x8B\x2E\x76\x72\x6D";
constexpr char name5B78F4[] = "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75\x83\x49\x83\x74\x2E\x76\x72\x6D";
constexpr char name5B790C[] = "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67\x2E\x76\x72\x6D";
constexpr char name5B7928[] = "\x83\x49\x81\x5B\x83\x67\x83\x5A\x81\x5B\x83\x75\x83\x4E\x83\x8A\x83\x62\x83\x4E\x2E\x76\x72\x6D";

const GuiButtonDescriptorNative2Ch loadDescriptors[4]{
    {{9, {{name5B756C, nullptr, nullptr, name5B7588, name5B7588, name5B75A0, nullptr }}, nullptr}, 0, 19},
    {{10, {{name5B75BC, nullptr, nullptr, name5B75D4, name5B75D4, name5B75E8, nullptr }}, nullptr}, 0, 17},
    {{11, {{name5B7600, nullptr, nullptr, name5B7618, name5B7618, name5B762C, nullptr }}, nullptr}, 0, -1},
    {{12, {{name5B7644, name5B7658, nullptr, name5B7668, name5B7668, name5B7678, nullptr }}, nullptr}, 0, -1},
};
constexpr std::array<std::uint32_t, 11> loadRows[4]{
    {{0x9u, 0x5B756Cu, 0x0u, 0x0u, 0x5B7588u, 0x5B7588u, 0x5B75A0u, 0x0u, 0x0u, 0x0u, 0x13u}},
    {{0xAu, 0x5B75BCu, 0x0u, 0x0u, 0x5B75D4u, 0x5B75D4u, 0x5B75E8u, 0x0u, 0x0u, 0x0u, 0x11u}},
    {{0xBu, 0x5B7600u, 0x0u, 0x0u, 0x5B7618u, 0x5B7618u, 0x5B762Cu, 0x0u, 0x0u, 0x0u, 0xFFFFFFFFu}},
    {{0xCu, 0x5B7644u, 0x5B7658u, 0x0u, 0x5B7668u, 0x5B7668u, 0x5B7678u, 0x0u, 0x0u, 0x0u, 0xFFFFFFFFu}},
};
const SaveLoadSystemPageNativeGroup loadGroup{0x605FF8u, 0x603DB0u, 1, 4, loadDescriptors, loadRows};

const GuiButtonDescriptorNative2Ch saveDescriptors[7]{
    {{13, {{name5B768C, name5B76A8, nullptr, name5B76F0, name5B76F0, name5B7708, nullptr }}, nullptr}, 0, 18},
    {{10, {{name5B7724, name5B773C, nullptr, name5B7750, name5B7750, name5B7764, nullptr }}, nullptr}, 0, 17},
    {{11, {{name5B777C, nullptr, nullptr, name5B7794, name5B7794, name5B77A8, nullptr }}, nullptr}, 0, -1},
    {{12, {{name5B77C0, name5B77D4, nullptr, name5B77E4, name5B77E4, name5B77F4, nullptr }}, nullptr}, 0, -1},
    {{14, {{name5B7808, name5B7820, nullptr, name5B7834, name5B7834, name5B7850, nullptr }}, nullptr}, 0, -1},
    {{15, {{name5B7868, name5B7884, nullptr, name5B789C, name5B789C, name5B78BC, nullptr }}, nullptr}, 0, -1},
    {{16, {{name5B78D8, name5B78F4, nullptr, name5B790C, name5B790C, name5B7928, nullptr }}, nullptr}, 0, -1},
};
constexpr std::array<std::uint32_t, 11> saveRows[7]{
    {{0xDu, 0x5B768Cu, 0x5B76A8u, 0x0u, 0x5B76F0u, 0x5B76F0u, 0x5B7708u, 0x0u, 0x0u, 0x0u, 0x12u}},
    {{0xAu, 0x5B7724u, 0x5B773Cu, 0x0u, 0x5B7750u, 0x5B7750u, 0x5B7764u, 0x0u, 0x0u, 0x0u, 0x11u}},
    {{0xBu, 0x5B777Cu, 0x0u, 0x0u, 0x5B7794u, 0x5B7794u, 0x5B77A8u, 0x0u, 0x0u, 0x0u, 0xFFFFFFFFu}},
    {{0xCu, 0x5B77C0u, 0x5B77D4u, 0x0u, 0x5B77E4u, 0x5B77E4u, 0x5B77F4u, 0x0u, 0x0u, 0x0u, 0xFFFFFFFFu}},
    {{0xEu, 0x5B7808u, 0x5B7820u, 0x0u, 0x5B7834u, 0x5B7834u, 0x5B7850u, 0x0u, 0x0u, 0x0u, 0xFFFFFFFFu}},
    {{0xFu, 0x5B7868u, 0x5B7884u, 0x0u, 0x5B789Cu, 0x5B789Cu, 0x5B78BCu, 0x0u, 0x0u, 0x0u, 0xFFFFFFFFu}},
    {{0x10u, 0x5B78D8u, 0x5B78F4u, 0x0u, 0x5B790Cu, 0x5B790Cu, 0x5B7928u, 0x0u, 0x0u, 0x0u, 0xFFFFFFFFu}},
};
const SaveLoadSystemPageNativeGroup saveGroup{0x606004u, 0x603E60u, 1, 7, saveDescriptors, saveRows};

constexpr std::array<std::uint32_t, 35> originalSlots{{
    0x509ED0u, 0x4FE0A0u, 0x5025A0u, 0x50A5D0u, 0x4FF020u,
    0x509D20u, 0x536310u, 0x536500u, 0x50FF40u, 0x538660u,
    0x510630u, 0x500800u, 0x532170u, 0x51E740u, 0x510B90u,
    0x510800u, 0x511AA0u, 0x50EC20u, 0x52FDA0u, 0x5094C0u,
    0x50ABC0u, 0x518C30u, 0x525FB0u, 0x5249C0u, 0x524960u,
    0x50FC80u, 0x522E90u, 0x510030u, 0x5100E0u, 0x50A6A0u,
    0x50B920u, 0x510220u, 0x510170u, 0x5102B0u, 0x518A80u,
}};
constexpr std::array<std::int32_t, 6> shortcuts{{14, 15, 15, 16, 16, 14}};
}
const SaveLoadSystemPageNativeGroup& SaveLoadSystemPageNativeGroupForId(std::int32_t id) noexcept
{ return id == 0 ? loadGroup : saveGroup; }
std::span<const std::uint32_t, 35> SaveLoadSystemPageNativeOriginalSlots() noexcept
{ return originalSlots; }
std::span<const std::int32_t, 6> SaveLoadSystemPageNativeShortcuts() noexcept
{ return shortcuts; }
}


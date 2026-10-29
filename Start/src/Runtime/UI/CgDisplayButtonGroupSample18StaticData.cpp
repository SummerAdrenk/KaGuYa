#include <Runtime/CgDisplayButtonGroupSample18StaticData.h>

#include <array>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <utility>

namespace KaGuYa::Start::Runtime
{
namespace
{
using IdentifierRef = CgDisplayButtonGroupResourceIdentifierRefAt499DC0;

struct ResourcePointer final
{
    std::uintptr_t address;
    const char* cp932;
};

constexpr std::array<ResourcePointer, 27> kResourcePointers{{
    {0x602398u, "vrm:\\\x83\x56\x81\x5B\x83\x93\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x90\x53\x8D\xD8.bmp"},
    {0x60239Cu, nullptr},
    {0x6023A0u, "vrm:\\\x83\x56\x81\x5B\x83\x93\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x90\xE1\x88\xA4.bmp"},
    {0x6023A4u, nullptr},
    {0x6023A8u, "vrm:\\\x83\x56\x81\x5B\x83\x93\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67.bmp"},
    {0x6023ACu, nullptr},
    {0x6023B0u, "vrm:\\\x83\x56\x81\x5B\x83\x93\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x89\xC4\x91\xE3\x8E\x71.bmp"},
    {0x6023B4u, "vrm:\\\x83\x56\x81\x5B\x83\x93\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x89\xC4\x91\xE3\x8E\x71\x96\xA2\x8A\x4A\x95\xFA.bmp"},
    {0x6023B8u, "vrm:\\\x83\x56\x81\x5B\x83\x93\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x82\xBB\x82\xCC\x91\xBC.bmp"},
    {0x6023BCu, nullptr},
    {0x5B42C0u, "vrm:\\CG\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x90\x53\x8D\xD8.bmp"},
    {0x5B42C4u, nullptr},
    {0x5B42C8u, "vrm:\\CG\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x90\xE1\x88\xA4.bmp"},
    {0x5B42CCu, nullptr},
    {0x5B42D0u, "vrm:\\CG\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x83\x75\x83\x8A\x83\x85\x83\x47\x83\x62\x83\x67.bmp"},
    {0x5B42D4u, nullptr},
    {0x5B42D8u, "vrm:\\CG\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x89\xC4\x91\xE3\x8E\x71.bmp"},
    {0x5B42DCu, "vrm:\\CG\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x89\xC4\x91\xE3\x8E\x71\x96\xA2\x8A\x4A\x95\xFA.bmp"},
    {0x5B42E0u, "vrm:\\CG\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA_\x82\xBB\x82\xCC\x91\xBC.bmp"},
    {0x5B42E4u, nullptr},
    {0x602094u, "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA" "1\x83\x79\x81\x5B\x83\x57.alp"},
    {0x602098u, "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA" "2\x83\x79\x81\x5B\x83\x57.alp"},
    {0x60209Cu, "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA" "3\x83\x79\x81\x5B\x83\x57.alp"},
    {0x6021D0u, "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x83\x54\x83\x80\x83\x6C\x83\x4E\x83\x8A\x83\x62\x83\x4E.alp"},
    {0x6021D4u, "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x83\x54\x83\x80\x83\x6C\x83\x5E\x81\x5B\x83\x51\x83\x62\x83\x67.alp"},
    {0x6021D8u, "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x83\x54\x83\x80\x83\x6C\x83\x43\x83\x8B\x83\x49\x83\x74.alp"},
    {0x5B4038u, "vrm:\\\x8A\xD3\x8F\xDC\x89\xE6\x96\xCA\x8D\xB7\x95\xAA\x95\x5C\x8E\xA6\x90\x94\x83\x58\x83\x89\x83\x62\x83\x56\x83\x85.alp"},
}};

constexpr std::array<std::int32_t, 9> kItemX{
    112, 349, 586, 112, 349, 586, 112, 349, 586};
constexpr std::array<std::int32_t, 9> kItemY{
    157, 157, 157, 299, 299, 299, 441, 441, 441};
constexpr std::array<std::int32_t, 9> kValueY{
    273, 273, 273, 415, 415, 415, 557, 557, 557};
constexpr std::array<std::int32_t, 9> kSlashX{
    269, 506, 743, 269, 506, 743, 269, 506, 743};

constexpr std::array<std::array<std::int32_t, 4>, 27> kNavigation{{
    {6, 3, 2, 1}, {7, 4, 0, 2}, {8, 5, 1, 0},
    {0, 6, 5, 4}, {1, 7, 3, 5}, {2, 8, 4, 3},
    {3, 0, 8, 7}, {4, 1, 6, 8}, {5, 2, 7, 6},
    {15, 12, 11, 10}, {16, 13, 9, 11}, {17, 14, 10, 9},
    {9, 15, 14, 13}, {10, 16, 12, 14}, {11, 17, 13, 12},
    {12, 9, 17, 16}, {13, 10, 15, 17}, {14, 11, 16, 15},
    {24, 21, 20, 19}, {25, 22, 18, 20}, {26, 23, 19, 18},
    {18, 24, 23, 22}, {19, 25, 21, 23}, {20, 26, 22, 21},
    {21, 18, 26, 25}, {22, 19, 24, 26}, {23, 20, 25, 24},
}};

[[nodiscard]] IdentifierRef MakeIdentifier(const char* const cp932)
{
    return cp932 == nullptr
        ? IdentifierRef{}
        : std::make_shared<CgDisplayStringResourceIdentifierAt499DC0>(cp932);
}

[[nodiscard]] std::optional<std::int32_t> ReadStride8Table(
    const std::uintptr_t address,
    const std::uintptr_t base,
    const std::array<std::int32_t, 9>& values)
{
    if (address < base)
        return std::nullopt;
    const std::uintptr_t offset = address - base;
    if ((offset % 8u) != 0u || offset / 8u >= values.size())
        return std::nullopt;
    return values[static_cast<std::size_t>(offset / 8u)];
}
}

CgDisplayButtonGroupResourceIdentifierRefAt499DC0
CgDisplayButtonGroupSample18StaticDataAt499DC0::
    ReadResourcePointerAtAddress(std::uintptr_t pointerAddress) const
{
    // 原 PE 两组指针逐 DWORD 相同：Replay 复用同一页叠图和状态框资源。
    switch (pointerAddress)
    {
    case 0x6021DCu: pointerAddress = 0x602094u; break;
    case 0x6021E0u: pointerAddress = 0x602098u; break;
    case 0x6021E4u: pointerAddress = 0x60209Cu; break;
    case 0x602468u: pointerAddress = 0x6021D0u; break;
    case 0x60246Cu: pointerAddress = 0x6021D4u; break;
    case 0x602470u: pointerAddress = 0x6021D8u; break;
    default: break;
    }
    for (const ResourcePointer& entry : kResourcePointers)
    {
        if (entry.address == pointerAddress)
            return MakeIdentifier(entry.cp932);
    }
    throw std::out_of_range("sample 18 CCGdispBtn resource pointer address is unknown");
}

std::int32_t CgDisplayButtonGroupSample18StaticDataAt499DC0::
    ReadDwordAtAddress(const std::uintptr_t address) const
{
    switch (address)
    {
    case 0x6021C0u:
    case 0x6021C4u:
    case 0x602458u:
    case 0x60245Cu:
        return 5;
    case 0x6021C8u:
    case 0x6021CCu:
    case 0x602460u:
    case 0x602464u:
        return 6;
    default:
        break;
    }

    if (const auto value = ReadStride8Table(address, 0x5B42E8u, kItemX))
        return *value;
    if (const auto value = ReadStride8Table(address, 0x5B42ECu, kItemY))
        return *value;
    if (const auto value = ReadStride8Table(address, 0x602410u, kItemX))
        return *value;
    if (const auto value = ReadStride8Table(address, 0x602414u, kItemY))
        return *value;
    if (const auto value = ReadStride8Table(address, 0x5B4330u, kValueY))
        return *value;
    if (const auto value = ReadStride8Table(address, 0x5B4334u, kValueY))
        return *value;
    if (const auto value = ReadStride8Table(address, 0x5B41F0u, kSlashX))
        return *value;
    if (const auto value = ReadStride8Table(address, 0x5B41F4u, kValueY))
        return *value;

    throw std::out_of_range("sample 18 CCGdispBtn DWORD address is unknown");
}

CgDisplayButtonGroupResourceIdentifierRefAt499DC0
CgDisplayButtonGroupSample18StaticDataAt499DC0::
    FormatResourceIdentifierAt417130(
        const std::uintptr_t formatAddress,
        const std::int32_t categoryIndex,
        const std::int32_t itemIndex) const
{
    const char* format = nullptr;
    switch (formatAddress)
    {
    case 0x5B51CCu:
        format = "\x83\x5E\x83\x43\x83\x76%d_\x89\xE6\x91\x9C%d_\x83\x6D\x81\x5B\x83\x7D\x83\x8B";
        break;
    case 0x5B51E8u:
        format = "\x83\x5E\x83\x43\x83\x76%d_\x89\xE6\x91\x9C%d_\x83\x4E\x83\x8A\x83\x62\x83\x4E";
        break;
    case 0x5B5204u:
        format = "\x83\x5E\x83\x43\x83\x76%d_\x89\xE6\x91\x9C%d_\x83\x74\x83\x48\x81\x5B\x83\x4A\x83\x58";
        break;
    default:
        throw std::out_of_range("sample 18 CCGdispBtn format address is unknown");
    }

    std::array<char, 96> buffer{};
    const int length = std::snprintf(
        buffer.data(), buffer.size(), format, categoryIndex, itemIndex);
    if (length < 0 || static_cast<std::size_t>(length) >= buffer.size())
        throw std::length_error("sample 18 CCGdispBtn formatted identifier is too long");
    return MakeIdentifier(buffer.data());
}

std::vector<std::array<std::int32_t, 4>>
CgDisplayButtonGroupSample18StaticDataAt499DC0::
    ReadNavigationTableAt6021E8() const
{
    return {kNavigation.begin(), kNavigation.end()};
}

std::vector<std::array<std::int32_t, 4>>
CgDisplayButtonGroupSample18StaticDataAt499DC0::ReadNavigationTableAt602478() const
{
    // 样本 18 PE：602478 与 6021E8 的 432 字节完全相同。
    return {kNavigation.begin(), kNavigation.end()};
}
}

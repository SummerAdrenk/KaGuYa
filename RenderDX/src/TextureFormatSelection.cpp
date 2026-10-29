#include "TextureFormatSelection.h"
#include "DebugMute.h"
#include <Logging/Logger.h>
#include <array>
#include <algorithm>
#include <bit>
#include <cstdint>
#include <limits>

namespace KaGuYa::RenderDX
{
namespace
{
struct FormatInfo
{
    D3DFORMAT format;
    std::uint32_t type;
    std::uint32_t bits;
    std::array<std::uint32_t, 5> components;
    std::uint32_t fallbackEligible;
};
static_assert(sizeof(FormatInfo) == 36);

// 原PE 1003E218至[1005D05C]，完整57条记录；不按当前显卡支持情况裁表。
constexpr std::array<FormatInfo, 57> Formats{{
    {static_cast<D3DFORMAT>(20u), 0u, 24u, {0u, 0u, 8u, 8u, 8u}, 0u},
    {static_cast<D3DFORMAT>(21u), 0u, 32u, {0u, 8u, 8u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(22u), 0u, 32u, {0u, 0u, 8u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(23u), 0u, 16u, {0u, 0u, 5u, 6u, 5u}, 1u},
    {static_cast<D3DFORMAT>(24u), 0u, 16u, {0u, 0u, 5u, 5u, 5u}, 1u},
    {static_cast<D3DFORMAT>(40u), 1u, 16u, {0u, 8u, 8u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(41u), 1u, 8u, {0u, 8u, 8u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(25u), 0u, 16u, {0u, 1u, 5u, 5u, 5u}, 1u},
    {static_cast<D3DFORMAT>(26u), 0u, 16u, {0u, 4u, 4u, 4u, 4u}, 1u},
    {static_cast<D3DFORMAT>(27u), 0u, 8u, {0u, 0u, 3u, 3u, 2u}, 1u},
    {static_cast<D3DFORMAT>(28u), 0u, 8u, {0u, 8u, 0u, 0u, 0u}, 1u},
    {static_cast<D3DFORMAT>(29u), 0u, 16u, {0u, 8u, 3u, 3u, 2u}, 1u},
    {static_cast<D3DFORMAT>(30u), 0u, 16u, {0u, 0u, 4u, 4u, 4u}, 1u},
    {static_cast<D3DFORMAT>(31u), 0u, 32u, {0u, 2u, 10u, 10u, 10u}, 1u},
    {static_cast<D3DFORMAT>(32u), 0u, 32u, {0u, 8u, 8u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(33u), 0u, 32u, {0u, 0u, 8u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(34u), 0u, 32u, {0u, 0u, 16u, 16u, 0u}, 1u},
    {static_cast<D3DFORMAT>(35u), 0u, 32u, {0u, 2u, 10u, 10u, 10u}, 1u},
    {static_cast<D3DFORMAT>(36u), 0u, 64u, {0u, 16u, 16u, 16u, 16u}, 1u},
    {static_cast<D3DFORMAT>(50u), 2u, 8u, {0u, 0u, 8u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(51u), 2u, 16u, {0u, 8u, 8u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(52u), 2u, 8u, {0u, 4u, 4u, 4u, 4u}, 1u},
    {static_cast<D3DFORMAT>(81u), 2u, 16u, {0u, 0u, 16u, 16u, 16u}, 1u},
    {static_cast<D3DFORMAT>(60u), 3u, 16u, {0u, 0u, 0u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(61u), 3u, 16u, {6u, 0u, 0u, 5u, 5u}, 1u},
    {static_cast<D3DFORMAT>(62u), 3u, 32u, {8u, 0u, 0u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(63u), 3u, 32u, {0u, 8u, 8u, 8u, 8u}, 1u},
    {static_cast<D3DFORMAT>(64u), 3u, 32u, {0u, 0u, 0u, 16u, 16u}, 1u},
    {static_cast<D3DFORMAT>(67u), 3u, 32u, {2u, 0u, 10u, 10u, 10u}, 1u},
    {static_cast<D3DFORMAT>(110u), 3u, 64u, {0u, 16u, 16u, 16u, 16u}, 1u},
    {static_cast<D3DFORMAT>(117u), 3u, 16u, {8u, 0u, 0u, 8u, 8u}, 0u},
    {static_cast<D3DFORMAT>(1498831189u), 0u, 16u, {0u, 0u, 8u, 8u, 8u}, 0u},
    {static_cast<D3DFORMAT>(1195525970u), 0u, 16u, {0u, 0u, 8u, 8u, 8u}, 0u},
    {static_cast<D3DFORMAT>(844715353u), 0u, 16u, {0u, 0u, 8u, 8u, 8u}, 0u},
    {static_cast<D3DFORMAT>(1111970375u), 0u, 16u, {0u, 0u, 8u, 8u, 8u}, 0u},
    {static_cast<D3DFORMAT>(827611204u), 0u, 16u, {0u, 1u, 5u, 6u, 5u}, 0u},
    {static_cast<D3DFORMAT>(844388420u), 0u, 16u, {0u, 4u, 5u, 6u, 5u}, 0u},
    {static_cast<D3DFORMAT>(861165636u), 0u, 16u, {0u, 4u, 5u, 6u, 5u}, 0u},
    {static_cast<D3DFORMAT>(877942852u), 0u, 16u, {0u, 8u, 5u, 6u, 5u}, 0u},
    {static_cast<D3DFORMAT>(894720068u), 0u, 16u, {0u, 8u, 5u, 6u, 5u}, 0u},
    {static_cast<D3DFORMAT>(70u), 4u, 16u, {1u, 0u, 0u, 0u, 16u}, 1u},
    {static_cast<D3DFORMAT>(82u), 4u, 32u, {1u, 0u, 0u, 0u, 32u}, 0u},
    {static_cast<D3DFORMAT>(71u), 4u, 32u, {0u, 0u, 0u, 0u, 32u}, 1u},
    {static_cast<D3DFORMAT>(73u), 4u, 16u, {0u, 0u, 0u, 1u, 15u}, 1u},
    {static_cast<D3DFORMAT>(75u), 4u, 32u, {0u, 0u, 0u, 8u, 24u}, 1u},
    {static_cast<D3DFORMAT>(80u), 4u, 16u, {0u, 0u, 0u, 0u, 16u}, 1u},
    {static_cast<D3DFORMAT>(77u), 4u, 32u, {0u, 0u, 0u, 0u, 24u}, 1u},
    {static_cast<D3DFORMAT>(79u), 4u, 32u, {0u, 0u, 0u, 4u, 24u}, 1u},
    {static_cast<D3DFORMAT>(83u), 4u, 32u, {0u, 0u, 0u, 8u, 24u}, 0u},
    {static_cast<D3DFORMAT>(111u), 0u, 16u, {0u, 0u, 16u, 0u, 0u}, 0u},
    {static_cast<D3DFORMAT>(112u), 0u, 32u, {0u, 0u, 16u, 16u, 0u}, 0u},
    {static_cast<D3DFORMAT>(113u), 0u, 64u, {0u, 16u, 16u, 16u, 16u}, 0u},
    {static_cast<D3DFORMAT>(114u), 0u, 32u, {0u, 0u, 32u, 0u, 0u}, 0u},
    {static_cast<D3DFORMAT>(115u), 0u, 64u, {0u, 0u, 32u, 32u, 0u}, 0u},
    {static_cast<D3DFORMAT>(116u), 0u, 128u, {0u, 32u, 32u, 32u, 32u}, 0u},
    {static_cast<D3DFORMAT>(909200449u), 2u, 32u, {0u, 16u, 16u, 16u, 16u}, 0u},
    {static_cast<D3DFORMAT>(909201952u), 0u, 48u, {0u, 0u, 16u, 16u, 16u}, 0u},
}};
constexpr std::array<std::uint32_t, 25> CompatibleTypes{{
    1u, 0u, 0u, 0u, 0u,
    1u, 1u, 0u, 0u, 0u,
    1u, 0u, 1u, 0u, 0u,
    0u, 0u, 0u, 1u, 0u,
    0u, 0u, 0u, 0u, 1u,
}};

std::uint32_t Score(const FormatInfo& source, const FormatInfo& destination) noexcept
{
    constexpr auto Rejected = (std::numeric_limits<std::uint32_t>::max)();
    if (!CompatibleTypes[source.type * 5 + destination.type]) return Rejected;
    std::uint32_t score = 0, channels = 0;
    for (std::size_t index = 0; index < source.components.size(); ++index)
    {
        const auto from = source.components[index], to = destination.components[index];
        if (from) ++channels;
        if (from > to) score += to ? (from - to) << 16u : 0x01000000u;
        else if (from < to) score += from ? to - from : 0x100u;
    }
    // 原04723：源的全部有效分量均丢失时拒绝，不能只按最小数值选择。
    return channels == (score >> 24u) ? Rejected : score;
}
}

D3DFORMAT SelectPrimaryTextureFormat(IDirect3DDevice9* device) noexcept
{
    static_cast<void>(SetDebugMute(1)); // 原04749，先于来源查询。
    IDirect3D9* direct3D = nullptr;
    D3DCAPS9 caps{};
    D3DDISPLAYMODE display{};
    // 原475C/4769/4775按此顺序取当前设备来源；普通返回不替代已写输出。
    const HRESULT rootResult = device->GetDirect3D(&direct3D);
    const HRESULT capsResult = device->GetDeviceCaps(&caps);
    const HRESULT displayResult = device->GetDisplayMode(0, &display);
    if (FAILED(rootResult) || FAILED(capsResult) || FAILED(displayResult))
        Logging::Write(Logging::Level::Error, "Texture format source query failed; consuming written outputs, unwritten fields safely zero-initialized.");

    constexpr auto Rejected = (std::numeric_limits<std::uint32_t>::max)();
    constexpr auto& source = Formats[2];
    static_assert(source.format == D3DFMT_X8R8G8B8);
    const FormatInfo* best = nullptr;
    std::uint32_t bestScore = Rejected;
    for (const auto& candidate : Formats)
    {
        // 原47B2：根为空时跳过能力查询，而不是自行认定没有格式。
        if (!candidate.format || (direct3D && FAILED(direct3D->CheckDeviceFormat(
            caps.AdapterOrdinal, caps.DeviceType, display.Format, 0, D3DRTYPE_TEXTURE, candidate.format))))
            continue;
        if (candidate.format == source.format)
        {
            best = &candidate;
            break;
        }
        if (!candidate.fallbackEligible) continue;
        const auto score = Score(source, candidate);
        if (score != Rejected && (score < bestScore ||
            (score == bestScore && best && candidate.bits < best->bits)))
        {
            bestScore = score;
            best = &candidate;
        }
    }
    if (direct3D) direct3D->Release();
    static_cast<void>(SetDebugMute(0)); // 原04825，退Direct3D之后。
    return best ? best->format : D3DFMT_UNKNOWN;
}

HRESULT PreparePrimaryTexture(IDirect3DDevice9* device, UINT& width, UINT& height,
    UINT& levels, D3DFORMAT& format) noexcept
{
    if (!device) return D3DERR_INVALIDCALL;
    const auto selected = SelectPrimaryTextureFormat(device);
    if (selected == D3DFMT_UNKNOWN) return D3DERR_NOTAVAILABLE;

    UINT w = width, h = height;
    constexpr UINT Default = (std::numeric_limits<UINT>::max)();
    if (w == Default) w = h == Default ? 256u : h;
    if (h == Default) h = w;
    if (!w) w = 1;
    if (!h) h = 1;
    D3DCAPS9 caps{};
    const auto result = device->GetDeviceCaps(&caps);
    if (FAILED(result))
        Logging::Write(Logging::Level::Error, "Texture dimension caps query failed; consuming written output with zero-initialized unwritten fields.");
    w = (std::min)(w, static_cast<UINT>(caps.MaxTextureWidth));
    h = (std::min)(h, static_cast<UINT>(caps.MaxTextureHeight));
    const UINT aspect = caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY ? 1u : caps.MaxTextureAspectRatio;
    if (aspect)
    {
        // 原4AEB/4AF7为DWORD低位乘法，不用64位乘法改变溢出后的比较。
        w = (std::min)(w, h * aspect);
        h = (std::min)(h, w * aspect);
    }
    if (caps.TextureCaps & D3DPTEXTURECAPS_POW2)
    {
        // 本链请求levels=0，原NONPOW2CONDITIONAL不会在4B29关闭POW2。
        if (w > 0x80000000u || h > 0x80000000u)
        {
            Logging::Write(Logging::Level::Error, "Texture power-of-two overflow rejected instead of reproducing the original wrapping loop.");
            return D3DERR_INVALIDCALL;
        }
        w = std::bit_ceil(w);
        h = std::bit_ceil(h);
    }
    UINT mipLevels;
    if (!(caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) ||
        ((caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) &&
            (((w - 1u) & w) || ((h - 1u) & h))))
        mipLevels = 1;
    else
        mipLevels = static_cast<UINT>(std::bit_width((std::max)(w, h)));
    // 原4C91之后才写各出参；格式选择失败不提前发布部分新规格。
    width = w;
    height = h;
    levels = mipLevels;
    format = selected;
    return D3D_OK;
}
}

